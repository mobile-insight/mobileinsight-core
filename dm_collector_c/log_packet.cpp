#include <Python.h>
#include <datetime.h>

#include "consts.h"
#include "log_packet.h"

#include <map>
#include <string>

// Return: i
static int
_find_result_index(PyObject *result, const char *target) {
    assert(PySequence_Check(result));
    int ret = -1;   // return -1 if fails

    Py_INCREF(result);
    int n = PySequence_Length(result);
    for (int i = 0; i < n; i++) {
        PyObject *t = PySequence_GetItem(result, i);
        PyObject *field_name = PySequence_GetItem(t, 0);
        const char *name = PyString_AsString(field_name);
        if (strcmp(name, target) == 0) {
            ret = i;
            Py_DECREF(t);
            Py_DECREF(field_name);
            break;
        } else {
            Py_DECREF(t);
            Py_DECREF(field_name);
        }
    }
    Py_DECREF(result);
    return ret;
}

// Return: New reference
static PyObject *
_search_result(PyObject *result, const char *target) {
    int i = _find_result_index(result, target);
    if (i >= 0) {
        PyObject *t = PySequence_GetItem(result, i);
        PyObject *ret = PySequence_GetItem(t, 1); // return new reference
        Py_DECREF(t);
        return ret;
    } else {
        return NULL;
    }
}

static int
_search_result_int(PyObject *result, const char *target) {
    PyObject *item = _search_result(result, target);
    assert(PyInt_Check(item));
    int val = (int) PyInt_AsLong(item);
    Py_DECREF(item);

    return val;
}

// Return: New reference to the old object
static PyObject *
_replace_result(PyObject *result, const char *target, PyObject *new_object) {
    int i = _find_result_index(result, target);
    if (i >= 0) {
        PyObject *t = PySequence_GetItem(result, i);
        PyObject *ret = PySequence_GetItem(t, 1); // return new reference
        Py_DECREF(t);

        PyList_SetItem(result, i, Py_BuildValue("(sOs)", target, new_object, ""));
        return ret;
    } else {
        return NULL;
    }
}

// Search a field that has a value of integer type, and map this integer to 
// a string, which replace the original integer.
// If there is no correponding string, or if the mapping is NULL, map this 
// integer to *not_found*.
// Return: old number
static int
_map_result_field_to_name(PyObject *result, const char *target,
                            const ValueName mapping [], int n,
                            const char *not_found) {
    int i = _find_result_index(result, target);
    if (i >= 0) {
        PyObject *t = PySequence_GetItem(result, i);
        PyObject *item = PySequence_GetItem(t, 1); // return new reference
        Py_DECREF(t);
        assert(PyInt_Check(item));
        int val = (int) PyInt_AsLong(item);
        Py_DECREF(item);

        const char* name = search_name(mapping, n, val);
        if (name == NULL)  // not found
            name = not_found;
        PyObject *pystr = Py_BuildValue("s", name);
        PyList_SetItem(result, i, Py_BuildValue("(sOs)", target, pystr, ""));
        Py_DECREF(pystr);
        return val;
    } else {
        return -1;
    }
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
                // TODO: make it little endian
                if (fmt[i].len <= 4)
                    decoded = Py_BuildValue("I", ii);
                else
                    decoded = Py_BuildValue("K", iiii);
                n_consumed += fmt[i].len;
                break;
            }

        case BYTE_STREAM:
            {
                assert(fmt[i].len > 0);
                char hex[10] = {};
                std::string ascii_data = "0x";
                for (int k = 0; k < fmt[i].len; k++) {
                    sprintf(hex, "%02x", p[k] & 0xFF);
                    ascii_data += hex;
                }
                decoded = Py_BuildValue("s", ascii_data.c_str());
                n_consumed += fmt[i].len;
                break;
            }

        case PLMN_MK1:
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

        case PLMN_MK2:
            {
                assert(fmt[i].len == 3);
                const char *plmn = p;
                decoded = PyString_FromFormat("%d%d%d-%d%d",
                                                plmn[0] & 0x0F,
                                                (plmn[0] >> 4) & 0x0F,
                                                plmn[1] & 0x0F,
                                                plmn[2] & 0x0F,
                                                (plmn[2] >> 4) & 0x0F);
                // MNC can have two or three digits
                int last_digit = (plmn[1] >> 4) & 0x0F;
                if (last_digit < 10)    // last digit exists
                    PyString_ConcatAndDel(&decoded, PyString_FromFormat("%d", last_digit));
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

        case BANDWIDTH:
            {
                assert(fmt[i].len == 1);
                unsigned int ii = *((unsigned char *) p);
                decoded = PyString_FromFormat("%d MHz", ii / 5);
                n_consumed += fmt[i].len;
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
    const char *ch_name = search_name(WcdmaSignalingMsgChannelType,
                                        ARRAY_SIZE(WcdmaSignalingMsgChannelType, ValueName),
                                        ch_num);

    if (ch_name == NULL) {  // not found
        printf("Unknown WCDMA Signalling Messages Channel Type: 0x%x\n", ch_num);
        return 0;
    }

    int pdu_length = _search_result_int(result, "Message Length");

    std::string type_str = "raw_msg/";
    type_str += ch_name;
    PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, pdu_length, type_str.c_str());
    PyList_Append(result, t);
    Py_DECREF(t);
    return pdu_length;
}

