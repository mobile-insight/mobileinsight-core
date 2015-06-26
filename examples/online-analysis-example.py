#!/usr/bin/python
# Filename: online-analysis-example.py
import os
import sys

#Import MobileInsight modules
from mobile_insight.analyzer import *
from mobile_insight.monitor import *

#Wireshark library path
LIBWIRESHARK_PATH = "/usr/local/lib"

if __name__ == "__main__":
    
    if len(sys.argv) < 3:
        opt.error("please specify physical port name and baudrate.")
        sys.exit(1)

    # Initialize a 3G/4G monitor
    src = DMCollector(prefs={
                        "ws_dissect_executable_path": "./ws_dissector/ws_dissector",
                        "libwireshark_path": LIBWIRESHARK_PATH})
    src.set_serial_port(sys.argv[1]) #the serial port to collect the traces
    src.set_baudrate(int(sys.argv[2])) #the baudrate of the port

    #Enable 3G/4G RRC (radio resource control) monitoring
    src.enable_log("LTE_RRC_OTA_Packet")
    src.enable_log("WCDMA_Signaling_Messages")

    #4G RRC analyzer
    lte_rrc_analyzer = LteRrcAnalyzer()
    lte_rrc_analyzer.set_source(src) #bind with the monitor

    #3G RRC analyzer
    wcdma_rrc_analyzer = WcdmaRrcAnalyzer()
    wcdma_rrc_analyzer.set_source(src) #bind with the monitor

    #save the analysis result. All analyzers share the same output file.
    wcdma_rrc_analyzer.set_log("3g-4g-rrc.txt") 

    #Start the monitoring
    src.run()