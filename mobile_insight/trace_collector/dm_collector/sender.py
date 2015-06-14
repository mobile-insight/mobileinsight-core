# -*- coding: utf-8 -*-
"""
sender.py

Tool to send commands to a Qualcomm phone.

Author: Jiayao Li, Samson Richard Wong
"""

import sys
import time
import string
import serial
import serial.tools.list_ports
import optparse
import binascii
from hdlc_parser import HdlcParser
from hdlc_frame import HdlcFrame

__all__ = ["sendRecv", "recvMessage"]


def init_opt():
    """
    Initialize and return the option parser.
    """
    opt = optparse.OptionParser(prog="com-sender",
                                usage="usage: %prog [options] LOGFILE ...",
                                description="COM serial port sender.")
    opt.add_option("-p", "--phy-serial-name",
                    metavar="STR",
                    action="store", type="string", dest="phy_serial_name", 
                    help="Manually set the name of physical serial name.")
    opt.add_option("--phy-baudrate",
                    metavar="N",
                    action="store", type="int", dest="phy_baudrate", 
                    help="Set the physical baud rate [default: %default].")
    opt.set_defaults(phy_baudrate=9600)
    return opt


def str_to_hex(s):
    """
    Convert each character of a string to its hex value, then return a
    whitespace-separated string of hex values.
    """
    return " ".join(c.encode("hex") for c in s)

def sendRecv(parser, phy_ser, b, cmd = None):
    if cmd == None:
        cmd = "%02x" % ord(b[0])
    sendMessage(phy_ser, b)
    return recvMessage(parser, phy_ser, cmd)

def sendMessage(phy_ser, b):
    b = HdlcFrame(b).binary()

    if b:
        phy_ser.write(b)

def recvMessage(parser, phy_ser, cmd):
    """
    Reads from a COM port and looks for a reply that starts with the same characters given by the cmd parameter.
    Returns a tuple (payload, crc_correct).

    If 
    """
    s = phy_ser.read(64)
    isReply = False
    rtn = (None, None)
    while s:
        parser.feed_binary(0,s)
        for t, payload, fcs, crc_correct in parser:
            # Irrelevant messages are discarded.
            if payload[0:1] == binascii.a2b_hex(cmd):
                rtn = (payload, crc_correct)
                isReply = True
            break
        if isReply:
            break
        s = phy_ser.read(64)
    return rtn

if __name__ == "__main__":
    opt = init_opt()
    options, args = opt.parse_args(sys.argv[1:])

    if options.phy_serial_name:
        phy_ser_name = options.phy_serial_name
    print "PHY COM: %s" % phy_ser_name

    if phy_ser_name is None:
        sys.stderr.write("Serial port name error.\n")
        sys.exit(1)

    phy_baudrate = options.phy_baudrate
    print "PHY BAUD RATE: %d" % phy_baudrate

    try:
        # Open COM ports. A zero timeout means that IO functions never suspend.
        phy_ser = serial.Serial(phy_ser_name, baudrate=phy_baudrate, timeout=.5)
        parser = HdlcParser()

        while True:
            s = raw_input('enter a command: ')
            sendMessage(phy_ser, s)
            payload, crc_correct = recvMessage(parser, phy_ser, s[0:2])
            print "reply: " + binascii.b2a_hex(payload)
            print "crc_correct: " + crc_correct

    except IOError, e:
        sys.exit(e)
    except Exception,e:
        sys.exit(e)
