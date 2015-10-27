#ifndef __DM_COLLECTOR_C_LOG_PACKET_H__
#define __DM_COLLECTOR_C_LOG_PACKET_H__

#include "consts.h"

enum FmtType {
    UINT,       // Little endian
    BYTE_STREAM,
    QCDM_TIMESTAMP,
    PLMN_MK1,   // in WCDMA Cell ID
    PLMN_MK2,   // in LTE NAS EMM State
    BANDWIDTH,  // in LTE RRC Serving Cell Info, LTE RRC MIB Message
    RSRP,
    RSRQ,
    SKIP,
    PLACEHOLDER
};

struct Fmt {
    FmtType type;
    const char *field_name;
    int len;
};

const Fmt LogPacketHeaderFmt [] = {
    {UINT, "len1", 2},
    {UINT, "len2", 2},
    {UINT, "type_id", 2},
    {QCDM_TIMESTAMP, "timestamp", 8}
};

//Yuanjie: the following comments are my suggestions for field name replacement
//No change if the comments are missing

const Fmt _1xEVSignalingFmt [] = {
    {UINT, "Band", 1},
    {UINT, "Channel Number", 2},
    {UINT, "Pilot PN", 2},
    {UINT, "HSTR", 2},
    {UINT, "Flags", 1}, // Unknown
    {UINT, "Seq No", 1},
    {UINT, "Ack Seq No", 1},
    {UINT, "SyncCCFlag", 1}
    // Unknown from here
};

const Fmt WcdmaCellIdFmt [] = {
    {UINT, "UTRA UL Absolute RF channel number", 4},    //Uplink RF channel number
    {UINT, "UTRA DL Absolute RF channel number", 4},    //Download RF channel number
    {UINT, "Cell identity (28-bits)", 4},               //Cell ID
    {UINT, "URA to use in case of overlapping URAs", 1}, // UTRA registration area (overlapping URAs)
    {SKIP, NULL, 2},    // Unknown yet
    {UINT, "Allowed Call Access", 1},   //Allowed call access
    {UINT, "PSC", 2},   //PSC
    {PLMN_MK1, "PLMN", 6},  //PLMN
    {UINT, "LAC id", 4},    //Location area code
    {UINT, "RAC id", 4}     //routing area code
};

// ------------------------------------------------------------
const Fmt WcdmaSignalingMessagesFmt [] = {
    {UINT, "Channel Type", 1},
    {UINT, "Radio Bearer ID", 1},
    {UINT, "Message Length", 2}
};

const ValueName WcdmaSignalingMsgChannelType [] = {
    {0x00, "RRC_UL_CCCH"},
    {0x01, "RRC_UL_DCCH"},
    {0x02, "RRC_DL_CCCH"},
    {0x03, "RRC_DL_DCCH"},
    {0x04, "RRC_DL_BCCH_BCH"},
    {0x06, "RRC_DL_PCCH"}
};

// ------------------------------------------------------------
// UMTS NAS GMM State
const Fmt UmtsNasGmmStateFmt [] = {
    {UINT, "GMM State", 1},
    {UINT, "GMM Substate", 1},
    {UINT, "GMM Update Status", 1}
};

const ValueName UmtsNasGmmState_GmmState [] = {
    {0, "GMM_NULL"}, 
    {1, "GMM_DEREGISTERED"}, 
    {2, "GMM_REGISTERED_INITIATED"}, 
    {3, "GMM_REGISTERED"}, 
    {4, "GMM_DEREGISTERED_INITIATED"}, 
    {5, "GMM_ROUTING_AREA_UPDATING_INITIATED"}, 
    {6, "GMM_SERVICE_REQUEST_INITIATED"}
};

const ValueName UmtsNasGmmState_GmmSubstate [] = {
    {0, "GMM_NORMAL_SERVICE"},
    {1, "GMM_LIMITED_SERVICE"},
    {2, "GMM_ATTACH_NEEDED"},
    {3, "GMM_ATTEMPTING_TO_ATTACH"},
    {4, "GMM_NO_IMSI"},
    {5, "GMM_NO_CELL_AVAILABLE"},
    {6, "GMM_PLMN_SEARCH"},
    {7, "GMM_SUSPENDED"},
    {8, "GMM_UPDATE_NEEDED"},
    {9, "GMM_ATTEMPTING_TO_UPDATE"},
    {10, "GMM_ATTEMPTING_TO_UPDATE_MM"},
    {11, "GMM_IMSI_DETACH_INITIATED"}
};

