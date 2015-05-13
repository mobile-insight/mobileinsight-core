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

from sender import sendRecv, recvMessage
from hdlc_parser import hdlc_parser
from dm_log_packet import DMLogPacket, FormatError
from dm_log_packet import consts as dm_log_consts

import xml.etree.ElementTree as ET

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
                command_files_path
                ws_dissect_executable_path
                libwireshark_path
        """
        TraceCollector.__init__(self)

        self.phy_baudrate = 9600
        self.phy_ser_name = None
        self._prefs = prefs

    def set_serial_port(self, phy_ser_name):
        self.phy_ser_name = phy_ser_name

    def set_baudrate(self, rate):
        self.phy_baudrate = rate

    def run(self):
        """
        Start collecting the QXDM traces.
        """
        assert self.phy_ser_name

        # FIXME: move to the path configuration to separate member functions
        print "PHY COM: %s" % self.phy_ser_name
        print "PHY BAUD RATE: %d" % self.phy_baudrate

        # Load configurations
        cmd_dict_path = os.path.join(self._prefs["command_files_path"], 'cmd_dict.txt')
        cmd_dict = init_cmd_dict(cmd_dict_path)

        cmd_file_path = os.path.join(self._prefs["command_files_path"], 'cmds.txt')
        target_cmds = init_target_cmds(cmd_file_path)

        try:
            # Initialize Wireshark dissector
            DMLogPacket.init(self._prefs)

            # Open COM ports
            phy_ser = serial.Serial(self.phy_ser_name,
                                    baudrate=self.phy_baudrate,
                                    timeout=.5)
            parser = hdlc_parser()

            # Disable logs
            payload, crc_correct = sendRecv(parser, phy_ser, cmd_dict.get("DISABLE"))
            print_reply(payload, crc_correct)
            
            # Enable logs
            for cmd in target_cmds:
                print "Enable: " + cmd
                binary = cmd_dict.get(cmd)
                if binary is None:      # To Samson: what does it exactly mean?
                    binary = cmd
                payload, crc_correct = sendRecv(parser, phy_ser, binary)
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
                    
                    #send event to analyzers
                    event = Event(ts,dm_log_consts.LOG_PACKET_NAME[type_id],log_item)
                    self.send(event)

        except (KeyboardInterrupt, RuntimeError), e:
            print "\n\n%s Detected: Disabling all logs" % type(e).__name__
            # Disable logs
            payload, crc_correct = sendRecv(parser, phy_ser, cmd_dict.get("DISABLE"))
            print_reply(payload, crc_correct)
            sys.exit(e)
        except IOError, e:
            sys.exit(e)
        except Exception,e:
            sys.exit(e)
