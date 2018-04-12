/*
 * LTE PHY PUSCH CSF
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePhyPuschCsf_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePhyPuschCsf_Payload_v22 [] = {
    {UINT, "Start System Sub-frame Number", 2}, // 4 bits
    {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
    {UINT, "PUSCH Reporting Mode", 1},  // right shift 1 bit, 3 bits
    {PLACEHOLDER, "Rank Index", 0}, // 1 bit
    {UINT, "Number of Subbands", 4},    // 5 bits
    {PLACEHOLDER, "WideBand CQI CW0", 0},   // 4 bits
    {PLACEHOLDER, "WideBand CQI CW1", 0},   // 4 bits
    {PLACEHOLDER, "SubBand Size (k)", 0},   // 7 bits
    {PLACEHOLDER, "Single WB PMI", 0},  // 4 bits
    {PLACEHOLDER, "Single MB PMI", 0},  // 4 bits
    {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
    {SKIP, NULL, 27},
    {UINT, "Carrier Index", 1}, // 4 bits
};

const Fmt LtePhyPuschCsf_Payload_v23 [] = {
    {UINT, "Start System Sub-frame Number", 2}, // 4 bits
    {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
    {UINT, "PUSCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
    {PLACEHOLDER, "Rank Index", 0}, // 1 bit
    {UINT, "Number of Subbands", 4},    // 5 bits
    {PLACEHOLDER, "WideBand CQI CW0", 0},   // 4 bits
    {PLACEHOLDER, "WideBand CQI CW1", 0},   // 4 bits
    {PLACEHOLDER, "SubBand Size (k)", 0},   // 7 bits
    {PLACEHOLDER, "Single WB PMI", 0},  // 4 bits
    {PLACEHOLDER, "Single MB PMI", 0},  // 4 bits
    {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
    {SKIP, NULL, 27},
    {UINT, "Carrier Index", 1}, // 4 bits
    {PLACEHOLDER, "Num CSIrs Ports", 0},    // 4 bits
};

const Fmt LtePhyPuschCsf_Payload_v42 [] = {
    {UINT, "Start System Sub-frame Number", 2}, // 4 bits
    {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
    {UINT, "PUSCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
    {PLACEHOLDER, "Rank Index", 0}, // 1 bit
    {PLACEHOLDER, "Csi Meas Set Index", 0},    // 1 bit
    {UINT, "Number of Subbands", 4},    // 5 bits
    {PLACEHOLDER, "WideBand CQI CW0", 0},   // 4 bits
    {PLACEHOLDER, "WideBand CQI CW1", 0},   // 4 bits
    {PLACEHOLDER, "SubBand Size (k)", 0},   // 7 bits
    {PLACEHOLDER, "Single WB PMI", 0},  // 4 bits
    {PLACEHOLDER, "Single MB PMI", 0},  // 4 bits
    {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
    {SKIP, NULL, 27},
    {UINT, "Carrier Index", 1}, // 4 bits
    {PLACEHOLDER, "Num CSIrs Ports", 0},    // 4 bits
};

const Fmt LtePhyPuschCsf_Payload_v101 [] = {
    {UINT, "Start System Sub-frame Number", 2}, // 4 bits
    {PLACEHOLDER, "Start System Frame Number", 0},  // 10 bits
    {UINT, "PUSCH Reporting Mode", 1},  // last 2 bits in previous byte + 1 bit
    {PLACEHOLDER, "Csi Meas Set Index", 0},    // 1 bit
    {PLACEHOLDER, "Rank Index", 0}, // 6 bit
    {UINT, "Number of Subbands", 4},    // 5 bits
    {PLACEHOLDER, "WideBand CQI CW0", 0},   // 4 bits
    {PLACEHOLDER, "WideBand CQI CW1", 0},   // 4 bits
    {PLACEHOLDER, "SubBand Size (k)", 0},   // 7 bits
    {PLACEHOLDER, "Single WB PMI", 0},  // 4 bits
    {PLACEHOLDER, "Single MB PMI", 0},  // 4 bits
    {PLACEHOLDER, "CSF Tx Mode", 0},    // 4 bits
    {SKIP, NULL, 27},
    {UINT, "Carrier Index", 1}, // 4 bits
    {PLACEHOLDER, "Num CSIrs Ports", 0},    // 4 bits
};

static int _decode_lte_phy_pusch_csf_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;
    int temp;

    switch (pkt_ver) {
    case 101:
        {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v101,
                    ARRAY_SIZE(LtePhyPuschCsf_Payload_v101, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Start System Sub-frame Number");
            int iSubFN = temp & 15;
            int iSysFN = (temp >> 4) & 1023;
            int ileft = (temp >> 14) & 3;
            temp = _search_result_int(result, "PUSCH Reporting Mode");
            int iPuschReportingMode = ((temp & 1) << 2) | ileft;
            int iCsiMeasSetIndex = (temp >> 1) & 1;
            int iRankIndex = (temp >> 2) & 63;
            unsigned int utemp = _search_result_uint(result, "Number of Subbands");
            int iNumSubbands = utemp & 31;
            int iWideBandCQICW0 = (utemp >> 5) & 15;
            int iWideBandCQICW1 = (utemp >> 9) & 15;
            int iSubBandSize = (utemp >> 13) & 127;
            int iSingleWBPMI = (utemp >> 20) & 15;
            int iSingleMBPMI = (utemp >> 24) & 15;
            int iCSFTxMode = (utemp >> 28) & 15;
            temp = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = temp & 15;
            int iNumCSIrsPorts = (temp >> 4) & 15;

            old_object = _replace_result_int(result,
                    "Start System Sub-frame Number", iSubFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Start System Frame Number", iSysFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "PUSCH Reporting Mode",
                    iPuschReportingMode);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "PUSCH Reporting Mode",
                    ValueNamePuschReportingMode,
                    ARRAY_SIZE(ValueNamePuschReportingMode, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Rank Index",
                    iRankIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Rank Index",
                    ValueNameRankIndex,
                    ARRAY_SIZE(ValueNameRankIndex, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Csi Meas Set Index",
                    iCsiMeasSetIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Csi Meas Set Index",
                    ValueNameCsiMeasSetIndex,
                    ARRAY_SIZE(ValueNameCsiMeasSetIndex, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Number of Subbands",
                    iNumSubbands);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "WideBand CQI CW0",
                    iWideBandCQICW0);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "WideBand CQI CW1",
                    iWideBandCQICW1);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "SubBand Size (k)",
                    iSubBandSize);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Single WB PMI",
                    iSingleWBPMI);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Single MB PMI",
                    iSingleMBPMI);
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
            old_object = _replace_result_int(result, "Num CSIrs Ports",
                    iNumCSIrsPorts);
            Py_DECREF(old_object);

            return offset - start;
        }

    case 42:
        {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v42,
                    ARRAY_SIZE(LtePhyPuschCsf_Payload_v42, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Start System Sub-frame Number");
            int iSubFN = temp & 15;
            int iSysFN = (temp >> 4) & 1023;
            int ileft = (temp >> 14) & 3;
            temp = _search_result_int(result, "PUSCH Reporting Mode");
            int iPuschReportingMode = ((temp & 1) << 2) | ileft;
            int iRankIndex = (temp >> 1) & 1;
            int iCsiMeasSetIndex = (temp >> 2) & 1;
            unsigned int utemp = _search_result_uint(result, "Number of Subbands");
            int iNumSubbands = utemp & 31;
            int iWideBandCQICW0 = (utemp >> 5) & 15;
            int iWideBandCQICW1 = (utemp >> 9) & 15;
            int iSubBandSize = (utemp >> 13) & 127;
            int iSingleWBPMI = (utemp >> 20) & 15;
            int iSingleMBPMI = (utemp >> 24) & 15;
            int iCSFTxMode = (utemp >> 28) & 15;
            temp = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = temp & 15;
            int iNumCSIrsPorts = (temp >> 4) & 15;

            old_object = _replace_result_int(result,
                    "Start System Sub-frame Number", iSubFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Start System Frame Number", iSysFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "PUSCH Reporting Mode",
                    iPuschReportingMode);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "PUSCH Reporting Mode",
                    ValueNamePuschReportingMode,
                    ARRAY_SIZE(ValueNamePuschReportingMode, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Rank Index",
                    iRankIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Rank Index",
                    ValueNameRankIndex,
                    ARRAY_SIZE(ValueNameRankIndex, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Csi Meas Set Index",
                    iCsiMeasSetIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Csi Meas Set Index",
                    ValueNameCsiMeasSetIndex,
                    ARRAY_SIZE(ValueNameCsiMeasSetIndex, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Number of Subbands",
                    iNumSubbands);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "WideBand CQI CW0",
                    iWideBandCQICW0);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "WideBand CQI CW1",
                    iWideBandCQICW1);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "SubBand Size (k)",
                    iSubBandSize);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Single WB PMI",
                    iSingleWBPMI);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Single MB PMI",
                    iSingleMBPMI);
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
            old_object = _replace_result_int(result, "Num CSIrs Ports",
                    iNumCSIrsPorts);
            Py_DECREF(old_object);

            return offset - start;
        }

    case 23:
        {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v23,
                    ARRAY_SIZE(LtePhyPuschCsf_Payload_v23, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Start System Sub-frame Number");
            int iSubFN = temp & 15;
            int iSysFN = (temp >> 4) & 1023;
            int ileft = (temp >> 14) & 3;
            temp = _search_result_int(result, "PUSCH Reporting Mode");
            int iPuschReportingMode = ((temp & 1) << 2) | ileft;
            int iRankIndex = (temp >> 1) & 1;
            unsigned int utemp = _search_result_uint(result, "Number of Subbands");
            int iNumSubbands = utemp & 31;
            int iWideBandCQICW0 = (utemp >> 5) & 15;
            int iWideBandCQICW1 = (utemp >> 9) & 15;
            int iSubBandSize = (utemp >> 13) & 127;
            int iSingleWBPMI = (utemp >> 20) & 15;
            int iSingleMBPMI = (utemp >> 24) & 15;
            int iCSFTxMode = (utemp >> 28) & 15;
            temp = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = temp & 15;
            int iNumCSIrsPorts = (temp >> 4) & 15;

            old_object = _replace_result_int(result,
                    "Start System Sub-frame Number", iSubFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Start System Frame Number", iSysFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "PUSCH Reporting Mode",
                    iPuschReportingMode);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "PUSCH Reporting Mode",
                    ValueNamePuschReportingMode,
                    ARRAY_SIZE(ValueNamePuschReportingMode, ValueName),
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
            old_object = _replace_result_int(result, "WideBand CQI CW0",
                    iWideBandCQICW0);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "WideBand CQI CW1",
                    iWideBandCQICW1);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "SubBand Size (k)",
                    iSubBandSize);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Single WB PMI",
                    iSingleWBPMI);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Single MB PMI",
                    iSingleMBPMI);
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
            old_object = _replace_result_int(result, "Num CSIrs Ports",
                    iNumCSIrsPorts);
            Py_DECREF(old_object);

            return offset - start;
        }

    case 22:
        {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v22,
                    ARRAY_SIZE(LtePhyPuschCsf_Payload_v22, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Start System Sub-frame Number");
            int iSubFN = temp & 15;
            int iSysFN = (temp >> 4) & 1023;
            temp = _search_result_int(result, "PUSCH Reporting Mode");
            int iPuschReportingMode = (temp >> 1) & 7;
            int iRankIndex = (temp >> 4) & 1;
            unsigned int utemp = _search_result_uint(result, "Number of Subbands");
            int iNumSubbands = utemp & 31;
            int iWideBandCQICW0 = (utemp >> 5) & 15;
            int iWideBandCQICW1 = (utemp >> 9) & 15;
            int iSubBandSize = (utemp >> 13) & 127;
            int iSingleWBPMI = (utemp >> 20) & 15;
            int iSingleMBPMI = (utemp >> 24) & 15;
            int iCSFTxMode = (utemp >> 28) & 15;
            temp = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = temp & 15;

            old_object = _replace_result_int(result,
                    "Start System Sub-frame Number", iSubFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Start System Frame Number", iSysFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "PUSCH Reporting Mode",
                    iPuschReportingMode);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "PUSCH Reporting Mode",
                    ValueNamePuschReportingMode,
                    ARRAY_SIZE(ValueNamePuschReportingMode, ValueName),
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
            old_object = _replace_result_int(result, "WideBand CQI CW0",
                    iWideBandCQICW0);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "WideBand CQI CW1",
                    iWideBandCQICW1);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "SubBand Size (k)",
                    iSubBandSize);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Single WB PMI",
                    iSingleWBPMI);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Single MB PMI",
                    iSingleMBPMI);
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
        printf("(MI)Unknown LTE PHY PUSCH CSF version: 0x%x\n", pkt_ver);
        return 0;
    }
}
