__all__ = ["TShark"]

import os
import binascii
import struct
import subprocess


class TShark:
    # TEMP_FILE = "temp.pcap"
    FNULL = open(os.devnull, 'w')
    SUPPORTED_TYPES = ["LTE-RRC_PCCH", "LTE-RRC_DL_DCCH", "LTE-RRC_UL_DCCH"]
    initialized = False
    proc = None
    
    @classmethod
    def init_proc(cls):
        if cls.initialized:
            return
        print "Initializing"
        cls.proc = {dissector_name:
                    subprocess.Popen(["tshark",
                                 "-o", 'uat:user_dlts:"User 1 (DLT=148)","%s","0","","0",""' % dissector_name,
                                 "-i", "-",
                                 "-l",  # Flush the stdout after the information for each packet is printed.
                                 "-Q",  # quiet
                                 "-S", "===___===", # separator
                                 "-V",
                                 ],
                                bufsize=-1,
                                stdin=subprocess.PIPE,
                                stdout=subprocess.PIPE,
                                )
                    for dissector_name
                    in [cls._dissector_name(typ) for typ in cls.SUPPORTED_TYPES]}
        for ign, p in cls.proc.items():
            input_data = """d4c3 b2a1 0200 0400 0000 0000 0000 0000
                            ffff 0000 9400 0000"""
            input_data = binascii.a2b_hex("".join(input_data.split()))
            p.stdin.write(input_data)
            p.stdin.flush()
        cls.initialized = True


    @classmethod
    def _dissector_name(cls, msg_type):
        """
        Example:
            cls._dissector_name("LTE-RRC_PCCH") => "lte-rrc.pcch"
        """
        return msg_type.lower().replace("_", ".")


    @classmethod
    def decode_msg(cls, msg_type, b):
        if msg_type not in cls.SUPPORTED_TYPES:
            return "Unsupported message"
        dissector_name = cls._dissector_name(msg_type)

        # proc = subprocess.Popen(["text2pcap",
        #                          # "-n",  # output pcapng format
        #                          "-l", "148",   # customized link layer type
        #                          "-",   # from stdin
        #                          # "-",   # to stdout
        #                          cls.TEMP_FILE,
        #                          ],
        #                         stdin=subprocess.PIPE,
        #                         stdout=cls.FNULL,
        #                         stderr=cls.FNULL)
        # input_data = "000000 " + " ".join([binascii.b2a_hex(c) for c in b]) + " 00"
        # proc.stdin.write(input_data)
        # proc.stdin.close()
        # proc.wait()
        # input_data = open(cls.TEMP_FILE, "rb").read()

        # input_data = """d4c3 b2a1 0200 0400 0000 0000 0000 0000
        #                 ffff 0000 9400 0000"""
        # input_data = binascii.a2b_hex("".join(input_data.split()))
        input_data = struct.pack("<IIII", 0, 0, len(b), len(b))
        input_data += b

        p = cls.proc[dissector_name]
        p.stdin.write(input_data)
        p.stdin.flush()
        p.stdout.flush()
        stdout_val = []
        while True:
            line = p.stdout.readline()
            if line.startswith("===___==="):
                break
            else:
                stdout_val.append(line)

        return "".join(stdout_val)


if __name__ == '__main__':
    b = "4001BF281AEBA00000"
    TShark.init_proc()

    for i in range(10):
        print TShark.decode_msg("LTE-RRC_PCCH", binascii.a2b_hex(b))