const ValueName UmtsNasGmmState_GmmUpdateStatus [] = {
    {0, "GMM_GU1_UPDATED"},
    {1, "GMM_GU2_NOT_UPDATED"},
    {2, "GMM_GU3_PLMN_NOT_ALLOWED"},
    {3, "GMM_GU3_ROUTING_AREA_NOT_ALLOWED"}
};

// ------------------------------------------------------------
// UMTS NAS MM State
const Fmt UmtsNasMmStateFmt [] = {
    {UINT, "MM State", 1},
    {UINT, "MM Substate", 1},
    {UINT, "MM Update Status", 1}
};

const ValueName UmtsNasMmState_MmState [] = {
    {0, "MM_NULL"},
    {3, "MM_LOCATION_UPDATE_INITIATED"},
    {5, "MM_WAIT_FOR_OUTGOING_MM_CONNECTION"},
    {6, "MM_CONNECTION_ACTIVE"},
    {7, "MM_IMSI_DETACH_INITIATED"},
    {9, "MM_WAIT_FOR_NETWORK_COMMAND"},
    {10, "MM_LOCATION_UPDATE_REJECTED"},
    {13, "MM_WAIT_FOR_RR_CONNECTION_UL"},
    {14, "MM_WAIT_FOR_RR_CONNECTION_MM"},
    {15, "MM_WAIT_FOR_RR_CONNECTION_IMSI_DETACH"},
    {17, "MM_REESTABLISHMENT_INITIATED"},
    {18, "MM_WAIT_FOR_RR_ACTIVE"},
    {19, "MM_IDLE"},
    {20, "MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION"},
    {21, "MM_WAIT_FOR_RR_CONNECTION_REESTABLISHMENT"},
    {22, "MM_WAIT_FOR_REESTABLISH_DECISION"}
};

const ValueName UmtsNasMmState_MmSubstate [] = {
    {0, "MM_NULL_SUBSTATE"},
    {1, "MM_NO_IMSI"},
    {2, "MM_PLMN_SEARCH"},
    {3, "MM_LIMITED_SERVICE"},
    {4, "MM_ATTEMPTING_TO_UPDATE"},
    {5, "MM_LOCATION_UPDATE_NEEDED"},
    {6, "MM_NO_CELL_AVAILABLE"},
    {7, "MM_PLMN_SEARCH_NORMAL_SERVICE"},
    {8, "MM_NORMAL_SERVICE"}
};

const ValueName UmtsNasMmState_MmUpdateStatus [] = {
    {0, "UPDATED"},
    {1, "NOT_UPDATED"},
    {2, "ROAMING_NOT_ALLOWED"}
};

// ------------------------------------------------------------
// UMTS NAS MM State
const Fmt UmtsNasOtaFmt [] = {
    {UINT, "Message Direction", 1},
    {UINT, "Message Length", 4}
    // Followed by NAS messages
};

const ValueName UmtsNasOtaFmt_MessageDirection [] = {
    {0, "TO_UE"},
    {1, "FROM_UE"}
};

// ------------------------------------------------------------
// LTE_RRC_OTA_Packet
const Fmt LteRrcOtaPacketFmt [] = {
    {UINT, "Pkt Version", 1},           //version
    {UINT, "RRC Release Number", 1},    //RRC release version
    {UINT, "Major/minor", 1},
    {UINT, "Radio Bearer ID", 1},       //no change
    {UINT, "Physical Cell ID", 2}       //Cell ID
    // continued in LteRrcOtaPacketFmt_v2 or LteRrcOtaPacketFmt_v7
};

const Fmt LteRrcOtaPacketFmt_v2 [] = {
    {UINT, "Freq", 2},                  //frequency
    {UINT, "SysFrameNum/SubFrameNum", 2},   //System/subsystem frame number
    {UINT, "PDU Number", 1},            //PDU number
    {UINT, "Msg Length", 1},
    {UINT, "SIB Mask in SI", 1}
};

const Fmt LteRrcOtaPacketFmt_v7 [] = {
    {UINT, "Freq", 2},                  //frequency
    {UINT, "SysFrameNum/SubFrameNum", 2},   //System/subsystem frame number
    {UINT, "PDU Number", 1},            //PDU number
    {SKIP, NULL, 4},            // Unknown yet, only for Pkt Version = 7
    {UINT, "Msg Length", 1},
    {UINT, "SIB Mask in SI", 1}
};

