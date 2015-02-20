# -*- coding: utf-8 -*-

__all__ = ["LOG_PACKET_ID", "LTE_RRC_OTA_PDU_NUMBER", "LTE_RRC_OTA_PDU_TYPE"]

LOG_PACKET_ID = {   "WCDMA_CELL_ID":    0x4127,
                    "LTE_RRC_OTA_Packet":   0xB0C0,
                     }

LTE_RRC_OTA_PDU_NUMBER = {  "PCCH": 0x04,
                            }

LTE_RRC_OTA_PDU_TYPE = dict([(v, k) for k, v in LTE_RRC_OTA_PDU_NUMBER.items()])
