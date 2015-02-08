import struct
import binascii
from datetime import *

from consts import *

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
    LOGITEM_FMT = {
                    LOG_PACKET_ID["WCDMA_CELL_ID"]:
                    [
                        ("UTRA UL Absolute RF channel number", "I"),
                        ("UTRA DL Absolute RF channel number", "I"),
                        ("Cell identity (28-bits)", "I"),
                        ("URA to use in case of overlapping URAs", "B"),
                        (None, 3),      # Unknown yet
                        ("PSC", "H"),
                        ("PLMN", "6B"),
                        ("LAC id", "I"),
                        ("RAC id", "I"),
                    ],
                    }

    @classmethod
    def decode(cls, b):
        cls._decode_header(b)

    @classmethod
    def _decode_header(cls, b):
        l1, l2, type_id, ts = struct.unpack_from(cls.HEADER_FMT, b)
        assert l1 == l2
        assert l1 + 2 == len(b)
        print l1, hex(type_id), cls._decode_ts(ts)
        if type_id not in cls.LOGITEM_FMT:
            raise Exception("hahaha")
        log_item = cls._decode_log_item(type_id, b[cls.HEADER_LEN:])
        print log_item

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
            if spec[0]:
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
        return res

if __name__ == '__main__':
    tests = ["2c002c002741b4008aef9740ce0040100000211100000f5660030000070030070301000401002cd90000ba000000",
            "2c002c0027419203b0a48540ce00c224000052260000fda3fa02d0000700801f0301000401003ffd00003c000000"]

    for b in tests:
        DMLogPacket.decode(binascii.a2b_hex(b))