const Fmt LteRrcOtaPacketFmt_v8 [] = {
    {UINT, "Freq", 4},                  //frequency
    {UINT, "SysFrameNum/SubFrameNum", 2},   //System/subsystem frame number
    {UINT, "PDU Number", 1},            //PDU number
    {UINT, "SIB Mask in SI", 1},
    {SKIP, NULL, 3},
    {UINT, "Msg Length", 2}
};

const ValueName LteRrcOtaPduType [] = {
    {0x02, "LTE-RRC_BCCH_DL_SCH"},
    {0x04, "LTE-RRC_PCCH"},
    {0x05, "LTE-RRC_DL_CCCH"},
    {0x06, "LTE-RRC_DL_DCCH"},
    {0x07, "LTE-RRC_UL_CCCH"},
    {0x08, "LTE-RRC_UL_DCCH"}
};

// ------------------------------------------------------------
// LTE NAS Plain
const Fmt LteNasPlainFmt [] = {
    {UINT, "Pkt Version", 1}
};

const Fmt LteNasPlainFmt_v1 [] = {
    {UINT, "RRC Release Number", 1},
    {UINT, "Major Version", 1},
    {UINT, "Minor Version", 1}
    // followed by NAS messages
};

// ------------------------------------------------------------
// LTE NAS EMM State
const Fmt LteNasEmmStateFmt [] = {
    {UINT, "Version", 1}
};

const Fmt LteNasEmmStateFmt_v2 [] = {
    {UINT, "EMM State", 1},
    {UINT, "EMM Substate", 2},
    {PLMN_MK2, "PLMN", 3},
    {UINT, "GUTI Valid", 1},
    {UINT, "GUTI UE Id", 1},
    {PLMN_MK2, "GUTI PLMN", 3},
    {BYTE_STREAM, "GUTI MME Group ID", 2},
    {BYTE_STREAM, "GUTI MME Code", 1},
    {BYTE_STREAM, "GUTI M-TMSI", 4}
};

const ValueName LteNasEmmState_v2_EmmState [] = {
    {0, "EMM_NULL"},                    // No substate
    {1, "EMM_DEREGISTERED"},            // Substate table: Deregistered
    {2, "EMM_REGISTERED_INITIATED"},    // Substate table: Registered_Initiated
    {3, "EMM_REGISTERED"},              // Substate table: Registered
    {4, "EMM_TRACKING_AREA_UPDATING_INITIATED"},    // The same as above
    {5, "EMM_SERVICE_REQUEST_INITIATED"},   // The same as above
    {6, "EMM_DEREGISTERED_INITIATED"}       // No substate
};

const ValueName LteNasEmmState_v2_EmmSubstate_Deregistered [] = {
    {0, "EMM_DEREGISTERED_NO_IMSI"},
    {1, "EMM_DEREGISTERED_PLMN_SEARCH"},
    {2, "EMM_DEREGISTERED_ATTACH_NEEDED"},
    {3, "EMM_DEREGISTERED_NO_CELL_AVAILABLE"},
    {4, "EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH"},
    {5, "EMM_DEREGISTERED_NORMAL_SERVICE"},
    {6, "EMM_DEREGISTERED_LIMITED_SERVICE"}
};

const ValueName LteNasEmmState_v2_EmmSubstate_Registered_Initiated [] = {
    {0, "EMM_WAITING_FOR_NW_RESPONSE"},
    {1, "EMM_WAITING_FOR_ESM_RESPONSE"}
};

const ValueName LteNasEmmState_v2_EmmSubstate_Registered [] = {
    {0, "EMM_REGISTERED_NORMAL_SERVICE"},
    {1, "EMM_REGISTERED_UPDATE_NEEDED"},
    {2, "EMM_REGISTERED_ATTEMPTING_TO_UPDATE"},
    {3, "EMM_REGISTERED_NO_CELL_AVAILABLE"},
    {4, "EMM_REGISTERED_PLMN_SEARCH"},
    {5, "EMM_REGISTERED_LIMITED_SERVICE"}
};

// ------------------------------------------------------------
const Fmt LteLl1PdschDemapperConfigFmt [] = {
    {UINT, "Version", 1}
};

