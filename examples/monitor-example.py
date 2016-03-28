#!/usr/bin/python
# Filename: monitor-example.py
import os
import sys

#Import MobileInsight modules
from mobile_insight.monitor import DMCollector
from mobile_insight.analyzer import MsgLogger,MsgSerializer

if __name__ == "__main__":
    
    if len(sys.argv) < 3:
        print "Error: please specify physical port name and baudrate."
        print __file__,"SERIAL_PORT_NAME BAUNRATE"
        sys.exit(1)

    # Initialize a 3G/4G monitor
    src = DMCollector()
    src.set_serial_port(sys.argv[1]) #the serial port to collect the traces
    src.set_baudrate(int(sys.argv[2])) #the baudrate of the port

    #Enable 3G/4G messages to be monitored. Here we enable RRC (radio resource control) monitoring
    src.enable_log("LTE_RRC_OTA_Pdacket")
    src.enable_log("WCDMA_RRC_OTA_Packet")
    src.enable_log("WCDMA_CELL_ID")

    #Dump the messages to std I/O. Comment it if it is not needed.
    dumper = MsgLogger()
    dumper.set_source(src)
    dumper.set_decoding(MsgLogger.XML)  #decode the message as xml

    #Save the messages to disk. Comment it if it is not needed.
    serializer = MsgSerializer()
    serializer.set_source(src)
    serializer.set_output_path("3g-4g-rrc.replay")

    #Start the monitoring
    src.run()