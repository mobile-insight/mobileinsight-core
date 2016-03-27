#!/usr/bin/python
# Filename: online-analysis-qmdl.py
import os
import sys

"""
Offline analysis with Qualcomm's QMDL traces
"""

#Import MobileInsight modules
from mobile_insight.monitor import QmdlReplayer
from mobile_insight.analyzer import LteRrcAnalyzer,WcdmaRrcAnalyzer,LteNasAnalyzer,UmtsNasAnalyzer

if __name__ == "__main__":
    
    # Initialize a 3G/4G monitor
    src = QmdlReplayer()
    src.set_input_path("./offline_log_example.mi2log")

    #Enable 3G/4G RRC (radio resource control) monitoring
    src.enable_log("LTE_RRC_OTA_Packet")
    src.enable_log("WCDMA_Signaling_Messages")

    #RRC analyzer
    lte_rrc_analyzer = LteRrcAnalyzer()
    lte_rrc_analyzer.set_source(src) #bind with the monitor
    
    wcdma_rrc_analyzer = WcdmaRrcAnalyzer()
    wcdma_rrc_analyzer.set_source(src) #bind with the monitor


    lte_nas_analyzer = LteNasAnalyzer()
    lte_nas_analyzer.set_source(src)

    umts_nas_analyzer = UmtsNasAnalyzer()
    umts_nas_analyzer.set_source(src)

    #Start the monitoring
    src.run()
