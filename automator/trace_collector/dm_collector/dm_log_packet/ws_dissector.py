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
    A wrapper class of the ws_dissector program, which calls functions 
    in libwireshark to dissect many types of messages, e.g. 3GPP 
    standardized messages.

    This wrapper communicates with the ws_dissector program using a 
    trivial TLV-formatted protocol named AWW (Automator Wireshark Wrapper), 
    through the standard input/output interfaces.
    """

    # Maps all supported message types to their AWW protocol number.
    # Keep consistent with ws_dissector/packet-aww.cpp
    SUPPORTED_TYPES = { # WCDMA RRC
                        "RRC_DL_BCCH_BCH": 100,
                        "RRC_MIB": 150,
                        "RRC_SIB1": 151,
                        "RRC_SIB7": 157,
                        "RRC_SIB12": 162,
                        # LTE RRC
                        "LTE-RRC_PCCH": 200,
                        "LTE-RRC_DL_DCCH": 201,
                        "LTE-RRC_UL_DCCH": 202,
                        "LTE-RRC_BCCH_DL_SCH": 203,
                        }
    _proc = None
    _init_proc_called = False

    @classmethod
    def init_proc(cls, executable_path, ws_library_path):
        """
        Launch the ws_dissector program. Must be called before any actual 
        decoding.

        Args:
            executable_path: the path of ws_dissect program.
            ws_library_path: a directory that contains libwireshark.
        """

        if cls._init_proc_called:
            return
        env = dict(os.environ)
        env["LD_LIBRARY_PATH"] = ws_library_path + ":" + env.get("LD_LIBRARY_PATH", "")
        cls._proc = subprocess.Popen([executable_path],
                                    bufsize=-1,
                                    stdin=subprocess.PIPE,
                                    stdout=subprocess.PIPE,
                                    env=env
                                    )
        cls._init_proc_called = True


    @classmethod
    def decode_msg(cls, msg_type, b):
        """
        Decode a binary message of type msg_type.

        Args:
            msg_type: a string identifying the type of the message to be 
                decoded
            b: binary data
        """
        assert cls._init_proc_called
        if msg_type not in cls.SUPPORTED_TYPES:
            return None

        input_data = struct.pack(
                                    "!II",  # in network order
                                    cls.SUPPORTED_TYPES[msg_type],
                                    len(b),
                                )
        input_data += b
        
        cls._proc.stdin.write(input_data)
        cls._proc.stdin.flush()
        cls._proc.stdout.flush()
        result = []
        while True:
            line = cls._proc.stdout.readline()
            if line.startswith("===___==="):
                break
            result.append(line)

        return "".join(result)


# Test decoding
if __name__ == "__main__":
    tests = [   ("LTE-RRC_PCCH", "4001BF281AEBA00000"), 
                ("RRC_MIB", "60c428205aa2fe0090c8506e422419822a3653940c40c0"),
                ("RRC_MIB", "10c424c05aa2fe00a0c850448c466608a8e54a80100a0100"),
                ("RRC_SIB1", "c764b108500b1ba01483078a2be62ad0"),
                # ("RRC_SIB5", "c4300b6d830021844a0585760184af40"),
                ("RRC_SIB7", "018000"),
                ("RRC_SIB12", "b38111d024541a42a0"),
                ]
    executable_path = os.path.join(os.path.abspath(os.getcwd()),
                                    "../../../../ws_dissector/ws_dissector")
    WSDissector.init_proc(executable_path, "/home/likayo/wireshark-local-1.12.3/lib")

    for typ, b in tests:
        print WSDissector.decode_msg(typ, binascii.a2b_hex(b))
