/* log_packet.h
 * Author: Jiayao Li
 * Defines constants and functions related to log packet messages.
 * The most important thing is a struct called Fmt, which defines a message
 * field. With this struct and other helper functions, message decoding can be
 * greatly simplified.
 */

#ifndef __DM_COLLECTOR_C_LOG_PACKET_H__
#define __DM_COLLECTOR_C_LOG_PACKET_H__

#include "consts.h"
#include <stddef.h>

// Field types
enum FmtType {
    UINT,       // Little endian. len = 1, 2, 4, 8
    UINT_BIG_ENDIAN,    // Big endian uint
    BYTE_STREAM,    // A stream of bytes.
    BYTE_STREAM_LITTLE_ENDIAN,  // a stream of bytes in little endian
    QCDM_TIMESTAMP, // Timestamp in all messages. len = 8
    PLMN_MK1,   // in WCDMA Cell ID
    PLMN_MK2,   // in LTE NAS EMM State
    BANDWIDTH,  // in LTE RRC Serving Cell Info, LTE RRC MIB Message
    RSRP,
    RSRQ,
    SKIP,   // This field is ignored (but bytes are consumed)
    PLACEHOLDER, // This field is created with a dummy value (no byte is consumed)
    WCDMA_MEAS, // Used for RSCP/RSSI/ECN0 in LTE_PHY_IRAT_MDB
};

struct Fmt {
    FmtType type;
    const char *field_name;
    int len;    // Some FmtType has restrictions on this field.
};

const Fmt LogPacketHeaderFmt [] = {
    {SKIP, NULL, 2},
    {UINT, "log_msg_len", 2},
    {UINT, "type_id", 2},
    {QCDM_TIMESTAMP, "timestamp", 8}
};

//Yuanjie: the following comments are my suggestions for field name replacement
//No change if the comments are missing

const Fmt WcdmaCellIdFmt [] = {
    {UINT, "Uplink RF channel number", 4},    //Uplink RF channel number
    {UINT, "Download RF channel number", 4},    //Download RF channel number
    {UINT, "Cell ID", 4},               //Cell ID
    {UINT, "UTRA registration area (overlapping URAs)", 1}, // UTRA registration area (overlapping URAs)
    {SKIP, NULL, 2},    // Unknown yet
    {UINT, "Allowed call access", 1},   //Allowed call access
    {UINT, "PSC", 2},   //PSC
    {PLMN_MK1, "PLMN", 6},  //PLMN
    {UINT, "LAC", 4},    //Location area code
    {UINT, "RAC", 4}     //routing area code
};

// ------------------------------------------------------------
const Fmt WcdmaSignalingMessagesFmt [] = {
    {UINT, "Channel Type", 1},
    {UINT, "Radio Bearer ID", 1},
    {UINT, "Message Length", 2}
};

const Fmt WcdmaSignalingMessagesFmtExtraSIBType [] = {
    {UINT, "Extra SIB Type", 1},
};

const ValueName ValueNameWcdmaExtraSIBType [] = {
    {0, "RRC_MIB"},
    {1, "RRC_SIB1"},
    {2, "RRC_SIB2"},
    {3, "RRC_SIB3"},
    {5, "RRC_SIB5"},
    {7, "RRC_SIB7"},
    {11, "RRC_SIB11"},
    {12, "RRC_SIB12"},
    {19, "RRC_SIB19"},
};

const Fmt WcdmaSignalingMessagesFmtExtensionSIBType [] = {
    {UINT, "Extension SIB Type", 1},
};

const ValueName ValueNameWcdmaExtensionSIBType [] = {
    {0x43, "RRC_SIB19"},    // Need more items here
};

const ValueName WcdmaSignalingMsgChannelType [] = {
    {0x00, "RRC_UL_CCCH"},
    {0x01, "RRC_UL_DCCH"},
    {0x02, "RRC_DL_CCCH"},
    {0x03, "RRC_DL_DCCH"},
    {0x04, "RRC_DL_BCCH_BCH"},
    {0x06, "RRC_DL_PCCH"},
    {0x09, "Extension SIB"},
    {0x84, "RRC_DL_BCCH_BCH"},  // Qualcom makes duplicate constant?
    {0xfe, "RRC_COMPLETE_SIB"},
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

const Fmt UmtsNasMmRegStateFmt [] = {
    {UINT, "Network operation mode", 1},
    {UINT, "CS/PS service type", 1},
    {PLMN_MK2, "PLMN", 3},
    {UINT, "LAC", 2},
    {UINT, "RAC", 1},
    {UINT, "Available PLMN count",1},
    {UINT, "CSG", 4}
};


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

// Apply to version 2 packets
const Fmt LteRrcOtaPacketFmt_v2 [] = {
    {UINT, "Freq", 2},                  //frequency
    {UINT, "SysFrameNum/SubFrameNum", 2},   //System/subsystem frame number
    {UINT, "PDU Number", 1},            //PDU number
    {UINT, "Msg Length", 1},
    {UINT, "SIB Mask in SI", 1}
};

const Fmt LteRrcOtaPacketFmt_v4 [] = {
    {UINT, "Freq", 2},                  //frequency
    {UINT, "SysFrameNum/SubFrameNum", 2},   //System/subsystem frame number
    {UINT, "PDU Number", 1},            //PDU number
    {UINT, "Msg Length", 2},
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

const Fmt LteRrcOtaPacketFmt_v9 [] = {
    {UINT, "Freq", 4},                  //frequency
    {UINT, "SysFrameNum/SubFrameNum", 2},   //System/subsystem frame number
    {UINT, "PDU Number", 1},            //PDU number
    {UINT, "SIB Mask in SI", 1},
    {SKIP, NULL, 3},
    {UINT, "Msg Length", 2}
};

const Fmt LteRrcOtaPacketFmt_v12 [] = {
    {UINT, "Freq", 4},                  //frequency
    {UINT, "SysFrameNum/SubFrameNum", 2},   //System/subsystem frame number
    {UINT, "PDU Number", 1},            //PDU number
    {UINT, "SIB Mask in SI", 1},
    {SKIP, NULL, 3},
    {UINT, "Msg Length", 2}
};

const Fmt LteRrcOtaPacketFmt_v13 [] = {
    {UINT, "Freq", 4},                  //frequency
    {UINT, "SysFrameNum/SubFrameNum", 2},   //System/subsystem frame number
    {UINT, "PDU Number", 1},            //PDU number
    {UINT, "SIB Mask in SI", 1},
    {SKIP, NULL, 3},
    {UINT, "Msg Length", 2}
};

const Fmt LteRrcOtaPacketFmt_v15 [] = {
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
    {0x08, "LTE-RRC_UL_DCCH"},
};

const ValueName LteRrcOtaPduType_v15 [] = {
    {0x02, "LTE-RRC_BCCH_DL_SCH"},
    {0x05, "LTE-RRC_PCCH"},
    {0x06, "LTE-RRC_DL_CCCH"},
    {0x07, "LTE-RRC_DL_DCCH"},
    {0x08, "LTE-RRC_UL_CCCH"},
    {0x09, "LTE-RRC_UL_DCCH"},
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

const Fmt LteNasEsmStateFmt [] = {
    {UINT, "Version", 1}
};

const Fmt LteNasEsmStateFmt_v1 [] = {
    {UINT, "EPS bearer type", 1},
    {UINT, "EPS bearer ID", 1},
    {UINT, "EPS bearer state", 1},
    {SKIP, NULL, 6},
    {UINT, "Len", 1},
    {UINT, "QCI", 1},
    {UINT, "UL MBR", 1},
    {UINT, "DL MBR", 1},
    {UINT, "UL GBR", 1},
    {UINT, "DL GBR", 1},
    {UINT, "UL MBR ext", 1},
    {UINT, "DL MBR ext", 1},
    {UINT, "UL GBR ext", 1},
    {UINT, "DL GBR ext", 1},
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
const Fmt LtePhyPdschDemapperConfigFmt [] = {
    {UINT, "Version", 1}
};

const Fmt LtePhyPdschDemapperConfigFmt_v23 [] = {
    {UINT, "Serving Cell ID", 1},
    {UINT, "System Frame Number", 2},
    {PLACEHOLDER, "Subframe Number", 0},
    {UINT, "PDSCH RNTIl ID", 2},
    {PLACEHOLDER, "PDSCH RNTI Type", 0},
    {UINT, "Number of Tx Antennas(M)", 2},
    {PLACEHOLDER, "Number of Rx Antennas(N)", 0},
    {PLACEHOLDER, "Spatial Rank", 0},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[0]", 8},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[1]", 8},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[0]", 8},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[1]", 8},
    {UINT, "Frequency Selective PMI", 1},   // right shift 1 bit, 2 bits
    {PLACEHOLDER, "PMI Index", 0},  // 4 bits
    {UINT, "Transmission Scheme", 1},   // 4 bits
    {SKIP, NULL, 2},
    // {UINT, "Transport Block Size Stream 0", 2},
    {UINT, "TBS 0", 2},
    // {UINT, "Modulation Stream 0", 2},
    {UINT, "MCS 0", 2},
    {PLACEHOLDER, "Traffic to Pilot Ratio", 0},
    // {UINT, "Transport Block Size Stream 1", 2},
    {UINT, "TBS 1", 2},
    // {UINT, "Modulation Stream 1", 2},
    {UINT, "MCS 1", 2},
    {PLACEHOLDER, "Carrier Index", 0}
};

const Fmt LtePhyPdschDemapperConfigFmt_v103 [] = {
    // TODO: just copy from v23.
    {UINT, "Serving Cell ID", 1},
    {UINT, "System Frame Number", 2},
    {PLACEHOLDER, "Subframe Number", 0},
    {UINT, "PDSCH RNTIl ID", 2},
    {PLACEHOLDER, "PDSCH RNTI Type", 0},
    {UINT, "Number of Tx Antennas(M)", 2},
    {PLACEHOLDER, "Number of Rx Antennas(N)", 0},
    {PLACEHOLDER, "Spatial Rank", 0},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[0]", 8},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[1]", 8},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[0]", 8},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[1]", 8},
    {UINT, "Frequency Selective PMI", 1},   // right shift 1 bit, 2 bits
    {PLACEHOLDER, "PMI Index", 0},  // 4 bits
    {UINT, "Transmission Scheme", 1},   // 4 bits
    {SKIP, NULL, 2},
    {UINT, "TBS 0", 2},
    {UINT, "MCS 0", 2},
    {PLACEHOLDER, "Traffic to Pilot Ratio", 0},
    {UINT, "TBS 1", 2},
    {UINT, "MCS 1", 2},
    {PLACEHOLDER, "Carrier Index", 0},
    {SKIP, NULL, 4},
};


const Fmt LtePhyPdschDemapperConfigFmt_v104 [] = {
    // TODO: just copy from v23.
    {UINT, "Serving Cell ID", 1},
    {UINT, "System Frame Number", 2},
    {PLACEHOLDER, "Subframe Number", 0},
    {UINT, "PDSCH RNTIl ID", 2},
    {PLACEHOLDER, "PDSCH RNTI Type", 0},
    {UINT, "Number of Tx Antennas(M)", 2},
    {PLACEHOLDER, "Number of Rx Antennas(N)", 0},
    {PLACEHOLDER, "Spatial Rank", 0},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[0]", 8},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 0[1]", 8},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[0]", 8},
    {BYTE_STREAM_LITTLE_ENDIAN, "RB Allocation Slot 1[1]", 8},
    {UINT, "Frequency Selective PMI", 1},   // right shift 1 bit, 2 bits
    {PLACEHOLDER, "PMI Index", 0},  // 4 bits
    {UINT, "Transmission Scheme", 1},   // 4 bits
    {UINT, "TBS 0", 2},
    {UINT, "MCS 0", 2},
    {PLACEHOLDER, "Traffic to Pilot Ratio", 0},
    {UINT, "TBS 1", 2},
    {UINT, "MCS 1", 2},
    {PLACEHOLDER, "Carrier Index", 0},
    {SKIP, NULL, 4},
};

const ValueName LtePhyPdschDemapperConfig_v23_Modulation [] = {
    {0, "QPSK"},
    {1, "16QAM"},
    {2, "64QAM"}
};

const ValueName LtePhyPdschDemapperConfig_v23_Carrier_Index [] = {
    {0, "PCC"},
    {1, "SCC"}
};

// ------------------------------------------------------------
// LTE_PHY_Connected_Mode_LTE_Intra_Freq_Meas_Results
const Fmt LtePhyCmlifmrFmt [] = {
    {UINT, "Version", 1},
    {SKIP, NULL, 3},        // Unknown
    {UINT, "Serving Cell Index", 1},    // 3 bits
    {SKIP, NULL, 3},        // Unknown
};

