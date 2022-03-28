/*
 * LTE NB1 ML1 Sum Sys Info
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LteNb1Ml1SumSysInfoFmt[] = {
    {UINT, "Version", 1},
};

const Fmt LteNb1Ml1SumSysInfoFmt_v1[] = {
    {UINT, "OP Mode", 2},    // 2 bits
    {PLACEHOLDER, "Meas BW",0},    // 4 bits
    {PLACEHOLDER, "Cell Id", 0},          // 10 bits
    {SKIP, NULL, 1},                  //Reserved
    {UINT, "Frequency", 4},          // 32 bits
    {UINT, "Inst Meas RSRP", 4},     // 11 bits
    {PLACEHOLDER, "Srxlev", 0},          // 7 bits
};


static int _decode_lte_nb1_ml1_sum_sys_info_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;

    switch (pkt_ver) {
        case 1:
        {
        	offset += _decode_by_fmt(LteNb1Ml1SumSysInfoFmt_v1,
                    ARRAY_SIZE(LteNb1Ml1SumSysInfoFmt_v1, Fmt),
                    b, offset, length, result);
            unsigned int iNonDecodeOPMode = _search_result_uint(result, "OP Mode");
            int iOPMode = iNonDecodeOPMode & 3;          // 2 bits
            int iMeasBW = (iNonDecodeOPMode >> 2) & 15;   // 4 bits
            int iCellId = (iNonDecodeOPMode >> 6) & 1023;   // 10 bits

            old_object = _replace_result_int(result, "OP Mode",
                    iOPMode);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "OP Mode",
                        LteRrcMibMessageLogPacketFmt_OpModeType,
                        ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_OpModeType, ValueName),
                        "(MI)Unknown");
            old_object = _replace_result_int(result, "Meas BW",
                    iMeasBW);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Meas BW",
                        ValueNameNB1_Sum_Sys_Info_MeasBWType,
                        ARRAY_SIZE(ValueNameNB1_Sum_Sys_Info_MeasBWType, ValueName),
                        "(MI)Unknown");
            old_object = _replace_result_int(result, "Cell Id",
                    iCellId);
            Py_DECREF(old_object);

            unsigned int iNonDecodeRSRP = _search_result_uint(result, "Inst Meas RSRP");
            int iRSRP = (int(iNonDecodeRSRP & 0x7ff) << 21) >> 21;                // 11 bits
            int iSrxlev = (int((iNonDecodeRSRP >> 11) & 0x7f) << 25) >> 25 ;   // 7 bits

            old_object = _replace_result_int(result, "Inst Meas RSRP",
                    iRSRP);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Srxlev",
                    iSrxlev);
            Py_DECREF(old_object);

            return offset - start;
        }
        default:
            printf("(MI)Unknown LTE NB1 ML1 SUM Sys Info version: 0x%x\n", pkt_ver);
            return 0;
    }
}