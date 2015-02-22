# -*- coding: utf-8 -*-
"""
dm_log_packet.py
Define DMLogPacket class.
"""

import struct
import binascii
from datetime import *

from consts import *
from tshark import *

__all__ = ["DMLogPacket"]

# TODO: remoce this code cloning later...
def static_var(varname, value):
    def decorate(func):
        setattr(func, varname, value)
        return func
    return decorate

class DMLogPacket:
    HEADER_FMT = "<HHHQ"
    HEADER_LEN = struct.calcsize(HEADER_FMT)
    DSL_SKIP = None
    LOGITEM_FMT = {
                    # See https://docs.python.org/2/library/struct.html#format-characters
                    # default byte order: little endian
                    LOG_PACKET_ID["WCDMA_CELL_ID"]:
                    [
                        ("UTRA UL Absolute RF channel number", "I"),
                        ("UTRA DL Absolute RF channel number", "I"),
                        ("Cell identity (28-bits)", "I"),
                        ("URA to use in case of overlapping URAs", "B"),
                        (DSL_SKIP, 2),      # Unknown yet
                        ("Allowed Call Access", "B"),
                        ("PSC", "H"),
                        ("PLMN", "6B"),
                        ("LAC id", "I"),
                        ("RAC id", "I"),
                    ],
                    LOG_PACKET_ID["LTE_RRC_OTA_Packet"]:
                    [
                        ("Pkt Version", "B"),
                        ("RRC Release Number", "B"),
                        ("Major/minor", "B"),
                        ("Radio Bearer ID", "B"),
                        ("Physical Cell ID", "H"),
                        ("Freq", "H"),
                        ("SysFrameNum/SubFrameNum", "H"),
                        ("PDU Number", "B"),
                        # (DSL_SKIP, 4),      # Unknown yet, only for Pkt Version = 7
                        ("Msg Length", "B"),
                        ("SIB Mask in SI", "B"),
                    ],
                    }

    @classmethod
    def decode(cls, b):
        l, type_id, ts = cls._decode_header(b)
        log_item = cls._decode_log_item(type_id, b[cls.HEADER_LEN:])
        return (l, type_id, ts, log_item)

    @classmethod
    def _decode_header(cls, b):
        l1, l2, type_id, ts = struct.unpack_from(cls.HEADER_FMT, b)
        assert l1 == l2
        assert l1 + 2 == len(b)
        if type_id not in cls.LOGITEM_FMT:
            raise Exception("hahaha")
        ts = cls._decode_ts(ts)
        return l1, type_id, ts

    @classmethod
    @static_var("epoch", datetime(1980, 1, 6, 0, 0, 0))  # 1980 Jan 6 00:00:00
    @static_var("per_sec", 52428800)
    def _decode_ts(cls, ts):
        secs = float(ts) / cls._decode_ts.per_sec
        return cls._decode_ts.epoch + timedelta(seconds=secs)

    @classmethod
    def _decode_log_item(cls, type_id, b):
        ind = 0
        res = []
        for spec in cls.LOGITEM_FMT[type_id]:
            if spec[0] != cls.DSL_SKIP:
                name = spec[0]
                fmt = spec[1]
                # print ind, binascii.b2a_hex(b[ind:ind+4])
                decoded = struct.unpack_from(fmt, b, ind)
                if len(decoded) == 1:
                    res.append((name, decoded[0]))
                else:
                    res.append((name, decoded))
                ind += struct.calcsize(fmt)
            else:   # padding/skip
                ind += spec[1]

        if type_id == LOG_PACKET_ID["LTE_RRC_OTA_Packet"]:
            for name, decoded in res:
                if name == "PDU Number":
                    pdu_number = decoded
                elif name == "Msg Length":
                    pdu_length = decoded
            msg = b[ind:(ind + pdu_length)]
            print binascii.b2a_hex(msg)
            if pdu_number in LTE_RRC_OTA_PDU_TYPE:
                decoded = cls._decode_msg(LTE_RRC_OTA_PDU_TYPE[pdu_number], msg)
                res.append(("Msg", decoded))
                print decoded
            else:
                print "Unknown PDU Type"
        return res

    @classmethod
    def _decode_msg(cls, msg_type, b):
        return TShark.decode_msg(msg_type, b)


if __name__ == '__main__':
    tests = [
            # WCDMA_CELL_ID
            "2c002c002741b4008aef9740ce0040100000211100000f5660030000070030070301000401002cd90000ba000000",
            "2c002c0027419203b0a48540ce00c224000052260000fda3fa02d0000700801f0301000401003ffd00003c000000",
            # LTE_RRC_OTA_Packet v7 LTE-RRC_PCCH
            "26002600C0B00000A3894A13CE00070A7100D801B70799390400000000090040012F05EC4E700000",
            # LTE_RRC_OTA_Packet v2 LTE-RRC_PCCH
            "22002200c0b00000c3ad2176ce000209a000d801b707390404090040019d056f48800000",
            # LTE_RRC_OTA_Packet v2 LTE-RRC_DL_DCCH
            "2D002D00C0B00000C6A4212BCD000209A001D200E308000006140022020516EE933E90E02F67000299422868020880",
            # LTE_RRC_OTA_Packet v2 LTE-RRC_UL_DCCH
            "1B001B00C0B00000C8A4212BCD000209A001D200E30800000802001200"
            ]

    for b in tests:
        l, type_id, ts, log_item = DMLogPacket.decode(binascii.a2b_hex(b))
        print l, hex(type_id), ts
        print log_item
