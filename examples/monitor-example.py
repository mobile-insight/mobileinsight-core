#!/usr/bin/python
# Filename: monitor-example.py
import os
import sys

# Import MobileInsight modules
from mobile_insight.monitor import OnlineMonitor
from mobile_insight.analyzer import MsgLogger

if __name__ == "__main__":

    if len(sys.argv) < 3:
        print("Error: please specify physical port name and baudrate.")
        print((__file__, "SERIAL_PORT_NAME BAUNRATE"))
        sys.exit(1)

    # Initialize a 3G/4G monitor
    src = OnlineMonitor()
    src.set_serial_port(sys.argv[1])  # the serial port to collect the traces
    src.set_baudrate(int(sys.argv[2]))  # the baudrate of the port

    # Save the monitoring results as an offline log
    src.save_log_as("./monitor-example.mi2log")

    # Enable 3G/4G messages to be monitored. Here we enable RRC (radio
    # resource control) monitoring
    src.enable_log("5G_NR_RRC_OTA_Packet")
    #src.enable_log("LTE_RRC_OTA_Packet")
    #src.enable_log("WCDMA_RRC_OTA_Packet")
    # src.enable_log("WCDMA_RRC_Serv_Cell_Info")
    #src.enable_log("5G_NR_RRC_OTA_Packet")
    src.enable_log("5G_NR_PDCP_UL_Control_Pdu")
    src.enable_log("5G_NR_L2_UL_TB")
    src.enable_log("5G_NR_L2_UL_BSR")
    src.enable_log("5G_NR_RLC_DL_Stats")
    src.enable_log("5G_NR_MAC_UL_TB_Stats")
    src.enable_log("5G_NR_MAC_UL_Physical_Channel_Schedule_Report")
    src.enable_log("5G_NR_MAC_PDSCH_Stats")
    src.enable_log("5G_NR_MAC_RACH_Trigger")
    src.enable_log("5G_NR_ML1_Serving_Cell_Beam_Management")
    src.enable_log("5G_NR_MAC_RACH_Attempt")
    #src.enable_log("5G_NR_MAC_DCI_Info")

    # Dump the messages to std I/O. Comment it if it is not needed.
    dumper = MsgLogger()
    dumper.set_source(src)
    dumper.set_decoding(MsgLogger.XML)  # decode the message as xml

    # Start the monitoring
    src.run()
