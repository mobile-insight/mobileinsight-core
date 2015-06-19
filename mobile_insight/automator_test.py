#!/usr/bin/env python
"""
automator.py

A tool that disables logs from a phone and then tells the phone to start transmitting RRC-OTA messages.

Author: Jiayao Li, Samson Richard Wong
"""

import binascii
import os
import optparse
import serial
import sys

from sender import sendRecv, recvMessage
from hdlc_parser import hdlc_parser
from dm_log_packet import DMLogPacket, FormatError
from dm_log_packet import consts as dm_log_consts

import xml.etree.ElementTree as ET

from analyzer import msg_filter_manager
from analyzer.logger import mem_logger
from analyzer.proto_analyzer import lte_rrc_analyzer

inExe = hasattr(sys, "frozen") # true if the code is being run in an exe
if (inExe):
    PROGRAM_PATH   = sys.executable
    COMMAND_FILES_PATH = "./command_files"
else:
    PROGRAM_PATH   = sys.argv[0]
    COMMAND_FILES_PATH = "../command_files"
PROGRAM_DIR_PATH = os.path.dirname(os.path.abspath(PROGRAM_PATH))
WS_DISSECT_EXECUTABLE_PATH = os.path.join(PROGRAM_DIR_PATH, "../ws_dissect/dissect")
LIBWIRESHARK_PATH = "/usr/local/lib"

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
    opt.add_option("-l", "--log-output",
                    metavar="STR",
                    action="store", type="string", dest="log_output", 
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
    try:
        with open(cmd_file_path, 'r') as cmd_file:
            for line in cmd_file:
                line = line.strip()
                line = line.replace('\n','')
                line = line.upper()
                if len(line) > 0 and not line.startswith('#'):
                    target_cmds.append(line)
    except IOError, e:
        raise RuntimeError("Cannot find command file %s. You can create it using example_cmds.txt as template." % cmd_file_path)
    return target_cmds

def print_reply(payload, crc_correct):
    if payload:
        print "reply: " + binascii.b2a_hex(payload)
        print "crc_correct: " + str(crc_correct)


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

    cmd_dict_path = os.path.join(PROGRAM_DIR_PATH, COMMAND_FILES_PATH + '/cmd_dict.txt')
    cmd_dict = init_cmd_dict(cmd_dict_path)

    if options.cmd_file_name is not None:
        cmd_file_path = options.cmd_file_name
    else:
        cmd_file_path = os.path.join(PROGRAM_DIR_PATH, COMMAND_FILES_PATH + '/cmds.txt')
    target_cmds = init_target_cmds(cmd_file_path)

    log = None
    if options.log_output is not None:
        log = open(options.log_output, "w")

    #Create a message analyzer
    logger=mem_logger.MemLogger()
    rrc_analyzer=lte_rrc_analyzer.LTE_RRC_Analyzer()
    #Create a centralized message analyzer manager
    msg_analyzer=msg_filter_manager.MsgFilterManager()
    #msg_analyzer.register(logger)
    msg_analyzer.register(rrc_analyzer)

    try:
        # Initialize Wireshark dissector
        DMLogPacket.init({
                     "ws_dissect_executable_path": WS_DISSECT_EXECUTABLE_PATH,
                     "libwireshark_path": LIBWIRESHARK_PATH,
                     })
        # Open COM ports. A zero timeout means that IO functions never suspend.
        phy_ser = serial.Serial(phy_ser_name, baudrate=phy_baudrate, timeout=.5)
        parser = hdlc_parser()

        # Disable logs
        payload, crc_correct = sendRecv(parser, phy_ser, cmd_dict.get("DISABLE"))
        print_reply(payload, crc_correct)
        
        for cmd in target_cmds:
            print "Enable: " + cmd
            binary = cmd_dict.get(cmd)
            if binary is None:      # To Samson: what does it exactly mean?
                binary = cmd
            payload, crc_correct = sendRecv(parser, phy_ser, binary)
            #print_reply(payload, crc_correct)

        while True:
            # cmd = 0x10 for log packets
            payload, crc_correct = recvMessage(parser, phy_ser, "10") 
            if payload:
                #print_reply(payload, crc_correct)
                l, type_id, ts, log_item = DMLogPacket.decode(payload[2:])
                #print l, hex(type_id), dm_log_consts.LOG_PACKET_NAME[type_id], ts
                #TODO: in the message manager, convert the log to the XML. Avoid redundant XML decoding
                
                #FIXME: change the onMessage API. Talk to Jiayao
                msg_analyzer.onMessage([ts,dm_log_consts.LOG_PACKET_NAME[type_id],log_item])
                # log_item_dict=dict(log_item)
                # if log_item_dict.has_key('Msg'):
                #     # print log_item_dict['Msg']
                #     root = ET.fromstring(log_item_dict['Msg'])
                #     for field in root.iter('field'):
                #         if field.get('name')=="lte-rrc.CarrierFreqUTRA_FDD_element":
                #             for freq in field.iter('field'):     
                #                 print freq.get('showname')
                #             print "\n\n"
        
    except (KeyboardInterrupt, RuntimeError), e:
        print "\n\n%s Detected: Disabling all logs" % type(e).__name__
        # Disable logs
        payload, crc_correct = sendRecv(parser, phy_ser, cmd_dict.get("DISABLE"))
        print_reply(payload, crc_correct)
        sys.exit(e)
    except IOError, e:
        sys.exit(e)
    except Exception,e:
        sys.exit(e)
