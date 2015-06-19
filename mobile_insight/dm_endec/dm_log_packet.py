# -*- coding: utf-8 -*-
"""
dm_log_packet.py
Define DMLogPacket class.

Author: Jiayao Li
"""

__all__ = ["DMLogPacket", "FormatError"]

import binascii
from datetime import *
import json
import struct
import xml.etree.ElementTree as ET

try:
    from utils import *
except ImportError, e:
    # TODO: WTF can I do to remove this dependence ..?
    def static_var(varname, value):
        def decorate(func):
            setattr(func, varname, value)
            return func
        return decorate

import dm_endec_c
from consts import *
from ws_dissector import *

class SuperTuple(tuple):
    def __new__(cls, data, xml_type, xml_list_item_tag=None):
        return super(SuperTuple, cls).__new__(cls, data)

    def __init__(self, data, xml_type, xml_list_item_tag=None):
        self.xml_type = xml_type
        self.xml_list_item_tag = xml_list_item_tag


class SuperEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, datetime):
            return str(obj)
        # Let the base class default method raise the TypeError
        return json.JSONEncoder.default(self, obj)


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
    """

    _init_called = False

    def __init__(self, b):
        self._binary = b
        self._decoded_list = None
        self._decoded_dict = None

    def decode(self):
        """
        Decode a QCDM log packet.

        Returns:
            a Python dict object that looks like:

            {
                "type_id": 0xB0C0, # for LTE RRC OTA
                "timestamp": datetime.datetime(......),
                "Pkt Version": 2,
                "RRC Release Number": 9,
                # other fields ...
                "Msg": \"\"\"
                    <msg>
                        <packet>...</packet>
                        <packet>...</packet>
                        <packet>...</packet>
                    </msg>
                    \"\"\",
            }

        Raises:
            FormatError: this message has an unknown type
        """
        cls = self.__class__
        if self._decoded_list is not None:
            return self._decoded_dict

        assert cls._init_called
        self._decoded_list = dm_endec_c.decode_log_packet(self._binary)
        self._decoded_dict = cls._parse_internal_list("dict", self._decoded_list)
        return self._decoded_dict

    @classmethod
    @static_var("wcdma_sib_types", {    0: "RRC_MIB",
                                        1: "RRC_SIB1",
                                        3: "RRC_SIB3",
                                        7: "RRC_SIB7",
                                        12: "RRC_SIB12",
                                        31: "RRC_SIB19",
                                        })
    def _parse_internal_list(cls, out_type, decoded_list):
        if out_type == "dict":
            d = dict()
        elif out_type == "list":
            lst = []

        for field_name, val, type_str in decoded_list:
            if not type_str:
                xx = val

            elif type_str.startswith("raw_msg/"):
                msg_type = type_str[8:]
                decoded = cls._decode_msg(msg_type, val)
                xmls = [decoded,]

                if msg_type == "RRC_DL_BCCH_BCH":
                    sib_types = cls._parse_internal_list.wcdma_sib_types
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
                                # print sib_types[sib_id]
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
                xx = cls._wrap_decoded_xml(xmls)
            elif type_str == "dict":
                xx = cls._parse_internal_list("dict", val)
            elif type_str == "list":
                xx = cls._parse_internal_list("list", val)

            if out_type == "dict":
                d[field_name] = xx
            elif out_type == "list":
                lst.append(xx)

        if out_type == "dict":
            return d
        elif out_type == "list":
            return tuple(lst)

    def decode_xml(self):
        """
        Decode the message and convert to a standard XML document.

        Returns:
            a string that contains the converted XML document.
        """
        cls = self.__class__
        if self._decoded_list is None:
            ignored = self.decode()

        # root tag is like a "dict" type in XML
        xml = cls._decode_xml_recursive(self._decoded_list, "dict", "")
        xml.tag = "qcdm_log_packet"
        return ET.tostring(xml)

    def decode_json(self):
        """
        Decode the message and convert to a standard JSON dictionary.

        Returns:
            a string that contains the converted XML document.
        """
        cls = self.__class__
        if self._decoded_list is None:
            ignored = self.decode()

        return json.dumps(self._decoded_dict, cls=SuperEncoder)

    @classmethod
    def _decode_xml_recursive(cls, element, xml_type=None, list_item_tag=""):
        """
        Return an XML element or a str, depending on the value of xml_type.
        """
        # TODO: rewrite this part
        if xml_type == "list":
            assert list_item_tag != ""
            lst_tag = ET.Element("list")
            for item in element:
                item_tag = ET.SubElement(lst_tag, list_item_tag)
                if isinstance(item, SuperTuple):
                    item_tag.append(cls._decode_xml_recursive(
                                                item,
                                                item.xml_type,
                                                item.xml_list_item_tag)
                                    )
                else:
                    item_tag.text = cls._decode_xml_recursive(item)
            return lst_tag

        elif xml_type == "dict":
            dict_tag = ET.Element("dict")
            for pair in element:
                pair_tag = ET.SubElement(dict_tag, "pair", {"key": pair[0]})
                if pair[0] == "Msg":
                    pair_tag.set("type", "list")
                    pair_tag.append(ET.fromstring(pair[1]))
                elif isinstance(pair, SuperTuple):
                    pair_tag.append(cls._decode_xml_recursive(
                                                pair[1],
                                                pair.xml_type,
                                                pair.xml_list_item_tag),
                                    )
                else:
                    pair_tag.text = cls._decode_xml_recursive(pair[1])
            return dict_tag

        else:
            return str(element)

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

    import os
    executable_path = os.path.join(os.path.abspath(os.getcwd()),
                                    "../../ws_dissector/ws_dissector")
    DMLogPacket.init({
                        "ws_dissect_executable_path": executable_path,
                        "libwireshark_path": "/home/likayo/wireshark-local-1.12.3/lib",
                        })
    i = 0
    for b in tests:
        packet = DMLogPacket(binascii.a2b_hex(b))
        d = packet.decode()
        print "> Packet #%d" % i
        print LOG_PACKET_NAME[d["type_id"]], d["timestamp"]
        print sorted(d.items())
        # print packet.decode_xml()
        # print packet.decode_json()
        # print d.get("Msg", "XML msg not found.")
        i += 1

    # s = binascii.a2b_hex(tests[-3])
    # import cProfile
    # cProfile.run("for i in range(20): l, type_id, ts, log_item = DMLogPacket.decode(s)")