static int
_decode_umts_nas_gmm_state(const char *b, int offset, int length,
                            PyObject *result) {
    (void) _map_result_field_to_name(result,
                                        "GMM State",
                                        UmtsNasGmmState_GmmState,
                                        ARRAY_SIZE(UmtsNasGmmState_GmmState, ValueName),
                                        "Unknown");
    (void) _map_result_field_to_name(result,
                                        "GMM Substate",
                                        UmtsNasGmmState_GmmSubstate,
                                        ARRAY_SIZE(UmtsNasGmmState_GmmSubstate, ValueName),
                                        "Unknown");
    (void) _map_result_field_to_name(result,
                                        "GMM Update Status",
                                        UmtsNasGmmState_GmmUpdateStatus,
                                        ARRAY_SIZE(UmtsNasGmmState_GmmUpdateStatus, ValueName),
                                        "Unknown");
    return 0;
}

static int
_decode_umts_nas_mm_state(const char *b, int offset, int length,
                            PyObject *result) {
    (void) _map_result_field_to_name(result,
                                        "MM State",
                                        UmtsNasMmState_MmState,
                                        ARRAY_SIZE(UmtsNasMmState_MmState, ValueName),
                                        "Unknown");
    (void) _map_result_field_to_name(result,
                                        "MM Substate",
                                        UmtsNasMmState_MmSubstate,
                                        ARRAY_SIZE(UmtsNasMmState_MmSubstate, ValueName),
                                        "Unknown");
    (void) _map_result_field_to_name(result,
                                        "MM Update Status",
                                        UmtsNasMmState_MmUpdateStatus,
                                        ARRAY_SIZE(UmtsNasMmState_MmUpdateStatus, ValueName),
                                        "Unknown");
    return 0;
}

static int
_decode_lte_rrc_ota(const char *b, int offset, int length,
                    PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Pkt Version");

    switch (pkt_ver) {
    case 2:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v2,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v2, Fmt),
                                    b, offset, length, result);
        break;
    case 7:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v7,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v7, Fmt),
                                    b, offset, length, result);
        break;
    default:
        printf("Unknown LTE RRC OTA packet version: %d\n", pkt_ver);
        return 0;
    }

    int pdu_number = _search_result_int(result, "PDU Number");
    int pdu_length = _search_result_int(result, "Msg Length");
    const char *type_name = search_name(LteRrcOtaPduType,
                                        ARRAY_SIZE(LteRrcOtaPduType, ValueName),
                                        pdu_number);

    if (type_name == NULL) {    // not found
        printf("Unknown LTE RRC PDU Type: 0x%x\n", pdu_number);
        return 0;
    } else {
        std::string type_str = "raw_msg/";
        type_str += type_name;
        PyObject *t = Py_BuildValue("(ss#s)",
                                    "Msg", b + offset, pdu_length, type_str.c_str());
        PyList_Append(result, t);
        Py_DECREF(t);
        return (offset - start) + pdu_length;
    }
}

