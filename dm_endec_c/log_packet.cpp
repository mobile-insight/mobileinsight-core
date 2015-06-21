#include <Python.h>
#include <datetime.h>

#include "consts.h"
#include "log_packet.h"

#include <map>
#include <string>

// Return: New reference
static PyObject *
_search_result(PyObject *result, const char *target) {
    assert(PySequence_Check(result));
    PyObject *ret = NULL;

    Py_INCREF(result);
    int n = PySequence_Length(result);
    for (int i = 0; i < n; i++) {
        PyObject *t = PySequence_GetItem(result, i);
        PyObject *field_name = PySequence_GetItem(t, 0);
        const char *name = PyString_AsString(field_name);
        if (strcmp(name, target) == 0) {
            Py_DECREF(t);
            Py_DECREF(field_name);
            ret = PySequence_GetItem(t, 1); // return new reference
            break;
        } else {
            Py_DECREF(t);
            Py_DECREF(field_name);
        }
    }
    Py_DECREF(result);
    return ret;
}

static int
_search_result_int(PyObject *result, const char *target) {
    PyObject *item = _search_result(result, target);
    assert(PyInt_Check(item));
    int val = (int) PyInt_AsLong(item);
    Py_DECREF(item);

    return val;
}

// Append to result
static int
_decode_by_fmt (const Fmt fmt [], int n_fmt,
                const char *b, int offset, int length,
                PyObject *result) {
    assert(PyList_Check(result));
    int n_consumed = 0;

    Py_INCREF(result);
    for (int i = 0; i < n_fmt; i++) {
        PyObject *decoded = NULL;
        const char *p = b + offset + n_consumed;
        switch (fmt[i].type) {
        case UINT:
            {
                unsigned int ii = 0;
                unsigned long long iiii = 0;
                switch (fmt[i].len) {
                case 1:
                    ii = *((unsigned char *) p);
                    break;
                case 2:
                    ii = *((unsigned short *) p);
                    break;
                case 4:
                    ii = *((unsigned int *) p);
                    break;
                case 8:
                    iiii = *((unsigned long long *) p);
                    break;
                default:
                    assert(false);
                    break;
                }
                // Convert to a Python integer object or a Python long integer object
                if (fmt[i].len <= 4)
                    decoded = Py_BuildValue("I", ii);
                else
                    decoded = Py_BuildValue("K", iiii);
                n_consumed += fmt[i].len;
                break;
            }

        case PLMN:
            {
                assert(fmt[i].len == 6);
                const char *plmn = p;
                decoded = PyString_FromFormat("%d%d%d-%d%d%d",
                                                plmn[0],
                                                plmn[1],
                                                plmn[2],
                                                plmn[3],
                                                plmn[4],
                                                plmn[5]);
                n_consumed += fmt[i].len;
                break;
            }

        case QCDM_TIMESTAMP:
            {
                const double PER_SECOND = 52428800.0;
                const double PER_USECOND = 52428800.0 / 1.0e6;
                assert(fmt[i].len == 8);
                // Convert to a Python long integer object
                unsigned long long iiii = *((unsigned long long *) p);
                int seconds = int(double(iiii) / PER_SECOND);
                int useconds = (double(iiii) / PER_USECOND) - double(seconds) * 1.0e6;
                PyObject *epoch = PyDateTime_FromDateAndTime(1980, 1, 6, 0, 0, 0, 0);
                PyObject *delta = PyDelta_FromDSU(0, seconds, useconds);
                decoded = PyNumber_Add(epoch, delta);
                n_consumed += fmt[i].len;
                Py_DECREF(epoch);
                Py_DECREF(delta);
                break;
            }

        case RSRP:
            {
                // (0.0625 * x - 180) dBm
                assert(fmt[i].len == 2);
                short val = *((short *) p);
                decoded = Py_BuildValue("f", val * 0.0625 - 180);
                n_consumed += fmt[i].len;
                break;
            }

        case RSRQ:
            {
                // (0.0625 * x - 30) dB
                assert(fmt[i].len == 2);
                short val = *((short *) p);
                decoded = Py_BuildValue("f", val * 0.0625 - 30);
                n_consumed += fmt[i].len;
                break;
            }

        case SKIP:
            n_consumed += fmt[i].len;
            break;

        default:
            assert(false);
            break;
        }

        if (decoded != NULL) {
            PyObject *t = Py_BuildValue("(sOs)",
                                        fmt[i].field_name, decoded, "");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(decoded);
        }
    }
    Py_DECREF(result);
    return n_consumed;
}

