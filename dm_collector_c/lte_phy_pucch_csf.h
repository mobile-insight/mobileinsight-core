/*
 * LTE PHY PUCCH CSF
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePhyPucchCsf_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePhyPucchCsf_Payload_v22 [] = {
    {UINT, "Start System Sub-frame Number", 2}, // 4 bits
    {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
    {UINT, "PUCCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
    {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
    {PLACEHOLDER, "Size BWP",0},
    {UINT, "Number of Subbands", 4},    // 4 bits
    {PLACEHOLDER, "BWP Index",0},
    {PLACEHOLDER, "Rank Index", 0}, // skip 3 bits, 1 bit
    {PLACEHOLDER, "SubBand Label",0},
    {PLACEHOLDER, "CQI CW0", 0},   // skip 2 bits, 4 bits
    {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
    {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits
    {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
    {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
};

const Fmt LtePhyPucchCsf_Payload_v24 [] = {
    {UINT, "Start System Sub-frame Number", 2}, // 4 bits
    {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
    {UINT, "PUCCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
    {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
    {PLACEHOLDER, "Size BWP",0},
    {UINT, "Number of Subbands", 4},    // 4 bits
    {PLACEHOLDER, "BWP Index",0},
    {PLACEHOLDER, "Rank Index", 0}, // skip 3 bits, 1 bit
    {PLACEHOLDER, "SubBand Label",0},
    {PLACEHOLDER, "CQI CW0", 0},   // skip 2 bits, 4 bits
    {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
    {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits
    {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
    {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
    {UINT, "Num Csirs Ports", 1},
    {PLACEHOLDER, "Wideband PMI1", 0},
    {UINT,"Pti",1},
    {SKIP, NULL, 2},
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

const Fmt LtePhyPucchCsf_Payload_v102 [] = {
    {UINT, "Start System Sub-frame Number", 2}, // 4 bits
    {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
    {UINT, "PUCCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
    {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
    {UINT, "Number of Subbands", 4},    // 4 bits
    {PLACEHOLDER,"Alt Cqi Table Data",0},
    {PLACEHOLDER, "CQI CW0", 0},   // skip 2 bits, 4 bits
    {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
    {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits
    {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
    {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
    {UINT,"Num Csirs Ports",1},
    {UINT, "Csi Meas Set Index",1},
    {PLACEHOLDER, "Rank Index",0},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPucchCsf_Payload_v103 [] = {
    {UINT, "Start System Sub-frame Number", 2}, // 4 bits
    {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
    {UINT, "PUCCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
    {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
    {UINT, "Number of Subbands", 4},    // 4 bits
    {PLACEHOLDER,"Alt Cqi Table Data",0},
    {PLACEHOLDER, "CQI CW0", 0},   // skip 2 bits, 4 bits
    {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
    {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits
    {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
    {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
    {UINT,"Num Csirs Ports",1},
    {UINT, "Csi Meas Set Index",1},
    {PLACEHOLDER, "Rank Index",0},
    {UINT, "Forced Max RI",1},
    {SKIP, NULL, 1},
};

const Fmt LtePhyPucchCsf_Payload_v142 [] = {
    {UINT, "Start System Sub-frame Number", 4}, // 4 bits
    {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
    {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
    {PLACEHOLDER, "Scell Index",0},     // 5 bits
    {PLACEHOLDER, "PUCCH Reporting Mode", 0},  //reserve 1 bit, size 2
    {PLACEHOLDER, "PUCCH Report Type", 0},    // 4 bit
    {PLACEHOLDER, "Size BWP",0}, //3 bits, 1 bit in next

    {UINT, "Number of Subbands", 1},    // right shift 1 bit, 4 bits
    {PLACEHOLDER, "BWP Index",0}, //3 bits

    {UINT,"Alt Cqi Table Data",2}, //1 bit
    {PLACEHOLDER, "SubBand Label",0}, //2 bits
    {PLACEHOLDER, "CQI CW0", 0},   // 4 bits
    {PLACEHOLDER, "CQI CW1", 0},   // 4 bits
    {PLACEHOLDER, "Wideband PMI", 0},   // 4 bits

    {UINT, "CSF Tx Mode", 2},    // 4 bits
    {PLACEHOLDER, "PUCCH Reporting Submode", 0},  //size 2
    {PLACEHOLDER,"Num Csirs Ports",0}, //4 bits
    {PLACEHOLDER, "Wideband PMI1",0},  //4 bits
    {PLACEHOLDER, "Pti",0}, //1 bit
    {PLACEHOLDER, "Csi Meas Set Index",0}, //1 bit

    {UINT, "Rank Index",2}, //2 bits
    {PLACEHOLDER, "CRI",0}, //3 bits
    {PLACEHOLDER, "UL Frame Number",0}, //10 bits
    {PLACEHOLDER, "UL Channel Type",0}, //1 bit

    {UINT, "UL Subframe Number",4}, //4 bits
    {PLACEHOLDER, "UL Payload Length",0}, //5 bits,
};

static int _decode_lte_phy_pucch_csf_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;
    int temp;

    switch (pkt_ver) {
    case 22:
        {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v22,
                    ARRAY_SIZE(LtePhyPucchCsf_Payload_v22, Fmt),
                    b, offset, length, result);

            temp = _search_result_int(result, "Start System Sub-frame Number");
            int iSubFN = temp & 15;
            int iSysFN = (temp >> 4) & 1023;

            temp = _search_result_int(result, "PUCCH Reporting Mode");
            int iPucchReportingMode = (temp >>1) & 0x3;
            int iPucchReportType = (temp >> 3) & 0x3;
            int iSizeBWP =(temp>>5) & 7;

            unsigned int utemp = _search_result_uint(result, "Number of Subbands");
            int iNumSubbands = utemp & 15;
            int iBWPIndex = (utemp >>4) & 7;
            int iRankIndex = (utemp >> 7) & 1;
            int iSubBandLabel = (utemp >>8) & 3;
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
                    ValueNamePucchReportingMode_v22,
                    ARRAY_SIZE(ValueNamePucchReportingMode_v22, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "PUCCH Report Type",
                    iPucchReportType);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "PUCCH Report Type",
                    ValueNamePucchReportType_v22,
                    ARRAY_SIZE(ValueNamePucchReportType_v22, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Size BWP",
                    iSizeBWP);
            Py_DECREF(old_object);
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
            old_object = _replace_result_int(result, "BWP Index",
                    iBWPIndex);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "SubBand Label",
                    iSubBandLabel);
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
    case 24:
        {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v24,
                    ARRAY_SIZE(LtePhyPucchCsf_Payload_v24, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Start System Sub-frame Number");
            int iSubFN = temp & 15;
            int iSysFN = (temp >> 4) & 1023;
            int ileft = (temp >> 14) & 3;
            temp = _search_result_int(result, "PUCCH Reporting Mode");
            int iPucchReportingMode = ((temp & 1) << 2) | ileft;
            int iPucchReportType = (temp >> 1) & 15;
            int iSizeBWP =(temp>>5) & 7;

            unsigned int utemp = _search_result_uint(result, "Number of Subbands");
            int iNumSubbands = utemp & 15;
            int iBWPIndex = (utemp >>4) & 7;
            int iRankIndex = (utemp >> 7) & 1;
            int iSubBandLabel = (utemp >>8) & 3;
            int iCQI0 = (utemp >> 10) & 15;
            int iCQI1 = (utemp >> 14) & 15;
            int iWidebandPMI = (utemp >> 18) & 15;
            int iCarrierIndex = (utemp >> 22) & 15;
            int iCSFTxMode = (utemp >> 26) & 15;

            utemp =_search_result_uint(result, "Num Csirs Ports");
            int iNumCsirsPorts=utemp & 0xf;
            int iWidebandPMI1 = (utemp>>4) & 0xf;

            utemp =_search_result_uint(result, "Pti");
            int iPti = utemp & 0x1;

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
            old_object = _replace_result_int(result, "Size BWP",
                    iSizeBWP);
            Py_DECREF(old_object);
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
            old_object = _replace_result_int(result, "BWP Index",
                    iBWPIndex);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "SubBand Label",
                    iSubBandLabel);
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

            old_object = _replace_result_int(result, "Num Csirs Ports",
                    iNumCsirsPorts);
            Py_DECREF(old_object);

            old_object = _replace_result_int(result, "Wideband PMI1",
                    iWidebandPMI1);
            Py_DECREF(old_object);

            old_object = _replace_result_int(result, "Pti",
                    iPti);
            Py_DECREF(old_object);

            return offset - start;
        }
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
    case 101:
    case 102:
        {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v102,
                    ARRAY_SIZE(LtePhyPucchCsf_Payload_v102, Fmt),
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
            int iAltCqiTableData=(utemp >> 7) & 1;
            int iCQI0 = (utemp >> 10) & 15;
            int iCQI1 = (utemp >> 14) & 15;
            int iWidebandPMI = (utemp >> 18) & 15;
            int iCarrierIndex = (utemp >> 22) & 15;
            int iCSFTxMode = (utemp >> 26) & 15;

            temp = _search_result_int(result, "Num Csirs Ports");
            int iNumCsirsPorts=temp & 15;
            old_object = _replace_result_int(result,
                    "Num Csirs Ports", iNumCsirsPorts);
            Py_DECREF(old_object);

            old_object = _replace_result_int(result,
                    "Alt Cqi Table Data", iAltCqiTableData);
            Py_DECREF(old_object);

            temp = _search_result_int(result, "Csi Meas Set Index");
            int iCsiMeasSetIndex = (temp>>1) & 0x1;
            int iRankIndex = (temp >> 2) & 0x3;

            old_object = _replace_result_int(result,
                    "Csi Meas Set Index", iCsiMeasSetIndex);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Rank Index", iRankIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Rank Index",
                    ValueNameRankIndex,
                    ARRAY_SIZE(ValueNameRankIndex, ValueName),
                    "(MI)Unknown");

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
    case 103:
        {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v103,
                    ARRAY_SIZE(LtePhyPucchCsf_Payload_v103, Fmt),
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
            int iAltCqiTableData=(utemp >> 7) & 1;
            int iCQI0 = (utemp >> 10) & 15;
            int iCQI1 = (utemp >> 14) & 15;
            int iWidebandPMI = (utemp >> 18) & 15;
            int iCarrierIndex = (utemp >> 22) & 15;
            int iCSFTxMode = (utemp >> 26) & 15;

            temp = _search_result_int(result, "Num Csirs Ports");
            int iNumCsirsPorts=temp & 15;
            old_object = _replace_result_int(result,
                    "Num Csirs Ports", iNumCsirsPorts);
            Py_DECREF(old_object);

            old_object = _replace_result_int(result,
                    "Alt Cqi Table Data", iAltCqiTableData);
            Py_DECREF(old_object);

            temp = _search_result_int(result, "Csi Meas Set Index");
            int iCsiMeasSetIndex = (temp>>1) & 0x1;
            int iRankIndex = (temp >> 2) & 0x3;
            int iForcedMaxRI = (temp >> 4) & 0xf;

            temp = _search_result_int(result, "Forced Max RI");
            iForcedMaxRI = iForcedMaxRI + ((temp & 0xf)<<4);

            old_object = _replace_result_int(result,
                    "Csi Meas Set Index", iCsiMeasSetIndex);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Rank Index", iRankIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Rank Index",
                    ValueNameRankIndex,
                    ARRAY_SIZE(ValueNameRankIndex, ValueName),
                    "(MI)Unknown");

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
            old_object = _replace_result_int(result, "Forced Max RI",
                    iForcedMaxRI);
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
            (void) _map_result_field_to_name(result, "Csi Meas Set Index",
                    ValueNameCsiMeasSetIndex,
                    ARRAY_SIZE(ValueNameCsiMeasSetIndex, ValueName),
                    "(MI)Unknown");
            return offset - start;
        }
    case 142:
        {
            offset += _decode_by_fmt(LtePhyPucchCsf_Payload_v142,
                    ARRAY_SIZE(LtePhyPucchCsf_Payload_v142, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Start System Sub-frame Number");
            int iSubFN = temp & 15;// 4 bits
            int iSysFN = (temp >> 4) & 1023;// 10 bits
            int iCarrierIndex = (temp >> 14) & 15;//4 bits
            int iScellIndex = (temp>>18) & 31;// 5 bits
            int iPucchReportingMode = (temp>>24) & 3; // reserve 1 bit ,2 bits
            int iPucchReportType = (temp >> 26) & 15; // 4 bits
            int iSizeBWP = (temp>>30) & 3;

            unsigned int utemp = _search_result_uint(result, "Number of Subbands");
            iSizeBWP += ((utemp & 1)<<2);
            int iNumSubbands = (utemp>>1) & 15;
            int iBWPIndex = (utemp>>5) & 7;

            old_object = _replace_result_int(result,
                    "Start System Sub-frame Number", iSubFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Start System Frame Number", iSysFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Carrier Index",
                    iCarrierIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Carrier Index",
                    ValueNameCarrierIndex,
                    ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Scell Index",
                    iScellIndex);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "PUCCH Reporting Mode",
                    iPucchReportingMode);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "PUCCH Reporting Mode",
                    ValueNamePucchReportingMode_v22,
                    ARRAY_SIZE(ValueNamePucchReportingMode_v22, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "PUCCH Report Type",
                    iPucchReportType);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "PUCCH Report Type",
                    ValueNamePucchReportType,
                    ARRAY_SIZE(ValueNamePucchReportType, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Size BWP",
                    iSizeBWP);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Number of Subbands",
                    iNumSubbands);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "BWP Index",
                    iBWPIndex);
            Py_DECREF(old_object);

            utemp = _search_result_uint(result, "Alt Cqi Table Data");
            int iAltCqiTableData = utemp & 1;
            int iSubBandLabel = (utemp>>1) & 3;
            int iCQI0 = (utemp>>3) & 15;
            int iCQI1 = (utemp>>7) & 15;
            int iWidebandPMI = (utemp>>11) & 15;

            old_object = _replace_result_int(result, "Alt Cqi Table Data",
                    iAltCqiTableData);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "SubBand Label",
                    iSubBandLabel);
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

            utemp = _search_result_uint(result, "CSF Tx Mode");
            int iCSFTxMode = utemp & 15;
            int iPUCCHReportingSubmode = (utemp>>4) & 3;
            int iNumCsirsPorts = (utemp>>6) & 15;
            int iWidebandPMI1 = (utemp >> 10) & 15;
            int iPti = (utemp >>14) & 1;
            int iCsiMeasSetIndex =(utemp>>15) & 1;

            old_object = _replace_result_int(result, "CSF Tx Mode",
                    iCSFTxMode);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "CSF Tx Mode",
                    ValueNameCSFTxMode,
                    ARRAY_SIZE(ValueNameCSFTxMode, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "PUCCH Reporting Submode",
                    iPUCCHReportingSubmode);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Num Csirs Ports", iNumCsirsPorts);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Wideband PMI1", iWidebandPMI1);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Pti", iPti);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Csi Meas Set Index", iCsiMeasSetIndex);
            Py_DECREF(old_object);

            utemp = _search_result_uint(result, "Rank Index");
            int iRankIndex = (utemp) & 3;
            int iCRI = (utemp>>2) & 7;
            int iULFrameNumber = (utemp>>5) & 0x3ff;
            int iULChannelType = (utemp>>15) & 1;

            old_object = _replace_result_int(result,
                    "Rank Index", iRankIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Rank Index",
                    ValueNameRankIndex,
                    ARRAY_SIZE(ValueNameRankIndex, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result,
                    "CRI", iCRI);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "UL Frame Number", iULFrameNumber);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "UL Channel Type", iULChannelType);
            Py_DECREF(old_object);

            utemp = _search_result_uint(result, "UL Subframe Number");
            int iULSubFrameNumber = (utemp) & 0xf;
            int iULPayloadLength = (utemp>>4) & 0x1f;

            old_object = _replace_result_int(result,
                    "UL Subframe Number", iULSubFrameNumber);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "UL Payload Length", iULPayloadLength);
            Py_DECREF(old_object);

            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PHY PUCCH CSF version: 0x%x\n", pkt_ver);
        return 0;
    }
}
