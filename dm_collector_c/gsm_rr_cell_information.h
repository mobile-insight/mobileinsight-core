/*
 * GSM_RR_Cell_Information
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt GsmRrCellInfo_Fmt [] = {
    {UINT, "BCCH ARFCN", 2},    // 12 bits
    {UINT, "BSIC-NCC", 1},
    {UINT, "BSIC-BCC", 1},
    {UINT, "Cell ID", 2},
    {BYTE_STREAM_LITTLE_ENDIAN, "MCC-MNC", 3},
    {BYTE_STREAM_LITTLE_ENDIAN, "LAC", 2},
    {UINT, "Cell Selection Priority", 1},
    {BYTE_STREAM, "NCC Permitted", 1},
};

static int _decode_gsm_rci_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int temp = _search_result_int(result, "BCCH ARFCN");
    int iArfcn = temp & 4095;
    PyObject *old_object = _replace_result_int(result, "BCCH ARFCN", iArfcn);
    Py_DECREF(old_object);
    (void) _map_result_field_to_name(result, "Cell Selection Priority",
            ValueNameNormalOrNot,
            ARRAY_SIZE(ValueNameNormalOrNot, ValueName),
            "(MI)Unknown");
    return offset;
}