const Fmt LteLl1PdschDemapperConfigFmt_v23 [] = {
    {UINT, "Serving Cell ID", 1},
    {UINT, "System Frame Number", 2},
    {PLACEHOLDER, "Subframe Number", 0},
    {UINT, "PDSCH RNTIl ID", 2},
    {UINT, "Number of Tx Antennas(M)", 2},
    {PLACEHOLDER, "Number of Rx Antennas(N)", 0},
    {UINT, "RB Allocation Slot 0[0]", 8},
    {UINT, "RB Allocation Slot 0[1]", 8},
    {UINT, "RB Allocation Slot 1[0]", 8},
    {UINT, "RB Allocation Slot 1[1]", 8},
    {SKIP, NULL, 4},
    {UINT, "Transport Block Size Stream 0", 2},
    {UINT, "Modulation Stream 0", 2},
    {PLACEHOLDER, "Traffic to Pilot Ratio", 0},
    {UINT, "Transport Block Size Stream 1", 2},
    {UINT, "Modulation Stream 1", 2},
    {PLACEHOLDER, "Carrier Index", 0}
};

const ValueName LteLl1PdschDemapperConfig_v23_Modulation [] = {
    {0, "QPSK"},
    {1, "16QAM"},
    {2, "64QAM"}
};

const ValueName LteLl1PdschDemapperConfig_v23_Carrier_Index [] = {
    {0, "PCC"},
    {1, "SCC"}
};

// ------------------------------------------------------------
// LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results
const Fmt LteMl1CmlifmrFmt [] = {
    {UINT, "Version", 1},
    {SKIP, NULL, 7},        // Unknown
};

const Fmt LteMl1CmlifmrFmt_v3_Header [] = {
    {UINT, "E-ARFCN", 2},
    {UINT, "Serving Physical Cell ID", 2},  //serving cell ID
    {UINT, "Sub-frame Number", 2},
    {RSRP, "Serving Filtered RSRP(dBm)", 2}, //Filtered RSRP (dBm)
    {SKIP, NULL, 2},    // Duplicated
    {RSRQ, "Serving Filtered RSRQ(dB)", 2}, //Filtered RSRQ (dBm)
    {SKIP, NULL, 2},    // Duplicated
    {UINT, "Number of Neighbor Cells", 1},
    {UINT, "Number of Detected Cells", 1}
};

const Fmt LteMl1CmlifmrFmt_v3_Neighbor_Cell [] = {
    {UINT, "Physical Cell ID", 2},  //cell ID
    {RSRP, "Filtered RSRP(dBm)", 2},
    {SKIP, NULL, 2},    // Duplicated
    {RSRQ, "Filtered RSRQ(dB)", 2},
    {SKIP, NULL, 4}     // Duplicated & reserved
};

const Fmt LteMl1CmlifmrFmt_v3_Detected_Cell [] = {
    {UINT, "Physical Cell ID", 4},  //cell ID
    {UINT, "SSS Corr Value", 4},
    {UINT, "Reference Time", 8}
};

// ------------------------------------------------------------
const Fmt LteMl1SubpktFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Number of SubPackets", 1},
    {SKIP, NULL, 2}     // Unknown
};

const Fmt LteMl1SubpktFmt_v1_SubpktHeader [] = {
    {UINT, "SubPacket ID", 1},
    {UINT, "Version", 1},
    {UINT, "SubPacket Size", 2},
};

const ValueName LteMl1Subpkt_SubpktType [] = {
    {25, "Serving_Cell_Measurement_Result"}
};

// Serving_Cell_Measurement_Result
const Fmt LteMl1SubpktFmt_v1_Scmr_v4 [] = {
    {UINT, "E-ARFCN", 2},
    {UINT, "Physical Cell ID", 2}   //cell ID
};

// ------------------------------------------------------------
// LTE RRC Serving Cell Info Log Pkt
const Fmt LteRrcServCellInfoLogPacketFmt [] = {
    {UINT, "Version", 1},
};

const Fmt LteRrcServCellInfoLogPacketFmt_v2 [] = {
    {UINT, "Physical Cell ID", 2},  //Physical cell ID
    {UINT, "DL FREQ", 2},   //Downlink frequency
    {UINT, "UL FREQ", 2},   //Uplink frequency
    {BANDWIDTH, "DL BW", 1},    //Downlink bandwidth
    {BANDWIDTH, "UL BW", 1},    //Uplink bandwidth
    {UINT, "Cell Identity", 4}, //cell ID
    {UINT, "TAC", 2},   //Tracking area code
    {UINT, "Band Indicator", 4},    //Band indicator
    {UINT, "MCC", 2},   //MCC
    {UINT, "MNC Digit", 1}, //MNC digit
    {UINT, "MNC", 2},   //MNC
    {UINT, "Allowed Access", 1} //Allowed access
};

