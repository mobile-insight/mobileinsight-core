# -*- coding: utf-8 -*-
"""
consts.py
Define constants used in decoding QCDM log packets.

Author: Jiayao Li
"""


__all__ = [ "LOG_PACKET_ID",
            "LOG_PACKET_NAME",
            "WCDMA_SIGNALLING_MSG_CHANNEL_NUMBER",
            "WCDMA_SIGNALLING_MSG_CHANNEL_TYPE",
            "LTE_RRC_OTA_PDU_NUMBER",
            "LTE_RRC_OTA_PDU_TYPE",
            "LTE_ML1_SUBPACKET_ID",
            "LTE_ML1_SUBPACKET_TYPE",
            ]

def dict_reversed(d):
    return dict([(v, k) for k, v in d.items()])

LOG_PACKET_ID = {   "WCDMA_CELL_ID":    0x4127,
                    "WCDMA_Signaling_Messages": 0x412F,
                    "LTE_RRC_OTA_Packet":   0xB0C0,
                    "LTE_RRC_MIB_Message_Log_Packet":   0xB0C1,
                    "LTE_RRC_Serv_Cell_Info_Log_Packet":    0xB0C2,
                    "LTE_NAS_ESM_Plain_OTA_Incoming_Message":   0xB0E2,
                    "LTE_NAS_ESM_Plain_OTA_Outgoing_Message":   0xB0E3,
                    "LTE_NAS_EMM_Plain_OTA_Incoming_Message":   0xB0EC,
                    "LTE_NAS_EMM_Plain_OTA_Outgoing_Message":   0xB0ED,
                    "LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results": 0xB179,
                    "LTE_ML1_IRAT_Measurement_Request": 0xB187,
                    "LTE_ML1_Serving_Cell_Measurement_Result":  0xB193,
                    "LTE_ML1_Connected_Mode_Neighbor_Meas_Req/Resp":    0xB195,
                    }

LOG_PACKET_NAME = dict_reversed(LOG_PACKET_ID)

WCDMA_SIGNALLING_MSG_CHANNEL_NUMBER = { "RRC_UL_CCCH":  0x00,
                                        "RRC_UL_DCCH":  0x01,
                                        "RRC_DL_CCCH":  0x02,
                                        "RRC_DL_DCCH":  0x03,
                                        "RRC_DL_BCCH_BCH":  0x04,
                                        }

WCDMA_SIGNALLING_MSG_CHANNEL_TYPE = dict_reversed(WCDMA_SIGNALLING_MSG_CHANNEL_NUMBER)

LTE_RRC_OTA_PDU_NUMBER = {  "LTE-RRC_BCCH_DL_SCH": 0x02,
                            "LTE-RRC_PCCH": 0x04,
                            "LTE-RRC_DL_DCCH":  0x06,
                            "LTE-RRC_UL_DCCH":  0x08,
                            }

LTE_RRC_OTA_PDU_TYPE = dict_reversed(LTE_RRC_OTA_PDU_NUMBER)

LTE_ML1_SUBPACKET_ID = {    "Serving_Cell_Measurement_Result": 25,
                            }

LTE_ML1_SUBPACKET_TYPE = dict_reversed(LTE_ML1_SUBPACKET_ID)