const Fmt LtePhyCmlifmrFmt_v3_Header [] = {
    {UINT, "E-ARFCN", 2},
    {UINT, "Serving Physical Cell ID", 2},  //serving cell ID
    {UINT, "Sub-frame Number", 2},
    {RSRP, "RSRP(dBm)", 2}, //Filtered RSRP (dBm)
    {SKIP, NULL, 2},    // Duplicated
    {RSRQ, "RSRQ(dB)", 2}, //Filtered RSRQ (dBm)
    {SKIP, NULL, 2},    // Duplicated
    {UINT, "Number of Neighbor Cells", 1},
    {UINT, "Number of Detected Cells", 1}
};

const Fmt LtePhyCmlifmrFmt_v4_Header [] = {
    {UINT, "E-ARFCN", 4},
    {UINT, "Serving Physical Cell ID", 2},  //serving cell ID
    {UINT, "Sub-frame Number", 2},
    // {RSRP, "Serving Filtered RSRP(dBm)", 2}, //Filtered RSRP (dBm)
    {RSRP, "RSRP(dBm)", 2}, //Filtered RSRP (dBm)
    {SKIP, NULL, 2},    // Duplicated
    // {RSRQ, "Serving Filtered RSRQ(dB)", 2}, //Filtered RSRQ (dBm)
    {RSRQ, "RSRQ(dB)", 2}, //Filtered RSRQ (dBm)
    {SKIP, NULL, 2},    // Duplicated
    {UINT, "Number of Neighbor Cells", 1},
    {UINT, "Number of Detected Cells", 1},
    {SKIP, NULL, 2}
};

const Fmt LtePhyCmlifmrFmt_v3_Neighbor_Cell [] = {
    {UINT, "Physical Cell ID", 2},  //cell ID
    // {RSRP, "Filtered RSRP(dBm)", 2},
    {RSRP, "RSRP(dBm)", 2},
    {SKIP, NULL, 2},    // Duplicated
    // {RSRQ, "Filtered RSRQ(dB)", 2},
    {RSRQ, "RSRQ(dB)", 2},
    {SKIP, NULL, 4}     // Duplicated & reserved
};

const Fmt LtePhyCmlifmrFmt_v4_Neighbor_Cell [] = {
    {UINT, "Physical Cell ID", 2},  //cell ID
    // {RSRP, "Filtered RSRP(dBm)", 2},
    {RSRP, "RSRP(dBm)", 2},
    {SKIP, NULL, 2},    // Duplicated
    // {RSRQ, "Filtered RSRQ(dB)", 2},
    {RSRQ, "RSRQ(dB)", 2},
    {SKIP, NULL, 4}     // Duplicated & reserved
};

const Fmt LtePhyCmlifmrFmt_v3_Detected_Cell [] = {
    {UINT, "Physical Cell ID", 4},  //cell ID
    {UINT, "SSS Corr Value", 4},
    {UINT, "Reference Time", 8}
};

const Fmt LtePhyCmlifmrFmt_v4_Detected_Cell [] = {
    {UINT, "Physical Cell ID", 2},  //cell ID
    {SKIP, NULL, 2},
    {UINT, "SSS Corr Value", 4},
    {UINT, "Reference Time", 8}
};

// ------------------------------------------------------------
const Fmt LtePhySubpktFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Number of SubPackets", 1},
    {SKIP, NULL, 2}     // Unknown
};

const Fmt LtePhySubpktFmt_v1_SubpktHeader [] = {
    {UINT, "SubPacket ID", 1},
    {UINT, "SubPacket Version", 1},
    {UINT, "SubPacket Size", 2},
};

const ValueName LtePhySubpkt_SubpktType [] = {
    {25, "Serving_Cell_Measurement_Result"}
};

// Serving_Cell_Measurement_Result
const Fmt LtePhySubpktFmt_v1_Scmr_v4 [] = {
    {UINT, "E-ARFCN", 2},
    {UINT, "Physical Cell ID", 2},  // 9 bits
    {PLACEHOLDER, "Serving Cell Index", 0},    // 3 bits
    {UINT, "Current SFN", 2},   // 10 bits
    {PLACEHOLDER, "Current Subframe Number", 0},    // 4 bits
    {SKIP, NULL, 2},
    {SKIP, NULL, 4},    // Is Restricted, Cell Timing [0]
    {UINT, "RSRP Rx[0]", 4},    // skip 10 bits, then 12 bits. (0.0625 * x - 180) dBm
    {UINT, "RSRP Rx[1]", 4},    // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
    {UINT, "RSRP", 4},          // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
    {UINT, "RSRQ Rx[0]", 4},    // skip 12 bits, then 10 bits, (0.0625 * x - 30) dB
    {UINT, "RSRQ Rx[1]", 4},    // 10 bits, (0.0625 * x) - 30 dB
    {PLACEHOLDER, "RSRQ", 0},   // skip 20 bits, then 10 bits, (0.0625 * x - 30) dB
    {UINT, "RSSI Rx[0]", 4},    // skip 10 bits, them 11 bits (0.0625 * x - 110) dBm
    {PLACEHOLDER, "RSSI Rx[1]", 0}, // skip 21 bits, then 11 bits (0.0625 * x - 110) dBm
    {UINT, "RSSI", 4},    // 11 bits, (0.0625 * x - 110) dBm
    {SKIP, NULL, 20},
    {UINT, "FTL SNR Rx[0]", 4}, // 9 bits
    {PLACEHOLDER, "FTL SNR Rx[1]", 0},  // skip 9 bits, then 9 bits (0.1 * x - 20) dB
    {SKIP, NULL, 12},
};

const Fmt LtePhySubpktFmt_v1_Scmr_v7 [] = {
    {UINT, "E-ARFCN", 4},
    {UINT, "Physical Cell ID", 2},  // 9 bits
    {PLACEHOLDER, "Serving Cell Index", 0},    // 3 bits
    {SKIP, NULL, 2},
    {UINT, "Current SFN", 2},   // 10 bits
    {PLACEHOLDER, "Current Subframe Number", 0},    // 4 bits
    {SKIP, NULL, 2},
    {SKIP, NULL, 4},    // Is Restricted, Cell Timing [0]
    {UINT, "RSRP Rx[0]", 4},    // skip 10 bits, then 12 bits. (0.0625 * x - 180) dBm
    {UINT, "RSRP Rx[1]", 4},    // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
    {UINT, "RSRP", 4},          // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
    {UINT, "RSRQ Rx[0]", 4},    // skip 12 bits, then 10 bits, (0.0625 * x - 30) dB
    {UINT, "RSRQ Rx[1]", 4},    // 10 bits, (0.0625 * x) - 30 dB
    {PLACEHOLDER, "RSRQ", 0},   // skip 20 bits, then 10 bits, (0.0625 * x - 30) dB
    {UINT, "RSSI Rx[0]", 4},    // skip 10 bits, them 11 bits (0.0625 * x - 110) dBm
    {PLACEHOLDER, "RSSI Rx[1]", 0}, // skip 21 bits, then 11 bits (0.0625 * x - 110) dBm
    {UINT, "RSSI", 4},    // 11 bits, (0.0625 * x - 110) dBm
    {SKIP, NULL, 20},
    {UINT, "FTL SNR Rx[0]", 4}, // 9 bits
    {PLACEHOLDER, "FTL SNR Rx[1]", 0},  // skip 9 bits, then 9 bits (0.1 * x - 20) dB
    {SKIP, NULL, 12},
};

const Fmt LtePhySubpktFmt_v1_Scmr_v19 [] = {
    {UINT, "E-ARFCN", 4},
    {UINT, "Num-of-cells", 2},
    {SKIP, NULL, 2},
    {UINT, "Physical Cell ID", 2},  // 9 bits
    {PLACEHOLDER, "Serving Cell Index", 0},    // 3 bits
    {PLACEHOLDER, "Is Serving Cell", 0},    // 1 bit
    {SKIP, NULL, 2},
    {UINT, "Current SFN", 2},   // 10 bits
    {PLACEHOLDER, "Current Subframe Number", 0},    // 4 bits
    {SKIP, NULL, 2},
    {SKIP, NULL, 4},    // Is Restricted, Cell Timing [0]
    {SKIP, NULL, 4},    // Cell Timing [1], Cell Timing SFN[0]
    {UINT, "RSRP Rx[0]", 4},    // skip 10 bits, then 12 bits. (0.0625 * x - 180) dBm
    {UINT, "RSRP Rx[1]", 4},    // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
    {UINT, "RSRP", 4},    // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
    {UINT, "RSRQ Rx[0]", 4},    // skip 12 bits, then 10 bits, (0.0625 * x - 30) dB
    {UINT, "RSRQ Rx[1]", 4},    // 10 bits, (0.0625 * x) - 30 dB
    {PLACEHOLDER, "RSRQ", 0},   // skip 20 bits, then 10 bits, (0.0625 * x - 30) dB
    {UINT, "RSSI Rx[0]", 4},    // skip 10 bits, them 11 bits (0.0625 * x - 110) dBm
    {PLACEHOLDER, "RSSI Rx[1]", 0}, // skip 21 bits, then 11 bits (0.0625 * x - 110) dBm
    {UINT, "RSSI", 4},    // 11 bits, (0.0625 * x - 110) dBm
    {SKIP, NULL, 20},
    {UINT, "FTL SNR Rx[0]", 4}, // 9 bits
    {PLACEHOLDER, "FTL SNR Rx[1]", 0},  // skip 9 bits, then 9 bits (0.1 * x - 20) dB
    {SKIP, NULL, 12},
    {UINT, "Projected SIR", 4}, // Projected Sir, if x & 1<<31: x -= 4294967296
                                // x /= 16
    {UINT, "Post IC RSRQ", 4},  // (0.0625 * x - 30) dB
};

const Fmt LtePhySubpktFmt_v1_Scmr_v22 [] = {
    {UINT, "E-ARFCN", 4},
    {UINT, "Num-of-cells", 2},
    {SKIP, NULL, 2},
    {UINT, "Physical Cell ID", 2},  // 9 bits
    {PLACEHOLDER, "Serving Cell Index", 0},    // 3 bits
    {PLACEHOLDER, "Is Serving Cell", 0},    // 1 bit
    {SKIP, NULL, 2},
    {UINT, "Current SFN", 2},   // 10 bits
    {PLACEHOLDER, "Current Subframe Number", 0},    // 4 bits
    {SKIP, NULL, 2},
    {SKIP, NULL, 4},    // Cell Timing [0]
    {SKIP, NULL, 4},    // Cell Timing [1], Cell Timing SFN [0]
    {UINT, "RSRP Rx[0]", 4},    // skip 10 bits, then 12 bits. (0.0625 * x - 180) dBm
    {UINT, "RSRP Rx[1]", 4},    // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
    {SKIP, NULL, 4},
    {UINT, "RSRP", 4},    // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
    {UINT, "RSRQ Rx[0]", 4},    // skip 12 bits, then 10 bits, (0.0625 * x - 30) dB
    {UINT, "RSRQ Rx[1]", 4},    // 10 bits, (0.0625 * x) - 30 dB
    {UINT, "RSRQ", 4},    // skip 10 bits, then 10 bits, (0.0625 * x - 30) dB
    {UINT, "RSSI Rx[0]", 4},    // skip 10 bits, them 11 bits (0.0625 * x - 110) dBm
    {PLACEHOLDER, "RSSI Rx[1]", 0}, // skip 21 bits, then 11 bits (0.0625 * x - 110) dBm
    {SKIP, NULL, 4},
    {UINT, "RSSI", 4},    // then 11 bits, (0.0625 * x - 110) dBm
    {SKIP, NULL, 20},
    {UINT, "FTL SNR Rx[0]", 4}, // 9 bits
    {PLACEHOLDER, "FTL SNR Rx[1]", 0},  // skip 9 bits, then 9 bits (0.1 * x - 20) dB
    {SKIP, NULL, 16},
    {UINT, "Projected SIR", 4}, // Projected Sir, if x & 1<<31: x -= 4294967296
                                // x /= 16
    {UINT, "Post IC RSRQ", 4},  // (0.0625 * x - 30) dB
    {UINT, "CINR Rx[0]", 4},
    {UINT, "CINR Rx[1]", 4},
};