const Fmt LteRrcServCellInfoLogPacketFmt_v3 [] = {
    {UINT, "Physical Cell ID", 2},  //Physical cell ID
    {UINT, "DL FREQ", 4},   //Downlink frequency
    {UINT, "UL FREQ", 4},   //Uplink frequency
    {BANDWIDTH, "DL BW", 1},    //Downlink bandwidth
    {BANDWIDTH, "UL BW", 1},    //Uplink bandwidth
    {UINT, "Cell Identity", 4}, //cell ID
    {UINT, "TAC", 2},   //Tracking area code
    {UINT, "Band Indicator", 4},    //Band indicator
    {UINT, "MCC", 2},   //MCC
    {UINT, "MNC Digit", 1}, //MNC digit
    {UINT, "MNC", 2},   //MNC
    {UINT, "Allowed Access", 1} //Allowed access
};

// TODO: interpret the value of "Allowed Access"

// ------------------------------------------------------------
const Fmt LteRrcMibMessageLogPacketFmt [] = {
    {UINT, "Version", 1}
};

const Fmt LteRrcMibMessageLogPacketFmt_v1 []  = {
    {UINT, "Physical Cell ID", 2},  //cell ID
    {UINT, "Freq", 2},  //frequency
    {UINT, "SFN", 2},
    {UINT, "Number of Antenna", 1},
    {BANDWIDTH, "DL BW", 1}     //downlink bandwidth
};

const Fmt LteRrcMibMessageLogPacketFmt_v2 []  = {
    {UINT, "Physical Cell ID", 2},  //cell ID
    {UINT, "Freq", 4},  //frequency
    {UINT, "SFN", 2},
    {UINT, "Number of Antenna", 1},
    {BANDWIDTH, "DL BW", 1}     //downlink bandwidth
};

// ----------------------------------------------------------------------------
// TODO: Haotian
const Fmt LtePdcpDlSrbIntegrityDataPduFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num SubPkt", 1},
    {SKIP, NULL, 44},
    {UINT, "PDU Size", 2}, // 47-48
    {SKIP, NULL, 16}
};

// ----------------------------------------------------------------------------
// TODO: Haotian
const Fmt LtePdcpUlSrbIntegrityDataPduFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num SubPkt", 1},
    {SKIP, NULL, 44},
    {UINT, "PDU Size", 2}, // 47-48
    {SKIP, NULL, 12}
};

// ----------------------------------------------------------------------------
// MAC_Configuration
// TODO: Jie
const ValueName LteMacConfigurationSubpkt_SubpktType [] = {
    {0, "Config Type Subpacket"},
    {1, "DL Config SubPacket"},
    {2, "UL Config SubPacket"},
    {3, "RACH Config SubPacket"},
    {4, "LC Config SubPacket"},
    {7, "DL Transport Block"},
    {8, "UL Transport Block"},
    {13, "eMBMS Config SubPacket"}
};

const Fmt LteMacConfigurationFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num SubPkt", 1},
    {SKIP, NULL, 2}
};

const Fmt LteMacConfiguration_SubpktHeader [] = {
    {UINT, "SubPacket ID", 1},
    {UINT, "Version", 1},
    {UINT, "SubPacket Size", 2}
};

const Fmt LteMacConfigurationSubpkt_ConfigType [] = {
    {UINT, "Config reason", 4}
};

const ValueName LteMacConfigurationConfigType_ConfigReason [] = {
    {2050, "CONNECTION RELEASE"}
};

const Fmt LteMacConfigurationSubpkt_DLConfig [] = {
    {UINT, "TA Timer", 2}, // 0xFF need to be read as infinity
    {SKIP, "NULL", 2}
};

const Fmt LteMacConfigurationSubpkt_ULConfig [] = {
    {UINT, "SR periodicity", 3},
    {UINT, "BSR timer", 2},
    {UINT, "SPS Number of Tx released", 2},
    {UINT, "Retx BSR timer", 2}, // 0xFF need to be read as infinity
    {SKIP, "NULL", 3}
};

