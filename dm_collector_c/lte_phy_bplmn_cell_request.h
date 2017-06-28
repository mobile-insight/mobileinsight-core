/*
 * LTE PHY BPLMN Cell Request
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePhyBplmnCellRequest_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePhyBplmnCellRequest_Payload_v4 [] = {
    {UINT, "Standards Version", 1},
    {SKIP, NULL, 2},
    {UINT, "Frequency", 4},
    {UINT, "Cell ID", 2},   // 10 bits
    {PLACEHOLDER, "Barred Status", 0},  // 4 bits
    {SKIP, NULL, 2},
    {UINT, "Q Rx Lev Min", 4},  // 8 bits (x-256)
    {PLACEHOLDER, "Q Rx Lev Min Offset", 0},    // 4 bits
    {PLACEHOLDER, "P Max", 0},  // 8 bits
};
const Fmt LtePhyBplmnCellRequest_Rel9Info [] = {
    {PLACEHOLDER, "Rel 9 Info Q Qual Min Data", 0},    // 6 bits
    {PLACEHOLDER, "Rel 9 Info Q Qual Min Offset", 0},  // 6 bits
};

const ValueName LtePhyBplmnCellRequest_StandardsVersion [] = {
    {1, "Release 9"},
};
const ValueName LtePhyBplmnCellRequest_BarredStatus [] = {
    {0, "Not Barred"},
    {1, "Cell Barred"},
};

static int _decode_lte_phy_bplmn_cell_request_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 4:
        {
            offset += _decode_by_fmt(LtePhyBplmnCellRequest_Payload_v4,
                    ARRAY_SIZE(LtePhyBplmnCellRequest_Payload_v4, Fmt),
                    b, offset, length, result);
            int iStandrdsVersion = _search_result_int(result,
                    "Standards Version");
            (void) _map_result_field_to_name(result,
                    "Standards Version",
                    LtePhyBplmnCellRequest_StandardsVersion,
                    ARRAY_SIZE(LtePhyBplmnCellRequest_StandardsVersion, ValueName),
                    "(MI)Unknown");
            int iNonDecodeP1 = _search_result_int(result, "Cell ID");
            int iCellID = iNonDecodeP1 & 1023; // last 10 bits
            int iBarredStatus = (iNonDecodeP1 >> 10) & 15; // next 4 bits
            PyObject *old_object = _replace_result_int(result, "Cell ID",
                    iCellID);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Barred Status",
                    iBarredStatus);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result,
                    "Barred Status",
                    LtePhyBplmnCellRequest_BarredStatus,
                    ARRAY_SIZE(LtePhyBplmnCellRequest_BarredStatus, ValueName),
                    "(MI)Unknown");
            unsigned int iNonDecodeP2 = _search_result_uint(result, "Q Rx Lev Min");
            int iQRxLevMin = iNonDecodeP2 & 255; // last 8 bits
            int iQRxLevMinOffset = (iNonDecodeP2 >> 8) & 15; // next 4 bits
            int iPMax = (iNonDecodeP2 >> 12) & 255; // next 8 bits
            iQRxLevMin -= 256;
            old_object = _replace_result_int(result, "Q Rx Lev Min", iQRxLevMin);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Q Rx Lev Min Offset",
                    iQRxLevMinOffset);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "P Max", iPMax);
            Py_DECREF(old_object);

            if (iStandrdsVersion == 1) {
                offset += _decode_by_fmt(LtePhyBplmnCellRequest_Rel9Info,
                        ARRAY_SIZE(LtePhyBplmnCellRequest_Rel9Info, Fmt),
                        b, offset, length, result);
                int iR9QMinData = (iNonDecodeP2 >> 20) & 63; // last 6 bits
                int iR9QMinOffset = (iNonDecodeP2 >> 26) & 63; // next 6 bits
                old_object = _replace_result_int(result,
                        "Rel 9 Info Q Qual Min Data", iR9QMinData);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result,
                        "Rel 9 Info Q Qual Min Offset", iR9QMinOffset);
                Py_DECREF(old_object);
            }

            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PHY BPLMN Cell Request version: 0x%x\n", pkt_ver);
        return 0;
    }
}
