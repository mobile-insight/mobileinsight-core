#!/usr/bin/python
# Filename: lte-measurement-example
import os
import sys

#Import MobileInsight modules
from mobile_insight.analyzer import *
from mobile_insight.monitor import *

#Wireshark library path
LIBWIRESHARK_PATH = "/usr/local/lib"

"""
This example shows how to get LTE radio measurements with LteMeasurementAnalyzer
"""

if __name__ == "__main__":
    
    if len(sys.argv) < 3:
        opt.error("please specify physical port name and baudrate.")
        sys.exit(1)

    # Initialize a 3G/4G monitor
    src = DMCollector(prefs={"libwireshark_path": LIBWIRESHARK_PATH})
    src.set_serial_port(sys.argv[1]) #the serial port to collect the traces
    src.set_baudrate(int(sys.argv[2])) #the baudrate of the port

    src.enable_log("LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results")

    ML1_analyzer = LteMeasurementAnalyzer()
    ML1_analyzer.set_source(src)

    #save the analysis result. All analyzers share the same output file.
    ML1_analyzer.set_log("lte-measurement-example.txt") 

    #Start the monitoring
    src.run()