#!/usr/bin/python
# Filename: online-analysis-qmdl.py
import os
import sys

"""
Offline analysis with Qualcomm's QMDL traces
"""

#Import MobileInsight modules
from mobile_insight.monitor import QmdlReplayer
from mobile_insight.analyzer import RrcAnalyzer

if __name__ == "__main__":
    
    # Initialize a 3G/4G monitor
    src = QmdlReplayer()
    src.set_input_path("qmdl_example.qmdl")

    #Enable 3G/4G RRC (radio resource control) monitoring
    src.enable_log("LTE_RRC_OTA_Packet")
    src.enable_log("WCDMA_Signaling_Messages")

    #RRC analyzer
    rrc_analyzer = RrcAnalyzer()
    rrc_analyzer.set_source(src) #bind with the monitor
    rrc_analyzer.set_log("3g-4g-rrc.txt") #save the analysis result

    #Start the monitoring
    src.run()
