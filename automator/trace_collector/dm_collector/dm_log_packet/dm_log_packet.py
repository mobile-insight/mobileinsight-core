# -*- coding: utf-8 -*-
"""
dm_log_packet.py
Define DMLogPacket class.

Author: Jiayao Li
"""

__all__ = ["DMLogPacket", "FormatError"]

import binascii
from datetime import *
import os
import struct
import xml.etree.ElementTree as ET

try:
    from utils import *
except ImportError, e:
    # TODO: wtf can I do to remove this dependence ..?
    def static_var(varname, value):
        def decorate(func):
            setattr(func, varname, value)
            return func
        return decorate

from consts import *
from ws_dissector import *


class FormatError(RuntimeError):
    """
    Error in decoding messages.
    """
    pass


class DMLogPacket:
    """
    QCDM log packet decoder.

    A log packet contains a header that specifies the packet type and 
    timestamp, and a payload field that store useful information of a 
    phone. This class will decode both the header and payload fields.

    This class depends on Wireshark to decode some 3GPP standardized
    messages.

    This class should not be instancialized.
    """
    # Format strings have the same meanings as in Python struct module. See 
    # https://docs.python.org/2/library/struct.html for details.
    _HEADER_FMT = "<HHHQ"    # little endian
    _HEADER_LEN = struct.calcsize(_HEADER_FMT)
    _DSL_SKIP = None

    # A dict that maps the type ids to the format of the payload field.
    # The format of a format spec:
    #   (name, field_type[, elem_type])
    #       name: a string or _DSL_SKIP
    #       field_type: 
    #           If "name" is a string, this is a format string defined
    #           by the struct module; otherwise, it is a number that
    #           specify how many bytes should be skipped
    #       elem_type:
    #           can be "rsrp" or "rsrq"
    _LOGITEM_FMT = {
                    # default byte order: little endian
                    LOG_PACKET_ID["WCDMA_CELL_ID"]:
                    (
                        ("UTRA UL Absolute RF channel number", "I"),
                        ("UTRA DL Absolute RF channel number", "I"),
                        ("Cell identity (28-bits)", "I"),
                        ("URA to use in case of overlapping URAs", "B"),
                        (_DSL_SKIP, 2),      # Unknown yet
                        ("Allowed Call Access", "B"),
                        ("PSC", "H"),
                        ("PLMN", "6B"),
                        ("LAC id", "I"),
                        ("RAC id", "I"),
                    ),
                    LOG_PACKET_ID["WCDMA_Signaling_Messages"]:
                    (
                        ("Channel Type", "B"),
                        ("Radio Bearer ID", "B"),
                        ("Message Length", "H"),
                    ),

                    LOG_PACKET_ID["LTE_RRC_OTA_Packet"]:
                    (
                        ("Pkt Version", "B"),
                        ("RRC Release Number", "B"),
                        ("Major/minor", "B"),
                        ("Radio Bearer ID", "B"),
                        ("Physical Cell ID", "H"),
                        ("Freq", "H"),
                        ("SysFrameNum/SubFrameNum", "H"),
                        ("PDU Number", "B"),
                        # continued
                    ),
                    LOG_PACKET_ID["LTE_NAS_ESM_Plain_OTA_Incoming_Message"]:
                    (
                        ("Pkt Version", "B"),
                        # continued in _LTE_NAS_PLAIN_FMT
                    ),
                    LOG_PACKET_ID["LTE_NAS_ESM_Plain_OTA_Outgoing_Message"]:
                    (
                        ("Pkt Version", "B"),
                        # continued in _LTE_NAS_PLAIN_FMT
                    ),
                    LOG_PACKET_ID["LTE_NAS_EMM_Plain_OTA_Incoming_Message"]:
                    (
                        ("Pkt Version", "B"),
                        # continued in _LTE_NAS_PLAIN_FMT
                    ),
                    LOG_PACKET_ID["LTE_NAS_EMM_Plain_OTA_Outgoing_Message"]:
                    (
                        ("Pkt Version", "B"),
                        # continued in _LTE_NAS_PLAIN_FMT
                    ),
                    LOG_PACKET_ID["LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results"]:
                    (
                        ("Version", "B"),
                        (_DSL_SKIP, 7),     # Unknown
                        # continued in _LTE_ML1_CMIFMR_FMT
                    ),
                    LOG_PACKET_ID["LTE_ML1_IRAT_Measurement_Request"]:
                    (),
                    LOG_PACKET_ID["LTE_ML1_Serving_Cell_Measurement_Result"]:
                    (  
                        ("Version", "B"),
                        ("Number of SubPackets", "B"),
                        (_DSL_SKIP, 2),      # Unknown
                        # continued in _LTE_ML1_SUBPKT_FMT
                    ),
                    LOG_PACKET_ID["LTE_ML1_Connected_Mode_Neighbor_Meas_Req/Resp"]:
                    (),
                    LOG_PACKET_ID["LTE_RRC_Serv_Cell_Info_Log_Packet"]:
                    (
                        ("Version", "B"),
                        ("Physical Cell ID", "H"),
                        ("DL FREQ","H"),
                        ("UL FREQ","H"),
                        ("DL BW", "B"),
                        ("UL BW", "B"),
                        ("Cell Identity", "I"),
                        ("TAC", "H"),
                        ("Band Indicator", "I"),
                        ("MCC", "H"),
                        ("MCC Digit", "B"),
                        ("MNC", "H"),
                        ("Allowed Access", "B"),
                    ),
                    LOG_PACKET_ID["LTE_RRC_MIB_Message_Log_Packet"]:
                    (
                        ("Version", "B"),
                        ("Physical Cell ID", "H"),
                        ("Freq", "H"),
                        ("SFN", "H"),
                        ("Number of Antenna", "B"),
                        ("DL BW", "B"),
                    ),
                }

    # For LTE_NAS_EMM_Plain_OTA_Incoming_Message and LTE_NAS_EMM_Plain_OTA_Outgoing_Message
    _LTE_NAS_PLAIN_FMT = {
                    1:  # Version 1
                    (
                        ("RRC Release Number", "B"),
                        ("Major Version", "B"),
                        ("Minor Version", "B"),
                        # followed by NAS messages
                    ),
                }

    _LTE_ML1_SUBPKT_FMT = {
                    "Header":
                    (
                        ("SubPacket ID", "B"),
                        ("Version", "B"),
                        ("SubPacket Size", "H"),
                    ),
                    LTE_ML1_SUBPACKET_ID["Serving_Cell_Measurement_Result"]:
                    {
                        4: # Version 4
                        (
                            ("E-ARFCN", "H"),
                            ("Physical Cell ID", "H"),
                            # TODO: extra effort to deal with unaligned fields
                        ),
                    },
                }

    # LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results
    _LTE_ML1_CMIFMR_FMT = {
                    3:  # Version 3
                    {
                        "Header":
                        (
                            ("E-ARFCN", "H"),
                            ("Serving Physical Cell ID", "H"),
                            ("Sub-frame Number", "H"),
                            ("Serving Filtered RSRP(dBm)", "H", "rsrp"),
                            (_DSL_SKIP, 2),     # Duplicated
                            ("Serving Filtered RSRQ(dB)", "H", "rsrq"),
                            (_DSL_SKIP, 2),     # Duplicated
                            ("Number of Neighbor Cells", "B"),
                            ("Number of Detected Cells", "B"),
                        ),
                        "Neighbor Cell":
                        (
                            ("Physical Cell ID", "H"),
                            ("Filtered RSRP(dBm)", "H", "rsrp"),
                            (_DSL_SKIP, 2),     # Duplicated
                            ("Filtered RSRQ(dB)", "H", "rsrq"),
                            (_DSL_SKIP, 4),     # Duplicated & reserved
                        ),
                        "Detected Cell":
                        (
                            ("Physical Cell ID", "I"),
                            ("SSS Corr Value", "I"),
                            ("Reference Time", "Q"),
                        ),
                    }
                }

    _init_called = False

    @classmethod
    def init(cls, prefs):
        """
        Configure the DMLogPacket class with user preferences.

        This method should be called before any actual decoding.

        Args:
            prefs: a dict storing the preferences.
        """
        if cls._init_called:
            return
        WSDissector.init_proc(prefs["ws_dissect_executable_path"],
                                prefs["libwireshark_path"])
        cls._init_called = True

    @classmethod
    def decode(cls, b):
        """
        Decode a QCDM log packet.

        Args:
            b: a string containing binary data of a packet

        Returns:
            a four-element tuple (len, type_id, ts, log_item). The meaning 
            of each element is as follows:

            len: the number of bytes in the payload field.
            type_id: an integer identifying the type.
            ts: a datetime object storing the timestamp of this packet,
                which is provided by the packet itself.
            log_item: the decoding result of the payload.

        Raises:
            FormatError: the type of this message is unknown
        """
        assert cls._init_called

        l, type_id, ts = cls._decode_header(b)
        log_item = cls._decode_log_item(type_id, b[cls._HEADER_LEN:])
        return (l - cls._HEADER_LEN + 2, type_id, ts, log_item)

    @classmethod
    def _decode_header(cls, b):
        """
        Decode the 14-byte header of the packet.
        """
        l1, l2, type_id, ts = struct.unpack_from(cls._HEADER_FMT, b)
        assert l1 == l2
        assert l1 + 2 == len(b)
        if type_id not in cls._LOGITEM_FMT:
            raise FormatError("Unknown Type ID: 0x%x" % type_id)
        ts = cls._decode_ts(ts)
        return l1, type_id, ts

    @classmethod
    @static_var("epoch", datetime(1980, 1, 6, 0, 0, 0))  # 1980 Jan 6 00:00:00
    @static_var("per_sec", 52428800)
    def _decode_ts(cls, ts):
        """
        Decode the 8-byte packet timestamp in the header.

        Returns:
            a datetime object storing the interpreted timestamp.
        """
        secs = float(ts) / cls._decode_ts.per_sec
        return cls._decode_ts.epoch + timedelta(seconds=secs)

    @classmethod
    def _search_result(cls, result, target):
        if isinstance(target, str):
            target = [target]
        assert isinstance(target, (list, tuple))
        assert len(target) > 0

        ret = [None for i in range(len(target))]
        for name, decoded in result:
            i = 0
            for t in target:
                if name == t:
                    ret[i] = decoded
                i += 1

        assert all([x is not None for x in ret])
        if len(target) == 1:
            return ret[0]
        else:
            return tuple(ret)

    @classmethod
    def _decode_log_item(cls, type_id, b):
        """
        Decode the payload field according to its type.
        """
        res, offset = cls._decode_by_format(cls._LOGITEM_FMT[type_id], b, 0)
        ind = 0 + offset

        if type_id == LOG_PACKET_ID["WCDMA_Signaling_Messages"]:
            offset = cls._decode_wcdma_signaling_messages(b, ind, res)
            ind += offset

        elif type_id == LOG_PACKET_ID["LTE_RRC_OTA_Packet"]:
            pkt_ver = cls._search_result(res, "Pkt Version")
            if pkt_ver not in (2, 7):
                print "Unknown LTE RRC OTA packet version: %d" % pkt_ver
                fmt = None
            elif pkt_ver == 7:
                fmt = [ (cls._DSL_SKIP, 4),      # Unknown yet, only for Pkt Version = 7
                        ("Msg Length", "B"),
                        ("SIB Mask in SI", "B"),
                        ]
            elif pkt_ver == 2:
                fmt = [ ("Msg Length", "B"),
                        ("SIB Mask in SI", "B"),
                        ]
            if fmt:
                res2, offset = cls._decode_by_format(fmt, b, ind)
                res.extend(res2)
                ind = ind + offset
                pdu_number, pdu_length = cls._search_result(
                                                res,
                                                ("PDU Number", "Msg Length",))
                msg = b[ind:(ind + pdu_length)]
                if pdu_number in LTE_RRC_OTA_PDU_TYPE:
                    decoded = cls._decode_msg(LTE_RRC_OTA_PDU_TYPE[pdu_number], msg)
                    res.append( ("Msg", cls._wrap_decoded_xml(decoded)) )
                    # print decoded
                else:
                    print "Unknown LTE RRC PDU Type: 0x%x" % pdu_number

        elif type_id in (   LOG_PACKET_ID["LTE_NAS_ESM_Plain_OTA_Incoming_Message"],
                            LOG_PACKET_ID["LTE_NAS_ESM_Plain_OTA_Outgoing_Message"],
                            LOG_PACKET_ID["LTE_NAS_EMM_Plain_OTA_Incoming_Message"],
                            LOG_PACKET_ID["LTE_NAS_EMM_Plain_OTA_Outgoing_Message"],
                            ):
            offset = cls._decode_lte_nas_plain(b, ind, res)
            ind += offset

        elif type_id == LOG_PACKET_ID["LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results"]:
            pkt_ver = cls._search_result(res, "Version")
            if pkt_ver not in cls._LTE_ML1_CMIFMR_FMT:
                print "Unknown LTE ML1 CMIFMR version: 0x%x" % pkt_ver
            else:
                # Decode header
                fmt = cls._LTE_ML1_CMIFMR_FMT[pkt_ver]["Header"]
                res2, offset = cls._decode_by_format(fmt, b, ind)
                ind += offset
                n_neighbor_cells, n_detected_cells = cls._search_result( 
                                                res2,
                                                ("Number of Neighbor Cells",
                                                 "Number of Detected Cells",))
                # Decode each line of neighbor cells
                res_allcell = []
                fmt = cls._LTE_ML1_CMIFMR_FMT[pkt_ver]["Neighbor Cell"]
                for i in range(n_neighbor_cells):
                    res_cell, offset = cls._decode_by_format(fmt, b, ind)
                    ind += offset
                    res_allcell.append(tuple(res_cell))
                res2.append(("Neighbor Cells", tuple(res_allcell)))
                # Decode each line of detected cells
                res_allcell = []
                fmt = cls._LTE_ML1_CMIFMR_FMT[pkt_ver]["Detected Cell"]
                for i in range(n_detected_cells):
                    res_cell, offset = cls._decode_by_format(fmt, b, ind)
                    ind += offset
                    res_allcell.append(tuple(res_cell))
                res2.append(("Detected Cells", tuple(res_allcell)))

                res.extend(tuple(res2))

        elif type_id == LOG_PACKET_ID["LTE_ML1_Serving_Cell_Measurement_Result"]:
            n_subpkt = cls._search_result(res, "Number of SubPackets")
            res_allpkt = []
            for i in range(n_subpkt):
                # Decode subpacket header
                fmt = cls._LTE_ML1_SUBPKT_FMT["Header"]
                res_pkt, offset = cls._decode_by_format(fmt, b, ind)
                ind += offset
                # Decode payload
                subpkt_id, subpkt_ver = cls._search_result( 
                                                res_pkt,
                                                ("SubPacket ID", "Version"))
                if subpkt_id not in cls._LTE_ML1_SUBPKT_FMT:
                    print "Unknown LTE ML1 Subpacket ID: 0x%x" % subpkt_id
                elif subpkt_ver not in cls._LTE_ML1_SUBPKT_FMT[subpkt_id]:
                    print "Unknown LTE ML1 Subpacket version: 0x%x - %d" % (subpkt_id, subpkt_ver)
                else:
                    fmt = cls._LTE_ML1_SUBPKT_FMT[subpkt_id][subpkt_ver]
                    res2, offset = cls._decode_by_format(fmt, b, ind)
                    res_pkt.extend(res2)
                    
                res_allpkt.append(tuple(res_pkt))
            res.append(("Subpackets", tuple(res_allpkt)))

        return res

    @classmethod
    def _decode_by_format(cls, fmt, b, start):
        """
        Decode binary messsage according to a specific format.

        Args:
            fmt: the specific format
            b: a string containing binary data to be decoded
            start: the position where should we start the decoding.

        Returns:
            a 2-element tuple (res, n).

            res: the decoding result (a list)
            n: how many bytes are consumed in the decoding
        """
        ind = start
        result = []
        for spec in fmt:
            if spec[0] != cls._DSL_SKIP:
                name = spec[0]
                fmt = spec[1]
                # print ind, binascii.b2a_hex(b[ind:ind+4])
                decoded = struct.unpack_from(fmt, b, ind)
                if len(decoded) == 1:
                    decoded = decoded[0]
                    if len(spec) > 2:   # the type of this element is specified
                        elem_type = spec[2]
                        decoded = cls._transform_element(elem_type, decoded)
                result.append( (name, decoded) )
                ind += struct.calcsize(fmt)
            else:   # padding/skip
                ind += spec[1]
        return result, (ind - start)

    @classmethod
    def _transform_element(cls, elem_type, val):
        if elem_type == "rsrp":
            # (0.0625 * x - 180) dBm
            return val * 0.0625 - 180
        elif elem_type == "rsrq":
            # (0.0625 * x - 30) dB
            return val * 0.0625 - 30
        else:
            raise NotImplementedError

    @classmethod
    def _decode_msg(cls, msg_type, b):
        """
        Decode standard message using WSDissector.
        """
        s = WSDissector.decode_msg(msg_type, b)
        return s

    @classmethod
    def _wrap_decoded_xml(cls, xmls):
        """
        Return an XML string that looks like:

        \"\"\"<msg>
            <packet>...</packet>
            <packet>...</packet>
            <packet>...</packet>
        </msg>\"\"\"
        """
        if isinstance(xmls, str):
            xmls = [xmls]
        assert isinstance(xmls, (list, tuple))
        return "<msg>\n" + "".join(xmls) + "</msg>\n"

    @classmethod
    # Keep strings consistent with ws_dissector.py
    @static_var("sib_types", {  0: "RRC_MIB",
                                1: "RRC_SIB1",
                                3: "RRC_SIB3",
                                7: "RRC_SIB7",
                                12: "RRC_SIB12",
                                31: "RRC_SIB19",
                                })
    def _decode_wcdma_signaling_messages(cls, b, start, result):
        """
        Return the number of consumed bytes.
        """
        sib_types = cls._decode_wcdma_signaling_messages.sib_types
        ch_num, pdu_length = cls._search_result(
                                            result, 
                                            ("Channel Type", "Message Length"))
        if ch_num not in WCDMA_SIGNALLING_MSG_CHANNEL_TYPE:
            print "Unknown WCDMA Signalling Messages Channel Type: 0x%x" % ch_num
            return 0

        msg = b[start:(start + pdu_length)]
        xmls = []
        decoded = cls._decode_msg(WCDMA_SIGNALLING_MSG_CHANNEL_TYPE[ch_num], msg)
        xmls.append(decoded)
        # print decoded
        if WCDMA_SIGNALLING_MSG_CHANNEL_TYPE[ch_num] == "RRC_DL_BCCH_BCH":
            xml = ET.fromstring(decoded)
            sibs = xml.findall(".//field[@name='rrc.CompleteSIBshort_element']")
            if sibs:
                # deal with a list of complete SIBs
                for complete_sib in sibs:
                    field = complete_sib.find("field[@name='rrc.sib_Type']")
                    sib_id = int(field.get("show"))
                    field = complete_sib.find("field[@name='rrc.sib_Data_variable']")
                    sib_msg = binascii.a2b_hex(field.get("value"))
                    if sib_id in sib_types:
                        decoded = cls._decode_msg(sib_types[sib_id], sib_msg)
                        xmls.append(decoded)
                        print sib_types[sib_id]
                    else:
                        print "Unknown RRC SIB Type: %d" % sib_id
            else:
                # deal with a segmented SIB
                sib_segment = xml.find(".//field[@name='rrc.firstSegment_element']")
                if sib_segment is None:
                    sib_segment = xml.find(".//field[@name='rrc.subsequentSegment_element']")
                if sib_segment is None:
                    sib_segment = xml.find(".//field[@name='rrc.lastSegmentShort_element']")
                if sib_segment is not None:
                    field = sib_segment.find("field[@name='rrc.sib_Type']")
                    sib_id = int(field.get("show"))
                    print "RRC SIB Segment(type: %d) not handled" % sib_id
                    # if sib_segment.get("name") != "rrc.lastSegmentShort_element":
                    #     field = sib_segment.find("field[@name='rrc.sib_Data_fixed']")
                    # else:
                    #     field = sib_segment.find("field[@name='rrc.sib_Data_variable']")
                    # sib_msg = binascii.a2b_hex(field.get("value"))
        result.append( ("Msg", cls._wrap_decoded_xml(xmls)) )
        return pdu_length

    @classmethod
    def _decode_lte_nas_plain(cls, b, start, result):
        pkt_ver = cls._search_result(result, "Pkt Version")
        if pkt_ver not in cls._LTE_NAS_PLAIN_FMT:
            print "Unknown LTE NAS version: 0x%x" % pkt_ver
            return 0

        fmt = cls._LTE_NAS_PLAIN_FMT[pkt_ver]
        res2, offset = cls._decode_by_format(fmt, b, start)
        result.extend(res2)
        nas_msg_plain = b[(start + offset):]
        decoded = cls._decode_msg("LTE-NAS_EPS_PLAIN", nas_msg_plain)
        result.append( ("Msg", cls._wrap_decoded_xml(decoded)) )
        return len(b) - start

