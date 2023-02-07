#!/usr/bin/python
# Filename: 5g_pdcp_testing.py
import os
import sys
import math

"""
Offline analysis by replaying logs
"""

# Import MobileInsight modules
from mobile_insight.monitor import OfflineReplayer
from mobile_insight.analyzer import MsgLogger, NrPdcpDlDataPduAnalyzer
if __name__ == "__main__":

    # Initialize a monitor
    src = OfflineReplayer()
    src.set_input_path("./logs/")

    #src.enable_log("NR_PDCP_DL_Data_Pdu")

    logger = MsgLogger()
    logger.set_decode_format(MsgLogger.DICT)
    logger.set_dump_type(MsgLogger.FILE_ONLY)
    logger.save_decoded_msg_as("./decoded_nr_pdcp_dl_data_pdu.txt")
    logger.set_source(src)

    # # Analyzers
    nr_dci_analyzer = NrPdcpDlDataPduAnalyzer()
    nr_dci_analyzer.set_source(src) # bind with the monitor 
    
    src.run()