/*
 * LTE PDSCH Stat Indication
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePdschStatIndication_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePdschStatIndication_Payload_v36 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdschStatIndication_Payload_v32 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdschStatIndication_Payload_v24 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdschStatIndication_Payload_v16 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdschStatIndication_Payload_v5 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdschStatIndication_Record_v36_P1 [] = {
    {UINT, "Subframe Num", 2},
    {PLACEHOLDER, "Frame Num", 0},
    {UINT, "Num RBs", 1},
    {UINT, "Num Layers", 1},
    {UINT, "Num Transport Blocks Present", 1},
    {UINT, "Serving Cell Index", 1},
    {PLACEHOLDER, "HSIC Enabled", 0},
    {SKIP, NULL, 6},//modified
};

const Fmt LtePdschStatIndication_Record_v32_P1 [] = {
    {UINT, "Subframe Num", 2},
    {PLACEHOLDER, "Frame Num", 0},
    {UINT, "Num RBs", 1},
    {UINT, "Num Layers", 1},
    {UINT, "Num Transport Blocks Present", 1},
    {UINT, "Serving Cell Index", 1},
    {PLACEHOLDER, "HSIC Enabled", 0},
};

const Fmt LtePdschStatIndication_Record_v24_P1 [] = {
    {UINT, "Subframe Num", 2},
    {PLACEHOLDER, "Frame Num", 0},
    {UINT, "Num RBs", 1},
    {UINT, "Num Layers", 1},
    {UINT, "Num Transport Blocks Present", 1},
    {UINT, "Serving Cell Index", 1},
    {PLACEHOLDER, "HSIC Enabled", 0},
};

const Fmt LtePdschStatIndication_Record_v16_P1 [] = {
    {UINT, "Subframe Num", 2},
    {PLACEHOLDER, "Frame Num", 0},
    {UINT, "Num RBs", 1},
    {UINT, "Num Layers", 1},
    {UINT, "Num Transport Blocks Present", 1},
    {UINT, "Serving Cell Index", 1},
    {PLACEHOLDER, "HSIC Enabled", 0},
};

const Fmt LtePdschStatIndication_Record_v5_P1 [] = {
    {UINT, "Subframe Num", 2},
    {PLACEHOLDER, "Frame Num", 0},
    {UINT, "Num RBs", 1},
    {UINT, "Num Layers", 1},
    {UINT, "Num Transport Blocks Present", 1},
    {UINT, "Serving Cell Index", 1},
};

const Fmt LtePdschStatIndication_Record_TB_v36 [] = {
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RV", 0}, // 2 bits
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "CRC Result", 0}, // 1 bit
    {UINT, "RNTI Type", 1}, // 4 bits
    {PLACEHOLDER, "TB Index", 0}, // 1 bit
    {PLACEHOLDER, "Discarded reTx Present", 0}, // 1 bit
    {PLACEHOLDER, "Did Recombining", 0}, // 1 bits
    {SKIP,NULL,2},
    {UINT, "TB Size", 2},
    {UINT, "MCS", 1},
    {UINT, "Num RBs", 1},
    {UINT, "Modulation Type", 1},
    {UINT, "QED2 Interim Status", 1},//added 2 bit
    {UINT, "QED Iteration", 0},//added 6 bit
    {SKIP, NULL, 2},//modified
    {PLACEHOLDER, "ACK/NACK Decision", 0},
};

const Fmt LtePdschStatIndication_Record_TB_v32 [] = {
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RV", 0}, // 2 bits
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "CRC Result", 0}, // 1 bit
    {UINT, "RNTI Type", 1}, // 4 bits
    {PLACEHOLDER, "TB Index", 0}, // 1 bit
    {PLACEHOLDER, "Discarded reTx Present", 0}, // 1 bit
    {PLACEHOLDER, "Did Recombining", 0}, // 1 bits
    {UINT, "TB Size", 2},
    {UINT, "MCS", 1},
    {UINT, "Num RBs", 1},
    {UINT, "Modulation Type", 1},
    {SKIP, NULL, 1},
    {PLACEHOLDER, "ACK/NACK Decision", 0},
};

const Fmt LtePdschStatIndication_Record_TB_v24 [] = {
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RV", 0}, // 2 bits
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "CRC Result", 0}, // 1 bit
    {UINT, "RNTI Type", 1}, // 4 bits
    {PLACEHOLDER, "TB Index", 0}, // 1 bit
    {PLACEHOLDER, "Discarded reTx Present", 0}, // 1 bit
    {PLACEHOLDER, "Did Recombining", 0}, // 1 bits
    {UINT, "TB Size", 2},
    {UINT, "MCS", 1},
    {UINT, "Num RBs", 1},
    {UINT, "Modulation Type", 1},
    {SKIP, NULL, 1},
    {PLACEHOLDER, "ACK/NACK Decision", 0},
};

const Fmt LtePdschStatIndication_Record_TB_v16 [] = {
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RV", 0}, // 2 bits
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "CRC Result", 0}, // 1 bit
    {UINT, "RNTI Type", 1}, // 4 bits
    {PLACEHOLDER, "TB Index", 0}, // 1 bit
    {PLACEHOLDER, "Discarded reTx Present", 0}, // 1 bit
    {PLACEHOLDER, "Did Recombining", 0}, // 1 bits
    {UINT, "TB Size", 2},
    {UINT, "MCS", 1},
    {UINT, "Num RBs", 1},
    {PLACEHOLDER, "ACK/NACK Decision", 0},
};

const Fmt LtePdschStatIndication_Record_TB_v5 [] = {
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RV", 0}, // 2 bits
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "CRC Result", 0}, // 1 bit
    {UINT, "RNTI Type", 1}, // 4 bits
    {PLACEHOLDER, "TB Index", 0}, // 1 bit
    {PLACEHOLDER, "Discarded reTx Present", 0}, // 1 bit
    {PLACEHOLDER, "Did Recombining", 0}, // 1 bits
    {UINT, "TB Size", 2},
    {UINT, "MCS", 1},
    {PLACEHOLDER, "Modulation Type", 0},
    {UINT, "Num RBs", 1},
    {PLACEHOLDER, "ACK/NACK Decision", 0},
};

const ValueName LtePdschStatIndication_Record_TB_Modulation [] = {
    {0, "QPSK"},
    {1, "16QAM"},
    {2, "64QAM"},
    {8, "256QAM"},
};

const ValueName LtePdschStatIndication_Record_TB_Modulation_v24 [] = {
    {2, "QPSK"},
    {4, "16QAM"},
    {6, "64QAM"},
    {8, "256QAM"},
};

const ValueName LtePdschStatIndication_Record_TB_Modulation_v32 [] = {
    {2, "QPSK"},
    {4, "16QAM"},
    {6, "64QAM"},
    {8, "256QAM"},
};

const ValueName LtePdschStatIndication_Record_TB_Modulation_v36 [] = {
    {2, "QPSK"},
    {4, "16QAM"},
    {6, "64QAM"},
    {8, "256QAM"},
};

const ValueName LtePdschStatIndication_Record_TB_Modulation_v34 [] = {
    {2, "QPSK"},
    {4, "16QAM"},
    {6, "64QAM"},
    {8, "256QAM"},
};

const ValueName LtePdschStatIndication_Record_TB_CrcResult [] = {
    {0, "Fail"},
    {1, "Pass"},
};
const ValueName LtePdschStatIndication_Record_TB_DiscardedReTxPresent [] = {
    {0, "None"},
    {1, "Present"},
};

const ValueName LtePdschStatIndication_Record_TB_DidRecombining [] = {
    {0, "No"},
    {1, "Yes"},
};

const ValueName LtePdschStatIndication_Record_TB_Discarded_ReTx [] = {
    {0, "NO_DISCARD"},
    {1, "DISCARD_AND_ACK"},
};

const ValueName LtePdschStatIndication_Record_TB_AckNackDecision [] = {
    {0, "NACK"},
    {1, "ACK"},
};

const Fmt LtePdschStatIndication_Record_v36_P2 [] = {
    {UINT, "PMCH ID", 1},
    {UINT, "Area ID", 1},
    {SKIP,NULL,2},
};

const Fmt LtePdschStatIndication_Record_v32_P2 [] = {
    {UINT, "PMCH ID", 1},
    {UINT, "Area ID", 1},
};

const Fmt LtePdschStatIndication_Record_v24_P2 [] = {
    {UINT, "PMCH ID", 1},
    {UINT, "Area ID", 1},
};

const Fmt LtePdschStatIndication_Record_v16_P2 [] = {
    {UINT, "PMCH ID", 1},
    {UINT, "Area ID", 1},
};

const Fmt LtePdschStatIndication_Record_v5_P2 [] = {
    {UINT, "PMCH ID", 1},
    {UINT, "Area ID", 1},
};

const Fmt LtePdschStatIndication_Payload_v37 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdschStatIndication_Record_v37_P1 [] = {
    {UINT, "Subframe Num", 2},
    {PLACEHOLDER, "Frame Num", 0},
    {UINT, "Num RBs", 1},
    {UINT, "Num Layers", 1},
    {UINT, "Num Transport Blocks Present", 1},
    {UINT, "Serving Cell Index", 1},
    //{PLACEHOLDER, "HSIC Enabled", 0},//removed

    {SKIP,NULL,1},
    {UINT, "Alt TBS Enabled",1},
    {SKIP, NULL, 4},
};

const Fmt LtePdschStatIndication_Record_TB_v37 [] = {
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RV", 0}, // 2 bits
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "CRC Result", 0}, // 1 bit
    {UINT, "RNTI Type", 1}, // 4 bits
    {PLACEHOLDER, "TB Index", 0}, // 1 bit
    {PLACEHOLDER, "Discarded reTx Present", 0}, // 1 bit
    {PLACEHOLDER, "Discarded ReTx",0}, // shift 6 bits, total 2 bits
    {UINT, "Did Recombining", 1}, // 1 bits
    {SKIP,NULL,1},
    {UINT, "TB Size", 2},
    {UINT, "MCS", 1},
    {UINT, "Num RBs", 1},
    {UINT, "Modulation Type", 1},
    {UINT, "QED2 Interim Status", 1},//added 2 bit
    {UINT, "QED Iteration", 0},//added 6 bit
    {SKIP, NULL, 2},//modified
    {PLACEHOLDER, "ACK/NACK Decision", 0},
};

const Fmt LtePdschStatIndication_Record_v37_P2 [] = {
    {UINT, "PMCH ID", 1},
    {UINT, "Area ID", 1},
    {SKIP,NULL,2},
};

const Fmt LtePdschStatIndication_Payload_v40 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdschStatIndication_Record_v40_P1 [] = {
    {UINT, "Subframe Num", 2},
    {PLACEHOLDER, "Frame Num", 0},
    {UINT, "Num RBs", 1},
    {UINT, "Num Layers", 1},
    {UINT, "Num Transport Blocks Present", 1},
    {UINT, "Serving Cell Index", 1},
    {SKIP,NULL,1},//skip Alt TBS Enabled
    //{UINT, "Alt TBS Enabled",1},
    {SKIP, NULL, 5},
};

const Fmt LtePdschStatIndication_Record_TB_v40 [] = {
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RV", 0}, // 2 bits
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "CRC Result", 0}, // 1 bit
    {UINT, "RNTI Type", 2}, // 4 bits
    {PLACEHOLDER, "TB Index", 0}, // 1 bit
    {PLACEHOLDER, "Discarded reTx Present", 0}, // 1 bit
    {PLACEHOLDER,"Discarded ReTx",0},   // shift 6 bits,total 4 bits
    {PLACEHOLDER, "Did Recombining", 0}, // shift 10 bits, total 1 bits
    {SKIP,NULL,1},
    {UINT, "TB Size", 2},
    {UINT, "MCS", 1},
    {UINT, "Num RBs", 1},
    {UINT, "Modulation Type", 1},
    {UINT, "QED2 Interim Status", 1},//added 2 bit
    {UINT, "QED Iteration", 0},//added 6 bit
    {SKIP, NULL, 2},//modified
    {PLACEHOLDER, "ACK/NACK Decision", 0},
};

const Fmt LtePdschStatIndication_Record_v40_P2 [] = {
    {UINT, "PMCH ID", 1},
    {UINT, "Area ID", 1},
    {SKIP,NULL,2},
};

const Fmt LtePdschStatIndication_Payload_v34 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePdschStatIndication_Record_v34_P1 [] = {
    {UINT, "Subframe Num", 2},
    {PLACEHOLDER, "Frame Num", 0},
    {UINT, "Num RBs", 1},
    {UINT, "Num Layers", 1},
    {UINT, "Num Transport Blocks Present", 1},
    {UINT, "Serving Cell Index", 1},
};

const Fmt LtePdschStatIndication_Record_TB_v34 [] = {
    {UINT, "HARQ ID", 1},   // 4 bits
    {PLACEHOLDER, "RV", 0}, // 2 bits
    {PLACEHOLDER, "NDI", 0},    // 1 bit
    {PLACEHOLDER, "CRC Result", 0}, // 1 bit        
    {UINT, "RNTI Type", 1}, // 4 bits
    {PLACEHOLDER, "TB Index", 0}, // 1 bit
    {PLACEHOLDER, "Discarded ReTx", 0}, // 1 bit
    {PLACEHOLDER, "Did Recombining", 0}, // 1 bits
    {UINT, "TB Size", 2},
    {UINT, "MCS", 1},
    {UINT, "Num RBs", 1},
    {UINT, "Modulation Type", 1},
    {UINT, "QED2 Interim Status", 1},//added 2 bit
    {UINT, "QED Iteration", 0},//added 6 bit
    {PLACEHOLDER, "ACK/NACK Decision", 0},
};

const ValueName LtePdschStatIndication_Record_TB_v34_QED_Iteration [] = {
    {0, "OFF"},
};

const Fmt LtePdschStatIndication_Record_v34_P2 [] = {
    {UINT, "PMCH ID", 1},
    {UINT, "Area ID", 1},
}; 

static int _decode_lte_pdsch_stat_indication_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 36:
        {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v36,
                    ARRAY_SIZE(LtePdschStatIndication_Payload_v36, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v36_P1,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v36_P1, Fmt),
                        b, offset, length, result_record_item);
                int iNonDecodeP1_1 = _search_result_int(result_record_item,
                        "Subframe Num");
                int iSubFN = iNonDecodeP1_1 & 15;
                int iFN = (iNonDecodeP1_1 >> 4) & 4095;
                PyObject *old_object = _replace_result_int(result_record_item,
                        "Subframe Num", iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Frame Num", iFN);
                Py_DECREF(old_object);
                int iNonDecodeP1_2 = _search_result_int(result_record_item,
                        "Serving Cell Index");
                int iServCellIdx = iNonDecodeP1_2 & 7; // last 3 bits
                int iHSICEnabled = (iNonDecodeP1_2 >> 3) & 15; // next 4 bits
                old_object = _replace_result_int(result_record_item,
                        "Serving Cell Index", iServCellIdx);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Serving Cell Index",
                        ValueNameCellIndex,
                        ARRAY_SIZE(ValueNameCellIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "HSIC Enabled", iHSICEnabled);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "HSIC Enabled",
                        ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                int num_TB = _search_result_int(result_record_item,
                        "Num Transport Blocks Present");
                PyObject *result_record_item_TB_list = PyList_New(0);
                for (int i = 0; i < num_TB; i++) {
                    PyObject *result_record_item_TB_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v36,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_v36, Fmt),
                            b, offset, length, result_record_item_TB_item);
                    int iNonDecodeP2_1 = _search_result_int(
                            result_record_item_TB_item, "HARQ ID");
                    int iHarqId = iNonDecodeP2_1 & 15; // last 4 bits
                    int iRV = (iNonDecodeP2_1 >> 4) & 3; // next 2 bits
                    int iNDI = (iNonDecodeP2_1 >> 6) & 1; // next 1 bit
                    int iCrcResult = (iNonDecodeP2_1 >> 7) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "HARQ ID", iHarqId);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RV", iRV);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "NDI", iNDI);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "CRC Result", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "CRC Result",
                            LtePdschStatIndication_Record_TB_CrcResult,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult,
                                ValueName),
                            "(MI)Unknown");
                    int iNonDecodeP2_2 = _search_result_int(result_record_item_TB_item,
                            "RNTI Type");
                    int iRNTI = iNonDecodeP2_2 & 15; // last 4 bits;
                    int iTbIdx = (iNonDecodeP2_2 >> 4) & 1; // next 1 bit
                    int iDiscardedReTxPresent = (iNonDecodeP2_2 >> 5) & 1; // next 1 bit
                    int iDidRecombining = (iNonDecodeP2_2 >> 6) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RNTI Type", iRNTI);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "RNTI Type",
                            RNTIType,
                            ARRAY_SIZE(RNTIType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "TB Index", iTbIdx);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Discarded reTx Present", iDiscardedReTxPresent);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Discarded reTx Present",
                            LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Did Recombining", iDidRecombining);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Did Recombining",
                            LtePdschStatIndication_Record_TB_DidRecombining,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining,
                                ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Modulation Type",
                            LtePdschStatIndication_Record_TB_Modulation_v36,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v36,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "ACK/NACK Decision", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "ACK/NACK Decision",
                            LtePdschStatIndication_Record_TB_AckNackDecision,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision,
                                ValueName),
                            "(MI)Unknown");

                    int qedInterimStatus = _search_result_int(result_record_item_TB_item,
                            "QED2 Interim Status");

                    int qedIterations=(qedInterimStatus>>2)&47;
                    qedInterimStatus=qedInterimStatus & 3;

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "QED2 Interim Status", qedInterimStatus);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "QED Iteration", qedIterations);
                    Py_DECREF(old_object);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_item_TB_item, "dict");
                    PyList_Append(result_record_item_TB_list, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_item_TB_item);
                }
                PyObject *t2 = Py_BuildValue("(sOs)", "Transport Blocks",
                        result_record_item_TB_list, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_item_TB_list);
                if (num_TB == 1) {
                    offset += 12;    // v32, TB is 8 bytes
                }
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v36_P2,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v36_P2, Fmt),
                        b, offset, length, result_record_item);

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
    case 32:
        {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v32,
                    ARRAY_SIZE(LtePdschStatIndication_Payload_v32, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v32_P1,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v32_P1, Fmt),
                        b, offset, length, result_record_item);
                int iNonDecodeP1_1 = _search_result_int(result_record_item,
                        "Subframe Num");
                int iSubFN = iNonDecodeP1_1 & 15;
                int iFN = (iNonDecodeP1_1 >> 4) & 4095;
                PyObject *old_object = _replace_result_int(result_record_item,
                        "Subframe Num", iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Frame Num", iFN);
                Py_DECREF(old_object);
                int iNonDecodeP1_2 = _search_result_int(result_record_item,
                        "Serving Cell Index");
                int iServCellIdx = iNonDecodeP1_2 & 7; // last 3 bits
                int iHSICEnabled = (iNonDecodeP1_2 >> 3) & 15; // next 4 bits
                old_object = _replace_result_int(result_record_item,
                        "Serving Cell Index", iServCellIdx);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Serving Cell Index",
                        ValueNameCellIndex,
                        ARRAY_SIZE(ValueNameCellIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "HSIC Enabled", iHSICEnabled);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "HSIC Enabled",
                        ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                int num_TB = _search_result_int(result_record_item,
                        "Num Transport Blocks Present");
                PyObject *result_record_item_TB_list = PyList_New(0);
                for (int i = 0; i < num_TB; i++) {
                    PyObject *result_record_item_TB_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v32,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_v32, Fmt),
                            b, offset, length, result_record_item_TB_item);
                    int iNonDecodeP2_1 = _search_result_int(
                            result_record_item_TB_item, "HARQ ID");
                    int iHarqId = iNonDecodeP2_1 & 15; // last 4 bits
                    int iRV = (iNonDecodeP2_1 >> 4) & 3; // next 2 bits
                    int iNDI = (iNonDecodeP2_1 >> 6) & 1; // next 1 bit
                    int iCrcResult = (iNonDecodeP2_1 >> 7) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "HARQ ID", iHarqId);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RV", iRV);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "NDI", iNDI);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "CRC Result", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "CRC Result",
                            LtePdschStatIndication_Record_TB_CrcResult,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult,
                                ValueName),
                            "(MI)Unknown");
                    int iNonDecodeP2_2 = _search_result_int(result_record_item_TB_item,
                            "RNTI Type");
                    int iRNTI = iNonDecodeP2_2 & 15; // last 4 bits;
                    int iTbIdx = (iNonDecodeP2_2 >> 4) & 1; // next 1 bit
                    int iDiscardedReTxPresent = (iNonDecodeP2_2 >> 5) & 1; // next 1 bit
                    int iDidRecombining = (iNonDecodeP2_2 >> 6) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RNTI Type", iRNTI);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "RNTI Type",
                            RNTIType,
                            ARRAY_SIZE(RNTIType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "TB Index", iTbIdx);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Discarded reTx Present", iDiscardedReTxPresent);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Discarded reTx Present",
                            LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Did Recombining", iDidRecombining);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Did Recombining",
                            LtePdschStatIndication_Record_TB_DidRecombining,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining,
                                ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Modulation Type",
                            LtePdschStatIndication_Record_TB_Modulation_v32,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v32,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "ACK/NACK Decision", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "ACK/NACK Decision",
                            LtePdschStatIndication_Record_TB_AckNackDecision,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision,
                                ValueName),
                            "(MI)Unknown");
                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_item_TB_item, "dict");
                    PyList_Append(result_record_item_TB_list, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_item_TB_item);
                }
                PyObject *t2 = Py_BuildValue("(sOs)", "Transport Blocks",
                        result_record_item_TB_list, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_item_TB_list);
                if (num_TB == 1) {
                    offset += 8;    // v32, TB is 8 bytes
                }
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v32_P2,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v32_P2, Fmt),
                        b, offset, length, result_record_item);

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
    case 24:
        {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v24,
                    ARRAY_SIZE(LtePdschStatIndication_Payload_v24, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v24_P1,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v24_P1, Fmt),
                        b, offset, length, result_record_item);
                int iNonDecodeP1_1 = _search_result_int(result_record_item,
                        "Subframe Num");
                int iSubFN = iNonDecodeP1_1 & 15;
                int iFN = (iNonDecodeP1_1 >> 4) & 4095;
                PyObject *old_object = _replace_result_int(result_record_item,
                        "Subframe Num", iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Frame Num", iFN);
                Py_DECREF(old_object);
                int iNonDecodeP1_2 = _search_result_int(result_record_item,
                        "Serving Cell Index");
                int iServCellIdx = iNonDecodeP1_2 & 7; // last 3 bits
                int iHSICEnabled = (iNonDecodeP1_2 >> 3) & 15; // next 4 bits
                old_object = _replace_result_int(result_record_item,
                        "Serving Cell Index", iServCellIdx);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Serving Cell Index",
                        ValueNameCellIndex,
                        ARRAY_SIZE(ValueNameCellIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "HSIC Enabled", iHSICEnabled);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "HSIC Enabled",
                        ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                int num_TB = _search_result_int(result_record_item,
                        "Num Transport Blocks Present");
                PyObject *result_record_item_TB_list = PyList_New(0);
                for (int i = 0; i < num_TB; i++) {
                    PyObject *result_record_item_TB_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v24,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_v24, Fmt),
                            b, offset, length, result_record_item_TB_item);
                    int iNonDecodeP2_1 = _search_result_int(
                            result_record_item_TB_item, "HARQ ID");
                    int iHarqId = iNonDecodeP2_1 & 15; // last 4 bits
                    int iRV = (iNonDecodeP2_1 >> 4) & 3; // next 2 bits
                    int iNDI = (iNonDecodeP2_1 >> 6) & 1; // next 1 bit
                    int iCrcResult = (iNonDecodeP2_1 >> 7) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "HARQ ID", iHarqId);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RV", iRV);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "NDI", iNDI);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "CRC Result", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "CRC Result",
                            LtePdschStatIndication_Record_TB_CrcResult,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult,
                                ValueName),
                            "(MI)Unknown");
                    int iNonDecodeP2_2 = _search_result_int(result_record_item_TB_item,
                            "RNTI Type");
                    int iRNTI = iNonDecodeP2_2 & 15; // last 4 bits;
                    int iTbIdx = (iNonDecodeP2_2 >> 4) & 1; // next 1 bit
                    int iDiscardedReTxPresent = (iNonDecodeP2_2 >> 5) & 1; // next 1 bit
                    int iDidRecombining = (iNonDecodeP2_2 >> 6) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RNTI Type", iRNTI);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "RNTI Type",
                            RNTIType,
                            ARRAY_SIZE(RNTIType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "TB Index", iTbIdx);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Discarded reTx Present", iDiscardedReTxPresent);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Discarded reTx Present",
                            LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Did Recombining", iDidRecombining);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Did Recombining",
                            LtePdschStatIndication_Record_TB_DidRecombining,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining,
                                ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Modulation Type",
                            LtePdschStatIndication_Record_TB_Modulation_v24,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v24,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "ACK/NACK Decision", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "ACK/NACK Decision",
                            LtePdschStatIndication_Record_TB_AckNackDecision,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision,
                                ValueName),
                            "(MI)Unknown");
                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_item_TB_item, "dict");
                    PyList_Append(result_record_item_TB_list, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_item_TB_item);
                }
                PyObject *t2 = Py_BuildValue("(sOs)", "Transport Blocks",
                        result_record_item_TB_list, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_item_TB_list);
                if (num_TB == 1) {
                    offset += 8;    // v24, TB is 8 bytes
                }
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v24_P2,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v24_P2, Fmt),
                        b, offset, length, result_record_item);

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
    case 16:
        {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v16,
                    ARRAY_SIZE(LtePdschStatIndication_Payload_v16, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v16_P1,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v16_P1, Fmt),
                        b, offset, length, result_record_item);
                int iNonDecodeP1_1 = _search_result_int(result_record_item,
                        "Subframe Num");
                int iSubFN = iNonDecodeP1_1 & 15;
                int iFN = (iNonDecodeP1_1 >> 4) & 4095;
                PyObject *old_object = _replace_result_int(result_record_item,
                        "Subframe Num", iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Frame Num", iFN);
                Py_DECREF(old_object);
                int iNonDecodeP1_2 = _search_result_int(result_record_item,
                        "Serving Cell Index");
                int iServCellIdx = iNonDecodeP1_2 & 7; // last 3 bits
                int iHSICEnabled = (iNonDecodeP1_2 >> 3) & 15; // next 4 bits
                old_object = _replace_result_int(result_record_item,
                        "Serving Cell Index", iServCellIdx);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Serving Cell Index",
                        ValueNameCellIndex,
                        ARRAY_SIZE(ValueNameCellIndex, ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "HSIC Enabled", iHSICEnabled);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "HSIC Enabled",
                        ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                int num_TB = _search_result_int(result_record_item,
                        "Num Transport Blocks Present");
                PyObject *result_record_item_TB_list = PyList_New(0);
                for (int i = 0; i < num_TB; i++) {
                    PyObject *result_record_item_TB_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v16,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_v16, Fmt),
                            b, offset, length, result_record_item_TB_item);
                    int iNonDecodeP2_1 = _search_result_int(
                            result_record_item_TB_item, "HARQ ID");
                    int iHarqId = iNonDecodeP2_1 & 15; // last 4 bits
                    int iRV = (iNonDecodeP2_1 >> 4) & 3; // next 2 bits
                    int iNDI = (iNonDecodeP2_1 >> 6) & 1; // next 1 bit
                    int iCrcResult = (iNonDecodeP2_1 >> 7) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "HARQ ID", iHarqId);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RV", iRV);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "NDI", iNDI);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "CRC Result", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "CRC Result",
                            LtePdschStatIndication_Record_TB_CrcResult,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult,
                                ValueName),
                            "(MI)Unknown");
                    int iNonDecodeP2_2 = _search_result_int(result_record_item_TB_item,
                            "RNTI Type");
                    int iRNTI = iNonDecodeP2_2 & 15; // last 4 bits;
                    int iTbIdx = (iNonDecodeP2_2 >> 4) & 1; // next 1 bit
                    int iDiscardedReTxPresent = (iNonDecodeP2_2 >> 5) & 1; // next 1 bit
                    int iDidRecombining = (iNonDecodeP2_2 >> 6) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RNTI Type", iRNTI);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "RNTI Type",
                            RNTIType,
                            ARRAY_SIZE(RNTIType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "TB Index", iTbIdx);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Discarded reTx Present", iDiscardedReTxPresent);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Discarded reTx Present",
                            LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Did Recombining", iDidRecombining);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Did Recombining",
                            LtePdschStatIndication_Record_TB_DidRecombining,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "ACK/NACK Decision", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "ACK/NACK Decision",
                            LtePdschStatIndication_Record_TB_AckNackDecision,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision,
                                ValueName),
                            "(MI)Unknown");
                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_item_TB_item, "dict");
                    PyList_Append(result_record_item_TB_list, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_item_TB_item);
                }
                PyObject *t2 = Py_BuildValue("(sOs)", "Transport Blocks",
                        result_record_item_TB_list, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_item_TB_list);
                if (num_TB == 1) {
                    offset += 6;
                }
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v16_P2,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v16_P2, Fmt),
                        b, offset, length, result_record_item);

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
    case 5:
        {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v5,
                    ARRAY_SIZE(LtePdschStatIndication_Payload_v5, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v5_P1,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v5_P1, Fmt),
                        b, offset, length, result_record_item);
                int iNonDecodeP1_1 = _search_result_int(result_record_item,
                        "Subframe Num");
                int iSubFN = iNonDecodeP1_1 & 15;
                int iFN = (iNonDecodeP1_1 >> 4) & 4095;
                PyObject *old_object = _replace_result_int(result_record_item,
                        "Subframe Num", iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Frame Num", iFN);
                Py_DECREF(old_object);
                int iNonDecodeP1_2 = _search_result_int(result_record_item,
                        "Serving Cell Index");
                int iServCellIdx = iNonDecodeP1_2 & 7; // last 3 bits
                old_object = _replace_result_int(result_record_item,
                        "Serving Cell Index", iServCellIdx);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Serving Cell Index",
                        ValueNameCellIndex,
                        ARRAY_SIZE(ValueNameCellIndex, ValueName),
                        "(MI)Unknown");
                int num_TB = _search_result_int(result_record_item,
                        "Num Transport Blocks Present");
                PyObject *result_record_item_TB_list = PyList_New(0);
                for (int i = 0; i < num_TB; i++) {
                    PyObject *result_record_item_TB_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v5,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_v5, Fmt),
                            b, offset, length, result_record_item_TB_item);
                    int iNonDecodeP2_1 = _search_result_int(
                            result_record_item_TB_item, "HARQ ID");
                    int iHarqId = iNonDecodeP2_1 & 15; // last 4 bits
                    int iRV = (iNonDecodeP2_1 >> 4) & 3; // next 2 bits
                    int iNDI = (iNonDecodeP2_1 >> 6) & 1; // next 1 bit
                    int iCrcResult = (iNonDecodeP2_1 >> 7) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "HARQ ID", iHarqId);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RV", iRV);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "NDI", iNDI);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "CRC Result", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "CRC Result",
                            LtePdschStatIndication_Record_TB_CrcResult,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult,
                                ValueName),
                            "(MI)Unknown");
                    int iNonDecodeP2_2 = _search_result_int(result_record_item_TB_item,
                            "RNTI Type");
                    int iRNTI = iNonDecodeP2_2 & 15; // last 4 bits;
                    int iTbIdx = (iNonDecodeP2_2 >> 4) & 1; // next 1 bit
                    int iDiscardedReTxPresent = (iNonDecodeP2_2 >> 5) & 1; // next 1 bit
                    int iDidRecombining = (iNonDecodeP2_2 >> 6) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RNTI Type", iRNTI);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "RNTI Type",
                            RNTIType,
                            ARRAY_SIZE(RNTIType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "TB Index", iTbIdx);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Discarded reTx Present", iDiscardedReTxPresent);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Discarded reTx Present",
                            LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Did Recombining", iDidRecombining);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Did Recombining",
                            LtePdschStatIndication_Record_TB_DidRecombining,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining,
                                ValueName),
                            "(MI)Unknown");
                    int iMCS = _search_result_int(result_record_item_TB_item,
                            "MCS");
                    int iModulationType = -1;
                    if (iMCS >= 17) {
                        iModulationType = 2;
                    } else if (iMCS >= 10 && iMCS < 17) {
                        iModulationType = 1;
                    } else if (iMCS < 10) {
                        iModulationType = 0;
                    }
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Modulation Type", iModulationType);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Modulation Type",
                            LtePdschStatIndication_Record_TB_Modulation,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "ACK/NACK Decision", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "ACK/NACK Decision",
                            LtePdschStatIndication_Record_TB_AckNackDecision,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision,
                                ValueName),
                            "(MI)Unknown");
                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_item_TB_item, "dict");
                    PyList_Append(result_record_item_TB_list, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_item_TB_item);
                }
                PyObject *t2 = Py_BuildValue("(sOs)", "Transport Blocks",
                        result_record_item_TB_list, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_item_TB_list);
                if (num_TB == 1) {
                    offset += 6;
                }
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v5_P2,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v5_P2, Fmt),
                        b, offset, length, result_record_item);

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
    case 37:
        {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v37,
                    ARRAY_SIZE(LtePdschStatIndication_Payload_v37, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v37_P1,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v37_P1, Fmt),
                        b, offset, length, result_record_item);
                int iNonDecodeP1_1 = _search_result_int(result_record_item,
                        "Subframe Num");
                int iSubFN = iNonDecodeP1_1 & 15;
                int iFN = (iNonDecodeP1_1 >> 4) & 4095;
                PyObject *old_object = _replace_result_int(result_record_item,
                        "Subframe Num", iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Frame Num", iFN);
                Py_DECREF(old_object);
                int iNonDecodeP1_2 = _search_result_int(result_record_item,
                        "Serving Cell Index");
                int iServCellIdx = iNonDecodeP1_2 & 7; // last 3 bits
                old_object = _replace_result_int(result_record_item,
                        "Serving Cell Index", iServCellIdx);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Serving Cell Index",
                        ValueNameCellIndex,
                        ARRAY_SIZE(ValueNameCellIndex, ValueName),
                        "(MI)Unknown");

                int num_TB = _search_result_int(result_record_item,
                        "Num Transport Blocks Present");
                PyObject *result_record_item_TB_list = PyList_New(0);
                for (int i = 0; i < num_TB; i++) {
                    PyObject *result_record_item_TB_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v37,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_v37, Fmt),
                            b, offset, length, result_record_item_TB_item);
                    int iNonDecodeP2_1 = _search_result_int(
                            result_record_item_TB_item, "HARQ ID");
                    int iHarqId = iNonDecodeP2_1 & 15; // last 4 bits
                    int iRV = (iNonDecodeP2_1 >> 4) & 3; // next 2 bits
                    int iNDI = (iNonDecodeP2_1 >> 6) & 1; // next 1 bit
                    int iCrcResult = (iNonDecodeP2_1 >> 7) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "HARQ ID", iHarqId);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RV", iRV);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "NDI", iNDI);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "CRC Result", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "CRC Result",
                            LtePdschStatIndication_Record_TB_CrcResult,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult,
                                ValueName),
                            "(MI)Unknown");
                    int iNonDecodeP2_2 = _search_result_int(result_record_item_TB_item,
                            "RNTI Type");
                    int iRNTI = iNonDecodeP2_2 & 15; // last 4 bits;
                    int iTbIdx = (iNonDecodeP2_2 >> 4) & 1; // next 1 bit
                    int iDiscardedReTxPresent = (iNonDecodeP2_2 >> 5) & 1; // next 1 bit
                    int iDiscardedReTx = (iNonDecodeP2_2 >> 6) & 3; //shift 6 bit, next 2 bits

                    int temp =_search_result_int(result_record_item_TB_item,
                            "Did Recombining");
                    int iDidRecombining = temp & 1; // next 1 bit

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RNTI Type", iRNTI);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "RNTI Type",
                            RNTIType,
                            ARRAY_SIZE(RNTIType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "TB Index", iTbIdx);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Discarded ReTx", iDiscardedReTx);
                    Py_DECREF(old_object);

                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Discarded ReTx",
                            LtePdschStatIndication_Record_TB_Discarded_ReTx,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_Discarded_ReTx,
                                ValueName),
                            "(MI)Unknown");

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Discarded reTx Present", iDiscardedReTxPresent);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Discarded reTx Present",
                            LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                                ValueName),
                            "(MI)Unknown");

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Did Recombining", iDidRecombining);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Did Recombining",
                            LtePdschStatIndication_Record_TB_DidRecombining,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining,
                                ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Modulation Type",
                            LtePdschStatIndication_Record_TB_Modulation_v36,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v36,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "ACK/NACK Decision", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "ACK/NACK Decision",
                            LtePdschStatIndication_Record_TB_AckNackDecision,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision,
                                ValueName),
                            "(MI)Unknown");

                    int qedInterimStatus = _search_result_int(result_record_item_TB_item,
                            "QED2 Interim Status");

                    int qedIterations=(qedInterimStatus>>2)&47;
                    qedInterimStatus=qedInterimStatus & 3;

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "QED2 Interim Status", qedInterimStatus);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "QED Iteration", qedIterations);
                    Py_DECREF(old_object);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_item_TB_item, "dict");
                    PyList_Append(result_record_item_TB_list, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_item_TB_item);
                }
                PyObject *t2 = Py_BuildValue("(sOs)", "Transport Blocks",
                        result_record_item_TB_list, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_item_TB_list);
                if (num_TB == 1) {
                    offset += 12;    // v32, TB is 8 bytes
                }
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v37_P2,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v37_P2, Fmt),
                        b, offset, length, result_record_item);

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
    case 40:
        {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v40,
                    ARRAY_SIZE(LtePdschStatIndication_Payload_v40, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v40_P1,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v40_P1, Fmt),
                        b, offset, length, result_record_item);
                int iNonDecodeP1_1 = _search_result_int(result_record_item,
                        "Subframe Num");
                int iSubFN = iNonDecodeP1_1 & 15;
                int iFN = (iNonDecodeP1_1 >> 4) & 4095;
                PyObject *old_object = _replace_result_int(result_record_item,
                        "Subframe Num", iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Frame Num", iFN);
                Py_DECREF(old_object);
                int iNonDecodeP1_2 = _search_result_int(result_record_item,
                        "Serving Cell Index");
                int iServCellIdx = iNonDecodeP1_2 & 7; // last 3 bits
                old_object = _replace_result_int(result_record_item,
                        "Serving Cell Index", iServCellIdx);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Serving Cell Index",
                        ValueNameCellIndex,
                        ARRAY_SIZE(ValueNameCellIndex, ValueName),
                        "(MI)Unknown");
                (void) _map_result_field_to_name(result_record_item,
                        "HSIC Enabled",
                        ValueNameEnableOrDisable,
                        ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                        "(MI)Unknown");
                int num_TB = _search_result_int(result_record_item,
                        "Num Transport Blocks Present");
                PyObject *result_record_item_TB_list = PyList_New(0);
                for (int i = 0; i < num_TB; i++) {
                    PyObject *result_record_item_TB_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v40,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_v40, Fmt),
                            b, offset, length, result_record_item_TB_item);
                    int iNonDecodeP2_1 = _search_result_int(
                            result_record_item_TB_item, "HARQ ID");
                    int iHarqId = iNonDecodeP2_1 & 15; // last 4 bits
                    int iRV = (iNonDecodeP2_1 >> 4) & 3; // next 2 bits
                    int iNDI = (iNonDecodeP2_1 >> 6) & 1; // next 1 bit
                    int iCrcResult = (iNonDecodeP2_1 >> 7) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "HARQ ID", iHarqId);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RV", iRV);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "NDI", iNDI);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "CRC Result", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "CRC Result",
                            LtePdschStatIndication_Record_TB_CrcResult,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult,
                                ValueName),
                            "(MI)Unknown");
                    int iNonDecodeP2_2 = _search_result_int(result_record_item_TB_item,
                            "RNTI Type");
                    int iRNTI = iNonDecodeP2_2 & 15; // last 4 bits;
                    int iTbIdx = (iNonDecodeP2_2 >> 4) & 1; // next 1 bit
                    int iDiscardedReTxPresent = (iNonDecodeP2_2 >> 5) & 1; // next 1 bit
                    int iDiscardedReTx = (iNonDecodeP2_2 >> 6 ) & 0xf;//next 4 bit
                    int iDidRecombining = (iNonDecodeP2_2 >> 10) & 1; // next 1 bit
                    
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RNTI Type", iRNTI);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "RNTI Type",
                            RNTIType,
                            ARRAY_SIZE(RNTIType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "TB Index", iTbIdx);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Discarded reTx Present", iDiscardedReTxPresent);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Discarded reTx Present",
                            LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DiscardedReTxPresent,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Discarded ReTx", iDiscardedReTx);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Did Recombining", iDidRecombining);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Did Recombining",
                            LtePdschStatIndication_Record_TB_DidRecombining,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining,
                                ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Modulation Type",
                            LtePdschStatIndication_Record_TB_Modulation_v36,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v36,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "ACK/NACK Decision", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "ACK/NACK Decision",
                            LtePdschStatIndication_Record_TB_AckNackDecision,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision,
                                ValueName),
                            "(MI)Unknown");

                    int qedInterimStatus = _search_result_int(result_record_item_TB_item,
                            "QED2 Interim Status");

                    int qedIterations=(qedInterimStatus>>2)&47;
                    qedInterimStatus=qedInterimStatus & 3;

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "QED2 Interim Status", qedInterimStatus);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "QED Iteration", qedIterations);
                    Py_DECREF(old_object);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_item_TB_item, "dict");
                    PyList_Append(result_record_item_TB_list, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_item_TB_item);
                }
                PyObject *t2 = Py_BuildValue("(sOs)", "Transport Blocks",
                        result_record_item_TB_list, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_item_TB_list);
                if (num_TB == 1) {
                    offset += 12;    // v32, TB is 8 bytes
                }
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v40_P2,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v40_P2, Fmt),
                        b, offset, length, result_record_item);

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
        
    case 34:
        {
            offset += _decode_by_fmt(LtePdschStatIndication_Payload_v34,
                    ARRAY_SIZE(LtePdschStatIndication_Payload_v34, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v34_P1,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v34_P1, Fmt),
                        b, offset, length, result_record_item);
                int iNonDecodeP1_1 = _search_result_int(result_record_item,
                        "Subframe Num");
                int iSubFN = iNonDecodeP1_1 & 15;
                int iFN = (iNonDecodeP1_1 >> 4) & 4095;
                PyObject *old_object = _replace_result_int(result_record_item,
                        "Subframe Num", iSubFN);
                Py_DECREF(old_object);
                
                old_object = _replace_result_int(result_record_item,
                        "Frame Num", iFN);
                Py_DECREF(old_object);

                int iNonDecodeP1_2 = _search_result_int(result_record_item,
                        "Serving Cell Index");
                int iServCellIdx = iNonDecodeP1_2 & 7; // last 3 bits
                old_object = _replace_result_int(result_record_item,
                        "Serving Cell Index", iServCellIdx);
                Py_DECREF(old_object);

                (void) _map_result_field_to_name(result_record_item,
                        "Serving Cell Index",
                        ValueNameCellIndex,
                        ARRAY_SIZE(ValueNameCellIndex, ValueName),
                        "(MI)Unknown");

                int num_TB = _search_result_int(result_record_item,
                        "Num Transport Blocks Present");

                PyObject *result_record_item_TB_list = PyList_New(0);
                for (int i = 0; i < num_TB; i++) {
                    PyObject *result_record_item_TB_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdschStatIndication_Record_TB_v34,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_v34, Fmt),
                            b, offset, length, result_record_item_TB_item);

                    int iNonDecodeP2_1 = _search_result_int(
                            result_record_item_TB_item, "HARQ ID");

                    int iHarqId = iNonDecodeP2_1 & 15; // last 4 bits
                    int iRV = (iNonDecodeP2_1 >> 4) & 3; // next 2 bits
                    int iNDI = (iNonDecodeP2_1 >> 6) & 1; // next 1 bit
                    int iCrcResult = (iNonDecodeP2_1 >> 7) & 1; // next 1 bit
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "HARQ ID", iHarqId);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RV", iRV);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "NDI", iNDI);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "CRC Result", iCrcResult);
                    Py_DECREF(old_object);

                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "CRC Result",
                            LtePdschStatIndication_Record_TB_CrcResult,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_CrcResult,
                                ValueName),
                            "(MI)Unknown");

                    int iNonDecodeP2_2 = _search_result_int(result_record_item_TB_item,
                            "RNTI Type");

                    int iRNTI = iNonDecodeP2_2 & 15; // last 4 bits;
                    int iTbIdx = (iNonDecodeP2_2 >> 4) & 1; // next 1 bit
                    int iDiscardedReTx = (iNonDecodeP2_2 >> 5) & 1; // next 1 bit
                    int iDidRecombining = (iNonDecodeP2_2 >> 6) & 1; //shift 6 bit, next 2 bits
                 
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "RNTI Type", iRNTI);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "RNTI Type",
                            RNTIType,
                            ARRAY_SIZE(RNTIType, ValueName),
                            "(MI)Unknown");

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "TB Index", iTbIdx);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Discarded ReTx", iDiscardedReTx);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Discarded ReTx",
                            LtePdschStatIndication_Record_TB_Discarded_ReTx,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_Discarded_ReTx,
                                ValueName),
                            "(MI)Unknown");

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "Did Recombining", iDidRecombining);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Did Recombining",
                            LtePdschStatIndication_Record_TB_DidRecombining,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_DidRecombining,
                                ValueName),
                            "(MI)Unknown");

                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "Modulation Type",
                            LtePdschStatIndication_Record_TB_Modulation_v34,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_Modulation_v34,
                                ValueName),
                            "(MI)Unknown");

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "ACK/NACK Decision", iCrcResult);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "ACK/NACK Decision",
                            LtePdschStatIndication_Record_TB_AckNackDecision,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_AckNackDecision,
                                ValueName),
                            "(MI)Unknown");

                    int qedInterimStatus = _search_result_int(result_record_item_TB_item,
                            "QED2 Interim Status");
                    int qedIterations=(qedInterimStatus>>2)&47;
                    qedInterimStatus=qedInterimStatus & 3;
                    old_object = _replace_result_int(result_record_item_TB_item,
                            "QED2 Interim Status", qedInterimStatus);
                    Py_DECREF(old_object);

                    old_object = _replace_result_int(result_record_item_TB_item,
                            "QED Iteration", qedIterations);
                    Py_DECREF(old_object);

                    (void) _map_result_field_to_name(result_record_item_TB_item,
                            "QED Iteration",
                            LtePdschStatIndication_Record_TB_v34_QED_Iteration,
                            ARRAY_SIZE(LtePdschStatIndication_Record_TB_v34_QED_Iteration,
                                ValueName),
                            "(MI)Unknown");

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_item_TB_item, "dict");
                    PyList_Append(result_record_item_TB_list, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_item_TB_item);
                }

                PyObject *t2 = Py_BuildValue("(sOs)", "Transport Blocks",
                        result_record_item_TB_list, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_item_TB_list);

                if (num_TB == 1) {
                    offset += 8;    
                }
                offset += _decode_by_fmt(LtePdschStatIndication_Record_v34_P2,
                        ARRAY_SIZE(LtePdschStatIndication_Record_v34_P2, Fmt),
                        b, offset, length, result_record_item);

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
        printf("(MI)Unknown LTE PDSCH Stat Indication version: 0x%x\n", pkt_ver);
        return 0;
    }
}
