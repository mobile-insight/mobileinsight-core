# -*- coding: utf-8 -*-
"""
hdlc_frame.py
Define HdlcFrame, an easy-to-use HDLC-like frame constructor.

Author: Jiayao Li
"""

__all__ = ["HdlcFrame"]

import struct
import crcmod
import binascii

try:
    from utils import *
except ImportError, e:
    # TODO: WTF can I do to remove this dependence ..?
    def static_var(varname, value):
        def decorate(func):
            setattr(func, varname, value)
            return func
        return decorate


class HdlcFrame:
    """
    An easy HDLC-like frame constructor class.
    """
    
    def __init__(self, payld):
        """
        Initialize a frame given payload.

        Args:
            payld: a binary string
        """
        s = self._calc_checksum(payld)
        fcs = struct.pack("<H", s)  # little endian
        self._binary = payld + fcs
        self._binary = self._escape(self._binary)
        self._binary += "\x7e"

    def binary(self):
        """
        Return the binary form of the frame.
        """
        return self._binary

    @classmethod
    def _calc_checksum(cls, payld):
        crc16 = crcmod.predefined.Crc('x-25')
        for c in payld:
            crc16.update(c)
        calc_crc = struct.unpack(">H", crc16.digest())[0]   # big endian
        return calc_crc

    @classmethod
    @static_var("escaped_chars", {"\x7d", "\x7e"})
    def _escape(cls, b):
        escaped = []
        for c in b:
            if c in cls._escape.escaped_chars:
                escaped_c = chr(ord(c) ^ 0x20)
                escaped.append("\x7d" + escaped_c)
            else:
                escaped.append(c)
        escaped = ''.join(escaped)
        return escaped


# Test decoding
if __name__ == '__main__':
    tests = [("7d028813a513", "7d5d028813a513c3407e"),\
            ("7d05000000000000", "7d5d0500000000000074417e"),\
            ("7300000000000000", "7300000000000000da817e"),\
            ("1d", ""),\
            ("00", ""),\
            ("7c", ""),\
            ("1c", ""),\
            ("0c", ""),\
            ("63", ""),\
            ("4b0f0000", ""),\
            ("4b090000", ""),\
            ("4b080000", ""),\
            ("4b080100", ""),\
            ("4b040000", ""),\
            ("4b040f00", ""),\
            ("6000", ""),\
            ("\
            73000000\
            03000000\
            0b000000\
            c5010000\
            00000000\
            00000000\
            00000000\
            00000000\
            00000000\
            00000000\
            01000000\
            00000000\
            00000000\
            00000000\
            00000000\
            00000000\
            00000000\
            00000000\
            00",""),\
            ]

    for payld, frame in tests:
        payld = binascii.a2b_hex(payld)
        frame = binascii.a2b_hex(frame)
        print "Correct answer: " + binascii.b2a_hex(frame)
        print "Construct: " + binascii.b2a_hex(HdlcFrame(payld).binary())