# Test decoding
if __name__ == '__main__':
    tests = [
            # WCDMA_CELL_ID
            "2c002c002741b4008aef9740ce0040100000211100000f5660030000070030070301000401002cd90000ba000000",
            "2c002c0027419203b0a48540ce00c224000052260000fda3fa02d0000700801f0301000401003ffd00003c000000",
            # WCDMA_SIGNALLING_MESSAGES UL_CCCH
            "200020002F41BA017FA40EDECD000000100039A99DCF8C310A086C96100B54B773C8",
            # WCDMA_SIGNALLING_MESSAGES UL_DCCH
            "4c004C002F41CC00459D0EDECD0001023C004A880000A0005C32AAD506A556A88A4830408001800151B84071CFBE7280E39F7CF501C73EF9E2038E7DF3CF0866B1A812694845F9228D64802981A0",
            # WCDMA_SIGNALLING_MESSAGES DL_CCCH
            "810081002F41CB00019D0EDECD000200710030E73533B9F18621410D92C0D80F5AF4782693A78109E0D420002819FD502C698270B9D3C284F0EA3000144CFE681634C1385CE9E24A78B528002A467F340B1A609C2E74F1A53C7A9C00153200018B4A3D03C0090088EE1982780090088F95E4930E0D1EB5ECFE402843FE207303D029E0",
            # WCDMA_SIGNALLING_MESSAGES DL_DCCH
            "180018002F41E100E89D0EDECD000302080020871B86097A6E3A",

            # WCDMA_SIGNALLING_MESSAGES DL_BCCH_BCH
            #   MIB
            "2f002f002f414a01b442814bcf0004281f00948e00bf10c424c05aa2fe00a0c850448c466608a8e54a80100a0100000003",
            #   SIB1
            "2f002F002F414600D9581EDECD0004281F00844E017FC764B108500B1BA01483078A2BE62AD00000000000000000000000",
            #   SIB3
            "2f002F002F41290029D010DECD0004281F0072EE03760D801F4544FC60005001000011094E000000000000000000000002",
            #   SIB5 segment 1
            "2f002F002F414800E9980EDECD0004281F0004625263403AFFFF03FFFC5010F0290C0A8018000C8BF5B15EA0000003F521",
            #   SIB5 segment 2
            "2f002F002F414C0009990EDECD0004281F0004A450438C000091E25080500004110018088B958C0318080B00030533000E",
            #   SIB5 segment 3
            "2f002F002F414E0019990EDECD0004281F0004C6517CC4300B6D830021844A0585760186AF400000000000000000000002",
            #   SIB7 & SIB12
            "2f002F002F41340049581EDECD0004281F00832E2C43B38111D024541A42A38800C0000000000000000000000000000001",
            #   SIB19
            "2f002F002F417E029DCA10DECD0004281F0067AE1F3B41A1001694E4947000000000000000000000000000000000000002",

            # LTE_RRC_OTA_Packet v7 LTE-RRC_PCCH
            "26002600C0B00000A3894A13CE00070A7100D801B70799390400000000090040012F05EC4E700000",
            # LTE_RRC_OTA_Packet v2 LTE-RRC_PCCH
            "22002200c0b00000c3ad2176ce000209a000d801b707390404090040019d056f48800000",
            # LTE_RRC_OTA_Packet v2 LTE-RRC_DL_DCCH
            "2D002D00C0B00000C6A4212BCD000209A001D200E308000006140022020516EE933E90E02F67000299422868020880",
            # LTE_RRC_OTA_Packet v2 LTE-RRC_UL_DCCH
            "1B001B00C0B00000C8A4212BCD000209A001D200E30800000802001200",
            # LTE_RRC_OTA_Packet v7 LTE-RRC_BCCH_DL_SCH / SystemInformation
            "42004200C0B0000020FC1AEDCD00070A7100B900D5020000020C000000250000804D499F848011580A920220040800D209456AAB00203007B763492540811231BC235FD4",
            "46004600C0B0000020FC1AEDCD00070A7100B900D502000002700000002900010881F5182916943B54003A41F5229A8A992800944419C25001288836A4A00251196FE9C004A22000",

            # LTE_NAS_ESM_Plain_OTA_Incoming_Message v1
            "1f001F00E2B000004F7C4D13CE00010905005200C93209833401085E04FEFEC553",
            # LTE_NAS_ESM_Plain_OTA_Outgoing_Message v1
            "19001900E3B00000507C4D13CE00010905005200CA000000000000",
            # LTE_NAS_EMM_Plain_OTA_Incoming_Message v1
            "3e003E00ECB000001C6E4D13CE00010905000749015A49500BF6130014FF5018D01FFD7C54060013001487405702200013130014D92C2305F4BA302390640100",
            # LTE_NAS_EMM_Plain_OTA_Outgoing_Message v1
            "14001400EDB00000C5D84913CE0001090500C7010400",

            # LTE_ML1_Serving_Cell_Measurement_Result v1
            "6000600093B10000E49484C7CF000101995119045000B707D80118E914023A85C00818751700DDE559009ED55D00DD555249134D5412255D4C5D17030000F2FFF5FF340037000C000000D0AE0C000A9A190037AB0200690900009A1600002AA10200",
            # LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results v3
            "3000300079B100000C9584C7CF000300000000000000B707D8018C0AE605E6053101310101009A00C904C904420042000000",
            "5C005C0079B10000E9F684C7CF000300000000000000B707D801200D160616062F012F0102029A00EA04EA040500050000009B00EA04EA042D002D0000009A000000D313000010634B1F000000009B0000003811000030E3701F00000000",
            ]

    executable_path = os.path.join(os.path.abspath(os.getcwd()),
                                    "../../../../ws_dissector/ws_dissector")
    DMLogPacket.init({
                        "ws_dissect_executable_path": executable_path,
                        "libwireshark_path": "/home/likayo/wireshark-local-1.12.3/lib",
                        })
    i = 0
    for b in tests:
        l, type_id, ts, log_item = DMLogPacket.decode(binascii.a2b_hex(b))
        print "> Packet #%d" % i
        print l, LOG_PACKET_NAME[type_id], ts
        print [k for k, v in log_item]
        print dict(log_item).get("Msg", "XML msg not found.")
        i += 1

    # s = binascii.a2b_hex(tests[-3])
    # import cProfile
    # cProfile.run("for i in range(20): l, type_id, ts, log_item = DMLogPacket.decode(s)")
