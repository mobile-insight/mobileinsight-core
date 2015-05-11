# -*- coding: utf-8 -*-
"""
ws_dissector.py
Define WSDissector class.

Author: Jiayao Li
"""

__all__ = ["WSDissector"]

import os
import binascii
import struct
import subprocess


class WSDissector:
    """
    A wrapper class of the ws_dissector program, which calls functions in
    libwireshark to dissect many types of messages, e.g. 3GPP standardized.

    This wrapper communicates with the ws_dissector program using a trivial
    TLV-formatted protocol named AWW (Automator Wireshark Wrapper), through
    the standard input/output interfaces.
    """

    # maps all supported message types to their AWW protocol number.
    SUPPORTED_TYPES = { # WCDMA RRC
                        "DL_BCCH_BCH": 100,
                        # LTE RRC
                        "LTE-RRC_PCCH": 200,
                        "LTE-RRC_DL_DCCH": 201,
                        "LTE-RRC_UL_DCCH": 202,
                        "LTE-RRC_BCCH_DL_SCH": 203
                        }
    proc = None
    init_proc_called = False

    @classmethod
    def init_proc(cls, executable_path, ws_library_path):
        """
        Launch the ws_dissector program. Must be called before any actual decoding.

        Args:
            executable_path: the path of ws_dissect program.
            ws_library_path: a directory that contains libwireshark.
        """

        if cls.init_proc_called:
            return
        env = dict(os.environ)
        env["LD_LIBRARY_PATH"] = ws_library_path + ":" + env.get("LD_LIBRARY_PATH", "")
        cls.proc = subprocess.Popen([executable_path],
                                    bufsize=-1,
                                    stdin=subprocess.PIPE,
                                    stdout=subprocess.PIPE,
                                    env=env
                                    )
        cls.init_proc_called = True


    @classmethod
    def decode_msg(cls, msg_type, b):
        """
        Decode a binary message of type msg_type.

        Args:
            msg_type: a string identifying the type of the message to be decoded
            b: binary data
        """
        assert cls.init_proc_called
        if msg_type not in cls.SUPPORTED_TYPES:
            return None

        input_data = struct.pack(
                                    "!II",  # in network order
                                    len(b),
                                    cls.SUPPORTED_TYPES[msg_type],
                                )
        input_data += b
        
        cls.proc.stdin.write(input_data)
        cls.proc.stdin.flush()
        cls.proc.stdout.flush()
        result = []
        while True:
            line = cls.proc.stdout.readline()
            if line.startswith("===___==="):
                break
            result.append(line)

        return "".join(result)


# Test decoding
if __name__ == '__main__':
    b = "4001BF281AEBA00000"
    WSDissector.init_proc()

    for i in range(1):
        print WSDissector.decode_msg("LTE-RRC_PCCH", binascii.a2b_hex(b))
