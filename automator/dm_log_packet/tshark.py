__all__ = ["TShark"]

import os
import binascii
import subprocess


class TShark:
    TEMP_FILE = "temp.pcapng"
    FNULL = open(os.devnull, 'w')
    SUPPORTED_TYPES = ["LTE-RRC_PCCH", "LTE-RRC_DL_DCCH", "LTE-RRC_UL_DCCH"]

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
        input_data = "000000 " + " ".join([binascii.b2a_hex(c) for c in b]) + " 00"
        # print input_data
        dissector_name = cls._dissector_name(msg_type)
        proc = subprocess.Popen(["text2pcap",
                                 "-n",  # output pcapng format
                                 "-l", "148",   # customized link layer type
                                 "-",   # from stdin
                                 # "-",   # to stdout
                                 cls.TEMP_FILE,
                                 ],
                                stdin=subprocess.PIPE,
                                stdout=subprocess.PIPE,
                                stderr=cls.FNULL)
        proc.stdin.write(input_data)
        proc.stdin.close()
        input_data = open(cls.TEMP_FILE, "rb").read()
        # print repr(input_data)
        proc2 = subprocess.Popen(['tshark',
                                 '-o', 'uat:user_dlts:"User 1 (DLT=148)","%s","0","","0",""' % dissector_name,
                                 '-r', "-",
                                 '-V',
                                 ],
                                stdin=subprocess.PIPE,
                                stdout=subprocess.PIPE,
                                )
        stdout_value, stderr_value = proc2.communicate(input_data);
        os.remove(cls.TEMP_FILE)
        return stdout_value


if __name__ == '__main__':
    input_data = "4001BF281AEBA00000"

    print TShark.decode_msg("LTE-RRC_PCCH", binascii.a2b_hex(input_data))
