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
import xml.etree.ElementTree as ET

from sender import sendRecv, recvMessage
from hdlc_parser import hdlc_parser
from dm_log_packet import DMLogPacket, FormatError
from dm_log_packet import consts as dm_log_consts
from dm_log_config_msg import DMLogConfigMsg


def init_cmd_dict(cmd_dict_path):
    cmd_dict = {}
    with open(cmd_dict_path, 'r') as cmd_dict_file:
        for line in cmd_dict_file:
            line = line.strip()
            if len(line) > 0 and not line.startswith('#'):
                cmd, colon, binary = line.partition(':')
                cmd = cmd.strip()
                binary = binary.strip()
                cmd_dict[cmd] = binary
    return cmd_dict

def init_target_cmds(cmd_file_path):
    target_cmds = []
    try:
        with open(cmd_file_path, 'r') as cmd_file:
            for line in cmd_file:
                line = line.strip()
                line = line.replace('\n','')
                line = line.upper()
                if len(line) > 0 and not line.startswith('#'):
                    target_cmds.append(line)
    except IOError, e:
        raise RuntimeError("Cannot find command file %s. You can create it using example_cmds.txt as template." % cmd_file_path)
    return target_cmds

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
        self._prefs = prefs
        self._type_ids = []

    def set_serial_port(self, phy_ser_name):
        self.phy_ser_name = phy_ser_name

    def set_baudrate(self, rate):
        self.phy_baudrate = rate

    def enable_log(self, type_name):
        if isinstance(type_name, str):
            type_name = [type_name]
        for n in type_name:
            if n not in dm_log_consts.LOG_PACKET_ID:
                raise ValueError("Unsupported log packet type: %s" % n)
            else:
                self._type_ids.append(dm_log_consts.LOG_PACKET_ID[n])

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

        try:
            # Initialize Wireshark dissector
            DMLogPacket.init(self._prefs)

            # Open COM ports
            phy_ser = serial.Serial(self.phy_ser_name,
                                    baudrate=self.phy_baudrate,
                                    timeout=.5)
            parser = hdlc_parser()

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
                payload, crc_correct = recvMessage(parser, phy_ser, "10") 
                if payload:
                    # print_reply(payload, crc_correct)
                    # Note that the beginning 2 bytes are skipped.
                    l, type_id, ts, log_item = DMLogPacket.decode(payload[2:])
                    # print l, hex(type_id), dm_log_consts.LOG_PACKET_NAME[type_id], ts
                    # print log_item

                    # print ""
                    
                    # conver the message to xml
                    log_item_dict = dict(log_item)
                    if log_item_dict.has_key('Msg'):
                        log_xml = ET.fromstring(log_item_dict['Msg'])
                        #send event to analyzers
                        event = Event(ts,dm_log_consts.LOG_PACKET_NAME[type_id],log_xml)
                        self.send(event)
                    # if log is not None:
                    #     log.write("%s\n" % ts)
                    #     log.write("Binary: " + binascii.b2a_hex(payload) + "\n")
                    #     log.write("Length: %d, Type: 0x%x(%s)\n" % (l, type_id, dm_log_consts.LOG_PACKET_NAME[type_id]))
                    #     log.write(str(log_item))
                    #     log.write("\n\n")

        except (KeyboardInterrupt, RuntimeError), e:
            print "\n\n%s Detected: Disabling all logs" % type(e).__name__
            # Disable logs
            payload, crc_correct = sendRecv(parser, phy_ser, DMLogConfigMsg("DISABLE").binary())
            print_reply(payload, crc_correct)
            sys.exit(e)
        except IOError, e:
            sys.exit(e)
        except Exception,e:
            sys.exit(e)