static int
_decode_wcdma_signaling_messages(const char *b, int offset, int length,
                                    PyObject *result) {
    int ch_num = _search_result_int(result, "Channel Type");

    if (WcdmaSignalingMsgChannelType.count(ch_num) == 0) {
        printf("Unknown WCDMA Signalling Messages Channel Type: 0x%x\n", ch_num);
        return 0;
    }

    int pdu_length = _search_result_int(result, "Message Length");

    std::string type_str = "raw_msg/";
    type_str += WcdmaSignalingMsgChannelType.at(ch_num);
    PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, pdu_length, type_str.c_str());
    PyList_Append(result, t);
    Py_DECREF(t);
    return pdu_length;
}

static int
_decode_lte_rrc_ota(const char *b, int offset, int length,
                    PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Pkt Version");

    switch (pkt_ver) {
    case 2:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v2,
                                    sizeof(LteRrcOtaPacketFmt_v2) / sizeof(Fmt),
                                    b, offset, length, result);
        break;
    case 7:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v7,
                                    sizeof(LteRrcOtaPacketFmt_v7) / sizeof(Fmt),
                                    b, offset, length, result);
        break;
    default:
        printf("Unknown LTE RRC OTA packet version: %d\n", pkt_ver);
        return 0;
    }

    int pdu_number = _search_result_int(result, "PDU Number");
    int pdu_length = _search_result_int(result, "Msg Length");

    if (LteRrcOtaPduType.count(pdu_number) == 0) {
        printf("Unknown LTE RRC PDU Type: 0x%x\n", pdu_number);
        return 0;
    } else {
        std::string type_str = "raw_msg/";
        type_str += LteRrcOtaPduType.at(pdu_number);
        PyObject *t = Py_BuildValue("(ss#s)",
                                    "Msg", b + offset, pdu_length, type_str.c_str());
        PyList_Append(result, t);
        Py_DECREF(t);
        return (offset - start) + pdu_length;
    }
}

static int
_decode_lte_nas_plain(const char *b, int offset, int length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Pkt Version");

    switch (pkt_ver) {
    case 1:
        offset += _decode_by_fmt(LteNasPlainFmt_v1,
                                    sizeof(LteNasPlainFmt_v1) / sizeof(Fmt),
                                    b, offset, length, result);
        break;
    default:
        printf("Unknown LTE NAS version: 0x%x\n", pkt_ver);
        return 0;
    }

    int pdu_length = length - offset;
    PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, pdu_length,
                                "raw_msg/LTE-NAS_EPS_PLAIN");
    PyList_Append(result, t);
    Py_DECREF(t);
    return length - start;
}

static int
_decode_lte_ml1_cmlifmr(const char *b, int offset, int length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 3:
        {
            offset += _decode_by_fmt(LteMl1CmlifmrFmt_v3_Header,
                                        sizeof(LteMl1CmlifmrFmt_v3_Header) / sizeof(Fmt),
                                        b, offset, length, result);
            int n_neighbor_cells = _search_result_int(result, "Number of Neighbor Cells");
            int n_detected_cells = _search_result_int(result, "Number of Detected Cells");

            PyObject *t = NULL;
            PyObject *result_allcells = NULL;
            // decode "Neighbor Cells"
            result_allcells = PyList_New(0);
            for (int i = 0; i < n_neighbor_cells; i++) {
                PyObject *result_cell = PyList_New(0);
                offset += _decode_by_fmt(LteMl1CmlifmrFmt_v3_Neighbor_Cell,
                                            sizeof(LteMl1CmlifmrFmt_v3_Neighbor_Cell) / sizeof(Fmt),
                                            b, offset, length, result_cell);
                t = Py_BuildValue("(sOs)", "Ignored", result_cell, "dict");
                PyList_Append(result_allcells, t);
                Py_DECREF(t);
            }
            t = Py_BuildValue("(sOs)", "Neighbor Cells", result_allcells, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allcells);
            
            // decode "Detected Cells"
            result_allcells = PyList_New(0);
            for (int i = 0; i < n_detected_cells; i++) {
                PyObject *result_cell = PyList_New(0);
                offset += _decode_by_fmt(LteMl1CmlifmrFmt_v3_Detected_Cell,
                                            sizeof(LteMl1CmlifmrFmt_v3_Detected_Cell) / sizeof(Fmt),
                                            b, offset, length, result_cell);
                t = Py_BuildValue("(sOs)", "Ignored", result_cell, "dict");
                PyList_Append(result_allcells, t);
                Py_DECREF(t);
            }
            t = Py_BuildValue("(sOs)", "Detected Cells", result_allcells, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allcells);

            return offset - start;
        }
    default:
        printf("Unknown LTE ML1 CMLIFMR version: 0x%x\n", pkt_ver);
        return 0;
    }
}

