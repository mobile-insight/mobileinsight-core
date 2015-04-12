# -*- coding: utf-8 -*-

__all__ = [ "LOG_PACKET_ID",
            "LOG_PACKET_NAME",
            "WCDMA_SIGNALLING_MSG_CHANNEL_NUMBER",
            "WCDMA_SIGNALLING_MSG_CHANNEL_TYPE",
            "LTE_RRC_OTA_PDU_NUMBER",
            "LTE_RRC_OTA_PDU_TYPE",
            ]

LOG_PACKET_ID = {   "WCDMA_CELL_ID":    0x4127,
                    "WCDMA_Signaling_Messages": 0x412F,
                    "LTE_RRC_OTA_Packet":   0xB0C0,
                    "LTE_ML1_IRAT_Measurement_Request": 0xB187,
                    "LTE_ML1_Serving_Cell_Measurement_Result":  0xB193,
                    "LTE_ML1_Connected_Mode_Neighbor_Meas_Req/Resp":    0xB195
                    }

LOG_PACKET_NAME = dict([(v, k) for k, v in LOG_PACKET_ID.items()])

WCDMA_SIGNALLING_MSG_CHANNEL_NUMBER = {"DL_BCCH_BCH":  0x04,
                                            }

WCDMA_SIGNALLING_MSG_CHANNEL_TYPE = dict([(v, k) for k, v in WCDMA_SIGNALLING_MSG_CHANNEL_NUMBER.items()])

LTE_RRC_OTA_PDU_NUMBER = {  "LTE-RRC_PCCH": 0x04,
                            "LTE-RRC_DL_DCCH":  0x06,
                            "LTE-RRC_UL_DCCH":  0x08,
                            }

LTE_RRC_OTA_PDU_TYPE = dict([(v, k) for k, v in LTE_RRC_OTA_PDU_NUMBER.items()])
