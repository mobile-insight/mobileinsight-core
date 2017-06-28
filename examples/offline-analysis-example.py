#!/usr/bin/python
# Filename: offline-analysis-example.py
import os
import sys

"""
Offline analysis by replaying logs
"""

# Import MobileInsight modules
from mobile_insight.monitor import OfflineReplayer
from mobile_insight.analyzer import MsgLogger, LteRrcAnalyzer, WcdmaRrcAnalyzer, LteNasAnalyzer, UmtsNasAnalyzer, LtePhyAnalyzer, LteMacAnalyzer

if __name__ == "__main__":

    # Initialize a 3G/4G monitor
    src = OfflineReplayer()
    src.set_input_path("./offline_log_example.mi2log")

    logger = MsgLogger()
    logger.set_decode_format(MsgLogger.XML)
    logger.set_dump_type(MsgLogger.FILE_ONLY)
    logger.save_decoded_msg_as("./test.txt")
    logger.set_source(src)

    # Analyzers
    lte_rrc_analyzer = LteRrcAnalyzer()
    lte_rrc_analyzer.set_source(src)  # bind with the monitor

    wcdma_rrc_analyzer = WcdmaRrcAnalyzer()
    wcdma_rrc_analyzer.set_source(src)  # bind with the monitor

    lte_nas_analyzer = LteNasAnalyzer()
    lte_nas_analyzer.set_source(src)

    umts_nas_analyzer = UmtsNasAnalyzer()
    umts_nas_analyzer.set_source(src)

    lte_phy_analyzer = LtePhyAnalyzer()
    lte_phy_analyzer.set_source(src)

    lte_mac_analyzer = LteMacAnalyzer()
    lte_mac_analyzer.set_source(src)

    # Start the monitoring
    src.run()
