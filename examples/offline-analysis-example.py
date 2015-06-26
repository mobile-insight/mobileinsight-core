#!/usr/bin/python
# Filename: online-analysis-example.py
import os
import sys

#Import MobileInsight modules
from mobile_insight.monitor import Replayer
from mobile_insight.analyzer import LteRrcAnalyzer

if __name__ == "__main__":
    
    # Initialize a 3G/4G monitor
    src = Replayer()
    src.set_input_path("3g-4g-rrc.replay")

    #Enable 3G/4G RRC (radio resource control) monitoring
    src.enable_log("LTE_RRC_OTA_Packet")
    src.enable_log("WCDMA_Signaling_Messages")

    #RRC analyzer
    rrc_analyzer = LteRrcAnalyzer()
    rrc_analyzer.set_source(src) #bind with the monitor
    rrc_analyzer.set_log("3g-4g-rrc.txt") #save the analysis result

    #Start the monitoring
    src.run()