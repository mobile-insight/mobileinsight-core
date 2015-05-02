__all__ = ["WSDissector"]

import os
import binascii
import struct
import subprocess

class WSDissector:
    FNULL = open(os.devnull, 'w')
    SUPPORTED_TYPES = { # WCDMA RRC
                        "DL_BCCH_BCH": 100,
                        # LTE RRC
                        "LTE-RRC_PCCH": 200,
                        "LTE-RRC_DL_DCCH": 201,
                        "LTE-RRC_UL_DCCH": 202,
                        "LTE-RRC_BCCH_DL_SCH": 203
                        }
    TYPES_TO_ID = {}
    WCDMA_CHANNEL_TYPE_TO_DISSECTOR = {"DL_BCCH_BCH": "rrc.bcch.bch"}
    EXECUTABLE_PATH = "/home/likayo/Code/test_libwireshark/"
    LD_LIBRARY_PATH = "/home/likayo/wireshark-local-1.12.3/lib"
    proc = None
    init_proc_called = False


    @classmethod
    def init_proc(cls):
        if cls.init_proc_called:
            return
        env = dict(os.environ)
        env["LD_LIBRARY_PATH"] = cls.LD_LIBRARY_PATH + ":" + env.get("LD_LIBRARY_PATH", "")
        cls.proc = subprocess.Popen([os.path.join(cls.EXECUTABLE_PATH, "dissect")],
                                    bufsize=-1,
                                    stdin=subprocess.PIPE,
                                    stdout=subprocess.PIPE,
                                    env=env
                                    )
        cls.init_proc_called = True


    @classmethod
    def _dissector_name(cls, msg_type):
        """
        Example:
            cls._dissector_name("LTE-RRC_PCCH") => "lte-rrc.pcch"
            cls._dissector_name("DL_BCCH_BCH") => "rrc.bcch.bch"
        """
        if msg_type.startswith("LTE-RRC_"):
            return msg_type.lower().replace("_", ".")
        elif msg_type in cls.WCDMA_CHANNEL_TYPE_TO_DISSECTOR:
            return cls.WCDMA_CHANNEL_TYPE_TO_DISSECTOR[msg_type]
        else:
            raise NotImplementedError


    @classmethod
    def decode_msg(cls, msg_type, b):
        if not cls.init_proc_called:
            return None
        if msg_type not in cls.SUPPORTED_TYPES:
            return None
        # dissector_name = cls._dissector_name(msg_type)

        input_data = struct.pack("!II",
                                len(b),
                                cls.SUPPORTED_TYPES[msg_type]) # in network order
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


if __name__ == '__main__':
    b = "4001BF281AEBA00000"
    WSDissector.init_proc()

    for i in range(1):
        print WSDissector.decode_msg("LTE-RRC_PCCH", binascii.a2b_hex(b))
