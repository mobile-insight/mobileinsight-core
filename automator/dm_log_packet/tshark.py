__all__ = ["TShark"]

import os
import binascii
import struct
import subprocess


class TShark:
    # TEMP_FILE = "temp.pcap"
    FNULL = open(os.devnull, 'w')
    SUPPORTED_TYPES = [ "DL_BCCH_BCH",
                        "LTE-RRC_PCCH", "LTE-RRC_DL_DCCH", "LTE-RRC_UL_DCCH"]
    WCDMA_CHANNEL_TYPE_TO_DISSECTOR = {"DL_BCCH_BCH": "rrc.bcch.bch"}
    initialized = False
    proc = None
    
    @classmethod
    def init_proc(cls):
        if cls.initialized:
            return
        print "Initializing"
        # cls.proc = {dissector_name:
        #             subprocess.Popen(["tshark",
        #                          "-o", 'uat:user_dlts:"User 1 (DLT=148)","%s","0","","0",""' % dissector_name,
        #                          "-i", "-",
        #                          "-l",  # Flush the stdout after the information for each packet is printed.
        #                          "-Q",  # quiet
        #                          "-S", "===___===", # separator
        #                          "-V",
        #                          ],
        #                         bufsize=-1,
        #                         stdin=subprocess.PIPE,
        #                         stdout=subprocess.PIPE,
        #                         )
        #             for dissector_name
        #             in set([cls._dissector_name(typ) for typ in cls.SUPPORTED_TYPES])}
        # for ign, p in cls.proc.items():
        #     print ign
        #     input_data = """d4c3 b2a1 0200 0400 0000 0000 0000 0000
        #                     ffff 0000 %x00 0000""" % 148
        #     input_data = binascii.a2b_hex("".join(input_data.split()))
        #     p.stdin.write(input_data)
        #     p.stdin.flush()
        print "Complete"
        cls.initialized = True


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
        cls.init_proc()
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

        input_data = """d4c3 b2a1 0200 0400 0000 0000 0000 0000
                        ffff 0000 %x00 0000""" % 148
        input_data = binascii.a2b_hex("".join(input_data.split()))
        input_data += struct.pack("<IIII", 0, 0, len(b) + 1, len(b) + 1)
        input_data += b + "\x00"
        open("temp.pcap", "wb").write(input_data)

        # p = cls.proc[dissector_name]
        # p.stdin.write(input_data)
        # p.stdin.flush()
        # p.stdout.flush()
        # stdout_val = []
        p = subprocess.Popen(["tshark",
                     "-o", 'uat:user_dlts:"User 1 (DLT=148)","%s","0","","0",""' % dissector_name,
                     "-r", "temp.pcap",
                     "-l",  # Flush the stdout after the information for each packet is printed.
                     "-Q",  # quiet
                     # "-S", "===___===", # separator
                     "-V",
                     ],
                    bufsize=-1,
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    )
        stdout_val, ign = p.communicate(None)
        result = []
        start_record = False
        for line in stdout_val.splitlines(1):
            if line.startswith("===___==="):
                break
            elif not start_record and line.startswith("DLT"):
                start_record = True

            if start_record:
                result.append(line)

        return "".join(result)


if __name__ == '__main__':
    b = "4001BF281AEBA00000"
    TShark.init_proc()

    for i in range(10):
        print TShark.decode_msg("LTE-RRC_PCCH", binascii.a2b_hex(b))
