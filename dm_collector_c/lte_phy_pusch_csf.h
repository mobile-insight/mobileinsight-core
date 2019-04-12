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

const Fmt LtePhyPuschCsf_Payload_v102 [] = {
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

const Fmt LtePhyPuschCsf_Payload_v103 [] = {
    {UINT,"Start System Sub-frame Number",4}, 	// shift 0 bits,total 4 bits
    {PLACEHOLDER,"Start System Frame Number",0}, 	// shift 4 bits,total 10 bits
    {PLACEHOLDER,"PUSCH Reporting Mode",0}, 	// shift 14 bits,total 3 bits
    {PLACEHOLDER,"Csi Meas Set Index",0}, 	// shift 17 bits,total 1 bits
    {PLACEHOLDER,"Rank Index",0}, 	// shift 18 bits,total 2 bits
    {PLACEHOLDER,"Wideband PMI1",0}, 	// shift 20 bits,total 4 bits
    {PLACEHOLDER,"Number of subbands",0}, 	// shift 24 bits,total 5 bits
    {PLACEHOLDER,"WideBand CQI CW0",0}, 	// shift 29 bits,total 4 bits

    {UINT,"WideBand CQI CW1",2}, 	// shift 1 bits,total 4 bits
    {PLACEHOLDER,"SubBand Size (k)",0}, 	// shift 5 bits,total 4 bits
    {PLACEHOLDER,"Size M",0}, 	// shift 9 bits,total 3 bits
    {PLACEHOLDER,"Single WB PMI",0}, 	// shift 12 bits,total 4 bits

    {UINT,"Single MB PMI",1}, 	// shift 0 bits,total 4 bits
    {PLACEHOLDER,"CSF Tx Mode",0}, 	// shift 4 bits,total 4 bits

    {SKIP,NULL,14},

    {UINT,"Forced Max RI",1}, 	// shift 0 bits,total 8 bits
    {SKIP,NULL,5},

    {UINT,"Alt Cqi Table Data",1}, 	// shift 1 bits,total 1 bits
    {SKIP,NULL,7},

    {UINT,"Carrier Index",1}, 	// shift 0 bits,total 4 bits
    {PLACEHOLDER,"Num Csirs Ports",0}, 	// shift 4 bits,total 4 bits
};

const Fmt LtePhyPuschCsf_Payload_v142 [] = {
    {UINT,"Start System Sub-frame Number",4}, 	// shift 0 bits,total 4 bits
    {PLACEHOLDER,"Start System Frame Number",0}, 	// shift 4 bits,total 10 bits
    {PLACEHOLDER,"Carrier Index",0}, 	// shift 14 bits,total 4 bits
    {PLACEHOLDER,"Scell Index",0}, 	// shift 18 bits,total 5 bits
    //{PLACEHOLDER,"Reserved",0}, 	// shift 23 bits,total 1 bits
    {PLACEHOLDER,"PUSCH Reporting Mode",0}, 	// shift 24 bits,total 3 bits
    {PLACEHOLDER,"Csi Meas Set Index",0}, 	// shift 27 bits,total 1 bits
    {PLACEHOLDER,"Rank Index",0}, 	// shift 28 bits,total 2 bits
    {PLACEHOLDER,"Wideband PMI1",0}, 	// shift 30 bits,total 4 bits

    {UINT,"Number of subbands",4}, 	// shift 2 bits,total 5 bits
    {PLACEHOLDER,"WideBand CQI CW0",0}, 	// shift 7 bits,total 4 bits
    {PLACEHOLDER,"WideBand CQI CW1",0}, 	// shift 11 bits,total 4 bits
    {PLACEHOLDER,"SubBand Size (k)",0}, 	// shift 15 bits,total 4 bits
    {PLACEHOLDER,"Size M",0}, 	// shift 19 bits,total 3 bits
    //{PLACEHOLDER,"Reserved",0}, 	// shift 22 bits,total 2 bits
    {PLACEHOLDER,"Single WB PMI",0}, 	// shift 24 bits,total 4 bits
    {PLACEHOLDER,"Single MB PMI",0}, 	// shift 28 bits,total 4 bits

    {UINT,"CSF Tx Mode",1}, 	// shift 0 bits,total 4 bits

    {SKIP,NULL,25},//


    {UINT,"Num Csirs Ports",1}, 	// shift 0 bits,total 4 bits
    //{PLACEHOLDER,"Reserved",0}, 	// shift 4 bits,total 4 bits

    {UINT,"CRI",4}, 	// shift 0 bits,total 3 bits
    {PLACEHOLDER,"Reserved",0}, 	// shift 3 bits,total 1 bits
    {PLACEHOLDER,"UL Frame Number",0}, 	// shift 4 bits,total 10 bits
    {PLACEHOLDER,"UL Subframe Number",0}, 	// shift 14 bits,total 4 bits
    {PLACEHOLDER,"UL Payload Length",0}, 	// shift 18 bits,total 7 bits
    //{PLACEHOLDER,"Reserved5",0}, 	// shift 25 bits,total 11 bits
};

static int _decode_lte_phy_pusch_csf_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;
    int temp;

    switch (pkt_ver) {
     //copied from case 22 only, needs to be double checked
    case 142:
        {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v142,
                    ARRAY_SIZE(LtePhyPuschCsf_Payload_v142, Fmt),
                    b, offset, length, result);

            unsigned int utemp = _search_result_uint(result,"Start System Sub-frame Number");
            int iresulttemp =(utemp>>0) & 0xf;
            old_object = _replace_result_int(result, "Start System Sub-frame Number",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>4) & 0x3ff;
            old_object = _replace_result_int(result, "Start System Frame Number",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>14) & 0xf;
            old_object = _replace_result_int(result, "Carrier Index",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>18) & 0x1f;
            old_object = _replace_result_int(result, "Scell Index",iresulttemp);
            Py_DECREF(old_object);
            //iresulttemp =(utemp>>23) & 0x1;
            //old_object = _replace_result_int(result, "Reserved",iresulttemp);
            //Py_DECREF(old_object);
            iresulttemp =(utemp>>24) & 0x7;
            old_object = _replace_result_int(result, "PUSCH Reporting Mode",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>27) & 0x1;
            old_object = _replace_result_int(result, "Csi Meas Set Index",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>28) & 0x3;
            old_object = _replace_result_int(result, "Rank Index",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>30) & 0xf;
            old_object = _replace_result_int(result, "Wideband PMI1",iresulttemp);
            Py_DECREF(old_object);

            utemp = _search_result_uint(result,"Number of subbands");
            iresulttemp =(utemp>>2) & 0x1f;
            old_object = _replace_result_int(result, "Number of subbands",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>7) & 0xf;
            old_object = _replace_result_int(result, "WideBand CQI CW0",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>11) & 0xf;
            old_object = _replace_result_int(result, "WideBand CQI CW1",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>15) & 0xf;
            old_object = _replace_result_int(result, "SubBand Size (k)",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>19) & 0x7;
            old_object = _replace_result_int(result, "Size M",iresulttemp);
            Py_DECREF(old_object);
            //iresulttemp =(utemp>>22) & 0x3;
            //old_object = _replace_result_int(result, "Reserved",iresulttemp);
            //Py_DECREF(old_object);
            iresulttemp =(utemp>>24) & 0xf;
            old_object = _replace_result_int(result, "Single WB PMI",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>28) & 0xf;
            old_object = _replace_result_int(result, "Single MB PMI",iresulttemp);
            Py_DECREF(old_object);

            utemp = _search_result_uint(result,"CSF Tx Mode");
            iresulttemp =(utemp>>0) & 0xf;
            old_object = _replace_result_int(result, "CSF Tx Mode",iresulttemp);
            Py_DECREF(old_object);

            utemp = _search_result_uint(result,"Num Csirs Ports");
            iresulttemp =(utemp>>0) & 0xf;
            old_object = _replace_result_int(result, "Num Csirs Ports",iresulttemp);
            Py_DECREF(old_object);
            //iresulttemp =(utemp>>4) & 0xf;
            //old_object = _replace_result_int(result, "Reserved",iresulttemp);
            //Py_DECREF(old_object);

            utemp = _search_result_uint(result,"CRI");
            iresulttemp =(utemp>>0) & 0x7;
            old_object = _replace_result_int(result, "CRI",iresulttemp);
            Py_DECREF(old_object);
            //iresulttemp =(utemp>>3) & 0x1;
            //old_object = _replace_result_int(result, "Reserved",iresulttemp);
            //Py_DECREF(old_object);
            iresulttemp =(utemp>>4) & 0x3ff;
            old_object = _replace_result_int(result, "UL Frame Number",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>14) & 0xf;
            old_object = _replace_result_int(result, "UL Subframe Number",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>18) & 0x7f;
            old_object = _replace_result_int(result, "UL Payload Length",iresulttemp);
            Py_DECREF(old_object);
            //iresulttemp =(utemp>>25) & 0x7ff;
            //old_object = _replace_result_int(result_pdcch_item, "Reserved5",iresulttemp);
            //Py_DECREF(old_object);

            (void) _map_result_field_to_name(result, "Carrier Index",
                    ValueNameCarrierIndex,
                    ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                    "(MI)Unknown");

            (void) _map_result_field_to_name(result, "PUSCH Reporting Mode",
                    ValueNamePuschReportingMode,
                    ARRAY_SIZE(ValueNamePuschReportingMode, ValueName),
                    "(MI)Unknown");

            (void) _map_result_field_to_name(result, "Rank Index",
                    ValueNameRankIndex,
                    ARRAY_SIZE(ValueNameRankIndex, ValueName),
                    "(MI)Unknown");

            (void) _map_result_field_to_name(result, "CSF Tx Mode",
                    ValueNameCSFTxMode,
                    ARRAY_SIZE(ValueNameCSFTxMode, ValueName),
                    "(MI)Unknown");

            return offset - start;
        }

    case 102:
        {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v102,
                    ARRAY_SIZE(LtePhyPuschCsf_Payload_v102, Fmt),
                    b, offset, length, result);
            temp = _search_result_int(result, "Start System Sub-frame Number");
            int iSubFN = temp & 15;
            int iSysFN = (temp >> 4) & 1023;

            int before_temp=(temp >>14) & 3;

            temp = _search_result_int(result, "PUSCH Reporting Mode");

            //int iPuschReportingMode = (temp >> 1) & 7;
            int iPuschReportingMode = ((temp & 1) << 2) + before_temp;

            //int iRankIndex = (temp >> 4) & 1;
            int iRankIndex = (temp >> 2) & 1;

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

    case 103:
        {
            offset += _decode_by_fmt(LtePhyPuschCsf_Payload_v103,
                    ARRAY_SIZE(LtePhyPuschCsf_Payload_v103, Fmt),
                    b, offset, length, result);

            unsigned int utemp = _search_result_uint(result,"Start System Sub-frame Number");
            int iresulttemp =(utemp>>0) & 0xf;
            old_object = _replace_result_int(result, "Start System Sub-frame Number",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>4) & 0x3ff;
            old_object = _replace_result_int(result, "Start System Frame Number",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>14) & 0x7;
            old_object = _replace_result_int(result, "PUSCH Reporting Mode",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>17) & 0x1;
            old_object = _replace_result_int(result, "Csi Meas Set Index",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>18) & 0x3;
            old_object = _replace_result_int(result, "Rank Index",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>20) & 0xf;
            old_object = _replace_result_int(result, "Wideband PMI1",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>24) & 0x1f;
            old_object = _replace_result_int(result, "Number of subbands",iresulttemp);
            Py_DECREF(old_object);

            iresulttemp =(utemp>>29) & 0x7;
            int iWideBandCQICW0=iresulttemp;

            utemp = _search_result_uint(result,"WideBand CQI CW1");
            iWideBandCQICW0=iWideBandCQICW0+((utemp & 1)<<3);

            old_object = _replace_result_int(result, "WideBand CQI CW0",iWideBandCQICW0);
            Py_DECREF(old_object);

            iresulttemp =(utemp>>1) & 0xf;
            old_object = _replace_result_int(result, "WideBand CQI CW1",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>5) & 0xf;
            old_object = _replace_result_int(result, "SubBand Size (k)",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>9) & 0x7;
            old_object = _replace_result_int(result, "Size M",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>12) & 0xf;
            old_object = _replace_result_int(result, "Single WB PMI",iresulttemp);
            Py_DECREF(old_object);

            utemp = _search_result_uint(result,"Single MB PMI");
            iresulttemp =(utemp>>0) & 0xf;
            old_object = _replace_result_int(result, "Single MB PMI",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>4) & 0xf;
            old_object = _replace_result_int(result, "CSF Tx Mode",iresulttemp);
            Py_DECREF(old_object);

            utemp = _search_result_uint(result,"Alt Cqi Table Data");
            iresulttemp =(utemp>>1) & 0x1;

            utemp = _search_result_uint(result,"Carrier Index");
            old_object = _replace_result_int(result, "Carrier Index",iresulttemp);
            Py_DECREF(old_object);
            iresulttemp =(utemp>>4) & 0xf;
            old_object = _replace_result_int(result, "Num Csirs Ports",iresulttemp);
            Py_DECREF(old_object);

            (void) _map_result_field_to_name(result, "PUSCH Reporting Mode",
                    ValueNamePuschReportingMode,
                    ARRAY_SIZE(ValueNamePuschReportingMode, ValueName),
                    "(MI)Unknown");

            (void) _map_result_field_to_name(result, "Rank Index",
                    ValueNameRankIndex,
                    ARRAY_SIZE(ValueNameRankIndex, ValueName),
                    "(MI)Unknown");

            (void) _map_result_field_to_name(result, "CSF Tx Mode",
                    ValueNameCSFTxMode,
                    ARRAY_SIZE(ValueNameCSFTxMode, ValueName),
                    "(MI)Unknown");

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
