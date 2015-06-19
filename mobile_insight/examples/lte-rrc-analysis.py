#!/usr/bin/env python
"""
lte-nas-analysis.py

An example appplication for LTE RRC analyzer

Author: Yuanjie Li
"""

import binascii
import os
import optparse
import serial
import sys
import logging

from trace_collector import *
from analyzer import *


# Define some constants
# TODO: eliminate these with automatic path configuration
IN_EXE = hasattr(sys, "frozen") # true if the code is being run in an exe
if IN_EXE:
    PROGRAM_PATH   = sys.executable
    COMMAND_FILES_PATH = "./command_files"
else:
    PROGRAM_PATH   = sys.argv[0]
    COMMAND_FILES_PATH = "../command_files"
PROGRAM_DIR_PATH = os.path.dirname(os.path.abspath(PROGRAM_PATH))
WS_DISSECT_EXECUTABLE_PATH = os.path.join(PROGRAM_DIR_PATH, "../ws_dissector/ws_dissector")
LIBWIRESHARK_PATH = "/usr/local/lib"


def init_opt():
    """
    Initialize and return the option parser.
    """
    opt = optparse.OptionParser(prog="lte-nas-analysis",
                                usage="usage: %prog [options] -p PHY_PORT_NAME ...",
                                description="Example for LTE NAS analysis")
    opt.add_option("-p", "--phy-serial-name",
                    metavar="STR",
                    action="store", type="string", dest="phy_serial_name", 
                    help="Manually set the name of physical serial name.")
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
                        "command_files_path": os.path.join(PROGRAM_DIR_PATH, COMMAND_FILES_PATH),
                        "ws_dissect_executable_path": WS_DISSECT_EXECUTABLE_PATH,
                        "libwireshark_path": LIBWIRESHARK_PATH,
                        })
    src.set_serial_port(options.phy_serial_name)
    src.set_baudrate(options.phy_baudrate)


    # Initialize the analyzer
    lte_rrc_analyzer = LteRrcAnalyzer()
    lte_rrc_analyzer.set_log("lte-rrc-analysis.log",logging.INFO)

    # Link the analyzer with the trace collector
    lte_rrc_analyzer.set_source(src)
    

    # Start the analysis
    src.run()