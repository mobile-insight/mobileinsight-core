#! /usr/bin/env python
"""
dm_collector.py

A QCDM trace collector.
Currently it's a re-package of automator

Author: Jiayao Li

"""

__all__ = ["DMCollector"]

from ..trace_collector import *

import binascii
import os
import optparse
import serial
import sys
import timeit

from dm_endec import *
import dm_collector_c


def print_reply(payload, crc_correct):
    if payload:
        print "reply: " + binascii.b2a_hex(payload)
        print "crc_correct: " + str(crc_correct)

class DMCollector(TraceCollector):

    def __init__(self, prefs):
        """
        Configure this class with user preferences.

        This method should be called before any actual decoding.

        Args:
            prefs: a dict that should contain the following items:
                ws_dissect_executable_path
                libwireshark_path
        """
        TraceCollector.__init__(self)

        self.phy_baudrate = 9600
        self.phy_ser_name = None
        self.supported_types = set(dm_collector_c.log_packet_types)
        self._prefs = prefs
        self._type_names = []
        # Initialize Wireshark dissector
        DMLogPacket.init(self._prefs)

    def set_serial_port(self, phy_ser_name):
        self.phy_ser_name = phy_ser_name

    def set_baudrate(self, rate):
        self.phy_baudrate = rate

    def enable_log(self, type_name):
        if isinstance(type_name, str):
            type_name = [type_name]
        for n in type_name:
            if n not in self.supported_types:
                raise ValueError("Unsupported log packet type: %s" % n)
            else:
                self._type_names.append(n)

    def _generate_type_dict(self):
        assert len(self._type_ids) > 0
        type_ids = set(self._type_ids)
        s = set([DMLogConfigMsg.get_equip_id(i) for i in type_ids])
        self._type_id_dict = {i: [] for i in s}
        for type_id in type_ids:
            self._type_id_dict[DMLogConfigMsg.get_equip_id(type_id)].append(type_id)

    def run(self):
        """
        Start collecting the QXDM traces.
        """
        assert self.phy_ser_name

        print "PHY COM: %s" % self.phy_ser_name
        print "PHY BAUD RATE: %d" % self.phy_baudrate

        print "Supported type:", self.supported_types

        try:
            # Open COM ports
            phy_ser = serial.Serial(self.phy_ser_name,
                                    baudrate=self.phy_baudrate,
                                    timeout=.5)
            # phy_ser = open("hahaha.txt", "r+b")

            # Disable logs
            print "Disable logs"
            dm_collector_c.disable_logs(phy_ser)
            
            # Enable logs
            print "Enable logs"
            dm_collector_c.enable_logs(phy_ser, self._type_names)

            # Read log packets from serial port and decode their contents
            while True:
                s = phy_ser.read(64)
                dm_collector_c.feed_binary(s)
                decoded = dm_collector_c.receive_log_packet()
                if decoded:
                    # print_reply(payload, crc_correct)
                    try:
                        # Note that the beginning 2 bytes are skipped.
                        packet = DMLogPacket(decoded)
                        d = packet.decode()
                        # xml = packet.decode_xml()
                        # print hex(d["type_id"]), dm_endec.consts.LOG_PACKET_NAME[d["type_id"]], d["timestamp"]
                        # print xml
                        # print ""
                        # Send event to analyzers
                        event = Event(  timeit.default_timer(),
                                        d["type_id"],
                                        packet)
                        self.send(event)
                    except FormatError, e:
                        # skip this packet
                        print "FormatError: ", e


        except (KeyboardInterrupt, RuntimeError), e:
            print "\n\n%s Detected: Disabling all logs" % type(e).__name__
            # Disable logs
            dm_collector_c.disable_logs(phy_ser)
            phy_ser.close()
            sys.exit(e)
        except Exception, e:
            sys.exit(e)
