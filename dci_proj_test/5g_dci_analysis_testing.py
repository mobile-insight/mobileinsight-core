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

    start_time = 90000
    end_time = 110000
    nr_dci_analyzer.draw_assignment_pattern(figure_size=(35,4), start_time=start_time, end_time=end_time)
    nr_dci_analyzer.draw_throughput_ul(figure_size=(50,4), outlier_filter_m=3, start_time=start_time, end_time=end_time)
    nr_dci_analyzer.draw_throughput_dl(figure_size=(50,4), outlier_filter_m=3, start_time=start_time, end_time=end_time)
    nr_dci_analyzer.reset()