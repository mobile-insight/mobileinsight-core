# -*- coding: utf-8 -*-
"""
ws_dissector.py
A wrapper class of the ws_dissector program.

Author: Jiayao Li
"""

__all__ = ["WSDissector"]

import os
import binascii
import platform
import struct
import subprocess
import sys


class WSDissector:
    """
    A wrapper class of the ws_dissector program, which calls libwireshark
    to dissect many types of messages, e.g. 3GPP standardized messages.

    This wrapper communicates with the ws_dissector program using a
    trivial TLV-formatted protocol named AWW (Automator Wireshark Wrapper),
    through the standard input/output interfaces.
    """

    # Maps all supported message types to their AWW protocol number.
    # Keep consistent with ws_dissector/packet-aww.cpp
    """Test comment for SUPPORTED_TYPES"""
    SUPPORTED_TYPES = {
        # WCDMA: 100-199
        # - WCDMA RRC: 100-149
        "RRC_UL_CCCH": 100,
        "RRC_UL_DCCH": 101,
        "RRC_DL_CCCH": 102,
        "RRC_DL_DCCH": 103,
        "RRC_DL_BCCH_BCH": 104,
        "RRC_DL_PCCH": 106,
        # - WCDMA RRC SysInfo: 150-199
        "RRC_MIB": 150,
        "RRC_SIB1": 151,
        "RRC_SIB2": 152,
        "RRC_SIB3": 153,
        "RRC_SIB5": 155,
        "RRC_SIB7": 157,
        "RRC_SIB11": 161,
        "RRC_SIB12": 162,
        "RRC_SIB19": 169,
        # - WCDMA RRC SchedulingBlock
        "RRC_SB1": 181,
        # - NAS
        "NAS": 190,
        # LTE: 200-299
        "LTE-RRC_PCCH": 200,
        "LTE-RRC_DL_DCCH": 201,
        "LTE-RRC_UL_DCCH": 202,
        "LTE-RRC_BCCH_DL_SCH": 203,
        "LTE-RRC_DL_CCCH": 204,
        "LTE-RRC_UL_CCCH": 205,
        "LTE-RRC_DL_DCCH_NB": 206,
        "LTE-RRC_UL_DCCH_NB": 207,
        "LTE-RRC_BCCH_DL_SCH_NB": 208,
        "LTE-RRC_DL_CCCH_NB": 209,
        "LTE-RRC_UL_CCCH_NB": 210,
        "LTE-NAS_EPS_PLAIN": 250,
        "LTE-PDCP_DL_SRB": 300,
        "LTE-PDCP_UL_SRB": 301,

        # 5GNR: 400-499
        "nr-rrc.ue_radio_paging_info": 400,
        "nr-rrc.ue_radio_access_cap_info": 401,
        "nr-rrc.bcch.bch": 402,
        "nr-rrc.bcch.dl.sch": 403,
        "nr-rrc.dl.ccch": 404,
        "nr-rrc.dl.dcch": 405,
        "nr-rrc.pcch": 406,
        "nr-rrc.ul.ccch": 407,
        "nr-rrc.ul.ccch1": 408,
        "nr-rrc.ul.dcch": 409,
        "nr-rrc.rrc_reconf": 410,
        "nr-rrc.ue_mrdc_cap": 411,
        "nr-rrc.ue_nr_cap": 412,
        "nr-rrc.sbcch.sl.bch": 413,
        "nr-rrc.scch": 414,
        "nr-rrc.radio_bearer_conf": 415,
        "nas-5gs":416,
    }
    _proc = None
    _init_proc_called = False

    @classmethod
    def init_proc(cls, executable_path, ws_library_path):
        """
        Launch the ws_dissector program. Must be called before any actual
        decoding, and should be called only once.

        :param executable_path: path to ws_dissector. If set to None, uses the default path.
        :type executable_path: string or None

        :param ws_library_path: a directory that contains libwireshark. If set to None, uses the default path.
        :type ws_library_path: string or None
        """

        if cls._init_proc_called:
            return
        if executable_path:
            real_executable_path = executable_path
        else:
            if platform.system() == "Windows":
                real_executable_path = sys.exec_prefix + "/mobile_insight/ws_dissector/ws_dissector.exe"
            else:   # Linux or macOS
                real_executable_path = "/usr/local/bin/ws_dissector"

        env = dict(os.environ)
        if platform.system() == "Windows":
            if ws_library_path:
                env["PATH"] = ws_library_path + ";" + env.get("PATH", "")
        else:
            if ws_library_path:
                env["LD_LIBRARY_PATH"] = ws_library_path + \
                    ":" + env.get("LD_LIBRARY_PATH", "")
        cls._proc = subprocess.Popen([real_executable_path],
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

        :param msg_type: the type of the message to be decoded
        :type msg_type: string

        :param b: binary data to be decoded
        :type b: string

        :returns: an XML string
        """
        assert cls._init_proc_called
        if msg_type not in cls.SUPPORTED_TYPES:
            print(("MI(Unknown) Unsupported message for ws_dissector:", msg_type))
            return None
        if len(b) > 3000:
            print(("MI(Ignore) Length of message is too large for ws_dissector:", len(b), "bytes"))
            return None

        input_data = struct.pack(
            "!II",  # in network order
            cls.SUPPORTED_TYPES[msg_type],
            len(b),
        )
        input_data += b

        cls._proc.stdin.write(input_data)
        cls._proc.stdin.flush()
        result = []
        while True:
            line = cls._proc.stdout.readline().decode("utf-8")
            if line.startswith("===___==="):
                break
            result.append(line)

        return "".join(result)


# Test decoding
if __name__ == "__main__":
    tests = [
            # ("LTE-RRC_PCCH", "4001BF281AEBA00000"),
            ("LTE-RRC_UL_DCCH", "3C0308277DDB80506092A84C853870D1BF87F1BF87F1BF87F1BF87F1BF87F1BF87F1BF87EFD9FF53C10329D2A0A8DD23D00000000010254EF800000016A200600600600441086006084300304218018210C1086084100241086084304218018210C1086084304218210080180180141086006084304210B7E2A31A0280308001040E50004103A0001040E6000410384001000E700040031C001001C200041420820610002482D010708001040E50004907A000105A02081D0000920F400020B304105A020F30002082D01071C001000E600049079C001019C2001CE000900B4049C20004107A000125A02082D0127A0001040E80004D07A000125982082D012798001041E800041660920B4041E60004905A024E38002003CE00090CE100167091CE000800738002601CA020920740082481CC020920738082403D002092D01040E80104D07A004125982082D010798041241E7010486708007380826288DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F8DFC3F6CE80000F6206CE8000023E74000028011743A1E3F21D0F0E0021D1FA1C10EFE1014B00F1979A040148DFFFE694BBC500E80008026060500924A100100025828802401200900480240120090140802404120090140802405020209010480240502009010480A04041200901408082405020209010480A040412009002070020700207002070120610020701206101020701206100206209534010A04E000080041042021133035758A6611404E291010F1A1E50000031F1AF3632B259642479C0036006EFA319091E7000D801A3E2C642479C00360040"),
            # ("RRC_MIB", "60c428205aa2fe0090c8506e422419822a3653940c40c0"),
            # ("RRC_MIB", "10c424c05aa2fe00a0c850448c466608a8e54a80100a0100"),
            # ("RRC_SIB1", "c764b108500b1ba01483078a2be62ad0"),
            # ("RRC_SIB3", "0d801f4544fc60005001000011094e"),
            # ("RRC_SIB5", "63403AFFFF03FFFC5010F0290C0A8018000C8BF5B15EA0000003F5210E30000247894201400010440060222E56300C60202C000C14CC003C4300B6D830021844A0585760186AF400"),
            # ("RRC_SIB7", "018000"),
            # ("RRC_SIB12", "b38111d024541a42a0"),
            # ("RRC_SIB19", "41a1001694e49470"),
            ]

    executable_path = None
    WSDissector.init_proc(executable_path, "/usr/local/lib")

    for typ, b in tests:
        print((WSDissector.decode_msg(typ, binascii.a2b_hex(b))))