const Fmt LtePhySubpktFmt_v1_Scmr_v35 [] = {
    {UINT, "E-ARFCN", 4},
    {UINT, "Num-of-cells", 2},
    {SKIP, NULL, 2},
    {UINT, "Physical Cell ID", 2},  // 9 bits
    {PLACEHOLDER, "Serving Cell Index", 0},    // 3 bits
    {PLACEHOLDER, "Is Serving Cell", 0},    // 1 bit
    {SKIP, NULL, 2},
    {UINT, "Current SFN", 2},   // 10 bits
    {PLACEHOLDER, "Current Subframe Number", 0},    // 4 bits
    {SKIP, NULL, 2},
    {SKIP, NULL, 4},    // Cell Timing [0]
    {SKIP, NULL, 4},    // Cell Timing [1], Cell Timing SFN [0]
    {UINT, "RSRP Rx[0]", 4},    // skip 10 bits, then 12 bits. (0.0625 * x - 180) dBm
    {UINT, "RSRP Rx[1]", 4},    // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
    {UINT, "RSRP", 4},    // 12 bits (0.0625 * x - 180) dBm
    {UINT, "RSRQ Rx[0]", 4},    // 10 bits, (0.0625 * x - 30) dB
    {PLACEHOLDER, "RSRQ Rx[1]", 0},    // skip 20 bits, then 10 bits, (0.0625 * x - 30) dB
    {SKIP, NULL, 4},
    {UINT, "RSRQ", 4},    // 10 bits, (0.0625 * x - 30) dB
    // Unknown
};

// ------------------------------------------------------------
// LTE PHY IRAT MDB

enum LtePhyIratType {
    LtePhyIratType_HRPD=14,
    LtePhyIratType_WCDMA=35,
    LtePhyIratType_1x=41,
    LtePhyIratType_GSM=42,
};


const Fmt LtePhyIratFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Subpacket count", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePhyIratSubPktFmt [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Version", 1},
    {UINT, "Subpacket size", 2},
};

const Fmt LtePhyIratWCDMAFmt [] = {
    {UINT, "Current DRX cycle", 4},
    {UINT, "Number of frequencies", 1},
    {SKIP, NULL, 3},
};

const Fmt LtePhyIratWCDMACellMetaFmt [] = {
    {UINT, "Frequency", 2},
    {UINT, "Number of cells", 1},
    {SKIP, NULL, 1},
};

const Fmt LtePhyIratWCDMACellFmt [] = {
    {UINT, "PSC+Energy", 2},
    {UINT, "CSG", 1},
    {SKIP, NULL, 1},
    {UINT, "Pn Pos", 4},
    {WCDMA_MEAS, "RSCP",1},
    {WCDMA_MEAS, "RSSI",1},
    {WCDMA_MEAS, "EcNo",1},
    {UINT,"SrxLev",1},
    {UINT,"DRX cycle count since last measurement",2},
    {UINT,"Treselection",1},
    {UINT,"Squal",1}

};

const Fmt LtePhyIratCDMACellFmt [] = {
    {UINT, "Number of Pilots", 1},
    {UINT, "Band", 1},
    {UINT, "Channel", 2},
};

const Fmt LtePhyIratCDMACellPilotFmt [] = {
    {UINT, "Pilot ID", 2},
    {UINT, "RSS (dB)", 2},
    {SKIP, NULL, 4},
    {UINT, "EcNo", 2},
    {SKIP, NULL, 6},
};




// ------------------------------------------------------------
// LTE RRC Serving Cell Info Log Pkt
const Fmt LteRrcServCellInfoLogPacketFmt [] = {
    {UINT, "Version", 1},
};

const Fmt LteRrcServCellInfoLogPacketFmt_v2 [] = {
    {UINT, "Cell ID", 2},  //Physical cell ID
    {UINT, "Downlink frequency", 2},   //Downlink frequency
    {UINT, "Uplink frequency", 2},   //Uplink frequency
    {BANDWIDTH, "Downlink bandwidth", 1},    //Downlink bandwidth
    {BANDWIDTH, "Uplink bandwidth", 1},    //Uplink bandwidth
    {UINT, "Cell Identity", 4}, //cell ID
    {UINT, "TAC", 2},   //Tracking area code
    {UINT, "Band Indicator", 4},    //Band indicator
    {UINT, "MCC", 2},   //MCC
    {UINT, "MNC Digit", 1}, //MNC digit
    {UINT, "MNC", 2},   //MNC
    {UINT, "Allowed Access", 1} //Allowed access
};

const Fmt LteRrcServCellInfoLogPacketFmt_v3 [] = {
    {UINT, "Cell ID", 2},  //Physical cell ID
    {UINT, "Downlink frequency", 4},   //Downlink frequency
    {UINT, "Uplink frequency", 4},   //Uplink frequency
    {BANDWIDTH, "Downlink bandwidth", 1},    //Downlink bandwidth
    {BANDWIDTH, "Uplink bandwidth", 1},    //Uplink bandwidth
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
// Haotian
const Fmt LtePdcpDlSrbIntegrityDataPduFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num SubPkt", 1},
    {SKIP, NULL, 44},
    {UINT, "PDU Size", 2}, // 47-48
    {SKIP, NULL, 16}
};

// ----------------------------------------------------------------------------
// Haotian
const Fmt LtePdcpUlSrbIntegrityDataPduFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num SubPkt", 1},
    {SKIP, NULL, 44},
    {UINT, "PDU Size", 2}, // 47-48
    {SKIP, NULL, 12}
};

// ----------------------------------------------------------------------------
// MAC_Configuration
// Jie
const ValueName LteMacConfigurationSubpkt_SubpktType [] = {
    {0, "Config Type Subpacket"},
    {1, "DL Config SubPacket"},
    {2, "UL Config SubPacket"},
    {3, "RACH Config SubPacket"},
    {4, "LC Config SubPacket"},
    {7, "DL Transport Block"},
    {8, "UL Transport Block"},
    {10, "UL Buffer Status SubPacket"},
    {11, "UL Tx Statistics SubPacket"},
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
    {UINT, "Num eMBMS Active LCs", 2}, // Not sure if this offset and length of this field is correct
    {SKIP, "NULL", 98}
};

// ----------------------------------------------------------
// MAC UL Transport Block
// Jie
const ValueName BSREvent [] = {
    {0, "None"},
    {1, "Periodic"},
    {2, "High Data Arrival"},
    {3, "Robustness BSR"},
};

const ValueName BSRTrig [] = {
    {0, "No BSR"},
    {1, "Cancelled"},
    {2, "L-BSR"},
    {3, "S-BSR"},
    {4, "Pad L-BSR"},
    {5, "Pad S-BSR"},
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
};

const Fmt LteMacULTransportBlock_SubpktV1_SampleFmt [] = {
    {UINT, "HARQ ID", 1},
    {UINT, "RNTI Type", 1},
    {UINT, "Sub-FN", 2},    // 4 bits
    {PLACEHOLDER, "SFN", 0},
    {UINT, "Grant (bytes)", 2},
    {UINT, "RLC PDUs", 1},
    {UINT, "Padding (bytes)", 2},
    {UINT, "BSR event", 1},
    {UINT, "BSR trig", 1},
    {UINT, "HDR LEN", 1},
    // Mac Hdr + CE and UL TB Other Structure
};

const Fmt LteMacULTransportBlock_SubpktV2_SampleFmt [] = {
    {UINT, "Sub Id", 1},
    {UINT, "Cell Id", 1},
    {UINT, "HARQ ID", 1},
    {UINT, "RNTI Type", 1},
    {UINT, "Sub-FN", 2},    // 4 bits
    {PLACEHOLDER, "SFN", 0},
    {UINT, "Grant (bytes)", 2},
    {UINT, "RLC PDUs", 1},
    {UINT, "Padding (bytes)", 2},
    {UINT, "BSR event", 1},
    {UINT, "BSR trig", 1},
    {UINT, "HDR LEN", 1},
    // Mac Hdr + CE and UL TB Other Structure
};

// ----------------------------------------------------------
// MAC DL Transport Block
// Jie
const ValueName RNTIType [] = {
    {0, "C-RNTI"},
    {2, "P-RNTI"},
    {3, "RA-RNTI"},
    {4, "Temporary-C-RNTI"},
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
    {UINT, "Sub-FN", 2},
    {PLACEHOLDER, "SFN", 0},
    {UINT, "RNTI Type", 1},
    {UINT, "HARQ ID", 1},
    {PLACEHOLDER, "Area ID", 0},
    {UINT, "PMCH ID", 2},
    {UINT, "DL TBS (bytes)", 2},
    {UINT, "RLC PDUs", 1},
    {UINT, "Padding (bytes)", 2},
    {UINT, "HDR LEN", 1},
    // Mac Hdr + CE and UL TB Other Structure
};

const Fmt LteMacDLTransportBlock_SubpktV4_SampleFmt [] = {
    {UINT, "Sub Id", 1},
    {UINT, "Cell Id", 1},
    {UINT, "Sub-FN", 2},
    {PLACEHOLDER, "SFN", 0},
    {UINT, "RNTI Type", 1},
    {UINT, "HARQ ID", 1},
    {PLACEHOLDER, "Area ID", 0},
    {UINT, "PMCH ID", 2},
    {UINT, "DL TBS (bytes)", 2},
    {UINT, "RLC PDUs", 1},
    {UINT, "Padding (bytes)", 2},
    {UINT, "HDR LEN", 1},
    // Mac Hdr + CE and UL TB Other Structure
};

// ----------------------------------------------------------
// LTE Mac UL Tx Statistics
// Jie
const Fmt LteMacULTxStatisticsFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num SubPkt", 1},
    {SKIP, NULL, 2}
};

const Fmt LteMacULTxStatistics_SubpktHeaderFmt [] = {
    {UINT, "SubPacket ID", 1},
    {UINT, "Version", 1},
    {UINT, "SubPacket Size", 2},
};

const Fmt LteMacULTxStatistics_ULTxStatsSubPacketFmt [] = {
    {UINT, "Number of samples", 1},
    {UINT, "Number of padding BSR", 1},
    {UINT, "Number of regular BSR", 1},
    {UINT, "Number of periodic BSR", 1},
    {UINT, "Number of cancel BSR", 1},
    {UINT, "Grant received", 4},
    {UINT, "Grant utilized", 4},
    {SKIP, NULL, 3}
};

const Fmt LteMacULTxStatistics_ULTxStatsSubPacketFmtV2 [] = {
    {UINT, "Sub Id", 1},
    {UINT, "Number of samples", 1},
    {UINT, "Number of padding BSR", 1},
    {UINT, "Number of regular BSR", 1},
    {UINT, "Number of periodic BSR", 1},
    {UINT, "Number of cancel BSR", 1},
    {UINT, "Grant received", 4},
    {UINT, "Grant utilized", 4},
    {SKIP, NULL, 2}
};

// ----------------------------------------------------------
// LTE MAC UL Buffer Status Internal
// Jie
const Fmt LteMacULBufferStatusInternalFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num SubPkt", 1},
    {SKIP, NULL, 2}
};

const Fmt LteMacULBufferStatusInternal_SubpktHeaderFmt [] = {
    {UINT, "SubPacket ID", 1},
    {UINT, "Version", 1},
    {UINT, "SubPacket Size", 2},
    {UINT, "Num Samples", 1}
};

const Fmt LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt [] = {
    {UINT, "Sub FN", 2},
    {PLACEHOLDER, "Sys FN", 0},
    {UINT, "Number of active LCID", 1}
};

const Fmt LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt [] = {
    {UINT, "Ld Id", 1},
    {UINT, "Priority", 1},
    {UINT, "New bytes", 4},
    {UINT, "Retx bytes", 4},
    {UINT, "Ctrl bytes", 2},
};

const Fmt LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt_v24 [] = {
    {UINT, "Sub Id", 1},
    {UINT, "Sub FN", 2},
    {PLACEHOLDER, "Sys FN", 0},
    {UINT, "Number of active LCID", 1}
};

const Fmt LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt_v24 [] = {
    {UINT, "Ld Id", 1},
    {UINT, "Priority", 1},
    {UINT, "New Uncompressed Bytes", 4},
    {UINT, "New Compressed Bytes", 4},
    {UINT, "Retx bytes", 4},
    {UINT, "Ctrl bytes", 2},
    {PLACEHOLDER, "Total Bytes", 0},
};

// ----------------------------------------------------------------------------
// LTE_RLC_UL_Config_Log_Packet

const Fmt LteRlcUlConfigLogPacketFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num SubPkt", 1},
    {SKIP, NULL, 2}, // reserved
};

