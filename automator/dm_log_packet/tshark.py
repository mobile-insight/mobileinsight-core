__all__ = ["TShark"]

import os
import binascii
import subprocess

class TShark:
    WIRESHARK_PATH = "D:\\Program Files\\Wireshark\\"
    TEMP_FILE = "temp.pcap"
    FNULL = open(os.devnull, 'w')

    @classmethod
    def decode_msg(cls, msg_type, b):
        # TODO: check msg_type and change command line args according to its value
        if msg_type != "PCCH":
            return ""
        input_data = "0 " + " ".join([binascii.b2a_hex(c) for c in b])
        print input_data
        proc = subprocess.Popen([cls.WIRESHARK_PATH + 'text2pcap',
                                 '-l', '148', '-', cls.TEMP_FILE],
                                stdin=subprocess.PIPE,
                                stdout=cls.FNULL,
                                stderr=cls.FNULL)
        stdout_value, stderr_value = proc.communicate(input_data);
        proc = subprocess.Popen([cls.WIRESHARK_PATH + 'tshark',
                                 '-o', 'uat:user_dlts:"User 1 (DLT=148)","lte-rrc.pcch","0","","0",""',
                                 '-r', cls.TEMP_FILE,
                                 '-V'],
                                stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        stdout_value, stderr_value = proc.communicate(input_data);
        # print "stdout_value: " + stdout_value
        return stdout_value

if __name__ == '__main__':
    input_data = "4001BF281AEBA00000"

    TShark.decode_msg("PCCH", binascii.a2b_hex(input_data))