static int
_decode_lte_ml1_subpkt(const char *b, int offset, int length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Number of SubPackets");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMl1SubpktFmt_v1_SubpktHeader,
                                            sizeof(LteMl1SubpktFmt_v1_SubpktHeader) / sizeof(Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                if (LteMl1Subpkt_SubpktType.count(subpkt_id) == 0) {
                    printf("Unknown LTE ML1 Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    const char *type_name = LteMl1Subpkt_SubpktType.at(subpkt_id);
                    if (strcmp(type_name, "Serving_Cell_Measurement_Result") == 0) {
                        switch (subpkt_ver) {
                        case 4:
                            offset += _decode_by_fmt(LteMl1SubpktFmt_v1_Scmr_v4,
                                                    sizeof(LteMl1SubpktFmt_v1_Scmr_v4) / sizeof(Fmt),
                                                    b, offset, length, result_subpkt);
                            success = true;
                            break;
                        default:
                            break;
                        }
                    }

                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "Ignored", result_subpkt, "dict");
                        PyList_Append(result_allpkts, t);
                        Py_DECREF(result_subpkt);
                    } else {
                        printf("Unknown LTE ML1 Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                    }
                }
            }
            PyObject *t = Py_BuildValue("(sOs)",
                                        "Subpackets", result_allpkts, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("Unknown LTE ML1 packet version: 0x%x\n", pkt_ver);
        return 0;
    }
}

bool
is_log_packet (const char *b, int length) {
    return length >= 2 && b[0] == '\x10';
}

PyObject *
decode_log_packet (const char *b, int length) {
    if (PyDateTimeAPI == NULL)
        PyDateTime_IMPORT;

    PyObject *result = NULL;
    PyObject *item = NULL;
    int offset = 0;

    // Parse Header
    result = PyList_New(0);
    offset = 0;
    offset += _decode_by_fmt(LogPacketHeaderFmt, sizeof(LogPacketHeaderFmt) / sizeof(Fmt),
                                b, offset, length, result);
    PyObject *old_result = result;
    result = PyList_GetSlice(result, 2, 4);
    Py_DECREF(old_result);
    old_result = NULL;


    // Differentiate using type ID
    item = _search_result(result, "type_id");
    assert(PyInt_Check(item));
    LogPacketType type_id = (LogPacketType) PyInt_AsLong(item);
    Py_DECREF(item);
    item = NULL;

    switch (type_id) {
    case WCDMA_CELL_ID:
        offset += _decode_by_fmt(WcdmaCellIdFmt,
                                    sizeof(WcdmaCellIdFmt) / sizeof(Fmt),
                                    b, offset, length, result);
        break;

    case WCDMA_Signaling_Messages:
        offset += _decode_by_fmt(WcdmaSignalingMessagesFmt,
                                    sizeof(WcdmaSignalingMessagesFmt) / sizeof(Fmt),
                                    b, offset, length, result);
        offset += _decode_wcdma_signaling_messages(b, offset, length, result);
        break;

    case LTE_RRC_OTA_Packet:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt,
                                    sizeof(LteRrcOtaPacketFmt) / sizeof(Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_rrc_ota(b, offset, length, result);
        break;

    case LTE_RRC_MIB_Message_Log_Packet:
        offset += _decode_by_fmt(LteRrcServCellInfoLogPacketFmt,
                                    sizeof(LteRrcServCellInfoLogPacketFmt) / sizeof(Fmt),
                                    b, offset, length, result);
        break;

    case LTE_RRC_Serv_Cell_Info_Log_Packet:
        offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt,
                                    sizeof(LteRrcMibMessageLogPacketFmt) / sizeof(Fmt),
                                    b, offset, length, result);
        break;

    case LTE_NAS_ESM_Plain_OTA_Incoming_Message:
    case LTE_NAS_ESM_Plain_OTA_Outgoing_Message:
    case LTE_NAS_EMM_Plain_OTA_Incoming_Message:
    case LTE_NAS_EMM_Plain_OTA_Outgoing_Message:
        offset += _decode_by_fmt(LteNasPlainFmt,
                                    sizeof(LteNasPlainFmt) / sizeof(Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_nas_plain(b, offset, length, result);
        break;

    case LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results:
        offset += _decode_by_fmt(LteMl1CmlifmrFmt,
                                    sizeof(LteMl1CmlifmrFmt) / sizeof(Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_ml1_cmlifmr(b, offset, length, result);
        break;

    case LTE_ML1_Serving_Cell_Measurement_Result:
        offset += _decode_by_fmt(LteMl1SubpktFmt,
                                    sizeof(LteMl1SubpktFmt) / sizeof(Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_ml1_subpkt(b, offset, length, result);
        break;

    default:
        break;
    };
    return result;
}
