# -*- coding: utf-8 -*-
"""
sender.py
COM port monitor. Work with SAMSUNG phone and com0com, a Null-modem emulator.
To learn about com0com, see http://com0com.sourceforge.net/ .

Author: Jiayao Li, Samson Richard Wong
"""

import sys
import time
import string
import serial
import serial.tools.list_ports
import optparse
import binascii
from hdlc_parser import hdlc_parser
from hdlc_frame import hdlc_frame


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

def detect_ports():
    """
    Automatically detect and return the physical COM port name.
    """
    ports = serial.tools.list_ports.comports()
    phy_ser_name = None
    for name, description, hardware_id in ports:
        # Find the physical name for SAMSUNG phone
        # WARNING(Jiayao): in usual cases there will be more than one port found.
        # On my computer, the lowest one is what we want. This may vary from 
        # computer to computer.
        if description.startswith("SAMSUNG Mobile USB Serial Port"):
            if phy_ser_name is None or phy_ser_name > name:
                phy_ser_name = name
    return phy_ser_name


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
        parser = hdlc_parser()

        while True:
            s = raw_input('enter a command: ')
            s = s.replace(" ", "")
            s = binascii.a2b_hex(s)
            s = hdlc_frame(s).binary()

            print "s: " + binascii.b2a_hex(s)

            cmd = s[0:1]

            if s:
                phy_ser.write(s)

            s = phy_ser.read(64)
            isCorrectPacket = False
            while s:
                parser.feed_binary(0,s)
                for t, payload, fcs, crc_correct in parser:
                    print "payload: " + payload
                    print "cmd: " + binascii.b2a_hex(cmd)
                    if payload[0:1] == cmd:
                        print('reply: ' + str_to_hex(payload))
                        print('crc_correct: ' + repr(crc_correct))
                        isCorrectPacket = True
                    break
                if isCorrectPacket:
                    break
                s = phy_ser.read(64)

    except IOError, e:
        sys.exit(e)
    except Exception,e:
        sys.exit(e)
