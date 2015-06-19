#!/usr/bin/env python
"""
lte_trace_collect.py

Offline trace collection (for Scott and Chi-yu)

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
    opt = optparse.OptionParser(prog="automator",
                                usage="usage: %prog [options] -p PHY_PORT_NAME ...",
                                description="Automatic mobile trace collector.")
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

    #Enable messages to be collected
    src.enable_log("LTE_NAS_ESM_Plain_OTA_Incoming_Message")
    src.enable_log("LTE_NAS_ESM_Plain_OTA_Outgoing_Message")
    src.enable_log("LTE_NAS_EMM_Plain_OTA_Incoming_Message")
    src.enable_log("LTE_NAS_EMM_Plain_OTA_Outgoing_Message")
    src.enable_log("LTE_RRC_OTA_Packet")
    src.enable_log("LTE_RRC_MIB_Message_Log_Packet")
    src.enable_log("LTE_RRC_Serv_Cell_Info_Log_Packet")
    src.enable_log("LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results")
    src.enable_log("LTE_ML1_IRAT_Measurement_Request")
    src.enable_log("LTE_ML1_Serving_Cell_Measurement_Result")
    src.enable_log("LTE_ML1_Connected_Mode_Neighbor_Meas_Req/Resp")

    pickle_dumper = PickleDump()
    pickle_dumper.set_source(src)
    pickle_dumper.set_output_path("lte-sample.replay")

    src.run()