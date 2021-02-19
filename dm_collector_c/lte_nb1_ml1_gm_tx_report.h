/*
 * LTE NB1 ML1 GM TX Report
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LteNb1Ml1GmTxReport[] = {
    {UINT, "Version", 1},
    {SKIP, NULL, 1}, //Reserved
};

const Fmt LteNb1Ml1GmTxReport_v2[] = {
	{UINT, "Subcarrier Space", 1},    //left 3 bits
    {UINT, "Num of Records", 1},
};

const Fmt LteNb1Ml1GmTxReport_Record_v2[] = {
    {UINT, "NPDCCH Timing HSFN", 4},         // 10 bits
	{PLACEHOLDER, "NPDCCH Timing SFN",0},    // 10 bits
    {PLACEHOLDER, "NPDCCH Timing Sub FN",0}, // 4 bits
    {PLACEHOLDER, "NPUSCH Format",0}, // 1 bit
    {PLACEHOLDER, "Is MSG3",0}, // 1 bit
    {PLACEHOLDER, "ITBS",0}, // 6 bits
    {UINT, "Repetition Number", 4},         // 8 bits
    {PLACEHOLDER, "Num RU",0}, // 4 bits
    {PLACEHOLDER, "RV Index",0}, // 1 bit
    {PLACEHOLDER, "Num Tone",0}, // 4 bits
    {PLACEHOLDER, "Start Tone",0}, // 7 bits
    {PLACEHOLDER, "TX Power",0}, // 8 bits
    {UINT, "NPUSCH format 1 TX Type", 4},         // 1 bit
    {PLACEHOLDER, "ACK NACK",0}, // 1 bit
    {PLACEHOLDER, "RFLM",0}, // 8 bits
    {PLACEHOLDER, "PRACH Collision Valid",0}, // 1 bits
    {PLACEHOLDER, "PRACH Collision 7680ms",0}, // 3 bits
    {PLACEHOLDER, "Reserved",0}, // 18 bits
    {UINT, "Scrambling Mask",4},

};

static int _decode_lte_nb1_ml1_gm_tx_report_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;

    switch (pkt_ver) {
        case 2:
        {
        	offset += _decode_by_fmt(LteNb1Ml1GmTxReport_v2,
                    ARRAY_SIZE(LteNb1Ml1GmTxReport_v2, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num of Records");

            unsigned int iNonDecodeSubcarrierSpace = _search_result_uint(result, "Subcarrier Space");
            int iSubcarrierSpace = (iNonDecodeSubcarrierSpace >> 5) & 7;
            old_object = _replace_result_int(result, "Subcarrier Space",
                        iSubcarrierSpace);
                Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Subcarrier Space",
                        ValueNameNB1_GM_TX_Report_Subcarrier_Space_Type,
                        ARRAY_SIZE(ValueNameNB1_GM_TX_Report_Subcarrier_Space_Type, ValueName),
                        "(MI)Unknown");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LteNb1Ml1GmTxReport_Record_v2,
                        ARRAY_SIZE(LteNb1Ml1GmTxReport_Record_v2, Fmt),
                        b, offset, length, result_record_item);

                unsigned int iNonDecodeHSFN = _search_result_uint(result_record_item, "NPDCCH Timing HSFN");
                int iHSFN = iNonDecodeHSFN & 1023;          // 10 bits
                int iSFN = (iNonDecodeHSFN >> 10) & 1023;   // 10 bits
                int iSubFN = (iNonDecodeHSFN >> 20) & 15;   // 4 bits
                int iNPUSCH_Fmt = (iNonDecodeHSFN >> 24) & 1;              // 1 bit
                int isMSG3 = (iNonDecodeHSFN >> 25) & 1;         // 1 bit
                int iITBS = (iNonDecodeHSFN >> 26) & 63;           // 6 bits

                old_object = _replace_result_int(result_record_item, "NPDCCH Timing HSFN",
                        iHSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "NPDCCH Timing SFN",
                        iSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "NPDCCH Timing Sub FN",
                        iSubFN);
                Py_DECREF(old_object);

                old_object = _replace_result_int(result_record_item, "NPUSCH Format",
                        iNPUSCH_Fmt);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item, "NPUSCH Format",
                        ValueNameNPUSCHFormat,
                        ARRAY_SIZE(ValueNameNPUSCHFormat, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "Is MSG3",
                        isMSG3);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item, "Is MSG3",
                        ValueNameTrueOrFalse,
                        ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "ITBS",
                        iITBS);
                Py_DECREF(old_object);

                int iNonDecodeRepNum = _search_result_int(result_record_item, "Repetition Number");
                int iRepNum = iNonDecodeRepNum & 255;          // 8 bits
                int iRUNum = (iNonDecodeRepNum >> 8) & 15;   // 4 bits
                int iRVInd = (iNonDecodeRepNum >> 12) & 1;   // 1 bits
                int iNumTone = (iNonDecodeRepNum >> 13) & 15;              // 4 bit
                int iStartTone = (iNonDecodeRepNum >> 17) & 127;         // 7 bit
                int iTxPower = iNonDecodeRepNum >> 24;           // 8 bits, but it is signed 8 bits

                old_object = _replace_result_int(result_record_item, "Repetition Number",
                        iRepNum);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Num RU",
                        iRUNum);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "PRACH Collision Valid",
                        iRVInd);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Num Tone",
                        iNumTone);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Start Tone",
                        iStartTone);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "TX Power",
                        iTxPower);
                Py_DECREF(old_object);

                unsigned int iNonDecodeNPUSCHFmtTX = _search_result_uint(result_record_item, "NPUSCH format 1 TX Type");
                int iNPUSCH_Fmt_TX = iNonDecodeNPUSCHFmtTX & 1;          // 1 bits
                int iACK = (iNonDecodeNPUSCHFmtTX >> 1) & 1;   // 1 bits
                int iRFLM = (iNonDecodeNPUSCHFmtTX >> 2) & 255;   // 8 bits
                int iPRACHColVal = (iNonDecodeNPUSCHFmtTX >> 10) & 1;              // 1 bit
                int iPRACHCol7680 = (iNonDecodeNPUSCHFmtTX >> 11) & 7;         // 3 bit
                int iReserved = (iNonDecodeNPUSCHFmtTX >> 14) & 0x3ffff;           // 18 bits

                old_object = _replace_result_int(result_record_item, "NPUSCH format 1 TX Type",
                        iNPUSCH_Fmt_TX);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item, "NPUSCH format 1 TX Type",
                        ValueNameNPUSCHFormat_1_TX_Type,
                        ARRAY_SIZE(ValueNameNPUSCHFormat_1_TX_Type, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "ACK NACK",
                        iACK);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item, "ACK NACK",
                        ValueNameAckOrNack,
                        ARRAY_SIZE(ValueNameAckOrNack, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "RFLM",
                        iRFLM);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "PRACH Collision Valid",
                        iPRACHColVal);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "PRACH Collision 7680ms",
                        iPRACHCol7680);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Reserved",
                        iReserved);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);

            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
        default:
            printf("(MI)Unknown LTE NB1 ML1 GM TX Report version: 0x%x\n", pkt_ver);
            return 0;
    }
}