const Fmt LteRlcUlConfigLogPacket_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LteRlcUlConfigLogPacket_SubpktPayload [] = {
    {UINT, "Reason", 1},
    {UINT, "Max Size RBs", 1},
};

const ValueName LteRlcUlConfigLogPacket_Subpkt_Reason [] = {
    {1, "Configuration"},
    {4, "RB Release"},
};

const Fmt LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Header [] = {
    {UINT, "Number of Released RBs", 1},
};
const Fmt LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Fmt [] = {
    {UINT, "Released RB Cfg Index", 1},
};

const Fmt LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Header [] = {
    {UINT, "Number of Added/Modified RBs", 1},
};
const Fmt LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt [] = {
    {UINT, "Added/Modified RB Cfg Index", 1},
    {UINT, "Action", 1},
};
const ValueName LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Action [] = {
    {1, "Add"},
    {2, "Modify"},
};

const Fmt LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Header [] = {
    {UINT, "Number of Active RBs", 1},
};
const Fmt LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Fmt [] = {
    {PLACEHOLDER, "RLCUL CFG", 0},
    {UINT, "RB Mode", 1},
    {UINT, "LC ID", 1},
    {UINT, "RB ID", 1},
    {UINT, "RB Cfg Idx", 1},
    {UINT, "RB Type", 1},
    {PLACEHOLDER, "SN Length", 0},
    {UINT, "Poll Byte", 4},
    {UINT, "Poll PDU", 2},
    {UINT, "T Poll Retx (ms)", 2},
    {UINT, "Max Retx Threshold", 1},
};
const ValueName LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBMode [] = {
    {1, "AM"},
};
const ValueName LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBType [] = {
    {1, "SRB"},
    {2, "DRB"},
};

// ----------------------------------------------------------------------------
// LTE_RLC_DL_Config_Log_Packet

const Fmt LteRlcDlConfigLogPacketFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num SubPkt", 1},
    {SKIP, NULL, 2}, // reserved
};

const Fmt LteRlcDlConfigLogPacket_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LteRlcDlConfigLogPacket_SubpktPayload [] = {
    {UINT, "Reason", 1},
    {UINT, "Max Size RBs", 1},
};

const ValueName LteRlcDlConfigLogPacket_Subpkt_Reason [] = {
    {1, "Configuration"},
    {4, "RB Release"},
};

const Fmt LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Header [] = {
    {UINT, "Number of Released RBs", 1},
};
const Fmt LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Fmt [] = {
    {UINT, "Released RB Cfg Index", 1},
};

const Fmt LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Header [] = {
    {UINT, "Number of Added/Modified RBs", 1},
};
const Fmt LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt [] = {
    {UINT, "Added/Modified RB Cfg Index", 1},
    {UINT, "Action", 1},
};
const ValueName LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Action [] = {
    {1, "Add"},
    {2, "Modify"},
};

const Fmt LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Header [] = {
    {UINT, "Number of Active RBs", 1},
};
const Fmt LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Fmt [] = {
    {PLACEHOLDER, "RLCDL CFG", 0},
    {UINT, "RB Mode", 1},
    {UINT, "LC ID", 1},
    {UINT, "RB ID", 1},
    {UINT, "RB Cfg Idx", 1},
    {UINT, "RB Type", 1},
    {UINT, "T Reordering (ms)", 2},
    {PLACEHOLDER, "SN Length", 0},
    {UINT, "T Status Prohibit (ms)", 2},
};
const ValueName LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBMode [] = {
    {1, "AM"},
};
const ValueName LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBType [] = {
    {1, "SRB"},
    {2, "DRB"},
};

// ----------------------------------------------------------------------------
// LTE_RLC_UL_AM_All_PDU

const Fmt LteRlcUlAmAllPduFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Number of Subpackets", 1},
    {SKIP, NULL, 2},
};

