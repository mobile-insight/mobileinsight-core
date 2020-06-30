/*
 * LTE_PUCCH_Tx_Report
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"
#include <bitset>

// #define SSTRHEX( x ) static_cast< std::ostringstream & >( \
//         ( std::ostringstream() << std::hex << x ) ).str()

std::string SSTRHEX(int x){
    std::ostringstream stream;
    stream << std::hex << x;
    std::string result( stream.str() );
    return result;
}

const Fmt LtePhyPucchTxReport_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePhyPucchTxReport_Payload_v21 [] = {
    {UINT, "Serving Cell ID", 2},   // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p1_v21 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "CQI Payload", 2},
    {UINT, "Format", 4},    // 3 bits, LtePucchPowerControl_Record_v4_PUCCH_Format
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "SRS Shorting for 2nd Slot", 0},  // 1 bit
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {PLACEHOLDER, "DMRS Seq Slot 0", 0},    // 5 bits
    {PLACEHOLDER, "DMRS Seq Slot 1", 0},    // 5 bits
    {PLACEHOLDER, "ACK Payload", 0},    // 2 bits
};

const Fmt LtePhyPucchTxReport_Record_CyclicShift_v21 [] = {
    {UINT, "Cyclic Shift Seq per Symbol", 1},
};

const Fmt LtePhyPucchTxReport_Record_p2_v21 [] = {
    {UINT, "PUCCH Digital Gain (dB)", 1},
    {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits
    {UINT, "Tx Resampler", 4},  // X / 4294967295
    {UINT, "n_1_pucch", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p2_v22 [] = {
    {UINT, "PUCCH Digital Gain (dB)", 1},
    {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits
    {UINT, "Tx Resampler", 4},  // X / 4294967295
    {UINT, "n_1_pucch", 2},
    {SKIP, NULL, 2},

    //added different from v21
    {UINT, "Num Repetition",4},
    {PLACEHOLDER, "RB NB Start Index",0},
};

const Fmt LtePhyPucchTxReport_Payload_v43 [] = {
    {UINT, "Serving Cell ID", 2},   // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p1_v43 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "CQI Payload", 2},
    {UINT, "Format", 4},    // 3 bits, LtePucchPowerControl_Record_v4_PUCCH_Format
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "SRS Shorting for 2nd Slot", 0},  // 1 bit
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {PLACEHOLDER, "DMRS Seq Slot 0", 0},    // 5 bits
    {PLACEHOLDER, "DMRS Seq Slot 1", 0},    // 5 bits
};

const Fmt LtePhyPucchTxReport_Record_CyclicShift_v43 [] = {
    {UINT, "Cyclic Shift Seq per Symbol", 1},
};

const Fmt LtePhyPucchTxReport_Record_p2_v43 [] = {
    {UINT, "PUCCH Digital Gain (dB)", 1},
    {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits
    {UINT, "ACK Payload", 4},   // 20 bits
    {PLACEHOLDER, "ACK Payload Length", 0}, // 5 bits
    {PLACEHOLDER, "SR Bit FMT3", 0},    // 1 bit
    {PLACEHOLDER, "Num DL Carriers", 0},    // 4 bits
    {UINT, "n_1_pucch", 2},
    {UINT, "Ack Nak Index", 2}, // 12 bits
    {PLACEHOLDER, "Ack Nak Late", 0},   // 1 bit
    {PLACEHOLDER, "Csf Late", 0},   // 1 bit
    {PLACEHOLDER, "Dropped Pucch", 0},  // 1 bit
    {UINT, "Tx Resampler", 4},  // X / 4294967295
};

const Fmt LtePhyPucchTxReport_Payload_v101 [] = {
    {UINT, "Serving Cell ID", 2},   // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p1_v101 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "CQI Payload", 2},
    {UINT, "Carrier Index", 4}, // 2 bits
    {PLACEHOLDER, "Format", 0}, // 3 bits, LtePucchPowerControl_Record_v4_PUCCH_Format
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "SRS Shorting for 2nd Slot", 0},  // 1 bit
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {PLACEHOLDER, "DMRS Seq Slot 0", 0},    // 5 bits
    {PLACEHOLDER, "DMRS Seq Slot 1", 0},    // 5 bits
};

const Fmt LtePhyPucchTxReport_Record_CyclicShift_v101 [] = {
    {UINT, "Cyclic Shift Seq per Symbol", 1},
};

const Fmt LtePhyPucchTxReport_Record_p2_v101 [] = {
    {UINT, "PUCCH Digital Gain (dB)", 1},
    {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits
    {UINT, "ACK Payload", 4},   // 20 bits
    {PLACEHOLDER, "ACK Payload Length", 0}, // 5 bits
    {PLACEHOLDER, "SR Bit FMT3", 0},    // 1 bit
    {PLACEHOLDER, "Num DL Carriers", 0},    // 4 bits
    {UINT, "n_1_pucch", 2},
    {UINT, "Ack Nak Index", 2}, // 12 bits
    {PLACEHOLDER, "Ack Nak Late", 0},   // 1 bit
    {PLACEHOLDER, "Csf Late", 0},   // 1 bit
    {PLACEHOLDER, "Dropped Pucch", 0},  // 1 bit
    {UINT, "Tx Resampler", 4},  // X / 4294967295
};

const Fmt LtePhyPucchTxReport_Payload_v121 [] = {
    {UINT, "Serving Cell ID", 2},   // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p1_v121 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "CQI Payload", 2},
    {UINT, "Carrier Index", 4}, // 2 bits
    {PLACEHOLDER, "Format", 0}, // 3 bits, LtePucchPowerControl_Record_v4_PUCCH_Format
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "SRS Shorting for 2nd Slot", 0},  // 1 bit
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {PLACEHOLDER, "DMRS Seq Slot 0", 0},    // 5 bits
    {PLACEHOLDER, "DMRS Seq Slot 1", 0},    // 5 bits
};

const Fmt LtePhyPucchTxReport_Record_CyclicShift_v121 [] = {
    {UINT, "Cyclic Shift Seq per Symbol", 1},
};

const Fmt LtePhyPucchTxReport_Record_p2_v121 [] = {
//add DL Carrier Index
    {UINT, "PUCCH Digital Gain (dB)", 1},
    {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits
    {UINT, "ACK Payload", 4},   // 20 bits
    {PLACEHOLDER, "ACK Payload Length", 0}, // 5 bits
    {PLACEHOLDER, "SR Bit FMT3", 0},    // 1 bit
    {PLACEHOLDER, "Num DL Carriers", 0},    // 4 bits
    {PLACEHOLDER, "DL Carrier Index", 0},    // 4 bits
    {UINT, "n_1_pucch", 2},
    {UINT, "Ack Nak Index", 2}, // 12 bits
    {PLACEHOLDER, "Ack Nak Late", 0},   // 1 bit
    {PLACEHOLDER, "Csf Late", 0},   // 1 bit
    {PLACEHOLDER, "Dropped Pucch", 0},  // 1 bit
    {UINT, "Tx Resampler", 4},  // X / 4294967295
};

const Fmt LtePhyPucchTxReport_Payload_v142 [] = {
    {UINT, "Serving Cell ID", 2},   // 9 bits
    {PLACEHOLDER, "Number of Records", 0},  // 5 bits
    {SKIP, NULL, 1},
    {UINT, "Dispatch SFN SF", 2},
    {SKIP, NULL, 2},
};

const Fmt LtePhyPucchTxReport_Record_p1_v142 [] = {
    {UINT, "Current SFN SF", 2},
    {UINT, "CQI Payload", 2},
    {UINT, "Carrier Index", 4}, // 2 bits
    {PLACEHOLDER, "Format", 0}, // 3 bits, LtePucchPowerControl_Record_v4_PUCCH_Format
    {PLACEHOLDER, "Start RB Slot 0", 0},    // 7 bits
    {PLACEHOLDER, "Start RB Slot 1", 0},    // 7 bits
    {PLACEHOLDER, "SRS Shorting for 2nd Slot", 0},  // 1 bit
    {PLACEHOLDER, "UE SRS", 0}, // 1 bit
    {PLACEHOLDER, "DMRS Seq Slot 0", 0},    // 5 bits
    {PLACEHOLDER, "DMRS Seq Slot 1", 0},    // 5 bits
};

const Fmt LtePhyPucchTxReport_Record_CyclicShift_v142 [] = {
    {UINT, "Cyclic Shift Seq per Symbol", 1},
};

const Fmt LtePhyPucchTxReport_Record_p2_v142 [] = {
//add DL Carrier Index
    {UINT, "PUCCH Digital Gain (dB)", 1},
    {UINT, "PUCCH Tx Power (dBm)", 1},  // 6 bits

    {BYTE_STREAM, "ACK Payload[0]",4},
    {BYTE_STREAM, "ACK Payload[1]",4},
    {BYTE_STREAM, "ACK Payload[2]",4},
    {BYTE_STREAM, "ACK Payload[3]",4},

    {UINT, "ACK Payload Length", 1}, // 5 bits
    {PLACEHOLDER, "SR Bit FMT3", 0},    // 1 bit

    {UINT, "Num DL Carriers", 2},    // 4 bits
    {PLACEHOLDER, "DL Carrier Index", 0},    // 4 bits
    {PLACEHOLDER, "Num RB",0},
    {PLACEHOLDER, "CDM Index",0},
    {SKIP,NULL,1},

    {UINT, "n_1_pucch", 2},
    {UINT, "Ack Nak Index", 2}, // 12 bits
    {PLACEHOLDER, "Ack Nak Late", 0},   // 1 bit
    {PLACEHOLDER, "Csf Late", 0},   // 1 bit
    {PLACEHOLDER, "Dropped Pucch", 0},  // 1 bit
    {UINT, "Tx Resampler", 4},  // X / 4294967295
};

static int _decode_lte_phy_pucch_tx_report_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;
    PyObject *pydouble;
    PyObject *pystr;
    int temp;
    unsigned int u_temp;

    switch (pkt_ver) {
    case 21:
        {
            offset += _decode_by_fmt(LtePhyPucchTxReport_Payload_v21,
                    ARRAY_SIZE(LtePhyPucchTxReport_Payload_v21, Fmt),
                    b, offset, length, result);

            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p1_v21,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p1_v21, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item, "CQI Payload");
                std::string strCQIPayload =
                    (std::bitset<16>(temp)).to_string();
                pystr = Py_BuildValue("s", strCQIPayload.c_str());
                old_object = _replace_result(result_record_item, "CQI Payload",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);

                u_temp = _search_result_uint(result_record_item, "Format");
                int iFormat = u_temp & 7;   // 3 bits
                int iStartRBSlot0 = (u_temp >> 3) & 127;    // 7 bits
                int iStartRBSlot1 = (u_temp >> 10) & 127;   // 7 bits
                int iSRSShortingfor2ndSlot = (u_temp >> 17) & 1;    // 1 bit
                int iUESRS = (u_temp >> 18) & 1;    // 1 bit
                int iDMRSSeqSlot0 = (u_temp >> 19) & 31;    // 5 bits
                int iDMRSSeqSlot1 = (u_temp >> 24) & 31;    // 5 bits
                int iACKPayload = (u_temp >> 29) & 3;   // 2 bits

                old_object = _replace_result_int(result_record_item, "Format",
                        iFormat);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "Format",
                        LtePucchPowerControl_Record_v4_PUCCH_Format,
                        ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "SRS Shorting for 2nd Slot", iSRSShortingfor2ndSlot);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "SRS Shorting for 2nd Slot",
                        ValueNameSRSShortingfor2ndSlot,
                        ARRAY_SIZE(ValueNameSRSShortingfor2ndSlot,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 0", iDMRSSeqSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 1", iDMRSSeqSlot1);
                Py_DECREF(old_object);
                std::string strAckPayload =
                    (std::bitset<2>(iACKPayload)).to_string();
                pystr = Py_BuildValue("s", strAckPayload.c_str());
                old_object = _replace_result(result_record_item, "ACK Payload",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);

                PyObject *result_record_css = PyList_New(0);
                for (int j = 0; j < 14; j++) {
                    PyObject *result_record_css_item = PyList_New(0);
                    offset += _decode_by_fmt(
                            LtePhyPucchTxReport_Record_CyclicShift_v21,
                            ARRAY_SIZE(
                                LtePhyPucchTxReport_Record_CyclicShift_v21,
                                Fmt),
                            b, offset, length, result_record_css_item);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_css_item, "dict");
                    PyList_Append(result_record_css, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_css_item);
                }

                PyObject *t2 = Py_BuildValue("(sOs)", "Cyclic Shift Seq",
                        result_record_css, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_css);

                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p2_v21,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p2_v21, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)");
                int iPucchTxPower = temp & 63;  // 6 bits
                old_object = _replace_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)", iPucchTxPower);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Tx Resampler");
                double fTxResampler = u_temp / 4294967295.0;
                pydouble = Py_BuildValue("d", fTxResampler);
                old_object = _replace_result(result_record_item,
                        "Tx Resampler", pydouble);
                Py_DECREF(old_object);
                Py_DECREF(pydouble);

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
    case 22:
        {
            offset += _decode_by_fmt(LtePhyPucchTxReport_Payload_v21,
                    ARRAY_SIZE(LtePhyPucchTxReport_Payload_v21, Fmt),
                    b, offset, length, result);

            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p1_v21,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p1_v21, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item, "CQI Payload");
                std::string strCQIPayload =
                    (std::bitset<16>(temp)).to_string();
                pystr = Py_BuildValue("s", strCQIPayload.c_str());
                old_object = _replace_result(result_record_item, "CQI Payload",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);

                u_temp = _search_result_uint(result_record_item, "Format");
                int iFormat = u_temp & 7;   // 3 bits
                int iStartRBSlot0 = (u_temp >> 3) & 127;    // 7 bits
                int iStartRBSlot1 = (u_temp >> 10) & 127;   // 7 bits
                int iSRSShortingfor2ndSlot = (u_temp >> 17) & 1;    // 1 bit
                int iUESRS = (u_temp >> 18) & 1;    // 1 bit
                int iDMRSSeqSlot0 = (u_temp >> 19) & 31;    // 5 bits
                int iDMRSSeqSlot1 = (u_temp >> 24) & 31;    // 5 bits
                int iACKPayload = (u_temp >> 29) & 3;   // 2 bits

                old_object = _replace_result_int(result_record_item, "Format",
                        iFormat);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "Format",
                        LtePucchPowerControl_Record_v4_PUCCH_Format,
                        ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "SRS Shorting for 2nd Slot", iSRSShortingfor2ndSlot);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "SRS Shorting for 2nd Slot",
                        ValueNameSRSShortingfor2ndSlot,
                        ARRAY_SIZE(ValueNameSRSShortingfor2ndSlot,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 0", iDMRSSeqSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 1", iDMRSSeqSlot1);
                Py_DECREF(old_object);
                std::string strAckPayload =
                    (std::bitset<2>(iACKPayload)).to_string();
                pystr = Py_BuildValue("s", strAckPayload.c_str());
                old_object = _replace_result(result_record_item, "ACK Payload",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);

                PyObject *result_record_css = PyList_New(0);
                for (int j = 0; j < 14; j++) {
                    PyObject *result_record_css_item = PyList_New(0);
                    offset += _decode_by_fmt(
                            LtePhyPucchTxReport_Record_CyclicShift_v21,
                            ARRAY_SIZE(
                                LtePhyPucchTxReport_Record_CyclicShift_v21,
                                Fmt),
                            b, offset, length, result_record_css_item);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_css_item, "dict");
                    PyList_Append(result_record_css, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_css_item);
                }

                PyObject *t2 = Py_BuildValue("(sOs)", "Cyclic Shift Seq",
                        result_record_css, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_css);

                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p2_v22,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p2_v22, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)");
                int iPucchTxPower = temp & 63;  // 6 bits
                old_object = _replace_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)", iPucchTxPower);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Tx Resampler");
                double fTxResampler = u_temp / 4294967295.0;
                pydouble = Py_BuildValue("d", fTxResampler);
                old_object = _replace_result(result_record_item,
                        "Tx Resampler", pydouble);
                Py_DECREF(old_object);
                Py_DECREF(pydouble);

                u_temp = _search_result_uint(result_record_item,
                        "Num Repetition");
                int iNumRepetition = u_temp & 0xfff;
                int iRBNBStartIndex = (u_temp>>12) & 0xff;

                old_object = _replace_result_int(result_record_item,
                        "Num Repetition", iNumRepetition);
                Py_DECREF(old_object);

                old_object = _replace_result_int(result_record_item,
                        "RB NB Start Index", iRBNBStartIndex);
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
    case 43:
        {
            offset += _decode_by_fmt(LtePhyPucchTxReport_Payload_v43,
                    ARRAY_SIZE(LtePhyPucchTxReport_Payload_v43, Fmt),
                    b, offset, length, result);

            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p1_v43,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p1_v43, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item, "CQI Payload");
                std::string strCQIPayload =
                    (std::bitset<16>(temp)).to_string();
                pystr = Py_BuildValue("s", strCQIPayload.c_str());
                old_object = _replace_result(result_record_item, "CQI Payload",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);

                u_temp = _search_result_uint(result_record_item, "Format");
                int iFormat = u_temp & 7;   // 3 bits
                int iStartRBSlot0 = (u_temp >> 3) & 127;    // 7 bits
                int iStartRBSlot1 = (u_temp >> 10) & 127;   // 7 bits
                int iSRSShortingfor2ndSlot = (u_temp >> 17) & 1;    // 1 bit
                int iUESRS = (u_temp >> 18) & 1;    // 1 bit
                int iDMRSSeqSlot0 = (u_temp >> 19) & 31;    // 5 bits
                int iDMRSSeqSlot1 = (u_temp >> 24) & 31;    // 5 bits

                old_object = _replace_result_int(result_record_item, "Format",
                        iFormat);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "Format",
                        LtePucchPowerControl_Record_v4_PUCCH_Format,
                        ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "SRS Shorting for 2nd Slot", iSRSShortingfor2ndSlot);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "SRS Shorting for 2nd Slot",
                        ValueNameSRSShortingfor2ndSlot,
                        ARRAY_SIZE(ValueNameSRSShortingfor2ndSlot,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 0", iDMRSSeqSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 1", iDMRSSeqSlot1);
                Py_DECREF(old_object);

                PyObject *result_record_css = PyList_New(0);
                for (int j = 0; j < 14; j++) {
                    PyObject *result_record_css_item = PyList_New(0);
                    offset += _decode_by_fmt(
                            LtePhyPucchTxReport_Record_CyclicShift_v43,
                            ARRAY_SIZE(
                                LtePhyPucchTxReport_Record_CyclicShift_v43,
                                Fmt),
                            b, offset, length, result_record_css_item);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_css_item, "dict");
                    PyList_Append(result_record_css, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_css_item);
                }

                PyObject *t2 = Py_BuildValue("(sOs)", "Cyclic Shift Seq",
                        result_record_css, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_css);

                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p2_v43,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p2_v43, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)");
                int iPucchTxPower = temp & 63;  // 6 bits
                old_object = _replace_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)", iPucchTxPower);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "ACK Payload");
                int iAckPayload = u_temp & 1048575; // 20 bits
                int iAckPayloadLength = (u_temp >> 20) & 31;    // 5 bits
                int iSRBitFMT3 = (u_temp >> 25) & 1;    // 1 bit
                int iNumDLCarriers = (u_temp >> 26) & 15;   // 4 bits
                std::string strAckPayload =
                    (std::bitset<20>(iAckPayload)).to_string();
                pystr = Py_BuildValue("s", strAckPayload.c_str());
                old_object = _replace_result(result_record_item, "ACK Payload",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                old_object = _replace_result_int(result_record_item,
                        "ACK Payload Length", iAckPayloadLength);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "SR Bit FMT3", iSRBitFMT3);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num DL Carriers", iNumDLCarriers);
                Py_DECREF(old_object);

                temp = _search_result_int(result_record_item,
                        "Ack Nak Index");
                int iAckNakIndex = temp & 4095; // 12 bits
                int iAckNakLate = (temp >> 12) & 1; // 1 bit
                int iCsfLate = (temp >> 13) & 1;    // 1 bit
                int iDroppedPucch = (temp >> 14) & 1;   // 1 bit
                std::string strAckNakIndex = "0x";
                std::string hexAckNakIndex = SSTRHEX(iAckNakIndex);
                for (unsigned int idx = 0; idx < 4 - hexAckNakIndex.length(); idx++) {
                    strAckNakIndex += "0";
                }
                strAckNakIndex += hexAckNakIndex;
                pystr = Py_BuildValue("s", strAckNakIndex.c_str());
                old_object = _replace_result(result_record_item,
                        "Ack Nak Index", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nak Late", iAckNakLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Csf Late", iCsfLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Dropped Pucch", iDroppedPucch);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Tx Resampler");
                double fTxResampler = u_temp / 4294967295.0;
                pydouble = Py_BuildValue("d", fTxResampler);
                old_object = _replace_result(result_record_item,
                        "Tx Resampler", pydouble);
                Py_DECREF(old_object);
                Py_DECREF(pydouble);

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
    case 101:
        {
            offset += _decode_by_fmt(LtePhyPucchTxReport_Payload_v101,
                    ARRAY_SIZE(LtePhyPucchTxReport_Payload_v101, Fmt),
                    b, offset, length, result);

            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p1_v101,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p1_v101, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item, "CQI Payload");
                std::string strCQIPayload =
                    (std::bitset<16>(temp)).to_string();
                pystr = Py_BuildValue("s", strCQIPayload.c_str());
                old_object = _replace_result(result_record_item, "CQI Payload",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);

                u_temp = _search_result_uint(result_record_item, "Carrier Index");
                int iCarrierIndex = u_temp & 3; // 2 bits
                int iFormat = (u_temp >> 2) & 7;   // 3 bits
                int iStartRBSlot0 = (u_temp >> 5) & 127;    // 7 bits
                int iStartRBSlot1 = (u_temp >> 12) & 127;   // 7 bits
                int iSRSShortingfor2ndSlot = (u_temp >> 19) & 1;    // 1 bit
                int iUESRS = (u_temp >> 20) & 1;    // 1 bit
                int iDMRSSeqSlot0 = (u_temp >> 21) & 31;    // 5 bits
                int iDMRSSeqSlot1 = (u_temp >> 26) & 31;    // 5 bits

                old_object = _replace_result_int(result_record_item,
                        "Carrier Index", iCarrierIndex);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "Carrier Index",
                        ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "Format",
                        iFormat);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "Format",
                        LtePucchPowerControl_Record_v4_PUCCH_Format,
                        ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "SRS Shorting for 2nd Slot", iSRSShortingfor2ndSlot);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "SRS Shorting for 2nd Slot",
                        ValueNameSRSShortingfor2ndSlot,
                        ARRAY_SIZE(ValueNameSRSShortingfor2ndSlot,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 0", iDMRSSeqSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 1", iDMRSSeqSlot1);
                Py_DECREF(old_object);

                PyObject *result_record_css = PyList_New(0);
                for (int j = 0; j < 14; j++) {
                    PyObject *result_record_css_item = PyList_New(0);
                    offset += _decode_by_fmt(
                            LtePhyPucchTxReport_Record_CyclicShift_v101,
                            ARRAY_SIZE(
                                LtePhyPucchTxReport_Record_CyclicShift_v101,
                                Fmt),
                            b, offset, length, result_record_css_item);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_css_item, "dict");
                    PyList_Append(result_record_css, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_css_item);
                }

                PyObject *t2 = Py_BuildValue("(sOs)", "Cyclic Shift Seq",
                        result_record_css, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_css);

                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p2_v101,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p2_v101, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)");
                int iPucchTxPower = temp & 63;  // 6 bits
                old_object = _replace_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)", iPucchTxPower);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "ACK Payload");
                int iAckPayload = u_temp & 1048575; // 20 bits
                int iAckPayloadLength = (u_temp >> 20) & 31;    // 5 bits
                int iSRBitFMT3 = (u_temp >> 25) & 1;    // 1 bit
                int iNumDLCarriers = (u_temp >> 26) & 15;   // 4 bits
                std::string strAckPayload =
                    (std::bitset<20>(iAckPayload)).to_string();
                pystr = Py_BuildValue("s", strAckPayload.c_str());
                old_object = _replace_result(result_record_item, "ACK Payload",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                old_object = _replace_result_int(result_record_item,
                        "ACK Payload Length", iAckPayloadLength);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "SR Bit FMT3", iSRBitFMT3);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num DL Carriers", iNumDLCarriers);
                Py_DECREF(old_object);

                temp = _search_result_int(result_record_item,
                        "Ack Nak Index");
                int iAckNakIndex = temp & 4095; // 12 bits
                int iAckNakLate = (temp >> 12) & 1; // 1 bit
                int iCsfLate = (temp >> 13) & 1;    // 1 bit
                int iDroppedPucch = (temp >> 14) & 1;   // 1 bit
                std::string strAckNakIndex = "0x";
                std::string hexAckNakIndex = SSTRHEX(iAckNakIndex);
                for (unsigned int idx = 0; idx < 4 - hexAckNakIndex.length(); idx++) {
                    strAckNakIndex += "0";
                }
                strAckNakIndex += hexAckNakIndex;
                pystr = Py_BuildValue("s", strAckNakIndex.c_str());
                old_object = _replace_result(result_record_item,
                        "Ack Nak Index", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nak Late", iAckNakLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Csf Late", iCsfLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Dropped Pucch", iDroppedPucch);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Tx Resampler");
                double fTxResampler = u_temp / 4294967295.0;
                pydouble = Py_BuildValue("d", fTxResampler);
                old_object = _replace_result(result_record_item,
                        "Tx Resampler", pydouble);
                Py_DECREF(old_object);
                Py_DECREF(pydouble);

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
    case 121:
        {
            offset += _decode_by_fmt(LtePhyPucchTxReport_Payload_v121,
                    ARRAY_SIZE(LtePhyPucchTxReport_Payload_v121, Fmt),
                    b, offset, length, result);

            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p1_v121,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p1_v121, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item, "CQI Payload");
                std::string strCQIPayload =
                    (std::bitset<16>(temp)).to_string();
                pystr = Py_BuildValue("s", strCQIPayload.c_str());
                old_object = _replace_result(result_record_item, "CQI Payload",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);

                u_temp = _search_result_uint(result_record_item, "Carrier Index");
                int iCarrierIndex = u_temp & 3; // 2 bits
                int iFormat = (u_temp >> 2) & 7;   // 3 bits
                int iStartRBSlot0 = (u_temp >> 5) & 127;    // 7 bits
                int iStartRBSlot1 = (u_temp >> 12) & 127;   // 7 bits
                int iSRSShortingfor2ndSlot = (u_temp >> 19) & 1;    // 1 bit
                int iUESRS = (u_temp >> 20) & 1;    // 1 bit
                int iDMRSSeqSlot0 = (u_temp >> 21) & 31;    // 5 bits
                int iDMRSSeqSlot1 = (u_temp >> 26) & 31;    // 5 bits

                old_object = _replace_result_int(result_record_item,
                        "Carrier Index", iCarrierIndex);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "Carrier Index",
                        ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "Format",
                        iFormat);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "Format",
                        LtePucchPowerControl_Record_v4_PUCCH_Format,
                        ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "SRS Shorting for 2nd Slot", iSRSShortingfor2ndSlot);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "SRS Shorting for 2nd Slot",
                        ValueNameSRSShortingfor2ndSlot,
                        ARRAY_SIZE(ValueNameSRSShortingfor2ndSlot,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 0", iDMRSSeqSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 1", iDMRSSeqSlot1);
                Py_DECREF(old_object);

                PyObject *result_record_css = PyList_New(0);
                for (int j = 0; j < 14; j++) {
                    PyObject *result_record_css_item = PyList_New(0);
                    offset += _decode_by_fmt(
                            LtePhyPucchTxReport_Record_CyclicShift_v121,
                            ARRAY_SIZE(
                                LtePhyPucchTxReport_Record_CyclicShift_v121,
                                Fmt),
                            b, offset, length, result_record_css_item);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_css_item, "dict");
                    PyList_Append(result_record_css, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_css_item);
                }

                PyObject *t2 = Py_BuildValue("(sOs)", "Cyclic Shift Seq",
                        result_record_css, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_css);

                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p2_v121,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p2_v121, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)");
                int iPucchTxPower = temp & 63;  // 6 bits
                old_object = _replace_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)", iPucchTxPower);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "ACK Payload");
                int iAckPayload = u_temp & 1048575; // 20 bits
                int iAckPayloadLength = (u_temp >> 20) & 31;    // 5 bits
                int iSRBitFMT3 = (u_temp >> 25) & 1;    // 1 bit
                int iNumDLCarriers = (u_temp >> 26) & 15;   // 4 bits
                int iDLCarrierIndex = (u_temp>>30) &1; //1 bit
                std::string strAckPayload =
                    (std::bitset<20>(iAckPayload)).to_string();
                pystr = Py_BuildValue("s", strAckPayload.c_str());
                old_object = _replace_result(result_record_item, "ACK Payload",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                old_object = _replace_result_int(result_record_item,
                        "ACK Payload Length", iAckPayloadLength);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "SR Bit FMT3", iSRBitFMT3);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num DL Carriers", iNumDLCarriers);
                Py_DECREF(old_object);

                old_object = _replace_result_int(result_record_item,
                        "DL Carrier Index", iDLCarrierIndex);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "DL Carrier Index",
                        ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");

                temp = _search_result_int(result_record_item,
                        "Ack Nak Index");
                int iAckNakIndex = temp & 4095; // 12 bits
                int iAckNakLate = (temp >> 12) & 1; // 1 bit
                int iCsfLate = (temp >> 13) & 1;    // 1 bit
                int iDroppedPucch = (temp >> 14) & 1;   // 1 bit
                std::string strAckNakIndex = "0x";
                std::string hexAckNakIndex = SSTRHEX(iAckNakIndex);
                for (unsigned int idx = 0; idx < 4 - hexAckNakIndex.length(); idx++) {
                    strAckNakIndex += "0";
                }
                strAckNakIndex += hexAckNakIndex;
                pystr = Py_BuildValue("s", strAckNakIndex.c_str());
                old_object = _replace_result(result_record_item,
                        "Ack Nak Index", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nak Late", iAckNakLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Csf Late", iCsfLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Dropped Pucch", iDroppedPucch);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Tx Resampler");
                double fTxResampler = u_temp / 4294967295.0;
                pydouble = Py_BuildValue("d", fTxResampler);
                old_object = _replace_result(result_record_item,
                        "Tx Resampler", pydouble);
                Py_DECREF(old_object);
                Py_DECREF(pydouble);

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
    case 142:
        {
            offset += _decode_by_fmt(LtePhyPucchTxReport_Payload_v142,
                    ARRAY_SIZE(LtePhyPucchTxReport_Payload_v142, Fmt),
                    b, offset, length, result);

            temp = _search_result_int(result, "Serving Cell ID");
            int iServingCellId = temp & 511;    // 9 bits
            int num_record = (temp >> 9) & 31;  // 5 bits
            old_object = _replace_result_int(result, "Serving Cell ID",
                    iServingCellId);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p1_v142,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p1_v142, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item, "CQI Payload");
                std::string strCQIPayload =
                    (std::bitset<16>(temp)).to_string();
                pystr = Py_BuildValue("s", strCQIPayload.c_str());
                old_object = _replace_result(result_record_item, "CQI Payload",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);

                u_temp = _search_result_uint(result_record_item, "Carrier Index");
                int iCarrierIndex = u_temp & 3; // 2 bits
                int iFormat = (u_temp >> 2) & 7;   // 3 bits
                int iStartRBSlot0 = (u_temp >> 5) & 127;    // 7 bits
                int iStartRBSlot1 = (u_temp >> 12) & 127;   // 7 bits
                int iSRSShortingfor2ndSlot = (u_temp >> 19) & 1;    // 1 bit
                int iUESRS = (u_temp >> 20) & 1;    // 1 bit
                int iDMRSSeqSlot0 = (u_temp >> 21) & 31;    // 5 bits
                int iDMRSSeqSlot1 = (u_temp >> 26) & 31;    // 5 bits

                old_object = _replace_result_int(result_record_item,
                        "Carrier Index", iCarrierIndex);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "Carrier Index",
                        ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item, "Format",
                        iFormat);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "Format",
                        LtePucchPowerControl_Record_v4_PUCCH_Format,
                        ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 0", iStartRBSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Start RB Slot 1", iStartRBSlot1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "SRS Shorting for 2nd Slot", iSRSShortingfor2ndSlot);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "SRS Shorting for 2nd Slot",
                        ValueNameSRSShortingfor2ndSlot,
                        ARRAY_SIZE(ValueNameSRSShortingfor2ndSlot,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "UE SRS", iUESRS);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item, "UE SRS",
                        ValueNameOnOrOff,
                        ARRAY_SIZE(ValueNameOnOrOff, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 0", iDMRSSeqSlot0);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DMRS Seq Slot 1", iDMRSSeqSlot1);
                Py_DECREF(old_object);

                PyObject *result_record_css = PyList_New(0);
                for (int j = 0; j < 14; j++) {
                    PyObject *result_record_css_item = PyList_New(0);
                    offset += _decode_by_fmt(
                            LtePhyPucchTxReport_Record_CyclicShift_v142,
                            ARRAY_SIZE(
                                LtePhyPucchTxReport_Record_CyclicShift_v142,
                                Fmt),
                            b, offset, length, result_record_css_item);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_css_item, "dict");
                    PyList_Append(result_record_css, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_css_item);
                }

                PyObject *t2 = Py_BuildValue("(sOs)", "Cyclic Shift Seq",
                        result_record_css, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_css);

                offset += _decode_by_fmt(LtePhyPucchTxReport_Record_p2_v142,
                        ARRAY_SIZE(LtePhyPucchTxReport_Record_p2_v142, Fmt),
                        b, offset, length, result_record_item);

                temp = _search_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)");
                int iPucchTxPower = temp & 63;  // 6 bits
                old_object = _replace_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)", iPucchTxPower);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "ACK Payload Length");
                int iAckPayloadLength = u_temp & 0x7f;
                int iSRBitFMT3 = (u_temp>>7) & 0x1;
                old_object = _replace_result_int(result_record_item,
                        "ACK Payload Length", iAckPayloadLength);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "SR Bit FMT3", iSRBitFMT3);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item, "Num DL Carriers");
                int iNumDLCarriers = u_temp & 0x7;
                int iDLCarrierIndex = (u_temp>>3) & 0x7;
                int iNumRB = (u_temp>>6) & 0x7f;
                int iCDMIndex = (u_temp>>13) & 0x1;

                old_object = _replace_result_int(result_record_item,
                        "Num DL Carriers", iNumDLCarriers);
                Py_DECREF(old_object);

                old_object = _replace_result_int(result_record_item,
                        "DL Carrier Index", iDLCarrierIndex);
                Py_DECREF(old_object);
                (void)_map_result_field_to_name(result_record_item,
                        "DL Carrier Index",
                        ValueNameCarrierIndex,
                        ARRAY_SIZE(ValueNameCarrierIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Num RB", iNumRB);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "CDM Index", iCDMIndex);
                Py_DECREF(old_object);

                temp = _search_result_int(result_record_item,
                        "Ack Nak Index");
                int iAckNakIndex = temp & 4095; // 12 bits
                int iAckNakLate = (temp >> 12) & 1; // 1 bit
                int iCsfLate = (temp >> 13) & 1;    // 1 bit
                int iDroppedPucch = (temp >> 14) & 1;   // 1 bit
                std::string strAckNakIndex = "0x";
                std::string hexAckNakIndex = SSTRHEX(iAckNakIndex);
                for (unsigned int idx = 0; idx < 4 - hexAckNakIndex.length(); idx++) {
                    strAckNakIndex += "0";
                }
                strAckNakIndex += hexAckNakIndex;
                pystr = Py_BuildValue("s", strAckNakIndex.c_str());
                old_object = _replace_result(result_record_item,
                        "Ack Nak Index", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
                old_object = _replace_result_int(result_record_item,
                        "Ack Nak Late", iAckNakLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Csf Late", iCsfLate);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Dropped Pucch", iDroppedPucch);
                Py_DECREF(old_object);

                u_temp = _search_result_uint(result_record_item,
                        "Tx Resampler");
                int fTxResampler_origin = u_temp;
                double fTxResampler = fTxResampler_origin / 4294967295.0;
                pydouble = Py_BuildValue("d", fTxResampler);
                old_object = _replace_result(result_record_item,
                        "Tx Resampler", pydouble);
                Py_DECREF(old_object);
                Py_DECREF(pydouble);

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
        printf("(MI)Unknown LTE PHY PUCCH Tx Report version: %d\n", pkt_ver);
        return 0;
    }
}