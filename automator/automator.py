"""
automator.py

A tool that disables logs from a phone and then tells the phone to start transmitting RRC-OTA messages.

Author: Jiayao Li, Samson Richard Wong
"""

from sender import *
from hdlc_parser import hdlc_parser
import optparse
import sys
import serial
import os

cmd_dict = {}
inExe = hasattr(sys, "frozen") # true if the code is being run in an exe
if (inExe):
    PROGRAM_PATH   = sys.executable
    COMMAND_FILES_PATH = "./command_files"
else:
    PROGRAM_PATH   = sys.argv[0]
    COMMAND_FILES_PATH = "../command_files"
PROGRAM_DIR_PATH = os.path.dirname(os.path.abspath(PROGRAM_PATH))

def init_opt():
    """
    Initialize and return the option parser.
    """
    opt = optparse.OptionParser(prog="com-sender",
                                usage="usage: %prog [options] LOGFILE ...",
                                description="COM serial port sender.")
    opt.add_option("-p", "--phy-serial-name",
                    metavar="STR",
                    action="store", type="string", dest="phy_serial_name", 
                    help="Manually set the name of physical serial name.")
    opt.add_option("-l", "--log-name",
                    metavar="STR",
                    action="store", type="string", dest="log_name", 
                    help="Specify a log file to save packets in")
    opt.add_option("-c", "--commands-file",
                    metavar="STR",
                    action="store", type="string", dest="cmd_file_name", 
                    help="Specify the file which contains the commands to send to the phone.")
    opt.add_option("--phy-baudrate",
                    metavar="N",
                    action="store", type="int", dest="phy_baudrate", 
                    help="Set the physical baud rate [default: %default].")
    opt.set_defaults(phy_baudrate=9600)
    return opt

def init_cmd_dict():
    cmd_dict_path = os.path.join(PROGRAM_DIR_PATH, COMMAND_FILES_PATH + '/cmd_dict.txt')
    cmd_dict_file = open(cmd_dict_path, 'r')
    for line in cmd_dict_file:
        line = line.strip()
        if len(line) > 0 and not line.startswith('#'):
            cmd, colon, binary = line.partition(':')
            cmd = cmd.strip()
            binary = binary.strip()
            cmd_dict[cmd] = binary

if __name__ == "__main__":
    init_cmd_dict()
    opt = init_opt()
    options, args = opt.parse_args(sys.argv[1:])

    if not options.phy_serial_name:
        sys.stderr.write("Please use -p option to specify physical port name.\n")
        sys.exit(1)
    
    phy_ser_name = options.phy_serial_name
    print "PHY COM: %s" % phy_ser_name

    log = None
    if options.log_name is not None:
        log = open(options.log_name, "w")

    cmd_file = None
    if options.cmd_file_name is not None:
        cmd_file = open(options.cmd_file_name, "r")
    else:
        cmd_file_path = os.path.join(PROGRAM_DIR_PATH, COMMAND_FILES_PATH + '/example_cmds.txt')
        cmd_file = open(cmd_file_path, 'r')

    phy_baudrate = options.phy_baudrate
    print "PHY BAUD RATE: %d" % phy_baudrate

    try:
        # Open COM ports. A zero timeout means that IO functions never suspend.
        phy_ser = serial.Serial(phy_ser_name, baudrate=phy_baudrate, timeout=.5)
        parser = hdlc_parser()

        # disable logs
        print sendRecv(parser, phy_ser, cmd_dict.get('DISABLE')) + "\n"
        
        for line in cmd_file:
            line = line.strip()
            line = line.replace('\n','')
            line = line.upper()
            if len(line) > 0 and not line.startswith('#'):
                binary = cmd_dict.get(line)
                if binary is None:
                    binary = line
                print sendRecv(parser, phy_ser, binary) + "\n"

        while True:
            #cmd = 10 for log packets
            rec = recvMessage(parser, phy_ser, "10") 
            if rec != "":
                print rec + "\n"
                if log is not None:
                    log.write(rec + "\n\n")
        
    except KeyboardInterrupt, e:
        print "\n\nKeyboard Interrupt Detected: Disabling all logs"
        # disable logs
        print sendRecv(parser, phy_ser, cmd_dict.get('DISABLE')) + "\n"
        sys.exit(e)
    except IOError, e:
        sys.exit(e)
    except Exception,e:
        sys.exit(e)