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
    nr_pdcp_dl_data_analyzer = NrPdcpDlDataPduAnalyzer()
    nr_pdcp_dl_data_analyzer.set_source(src) # bind with the monitor 
    
    src.run()

    start_time = 0
    end_time = math.inf

    nr_pdcp_dl_data_analyzer.draw_num_IP_packets(figure_size=(10,4), start_time=start_time, end_time=end_time)

    nr_pdcp_dl_data_analyzer.draw_num_IP_packets_assignment(figure_size=(10,4), start_time=start_time, end_time=end_time)
    nr_pdcp_dl_data_analyzer.draw_num_IP_bytes_assignment(figure_size=(10,4), start_time=start_time, end_time=end_time)

    nr_pdcp_dl_data_analyzer.draw_rlc_path(figure_size=(10,4), start_time=start_time, end_time=end_time)
    nr_pdcp_dl_data_analyzer.draw_route_status(figure_size=(10,4), start_time=start_time, end_time=end_time)

    #nr_pdcp_dl_data_analyzer.draw_start_count_assignment(figure_size=(10,4), start_time=start_time, end_time=end_time)
    nr_pdcp_dl_data_analyzer.draw_end_count_assignment(figure_size=(10,4), start_time=start_time, end_time=end_time)
    nr_pdcp_dl_data_analyzer.draw_rlc_end_sn_assignment(figure_size=(10,4), start_time=start_time, end_time=end_time)
    nr_pdcp_dl_data_analyzer.draw_num_pdcp_sn(figure_size=(10,4), start_time=start_time, end_time=end_time)

    nr_pdcp_dl_data_analyzer.reset()