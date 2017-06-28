/*
 * LTE PHY PUCCH CSF
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePhyPucchCsf_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePhyPucchCsf_Payload_v43 [] = {
    {UINT, "Start System Sub-frame Number", 2}, // 4 bits
    {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
    {UINT, "PUCCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
    {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
    {UINT, "Number of Subbands", 4},    // 4 bits
    {PLACEHOLDER, "Rank Index", 0}, // skip 3 bits, 1 bit
    {PLACEHOLDER, "CQI CW0", 0},   // skip 2 bits, 4 bits
    {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
    {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits
    {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
    {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
    {SKIP, NULL, 4},
};

static int _decode_lte_phy_pucch_csf_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;
    int temp;

    switch (pkt_ver) {
    case 43:
        {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v43,
                    ARRAY_SIZE(LtePhyPucchCsf_Payload_v43, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Start System Sub-frame Number");
            int iSubFN = temp & 15;
            int iSysFN = (temp >> 4) & 1023;
            int ileft = (temp >> 14) & 3;
            temp = _search_result_int(result, "PUCCH Reporting Mode");
            int iPucchReportingMode = ((temp & 1) << 2) | ileft;
            int iPucchReportType = (temp >> 1) & 15;
            unsigned int utemp = _search_result_uint(result, "Number of Subbands");
            int iNumSubbands = utemp & 15;
            int iRankIndex = (utemp >> 7) & 1;
            int iCQI0 = (utemp >> 10) & 15;
            int iCQI1 = (utemp >> 14) & 15;
            int iWidebandPMI = (utemp >> 18) & 15;
            int iCarrierIndex = (utemp >> 22) & 15;
            int iCSFTxMode = (utemp >> 26) & 15;

            old_object = _replace_result_int(result,
                    "Start System Sub-frame Number", iSubFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Start System Frame Number", iSysFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "PUCCH Reporting Mode",
                    iPucchReportingMode);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "PUCCH Reporting Mode",
                    ValueNamePucchReportingMode,
                    ARRAY_SIZE(ValueNamePucchReportingMode, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "PUCCH Report Type",
                    iPucchReportType);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "PUCCH Report Type",
                    ValueNamePucchReportType,
                    ARRAY_SIZE(ValueNamePucchReportType, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Rank Index",
                    iRankIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Rank Index",
                    ValueNameRankIndex,
                    ARRAY_SIZE(ValueNameRankIndex, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Number of Subbands",
                    iNumSubbands);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "CQI CW0",
                    iCQI0);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "CQI CW1",
                    iCQI1);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Wideband PMI",
                    iWidebandPMI);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "CSF Tx Mode",
                    iCSFTxMode);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "CSF Tx Mode",
                    ValueNameCSFTxMode,
                    ARRAY_SIZE(ValueNameCSFTxMode, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Carrier Index",
                    iCarrierIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Carrier Index",
                    ValueNameCarrierIndex,
                    ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                    "(MI)Unknown");
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PHY PUCCH CSF version: 0x%x\n", pkt_ver);
        return 0;
    }
}