static int
_decode_lte_rrc_serv_cell_info(const char *b, int offset, int length,
                                PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 2:
        offset += _decode_by_fmt(LteRrcServCellInfoLogPacketFmt_v2,
                                    ARRAY_SIZE(LteRrcServCellInfoLogPacketFmt_v2, Fmt),
                                    b, offset, length, result);
        break;
    case 3:
        offset += _decode_by_fmt(LteRrcServCellInfoLogPacketFmt_v3,
                                    ARRAY_SIZE(LteRrcServCellInfoLogPacketFmt_v3, Fmt),
                                    b, offset, length, result);
        break;
    default:
        printf("Unknown LTE RRC Serving Cell Info packet version: %d\n", pkt_ver);
        return 0;
    }

    return offset - start;
}

static int
_decode_lte_nas_plain(const char *b, int offset, int length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Pkt Version");

    switch (pkt_ver) {
    case 1:
        offset += _decode_by_fmt(LteNasPlainFmt_v1,
                                    ARRAY_SIZE(LteNasPlainFmt_v1, Fmt),
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
_decode_lte_nas_emm_state(const char *b, int offset, int length,
                            PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 2:
        {
            offset += _decode_by_fmt(LteNasEmmStateFmt_v2,
                                        ARRAY_SIZE(LteNasEmmStateFmt_v2, Fmt),
                                        b, offset, length, result);
            int emm_state_id = _map_result_field_to_name(
                                        result,
                                        "EMM State",
                                        LteNasEmmState_v2_EmmState,
                                        ARRAY_SIZE(LteNasEmmState_v2_EmmState, ValueName),
                                        "Unknown");
            // Replace the value of "EMM Substate"
            const ValueName *table = NULL;
            int table_size = 0;
            switch (emm_state_id) {
            case 1: // EMM_DEREGISTERED
                table = LteNasEmmState_v2_EmmSubstate_Deregistered;
                table_size = ARRAY_SIZE(LteNasEmmState_v2_EmmSubstate_Deregistered, ValueName);
                break;

            case 2: // EMM_REGISTERED_INITIATED
                table = LteNasEmmState_v2_EmmSubstate_Registered_Initiated;
                table_size = ARRAY_SIZE(LteNasEmmState_v2_EmmSubstate_Registered_Initiated, ValueName);
                break;

            case 3: // EMM_REGISTERED
            case 4: // EMM_TRACKING_AREA_UPDATING_INITIATED
            case 5: // EMM_SERVICE_REQUEST_INITIATED
                table = LteNasEmmState_v2_EmmSubstate_Registered;
                table_size = ARRAY_SIZE(LteNasEmmState_v2_EmmSubstate_Registered, ValueName);
                break;

            case 0: // EMM_NULL
            case 6: // EMM_DEREGISTERED_INITIATED
            default:
                // No Substate
                break;
            }
            if (table != NULL && table_size > 0) {
                (void) _map_result_field_to_name(result, "EMM Substate", table, table_size, "Unknown");
            } else {
                PyObject *pystr = Py_BuildValue("s", "Undefined");
                PyObject *old_object = _replace_result(result, "EMM Substate", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
            }
            break;
        } // End of v2

    default:
        printf("Unknown LTE NAS EMM State version: 0x%x\n", pkt_ver);
        return 0;
    }

    return offset - start;
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
                                        ARRAY_SIZE(LteMl1CmlifmrFmt_v3_Header, Fmt),
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
                                            ARRAY_SIZE(LteMl1CmlifmrFmt_v3_Neighbor_Cell, Fmt),
                                            b, offset, length, result_cell);
                t = Py_BuildValue("(sOs)", "Ignored", result_cell, "dict");
                PyList_Append(result_allcells, t);
                Py_DECREF(t);
                Py_DECREF(result_cell);
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
                                            ARRAY_SIZE(LteMl1CmlifmrFmt_v3_Detected_Cell, Fmt),
                                            b, offset, length, result_cell);
                t = Py_BuildValue("(sOs)", "Ignored", result_cell, "dict");
                PyList_Append(result_allcells, t);
                Py_DECREF(t);
                Py_DECREF(result_cell);
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
                                            ARRAY_SIZE(LteMl1SubpktFmt_v1_SubpktHeader, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                const char *type_name = search_name(LteMl1Subpkt_SubpktType,
                                                    ARRAY_SIZE(LteMl1Subpkt_SubpktType, ValueName),
                                                    subpkt_id);
                if (type_name == NULL) {    // not found
                    printf("Unknown LTE ML1 Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    if (strcmp(type_name, "Serving_Cell_Measurement_Result") == 0) {
                        switch (subpkt_ver) {
                        case 4:
                            offset += _decode_by_fmt(LteMl1SubpktFmt_v1_Scmr_v4,
                                                    ARRAY_SIZE(LteMl1SubpktFmt_v1_Scmr_v4, Fmt),
                                                    b, offset, length, result_subpkt);
                            success = true;
                            break;
                        default:
                            break;
                        }
                    }
                    // TODO: replace type ID to name.

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
    int offset = 0;

    // Parse Header
    result = PyList_New(0);
    offset = 0;
    offset += _decode_by_fmt(LogPacketHeaderFmt, ARRAY_SIZE(LogPacketHeaderFmt, Fmt),
                                b, offset, length, result);
    PyObject *old_result = result;
    result = PyList_GetSlice(result, 2, 4);
    Py_DECREF(old_result);
    old_result = NULL;

    // Differentiate using type ID

    LogPacketType type_id = (LogPacketType) _map_result_field_to_name(
                                result,
                                "type_id",
                                LogPacketTypeID_To_Name,
                                ARRAY_SIZE(LogPacketTypeID_To_Name, ValueName),
                                "Unsupported");

    switch (type_id) {
    case WCDMA_CELL_ID:
        offset += _decode_by_fmt(WcdmaCellIdFmt,
                                    ARRAY_SIZE(WcdmaCellIdFmt, Fmt),
                                    b, offset, length, result);
        break;

    case WCDMA_Signaling_Messages:
        offset += _decode_by_fmt(WcdmaSignalingMessagesFmt,
                                    ARRAY_SIZE(WcdmaSignalingMessagesFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_wcdma_signaling_messages(b, offset, length, result);
        break;

    case UMTS_NAS_GMM_State:
        offset += _decode_by_fmt(UmtsNasGmmStateFmt,
                                    ARRAY_SIZE(UmtsNasGmmStateFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_umts_nas_gmm_state(b, offset, length, result);
        break;

    case UMTS_NAS_MM_State:
        offset += _decode_by_fmt(UmtsNasMmStateFmt,
                                    ARRAY_SIZE(UmtsNasMmStateFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_umts_nas_mm_state(b, offset, length, result);
        break;

    case LTE_RRC_OTA_Packet:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_rrc_ota(b, offset, length, result);
        break;

    case LTE_RRC_MIB_Message_Log_Packet:
        offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt,
                                    ARRAY_SIZE(LteRrcMibMessageLogPacketFmt, Fmt),
                                    b, offset, length, result);
        break;

    case LTE_RRC_Serv_Cell_Info_Log_Packet:
        offset += _decode_by_fmt(LteRrcServCellInfoLogPacketFmt,
                                    ARRAY_SIZE(LteRrcServCellInfoLogPacketFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_rrc_serv_cell_info(b, offset, length, result);
        break;

    case LTE_NAS_ESM_Plain_OTA_Incoming_Message:
    case LTE_NAS_ESM_Plain_OTA_Outgoing_Message:
    case LTE_NAS_EMM_Plain_OTA_Incoming_Message:
    case LTE_NAS_EMM_Plain_OTA_Outgoing_Message:
        offset += _decode_by_fmt(LteNasPlainFmt,
                                    ARRAY_SIZE(LteNasPlainFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_nas_plain(b, offset, length, result);
        break;

    case LTE_NAS_EMM_State:
        offset += _decode_by_fmt(LteNasEmmStateFmt,
                                    ARRAY_SIZE(LteNasEmmStateFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_nas_emm_state(b, offset, length, result);
        break;

    case LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results:
        offset += _decode_by_fmt(LteMl1CmlifmrFmt,
                                    ARRAY_SIZE(LteMl1CmlifmrFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_ml1_cmlifmr(b, offset, length, result);
        break;

    case LTE_ML1_Serving_Cell_Measurement_Result:
        offset += _decode_by_fmt(LteMl1SubpktFmt,
                                    ARRAY_SIZE(LteMl1SubpktFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_ml1_subpkt(b, offset, length, result);
        break;

    default:
        break;
    };
    return result;
}
