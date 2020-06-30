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
except ImportError as e:
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
            ("4b1327000800000028000000b9172f6e762f6974656d5f66696c65732f6d6f64656d2f6d6d6f64652f6c74655f62616e647072656600",""),\
            ("4b1326000800000040000800ba1708000000000000002f6e762f6974656d5f66696c65732f6d6f64656d2f6d6d6f64652f6c74655f62616e647072656600",""),\
            ]

    for payld, frame in tests:
        payld = binascii.a2b_hex(payld)
        frame = binascii.a2b_hex(frame)
        print(("Correct answer: " + binascii.b2a_hex(frame)))
        print(("Construct: " + binascii.b2a_hex(HdlcFrame(payld).binary())))