const Fmt LteRlcUlAmAllPdu_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LteRlcUlAmAllPdu_SubpktPayload [] = {
    {UINT, "RB Cfg Idx", 1},
    {UINT, "RB Mode", 1},
    {UINT, "SN Length", 1},
    {SKIP, NULL, 1},
    {UINT, "Enabled PDU Log Packets", 2}, // need to check bit by bit
    // this part is different from DL
    {UINT, "VT(A)", 2},
    {UINT, "VT(S)", 2},
    {UINT, "PDU Without Poll", 2},
    {UINT, "Byte Without Poll", 4},
    {UINT, "Poll SN", 2},
    {UINT, "Number of PDUs", 2},
};
const ValueName LteRlcUlAmAllPdu_Subpkt_RBMode [] = {
    {1, "AM"},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_Basic [] = {
    {PLACEHOLDER, "PDU TYPE", 0},
    {PLACEHOLDER, "rb_cfg_idx", 0},
    {UINT, "sys_fn", 2},
    {PLACEHOLDER, "sub_fn", 0},
    {UINT, "pdu_bytes", 2},
    {UINT, "logged_bytes", 2},
    {SKIP, NULL, 1},
    {UINT, "D/C LookAhead", 1},
    {UINT, "SN", 1},
    // for control: cpt = STATUS(0)
    // for data: RF, P, FI, E
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_Control [] = {
    {PLACEHOLDER, "cpt", 0},
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_NACK_ALLIGN [] = {
    {UINT, "NACK_SN", 2},
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_NACK_PADDING [] = {
    {UINT, "NACK_SN", 1},
};

const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_DATA [] = {
    {PLACEHOLDER, "RF", 0},
    {PLACEHOLDER, "P", 0},
    {PLACEHOLDER, "FI", 0},
    {PLACEHOLDER, "E", 0},
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_LI_ALLIGN [] = {
    {UINT, "LI", 2}, // 0x 0011 1100
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_LI_PADDING [] = {
    {UINT, "LI", 1}, //
};
const Fmt LteRlcUlAmAllPdu_Subpkt_PDU_LSF_SO [] = {
    {UINT, "LSF", 1},   // & 128
    {UINT, "SO", 1},    // the rest 7 bits of LSF (& 127) * 256 + this value
};

// ----------------------------------------------------------------------------
// LTE_RLC_DL_AM_All_PDU

const Fmt LteRlcDlAmAllPduFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Number of Subpackets", 1},
    {SKIP, NULL, 2},
};

const Fmt LteRlcDlAmAllPdu_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LteRlcDlAmAllPdu_SubpktPayload [] = {
    {UINT, "RB Cfg Idx", 1},
    {UINT, "RB Mode", 1},
    {UINT, "SN Length", 1},
    {SKIP, NULL, 1},
    {UINT, "Enabled PDU Log Packets", 2}, // need to check bit by bit
    // this part is different from DL
    {UINT, "VR(R)", 2},
    {UINT, "VR(X)", 2},
    {UINT, "VR(MS)", 2},
    {UINT, "VR(H)", 2},
    {UINT, "Number of PDUs", 2},
};
const ValueName LteRlcDlAmAllPdu_Subpkt_RBMode [] = {
    {1, "AM"},
};

const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_Basic [] = {
    {PLACEHOLDER, "PDU TYPE", 0},
    {PLACEHOLDER, "rb_cfg_idx", 0},
    {PLACEHOLDER, "Status", 0},
    {UINT, "sys_fn", 2},
    {PLACEHOLDER, "sub_fn", 0},
    {UINT, "pdu_bytes", 2},
    {UINT, "logged_bytes", 2},
    {SKIP, NULL, 1},
    {UINT, "D/C LookAhead", 1},
    {UINT, "SN", 1},
    // for control: cpt = STATUS(0)
    // for data: RF, P, FI, E
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_Control [] = {
    {PLACEHOLDER, "cpt", 0},
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_NACK_ALLIGN [] = {
    {UINT, "NACK_SN", 2},
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_NACK_PADDING [] = {
    {UINT, "NACK_SN", 1},
};

const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_DATA [] = {
    {PLACEHOLDER, "RF", 0},
    {PLACEHOLDER, "P", 0},
    {PLACEHOLDER, "FI", 0},
    {PLACEHOLDER, "E", 0},
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_LI_ALLIGN [] = {
    {UINT, "LI", 2}, // 0x 0011 1100
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_LI_PADDING [] = {
    {UINT, "LI", 1}, //
};
const Fmt LteRlcDlAmAllPdu_Subpkt_PDU_LSF_SO [] = {
    {UINT, "LSF", 1},   // & 128
    {UINT, "SO", 1},    // the rest 7 bits of LSF (& 127) * 256 + this value
};

// ----------------------------------------------------------------------------
// LTE_MAC_Rach_Trigger
const Fmt LteMacRachTriggerFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Number of Subpackets", 1},
    {SKIP, NULL, 2},
};

const Fmt LteMacRachTrigger_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LteMacRachTrigger_RachConfigSubpktPayload [] = {
    // Version 2
    {WCDMA_MEAS, "Preamble initial power (dB)", 1}, // Note sure if it is correct
    {SKIP, NULL, 1},
    {UINT, "Power ramping step (dB)", 1},
    {UINT, "RA index1", 1},
    {UINT, "RA index2", 1},
    {UINT, "Preamble trans max", 1},
    {UINT, "Contention resolution timer (ms)", 2},
    {UINT, "Message size Group_A", 2},
    {UINT, "Power offset Group_B", 1},
    {UINT, "PMax (dBm)", 2},
    {UINT, "Delta preamble Msg3", 2},
    {UINT, "PRACH config", 1},
    {UINT, "CS zone length", 1},
    {UINT, "Root seq index", 2},
    {UINT, "PRACH Freq Offset", 3},
    {PLACEHOLDER, "Preamble Format", 0},
    {UINT, "High speed flag", 1},
    {UINT, "Max retx Msg3", 1},
    {UINT, "RA rsp win size", 1},
};

const Fmt LteMacRachTrigger_RachReasonSubpktPayload [] = {
    // Version 1
    {UINT, "Rach reason", 1},
    {PLACEHOLDER, "RACH Contention", 0},
    {BYTE_STREAM, "Maching ID", 6},
    {SKIP, NULL, 1},
    {UINT, "Preamble", 1},
    {BYTE_STREAM, "Preamble RA mask", 1},
    {UINT, "Msg3 size", 1},
    {UINT, "Group chosen", 1},
    {UINT, "Radio condn (dB)", 1},
    {BYTE_STREAM, "CRNTI", 2},
};
const ValueName LteMacRachTrigger_RachReasonSubpkt_RachReason [] = {
    {0, "CONNECTION_REQ"},
};

// ----------------------------------------------------------------------------
// LTE MAC Rach Attempt

const Fmt LteMacRachAttempt_Fmt [] = {
    {UINT, "Version", 1},
    {UINT, "Number of Subpackets", 1},
    {SKIP, NULL, 2},
};

const Fmt LteMacRachAttempt_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LteMacRachAttempt_SubpktPayload[] = {
    // Version 2
    {UINT, "Retx counter", 1},
    {UINT, "Rach result", 1},
    {UINT, "Contention procedure", 1},
    {UINT, "Rach msg bmasks", 1},
};
const ValueName LteMacRachAttempt_Subpkt_RachResult [] = {
    {0, "Success"},
};
const ValueName LteMacRachAttempt_Subpkt_ContentionProcedure [] = {
    {1, "Contention Based RACH procedure"},
};

const Fmt LteMacRachAttempt_Subpkt_Msg1 [] = {
    {UINT, "Preamble Index", 1},
    {BYTE_STREAM, "Preamble index mask", 1},
    {WCDMA_MEAS, "Preamble power offset", 1},
    {SKIP, NULL, 1},
};

const Fmt LteMacRachAttempt_Subpkt_Msg2 [] = {
    {UINT, "Backoff Value (ms)", 2},
    {UINT, "Result", 1},
    {UINT, "TCRNTI", 2},
    {UINT, "TA value", 2},
};
const ValueName LteMacRachAttempt_Subpkt_Msg2_Result [] = {
    {0, "No"},
    {1, "True"},
};

const Fmt LteMacRachAttempt_Subpkt_Msg3 [] = {
    {SKIP, NULL, 1},
    {BYTE_STREAM, "Grant Raw", 3},
    {UINT, "Grant", 2},
    {UINT, "Harq ID", 1},
};
const Fmt LteMacRachAttempt_Subpkt_Msg3_MACPDU [] = {
    {BYTE_STREAM, "MAC PDU", 1},
};

// ----------------------------------------------------------------------------
// LTE PDCP DL Config

const Fmt LtePdcpDlConfig_Fmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num Subpkt", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdcpDlConfig_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LtePdcpDlConfig_SubpktPayload [] = {
    {UINT, "Reason", 1},
    {UINT, "SRB Cipher Algorithm", 1},
    {UINT, "SRB Cipher Key Idx", 1},
    {UINT, "SRB Integrity Algorithm", 1},
    {UINT, "SRB Integrity Key Idx", 1},
    {UINT, "DRB Cipher Algorithm", 1},
    {UINT, "DRB Cipher Key Idx", 1},
    {UINT, "Array size", 1},
};
const ValueName LtePdcpDlConfig_Subpkt_Reason [] = {
    {1, "Configuration"},
    {4, "RB Release"},
};
const ValueName LtePdcpDlConfig_Subpkt_CipherAlgo [] = {
    {2, "Snow3G"},
    {7, "None"},
};
const ValueName LtePdcpDlConfig_Subpkt_IntegAlgo [] = {
    {2, "AES"},
    {7, "None"},
};

const Fmt LtePdcpDlConfig_Subpkt_ReleaseRB_Header [] = {
    {UINT, "Number of Released RBs", 1},
};
const Fmt LtePdcpDlConfig_Subpkt_ReleaseRB_Fmt [] = {
    {UINT, "Released RB Cfg Index", 1},
};

const Fmt LtePdcpDlConfig_Subpkt_AddedModifiedRB_Header [] = {
    {UINT, "Number of Added/Modified RBs", 1},
};
const Fmt LtePdcpDlConfig_Subpkt_AddedModifiedRB_Fmt [] = {
    {UINT, "Added/Modified RB Cfg Index", 1},
    {UINT, "Action", 1},
};
const ValueName LtePdcpDlConfig_Subpkt_AddedModifiedRB_Action [] = {
    {1, "Add"},
    {2, "Modify"},
};

const Fmt LtePdcpDlConfig_Subpkt_ActiveRB_Header [] = {
    {UINT, "Number of active RBs", 1},
};
const Fmt LtePdcpDlConfig_Subpkt_ActiveRB_Fmt [] = {
    {UINT, "RB ID", 1},
    {UINT, "RB-Cfg Idx", 1},
    {UINT, "EPS ID", 1},
    {UINT, "RB mode", 1},
    {UINT, "RB type", 1},
    {UINT, "SN length", 1},
    {UINT, "Status report", 1},
    {UINT, "RoHC Max CID", 1},
    {UINT, "RoHC Enabled", 1},
    {BYTE_STREAM, "RoHC Mask", 4},
};
const ValueName LtePdcpDlConfig_Subpkt_ActiveRB_RBmode [] = {
    {1, "AM"},
};
const ValueName LtePdcpDlConfig_Subpkt_ActiveRB_RBtype [] = {
    {1, "SRB"},
    {2, "DRB"},
};
const ValueName LtePdcpDlConfig_Subpkt_ActiveRB_StatusReport [] = {
    {0, "NO"},
    {1, "YES"},
};
const ValueName LtePdcpDlConfig_Subpkt_ActiveRB_RoHCEnabled [] = {
    {0, "false"},
    {1, "true"},
};

// ----------------------------------------------------------------------------
// LTE PDCP UL Config

const Fmt LtePdcpUlConfig_Fmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num Subpkt", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdcpUlConfig_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LtePdcpUlConfig_SubpktPayload [] = {
    {UINT, "Reason", 1},
    {UINT, "SRB Cipher Algorithm", 1},
    {UINT, "SRB Cipher Key Idx", 1},
    {UINT, "SRB Integrity Algorithm", 1},
    {UINT, "SRB Integrity Key Idx", 1},
    {UINT, "DRB Cipher Algorithm", 1},
    {UINT, "DRB Cipher Key Idx", 1},
    {UINT, "Array size", 1},
};
const ValueName LtePdcpUlConfig_Subpkt_Reason [] = {
    {1, "Configuration"},
    {4, "RB Release"},
};
const ValueName LtePdcpUlConfig_Subpkt_CipherAlgo [] = {
    {2, "Snow3G"},
    {7, "None"},
};
const ValueName LtePdcpUlConfig_Subpkt_IntegAlgo [] = {
    {2, "AES"},
    {7, "None"},
};

const Fmt LtePdcpUlConfig_Subpkt_ReleaseRB_Header [] = {
    {UINT, "Number of Released RBs", 1},
};
const Fmt LtePdcpUlConfig_Subpkt_ReleaseRB_Fmt [] = {
    {UINT, "Released RB Cfg Index", 1},
};

const Fmt LtePdcpUlConfig_Subpkt_AddedModifiedRB_Header [] = {
    {UINT, "Number of Added/Modified RBs", 1},
};
const Fmt LtePdcpUlConfig_Subpkt_AddedModifiedRB_Fmt [] = {
    {UINT, "Added/Modified RB Cfg Index", 1},
    {UINT, "Action", 1},
};
const ValueName LtePdcpUlConfig_Subpkt_AddedModifiedRB_Action [] = {
    {1, "Add"},
    {2, "Modify"},
};

const Fmt LtePdcpUlConfig_Subpkt_ActiveRB_Header [] = {
    {UINT, "Number of active RBs", 1},
};
const Fmt LtePdcpUlConfig_Subpkt_ActiveRB_Fmt [] = {
    {UINT, "RB ID", 1},
    {UINT, "RB-Cfg Idx", 1},
    {UINT, "EPS ID", 1},
    {UINT, "RB mode", 1},
    {UINT, "RB type", 1},
    {UINT, "SN length", 1},
    {UINT, "Discard timer", 2},
    {UINT, "RoHC Max CID", 1},
    {UINT, "RoHC Enabled", 1},
    {BYTE_STREAM, "RoHC Mask", 4},
};
const ValueName LtePdcpUlConfig_Subpkt_ActiveRB_RBmode [] = {
    {1, "AM"},
};
const ValueName LtePdcpUlConfig_Subpkt_ActiveRB_RBtype [] = {
    {1, "SRB"},
    {2, "DRB"},
};
const ValueName LtePdcpUlConfig_Subpkt_ActiveRB_StatusReport [] = {
    {0, "NO"},
};
const ValueName LtePdcpUlConfig_Subpkt_ActiveRB_RoHCEnabled [] = {
    {0, "false"},
};

// ----------------------------------------------------------------------------
// LTE PDCP UL Data PDU

const Fmt LtePdcpUlDataPdu_Fmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num Subpkt", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdcpUlDataPdu_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LtePdcpUlDataPdu_SubpktPayload [] = {
    {UINT, "RB Cfg Index", 1},
    {UINT, "Mode", 1},
    {UINT, "SN Length (bit)", 1},
};
const ValueName LtePdcpUlDataPdu_Subpkt_Mode [] = {
    {1, "AM"},
};

const Fmt LtePdcpUlDataPdu_Subpkt_PDU_Header [] = {
    {UINT, "Num PDUs", 2},
};
const Fmt LtePdcpUlDataPdu_Subpkt_PDU_Fmt [] = {
    {UINT, "PDU Size", 2},
    {UINT, "Logged Bytes", 2},
    {UINT, "System Frame Number", 2},
    {PLACEHOLDER, "Subframe Number", 0},
    {UINT, "SN", 1},
    {SKIP, NULL, 2},
};

// ----------------------------------------------------------------------------
// LTE PDCP DL Stats

const Fmt LtePdcpDlStats_Fmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num Subpkt", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdcpDlStats_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LtePdcpDlStats_SubpktPayload [] = {
    {UINT, "Num RBs", 1},
    {UINT, "Num Errors", 4},
    {UINT, "Num Offload Q Full Count", 4},
    {UINT, "Num Packet Dropped Offload Q Full", 4},
};

const Fmt LtePdcpDlStats_Subpkt_RB_Fmt [] = {
    {UINT, "Rb Cfg Idx", 1},
    {UINT, "Mode", 1},
    {UINT, "PDCP Hdr Len", 1},
    {UINT, "Num RST", 4},
    {UINT, "Num Flow Ctrl Trigger", 4},
    {UINT, "Num Data PDU Rx", 4},
    {UINT, "Num Data PDU Rx Bytes", 4},
    {UINT, "Control PDU Bytes Rx", 4},
    {SKIP, NULL, 4},
    {UINT, "Num Control PDU Gen", 4},
    {UINT, "Num Control PDU Gen Bytes", 4},
    {UINT, "Num Stat Rprt Ctrl PDU Rx", 4},
    {UINT, "Num ROHC Ctrl PDU Rx", 4},
    {UINT, "Num PDU ROHC Fail", 4},
    {UINT, "Num PDU Integrity Fail", 4},
    {UINT, "Num Missing SDU To UL", 4},
    {UINT, "Num Missing SDU From UL", 4},
    {UINT, "Num PDU Dups", 4},
    {UINT, "Num PDU Dups Bytes", 4},
    {UINT, "Num PDU Out of Win", 4},
    {UINT, "Num PDU Out of Win Bytes", 4},
    {UINT, "Num PDU Invalid", 4},
    {UINT, "Num PDU Invalid Bytes", 4},
    {SKIP, NULL, 4},
    {UINT, "Num Flow Ctrl Trigger Rst", 4},
    {UINT, "Num Data PDU Rx Rst", 4},
    {UINT, "Num Data PDU Rx Bytes Rst", 4},
    {UINT, "Control PDU Bytes Rx Rst", 4},
    {SKIP, NULL, 4},
    {UINT, "Num Control PDU Gen Rst", 4},
    {UINT, "Num Control PDU Gen Bytes Rst", 4},
    {UINT, "Num Stat Rprt Ctrl PDU Rx Rst", 4},
    {UINT, "Num ROHC Ctrl PDU Rx Rst", 4},
    {UINT, "Num PDU ROHC Fail Rst", 4},
    {UINT, "Num PDU Integrity Fail Rst", 4},
    {UINT, "Num Missing SDU To UL Rst", 4},
    {UINT, "Num Missing SDU From UL Rst", 4},
    {UINT, "Num PDU Dups Rst", 4},
    {UINT, "Num PDU Dups Bytes Rst", 4},
    {UINT, "Num PDU Out of Win Rst", 4},
    {UINT, "Num PDU Out of Win Bytes Rst", 4},
    {UINT, "Num PDU Invalid Rst", 4},
    {UINT, "Num PDU Invalid Bytes Rst", 4},
    {SKIP, NULL, 4},
};
const ValueName LtePdcpDlStats_Subpkt_RB_Mode [] = {
    {1, "AM"},
};

// ----------------------------------------------------------------------------
// LTE PDCP UL Stats

const Fmt LtePdcpUlStats_Fmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num Subpkt", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdcpUlStats_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LtePdcpUlStats_SubpktPayload_v1 [] = {
    {UINT, "Num RBs", 1},
    {UINT, "PDCPUL Errors", 4},
};

const Fmt LtePdcpUlStats_SubpktPayload_v2 [] = {
    {UINT, "Num RBs", 1},
    {UINT, "PDCPUL Errors", 4},
};

const Fmt LtePdcpUlStats_Subpkt_RB_Fmt_v1 [] = {
    {UINT, "Rb Cfg Idx", 1},
    {UINT, "Mode", 1},
    {UINT, "PDCP Hdr Len", 1},
    {UINT, "Num RST", 4},
    {UINT, "Num Flow Ctrl Trigger", 4},
    {UINT, "Num Data PDU Tx", 4},
    {UINT, "Num Data PDU Tx Bytes", 4},
    {UINT, "Num Control PDU Tx", 4},
    {UINT, "Num Control PDU Tx Bytes", 4},
    {UINT, "Num Status Report", 4},
    {UINT, "Num ROHC Fail", 4},
    {UINT, "Num ROHC Ctrl PDU Tx", 4},
    {UINT, "Num Discard SDU", 4},
    {UINT, "Num Discard SDU Bytes", 4},
    {UINT, "Num PDU HO ReTx", 4},
    {UINT, "Num PDU HO ReTx Bytes", 4},
    {SKIP, NULL, 4},
    {UINT, "Num Flow Ctrl Trigger Rst", 4},
    {UINT, "Num Data PDU Tx Rst", 4},
    {UINT, "Num Data PDU Tx Bytes Rst", 4},
    {UINT, "Num Control PDU Tx Rst", 4},
    {UINT, "Num Control PDU Tx Bytes Rst", 4},
    {UINT, "Num Status Report Rst", 4},
    {UINT, "Num ROHC Fail Rst", 4},
    {UINT, "Num ROHC Ctrl PDU Tx Rst", 4},
    {UINT, "Num Discard SDU Rst", 4},
    {UINT, "Num Discard SDU Bytes Rst", 4},
    {UINT, "Num PDU HO ReTx Rst", 4},
    {UINT, "Num PDU HO ReTx Bytes Rst", 4},
    {SKIP, NULL, 4},
};

const Fmt LtePdcpUlStats_Subpkt_RB_Fmt_v2 [] = {
    {UINT, "Rb Cfg Idx", 1},
    {UINT, "Mode", 1},
    {UINT, "PDCP Hdr Len", 1},
    {UINT, "Num RST", 4},
    {UINT, "Num Pdcp Ul Buffer Pkt", 2},
    {UINT, "Num Pdcp Ul Buffer Pkt Bytes", 4},
    {UINT, "Num Flow Ctrl Trigger", 4},
    {UINT, "Num Data PDU Tx", 4},
    {UINT, "Num Data PDU Tx Bytes", 4},
    {UINT, "Num Control PDU Tx", 4},
    {UINT, "Num Control PDU Tx Bytes", 4},
    {UINT, "Num Status Report", 4},
    {UINT, "Num ROHC Ctrl PDU Tx", 4},
    {UINT, "Num ROHC Fail", 4},
    {UINT, "Num Discard SDU", 4},
    {UINT, "Num Discard SDU Bytes", 4},
    {UINT, "Num PDU HO ReTx", 4},
    {UINT, "Num PDU HO ReTx Bytes", 4},
    {UINT, "Num Piggybk Rohc Feedbk Rcvd", 4},
    {UINT, "Num Rohc Pdu Drop Ho", 4},
    {UINT, "Num Rohc Pdu Drop Ho Bytes", 4},
    {SKIP, NULL, 4},
    {UINT, "Num Flow Ctrl Trigger Rst", 4},
    {UINT, "Num Data PDU Tx Rst", 4},
    {UINT, "Num Data PDU Tx Bytes Rst", 4},
    {UINT, "Num Control PDU Tx Rst", 4},
    {UINT, "Num Control PDU Tx Bytes Rst", 4},
    {UINT, "Num Status Report Rst", 4},
    {UINT, "Num ROHC Ctrl PDU Tx Rst", 4},
    {UINT, "Num ROHC Fail Rst", 4},
    {UINT, "Num Discard SDU Rst", 4},
    {UINT, "Num Discard SDU Bytes Rst", 4},
    {UINT, "Num PDU HO ReTx Rst", 4},
    {UINT, "Num PDU HO ReTx Bytes Rst", 4},
    {UINT, "Num Piggybk Rohc Feedbk Rcvd Rst", 4},
    {UINT, "Num Rohc Pdu Drop Ho Rst", 4},
    {UINT, "Num Rohc Pdu Drop Ho Bytes Rst", 4},
    {SKIP, NULL, 4},

};

const ValueName LtePdcpUlStats_Subpkt_RB_Mode [] = {
    {1, "AM"},
};

// ----------------------------------------------------------------------------
// LTE RLC UL Statistics

const Fmt LteRlcUlStats_Fmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num Subpkt", 1},
    {SKIP, NULL, 2},
};

const Fmt LteRlcUlStats_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LteRlcUlStats_SubpktPayload [] = {
    {UINT, "Num RBs", 1},
    {UINT, "RLCUL Error Count", 4},
};

const Fmt LteRlcUlStats_Subpkt_RB_Fmt [] = {
    {UINT, "Rb Cfg Idx", 1},
    {UINT, "Mode", 1},
    {UINT, "Num RST", 4},
    {UINT, "Num New Data PDU", 4},
    {UINT, "Num New Data PDU Bytes", 4},
    {UINT, "Num SDU", 4},
    {UINT, "Num SDU Bytes", 4},
    {UINT, "Num Ctrl PDU Tx", 4},
    {UINT, "Num Ctrl PDU Bytes Tx", 4},
    {UINT, "Num Retx PDU", 4},
    {UINT, "Num Retx PDU Bytes", 4},
    {UINT, "Num Ctrl PDU Rx", 4},
    {UINT, "Num Complete NACK", 4},
    {UINT, "Num Segm NACK", 4},
    {UINT, "Num Invalid Ctrl PDU Rx", 4},
    {UINT, "Num Poll", 4},
    {UINT, "Num T Poll Retx Expiry", 4},
    {SKIP, NULL, 4},
    {UINT, "Num New Data PDU Rst", 4},
    {UINT, "Num New Data PDU Bytes Rst", 4},
    {UINT, "Num SDU Rst", 4},
    {UINT, "Num SDU Bytes Rst", 4},
    {UINT, "Num Ctrl PDU Tx Rst", 4},
    {UINT, "Num Ctrl PDU Bytes Tx Rst", 4},
    {UINT, "Num Retx PDU Rst", 4},
    {UINT, "Num Retx PDU Bytes Rst", 4},
    {UINT, "Num Ctrl PDU Rx Rst", 4},
    {UINT, "Num Complete NACK Rst", 4},
    {UINT, "Num Segm NACK Rst", 4},
    {UINT, "Num Invalid Ctrl PDU Rx Rst", 4},
    {UINT, "Num Poll Rst", 4},
    {UINT, "Num T Poll Retx Expiry Rst", 4},
    {SKIP, NULL, 4},
};
const ValueName LteRlcUlStats_Subpkt_RB_Mode [] = {
    {1, "AM"},
};

// ----------------------------------------------------------------------------
// LTE RLC DL Statistics

const Fmt LteRlcDlStats_Fmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num Subpkt", 1},
    {SKIP, NULL, 2},
};

const Fmt LteRlcDlStats_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LteRlcDlStats_SubpktPayload_v2 [] = {
    {UINT, "Num RBs", 1},
    {UINT, "RLC PDCP Q Full Count", 4},
    {UINT, "RLCDL Error Count", 4},
};

const Fmt LteRlcDlStats_SubpktPayload_v3 [] = {
    {UINT, "Num RBs", 1},
    {UINT, "RLC PDCP Q Full Count", 4},
    {UINT, "RLCDL Error Count", 4},
};

const Fmt LteRlcDlStats_Subpkt_RB_Fmt_v2 [] = {
    {UINT, "Rb Cfg Idx", 1},
    {UINT, "Mode", 1},
    {UINT, "Num RST", 4},
    {UINT, "Num Data PDU", 4},
    {UINT, "Data PDU Bytes", 4},
    {UINT, "Num Status Rxed", 4},
    {UINT, "Status Rxed Bytes", 4},
    {UINT, "Num Invalid PDU", 4},
    {UINT, "Invalid PDU Bytes", 4},
    {UINT, "Num Retx PDU", 4},
    {UINT, "Retx PDU Bytes", 4},
    {UINT, "Num Dup PDU", 4},
    {UINT, "Dup PDU Bytes", 4},
    {UINT, "Num Dropped PDU", 4},
    {UINT, "Dropped PDU Bytes", 4},
    {UINT, "Num Dropped PDU FC", 4},
    {UINT, "Dropped PDU Bytes FC", 4},
    {UINT, "Num SDU", 4},
    {UINT, "Num SDU Bytes", 4},
    {UINT, "Num NonSeq SDU", 4},
    {UINT, "Num Ctrl PDU", 4},
    {UINT, "Num Complete NACK", 4},
    {UINT, "Num Segments NACK", 4},
    {UINT, "Num t_reorder Expired", 4},
    {SKIP, NULL, 4},
    {UINT, "Num Data PDU Rst", 4},
    {UINT, "Data PDU Bytes Rst", 4},
    {UINT, "Num Status Rxed Rst", 4},
    {UINT, "Status Rxed Bytes Rst", 4},
    {UINT, "Num Invalid PDU Rst", 4},
    {UINT, "Invalid PDU Bytes Rst", 4},
    {UINT, "Num Retx PDU Rst", 4},
    {UINT, "Retx PDU Bytes Rst", 4},
    {UINT, "Num Dup PDU Rst", 4},
    {UINT, "Dup PDU Bytes Rst", 4},
    {UINT, "Num Dropped PDU Rst", 4},
    {UINT, "Dropped PDU Bytes Rst", 4},
    {UINT, "Num Dropped PDU FC Rst", 4},
    {UINT, "Dropped PDU Bytes FC Rst", 4},
    {UINT, "Num SDU Rst", 4},
    {UINT, "Num SDU Bytes Rst", 4},
    {UINT, "Num NonSeq SDU Rst", 4},
    {UINT, "Num Ctrl PDU Rst", 4},
    {UINT, "Num Complete NACK Rst", 4},
    {UINT, "Num Segments NACK Rst", 4},
    {UINT, "Num t_reorder Expired Rst", 4},
    {SKIP, NULL, 4},
};

const Fmt LteRlcDlStats_Subpkt_RB_Fmt_v3 [] = {
    {UINT, "Rb Cfg Idx", 1},
    {UINT, "Mode", 1},
    {UINT, "Num RST", 4},
    {UINT, "Num Data PDU", 4},
    {UINT, "Data PDU Bytes", 8},
    {UINT, "Num Status Rxed", 4},
    {UINT, "Status Rxed Bytes", 8},
    {UINT, "Num Invalid PDU", 4},
    {UINT, "Invalid PDU Bytes", 8},
    {UINT, "Num Retx PDU", 4},
    {UINT, "Retx PDU Bytes", 8},
    {UINT, "Num Dup PDU", 4},
    {UINT, "Dup PDU Bytes", 8},
    {UINT, "Num Dropped PDU", 4},
    {UINT, "Dropped PDU Bytes", 8},
    {UINT, "Num Dropped PDU FC", 4},
    {UINT, "Dropped PDU Bytes FC", 8},
    {UINT, "Num SDU", 4},
    {UINT, "Num SDU Bytes", 8},
    {UINT, "Num NonSeq SDU", 4},
    {UINT, "Num Ctrl PDU", 4},
    {UINT, "Num Complete NACK", 4},
    {UINT, "Num Segments NACK", 4},
    {UINT, "Num t_reorder Expired", 4},
    {UINT, "Num t_reorder Start", 4},
    {UINT, "Num Missed UM PDU", 4},
    {SKIP, NULL, 4},
    {UINT, "Num Data PDU Rst", 4},
    {UINT, "Data PDU Bytes Rst", 8},
    {UINT, "Num Status Rxed Rst", 4},
    {UINT, "Status Rxed Bytes Rst", 8},
    {UINT, "Num Invalid PDU Rst", 4},
    {UINT, "Invalid PDU Bytes Rst", 8},
    {UINT, "Num Retx PDU Rst", 4},
    {UINT, "Retx PDU Bytes Rst", 8},
    {UINT, "Num Dup PDU Rst", 4},
    {UINT, "Dup PDU Bytes Rst", 8},
    {UINT, "Num Dropped PDU Rst", 4},
    {UINT, "Dropped PDU Bytes Rst", 8},
    {UINT, "Num Dropped PDU FC Rst", 4},
    {UINT, "Dropped PDU Bytes FC Rst", 8},
    {UINT, "Num SDU Rst", 4},
    {UINT, "Num SDU Bytes Rst", 8},
    {UINT, "Num NonSeq SDU Rst", 4},
    {UINT, "Num Ctrl PDU Rst", 4},
    {UINT, "Num Complete NACK Rst", 4},
    {UINT, "Num Segments NACK Rst", 4},
    {UINT, "Num t_reorder Expired Rst", 4},
    {UINT, "Num t_reorder Start Rst", 4},
    {UINT, "Num Missed UM PDU Rst", 4},
    {SKIP, NULL, 4},
};
const ValueName LteRlcDlStats_Subpkt_RB_Mode [] = {
    {1, "AM"},
};

// ----------------------------------------------------------------------------
// LTE PDCP DL Ctrl PDU

const Fmt LtePdcpDlCtrlPdu_Fmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num Subpkt", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdcpDlCtrlPdu_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LtePdcpDlCtrlPdu_SubpktPayload [] = {
    {UINT, "RB Cfg Idx", 1},
    {UINT, "Mode", 1},
    {UINT, "SN Length (bit)", 1},
};
const ValueName LtePdcpDlCtrlPdu_Subpkt_Mode [] = {
    {1, "AM"},
};

const Fmt LtePdcpDlCtrlPdu_Subpkt_PDU_Header [] = {
    {UINT, "Num PDUs", 1},
};

const Fmt LtePdcpDlCtrlPdu_Subpkt_PDU_Fmt [] = {
    {UINT, "PDU Size", 2},
    {UINT, "Logged Bytes", 2},
    {UINT, "System Frame Number", 2},
    {PLACEHOLDER, "Subframe Number", 0},
    {PLACEHOLDER, "type", 0},
    {UINT, "fms", 2},
};

// ----------------------------------------------------------------------------
// LTE PDCP UL Ctrl PDU

const Fmt LtePdcpUlCtrlPdu_Fmt [] = {
    {UINT, "Version", 1},
    {UINT, "Num Subpkt", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdcpUlCtrlPdu_SubpktHeader [] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt LtePdcpUlCtrlPdu_SubpktPayload [] = {
    {UINT, "RB Cfg Idx", 1},
    {UINT, "Mode", 1},
    {UINT, "SN Length (bit)", 1},
};
const ValueName LtePdcpUlCtrlPdu_Subpkt_Mode [] = {
    {1, "AM"},
};

const Fmt LtePdcpUlCtrlPdu_Subpkt_PDU_Header [] = {
    {UINT, "Num PDUs", 1},
};

const Fmt LtePdcpUlCtrlPdu_Subpkt_PDU_Fmt [] = {
    {UINT, "PDU Size", 2},
    {UINT, "Logged Bytes", 2},
    {UINT, "System Frame Number", 2},
    {PLACEHOLDER, "Subframe Number", 0},
    {PLACEHOLDER, "type", 0},
    {UINT, "fms", 2},
};

// ----------------------------------------------------------------------------
// LTE PUCCH Power Control

const Fmt LtePucchPowerControl_Fmt [] = {
    {UINT, "Version", 1},
};
const Fmt LtePucchPowerControl_Fmt_v4 [] = {
    {SKIP, NULL, 2},
    {UINT, "Number of Records", 1},
};

const Fmt LtePucchPowerControl_Record_Fmt_v4 [] = {
    {UINT, "SFN", 4},
    // include Sub-FN,  Tx Power, DCI Format, PUCCH Format, N_HARQ
    {PLACEHOLDER, "Sub-FN", 0},
    {PLACEHOLDER, "PUCCH Tx Power (dBm)", 0},
    {PLACEHOLDER, "DCI Format", 0},
    {PLACEHOLDER, "PUCCH Format", 0},
    {PLACEHOLDER, "N_HARQ", 0},

    {UINT, "TPC Command", 4},
    // include N_CQI, DL Pass Loss,
    {PLACEHOLDER, "N_CQI", 0},
    {PLACEHOLDER, "DL Pass Loss", 0},

    {UINT, "g(i)", 2},
    {UINT, "PUCCH Actual Tx Power", 1},
    {SKIP, NULL, 1},
};
const ValueName LtePucchPowerControl_Record_v4_DCI_Format [] = {
    // Release 8
    // http://www.sharetechnote.com/html/LTE_Advanced_DCI.html
    {0, "Format 0"},
    {1, "Format 1"},
    {2, "Format 1A"},
    {3, "Format 1B"},
    {4, "Format 1C"},
    {5, "Format 1D"},
    {6, "Format 2"},
    {7, "Format 2A"},
    // {8, "Format 2B"},
    // {9, "Format 2C"},
    {10, "Format 3"},
    {11, "Format 3A"},
    // {12, "Format 4"},
};
const ValueName LtePucchPowerControl_Record_v4_PUCCH_Format [] = {
    {0, "Format 1"},
    {1, "Format 1A"},
    {2, "Format 1B"},
    {3, "Format 2"},
    {4, "Format 2A"},
    {5, "Format 2B"},
    {6, "Format 3"},
};
const ValueName LtePucchPowerControl_Record_v4_TPC [] = {
    {31, "Not present"},
    {63, "-1"},
};

// ----------------------------------------------------------------------------
// LTE PUSCH Power Control

const Fmt LtePuschPowerControl_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePuschPowerControl_Fmt_v4 [] = {
    {SKIP, NULL, 2},
    {UINT, "Number of Records", 1},
};

const Fmt LtePuschPowerControl_Fmt_v5 [] = {
    {SKIP, NULL, 2},
    {UINT, "Number of Records", 1},
};

const Fmt LtePuschPowerControl_Record_Fmt_v4 [] = {
    {UINT, "SFN", 4},
    {PLACEHOLDER, "Sub-FN", 0},
    {PLACEHOLDER, "PUSCH Tx Power (dBm)", 0},
    {PLACEHOLDER, "DCI Format", 0},
    {PLACEHOLDER, "Tx Type", 0},
    {UINT, "Transport Block Size", 4},
    {PLACEHOLDER, "DL Path Loss", 0},
    {PLACEHOLDER, "F(i)", 0},
    {UINT, "TPC", 4},
    {PLACEHOLDER, "PUSCH Actual Tx Power", 0},
};
const Fmt LtePuschPowerControl_Record_Fmt_v5 [] = {
    {UINT, "SFN", 4},
    {PLACEHOLDER, "Sub-FN", 0},
    {PLACEHOLDER, "PUSCH Tx Power (dBm)", 0},
    {PLACEHOLDER, "DCI Format", 0},
    {PLACEHOLDER, "Tx Type", 0},
    {UINT, "Num RBs", 4},
    {PLACEHOLDER, "Transport Block Size", 0},
    {PLACEHOLDER, "DL Path Loss", 0},
    {UINT, "F(i)", 4},
    {PLACEHOLDER, "TPC", 0},
    {PLACEHOLDER, "PUSCH Actual Tx Power", 0},
};
const ValueName LtePuschPowerControl_Record_v5_TxType [] = {
    {1, "Dynamic"},
    {2, "RACH MSG3"},
};
const ValueName LtePuschPowerControl_Record_v5_DCI_Format [] = {
    // Release 8
    // http://www.sharetechnote.com/html/LTE_Advanced_DCI.html
    {0, "Format 0"},
    {1, "Format 1"},
    {2, "Format 1A"},
    {3, "Format 1B"},
    {4, "Format 1C"},
    {5, "Format 1D"},
    {6, "Format 2"},
    {7, "Format 2A"},
    // {8, "Format 2B"},
    // {9, "Format 2C"},
    {10, "Format 3"},
    {11, "Format 3A"},
    // {12, "Format 4"},
};
const ValueName LtePuschPowerControl_Record_v5_TPC [] = {
    {15, "N/A"},
    {31, "-1"},
};

// ----------------------------------------------------------------------------
// LTE PDCCH-PHICH Indication Report

const Fmt LtePdcchPhichIndicationReport_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePdcchPhichIndicationReport_Fmt_v5 [] = {
    {UINT, "Duplex Mode", 2},
    {UINT, "Number of Records", 1},
};

const Fmt LtePdcchPhichIndicationReport_Fmt_v25 [] = {
    {UINT, "Duplex Mode", 2},
    {UINT, "Number of Records", 1},
};

const Fmt LtePdcchPhichIndicationReport_Fmt_v33 [] = {
    {UINT, "Duplex Mode", 2},
    {UINT, "Number of Records", 1},
};

const Fmt LtePdcchPhichIndicationReport_Record_v5_p1 [] = {
    {UINT, "Num PDCCH Results", 4},
    {PLACEHOLDER, "PDCCH Timing SFN", 0},
    {PLACEHOLDER, "PDCCH Timing Sub-FN", 0},
    {PLACEHOLDER, "PHICH Included", 0},
    {PLACEHOLDER, "PHICH 1 Included", 0},
    {PLACEHOLDER, "PHICH Timing SFN", 0},
    {UINT, "PHICH Timing Sub-FN", 4},
    {PLACEHOLDER, "PHICH Value", 0},
    {PLACEHOLDER, "PHICH 1 Value", 0},
};

const Fmt LtePdcchPhichIndicationReport_Record_v25_p1 [] = {
    {UINT, "Num PDCCH Results", 4},             // 3 bits
    {PLACEHOLDER, "Num PHICH Results", 0},      // 3 bits
    {PLACEHOLDER, "PDCCH Timing SFN", 0},       // 10 bits
    {PLACEHOLDER, "PDCCH Timing Sub-FN", 0},    // 4 bits
};

const Fmt LtePdcchPhichIndicationReport_Record_v33_p1 [] = {
    {UINT, "Num PDCCH Results", 4},             // 3 bits
    {PLACEHOLDER, "Num PHICH Results", 0},      // 3 bits
    {PLACEHOLDER, "PDCCH Timing SFN", 0},       // 10 bits
    {PLACEHOLDER, "PDCCH Timing Sub-FN", 0},    // 4 bits
    {SKIP, NULL, 8},
};

// totally 12 bytes for all phich
const Fmt LtePdcchPhichIndicationReport_Record_v25_phich [] = {
    {UINT, "Cell Index", 4},                    // 3 bits
    {PLACEHOLDER, "PHICH Included", 0},         // 1 bit
    {PLACEHOLDER, "PHICH 1 Included", 0},       // 1 bit
    {PLACEHOLDER, "PHICH Value", 0},            // 1 bit
    {PLACEHOLDER, "PHICH 1 Value", 0},          // 1 bit
};

// totally 20 bytes for all phich
const Fmt LtePdcchPhichIndicationReport_Record_v33_phich [] = {
    {UINT, "Cell Index", 4},                    // 3 bits
    {PLACEHOLDER, "PHICH Included", 0},         // 1 bit
    {PLACEHOLDER, "PHICH 1 Included", 0},       // 1 bit
    {PLACEHOLDER, "PHICH Value", 0},            // 1 bit
    {PLACEHOLDER, "PHICH 1 Value", 0},          // 1 bit
};

// totally number of pdcch info + number of pdcch hidden info = 8
const Fmt LtePdcchPhichIndicationReport_Record_v5_p2 [] = {
    // PDCCH Info
    {UINT, "Serv Cell Idx", 2},
    {PLACEHOLDER, "RNTI Type", 0},
    {PLACEHOLDER, "Payload Size", 0},
    {PLACEHOLDER, "Aggregation Level", 0},
    {UINT, "Search Space", 2},
    {PLACEHOLDER, "SPS Grant Type", 0},
    {PLACEHOLDER, "New DL Tx", 0},
    {PLACEHOLDER, "Num DL Trblks", 0},
};

// totally 64 bytes for all pdcch
const Fmt LtePdcchPhichIndicationReport_Record_v25_pdcch [] = {
    // PDCCH Info
    {UINT, "Serv Cell Idx", 2},                 // 3 bits
    {PLACEHOLDER, "RNTI Type", 0},              // 4 bits
    {PLACEHOLDER, "Payload Size", 0},           // 7 bits
    {PLACEHOLDER, "Aggregation Level", 0},      // 2 bits
    {UINT, "Search Space", 2},                  // 1 bit
    {PLACEHOLDER, "SPS Grant Type", 0},         // 3 bits
    {PLACEHOLDER, "New DL Tx", 0},              // 1 bit
    {PLACEHOLDER, "Num DL Trblks", 0},          // 2 bits
    {SKIP, NULL, 4},
};

// totally 64 bytes for all pdcch
const Fmt LtePdcchPhichIndicationReport_Record_v33_pdcch [] = {
    // PDCCH Info
    {UINT, "Serv Cell Idx", 2},                 // 3 bits
    {PLACEHOLDER, "RNTI Type", 0},              // 4 bits
    {PLACEHOLDER, "Payload Size", 0},           // 7 bits
    {PLACEHOLDER, "Aggregation Level", 0},      // 2 bits
    {UINT, "Search Space", 2},                  // 1 bit
    {PLACEHOLDER, "SPS Grant Type", 0},         // 3 bits
    {PLACEHOLDER, "New DL Tx", 0},              // 1 bit
    {PLACEHOLDER, "Num DL Trblks", 0},          // 2 bits
    {SKIP, NULL, 4},
};

const Fmt LtePdcchPhichIndicationReport_Record_v5_p3 [] = {
    // PDCCH Hidden Info
    {SKIP, NULL, 4},
};

const ValueName LtePdcchPhichIndicationReport_Record_v5_Included [] = {
    {0, "No"},
    {1, "Yes"},
};
const ValueName LtePdcchPhichIndicationReport_Record_v5_Value [] = {
    {0, "NACK"},
    {1, "ACK"},
};
const ValueName LtePdcchPhichIndicationReport_Record_v5_NewDLTx [] = {
    {0, "false"},
    {1, "true"},
};
const ValueName LtePdcchPhichIndicationReport_Record_v5_AggLv [] = {
    {0, "Agg1"},
    {1, "Agg2"},
    {2, "Agg4"},
    {3, "Agg8"},
};
const ValueName LtePdcchPhichIndicationReport_Record_v5_SS [] = {
    {0, "Common"},
    {1, "UE-specific"},
};

// ----------------------------------------------------------------------------
// Common ValueName

const ValueName ValueNameCarrierIndex [] = {
    // 4 bits
    {0, "PCC"},
    {1, "SCC"},
    {2, "SCC-2"},
};

const ValueName ValueNameAggregationLevel [] = {
    // 2 bits
    {0, "Agg1"},
    {1, "Agg2"},
    {2, "Agg4"},
    {3, "Agg8"},
};

const ValueName ValueNameSearchSpaceType [] = {
    // 1 bit
    {0, "Common"},
    {1, "UE-specific"},
};

const ValueName ValueNameRNTIType [] = {
    // 4 bits
    {0, "C-RNTI"},
    {1, "SPS-RNTI"},
    {2, "P-RNTI"},
    {3, "RA-RNTI"},
    {4, "Temporary-C-RNTI"},
    {5, "SI-RNTI"},
    {6, "TPC-PUSCH-RNTI"},
    {7, "TPC-PUCCH-RNTI"},
    {8, "MBMS RNTI"},
};
const ValueName ValueNameDCIFormat [] = {
    // 4 bits
    // Release 8
    // http://www.sharetechnote.com/html/LTE_Advanced_DCI.html
    {0, "Format 0"},
    {1, "Format 1"},
    {2, "Format 1A"},
    {3, "Format 1B"},
    {4, "Format 1C"},
    {5, "Format 1D"},
    {6, "Format 2"},
    {7, "Format 2A"},
    {8, "Format 3"},
    {9, "Format 3A"},
};

const ValueName ValueNameMatchOrNot [] = {
    // 1 bit
    {0, "Mismatch"},
    {1, "Match"},
};

const ValueName ValueNamePruneStatus [] = {
    // 11 bits
    //
    {1, "SUCCESS_DCI0"},
    {3, "SUCCESS_DCI1A"},
    {4, "SUCCESS_DCI1C"},
    {6, "SUCCESS_DCI2_2A_2B"},
    {8, "TAIL_MISMATCH"},
    {9, "FAIL_SURVIVOR_SELECT"},
    {10, "PADDING_ERROR"},
    {13, "RB_ALLOC_SET_NUM_ERROR_TYPE1"},
    {17, "DUPLICATE_HARQ_ID_ERROR"},
    {24, "BAD_RIV_DCI0"},
    {25, "RB_ALLOC_ERROR_DCI0"},
    {26, "INVALID_RB_NUM_DCI0"},
    {33, "RB_ALLOC_ERROR_DCI1A"},
    {34, "INVALID_RB_NUM_DCI1A"},
    {48, "PMI_ERROR_DCI2_2A"},
    {50, "NUM_LAYERS_ERROR_DCI2_2A_TB1"},
    {64, "FAIL_SER_ENGYMTRC_CHECK"},
};

const ValueName ValueNameFrameStructure [] = {
    {0, "FDD"},
    {1, "TDD"},
};

const ValueName ValueNameDuplexingMode [] = {
    {0, "FDD"},
    {1, "TDD"},
};

const ValueName ValueNameNumNBAntennas [] = {
    {0, "1 or 2"},
    {1, "2 antennas"},
    {2, "4 antennas"},
};

const ValueName ValueNameTrueOrFalse [] = {
    {0, "False"},
    {1, "True"},
};

const ValueName ValueNameHARQLogStatus [] = {
    {0, "Normal"},
};

const ValueName ValueNameCPType [] = {
    // 1 bit
    {0, "Normal"},
};

const ValueName ValueNameNormalOrNot [] = {
    {0, "Normal"},
};

const ValueName ValueNamePassOrFail [] = {
    {0, "Fail"},
    {1, "Pass"},
};

const ValueName ValueNameNumber [] = {
    {0, "First"},
    {1, "Second"},
    {2, "Third"},
    {3, "Fourth"},
    {4, "Fifth"},
    {5, "Sixth"},
    {6, "Seventh"},
    {7, "Eighth"},
};

const ValueName ValueNameCompandingStats [] = {
    {0, "3 bit LLR"},
    {1, "4 bit LLR"},
    {2, "6 bit LLR"},
};

const ValueName ValueNameEnableOrDisable [] = {
    {0, "Disabled"},
    {1, "Enabled"},
};

const ValueName ValueNameYesOrNo [] = {
    {0, "No"},
    {1, "Yes"},
};

const ValueName ValueNameAckOrNack [] = {
    {0, "NACK"},
    {1, "ACK"},
};

const ValueName ValueNameModulation [] = {
    {0, "BPSK"},
    {1, "QPSK"},
    {2, "16-QAM"},
    {3, "64-QAM"},
};

const ValueName ValueNameExistsOrNone [] = {
    {0, "None"},
    {1, "Exists"},
};

const ValueName ValueNameOnOrOff [] = {
    {0, "Off"},
    {1, "On"},
};

const ValueName ValueNameCSFTxMode [] = {
    {0, "TM_Invalid"},
    {1, "TM_Single_Ant_Port_0"},
    {2, "TM_TD_Rank_1"},
    {3, "TM_OL_SM"},
    {4, "TM_CL_SM"},
    {5, "TM_MU_MIMO"},
    {6, "TM_CL_Rank_1_PC"},
    {7, "TM_Single_Ant_Port_5"},
};

const ValueName ValueNameRankIndex [] = {
    {0, "Rank 1"},
    {1, "Rank 2"},
};

const ValueName ValueNameCsiMeasSetIndex [] = {
    {0, "CSI0"},
};

const ValueName ValueNamePuschReportingMode [] = {
    {0, "MODE_APERIODIC_RM12"},
    {1, "MODE_APERIODIC_RM20"},
    {2, "MODE_APERIODIC_RM22"},
    {3, "MODE_APERIODIC_RM30"},
    {4, "MODE_APERIODIC_RM31"},
};

const ValueName ValueNamePucchReportType [] = {
    {2, "Type 2, Wideband CQI, PMI Feedback"},
    {3, "Type 3, RI Feedback"},
};

const ValueName ValueNamePucchReportingMode [] = {
    {2, "MODE_1_1"},
};

const ValueName ValueNameTransmissionScheme [] = {
    // 4 bits
    {1, "Single Antenna Port (SISO or SIMO)"},
    {2, "Transmit diversity"},
    {3, "Open-loop spatial multiplexing"},
    {4, "Closed-loop spatial multiplexing"},
    {5, "Multi-User MIMO"},
    {6, "Closed-loop rank-1 spatial multiplexing"},
    {7, "Single Antenna Port Beamforming"},
    {8, "Dual-Layer Beamforming"},
};

const ValueName ValueNameFrequencySelectivePMI [] = {
    {0, "WideBand"},
};

const ValueName ValueNameCDRXEvent [] = {
    {0, "ON_DURATION_TIMER_START"},
    {1, "ON_DURATION_TIMER_END"},
    {2, "SHORT_CYCLE_START"},
    {3, "SHORT_CYCLE_END"},
    {4, "LONG_CYCLE_START"},
    {5, "LONG_CYCLE_END"},
    {6, "UL_RETX_TIMER_START"},
    {7, "UL_RETX_TIMER_END"},
    {8, "INACTIVITY_TIMER_START"},
    {9, "INACTIVITY_TIMER_END"},
    {10, "DL_DRX_RETX_TIMER_START"},
    {11, "DL_DRX_RETX_TIMER_END"},
    {12, "CDRX_ON_2_OFF"},
    {13, "CDRX_OFF_2_ON"},
};

const ValueName ValueNameWcdmaRrcStates [] = {
    {0, "DISCONNECTED"},
    {1, "CONNECTING"},
    {2, "CELL_FACH"},
    {3, "CELL_DCH"},
    {4, "CELL_PCH"},
    {5, "URA_PCH"},
};

const ValueName ValueNameCellIndex [] = {
    // 4 bits
    {0, "PCell"},
    {1, "SCell"},
};

const ValueName ValueNameBandClassGSM [] = {
    // 4 bits
    {0, "Current 900/1800 Setting"},
    {10, "1900 PCS"},
    {11, "GSM 850"},
};

const ValueName ValueNameBandClassCDMA [] = {
    {0, "800 MHz Cellular"},
    {1, "1.8 to 2.0 GHz PCS"},
    {10, "Secondary 800 MHz"},
    {255, "Disabled"},
};

const ValueName ValueNameTimerState [] = {
    // 8 bits
    {0, "Stopped"},
    {1, "Running"},
    {2, "Expired"},
};

const ValueName ValueNameTechnology [] = {
    {0, "1x CDMA"},
};

const ValueName ValueNameQueue [] = {
    {0, "HPQ0"},
    {2, "Reserved"},
};

const ValueName ValueNamePilotSet [] = {
    {0, "PreCandidate Set"},
    {1, "Active Set"},
    {2, "Candidate Set"},
    {4, "Neighbor Set"},
    {8, "Remaining Set"},
};

const ValueName ValueNameSearcherState [] = {
    {2, "Synchronization"},
    {3, "Idle"},
    {4, "Traffic"},
};

const ValueName ValueNameSRSShortingfor2ndSlot [] = {
    // 1 bit
    {0, "Normal"},
    {1, "Shorten 2nd"},
};

const ValueName ValueNameCipherAlgo [] = {
    // 1 byte
    {2, "LTE SNOW-3G"},
    {3, "LTE AES"},
    {7, "None"},
};

const ValueName ValueNamePdcpSNLength [] = {
    {0, "5 bit"},
    {1, "7 bit"},
    {2, "12 bit"},
};

const ValueName ValueNamePdcpCipherDataPduMode [] = {
    {0, "AM"},
};

const ValueName ValueNameGSMRxLevMin [] = {
    {0, "-111 dBm to -110 dBm"},
    {1, "-110 dBm to -109 dBm"},
    {2, "-109 dBm to -108 dBm"},
    {3, "-108 dBm to -107 dBm"},
    {4, "-107 dBm to -106 dBm"},
    {5, "-106 dBm to -105 dBm"},
    {6, "-105 dBm to -104 dBm"},
    {7, "-104 dBm to -103 dBm"},
    {8, "-103 dBm to -102 dBm"},
    {9, "-102 dBm to -101 dBm"},
    // Not sure about following lines
    {10, "-101 dBm to -100 dBm"},
    {11, "-100 dBm to -99 dBm"},
    {12, "-99 dBm to -98 dBm"},
    {13, "-98 dBm to -97 dBm"},
    {14, "-97 dBm to -96 dBm"},
    {15, "-96 dBm to -95 dBm"},
    {16, "-95 dBm to -94 dBm"},
    {17, "-94 dBm to -93 dBm"},
};

const ValueName ValueNameGSMAdditionalParam [] = {
    {0, "SysInfo 16/17 not supported"},
};

const ValueName ValueNameGSMPenaltyTime [] = {
    {0, "20 sec"},
};

const ValueName ValueNameSupportedOrNot [] = {
    {0, "Not Supported"},
    {1, "Supported"},
};

const ValueName ValueNameGSMChannelType [] = {
    {0, "DCCH"},
    {1, "BCCH"},
    {3, "CCCH"},
    {4, "SACCH"},
};

const ValueName ValueNameGSMSignalingMessageType [] = {
    {0, "System Information Type 13"},
    {7, "System Information Type 2quater"},
    {13, "Channel Release"},
    {21, "Measurement Report"},
    {22, "Classmark Change"},
    {25, "System Information Type 1"},
    {26, "System Information Type 2"},
    {27, "System Information Type 3"},
    {28, "System Information Type 4"},
    {29, "System Information Type 5"},
    {63, "Immediate Assignment"},
};

const ValueName ValueNameDirection [] = {
    {0, "Uplink"},
    {1, "Downlink"},
};

// ----------------------------------------------------------------------------
const Fmt ModemDebug_Fmt [] = {
    {UINT, "Version", 1},
    {SKIP, NULL, 1},
    {UINT, "Number of parameters", 1},
    {SKIP, NULL, 1},
    {QCDM_TIMESTAMP, "Timestamp", 8},
    {UINT, "Line of code", 2},
    {SKIP, NULL, 6},
};

bool is_log_packet (const char *b, size_t length);
bool is_debug_packet (const char *b, size_t length);   //Yuanjie: test if it's a debugging message

// Given a binary string, try to decode it as a log packet.
// Return a specailly formatted Python list that stores the decoding result.
// If skip_decoding is True, only the header would be decoded.
PyObject * decode_log_packet (const char *b, size_t length, bool skip_decoding);

void on_demand_decode (const char *b, size_t length, LogPacketType type_id, PyObject* result);


PyObject * decode_log_packet_modem (const char *b, size_t length, bool skip_decoding);

#endif  // __DM_COLLECTOR_C_LOG_PACKET_H__
