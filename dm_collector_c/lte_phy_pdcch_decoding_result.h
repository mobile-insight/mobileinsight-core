/*
 * LTE PHY PDCCH Decoding Result
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePhyPdcchDecodingResult_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePhyPdcchDecodingResult_Payload_v21 [] = {
    {UINT, "Subframe Number", 2},   // 4 bits
    {PLACEHOLDER, "System Frame Number", 0},    // 10 bits
    {SKIP, NULL, 3},
    {UINT, "Demback Mode Select", 1},   // 4 bits
    {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
    {UINT, "Number of Hypothesis", 1},
};

const Fmt LtePhyPdcchDecodingResult_Payload_v42 [] = {
    {UINT, "Subframe Number", 4},    // 4 bits
    {PLACEHOLDER, "System Frame Number", 0},    // 10 bits
    {PLACEHOLDER, "Band Width (MHz)", 0}, // 3 bits, (x-1)*5
    {PLACEHOLDER, "CIF Configured", 0}, // 1 bit
    {PLACEHOLDER, "Two bits CSI Configured", 0},    // 1 bit
    {PLACEHOLDER, "Aperiodic SRS Configured", 0},   // 1 bits
    {PLACEHOLDER, "Frame Structure", 0},    // 2 bits
    {PLACEHOLDER, "Num eNB Antennas", 0},   // 2 bits
    {PLACEHOLDER, "DL CP", 0},  // 2 bits
    {PLACEHOLDER, "SSC", 0},    // 2 bits
    {PLACEHOLDER, "CA FDD TDD", 0}, // 2 bits
    {SKIP, NULL, 1},
    {UINT, "Demback Mode Select", 1},   // 4 bits
    {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
    {UINT, "Number of Hypothesis", 1},
};

const Fmt LtePhyPdcchDecodingResult_Payload_v43 [] = {
    {UINT, "Num Records", 4},   // 4 bits
    {PLACEHOLDER, "Subframe Number", 0},    // 4 bits
    {PLACEHOLDER, "System Frame Number", 0},    // 10 bits
    {PLACEHOLDER, "Band Width (MHz)", 0}, // 4 bits, (x-1)*5
    {PLACEHOLDER, "Frame Structure", 0},    // 2 bits
    {PLACEHOLDER, "Num eNB Antennas", 0},   // 3 bits
    {SKIP, NULL, 2},
    {UINT, "Demback Mode Select", 1},   // 4 bits
    {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
};

const Fmt LtePhyPdcchDecodingResult_Payload_v101 [] = {
    {UINT, "Subframe Number", 4},    // 4 bits
    {PLACEHOLDER, "System Frame Number", 0},    // 10 bits
    {PLACEHOLDER, "Band Width (MHz)", 0}, // 3 bits, (x-1)*5
    {PLACEHOLDER, "CIF Configured", 0}, // 1 bit
    {PLACEHOLDER, "Two bits CSI Configured", 0},    // 1 bit
    {PLACEHOLDER, "Aperiodic SRS Configured", 0},   // 1 bits
    {PLACEHOLDER, "Frame Structure", 0},    // 2 bits
    {PLACEHOLDER, "Num eNB Antennas", 0},   // 2 bits
    {PLACEHOLDER, "DL CP", 0},  // 2 bits
    {PLACEHOLDER, "SSC", 0},    // 2 bits
    {PLACEHOLDER, "CA FDD TDD", 0}, // 2 bits
    {SKIP, NULL, 1},
    {UINT, "Demback Mode Select", 1},   // 4 bits
    {PLACEHOLDER, "Carrier Index", 0},  // 4 bits
    {UINT, "Number of Hypothesis", 1},
};

const Fmt LtePhyPdcchDecodingResult_Record_v43 [] = {
    {UINT, "Subframe Offset", 2},
    {UINT, "CIF Configured", 2},    // 1 bit
    {PLACEHOLDER, "Two bits CSI Configured", 0},    // 1 bit
    {PLACEHOLDER, "Aperiodic SRS Configured", 0},   // 1 bit
    {PLACEHOLDER, "Number of Hypothesis", 0},   // 8 bits
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v21 [] = {
    {BYTE_STREAM, "Payload", 8},
    {UINT, "Aggregation Level", 4}, // 2 bits
    {PLACEHOLDER, "Candidate", 0},  // 3 bits
    {PLACEHOLDER, "Search Space Type", 0},  // 1 bit
    {PLACEHOLDER, "DCI Format", 0}, // 4 bits
    {PLACEHOLDER, "Decode Status", 0},  // 4 bits
    {PLACEHOLDER, "Start CCE", 0},  // 7 bits
    {PLACEHOLDER, "Payload Size", 0},   // 8 bits
    {PLACEHOLDER, "Tail Match", 0}, // 1 bit
    {UINT, "Prune Status", 2},
    {SKIP, NULL, 2},
    {UINT, "Norm Energy Metric", 4},    // x/65535.0
    {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v42 [] = {
    // totally 28
    {BYTE_STREAM, "Payload", 8},
    {UINT, "Aggregation Level", 4}, // 2 bits
    {PLACEHOLDER, "Candidate", 0}, // 3 bits
    {PLACEHOLDER, "Search Space Type", 0}, // 1 bit
    {PLACEHOLDER, "DCI Format", 0},    // 4 bits
    {PLACEHOLDER, "Decode States", 0}, // 4 bits
    {PLACEHOLDER, "Payload Size", 0},  // 8 bits
    {PLACEHOLDER, "Tail Match", 0},    // 1 bit
    {PLACEHOLDER, "Non Zero Symbol Mismatch Count", 0},    // 9 bits
    {UINT, "Start CCE", 4}, // right shift 1 bit, 7 bits
    {PLACEHOLDER, "Non Zero Llr Count", 0}, // 9 bits
    {PLACEHOLDER, "Normal", 0},     // 15 bits
    {UINT, "Prune Status", 4},  //  11 bits
    {PLACEHOLDER, "Energy Metric", 0},  // 21 bits
    {UINT, "Norm Energy Metric", 4},    // x/65535.0
    {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v43 [] = {
    // totally 28
    {BYTE_STREAM, "Payload", 8},
    {UINT, "Aggregation Level", 4}, // 2 bits
    {PLACEHOLDER, "Candidate", 0}, // 3 bits
    {PLACEHOLDER, "Search Space Type", 0}, // 1 bit
    {PLACEHOLDER, "DCI Format", 0},    // 4 bits
    {PLACEHOLDER, "Decode States", 0}, // 4 bits
    {PLACEHOLDER, "Payload Size", 0},  // 8 bits
    {PLACEHOLDER, "Tail Match", 0},    // 1 bit
    {PLACEHOLDER, "Non Zero Symbol Mismatch Count", 0},    // 9 bits
    {UINT, "Start CCE", 4}, // right shift 1 bit, 7 bits
    {PLACEHOLDER, "Non Zero Llr Count", 0}, // 9 bits
    {PLACEHOLDER, "Normal", 0},     // 15 bits
    {UINT, "Prune Status", 4},  //  11 bits
    {PLACEHOLDER, "Energy Metric", 0},  // 21 bits
    {UINT, "Norm Energy Metric", 4},    // x/65535.0
    {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};

const Fmt LtePhyPdcchDecodingResult_Hypothesis_v101 [] = {
    // totally 28
    {BYTE_STREAM, "Payload", 8},
    {UINT, "Aggregation Level", 4}, // 2 bits
    {PLACEHOLDER, "Candidate", 0}, // 3 bits
    {PLACEHOLDER, "Search Space Type", 0}, // 1 bit
    {PLACEHOLDER, "DCI Format", 0},    // 4 bits
    {PLACEHOLDER, "Decode States", 0}, // 4 bits
    {PLACEHOLDER, "Payload Size", 0},  // 8 bits
    {PLACEHOLDER, "Tail Match", 0},    // 1 bit
    {PLACEHOLDER, "Non Zero Symbol Mismatch Count", 0},    // 9 bits
    {UINT, "Start CCE", 4}, // right shift 1 bit, 7 bits
    {PLACEHOLDER, "Non Zero Llr Count", 0}, // 9 bits
    {PLACEHOLDER, "Normal", 0},     // 15 bits
    {UINT, "Prune Status", 4},  //  11 bits
    {PLACEHOLDER, "Energy Metric", 0},  // 21 bits
    {UINT, "Norm Energy Metric", 4},    // x/65535.0
    {UINT, "Symbol Error Rate", 4}, // x/2147483648.0
};

static int _decode_lte_phy_pdcch_decoding_result_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;
    PyObject *pyfloat;

    switch (pkt_ver) {
    case 21:
        {
            offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Payload_v21,
                    ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v21, Fmt),
                    b, offset, length, result);
            int iNonDecodeP1 = _search_result_int(result, "Subframe Number");
            int iSubFN = iNonDecodeP1 & 15; // 4 bits
            int iSysFN = (iNonDecodeP1 >> 4) & 1023;
            old_object = _replace_result_int(result, "Subframe Number", iSubFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "System Frame Number",
                    iSysFN);
            Py_DECREF(old_object);
            int iNonDecodeP2 = _search_result_int(result, "Demback Mode Select");
            int iDMS = iNonDecodeP2 & 15; // 4 bits
            int iCarrierIndex = (iNonDecodeP2 >> 4) & 15;
            old_object = _replace_result_int(result, "Demback Mode Select", iDMS);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Carrier Index", iCarrierIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Carrier Index",
                    ValueNameCarrierIndex,
                    ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                    "(MI)Unknown");
            int num_record = _search_result_int(result, "Number of Hypothesis");
            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Hypothesis_v21,
                        ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v21, Fmt),
                        b, offset, length, result_record_item);
                unsigned int iNonDecodeP3 = _search_result_uint(result_record_item,
                        "Aggregation Level");
                int iAggLv = iNonDecodeP3 & 3;  // 2 bits
                int iCandidate = (iNonDecodeP3 >> 2) & 7;   // 3 bits
                int iSearchSpaceType = (iNonDecodeP3 >> 5) & 1; // 1 bit
                int iDCIFormat = (iNonDecodeP3 >> 6) & 15; // 4 bits
                int iDecodeStatus = (iNonDecodeP3 >> 10) & 15; // 4 bits
                int iStartCCE = (iNonDecodeP3 >> 14) & 127;    // 7 bits
                int iPayloadSize = (iNonDecodeP3 >> 21) & 255; // 8 bits
                int iTailMatch = (iNonDecodeP3 >> 29) & 1; // 1 bit

                old_object = _replace_result_int(result_record_item,
                        "Aggregation Level", iAggLv);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Aggregation Level",
                        ValueNameAggregationLevel,
                        ARRAY_SIZE(ValueNameAggregationLevel, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Candidate", iCandidate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Search Space Type", iSearchSpaceType);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Search Space Type",
                        ValueNameSearchSpaceType,
                        ARRAY_SIZE(ValueNameSearchSpaceType, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "DCI Format", iDCIFormat);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "DCI Format",
                        ValueNameDCIFormat,
                        ARRAY_SIZE(ValueNameDCIFormat, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Decode Status", iDecodeStatus);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Decode Status",
                        ValueNameRNTIType,
                        ARRAY_SIZE(ValueNameRNTIType, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Start CCE", iStartCCE);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Payload Size", iPayloadSize);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Tail Match", iTailMatch);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Tail Match",
                        ValueNameMatchOrNot,
                        ARRAY_SIZE(ValueNameMatchOrNot, ValueName),
                        "(MI)Unknown");
                (void) _map_result_field_to_name(result_record_item,
                        "Prune Status",
                        ValueNamePruneStatus,
                        ARRAY_SIZE(ValueNamePruneStatus, ValueName),
                        "(MI)Unknown");
                unsigned int iNonDecode = _search_result_uint(result_record_item,
                        "Norm Energy Metric");
                float fNEM = iNonDecode / 65535.0;
                pyfloat = Py_BuildValue("f", fNEM);
                old_object = _replace_result(result_record_item,
                        "Norm Energy Metric", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);
                iNonDecode = _search_result_uint(result_record_item,
                        "Symbol Error Rate");
                float fSER = iNonDecode / 2147483648.0;
                pyfloat = Py_BuildValue("f", fSER);
                old_object = _replace_result(result_record_item,
                        "Symbol Error Rate", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Hypothesis",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    case 42:
        {
            offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Payload_v42,
                    ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v42, Fmt),
                    b, offset, length, result);
            unsigned int temp = _search_result_uint(result, "Subframe Number");
            int iSubFN = temp & 15; // 4 bits
            int iSysFN = (temp >> 4) & 1023;    // 10 bits
            int iBandWidth = (temp >> 14) & 7; // 3 bits
            iBandWidth = (iBandWidth - 1) * 5;
            int iCC = (temp >> 17) & 1;  // 1 bit
            int iTBCC = (temp >> 18) & 1;   // 1 bit
            int iASC = (temp >> 19) & 1; // 1 bit
            int iFrameStructure = (temp >> 20) & 3; // 2 bits
            int iNumAntennas = (temp >> 22) & 3;    // 2 bits
            int iDLCP = (temp >> 24) & 3;   // 2 bits
            int iSSC = (temp >> 26) & 3;    // 2 bits
            int iCAFDDTDD = (temp >> 28) & 3;   // 2 bits
            old_object = _replace_result_int(result, "Subframe Number", iSubFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "System Frame Number",
                    iSysFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Band Width (MHz)", iBandWidth);
            Py_DECREF(old_object);

            old_object = _replace_result_int(result, "CIF Configured", iCC);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result,
                    "CIF Configured",
                    ValueNameTrueOrFalse,
                    ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result,
                    "Two bits CSI Configured", iTBCC);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result,
                    "Two bits CSI Configured",
                    ValueNameTrueOrFalse,
                    ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result,
                    "Aperiodic SRS Configured", iASC);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result,
                    "Aperiodic SRS Configured",
                    ValueNameTrueOrFalse,
                    ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Frame Structure", iFrameStructure);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Frame Structure",
                    ValueNameFrameStructure,
                    ARRAY_SIZE(ValueNameFrameStructure, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Num eNB Antennas", iNumAntennas);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Num eNB Antennas",
                    ValueNameNumNBAntennas,
                    ARRAY_SIZE(ValueNameNumNBAntennas, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "DL CP", iDLCP);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result,
                    "DL CP",
                    ValueNameNormalOrNot,
                    ARRAY_SIZE(ValueNameNormalOrNot, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "SSC", iSSC);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "CA FDD TDD", iCAFDDTDD);
            Py_DECREF(old_object);

            temp = _search_result_int(result, "Demback Mode Select");
            int iDMS = temp & 15; // 4 bits
            int iCarrierIndex = (temp >> 4) & 15;   // 4 bits
            old_object = _replace_result_int(result, "Demback Mode Select", iDMS);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Carrier Index", iCarrierIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Carrier Index",
                    ValueNameCarrierIndex,
                    ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                    "(MI)Unknown");

            int num_record = _search_result_int(result, "Number of Hypothesis");
            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                // parsing each hypothesis
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Hypothesis_v42,
                        ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v42, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_uint(result_record_item,
                        "Aggregation Level");

                int iAggLv = temp & 3;  // 2 bits
                int iCandidate = (temp >> 2) & 7;   // 3 bits
                int iSearchSpaceType = (temp >> 5) & 1; // 1 bit
                int iDCIFormat = (temp >> 6) & 15; // 4 bits
                int iDecodeStatus = (temp >> 10) & 15; // 4 bits
                int iPayloadSize = (temp >> 14) & 255; // 8 bits
                int iTailMatch = (temp >> 22) & 1; // 1 bit
                int iNZSMC = (temp >> 23) & 511;   // 9 bits
                temp = _search_result_uint(result_record_item,
                        "Start CCE");
                int iStartCCE = (temp >> 1) & 127; // 1 + 7 bits
                int iNZLC = (temp >> 8) & 511;  // 9 bits
                int iNormal = (temp >> 17) & 32767; // 15 bits
                temp = _search_result_uint(result_record_item,
                        "Prune Status");
                int iPruneStatus = temp & 2047; // 11 bits
                int iEnergyMetric = temp >> 11; // the rest 21 bits

                old_object = _replace_result_int(result_record_item,
                        "Aggregation Level", iAggLv);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Aggregation Level",
                        ValueNameAggregationLevel,
                        ARRAY_SIZE(ValueNameAggregationLevel, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Candidate", iCandidate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Search Space Type", iSearchSpaceType);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Search Space Type",
                        ValueNameSearchSpaceType,
                        ARRAY_SIZE(ValueNameSearchSpaceType, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "DCI Format", iDCIFormat);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "DCI Format",
                        ValueNameDCIFormat,
                        ARRAY_SIZE(ValueNameDCIFormat, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "DCI Format", iDecodeStatus);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "DCI Format",
                        ValueNameRNTIType,
                        ARRAY_SIZE(ValueNameRNTIType, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Payload Size", iPayloadSize);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Tail Match", iTailMatch);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Tail Match",
                        ValueNameMatchOrNot,
                        ARRAY_SIZE(ValueNameMatchOrNot, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Non Zero Symbol Mismatch Count", iNZSMC);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start CCE", iStartCCE);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Non Zero Llr Count", iNZLC);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Normal", iNormal);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Prune Status", iPruneStatus);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Prune Status",
                        ValueNamePruneStatus,
                        ARRAY_SIZE(ValueNamePruneStatus, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Energy Metric", iEnergyMetric);
                Py_DECREF(old_object);
                unsigned int iNonDecode = _search_result_uint(result_record_item,
                        "Norm Energy Metric");
                float fNEM = iNonDecode / 65535.0;
                pyfloat = Py_BuildValue("f", fNEM);
                old_object = _replace_result(result_record_item,
                        "Norm Energy Metric", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);
                iNonDecode = _search_result_uint(result_record_item,
                        "Symbol Error Rate");
                float fSER = iNonDecode / 2147483648.0;
                pyfloat = Py_BuildValue("f", fSER);
                old_object = _replace_result(result_record_item,
                        "Symbol Error Rate", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Hypothesis",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    case 43:
        {
            offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Payload_v43,
                    ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v43, Fmt),
                    b, offset, length, result);
            unsigned int temp = _search_result_uint(result, "Num Records");
            int num_record = temp & 15; // 4 bits
            int iSubFN = (temp >> 4) & 15; // 4 bits
            int iSysFN = (temp >> 8) & 1023; // 10 bits
            int iBandWidth = (temp >> 18) & 15; // 4 bits
            iBandWidth = (iBandWidth - 1) * 5;
            int iFrameStructure = (temp >> 22) & 3; // 2 bits
            int iNumAntennas = (temp >> 24) & 7;    // 3 bits
            temp = _search_result_int(result, "Demback Mode Select");
            int iDMS = temp & 15; // 4 bits
            int iCarrierIndex = (temp >> 4) & 15;   // 4 bits

            old_object = _replace_result_int(result, "Num Records", num_record);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Subframe Number", iSubFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "System Frame Number", iSysFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Band Width (MHz)", iBandWidth);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Frame Structure", iFrameStructure);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Frame Structure",
                    ValueNameFrameStructure,
                    ARRAY_SIZE(ValueNameFrameStructure, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Num eNB Antennas", iNumAntennas);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Num eNB Antennas",
                    ValueNameNumNBAntennas,
                    ARRAY_SIZE(ValueNameNumNBAntennas, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Demback Mode Select", iDMS);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Carrier Index", iCarrierIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Carrier Index",
                    ValueNameCarrierIndex,
                    ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                    "(MI)Unknown");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Record_v43,
                        ARRAY_SIZE(LtePhyPdcchDecodingResult_Record_v43, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "CIF Configured");
                int iCC = temp & 1; // 1 bit
                int iTBCC = (temp >> 1) & 1;    // 1 bit
                int iASC = (temp >> 2) & 1; // 1 bit
                int num_hypothesis = (temp >> 3) & 255; // 8 bits
                old_object = _replace_result_int(result_record_item,
                        "CIF Configured", iCC);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "CIF Configured",
                        ValueNameTrueOrFalse,
                        ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Two bits CSI Configured", iTBCC);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Two bits CSI Configured",
                        ValueNameTrueOrFalse,
                        ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Aperiodic SRS Configured", iASC);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Aperiodic SRS Configured",
                        ValueNameTrueOrFalse,
                        ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Number of Hypothesis", num_hypothesis);
                Py_DECREF(old_object);

                PyObject *result_record_hypothesis = PyList_New(0);
                for (int j = 0; j < num_hypothesis; j++) {
                    PyObject *result_record_hypothesis_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Hypothesis_v43,
                            ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v43, Fmt),
                            b, offset, length, result_record_hypothesis_item);

                    temp = _search_result_uint(result_record_hypothesis_item,
                            "Aggregation Level");
                    int iAggLv = temp & 3;  // 2 bits
                    int iCandidate = (temp >> 2) & 7;   // 3 bits
                    int iSearchSpaceType = (temp >> 5) & 1; // 1 bit
                    int iDCIFormat = (temp >> 6) & 15; // 4 bits
                    int iDecodeStatus = (temp >> 10) & 15; // 4 bits
                    int iPayloadSize = (temp >> 14) & 255; // 8 bits
                    int iTailMatch = (temp >> 22) & 1; // 1 bit
                    int iNZSMC = (temp >> 23) & 511;   // 9 bits
                    temp = _search_result_uint(result_record_hypothesis_item,
                            "Start CCE");
                    int iStartCCE = (temp >> 1) & 127; // 1 + 7 bits
                    int iNZLC = (temp >> 8) & 511;  // 9 bits
                    int iNormal = (temp >> 17) & 32767; // 15 bits
                    temp = _search_result_uint(result_record_hypothesis_item,
                            "Prune Status");
                    int iPruneStatus = temp & 2047; // 11 bits
                    int iEnergyMetric = temp >> 11; // the rest 21 bits

                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "Aggregation Level", iAggLv);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_hypothesis_item,
                            "Aggregation Level",
                            ValueNameAggregationLevel,
                            ARRAY_SIZE(ValueNameAggregationLevel, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "Candidate", iCandidate);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "Search Space Type", iSearchSpaceType);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_hypothesis_item,
                            "Search Space Type",
                            ValueNameSearchSpaceType,
                            ARRAY_SIZE(ValueNameSearchSpaceType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "DCI Format", iDCIFormat);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_hypothesis_item,
                            "DCI Format",
                            ValueNameDCIFormat,
                            ARRAY_SIZE(ValueNameDCIFormat, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "DCI Format", iDecodeStatus);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_hypothesis_item,
                            "DCI Format",
                            ValueNameRNTIType,
                            ARRAY_SIZE(ValueNameRNTIType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "Payload Size", iPayloadSize);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "Tail Match", iTailMatch);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_hypothesis_item,
                            "Tail Match",
                            ValueNameMatchOrNot,
                            ARRAY_SIZE(ValueNameMatchOrNot, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "Non Zero Symbol Mismatch Count", iNZSMC);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "Start CCE", iStartCCE);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "Non Zero Llr Count", iNZLC);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "Normal", iNormal);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "Prune Status", iPruneStatus);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_hypothesis_item,
                            "Prune Status",
                            ValueNamePruneStatus,
                            ARRAY_SIZE(ValueNamePruneStatus, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_hypothesis_item,
                            "Energy Metric", iEnergyMetric);
                    Py_DECREF(old_object);
                    unsigned int iNonDecode = _search_result_uint(result_record_hypothesis_item,
                            "Norm Energy Metric");
                    float fNEM = iNonDecode / 65535.0;
                    pyfloat = Py_BuildValue("f", fNEM);
                    old_object = _replace_result(result_record_hypothesis_item,
                            "Norm Energy Metric", pyfloat);
                    Py_DECREF(old_object);
                    Py_DECREF(pyfloat);
                    iNonDecode = _search_result_uint(result_record_hypothesis_item,
                            "Symbol Error Rate");
                    float fSER = iNonDecode / 2147483648.0;
                    pyfloat = Py_BuildValue("f", fSER);
                    old_object = _replace_result(result_record_hypothesis_item,
                            "Symbol Error Rate", pyfloat);
                    Py_DECREF(old_object);
                    Py_DECREF(pyfloat);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_hypothesis_item, "dict");
                    PyList_Append(result_record_hypothesis, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_hypothesis_item);
                }

                offset += (11 - num_hypothesis) * 28;
                PyObject *t2 = Py_BuildValue("(sOs)", "Hypothesis",
                        result_record_hypothesis, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_hypothesis);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "SF",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    case 101:
        {
            offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Payload_v101,
                    ARRAY_SIZE(LtePhyPdcchDecodingResult_Payload_v101, Fmt),
                    b, offset, length, result);
            unsigned int temp = _search_result_uint(result, "Subframe Number");
            int iSubFN = temp & 15; // 4 bits
            int iSysFN = (temp >> 4) & 1023;    // 10 bits
            int iBandWidth = (temp >> 14) & 7; // 3 bits
            iBandWidth = (iBandWidth - 1) * 5;
            int iCC = (temp >> 17) & 1;  // 1 bit
            int iTBCC = (temp >> 18) & 1;   // 1 bit
            int iASC = (temp >> 19) & 1; // 1 bit
            int iFrameStructure = (temp >> 20) & 3; // 2 bits
            int iNumAntennas = (temp >> 22) & 3;    // 2 bits
            int iDLCP = (temp >> 24) & 3;   // 2 bits
            int iSSC = (temp >> 26) & 3;    // 2 bits
            int iCAFDDTDD = (temp >> 28) & 3;   // 2 bits
            old_object = _replace_result_int(result, "Subframe Number", iSubFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "System Frame Number",
                    iSysFN);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Band Width (MHz)", iBandWidth);
            Py_DECREF(old_object);

            old_object = _replace_result_int(result, "CIF Configured", iCC);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result,
                    "CIF Configured",
                    ValueNameTrueOrFalse,
                    ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result,
                    "Two bits CSI Configured", iTBCC);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result,
                    "Two bits CSI Configured",
                    ValueNameTrueOrFalse,
                    ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result,
                    "Aperiodic SRS Configured", iASC);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result,
                    "Aperiodic SRS Configured",
                    ValueNameTrueOrFalse,
                    ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Frame Structure", iFrameStructure);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Frame Structure",
                    ValueNameFrameStructure,
                    ARRAY_SIZE(ValueNameFrameStructure, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Num eNB Antennas", iNumAntennas);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Num eNB Antennas",
                    ValueNameNumNBAntennas,
                    ARRAY_SIZE(ValueNameNumNBAntennas, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "DL CP", iDLCP);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result,
                    "DL CP",
                    ValueNameNormalOrNot,
                    ARRAY_SIZE(ValueNameNormalOrNot, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "SSC", iSSC);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "CA FDD TDD", iCAFDDTDD);
            Py_DECREF(old_object);

            temp = _search_result_int(result, "Demback Mode Select");
            int iDMS = temp & 15; // 4 bits
            int iCarrierIndex = (temp >> 4) & 15;   // 4 bits
            old_object = _replace_result_int(result, "Demback Mode Select", iDMS);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Carrier Index", iCarrierIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Carrier Index",
                    ValueNameCarrierIndex,
                    ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                    "(MI)Unknown");

            int num_record = _search_result_int(result, "Number of Hypothesis");
            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                // parsing each hypothesis
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Hypothesis_v101,
                        ARRAY_SIZE(LtePhyPdcchDecodingResult_Hypothesis_v101, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_uint(result_record_item,
                        "Aggregation Level");

                int iAggLv = temp & 3;  // 2 bits
                int iCandidate = (temp >> 2) & 7;   // 3 bits
                int iSearchSpaceType = (temp >> 5) & 1; // 1 bit
                int iDCIFormat = (temp >> 6) & 15; // 4 bits
                int iDecodeStatus = (temp >> 10) & 15; // 4 bits
                int iPayloadSize = (temp >> 14) & 255; // 8 bits
                int iTailMatch = (temp >> 22) & 1; // 1 bit
                int iNZSMC = (temp >> 23) & 511;   // 9 bits
                temp = _search_result_uint(result_record_item,
                        "Start CCE");
                int iStartCCE = (temp >> 1) & 127; // 1 + 7 bits
                int iNZLC = (temp >> 8) & 511;  // 9 bits
                int iNormal = (temp >> 17) & 32767; // 15 bits
                temp = _search_result_uint(result_record_item,
                        "Prune Status");
                int iPruneStatus = temp & 2047; // 11 bits
                int iEnergyMetric = temp >> 11; // the rest 21 bits

                old_object = _replace_result_int(result_record_item,
                        "Aggregation Level", iAggLv);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Aggregation Level",
                        ValueNameAggregationLevel,
                        ARRAY_SIZE(ValueNameAggregationLevel, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Candidate", iCandidate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Search Space Type", iSearchSpaceType);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Search Space Type",
                        ValueNameSearchSpaceType,
                        ARRAY_SIZE(ValueNameSearchSpaceType, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "DCI Format", iDCIFormat);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "DCI Format",
                        ValueNameDCIFormat,
                        ARRAY_SIZE(ValueNameDCIFormat, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "DCI Format", iDecodeStatus);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "DCI Format",
                        ValueNameRNTIType,
                        ARRAY_SIZE(ValueNameRNTIType, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Payload Size", iPayloadSize);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Tail Match", iTailMatch);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Tail Match",
                        ValueNameMatchOrNot,
                        ARRAY_SIZE(ValueNameMatchOrNot, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Non Zero Symbol Mismatch Count", iNZSMC);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start CCE", iStartCCE);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Non Zero Llr Count", iNZLC);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Normal", iNormal);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Prune Status", iPruneStatus);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Prune Status",
                        ValueNamePruneStatus,
                        ARRAY_SIZE(ValueNamePruneStatus, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Energy Metric", iEnergyMetric);
                Py_DECREF(old_object);
                unsigned int iNonDecode = _search_result_uint(result_record_item,
                        "Norm Energy Metric");
                float fNEM = iNonDecode / 65535.0;
                pyfloat = Py_BuildValue("f", fNEM);
                old_object = _replace_result(result_record_item,
                        "Norm Energy Metric", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);
                iNonDecode = _search_result_uint(result_record_item,
                        "Symbol Error Rate");
                float fSER = iNonDecode / 2147483648.0;
                pyfloat = Py_BuildValue("f", fSER);
                old_object = _replace_result(result_record_item,
                        "Symbol Error Rate", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Hypothesis",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }


    default:
        printf("(MI)Unknown LTE PHY PDCCH Decoding Result version: 0x%x\n", pkt_ver);
        return 0;
    }
}
