# -*- coding: utf-8 -*-
"""
hdlc_frame.py
Define hdlc_frame, an easy HDLC-like frame constructor class.

Author: Jiayao Li
"""

import struct
import crcmod
import binascii

from utils import static_var


class hdlc_frame:
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


if __name__ == '__main__':
    tests = [("7d028813a513", "7d5d028813a513c3407e")]

    for payld, frame in tests:
        payld = binascii.a2b_hex(payld)
        frame = binascii.a2b_hex(frame)
        print "Correct answer: " + binascii.b2a_hex(frame)
        print "Construct: " + binascii.b2a_hex(hdlc_frame(payld).binary())
