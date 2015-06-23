#!/usr/bin/python
# Filename: dm_collector.py
"""
A monitor for 2G/3G/4G mobile network protocols (RRC/EMM/ESM). 
It currently supports mobile devices with Qualcomm chipsets.

Author: Jiayao Li
"""

__all__ = ["DMCollector"]

from ..monitor import *

import binascii
import os
import optparse
import serial
import sys
import timeit

from sender import sendRecv, recvMessage
from hdlc_parser import HdlcParser
from dm_endec import DMLogPacket, DMLogConfigMsg, FormatError
import dm_endec.consts


def print_reply(payload, crc_correct):
    if payload:
        print "reply: " + binascii.b2a_hex(payload)
        print "crc_correct: " + str(crc_correct)

class DMCollector(Monitor):

    """
    A monitor for 2G/3G/4G mobile network protocols (RRC/EMM/ESM). 
    It currently supports mobile devices with Qualcomm chipsets.
    """

    def __init__(self, prefs):
        """
        Configure this class with user preferences.
        This method should be called before any actual decoding.

        param prefs: configurations that contain *ws_dissect_executable_path* and *libwireshark_path*
        type prefs: dictionary
        """
        Monitor.__init__(self)

        self.phy_baudrate = 9600
        self.phy_ser_name = None
        self._prefs = prefs
        self._type_ids = []

    def set_serial_port(self, phy_ser_name):
        """
        Configure the serial port that dumps messages

        :param phy_ser_name: the serial port name (path)
        :type phy_ser_name: string
        """
        self.phy_ser_name = phy_ser_name

    def set_baudrate(self, rate):
        """
        Configure the baudrate of the serial port

        :param rate: the baudrate of the port
        :type rate: int
        """
        self.phy_baudrate = rate

    def enable_log(self, type_name):
        """
        Enable the messages to be monitored.
        Currently DMCollector supports the following logs:

        WCDMA_CELL_ID

        WCDMA_Signaling_Messages

        LTE_RRC_OTA_Packet

        LTE_RRC_MIB_Message_Log_Packet

        LTE_RRC_Serv_Cell_Info_Log_Packet

        LTE_NAS_ESM_Plain_OTA_Incoming_Message

        LTE_NAS_ESM_Plain_OTA_Outgoing_Message

        LTE_NAS_EMM_Plain_OTA_Incoming_Message

        LTE_NAS_EMM_Plain_OTA_Outgoing_Message

        LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results

        LTE_ML1_IRAT_Measurement_Request

        LTE_ML1_Serving_Cell_Measurement_Result

        LTE_ML1_Connected_Mode_Neighbor_Meas_Req/Resp

        :param type_name: the message type to be monitored
        :type type_name: string
        """
        if isinstance(type_name, str):
            type_name = [type_name]
        for n in type_name:
            if n not in dm_endec.consts.LOG_PACKET_ID:
                raise ValueError("Unsupported log packet type: %s" % n)
            else:
                self._type_ids.append(dm_endec.consts.LOG_PACKET_ID[n])

    def _generate_type_dict(self):
        """
        Create a dictionary that maps message type to the binary code
        """
        assert len(self._type_ids) > 0
        type_ids = set(self._type_ids)
        s = set([DMLogConfigMsg.get_equip_id(i) for i in type_ids])
        self._type_id_dict = {i: [] for i in s}
        for type_id in type_ids:
            self._type_id_dict[DMLogConfigMsg.get_equip_id(type_id)].append(type_id)

    def run(self):
        """
        Start monitoring the mobile network. This is usually the entrance of monitoring and analysis.

        :except KeyboardInterrupt: if the user interrupts the monitoring (e.g. ctrl-C)
        :except RuntimeError: if unexpected runtime errors occur 
        """
        assert self.phy_ser_name

        print "PHY COM: %s" % self.phy_ser_name
        print "PHY BAUD RATE: %d" % self.phy_baudrate

        try:
            # Initialize Wireshark dissector
            DMLogPacket.init(self._prefs)

            # Open COM ports
            phy_ser = serial.Serial(self.phy_ser_name,
                                    baudrate=self.phy_baudrate,
                                    timeout=.5)
            parser = HdlcParser()

            # Disable logs
            payload, crc_correct = sendRecv(parser, phy_ser, DMLogConfigMsg("DISABLE").binary())
            print_reply(payload, crc_correct)
            
            # Enable logs
            self._generate_type_dict()
            for equip_id, type_ids in self._type_id_dict.items():
                print "Enabled: ", ["0x%04x" % i for i in type_ids]
                x = DMLogConfigMsg("SET_MASK", type_ids)
                payload, crc_correct = sendRecv(parser, phy_ser, x.binary())
                print_reply(payload, crc_correct)

            # Read log packets from serial port and decode their contents
            while True:
                # cmd = 0x10 for log packets
                cmd = "%02x" % dm_endec.consts.COMMAND_CODE["DIAG_CMD_LOG"]
                payload, crc_correct = recvMessage(parser, phy_ser, cmd)
                if payload:
                    # print_reply(payload, crc_correct)
                    try:
                        # Note that the beginning 2 bytes are skipped.
                        packet = DMLogPacket(payload[2:])
                        d = packet.decode()
                        # xml = packet.decode_xml()
                        # print hex(d["type_id"]), dm_endec.consts.LOG_PACKET_NAME[d["type_id"]], d["timestamp"]
                        # print xml
                        # print ""
                        # Send event to analyzers
                        event = Event(  timeit.default_timer(),
                                        dm_endec.consts.LOG_PACKET_NAME[d["type_id"]],
                                        packet)
                        self.send(event)
                    except FormatError, e:
                        # skip this packet
                        print "FormatError: ", e


        except (KeyboardInterrupt, RuntimeError), e:
            print "\n\n%s Detected: Disabling all logs" % type(e).__name__
            # Disable logs
            payload, crc_correct = sendRecv(parser, phy_ser, DMLogConfigMsg("DISABLE").binary())
            print_reply(payload, crc_correct)
            sys.exit(e)
        except Exception, e:
            sys.exit(e)
