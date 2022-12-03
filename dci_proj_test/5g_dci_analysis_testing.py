#!/usr/bin/python
# Filename: 5g_dci_analysis_testing.py
import os
import sys

"""
Offline analysis by replaying logs
"""

# Import MobileInsight modules
from mobile_insight.monitor import OfflineReplayer
from mobile_insight.analyzer import MsgLogger, NrDciAnalyzer
if __name__ == "__main__":

    # Initialize a monitor
    src = OfflineReplayer()
    src.set_input_path("./logs/")
    # src.enable_log_all()

    src.enable_log("NR_DCI_Message")


    logger = MsgLogger()
    logger.set_decode_format(MsgLogger.DICT)
    logger.set_dump_type(MsgLogger.FILE_ONLY)
    logger.save_decoded_msg_as("./decoded_dci.txt")
    logger.set_source(src)

    # # Analyzers
    nr_dci_analyzer = NrDciAnalyzer()
    nr_dci_analyzer.set_source(src) # bind with the monitor 
    
    src.run()

    nr_dci_analyzer.draw_assignment_pattern()
    nr_dci_analyzer.draw_throughput(outlier_filter_m=3)
    nr_dci_analyzer.reset()