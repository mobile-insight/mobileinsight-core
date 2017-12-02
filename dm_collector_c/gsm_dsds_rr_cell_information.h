/*
 * GSM_DSDS_RR_Cell_Information
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt GsmDsdsRrCellInfo_Fmt [] = {
    {UINT, "Subscription ID", 1},
    {UINT, "BCCH ARFCN", 2},    // 12 bits
    {UINT, "BSIC-NCC", 1},
    {UINT, "BSIC-BCC", 1},
    {UINT, "Cell ID", 2},
    {UINT, "MCC", 2},
    {UINT, "MNC", 1},
    {UINT, "LAC", 2},
    {UINT, "Cell Selection Priority", 1},
    {BYTE_STREAM, "NCC Permitted", 1},
};

static int _decode_gsm_drci_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int temp = _search_result_int(result, "BCCH ARFCN");
    int iArfcn = temp & 4095;
    PyObject *old_object = _replace_result_int(result, "BCCH ARFCN", iArfcn);
    Py_DECREF(old_object);
    temp = _search_result_int(result, "MCC");
    int mcc_3 = temp & 15;
    int mcc_2 = (temp >> 4) & 15;
    int mcc_1 = (temp >> 8) & 15;
    int mnc_1 = (temp >> 12) & 15;
    temp = _search_result_int(result, "MNC");
    int mnc_3 = temp & 15;
    int mnc_2 = (temp >> 4) & 15;
    int mcc = mcc_3 * 100 + mcc_2 * 10 + mcc_1;
    int mnc = mnc_3 * 100 + mnc_2 * 10 + mnc_1;
    old_object = _replace_result_int(result, "MCC", mcc);
    Py_DECREF(old_object);
    old_object = _replace_result_int(result, "MNC", mnc);
    Py_DECREF(old_object);

    temp = _search_result_int(result, "LAC");
    int lac_high = temp & 255;
    int lac_low = (temp >> 8) & 255;
    int lac = lac_high * 256 + lac_low;
    old_object = _replace_result_int(result, "LAC", lac);
    Py_DECREF(old_object);

    (void) _map_result_field_to_name(result, "Cell Selection Priority",
            ValueNameNormalOrNot,
            ARRAY_SIZE(ValueNameNormalOrNot, ValueName),
            "(MI)Unknown");
    return offset;
}