const Fmt LteMacConfigurationSubpkt_RACHConfig [] = {
    {RSRQ, "Preamble initial power", 2},
    {UINT, "Power ramping step", 1},
    {UINT, "RA index1", 1},
    {UINT, "RA index2", 1},
    {UINT, "Preamble trans max", 1},
    {UINT, "Contention resolution timer", 1},
    {SKIP, "NULL", 4},
    {UINT, "PMax", 2},
    {UINT, "Delta preamble Msg3", 2},
    {UINT, "PRACH config", 1},
    {UINT, "CS zone length", 1},
    {UINT, "Root seq index", 2},
    {UINT, "PRACH Freq Offset", 2},
    {UINT, "Max retx Msg3", 1},
    {UINT, "RA rsp win size", 1},
    {SKIP, "NULL", 1},
};

const Fmt LteMacConfigurationSubpkt_LCConfig [] = {
    {UINT, "Number of deleted LC", 1},
    {SKIP, "NULL", 32},
    {UINT, "Number of added/modified LC", 1}
//    {SKIP, "NULL", 290}
};

const Fmt LteMacConfiguration_LCConfig_LC [] = {
    {UINT, "LC ID", 1},
    {UINT, "PBR(KBytes/s)", 2},
    {UINT, "Priority", 1},
    {UINT, "LC group", 1},
    {UINT, "Token bucket size (bytes)", 4}
};

const Fmt LteMacConfigurationSubpkt_eMBMSConfig [] = {
    {UINT, "Num eMBMS Active LCs", 2}, // Not sure
    {SKIP, "NULL", 98}
};

// ----------------------------------------------------------
// MAC UL Transport Block
// TODO: Jie
const ValueName BSREvent [] = {
    {2, "High Data Arrival"},
    {1, "Periodic"},
};

const ValueName BSRTrig [] = {
    {4, "S-BSR"},
    {3, "Pad L-BSR"}
};

const Fmt LteMacULTransportBlockFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num SubPkt", 1},
    {SKIP, NULL, 2}
};

const Fmt LteMacULTransportBlock_SubpktHeaderFmt [] = {
    {UINT, "SubPacket ID", 1},
    {UINT, "Version", 1},
    {UINT, "SubPacket Size", 2},
    {UINT, "Num Samples", 1},
    {SKIP, "NULL", 2}
};

const Fmt LteMacULTransportBlock_SubpktV1_SampleFmt [] = {
    {SKIP, "SFN and Sub-FN", 2}, // not byte aligned
    // QCAT show "RNTI Type" and "HARQ ID" before Grant, but there's no corresponding hex data
    {UINT, "Grant (bytes)", 2},
    {UINT, "RLC PDUs", 1},
    {UINT, "Padding (bytes)", 2},
    {UINT, "BSR event", 1},
    {UINT, "BSR trig", 1},
    {UINT, "HDR LEN", 1},
    // 
    //{SKIP, "Mac Hdr + CE", 5}, // a flexible length field of Hex value in "Mac Hdr + CE", observed 5 or 7
    //{SKIP, "NULL", 2}
};

// ----------------------------------------------------------
// MAC DL Transport Block
// TODO: Jie
const ValueName RNTIType [] = {
    {0, "C-RNTI"},
    {2, "P-RNTI"},
    {3, "RA-RNTI"},
    {4, "T-C-RNTI"},
    {5, "SI-RNTI"}
};

const Fmt LteMacDLTransportBlockFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num SubPkt", 1},
    {SKIP, NULL, 2}
};

const Fmt LteMacDLTransportBlock_SubpktHeaderFmt [] = {
    {UINT, "SubPacket ID", 1},
    {UINT, "Version", 1},
    {UINT, "SubPacket Size", 2},
    {UINT, "Num Samples", 1},
};

const Fmt LteMacDLTransportBlock_SubpktV2_SampleFmt [] = {
    {SKIP, "SFN and Sub-FN", 2}, // not byte aligned
    {UINT, "RNTI Type", 1},
    {UINT, "HARQ ID", 1},
    {SKIP, "Area ID & PMCH ID", 2},
    {UINT, "DL TBS (bytes)", 2},
    {UINT, "RLC PDUs", 1},
    // QCAT shows a "EMBMS PDUs" but there's no corresponding hex data
    {UINT, "Padding (bytes)", 2},
    {UINT, "HDR LEN", 1},
    //
    //{SKIP, "Mac Hdr + CE", 5}, // a flexible length field of Hex value in "Mac Hdr + CE", observed 5 or 7
    //{SKIP, "NULL", 2}
};


bool is_log_packet (const char *b, int length);
PyObject * decode_log_packet (const char *b, int length);

#endif  // __DM_COLLECTOR_C_LOG_PACKET_H__
