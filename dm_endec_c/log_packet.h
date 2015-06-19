#ifndef __DM_ENDEC_C_LOG_PACKET_H__
#define __DM_ENDEC_C_LOG_PACKET_H__

#include <map>

enum FmtType { UINT, QCDM_TIMESTAMP, PLMN, RSRP, RSRQ, SKIP };
typedef std::map<int, const char*> ValueString;

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


const Fmt WcdmaCellIdFmt [] = {
    {UINT, "UTRA UL Absolute RF channel number", 4},
    {UINT, "UTRA DL Absolute RF channel number", 4},
    {UINT, "Cell identity (28-bits)", 4},
    {UINT, "URA to use in case of overlapping URAs", 1},
    {SKIP, NULL, 2},    // Unknown yet
    {UINT, "Allowed Call Access", 1},
    {UINT, "PSC", 2},
    {PLMN, "PLMN", 6},
    {UINT, "LAC id", 4},
    {UINT, "RAC id", 4}
};

// ------------------------------------------------------------
const Fmt WcdmaSignalingMessagesFmt [] = {
    {UINT, "Channel Type", 1},
    {UINT, "Radio Bearer ID", 1},
    {UINT, "Message Length", 2}
};

const ValueString WcdmaSignalingMsgChannelType = {
    {0x00, "RRC_UL_CCCH"},
    {0x01, "RRC_UL_DCCH"},
    {0x02, "RRC_DL_CCCH"},
    {0x03, "RRC_DL_DCCH"},
    {0x04, "RRC_DL_BCCH_BCH"}
};

// ------------------------------------------------------------
// LTE_RRC_OTA_Packet
const Fmt LteRrcOtaPacketFmt [] = {
    {UINT, "Pkt Version", 1},
    {UINT, "RRC Release Number", 1},
    {UINT, "Major/minor", 1},
    {UINT, "Radio Bearer ID", 1},
    {UINT, "Physical Cell ID", 2},
    {UINT, "Freq", 2},
    {UINT, "SysFrameNum/SubFrameNum", 2},
    {UINT, "PDU Number", 1}
    // continued in LteRrcOtaPacketFmt_v2 or LteRrcOtaPacketFmt_v7
};

const Fmt LteRrcOtaPacketFmt_v2 [] = {
    {UINT, "Msg Length", 1},
    {UINT, "SIB Mask in SI", 1}
};

const Fmt LteRrcOtaPacketFmt_v7 [] = {
    {SKIP, NULL, 4},            // Unknown yet, only for Pkt Version = 7
    {UINT, "Msg Length", 1},
    {UINT, "SIB Mask in SI", 1}
};

const ValueString LteRrcOtaPduType = {
    {0x02, "LTE-RRC_BCCH_DL_SCH"},
    {0x04, "LTE-RRC_PCCH"},
    {0x06, "LTE-RRC_DL_DCCH"},
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
// LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results
const Fmt LteMl1CmlifmrFmt [] = {
    {UINT, "Version", 1},
    {SKIP, NULL, 7},        // Unknown
};

const Fmt LteMl1CmlifmrFmt_v3_Header [] = {
    {UINT, "E-ARFCN", 2},
    {UINT, "Serving Physical Cell ID", 2},
    {UINT, "Sub-frame Number", 2},
    {RSRP, "Serving Filtered RSRP(dBm)", 2},
    {SKIP, NULL, 2},    // Duplicated
    {RSRQ, "Serving Filtered RSRQ(dB)", 2},
    {SKIP, NULL, 2},    // Duplicated
    {UINT, "Number of Neighbor Cells", 1},
    {UINT, "Number of Detected Cells", 1}
};

const Fmt LteMl1CmlifmrFmt_v3_Neighbor_Cell [] = {
    {UINT, "Physical Cell ID", 2},
    {RSRP, "Filtered RSRP(dBm)", 2},
    {SKIP, NULL, 2},    // Duplicated
    {RSRQ, "Filtered RSRQ(dB)", 2},
    {SKIP, NULL, 4}     // Duplicated & reserved
};

const Fmt LteMl1CmlifmrFmt_v3_Detected_Cell [] = {
    {UINT, "Physical Cell ID", 4},
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

const ValueString LteMl1Subpkt_SubpktType = {
    {25, "Serving_Cell_Measurement_Result"}
};

// Serving_Cell_Measurement_Result
const Fmt LteMl1SubpktFmt_v1_Scmr_v4 [] = {
    {UINT, "E-ARFCN", 2},
    {UINT, "Physical Cell ID", 2}
};


const Fmt LteRrcServCellInfoLogPacketFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Physical Cell ID", 2},
    {UINT, "DL FREQ",2},
    {UINT, "UL FREQ",2},
    {UINT, "DL BW", 1},
    {UINT, "UL BW", 1},
    {UINT, "Cell Identity", 4},
    {UINT, "TAC", 2},
    {UINT, "Band Indicator", 4},
    {UINT, "MCC", 2},
    {UINT, "MCC Digit", 1},
    {UINT, "MNC", 2},
    {UINT, "Allowed Access", 1}
};

const Fmt LteRrcMibMessageLogPacketFmt [] = {
    {UINT, "Version", 1},
    {UINT, "Physical Cell ID", 2},
    {UINT, "Freq", 2},
    {UINT, "SFN", 2},
    {UINT, "Number of Antenna", 1},
    {UINT, "DL BW", 1}
};

#endif  // __DM_ENDEC_C_LOG_PACKET_H__