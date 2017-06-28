/*
 * LTE PHY PDSCH Decoding Result
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePhyPdschDecodingResult_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePhyPdschDecodingResult_Payload_v24 [] = {
    {UINT, "Serving Cell ID", 4},   // 9 btis
    {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
    {PLACEHOLDER, "Starting System Frame Number", 0},   // 10 bits
    {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
    {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
    {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
    {SKIP, NULL, 1},
    {UINT, "Carrier Index", 1}, // 3 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
};

const Fmt LtePhyPdschDecodingResult_Payload_v44 [] = {
    {UINT, "Serving Cell ID", 4},   // 9 btis
    {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
    {PLACEHOLDER, "Starting System Frame Number", 0},   // 10 bits
    {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
    {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
    {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
    {SKIP, NULL, 4},
    {UINT, "Carrier Index", 2}, //  right shift 7 bits, 4 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
};

const Fmt LtePhyPdschDecodingResult_Record_v24 [] = {
    {UINT, "Subframe Offset", 2},
    {UINT, "PDSCH Channel ID", 2},
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
    {UINT, "System Information Msg Number", 2}, // 4 bits
    {PLACEHOLDER, "System Information Mask", 0},    // 12 bits
    {UINT, "HARQ Log Status", 1},   // right shift 3 bit, 2 bits
    {PLACEHOLDER, "Codeword Swap", 0},  // 1 bit
    {PLACEHOLDER, "Number of Streams", 0},  // 2 bits
};

const Fmt LtePhyPdschDecodingResult_Record_v44 [] = {
    {UINT, "Subframe Offset", 2},
    {UINT, "PDSCH Channel ID", 2},
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
    {UINT, "System Information Msg Number", 2}, // 4 bits
    {PLACEHOLDER, "System Information Mask", 0},    // 12 bits
    {UINT, "HARQ Log Status", 1},   // right shift 3 bit, 2 bits
    {PLACEHOLDER, "Codeword Swap", 0},  // 1 bit
    {PLACEHOLDER, "Number of Streams", 0},  // 2 bits
    {BYTE_STREAM, "Demap Sic Status", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPdschDecodingResult_Stream_v24 [] = {
    {UINT, "Transport Block CRC", 4},   // 1 bit
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
    {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
    {PLACEHOLDER, "Max TDEC Iter", 0},  // 4 bits
    {PLACEHOLDER, "Retransmission Number", 0},  // 3 bits
    {PLACEHOLDER, "RVID", 0},   // 2 bits
    {PLACEHOLDER, "Companding Stats", 0},   // 2 bits
    {PLACEHOLDER, "HARQ Combining", 0}, // 1 bit
    {PLACEHOLDER, "Decob TB CRC", 0},   // 1 bit
    {UINT, "Num RE", 4},    // right shift 10 bits, 16 bits
    {PLACEHOLDER, "Codeword Index", 0}, // right shift 27 bits, 4 bits
};

const Fmt LtePhyPdschDecodingResult_Stream_v44 [] = {
    {UINT, "Transport Block CRC", 4},   // 1 bit
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
    {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
    {PLACEHOLDER, "Max TDEC Iter", 0},  // 4 bits
    {PLACEHOLDER, "Retransmission Number", 0},  // 3 bits
    {PLACEHOLDER, "RVID", 0},   // 2 bits
    {PLACEHOLDER, "Companding Stats", 0},   // 2 bits
    {PLACEHOLDER, "HARQ Combining", 0}, // 1 bit
    {PLACEHOLDER, "Decob TB CRC", 0},   // 1 bit
    {UINT, "Num RE", 4},    // right shift 10 bits, 16 bits
    {PLACEHOLDER, "Codeword Index", 0}, // right shift 27 bits, 4 bits
    {UINT, "LLR Scale", 1}, // 4 bits
    {SKIP, NULL, 3},
};

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v24 [] = {
    // totally 13
    {UINT, "Energy Metric", 4}, // 21 bits
    {PLACEHOLDER, "Iteration Number", 0},   // 4 bits
    {PLACEHOLDER, "Code Block CRC Pass", 0},    // 1 bit
    {PLACEHOLDER, "Early Termination", 0},  // 1 bit
    {PLACEHOLDER, "HARQ Combine Enable", 0},    // 1 bit
    {PLACEHOLDER, "Deint Decode Bypass", 0},    // 1 bit
};

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v44 [] = {
    // totally 13
    {UINT, "Energy Metric", 4}, // 21 bits
    {PLACEHOLDER, "Iteration Number", 0},   // 4 bits
    {PLACEHOLDER, "Code Block CRC Pass", 0},    // 1 bit
    {PLACEHOLDER, "Early Termination", 0},  // 1 bit
    {PLACEHOLDER, "HARQ Combine Enable", 0},    // 1 bit
    {PLACEHOLDER, "Deint Decode Bypass", 0},    // 1 bit
};

static int _decode_lte_phy_pdsch_decoding_result_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;

    switch (pkt_ver) {
    case 24:
        {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v24,
                    ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v24, Fmt),
                    b, offset, length, result);
            unsigned int temp = _search_result_uint(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int iStartingSubframeNumber = (temp >> 9) & 15; // 4 bits
            int iStartingSystemFrameNumber = (temp >> 13) & 1023;   // 10 bits
            int iUECategory = (temp >> 24) & 15;    // 4 bits
            int iNumDlHarq = (temp >> 28) & 15; // 4 bits
            temp = _search_result_int(result, "TM Mode");
            int iTmMode = (temp >> 4) & 15;
            temp = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = temp & 7;   // 3 bits
            int num_record = (temp >> 3) & 31;  // 5 bits

            old_object = _replace_result_int(result, "Number of Records", num_record);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Starting Subframe Number",
                    iStartingSubframeNumber);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Starting System Frame Number", iStartingSystemFrameNumber);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "UE Category",
                    iUECategory);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Num DL HARQ",
                    iNumDlHarq);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "TM Mode",
                    iTmMode);
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
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v24,
                        ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v24, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "HARQ ID");
                int iHarqId = temp & 15;    // 4 bits
                int iRNTIType = (temp >> 4) & 15;   // 4 bits
                temp = _search_result_int(result_record_item,
                        "System Information Msg Number");
                int iSystemInformationMsgNumber = temp & 15;    // 4 bits
                int iSystemInformationMask = (temp >> 4) & 4095;    // 12 bits
                temp = _search_result_int(result_record_item, "HARQ Log Status");
                int iHarqLogStatus = (temp >> 3) & 3;  // 3 + 2 bits
                int iCodewordSwap = (temp >> 5) & 1;    // 1 bit
                int num_stream = (temp >> 6) & 3;   // 2 bit

                old_object = _replace_result_int(result_record_item,
                        "HARQ ID", iHarqId);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "RNTI Type", iRNTIType);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "RNTI Type",
                        ValueNameRNTIType,
                        ARRAY_SIZE(ValueNameRNTIType, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "System Information Msg Number", iSystemInformationMsgNumber);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "System Information Mask", iSystemInformationMask);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "HARQ Log Status", iHarqLogStatus);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "HARQ Log Status",
                        ValueNameHARQLogStatus,
                        ARRAY_SIZE(ValueNameHARQLogStatus, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Codeword Swap", iCodewordSwap);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Number of Streams", num_stream);
                Py_DECREF(old_object);

                PyObject *result_record_stream = PyList_New(0);
                for (int j = 0; j < num_stream; j++) {
                    PyObject *result_record_stream_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePhyPdschDecodingResult_Stream_v24,
                            ARRAY_SIZE(LtePhyPdschDecodingResult_Stream_v24, Fmt),
                            b, offset, length, result_record_stream_item);

                    temp = _search_result_uint(result_record_stream_item,
                            "Transport Block CRC");
                    int iTransportBlockCRC = temp & 1;  // 1 bit
                    int iNDI = (temp >> 1) & 1; // 1 bit
                    int iCodeBlockSizePlus = (temp >> 2) & 8191;    // 13 bits
                    int iNumCodeBlockPlus = (temp >> 15) & 15;  // 4 bits
                    int iMaxTdecIter = (temp >> 19) & 15;   // 4 bits
                    int iRetransmissionNumber = (temp >> 23) & 7;   // 3 bits
                    int iRVID = (temp >> 26) & 3;   // 2 bits
                    int iCompandingStats = (temp >> 28) & 3;    // 2 bits
                    int iHarqCombining = (temp >> 30) & 1;  // 1 bit
                    int iDecobTbCRC = (temp >> 31) & 1; // 1 bit
                    temp = _search_result_uint(result_record_stream_item,
                            "Num RE");
                    int iNumRE = (temp >> 10) & 65535;  // 10 + 6 bits
                    int iCodewordIndex = (temp >> 27) & 15; // 27 + 4 bits

                    old_object = _replace_result_int(result_record_stream_item,
                            "Transport Block CRC", iTransportBlockCRC);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "Transport Block CRC",
                            ValueNamePassOrFail,
                            ARRAY_SIZE(ValueNamePassOrFail, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_stream_item,
                            "NDI", iNDI);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Code Block Size Plus", iCodeBlockSizePlus);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Code Block Plus", iNumCodeBlockPlus);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Max TDEC Iter", iMaxTdecIter);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Retransmission Number", iRetransmissionNumber);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "Retransmission Number",
                            ValueNameNumber,
                            ARRAY_SIZE(ValueNameNumber, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_stream_item,
                            "RVID", iRVID);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Companding Stats", iCompandingStats);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "Companding Stats",
                            ValueNameCompandingStats,
                            ARRAY_SIZE(ValueNameCompandingStats, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_stream_item,
                            "HARQ Combining", iHarqCombining);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "HARQ Combining",
                            ValueNameEnableOrDisable,
                            ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_stream_item,
                            "Decob TB CRC", iDecobTbCRC);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num RE", iNumRE);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Codeword Index", iCodewordIndex);
                    Py_DECREF(old_object);
                    int num_energy_metric = iNumCodeBlockPlus;

                    PyObject *result_energy_metric = PyList_New(0);
                    for (int k = 0; k < num_energy_metric; k++) {
                        PyObject *result_energy_metric_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePhyPdschDecodingResult_EnergyMetric_v24,
                                ARRAY_SIZE(LtePhyPdschDecodingResult_EnergyMetric_v24, Fmt),
                                b, offset, length, result_energy_metric_item);
                        temp = _search_result_uint(result_energy_metric_item,
                                "Energy Metric");
                        int iEnergyMetric = temp & 2097151; // 21 bits
                        int iIterationNumber = (temp >> 21) & 15;   // 4 bits
                        int iCodeBlockCRCPass = (temp >> 25) & 1;   // 1 bit
                        int iEarlyTermination = (temp >> 26) & 1;   // 1 bit
                        int iHarqCombineEnable = (temp >> 27) & 1;  // 1 bit
                        int iDeintDecodeBypass = (temp >> 28) & 1;  // 1 bit
                        old_object = _replace_result_int(result_energy_metric_item,
                                "Energy Metric", iEnergyMetric);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_energy_metric_item,
                                "Iteration Number", iIterationNumber);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_energy_metric_item,
                                "Code Block CRC Pass", iCodeBlockCRCPass);
                        Py_DECREF(old_object);
                        (void) _map_result_field_to_name(result_energy_metric_item,
                                "Code Block CRC Pass",
                                ValueNamePassOrFail,
                                ARRAY_SIZE(ValueNamePassOrFail, ValueName),
                                "(MI)Unknown");
                        old_object = _replace_result_int(result_energy_metric_item,
                                "Early Termination", iEarlyTermination);
                        Py_DECREF(old_object);
                        (void) _map_result_field_to_name(result_energy_metric_item,
                                "Early Termination",
                                ValueNameYesOrNo,
                                ARRAY_SIZE(ValueNameYesOrNo, ValueName),
                                "(MI)Unknown");
                        old_object = _replace_result_int(result_energy_metric_item,
                                "HARQ Combine Enable", iHarqCombineEnable);
                        Py_DECREF(old_object);
                        (void) _map_result_field_to_name(result_energy_metric_item,
                                "HARQ Combine Enable",
                                ValueNameEnableOrDisable,
                                ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                                "(MI)Unknown");
                        old_object = _replace_result_int(result_energy_metric_item,
                                "Deint Decode Bypass", iDeintDecodeBypass);
                        Py_DECREF(old_object);

                        PyObject *t5 = Py_BuildValue("(sOs)", "Ignored",
                                result_energy_metric_item, "dict");
                        PyList_Append(result_energy_metric, t5);
                        Py_DECREF(t5);
                        Py_DECREF(result_energy_metric_item);
                    }
                    offset += (13 - num_energy_metric) * 4;

                    PyObject *t4 = Py_BuildValue("(sOs)", "Energy Metrics",
                            result_energy_metric, "list");
                    PyList_Append(result_record_stream_item, t4);
                    Py_DECREF(t4);
                    Py_DECREF(result_energy_metric);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_stream_item, "dict");
                    PyList_Append(result_record_stream, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_stream_item);
                }

                PyObject *t2 = Py_BuildValue("(sOs)", "Streams",
                        result_record_stream, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_stream);

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
    case 44:
        {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v44,
                    ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v44, Fmt),
                    b, offset, length, result);
            int temp = _search_result_uint(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int iStartingSubframeNumber = (temp >> 9) & 15; // 4 bits
            int iStartingSystemFrameNumber = (temp >> 13) & 1023;   // 10 bits
            int iUECategory = (temp >> 24) & 15;    // 4 bits
            int iNumDlHarq = (temp >> 28) & 15; // 4 bits
            temp = _search_result_int(result, "TM Mode");
            int iTmMode = (temp >> 4) & 15;
            temp = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = (temp >> 7) & 15;
            int num_record = (temp >> 11) & 31;

            old_object = _replace_result_int(result, "Number of Records", num_record);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Starting Subframe Number",
                    iStartingSubframeNumber);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result,
                    "Starting System Frame Number", iStartingSystemFrameNumber);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "UE Category",
                    iUECategory);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Num DL HARQ",
                    iNumDlHarq);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "TM Mode",
                    iTmMode);
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
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v44,
                        ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v44, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "HARQ ID");
                int iHarqId = temp & 15;    // 4 bits
                int iRNTIType = (temp >> 4) & 15;   // 4 bits
                temp = _search_result_int(result_record_item,
                        "System Information Msg Number");
                int iSystemInformationMsgNumber = temp & 15;    // 4 bits
                int iSystemInformationMask = (temp >> 4) & 4095;    // 12 bits
                temp = _search_result_int(result_record_item, "HARQ Log Status");
                int iHarqLogStatus = (temp >> 3) & 3;  // 3 + 2 bits
                int iCodewordSwap = (temp >> 5) & 1;    // 1 bit
                int num_stream = (temp >> 6) & 3;   // 2 bit

                old_object = _replace_result_int(result_record_item,
                        "HARQ ID", iHarqId);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "RNTI Type", iRNTIType);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "RNTI Type",
                        ValueNameRNTIType,
                        ARRAY_SIZE(ValueNameRNTIType, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "System Information Msg Number", iSystemInformationMsgNumber);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "System Information Mask", iSystemInformationMask);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "HARQ Log Status", iHarqLogStatus);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "HARQ Log Status",
                        ValueNameHARQLogStatus,
                        ARRAY_SIZE(ValueNameHARQLogStatus, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Codeword Swap", iCodewordSwap);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Number of Streams", num_stream);
                Py_DECREF(old_object);

                PyObject *result_record_stream = PyList_New(0);
                for (int j = 0; j < num_stream; j++) {
                    PyObject *result_record_stream_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePhyPdschDecodingResult_Stream_v44,
                            ARRAY_SIZE(LtePhyPdschDecodingResult_Stream_v44, Fmt),
                            b, offset, length, result_record_stream_item);

                    temp = _search_result_uint(result_record_stream_item,
                            "Transport Block CRC");
                    int iTransportBlockCRC = temp & 1;  // 1 bit
                    int iNDI = (temp >> 1) & 1; // 1 bit
                    int iCodeBlockSizePlus = (temp >> 2) & 8191;    // 13 bits
                    int iNumCodeBlockPlus = (temp >> 15) & 15;  // 4 bits
                    int iMaxTdecIter = (temp >> 19) & 15;   // 4 bits
                    int iRetransmissionNumber = (temp >> 23) & 7;   // 3 bits
                    int iRVID = (temp >> 26) & 3;   // 2 bits
                    int iCompandingStats = (temp >> 28) & 3;    // 2 bits
                    int iHarqCombining = (temp >> 30) & 1;  // 1 bit
                    int iDecobTbCRC = (temp >> 31) & 1; // 1 bit
                    temp = _search_result_uint(result_record_stream_item,
                            "Num RE");
                    int iNumRE = (temp >> 10) & 65535;  // 10 + 6 bits
                    int iCodewordIndex = (temp >> 27) & 15; // 27 + 4 bits
                    temp = _search_result_uint(result_record_stream_item,
                            "LLR Scale");
                    int iLLRScale = temp & 15;  // 4 bits

                    old_object = _replace_result_int(result_record_stream_item,
                            "Transport Block CRC", iTransportBlockCRC);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "Transport Block CRC",
                            ValueNamePassOrFail,
                            ARRAY_SIZE(ValueNamePassOrFail, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_stream_item,
                            "NDI", iNDI);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Code Block Size Plus", iCodeBlockSizePlus);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Code Block Plus", iNumCodeBlockPlus);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Max TDEC Iter", iMaxTdecIter);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Retransmission Number", iRetransmissionNumber);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "Retransmission Number",
                            ValueNameNumber,
                            ARRAY_SIZE(ValueNameNumber, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_stream_item,
                            "RVID", iRVID);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Companding Stats", iCompandingStats);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "Companding Stats",
                            ValueNameCompandingStats,
                            ARRAY_SIZE(ValueNameCompandingStats, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_stream_item,
                            "HARQ Combining", iHarqCombining);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "HARQ Combining",
                            ValueNameEnableOrDisable,
                            ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_stream_item,
                            "Decob TB CRC", iDecobTbCRC);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num RE", iNumRE);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Codeword Index", iCodewordIndex);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "LLR Scale", iLLRScale);
                    Py_DECREF(old_object);
                    int num_energy_metric = iNumCodeBlockPlus;

                    PyObject *result_energy_metric = PyList_New(0);
                    for (int k = 0; k < num_energy_metric; k++) {
                        PyObject *result_energy_metric_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePhyPdschDecodingResult_EnergyMetric_v44,
                                ARRAY_SIZE(LtePhyPdschDecodingResult_EnergyMetric_v44, Fmt),
                                b, offset, length, result_energy_metric_item);
                        temp = _search_result_uint(result_energy_metric_item,
                                "Energy Metric");
                        int iEnergyMetric = temp & 2097151; // 21 bits
                        int iIterationNumber = (temp >> 21) & 15;   // 4 bits
                        int iCodeBlockCRCPass = (temp >> 25) & 1;   // 1 bit
                        int iEarlyTermination = (temp >> 26) & 1;   // 1 bit
                        int iHarqCombineEnable = (temp >> 27) & 1;  // 1 bit
                        int iDeintDecodeBypass = (temp >> 28) & 1;  // 1 bit
                        old_object = _replace_result_int(result_energy_metric_item,
                                "Energy Metric", iEnergyMetric);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_energy_metric_item,
                                "Iteration Number", iIterationNumber);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_energy_metric_item,
                                "Code Block CRC Pass", iCodeBlockCRCPass);
                        Py_DECREF(old_object);
                        (void) _map_result_field_to_name(result_energy_metric_item,
                                "Code Block CRC Pass",
                                ValueNamePassOrFail,
                                ARRAY_SIZE(ValueNamePassOrFail, ValueName),
                                "(MI)Unknown");
                        old_object = _replace_result_int(result_energy_metric_item,
                                "Early Termination", iEarlyTermination);
                        Py_DECREF(old_object);
                        (void) _map_result_field_to_name(result_energy_metric_item,
                                "Early Termination",
                                ValueNameYesOrNo,
                                ARRAY_SIZE(ValueNameYesOrNo, ValueName),
                                "(MI)Unknown");
                        old_object = _replace_result_int(result_energy_metric_item,
                                "HARQ Combine Enable", iHarqCombineEnable);
                        Py_DECREF(old_object);
                        (void) _map_result_field_to_name(result_energy_metric_item,
                                "HARQ Combine Enable",
                                ValueNameEnableOrDisable,
                                ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                                "(MI)Unknown");
                        old_object = _replace_result_int(result_energy_metric_item,
                                "Deint Decode Bypass", iDeintDecodeBypass);
                        Py_DECREF(old_object);

                        PyObject *t5 = Py_BuildValue("(sOs)", "Ignored",
                                result_energy_metric_item, "dict");
                        PyList_Append(result_energy_metric, t5);
                        Py_DECREF(t5);
                        Py_DECREF(result_energy_metric_item);
                    }
                    offset += (13 - num_energy_metric) * 4;

                    PyObject *t4 = Py_BuildValue("(sOs)", "Energy Metrics",
                            result_energy_metric, "list");
                    PyList_Append(result_record_stream_item, t4);
                    Py_DECREF(t4);
                    Py_DECREF(result_energy_metric);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_stream_item, "dict");
                    PyList_Append(result_record_stream, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_stream_item);
                }

                PyObject *t2 = Py_BuildValue("(sOs)", "Streams",
                        result_record_stream, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_stream);

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
        printf("(MI)Unknown LTE PHY PDCCH Decoding Result version: 0x%x\n", pkt_ver);
        return 0;
    }
}
