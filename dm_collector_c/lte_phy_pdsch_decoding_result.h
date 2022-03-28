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

const Fmt LtePhyPdschDecodingResult_Payload_v106 [] = {
    {UINT, "Serving Cell ID", 4},   // 9 btis
    {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
    {PLACEHOLDER, "Starting System Frame Number", 0},   // 11 bits
    {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
    {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
    {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
    {SKIP,NULL,2},
    {UINT, "Nir",4},
    {PLACEHOLDER, "Carrier Index", 2}, //  right shift 7 bits, 4 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
};

const Fmt LtePhyPdschDecodingResult_Payload_v124 [] = {
    {UINT, "Serving Cell ID", 4},   // 9 btis
    {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
    {PLACEHOLDER, "Starting System Frame Number", 0},   // 10 bits
    {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
    {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
    {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
    {UINT, "C_RNTI",2},
    {SKIP,NULL,2},
    {UINT, "Carrier Index", 2}, //  right shift 7 bits, 4 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP,NULL,4}
};

const Fmt LtePhyPdschDecodingResult_Payload_v126 [] = {
    {UINT, "Serving Cell ID", 4},   // 9 btis
    {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
    {PLACEHOLDER, "Starting System Frame Number", 0},   // 10 bits
    {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
    {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
    {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
    {UINT, "C_RNTI",2},
    {SKIP,NULL,2},
    {UINT, "Carrier Index", 2}, //  right shift 7 bits, 4 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP,NULL,4}
};

const Fmt LtePhyPdschDecodingResult_Record_v24 [] = {
    {UINT, "Subframe Offset", 2},
    {UINT, "Subframe Number",0 },
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
    {UINT, "Subframe Number",0 },
    {UINT, "PDSCH Channel ID", 2},
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
    {UINT, "System Information Msg Number", 2}, // 4 bits
    {PLACEHOLDER, "System Information Mask", 0},    // 12 bits
    {UINT, "HARQ Log Status", 1},   // right shift 3 bit, 2 bits
    {PLACEHOLDER, "Codeword Swap", 0},  // 1 bit
    {PLACEHOLDER, "Number of Streams", 0},  // 2 bits
    {BYTE_STREAM, "Demap Sic Status",2},
    {SKIP, "Reserved",2},
};

const Fmt LtePhyPdschDecodingResult_Record_v105 [] = {
    {UINT, "Subframe Offset", 2},
    {UINT, "Subframe Number",0 },
    {UINT, "PDSCH Channel ID", 2},
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
    {UINT, "System Information Msg Number", 2}, // 4 bits
    {PLACEHOLDER, "System Information Mask", 0},    // 12 bits
    {UINT, "HARQ Log Status", 1},   // right shift 3 bit, 2 bits
    {PLACEHOLDER, "Codeword Swap", 0},  // 1 bit
    {PLACEHOLDER, "Number of Streams", 0},  // 2 bits
    {UINT, "Demap Sic Status",2},
    {SKIP, "Reserved",2},
};

const Fmt LtePhyPdschDecodingResult_Record_v106 [] = {
    {UINT, "Subframe Offset", 2},
    {UINT, "Subframe Number",0 },
    {UINT, "PDSCH Channel ID",2},
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
    {UINT, "System Information Msg Number",2},
    {PLACEHOLDER,"System Information Mask",0},
    {UINT, "HARQ Log Status",1},
    {PLACEHOLDER,"Codeword Swap",0},
    {PLACEHOLDER, "Number of Streams", 0},  // 2 bits
    {UINT, "Demap Sic Status",2},
    {PLACEHOLDER, "MVC Status",0},
    {PLACEHOLDER, "MVC Clock Request",0},
    {UINT, "MVC Req Margin Data",1},
    {UINT, "MVC Rsp Margin",1},
};
const Fmt LtePhyPdschDecodingResult_Record_v124 [] = {
    {UINT, "Subframe Offset", 2},
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
    {UINT, "Codeword Swap", 1},  // 1 bit
    {PLACEHOLDER, "Number of Transport Blks", 0},  // 2 bits
};

const Fmt LtePhyPdschDecodingResult_Record_v125 [] = {
    {UINT, "Subframe Offset", 2},
    {UINT, "Subframe Number",0 },
    {UINT, "System Subframe Number",0 },
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
    {UINT, "Codeword Swap", 1},  // 1 bit
    {PLACEHOLDER, "Number of Transport Blks", 0},  // 2 bits
};


const Fmt LtePhyPdschDecodingResult_Record_v126 [] = {
    {UINT, "Subframe Offset", 2},
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
    {UINT, "Codeword Swap", 1},  // 1 bit
    {PLACEHOLDER, "Number of Transport Blks", 0},  // 2 bits
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

const Fmt LtePhyPdschDecodingResult_Stream_v27 [] = {
    {UINT, "Transport Block CRC", 4},   // 1 bit
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
    {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
    {PLACEHOLDER, "Max TDEC Iter", 0},  // 4 bits
    {PLACEHOLDER, "RVID", 0},   // 2 bits
    {PLACEHOLDER, "Companding Stats", 0},   // 2 bits
    {PLACEHOLDER, "HARQ Combining", 0}, // 1 bit
    {PLACEHOLDER, "Decob TB CRC", 0},   // 1 bit
    {UINT, "Num RE", 4},    // right shift 10 bits, 16 bits
    {PLACEHOLDER, "Codeword Index", 0}, // right shift 27 bits, 4 bits
    {UINT, "Retransmission Number", 4},
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
    {PLACEHOLDER, "Codeword Index", 0}, // right shift 27 bits, 1 bits
    {UINT, "LLR Scale", 1}, // 4 bits
    {SKIP, NULL, 3},
};

const Fmt LtePhyPdschDecodingResult_Stream_v106 [] = {
    {UINT, "Transport Block CRC", 4},   // 1 bit
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
    {PLACEHOLDER, "Num Code Block Plus Data", 0},
    {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
    {PLACEHOLDER, "Max TDEC Iter", 0},  // 4 bits
    {PLACEHOLDER, "Retransmission Number", 0},  // 3 bits
    {PLACEHOLDER, "RVID", 0},   // 2 bits
    {UINT, "Companding Stats", 4},   // 2 bits
    {PLACEHOLDER, "HARQ Combining", 0}, // 1 bit
    {PLACEHOLDER, "Decob TB CRC", 0},   // 1 bit
    {PLACEHOLDER, "Effective Code Rate Data"},
    {PLACEHOLDER, "Num RE", 0},    // 15 bits
    {PLACEHOLDER, "Codeword Index", 0}, //  1 bits
    {UINT, "LLR Scale", 1}, // 4 bits
    {SKIP, NULL, 3},
};

const Fmt LtePhyPdschDecodingResult_TBlks_v124 [] = {
    {UINT, "Transport Block CRC", 4},   // 1 bit
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "Retransmission Number", 0},    // 1 bit
    {PLACEHOLDER, "RVID", 0},    // 1 bit
    {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
    {PLACEHOLDER, "Num Code Block Plus Data", 0},    // 4 bits
    {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
    {PLACEHOLDER, "Max Half Iter Data", 0},    // 4 bits

    {UINT, "Num Channel Bits",4},
    {PLACEHOLDER, "CW Idx", 0},  // 4 bits
    {PLACEHOLDER, "Llr Buf Valid", 0},  // 4 bits
    {PLACEHOLDER, "First Decoded CB Index", 0},  // 4 bits

    {UINT, "First Decoded CB Index Qed Iter2 Data",4},
    {PLACEHOLDER, "Last Decoded CB Index Qed Iter2 Data", 0},  // 4 bits
    {PLACEHOLDER, "Companding Format", 0},  // 4 bits
    {SKIP, NULL, 1},
    {UINT, "Effective Code Rate Data", 2},
    {PLACEHOLDER,"Effective Code Rate", 0},
    {UINT, "HARQ Combine Enable",1},
};

const Fmt LtePhyPdschDecodingResult_TBlks_v126 [] = {
    {UINT, "Transport Block CRC", 4},   // 1 bit
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "Retransmission Number", 0},    // 1 bit
    {PLACEHOLDER, "RVID", 0},    // 1 bit
    {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
    {PLACEHOLDER, "Num Code Block Plus Data", 0},    // 4 bits
    {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits
    {PLACEHOLDER, "Max Half Iter Data", 0},    // 4 bits

    {UINT, "Num Channel Bits",4},
    {PLACEHOLDER, "CW Idx", 0},  // 4 bits
    {PLACEHOLDER, "Llr Buf Valid", 0},  // 4 bits
    {PLACEHOLDER, "First Decoded CB Index", 0},  // 4 bits

    {UINT, "First Decoded CB Index Qed Iter2 Data",4},
    {PLACEHOLDER, "Last Decoded CB Index Qed Iter2 Data", 0},  // 4 bits
    {PLACEHOLDER, "Companding Format", 0},  // 4 bits

    {SKIP, NULL, 1},

    {UINT,"Effective Code Rate Data",2},
    {PLACEHOLDER,"Effective Code Rate", 0},
    {UINT, "HARQ Combine Enable",1},
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

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v106 [] = {
    // totally 13
    {UINT, "Energy Metric", 4},
    {PLACEHOLDER, "Iteration Num", 0},
    {PLACEHOLDER, "Code Block CRC Pass", 0},
    {PLACEHOLDER, "Early Termination", 0},
    {PLACEHOLDER, "HARQ Combine Enable", 0},
    {PLACEHOLDER, "Deint Decode Bypass", 0},
};

const Fmt LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v106 [] = {
    {UINT, "Hidden Energy Metric First Half", 4},
};

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v124 [] = {
    // totally 13
    {UINT, "Energy Metric", 4}, // 21 bits
    {PLACEHOLDER, "Min Abs LLR", 0},   // 4 bits

    {UINT, "Half Iter Run Data", 4}, // 21 bits
    {PLACEHOLDER, "Half Iter Run", 0},   // 4 bits
    {PLACEHOLDER, "Code Block CRC Pass", 0},    // 1 bit
};

const Fmt LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v124 [] = {
    // totally 13
    {UINT, "Hidden Energy Metric First Half", 4},
    {UINT, "Hidden Energy Metric Second Half", 4},
};

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v126 [] = {
    // totally 13
    {UINT, "Energy Metric", 4}, // 21 bits
    {PLACEHOLDER, "Min Abs LLR", 0},   // 4 bits

    {UINT, "Half Iter Run Data", 4}, // 21 bits
    {PLACEHOLDER, "Half Iter Run", 0},   // 4 bits
    {PLACEHOLDER, "Code Block CRC Pass", 0},    // 1 bit
};

const Fmt LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v126 [] = {
    // totally 13
    {UINT, "Hidden Energy Metric First Half", 4},
    {UINT, "Hidden Energy Metric Second Half", 4},
};

const Fmt LtePhyPdschDecodingResult_Payload_v143 [] = {
    {UINT, "Serving Cell ID", 4},   // 9 btis
    {PLACEHOLDER, "Starting Subframe Number", 0},   // 4 bits
    {PLACEHOLDER, "Starting System Frame Number", 0},   // 10 bits
    {PLACEHOLDER, "UE Category", 0},    // right shift 1 bit, 4 bits
    {PLACEHOLDER, "Num DL HARQ", 0},    // 4 bits
    {UINT, "TM Mode", 1},   // right shift 4 bits, 4 bits
    {UINT, "C_RNTI",2},
    {SKIP,NULL,2},
    {UINT, "Carrier Index", 2}, //  right shift 7 bits, 4 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP,NULL,4}
};

const Fmt LtePhyPdschDecodingResult_Record_v143 [] = {
    {UINT, "Subframe Offset", 2},
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RNTI Type", 0},  // 4 bits
    {UINT, "Codeword Swap", 1},  // 1 bit
    {PLACEHOLDER, "Number of Transport Blks", 0},  // 2 bits
};

const Fmt LtePhyPdschDecodingResult_TBlks_v143 [] = {
    {UINT, "Transport Block CRC", 4},   // 1 bit
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "Retransmission Number", 0},    // 1 bit
    {PLACEHOLDER, "RVID", 0},    // 1 bit
    {PLACEHOLDER, "Code Block Size Plus", 0},   // 13 bits
    {PLACEHOLDER, "Num Code Block Plus Data", 0},    // 4 bits
    {PLACEHOLDER, "Num Code Block Plus", 0},    // 4 bits

    {UINT, "Max Half Iter Data", 4},    // 4 bits
    {PLACEHOLDER, "Num Channel Bits",4}, // 19 bits
    {PLACEHOLDER, "CW Idx", 0},  // 1 bits
    {PLACEHOLDER, "Llr Buf Valid", 0},  // 4 bits
    {PLACEHOLDER, "First Decoded CB Index", 0},  // 4 bits

    //{UINT, "First Decoded CB Index Qed Iter2 Data",4},
    //{PLACEHOLDER, "Last Decoded CB Index Qed Iter2 Data", 0},  // 4 bits
    //{PLACEHOLDER, "Companding Format", 0},  // 4 bits

    {UINT, "Companding Format", 4},  // shift 18 bit 2 bits

    {SKIP, NULL, 1},

    {UINT,"Effective Code Rate Data",2},
    {UINT, "HARQ Combine Enable",1},
};

const Fmt LtePhyPdschDecodingResult_EnergyMetric_v143 [] = {
    // totally 13
    {UINT, "Energy Metric", 4}, // 21 bits
    {PLACEHOLDER, "Min Abs LLR", 0},   // 4 bits

    {UINT, "Half Iter Run Data", 4}, // 21 bits
    {PLACEHOLDER, "Half Iter Run", 0},   // 4 bits
    {PLACEHOLDER, "Code Block CRC Pass", 0},    // 1 bit
};

const Fmt LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v143 [] = {
    // totally 13
    {UINT, "Hidden Energy Metric First Half", 4},
    {UINT, "Hidden Energy Metric Second Half", 4},
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
                temp = _search_result_int(result_record_item, "Subframe Offset");
                int iSubframeNumber=(temp+iStartingSubframeNumber)%10;

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
                        "Subframe Number", iSubframeNumber);
                Py_DECREF(old_object);
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
    case 27:
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
                    offset += _decode_by_fmt(LtePhyPdschDecodingResult_Stream_v27,
                            ARRAY_SIZE(LtePhyPdschDecodingResult_Stream_v27, Fmt),
                            b, offset, length, result_record_stream_item);

                    temp = _search_result_uint(result_record_stream_item,
                            "Transport Block CRC");
                    int iTransportBlockCRC = temp & 1;  // 1 bit
                    int iNDI = (temp >> 1) & 1; // 1 bit
                    int iCodeBlockSizePlus = (temp >> 2) & 8191;    // 13 bits
                    int iNumCodeBlockPlus = (temp >> 15) & 15;  // 4 bits
                    int iMaxTdecIter = (temp >> 19) & 15;   // 4 bits
                    //reserved 3 bit
                    int iRVID = (temp >> 26) & 3;   // 2 bits
                    int iCompandingStats = (temp >> 28) & 3;    // 2 bits
                    int iHarqCombining = (temp >> 30) & 1;  // 1 bit
                    int iDecobTbCRC = (temp >> 31) & 1; // 1 bit

                    temp = _search_result_uint(result_record_stream_item,
                            "Num RE");
                    int iNumRE = (temp >> 10) & 65535;  // 10 + 6 bits
                    int iCodewordIndex = (temp >> 27) & 1; // 27 + 1 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "Retransmission Number");
                    int iRetransmissionNumber = temp & 0xfff;   // 12 bits

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
                temp = _search_result_int(result_record_item, "Subframe Offset");
                int iSubframeNumber=(temp+iStartingSubframeNumber)%10;
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
                        "Subframe Number", iSubframeNumber);
                Py_DECREF(old_object);
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
    case 105:
        {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v106,
                    ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v106, Fmt),
                    b, offset, length, result);
            int temp = _search_result_uint(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int iStartingSubframeNumber = (temp >> 9) & 15; // 4 bits
            int iStartingSystemFrameNumber = (temp >> 13) & 1023;   // 11 bits
            int iUECategory = (temp >> 24) & 15;    // 4 bits
            int iNumDlHarq = (temp >> 28) & 15; // 4 bits
            temp = _search_result_int(result, "TM Mode");
            int iTmMode = (temp >> 4) & 15;//4 bit

            temp = _search_result_int(result, "Nir");
            int iNir = (temp) & 0x7fffff;//23 bit
            int iCarrierIndex = (temp >> 23) & 15;//4 bit
            int num_record = (temp >> 27) & 31;// 5 bit

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
            old_object = _replace_result_int(result, "Nir", iNir);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Carrier Index", iCarrierIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Carrier Index",
                    ValueNameCarrierIndex,
                    ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Number of Records", num_record);
            Py_DECREF(old_object);
            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v105,
                        ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v105, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "Subframe Offset");
                int iSubframeNumber=(temp+iStartingSubframeNumber)%10;
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

                temp = _search_result_int(result_record_item, "Demap Sic Status");
                int iDemapSicStatus = temp & 0x1fff; //13 bit

                old_object = _replace_result_int(result_record_item,
                        "Subframe Number", iSubframeNumber);
                Py_DECREF(old_object);
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
                old_object = _replace_result_int(result_record_item,
                        "Demap Sic Status",iDemapSicStatus);
                Py_DECREF(old_object);
                //iNumofTBlks=0;
                PyObject *result_record_stream = PyList_New(0);
                for (int j = 0; j < num_stream; j++) {
                    PyObject *result_record_stream_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePhyPdschDecodingResult_Stream_v106,
                            ARRAY_SIZE(LtePhyPdschDecodingResult_Stream_v106, Fmt),
                            b, offset, length, result_record_stream_item);

                    temp = _search_result_uint(result_record_stream_item,
                            "Transport Block CRC");
                    int iTransportBlockCRC = temp & 1;  // 1 bit
                    int iNDI = (temp >> 1) & 1; // 1 bit
                    int iCodeBlockSizePlus = (temp >> 2) & 8191 ;    // 13 bits
                    int iNumCodeBlockPlusData=((temp >> 15) & 0x1f);
                    int iNumCodeBlockPlus = iNumCodeBlockPlusData +1;  // 5 bits
                    int iMaxTdecIter = (temp >> 20) & 15;   // 4 bits
                    int iRetransmissionNumber = (temp >> 24) & 0x3f;   // 6 bits
                    int iRVID = (temp >> 30) & 3;   // 2 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "Companding Stats");
                    int iCompandingStats = (temp) & 3;    // 2 bits
                    int iHarqCombining = (temp >> 2) & 1;  // 1 bit
                    int iDecobTbCRC = (temp >> 3) & 1; // 1 bit

                    int iNumRE = (temp >> 14) & 0x7fff;  // 15 bits
                    int iCodewordIndex = (temp >> 30) & 1; //1 bit

                    temp = _search_result_uint(result_record_stream_item,
                            "LLR Scale");
                    int iLLRScale = (temp>>6) & 3;  // 2 bits

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
                            "Num Code Block Plus Data", iNumCodeBlockPlusData);
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
                    offset += (32 - num_energy_metric) * 4;

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
    case 106:
        {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v106,
                    ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v106, Fmt),
                    b, offset, length, result);
            int temp = _search_result_uint(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int iStartingSubframeNumber = (temp >> 9) & 15; // 4 bits
            int iStartingSystemFrameNumber = (temp >> 13) & 1023;   // 11 bits
            int iUECategory = (temp >> 24) & 15;    // 4 bits
            int iNumDlHarq = (temp >> 28) & 15; // 4 bits
            temp = _search_result_int(result, "TM Mode");
            int iTmMode = (temp >> 4) & 15;//4 bit

            temp = _search_result_int(result, "Nir");
            int iNir = (temp) & 0x7fffff;//23 bit
            int iCarrierIndex = (temp >> 23) & 15;//4 bit
            int num_record = (temp >> 27) & 31;// 5 bit

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
            old_object = _replace_result_int(result, "Nir", iNir);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Carrier Index", iCarrierIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Carrier Index",
                    ValueNameCarrierIndex,
                    ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Number of Records", num_record);
            Py_DECREF(old_object);
            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v106,
                        ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v106, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "Subframe Offset");
                int iSubframeNumber=(temp+iStartingSubframeNumber)%10;

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

                temp = _search_result_int(result_record_item, "Demap Sic Status");
                int iDemapSicStatus = temp & 0x1fff; //13 bit
                int iMVCStatus = (temp>>13) & 0x1; //1 bit
                int iMVCClockRequest = (temp>>14) & 0x3; //2 bit
                
                old_object = _replace_result_int(result_record_item,
                        "Subframe Number", iSubframeNumber);
                Py_DECREF(old_object);

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
                old_object = _replace_result_int(result_record_item,
                        "Demap Sic Status",iDemapSicStatus);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "MVC Status",iMVCStatus);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "MVC Clock Request",iMVCClockRequest);
                Py_DECREF(old_object);
                //iNumofTBlks=0;
                PyObject *result_record_stream = PyList_New(0);
                for (int j = 0; j < num_stream; j++) {
                    PyObject *result_record_stream_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePhyPdschDecodingResult_Stream_v106,
                            ARRAY_SIZE(LtePhyPdschDecodingResult_Stream_v106, Fmt),
                            b, offset, length, result_record_stream_item);

                    temp = _search_result_uint(result_record_stream_item,
                            "Transport Block CRC");
                    int iTransportBlockCRC = temp & 1;  // 1 bit
                    int iNDI = (temp >> 1) & 1; // 1 bit
                    int iCodeBlockSizePlus = (temp >> 2) & 8191 ;    // 13 bits
                    int iNumCodeBlockPlusData= ((temp >> 15) & 0x1f);
                    int iNumCodeBlockPlus = iNumCodeBlockPlusData +1;  // 5 bits
                    int iMaxTdecIter = (temp >> 20) & 15;   // 4 bits
                    int iRetransmissionNumber = (temp >> 24) & 0x3f;   // 6 bits
                    int iRVID = (temp >> 30) & 3;   // 2 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "Companding Stats");
                    int iCompandingStats = (temp) & 3;    // 2 bits
                    int iHarqCombining = (temp >> 2) & 1;  // 1 bit
                    int iDecobTbCRC = (temp >> 3) & 1; // 1 bit
                    int iEffectiveCodeRateData = (temp>>4) & 0x3ff;//10 bit

                    int iNumRE = (temp >> 14) & 0x7fff;  // 15 bits
                    int iCodewordIndex = (temp >> 30) & 1; //1 bit

                    temp = _search_result_uint(result_record_stream_item,
                            "LLR Scale");
                    int iLLRScale = (temp>>6) & 3;  // 2 bits

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
                            "Num Code Block Plus Data", iNumCodeBlockPlusData);
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
                            "Effective Code Rate Data", iEffectiveCodeRateData);
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
                    offset += (32 - num_energy_metric) * 4;

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
    case 124:
        {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v124,
                    ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v124, Fmt),
                    b, offset, length, result);
            int temp = _search_result_uint(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int iStartingSubframeNumber = (temp >> 9) & 15; // 4 bits
            int iStartingSystemFrameNumber = (temp >> 13) & 2047;   // 11 bits
            int iUECategory = (temp >> 24) & 15;    // 4 bits
            int iNumDlHarq = (temp >> 28) & 15; // 4 bits
            temp = _search_result_int(result, "TM Mode");
            int iTmMode = (temp >> 4) & 15;//4 bit
            temp = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = (temp >> 7) & 15;//4 bit
            int num_record = (temp >> 11) & 31;//

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
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v124,
                        ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v124, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "HARQ ID");
                int iHarqId = temp & 15;    // 4 bits
                int iRNTIType = (temp >> 4) & 15;   // 4 bits

                temp = _search_result_int(result_record_item,
                        "Codeword Swap");
                int iCodewordSwap = temp & 1;    // 1 bits
                int iNumofTBlks = (temp >> 1) & 7;    // 3 bits

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
                        "Codeword Swap", iCodewordSwap);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Number of Transport Blks", iNumofTBlks);
                Py_DECREF(old_object);

                //iNumofTBlks=0;
                offset+=8;
                PyObject *result_record_stream = PyList_New(0);
                for (int j = 0; j < iNumofTBlks; j++) {
                    PyObject *result_record_stream_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePhyPdschDecodingResult_TBlks_v124,
                            ARRAY_SIZE(LtePhyPdschDecodingResult_TBlks_v124, Fmt),
                            b, offset, length, result_record_stream_item);

                    temp = _search_result_uint(result_record_stream_item,
                            "Transport Block CRC");
                    int iTransportBlockCRC = temp & 1;  // 1 bit
                    int iNDI = (temp >> 1) & 1; // 1 bit
                    int iRetransmissionNumber = (temp >> 2) & 7;   // 3 bits
                    int iRVID = (temp >> 8) & 3;   // 2 bits

                    int iCodeBlockSizePlus = (temp >> 10) & 16383;    // 14 bits

                    int iNumCodeBlockPlusData = (temp >> 23) & 3;  // 2 bits
                    int iNumCodeBlockPlus = iNumCodeBlockPlusData +1;

                    int iMaxHalfIterData=(temp>>28) & 15;//4 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "Num Channel Bits");
                    int iNumChannelBits=temp & 1023;//10 bits
                    int iCwIdx = (temp>>6)&3;//2 bits
                    int iLlrBufValid=(temp>>9)&3;//2 bits
                    int iFirstDecodedCBIndex=(temp>>11)&7;//2 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "First Decoded CB Index Qed Iter2 Data");
                    int iFirstDecodedCBIndexQedIter2Data=(temp>>6)&63;

                    int iLastDecodedCBIndexQedIter2Data=(temp>>12)&63;

                    int iCompandingFormat=(temp>>18)&31;//4 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "HARQ Combine Enable");
                    int iHarqCombineEnable=(temp>>1) &7;

                    temp = _search_result_uint(result_record_stream_item,"Effective Code Rate Data");
                    int iEffectiveCodeRateData = (temp>>5) & (0x7ff);
                    float fEffectiveCodeRateData=iEffectiveCodeRateData*1.0/1024.0;
                    std::string temps;
                    char hex[10]={};
                    sprintf(hex,"%0.3f",fEffectiveCodeRateData);
                    temps+=hex;
                    PyObject *pystr1= Py_BuildValue("s", temps.c_str());
                    PyObject *old_object2 = _replace_result(result_record_stream_item,"Effective Code Rate", pystr1);
                    Py_DECREF(old_object2);
                    Py_DECREF(pystr1);

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
                            "Code Block Size Plus", iCodeBlockSizePlus);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Code Block Plus Data", iNumCodeBlockPlusData);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Code Block Plus", iNumCodeBlockPlus);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Max Half Iter Data", iMaxHalfIterData);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Channel Bits", iNumChannelBits);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "CW Idx", iCwIdx);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Llr Buf Valid", iLlrBufValid);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "First Decoded CB Index", iFirstDecodedCBIndex);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "First Decoded CB Index Qed Iter2 Data", iFirstDecodedCBIndexQedIter2Data);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Last Decoded CB Index Qed Iter2 Data", iLastDecodedCBIndexQedIter2Data
                            );
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Companding Format", iCompandingFormat);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "Companding Format",
                            ValueNameCompandingStats,
                            ARRAY_SIZE(ValueNameCompandingStats, ValueName),
                            "(MI)Unknown");

                    old_object = _replace_result_int(result_record_stream_item,
                            "Effective Code Rate Data", iEffectiveCodeRateData);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "HARQ Combine Enable", iHarqCombineEnable);
                    Py_DECREF(old_object);


                    for(int k=0;k<iNumCodeBlockPlus;k++){
                        offset+=8;
                    }

                    int count_temp=0;
                    while ((unsigned int)(offset - start) < length) {
                        PyObject *result_hidden_record = PyList_New(0);
                        offset += _decode_by_fmt(LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v124,
                                ARRAY_SIZE(LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v124, Fmt),
                                b, offset, length, result_hidden_record);

                        int first_half_temp = _search_result_int(result_hidden_record,
                                "Hidden Energy Metric First Half");
                        int second_half_temp=_search_result_int(result_hidden_record,
                                "Hidden Energy Metric Second Half");
                        if(first_half_temp!=0 or second_half_temp!=0){
                            break;
                        }
                        count_temp=count_temp+1;
                        Py_DECREF(result_hidden_record);
                    }

                    offset=offset-8;

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
    case 125:
        {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v124,
                    ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v124, Fmt),
                    b, offset, length, result);
            int temp = _search_result_uint(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int iStartingSubframeNumber = (temp >> 9) & 15; // 4 bits
            int iStartingSystemFrameNumber = (temp >> 13) & 2047;   // 11 bits
            int iUECategory = (temp >> 24) & 15;    // 4 bits
            int iNumDlHarq = (temp >> 28) & 15; // 4 bits
            temp = _search_result_int(result, "TM Mode");
            int iTmMode = (temp >> 4) & 15;//4 bit
            temp = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = (temp >> 7) & 15;//4 bit
            int num_record = (temp >> 11) & 31;//

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
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v125,
                        ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v125, Fmt),
                        b, offset, length, result_record_item);

                int iSubFrameOffset=_search_result_int(result_record_item,"Subframe Offset");
                temp=iSubFrameOffset+iStartingSubframeNumber;
                int iSubframeNumber=temp%10;
                int iSystemFrameNumber=iStartingSystemFrameNumber+temp/10;
                temp = _search_result_int(result_record_item, "HARQ ID");
                int iHarqId = temp & 15;    // 4 bits
                int iRNTIType = (temp >> 4) & 15;   // 4 bits

                temp = _search_result_int(result_record_item,
                        "Codeword Swap");
                int iCodewordSwap = temp & 1;    // 1 bits
                int iNumofTBlks = (temp >> 1) & 7;    // 3 bits
                old_object = _replace_result_int(result_record_item,
                        "Subframe Number", iSubframeNumber);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "System Subframe Number", iSystemFrameNumber);
                Py_DECREF(old_object);
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
                        "Codeword Swap", iCodewordSwap);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Number of Transport Blks", iNumofTBlks);
                Py_DECREF(old_object);

                //iNumofTBlks=0;
                offset+=8;
                PyObject *result_record_stream = PyList_New(0);
                for (int j = 0; j < iNumofTBlks; j++) {
                    PyObject *result_record_stream_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePhyPdschDecodingResult_TBlks_v124,
                            ARRAY_SIZE(LtePhyPdschDecodingResult_TBlks_v124, Fmt),
                            b, offset, length, result_record_stream_item);

                    temp = _search_result_uint(result_record_stream_item,
                            "Transport Block CRC");
                    int iTransportBlockCRC = temp & 1;  // 1 bit
                    int iNDI = (temp >> 1) & 1; // 1 bit
                    int iRetransmissionNumber = (temp >> 2) & 0x3f;   // 6 bits
                    int iRVID = (temp >> 8) & 3;   // 2 bits

                    int iCodeBlockSizePlus = (temp >> 10) & 0x1fff;    // 13 bits

                    int iNumCodeBlockPlusData = (temp >> 23) & 0x1f;  // 5 bits
                    int iNumCodeBlockPlus = iNumCodeBlockPlusData +1;

                    int iMaxHalfIterData=(temp>>28) & 15;//4 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "Num Channel Bits");
                    int iNumChannelBits=temp & 0x7ffff;//19 bits
                    int iCwIdx = (temp>>19)&1;//1 bits
                    int iLlrBufValid=(temp>>20)&1;//2 bits
                    int iFirstDecodedCBIndex=(temp>>21)&0x3f;//6 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "First Decoded CB Index Qed Iter2 Data");
                    int iFirstDecodedCBIndexQedIter2Data=(temp>>6)&63;

                    int iLastDecodedCBIndexQedIter2Data=(temp>>12)&63;

                    int iCompandingFormat=(temp>>18)&31;//4 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "HARQ Combine Enable");
                    int iHarqCombineEnable=(temp>>1) &7;

                    temp = _search_result_uint(result_record_stream_item,"Effective Code Rate Data");
                    int iEffectiveCodeRateData = (temp>>5) & (0x7ff);
                    float fEffectiveCodeRateData=iEffectiveCodeRateData*1.0/1024.0;
                    std::string temps;
                    char hex[10]={};
                    sprintf(hex,"%0.3f",fEffectiveCodeRateData);
                    temps+=hex;
                    PyObject *pystr1= Py_BuildValue("s", temps.c_str());
                    PyObject *old_object2 = _replace_result(result_record_stream_item,"Effective Code Rate", pystr1);
                    Py_DECREF(old_object2);
                    Py_DECREF(pystr1);

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
                            "Code Block Size Plus", iCodeBlockSizePlus);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Code Block Plus Data", iNumCodeBlockPlusData);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Code Block Plus", iNumCodeBlockPlus);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Max Half Iter Data", iMaxHalfIterData);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Channel Bits", iNumChannelBits);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "CW Idx", iCwIdx);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Llr Buf Valid", iLlrBufValid);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "First Decoded CB Index", iFirstDecodedCBIndex);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "First Decoded CB Index Qed Iter2 Data", iFirstDecodedCBIndexQedIter2Data);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Last Decoded CB Index Qed Iter2 Data", iLastDecodedCBIndexQedIter2Data
                            );
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Companding Format", iCompandingFormat);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "Companding Format",
                            ValueNameCompandingStats,
                            ARRAY_SIZE(ValueNameCompandingStats, ValueName),
                            "(MI)Unknown");

                    old_object = _replace_result_int(result_record_stream_item,
                            "Effective Code Rate Data", iEffectiveCodeRateData);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "HARQ Combine Enable", iHarqCombineEnable);
                    Py_DECREF(old_object);


                    for(int k=0;k<iNumCodeBlockPlus;k++){
                        offset+=8;
                    }

                    int count_temp=0;
                    while ((unsigned int)(offset - start) < length) {
                        PyObject *result_hidden_record = PyList_New(0);
                        offset += _decode_by_fmt(LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v124,
                                ARRAY_SIZE(LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v124, Fmt),
                                b, offset, length, result_hidden_record);

                        int first_half_temp = _search_result_int(result_hidden_record,
                                "Hidden Energy Metric First Half");
                        int second_half_temp=_search_result_int(result_hidden_record,
                                "Hidden Energy Metric Second Half");
                        if(first_half_temp!=0 or second_half_temp!=0){
                            break;
                        }
                        count_temp=count_temp+1;
                        Py_DECREF(result_hidden_record);
                    }

                    offset=offset-8;

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
    case 126:
        {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v126,
                    ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v126, Fmt),
                    b, offset, length, result);
            int temp = _search_result_uint(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int iStartingSubframeNumber = (temp >> 9) & 15; // 4 bits
            int iStartingSystemFrameNumber = (temp >> 13) & 2047;   // 11 bits
            int iUECategory = (temp >> 24) & 15;    // 4 bits
            int iNumDlHarq = (temp >> 28) & 15; // 4 bits
            temp = _search_result_int(result, "TM Mode");
            int iTmMode = (temp >> 4) & 15;//4 bit
            temp = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = (temp >> 7) & 15;//4 bit
            int num_record = (temp >> 11) & 31;//

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
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v126,
                        ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v126, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "HARQ ID");
                int iHarqId = temp & 15;    // 4 bits
                int iRNTIType = (temp >> 4) & 15;   // 4 bits

                temp = _search_result_int(result_record_item,
                        "Codeword Swap");
                int iCodewordSwap = temp & 1;    // 1 bits
                int iNumofTBlks = (temp >> 1) & 7;    // 3 bits

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
                        "Codeword Swap", iCodewordSwap);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Number of Transport Blks", iNumofTBlks);
                Py_DECREF(old_object);

                //iNumofTBlks=0;
                offset+=8;
                PyObject *result_record_stream = PyList_New(0);
                for (int j = 0; j < iNumofTBlks; j++) {
                    PyObject *result_record_stream_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePhyPdschDecodingResult_TBlks_v126,
                            ARRAY_SIZE(LtePhyPdschDecodingResult_TBlks_v126, Fmt),
                            b, offset, length, result_record_stream_item);

                    temp = _search_result_uint(result_record_stream_item,
                            "Transport Block CRC");
                    int iTransportBlockCRC = temp & 1;  // 1 bit
                    int iNDI = (temp >> 1) & 1; // 1 bit
                    int iRetransmissionNumber = (temp >> 2) & 0x3f;   // 6 bits
                    int iRVID = (temp >> 8) & 3;   // 2 bits

                    int iCodeBlockSizePlus = (temp >> 10) & 0x1fff;    // 13 bits

                    int iNumCodeBlockPlusData = (temp >> 23) & 0x1f;  // 5 bits
                    int iNumCodeBlockPlus = iNumCodeBlockPlusData +1;

                    int iMaxHalfIterData=(temp>>28) & 15;//4 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "Num Channel Bits");
                    int iNumChannelBits=temp & 0x7ffff;//19 bits
                    int iCwIdx = (temp>>19)&1;//1 bits
                    int iLlrBufValid=(temp>>20)&1;//2 bits
                    int iFirstDecodedCBIndex=(temp>>21)&0x3f;//6 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "First Decoded CB Index Qed Iter2 Data");
                    int iFirstDecodedCBIndexQedIter2Data=(temp>>6)&63;

                    int iLastDecodedCBIndexQedIter2Data=(temp>>12)&63;

                    int iCompandingFormat=(temp>>18)&31;//4 bits


                    temp = _search_result_uint(result_record_stream_item,
                            "Effective Code Rate Data");

                    int iEffectiveCodeRateData=(temp>>5) & 0x7ff;

                    float fEffectiveCodeRateData=iEffectiveCodeRateData*1.0/1024.0;
                    std::string temps;
                    char hex[10]={};
                    sprintf(hex,"%0.3f",fEffectiveCodeRateData);
                    temps+=hex;
                    PyObject *pystr1= Py_BuildValue("s", temps.c_str());
                    PyObject *old_object2 = _replace_result(result_record_stream_item,"Effective Code Rate", pystr1);
                    Py_DECREF(old_object2);
                    Py_DECREF(pystr1);
                    
                    temp = _search_result_uint(result_record_stream_item,
                            "HARQ Combine Enable");
                    int iHarqCombineEnable=(temp>>1) &7;

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
                            "Code Block Size Plus", iCodeBlockSizePlus);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Code Block Plus Data", iNumCodeBlockPlusData);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Code Block Plus", iNumCodeBlockPlus);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Max Half Iter Data", iMaxHalfIterData);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Channel Bits", iNumChannelBits);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "CW Idx", iCwIdx);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Llr Buf Valid", iLlrBufValid);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "First Decoded CB Index", iFirstDecodedCBIndex);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "First Decoded CB Index Qed Iter2 Data", iFirstDecodedCBIndexQedIter2Data);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Last Decoded CB Index Qed Iter2 Data", iLastDecodedCBIndexQedIter2Data
                            );
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Companding Format", iCompandingFormat);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "Companding Format",
                            ValueNameCompandingStats,
                            ARRAY_SIZE(ValueNameCompandingStats, ValueName),
                            "(MI)Unknown");

                    old_object = _replace_result_int(result_record_stream_item,
                            "Effective Code Rate Data", iEffectiveCodeRateData);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "HARQ Combine Enable", iHarqCombineEnable);
                    Py_DECREF(old_object);


                    for(int k=0;k<iNumCodeBlockPlus;k++){
                        offset+=8;
                    }

                    int count_temp=0;
                    while ((unsigned int)(offset - start) < length) {
                        PyObject *result_hidden_record = PyList_New(0);
                        offset += _decode_by_fmt(LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v126,
                                ARRAY_SIZE(LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v126, Fmt),
                                b, offset, length, result_hidden_record);

                        int first_half_temp = _search_result_int(result_hidden_record,
                                "Hidden Energy Metric First Half");
                        int second_half_temp=_search_result_int(result_hidden_record,
                                "Hidden Energy Metric Second Half");
                        if(first_half_temp!=0 or second_half_temp!=0){
                            break;
                        }
                        count_temp=count_temp+1;
                        Py_DECREF(result_hidden_record);
                    }

                    offset=offset-8;

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
    case 143:
        {
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Payload_v143,
                    ARRAY_SIZE(LtePhyPdschDecodingResult_Payload_v143, Fmt),
                    b, offset, length, result);
            int temp = _search_result_uint(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int iStartingSubframeNumber = (temp >> 9) & 15; // 4 bits
            int iStartingSystemFrameNumber = (temp >> 13) & 2047;   // 11 bits
            int iUECategory = (temp >> 24) & 15;    // 4 bits
            int iNumDlHarq = (temp >> 28) & 15; // 4 bits
            temp = _search_result_int(result, "TM Mode");
            int iTmMode = (temp >> 4) & 15;//4 bit
            temp = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = (temp >> 7) & 15;//4 bit
            int num_record = (temp >> 11) & 31;//

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
                offset += _decode_by_fmt(LtePhyPdschDecodingResult_Record_v143,
                        ARRAY_SIZE(LtePhyPdschDecodingResult_Record_v143, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "HARQ ID");
                int iHarqId = temp & 15;    // 4 bits
                int iRNTIType = (temp >> 4) & 15;   // 4 bits

                temp = _search_result_int(result_record_item,
                        "Codeword Swap");
                int iCodewordSwap = temp & 1;    // 1 bits
                int iNumofTBlks = (temp >> 1) & 3;    // 2 bits

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
                        "Codeword Swap", iCodewordSwap);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Number of Transport Blks", iNumofTBlks);
                Py_DECREF(old_object);

                //iNumofTBlks=0;
                offset+=8;
                PyObject *result_record_stream = PyList_New(0);
                for (int j = 0; j < iNumofTBlks; j++) {
                    PyObject *result_record_stream_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePhyPdschDecodingResult_TBlks_v143,
                            ARRAY_SIZE(LtePhyPdschDecodingResult_TBlks_v143, Fmt),
                            b, offset, length, result_record_stream_item);

                    temp = _search_result_uint(result_record_stream_item,
                            "Transport Block CRC");
                    int iTransportBlockCRC = temp & 1;  // 1 bit
                    int iNDI = (temp >> 1) & 1; // 1 bit
                    int iRetransmissionNumber = (temp >> 2) & 0x3f;   // 6 bits
                    int iRVID = (temp >> 8) & 3;   // 2 bits

                    int iCodeBlockSizePlus = (temp >> 10) & 0x1fff;    // 13 bits

                    int iNumCodeBlockPlusData = (temp >> 23) & 0x1f;  // 5 bits
                    int iNumCodeBlockPlus = iNumCodeBlockPlusData +1;

                    temp = _search_result_uint(result_record_stream_item,
                            "Max Half Iter Data");
                    int iMaxHalfIterData=(temp>>0) & 15;//4 bits
                    int iNumChannelBits=(temp>>4) & 0x7ffff;//19 bits
                    int iCwIdx = (temp>>23)&1;//1 bits
                    int iLlrBufValid=(temp>>24)&1;//2 bits
                    int iFirstDecodedCBIndex=(temp>>26)&0x3f;//6 bits

                    temp = _search_result_uint(result_record_stream_item,
                            "Companding Format");
                    int iCompandingFormat=(temp>>18)&0x3;//2 bits


                    temp = _search_result_uint(result_record_stream_item,
                            "Effective Code Rate Data");

                    int iEffectiveCodeRateData=(temp>>5) & 0x7ff;

                    temp = _search_result_uint(result_record_stream_item,
                            "HARQ Combine Enable");
                    int iHarqCombineEnable=(temp>>1) &7;

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
                            "Code Block Size Plus", iCodeBlockSizePlus);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Code Block Plus Data", iNumCodeBlockPlusData);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Code Block Plus", iNumCodeBlockPlus);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Max Half Iter Data", iMaxHalfIterData);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Num Channel Bits", iNumChannelBits);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "CW Idx", iCwIdx);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Llr Buf Valid", iLlrBufValid);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "First Decoded CB Index", iFirstDecodedCBIndex);
                    Py_DECREF(old_object);

                    /*
                    old_object = _replace_result_int(result_record_stream_item,
                            "First Decoded CB Index Qed Iter2 Data", iFirstDecodedCBIndexQedIter2Data);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "Last Decoded CB Index Qed Iter2 Data", iLastDecodedCBIndexQedIter2Data
                            );
                    Py_DECREF(old_object);
                    */

                    old_object = _replace_result_int(result_record_stream_item,
                            "Companding Format", iCompandingFormat);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_stream_item,
                            "Companding Format",
                            ValueNameCompandingStats,
                            ARRAY_SIZE(ValueNameCompandingStats, ValueName),
                            "(MI)Unknown");

                    old_object = _replace_result_int(result_record_stream_item,
                            "Effective Code Rate Data", iEffectiveCodeRateData);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_stream_item,
                            "HARQ Combine Enable", iHarqCombineEnable);
                    Py_DECREF(old_object);


                    for(int k=0;k<iNumCodeBlockPlus;k++){
                        offset+=8;
                    }

                    int count_temp=0;
                    while ((unsigned int)(offset - start) < length) {
                        PyObject *result_hidden_record = PyList_New(0);
                        offset += _decode_by_fmt(LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v143,
                                ARRAY_SIZE(LtePhyPdschDecodingResult_Hidden_Energy_Metrics_v143, Fmt),
                                b, offset, length, result_hidden_record);

                        int first_half_temp = _search_result_int(result_hidden_record,
                                "Hidden Energy Metric First Half");
                        int second_half_temp=_search_result_int(result_hidden_record,
                                "Hidden Energy Metric Second Half");
                        if(first_half_temp!=0 or second_half_temp!=0){
                            break;
                        }
                        count_temp=count_temp+1;
                        Py_DECREF(result_hidden_record);
                    }

                    offset=offset-8;

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
        printf("(MI)Unknown LTE PHY PDSCH Decoding Result version: 0x%x\n", pkt_ver);
        return 0;
    }
}
