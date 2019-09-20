/*
 * LTE_PHY_Connected_Mode_Neighbor_Measurement
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePhyCncm_Fmt [] = {
    {UINT, "Version", 1},
    {UINT, "Number of SubPackets", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePhyCncm_Subpacket_Header_v1 [] = {
    {UINT, "SubPacket ID", 1},
    {UINT, "Version", 1},
    {UINT, "SubPacket Size", 2},
};

const Fmt LtePhyCncm_Subpacket_Payload_31v3 [] = {
    {UINT, "E-ARFCN", 2},
    {UINT, "Num Cells", 2}, // 6 bits
    // skip 1 bit
    {PLACEHOLDER, "Deplexing Mode", 0}, // 2 bits
    {PLACEHOLDER, "Serving Cell Index", 0}, // 4 bits
};

const Fmt LtePhyCncm_Subpacket_31v3_cell [] = {
    {UINT, "Physical Cell ID", 4},   // 10 bits
    {PLACEHOLDER, "FTL Cumulative Freq Offset", 0},    // 16 bit
    {SKIP, NULL, 4},    // RSRP[0]
    {SKIP, NULL, 4},    // RSRP[1]
    {UINT, "Inst Measured RSRP", 2},    // 12 bits (0.0625 * x - 180) dBm
    {SKIP, NULL, 2},
    {SKIP, NULL, 4},    // RSRQ[0] and RSRQ[1]
    {UINT, "Inst Measured RSRQ", 4},    // skip 10 bits, then 10 bits, (0.0625 * x - 30) dBm
    {SKIP, NULL, 4},    // RSRI[0]
    {UINT, "Inst Measured RSSI", 4},    // skip 11 bits, then 11 bits, (0.0625 * x - 110) dBm
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
};

const Fmt LtePhyCncm_Subpacket_Payload_31v4 [] = {
    {UINT, "E-ARFCN", 4},
    {UINT, "Num Cells", 2}, // 6 bits
    // skip 1 bit
    {PLACEHOLDER, "Deplexing Mode", 0}, // 2 bits
    {PLACEHOLDER, "Serving Cell Index", 0}, // 4 bits
    {SKIP, NULL, 2},
};

const Fmt LtePhyCncm_Subpacket_31v4_cell [] = {
    {UINT, "Physical Cell ID", 4},   // 10 bits
    {PLACEHOLDER, "FTL Cumulative Freq Offset", 0},    // 16 bit
    {SKIP, NULL, 4},    // RSRP[0]
    {SKIP, NULL, 4},    // RSRP[1]
    {UINT, "Inst Measured RSRP", 2},    // 12 bits (0.0625 * x - 180) dBm
    {SKIP, NULL, 2},
    {SKIP, NULL, 4},    // RSRQ[0] and RSRQ[1]
    {UINT, "Inst Measured RSRQ", 4},    // skip 10 bits, then 10 bits, (0.0625 * x - 30) dBm
    {SKIP, NULL, 4},    // RSRI[0]
    {UINT, "Inst Measured RSSI", 4},    // skip 11 bits, then 11 bits, (0.0625 * x - 110) dBm
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
};

// Note: not verified!
const Fmt LtePhyCncm_Subpacket_Payload_31v24 [] = {
    {UINT, "E-ARFCN", 4},
    {UINT, "Num Cells", 2}, // 6 bits
    // skip 1 bit
    {PLACEHOLDER, "Deplexing Mode", 0}, // 2 bits
    {PLACEHOLDER, "Serving Cell Index", 0}, // 4 bits
    {SKIP, NULL, 2},
};

// Note: not verified!
const Fmt LtePhyCncm_Subpacket_31v24_cell [] = {
    {UINT, "Physical Cell ID", 4},   // 10 bits
    {PLACEHOLDER, "FTL Cumulative Freq Offset", 0},    // 16 bit
    {SKIP, NULL, 4},    // RSRP[0]
    {SKIP, NULL, 4},    // RSRP[1]
    {UINT, "Inst Measured RSRP", 2},    // 12 bits (0.0625 * x - 180) dBm
    {SKIP, NULL, 2},
    {SKIP, NULL, 4},    // RSRQ[0] and RSRQ[1]
    {UINT, "Inst Measured RSRQ", 4},    // skip 10 bits, then 10 bits, (0.0625 * x - 30) dBm
    {SKIP, NULL, 4},    // RSRI[0]
    {UINT, "Inst Measured RSSI", 4},    // skip 11 bits, then 11 bits, (0.0625 * x - 110) dBm
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
    {SKIP, NULL, 4},
};

static int _decode_lte_phy_connected_neighbor_cell_meas_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkts = _search_result_int(result, "Number of SubPackets");

    PyObject *old_object;
    PyObject *pyfloat;
    int temp;

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkts; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LtePhyCncm_Subpacket_Header_v1,
                        ARRAY_SIZE(LtePhyCncm_Subpacket_Header_v1, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "SubPacket Size");

                if (subpkt_id == 30) {
                    // this is connected mode neighbor cell measurement
                    // request
                } else if (subpkt_id == 31 && subpkt_ver == 4) {
                    // this is connected mode neighbor cell measurement
                    // response v4
                    offset += _decode_by_fmt(
                            LtePhyCncm_Subpacket_Payload_31v4,
                            ARRAY_SIZE(LtePhyCncm_Subpacket_Payload_31v4,
                                Fmt),
                            b, offset, length, result_subpkt);
                    temp = _search_result_int(result_subpkt, "Num Cells");
                    int num_cells = temp & 63;  // 6 bits
                    // skip 1 bit
                    int duplexingMode = (temp >> 7) & 3;    // 2 bits
                    int servingCellIndx = (temp >> 9) & 15; // 4 bits
                    old_object = _replace_result_int(result_subpkt,
                            "Num Cells", num_cells);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_subpkt,
                            "Deplexing Mode", duplexingMode);
                    Py_DECREF(old_object);
                    (void)_map_result_field_to_name(result_subpkt,
                            "Deplexing Mode",
                            ValueNameDuplexingMode,
                            ARRAY_SIZE(ValueNameDuplexingMode, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_subpkt,
                            "Serving Cell Index", servingCellIndx);
                    Py_DECREF(old_object);
                    (void)_map_result_field_to_name(result_subpkt,
                            "Serving Cell Index",
                            ValueNameCellIndex,
                            ARRAY_SIZE(ValueNameCellIndex, ValueName),
                            "(MI)Unknown");

                    PyObject *result_cell = PyList_New(0);
                    for (int j = 0; j < num_cells; j++) {
                        PyObject *result_cell_item = PyList_New(0);

                        offset += _decode_by_fmt(LtePhyCncm_Subpacket_31v4_cell,
                                ARRAY_SIZE(LtePhyCncm_Subpacket_31v4_cell,
                                    Fmt),
                                b, offset, length, result_cell_item);
                        unsigned int utemp = _search_result_uint(
                                result_cell_item, "Physical Cell ID");
                        int iPhysicalCellId = utemp & 1023;  // 10 bits
                        int iFTLCFO = (utemp >> 10) & 65535; // 16 bits
                        old_object = _replace_result_int(result_cell_item,
                                "Physical Cell ID", iPhysicalCellId);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_cell_item,
                                "FTL Cumulative Freq Offset", iFTLCFO);
                        Py_DECREF(old_object);

                        temp = _search_result_int(result_cell_item,
                                "Inst Measured RSRP");
                        float RSRP = float(temp & 4095);
                        RSRP = RSRP * 0.0625 - 180.0;
                        pyfloat = Py_BuildValue("f", RSRP);
                        old_object = _replace_result(result_cell_item,
                                "Inst Measured RSRP", pyfloat);
                        Py_DECREF(old_object);
                        Py_DECREF(pyfloat);

                        utemp = _search_result_uint(result_cell_item,
                                "Inst Measured RSRQ");
                        float RSRQ = float((utemp >> 10) & 1023);
                        RSRQ = RSRQ * 0.0625 - 30.0;
                        pyfloat = Py_BuildValue("f", RSRQ);
                        old_object = _replace_result(result_cell_item,
                                "Inst Measured RSRQ", pyfloat);
                        Py_DECREF(old_object);
                        Py_DECREF(pyfloat);

                        utemp = _search_result_uint(result_cell_item,
                                "Inst Measured RSSI");
                        float RSSI = float((utemp >> 11) & 2047);
                        RSSI = RSSI * 0.0625 - 110.0;
                        pyfloat = Py_BuildValue("f", RSSI);
                        old_object = _replace_result(result_cell_item,
                                "Inst Measured RSSI", pyfloat);
                        Py_DECREF(old_object);
                        Py_DECREF(pyfloat);

                        PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                result_cell_item, "dict");
                        PyList_Append(result_cell, t3);
                        Py_DECREF(t3);
                        Py_DECREF(result_cell_item);
                    }
                    PyObject *t2 = Py_BuildValue("(sOs)", "Neighbor Cells",
                            result_cell, "list");
                    PyList_Append(result_subpkt, t2);
                    Py_DECREF(t2);
                    Py_DECREF(result_cell);

                }else if (subpkt_id == 31 && subpkt_ver == 3) {
                    // this is connected mode neighbor cell measurement
                    // response v4
                    offset += _decode_by_fmt(
                            LtePhyCncm_Subpacket_Payload_31v3,
                            ARRAY_SIZE(LtePhyCncm_Subpacket_Payload_31v3,
                                Fmt),
                            b, offset, length, result_subpkt);
                    temp = _search_result_int(result_subpkt, "Num Cells");
                    int num_cells = temp & 63;  // 6 bits
                    // skip 1 bit
                    int duplexingMode = (temp >> 7) & 3;    // 2 bits
                    int servingCellIndx = (temp >> 9) & 15; // 4 bits
                    old_object = _replace_result_int(result_subpkt,
                            "Num Cells", num_cells);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_subpkt,
                            "Deplexing Mode", duplexingMode);
                    Py_DECREF(old_object);
                    (void)_map_result_field_to_name(result_subpkt,
                            "Deplexing Mode",
                            ValueNameDuplexingMode,
                            ARRAY_SIZE(ValueNameDuplexingMode, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_subpkt,
                            "Serving Cell Index", servingCellIndx);
                    Py_DECREF(old_object);
                    (void)_map_result_field_to_name(result_subpkt,
                            "Serving Cell Index",
                            ValueNameCellIndex,
                            ARRAY_SIZE(ValueNameCellIndex, ValueName),
                            "(MI)Unknown");

                    PyObject *result_cell = PyList_New(0);
                    for (int j = 0; j < num_cells; j++) {
                        PyObject *result_cell_item = PyList_New(0);

                        offset += _decode_by_fmt(LtePhyCncm_Subpacket_31v3_cell,
                                ARRAY_SIZE(LtePhyCncm_Subpacket_31v3_cell,
                                    Fmt),
                                b, offset, length, result_cell_item);
                        unsigned int utemp = _search_result_uint(
                                result_cell_item, "Physical Cell ID");
                        int iPhysicalCellId = utemp & 1023;  // 10 bits
                        int iFTLCFO = (utemp >> 10) & 65535; // 16 bits
                        old_object = _replace_result_int(result_cell_item,
                                "Physical Cell ID", iPhysicalCellId);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_cell_item,
                                "FTL Cumulative Freq Offset", iFTLCFO);
                        Py_DECREF(old_object);

                        temp = _search_result_int(result_cell_item,
                                "Inst Measured RSRP");
                        float RSRP = float(temp & 4095);
                        RSRP = RSRP * 0.0625 - 180.0;
                        pyfloat = Py_BuildValue("f", RSRP);
                        old_object = _replace_result(result_cell_item,
                                "Inst Measured RSRP", pyfloat);
                        Py_DECREF(old_object);
                        Py_DECREF(pyfloat);

                        utemp = _search_result_uint(result_cell_item,
                                "Inst Measured RSRQ");
                        float RSRQ = float((utemp >> 10) & 1023);
                        RSRQ = RSRQ * 0.0625 - 30.0;
                        pyfloat = Py_BuildValue("f", RSRQ);
                        old_object = _replace_result(result_cell_item,
                                "Inst Measured RSRQ", pyfloat);
                        Py_DECREF(old_object);
                        Py_DECREF(pyfloat);

                        utemp = _search_result_uint(result_cell_item,
                                "Inst Measured RSSI");
                        float RSSI = float((utemp >> 11) & 2047);
                        RSSI = RSSI * 0.0625 - 110.0;
                        pyfloat = Py_BuildValue("f", RSSI);
                        old_object = _replace_result(result_cell_item,
                                "Inst Measured RSSI", pyfloat);
                        Py_DECREF(old_object);
                        Py_DECREF(pyfloat);

                        PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                result_cell_item, "dict");
                        PyList_Append(result_cell, t3);
                        Py_DECREF(t3);
                        Py_DECREF(result_cell_item);
                    }
                    PyObject *t2 = Py_BuildValue("(sOs)", "Neighbor Cells",
                            result_cell, "list");
                    PyList_Append(result_subpkt, t2);
                    Py_DECREF(t2);
                    Py_DECREF(result_cell);

                } else if (subpkt_id == 31 && subpkt_ver == 24) {
                    // this is connected mode neighbor cell measurement
                    // response v4
                    offset += _decode_by_fmt(
                            LtePhyCncm_Subpacket_Payload_31v4,
                            ARRAY_SIZE(LtePhyCncm_Subpacket_Payload_31v4,
                                Fmt),
                            b, offset, length, result_subpkt);
                    temp = _search_result_int(result_subpkt, "Num Cells");
                    int num_cells = temp & 63;  // 6 bits
                    // skip 1 bit
                    int duplexingMode = (temp >> 7) & 3;    // 2 bits
                    int servingCellIndx = (temp >> 9) & 15; // 4 bits
                    old_object = _replace_result_int(result_subpkt,
                            "Num Cells", num_cells);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_subpkt,
                            "Deplexing Mode", duplexingMode);
                    Py_DECREF(old_object);
                    (void)_map_result_field_to_name(result_subpkt,
                            "Deplexing Mode",
                            ValueNameDuplexingMode,
                            ARRAY_SIZE(ValueNameDuplexingMode, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_subpkt,
                            "Serving Cell Index", servingCellIndx);
                    Py_DECREF(old_object);
                    (void)_map_result_field_to_name(result_subpkt,
                            "Serving Cell Index",
                            ValueNameCellIndex,
                            ARRAY_SIZE(ValueNameCellIndex, ValueName),
                            "(MI)Unknown");

                    PyObject *result_cell = PyList_New(0);
                    for (int j = 0; j < num_cells; j++) {
                        PyObject *result_cell_item = PyList_New(0);

                        offset += _decode_by_fmt(LtePhyCncm_Subpacket_31v4_cell,
                                ARRAY_SIZE(LtePhyCncm_Subpacket_31v4_cell,
                                    Fmt),
                                b, offset, length, result_cell_item);
                        unsigned int utemp = _search_result_uint(
                                result_cell_item, "Physical Cell ID");
                        int iPhysicalCellId = utemp & 1023;  // 10 bits
                        int iFTLCFO = (utemp >> 10) & 65535; // 16 bits
                        old_object = _replace_result_int(result_cell_item,
                                "Physical Cell ID", iPhysicalCellId);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_cell_item,
                                "FTL Cumulative Freq Offset", iFTLCFO);
                        Py_DECREF(old_object);

                        temp = _search_result_int(result_cell_item,
                                "Inst Measured RSRP");
                        float RSRP = float(temp & 4095);
                        RSRP = RSRP * 0.0625 - 180.0;
                        pyfloat = Py_BuildValue("f", RSRP);
                        old_object = _replace_result(result_cell_item,
                                "Inst Measured RSRP", pyfloat);
                        Py_DECREF(old_object);
                        Py_DECREF(pyfloat);

                        utemp = _search_result_uint(result_cell_item,
                                "Inst Measured RSRQ");
                        float RSRQ = float((utemp >> 10) & 1023);
                        RSRQ = RSRQ * 0.0625 - 30.0;
                        pyfloat = Py_BuildValue("f", RSRQ);
                        old_object = _replace_result(result_cell_item,
                                "Inst Measured RSRQ", pyfloat);
                        Py_DECREF(old_object);
                        Py_DECREF(pyfloat);

                        utemp = _search_result_uint(result_cell_item,
                                "Inst Measured RSSI");
                        float RSSI = float((utemp >> 11) & 2047);
                        RSSI = RSSI * 0.0625 - 110.0;
                        pyfloat = Py_BuildValue("f", RSSI);
                        old_object = _replace_result(result_cell_item,
                                "Inst Measured RSSI", pyfloat);
                        Py_DECREF(old_object);
                        Py_DECREF(pyfloat);

                        PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                result_cell_item, "dict");
                        PyList_Append(result_cell, t3);
                        Py_DECREF(t3);
                        Py_DECREF(result_cell_item);
                    }
                    PyObject *t2 = Py_BuildValue("(sOs)", "Neighbor Cells",
                            result_cell, "list");
                    PyList_Append(result_subpkt, t2);
                    Py_DECREF(t2);
                    Py_DECREF(result_cell);


                } else {
                    printf("(MI)Unknown LTE PHY Connected Neighbor Cell Meas"
                            " subpkt id and version: %d - %d\n",
                            subpkt_id, subpkt_ver);
                }

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_subpkt, "dict");
                PyList_Append(result_allpkts, t1);
                Py_DECREF(t1);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "SubPackets",
                    result_allpkts, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PHY Connected Neighbor Cell Meas version: %d\n", pkt_ver);
        return 0;
    }
}
