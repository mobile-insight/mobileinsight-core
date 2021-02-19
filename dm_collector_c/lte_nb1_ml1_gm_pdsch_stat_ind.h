/*
 * LTE NB1 ML1 GM PDSCH STAT Ind
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LteNb1Ml1GmPdschStatIndFmt[] = {
    {UINT, "Version", 1},
    {SKIP, NULL, 2}, //Reserved
};

const Fmt LteNb1Ml1GmPdschStatIndFmt_v2[] = {
    {UINT, "Num of Records", 1},

};

const Fmt LteNb1Ml1GmPdschStatIndFmt_Record_v2[] = {
    {UINT, "SFN", 4},         //  10 bits
    {PLACEHOLDER, "Sub FN",0},    // 4 bits
    {PLACEHOLDER, "RNTI Type",0},    // 3 bits 
    {PLACEHOLDER, "CRC",0},    // 1 bit 
    {PLACEHOLDER, "TB Size",0},    // 14 bits
    {UINT, "Hyper SFN Data", 4},         //  10 bits
    {PLACEHOLDER, "MCS",0},    // 4 bits
    {PLACEHOLDER, "Isf",0},    // 3 bits
    {PLACEHOLDER, "Auto ACK",0},    // 1 bits
    {PLACEHOLDER, "NDI",0},    // 14 bits
};

static int _decode_lte_nb1_ml1_gm_pdsch_stat_ind_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;

    switch (pkt_ver) {
        case 2:
        {
        	offset += _decode_by_fmt(LteNb1Ml1GmPdschStatIndFmt_v2,
                    ARRAY_SIZE(LteNb1Ml1GmPdschStatIndFmt_v2, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num of Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LteNb1Ml1GmPdschStatIndFmt_Record_v2,
                        ARRAY_SIZE(LteNb1Ml1GmPdschStatIndFmt_Record_v2, Fmt),
                        b, offset, length, result_record_item);

                unsigned int iNonDecodeSFN = _search_result_uint(result_record_item, "SFN");
                int iSFN = iNonDecodeSFN & 1023;          // 10 bits
                int iSubFN = (iNonDecodeSFN >> 10) & 15;   // 4 bits
                int iRNTI_T = (iNonDecodeSFN >> 14) & 7;   // 3 bits
                int iCRC = (iNonDecodeSFN >> 17) & 1;   // 1 bits
                int iTB_S = (iNonDecodeSFN >> 18) & 0x3fff;   // 14 bits

                old_object = _replace_result_int(result_record_item, "SFN",
                        iSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Sub FN",
                        iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "RNTI Type",
                        iRNTI_T);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item, "RNTI Type",
                        ValueNameNB1_PDSCH_RNTIType,
                        ARRAY_SIZE(ValueNameNB1_PDSCH_RNTIType, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "CRC",
                        iCRC);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item, "CRC",
                        ValueNamePassOrFail,
                        ARRAY_SIZE(ValueNamePassOrFail, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "TB Size",
                        iTB_S);
                Py_DECREF(old_object);

                unsigned int iNonDecodeHyperSFN = _search_result_uint(result_record_item, "Hyper SFN Data");
                int iHyperSFN = iNonDecodeHyperSFN & 1023;          // 10 bits
                int iMCS = (iNonDecodeHyperSFN >> 10) & 15;   // 4 bits
                int iISF = (iNonDecodeHyperSFN >> 14) & 7;   // 3 bits
                int iAutoACK = (iNonDecodeHyperSFN >> 17) & 1;   // 1 bits
                int iNDI = (iNonDecodeHyperSFN >> 18) & 0x3fff;   // 14 bits

                old_object = _replace_result_int(result_record_item, "Hyper SFN Data",
                        iHyperSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "MCS",
                        iMCS);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Isf",
                        iISF);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Auto ACK",
                        iAutoACK);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item, "Auto ACK",
                        ValueNameTrueOrFalse,
                        ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "NDI",
                        iNDI);
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
            printf("(MI)Unknown LTE NB1 ML1 GM PDSCH Stat Ind version: 0x%x\n", pkt_ver);
            return 0;
    }
}