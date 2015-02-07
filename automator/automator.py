"""
automator.py

A tool that disables logs from a phone and then tells the phone to start transmitting RRC-OTA messages.

Author: Jiayao Li, Samson Richard Wong
"""

from sender import sendRecv, recvMessage
from hdlc_parser import hdlc_parser
import optparse
import sys
import serial
import os

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
    opt = optparse.OptionParser(prog="automator",
                                usage="usage: %prog [options] -p PHY_PORT_NAME ...",
                                description="Automatic mobile trace collector.")
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

def init_cmd_dict(cmd_dict_path):
    cmd_dict = {}
    with open(cmd_dict_path, 'r') as cmd_dict_file:
        for line in cmd_dict_file:
            line = line.strip()
            if len(line) > 0 and not line.startswith('#'):
                cmd, colon, binary = line.partition(':')
                cmd = cmd.strip()
                binary = binary.strip()
                cmd_dict[cmd] = binary
    return cmd_dict

def init_target_cmds(cmd_file_path):
    target_cmds = []
    with open(cmd_file_path, 'r') as cmd_file:
        for line in cmd_file:
            line = line.strip()
            line = line.replace('\n','')
            line = line.upper()
            if len(line) > 0 and not line.startswith('#'):
                target_cmds.append(line)
    return target_cmds
                
if __name__ == "__main__":
    opt = init_opt()
    options, args = opt.parse_args(sys.argv[1:])

    if not options.phy_serial_name:
        opt.error("please use -p option to specify physical port name.")
        sys.exit(1)
    
    phy_ser_name = options.phy_serial_name
    print "PHY COM: %s" % phy_ser_name

    phy_baudrate = options.phy_baudrate
    print "PHY BAUD RATE: %d" % phy_baudrate

    # Read command dict
    cmd_dict_path = os.path.join(PROGRAM_DIR_PATH, COMMAND_FILES_PATH + '/cmd_dict.txt')
    cmd_dict = init_cmd_dict(cmd_dict_path)

    # Read target commands
    if options.cmd_file_name is not None:
        cmd_file_path = options.cmd_file_name
    else:
        cmd_file_path = os.path.join(PROGRAM_DIR_PATH, COMMAND_FILES_PATH + '/example_cmds.txt')
    target_cmds = init_target_cmds(cmd_file_path)

    log = None
    if options.log_name is not None:
        log = open(options.log_name, "w")

    try:
        # Open COM ports. A zero timeout means that IO functions never suspend.
        phy_ser = serial.Serial(phy_ser_name, baudrate=phy_baudrate, timeout=.5)
        parser = hdlc_parser()

        # Disable logs
        print sendRecv(parser, phy_ser, cmd_dict.get("DISABLE")) + "\n"
        
        for cmd in target_cmds:
            binary = cmd_dict.get(cmd)
            if binary is None:      # To Samson: what does it exactly mean?
                binary = cmd
            print sendRecv(parser, phy_ser, binary) + "\n"

        while True:
            # cmd = 0x10 for log packets
            rec = recvMessage(parser, phy_ser, "10") 
            if rec != "":
                print rec + "\n"
                if log is not None:
                    log.write(rec + "\n\n")
        
    except KeyboardInterrupt, e:
        print "\n\nKeyboard Interrupt Detected: Disabling all logs"
        # Disable logs
        print sendRecv(parser, phy_ser, cmd_dict.get("DISABLE")) + "\n"
        sys.exit(e)
    except IOError, e:
        sys.exit(e)
    except Exception,e:
        sys.exit(e)