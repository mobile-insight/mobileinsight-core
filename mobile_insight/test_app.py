#!/usr/bin/env python
"""
test_app.py

for debug purpose

Author: Yuanjie Li
"""

import binascii
import os
import optparse
#import serial
import sys
#import logging

from monitor import *
from analyzer import *

# Define some constants
IN_EXE = hasattr(sys, "frozen") # true if the code is being run in an exe
if IN_EXE:
    PROGRAM_PATH = sys.executable
else:
    PROGRAM_PATH = sys.argv[0]
PROGRAM_DIR_PATH = os.path.dirname(os.path.abspath(PROGRAM_PATH))
WS_DISSECT_EXECUTABLE_PATH = os.path.join(PROGRAM_DIR_PATH, "../ws_dissector/ws_dissector")
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
    # TODO: temporarily removed
    # opt.add_option("-l", "--log-output",
    #                 metavar="STR",
    #                 action="store", type="string", dest="log_output", 
    #                 help="Specify a log file to save packets in")
    # TODO: temporarily removed or permanantly???
    # opt.add_option("-c", "--commands-file",
    #                 metavar="STR",
    #                 action="store", type="string", dest="cmd_file_name", 
    #                 help="Specify the file which contains the commands to send to the phone.")
    opt.add_option("--phy-baudrate",
                    metavar="N",
                    action="store", type="int", dest="phy_baudrate", 
                    help="Set the physical baud rate [default: %default].")
    opt.set_defaults(phy_baudrate=9600)
    return opt


if __name__ == "__main__":
    opt = init_opt()
    options, args = opt.parse_args(sys.argv[1:])

    if not options.phy_serial_name:
        opt.error("please use -p option to specify physical port name.")
        sys.exit(1)

    # Initialize trace collector
    src = DMCollector(prefs={
                        "ws_dissect_executable_path": WS_DISSECT_EXECUTABLE_PATH,
                        "libwireshark_path": LIBWIRESHARK_PATH,
                        })
    src.set_serial_port(options.phy_serial_name)
    src.set_baudrate(options.phy_baudrate)

    # src = Replayer()
    # src.set_input_path("/Users/yuanjieli/Desktop/test.replay")
    ###############################################

    # lte_rrc_analyzer = LteRrcAnalyzer()
    # lte_rrc_analyzer.set_source(src)

    # wcdma_rrc_analyzer = WcdmaRrcAnalyzer()
    # wcdma_rrc_analyzer.set_source(src)
    # rrc_analyzer = RrcAnalyzer()
    # rrc_analyzer.set_source(src)

    # lte_nas_analyzer = LteNasAnalyzer()
    # lte_nas_analyzer.set_source(src)

    # dumper = MsgFile("/Users/yuanjieli/Desktop/att-lte-active.txt")
    # dumper.set_source(src)

    dumper2 = MsgLogger()
    dumper2.set_source(src)

    # ue = LteUeAnalyzer()
    # ue.set_source(src)

    # serializer = MsgSerializer()
    # serializer.set_source(src)
    # serializer.set_output_path("/Users/yuanjieli/Desktop/test.replay")

    loop_detect = HandoffLoopAnalyzer()
    loop_detect.set_source(src)
    #loop_detect.set_log("/Users/yuanjieli/Desktop/verizon-mentone-lollicup.txt",logging.INFO)

    src.run()