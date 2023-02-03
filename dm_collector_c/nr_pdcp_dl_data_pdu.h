/*
 * NR5G PDCP DL Data Pdu
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

// 4 bytes
const Fmt NrPdcpDlDataPdu_MajorMinorVersion_Fmt[] = {
        {UINT, "Minor Version", 2},
        {UINT, "Major Version", 2},
        {PLACEHOLDER, "Major.Minor Version", 0}
        {PLACEHOLDER, "{id: 2003420 }", 0}
};

// 6 bytes
const Fmt NrPdcpDlDataPdu_MetaRbNumAndReserved_Fmt[] = {
        {UINT, "Number of Meta", 2},
        {UINT, "Number of RB", 2},
        {SKIP, NULL, 2}
};

// 16 bytes
const Fmt NrPdcpDlDataPdu_PdcpState_Fmt[] = {
        {UINT, "RB Cfg Index", 4},
        {UINT, "RX Deliv", 4},
        {UINT, "RX Next", 4},
        {UINT, "Next Count", 4}
};

// 4 bytes
const Fmt NrPdcpDlDataPdu_SystemTime_Fmt[] = {
        {UINT, "Slot", 1},
        {SKIP, NULL, 1},
        {UINT, "Frame", 2},
};

// 10 bytes
const Fmt NrPdcpDlDataPdu_RxTimetick_Fmt[] = {
        {UINT, "RX Timetick Raw", 8},
        {PLACEHOLDER, "RX Timetick", 0},
        {SKIP, NULL, 2}, // skip 0x0104 bits, seem to not be used in decoding at all
};

// 1 byte
const Fmt NrPdcpDlDataPdu_KeyIndex_Fmt[] = {
        {UINT, "Key Index", 1},
};

// 1 byte
const Fmt NrPdcpDlDataPdu_RlcPath_Fmt[] = {
        {UINT, "RLC Path", 1},
};

const ValueName ValueNameNrPdcpDlDataPdu_RlcPath[] = {
        {0, "NR"}
};

// 1 byte
const Fmt NrPdcpDlDataPdu_RouteStatus_Fmt[] = {
        {UINT, "Route Status", 1},
};

const ValueName ValueNameNrPdcpDlDataPdu_RouteStatus[] = {
        {2, "DELIV_DIRECT"}
};

// 2 bytes
const Fmt NrPdcpDlDataPdu_IpPacketHeaders_Fmt[] = {
        {UINT, "IP Packet Header[0]", 1}, // need to represent in hex
        {UINT, "IP Packet Header[1]", 1}, // need to represent in hex
};

// 4 bytes
const Fmt NrPdcpDlDataPdu_StartCount_Fmt[] = {
        {UINT, "Route Status", 4},
};

// 4 bytes
const Fmt NrPdcpDlDataPdu_EndCount_Fmt[] = {
        {UINT, "Route Status", 4},
};

// 4 bytes
const Fmt NrPdcpDlDataPdu_RlcEndSn_Fmt[] = {
        {UINT, "RLC end SN", 4},
};

// 8 bytes
const Fmt NrPdcpDlDataPdu_IpPacketNums_Fmt[] = {
        {UINT, "Number IP Pkts", 4},
        {UINT, "Number IP bytes", 4}
};

static int _decode_nr_pdcp_dl_data_pdu (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;

    // return offset - start to return however many bytes we have left remaining
    return offset - start;
}




















