/*
 * LTE NB1 ML1 Search PBCH Decode
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LteNb1Ml1SearchPbchDecodeFmt[] = {
    {UINT, "Version", 1},
    {SKIP, NULL, 3},
};

const Fmt LteNb1Ml1SearchPbchDecodeFmt_v1[] = {
    {UINT, "Payload", 4},
    {UINT, "Earfcn", 4}, 
    {UINT, "Cell ID", 2},  
    {UINT, "Decode Result", 1},    // 1 bit 
    {PLACEHOLDER, "OP Mode",0},    // 2 bits
    {PLACEHOLDER, "meas_bw", 0},          // 4 bits
    {PLACEHOLDER, "Access Barring Enabled", 0},          // 1 bits
    {UINT, "Num Tx Antenna", 1},
    {UINT, "SFN Msb", 1},      
    {UINT, "Hyper Sfn Lsb", 1},
    {UINT, "Scheduling Info SIB1", 1},
    {UINT, "System Info Value Tag", 1}, 
    {UINT, "Sfn Offset", 2}, 
    {UINT, "Hfn Offset", 2},     
};


static int _decode_lte_nb1_ml1_search_pbch_decode_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;

    switch (pkt_ver) {
        case 1:
        {
        	offset += _decode_by_fmt(LteNb1Ml1SearchPbchDecodeFmt_v1,
                    ARRAY_SIZE(LteNb1Ml1SearchPbchDecodeFmt_v1, Fmt),
                    b, offset, length, result);
            unsigned int iNonDecodeDecodeResult = _search_result_uint(result, "Decode Result");
            int iDecodeResult = iNonDecodeDecodeResult & 1;          // 1 bits
            int iOPMode = (iNonDecodeDecodeResult >> 1) & 3;          // 2 bits
            int iMeasBW = (iNonDecodeDecodeResult >> 3) & 15;   // 4 bits
            int iAcessBarringEnabled = (iNonDecodeDecodeResult >> 7) & 1;   // 1 bits

            old_object = _replace_result_int(result, "Decode Result",
                    iDecodeResult);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Decode Result",
                        ValueNamePassOrFail,
                        ARRAY_SIZE(ValueNamePassOrFail, ValueName),
                        "(MI)Unknown");
            old_object = _replace_result_int(result, "OP Mode",
                    iOPMode);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "OP Mode",
                        LteRrcMibMessageLogPacketFmt_OpModeType,
                        ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_OpModeType, ValueName),
                        "(MI)Unknown");
            old_object = _replace_result_int(result, "meas_bw",
                    iMeasBW);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "meas_bw",
                        ValueNameNB1_Sum_Sys_Info_MeasBWType,
                        ARRAY_SIZE(ValueNameNB1_Sum_Sys_Info_MeasBWType, ValueName),
                        "(MI)Unknown");
            old_object = _replace_result_int(result, "Access Barring Enabled",
                    iAcessBarringEnabled);
            Py_DECREF(old_object);


            return offset - start;
        }
        default:
            printf("(MI)Unknown LTE NB1 ML1 Search PBCH Decode version: 0x%x\n", pkt_ver);
            return 0;
    }
}