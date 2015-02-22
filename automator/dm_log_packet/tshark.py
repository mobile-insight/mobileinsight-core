__all__ = ["TShark"]

import os
import binascii
import subprocess


class TShark:
    WIRESHARK_PATH = "D:\\Program Files\\Wireshark\\"
    TEMP_FILE = "temp.pcap"
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
            return ""
        input_data = "0 " + " ".join([binascii.b2a_hex(c) for c in b]) + " 00"
        # print input_data
        dissector_name = cls._dissector_name(msg_type)
        proc = subprocess.Popen([cls.WIRESHARK_PATH + 'text2pcap',
                                 '-l', '148', '-', cls.TEMP_FILE],
                                stdin=subprocess.PIPE,
                                stdout=cls.FNULL,
                                stderr=cls.FNULL)
        stdout_value, stderr_value = proc.communicate(input_data);
        proc = subprocess.Popen([cls.WIRESHARK_PATH + 'tshark',
                                 '-o', 'uat:user_dlts:"User 1 (DLT=148)","%s","0","","0",""' % dissector_name,
                                 '-r', cls.TEMP_FILE,
                                 '-V',
                                 ],
                                stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        stdout_value, stderr_value = proc.communicate(input_data);
        os.remove(cls.TEMP_FILE)
        return stdout_value


if __name__ == '__main__':
    input_data = "4001BF281AEBA00000"

    TShark.decode_msg("PCCH", binascii.a2b_hex(input_data))
