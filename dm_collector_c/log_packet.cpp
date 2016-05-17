/* log_packet.cpp
 * Author: Jiayao Li, Jie Zhao
 * Implements log packet message decoding.
 */

#include <Python.h>
#include <datetime.h>

#include "consts.h"
#include "log_packet.h"

#include <map>
#include <string>
#include <sstream>

#include <fstream>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

// #ifdef __ANDROID__
// #include <android/log.h>
// #define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "TAG", __VA_ARGS__);
// #endif

//Yuanjie: In-phone version cannot locate std::to_string
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

/*
 * The decoding result is represented using a Python list object (called result
 * list in the source code).
 * All elements in a result list is a 3-element tuple: (name, value, type).
 * "name" is a C string, and "value" is a reference to a Python object.
 * "type" is a C string that can be one of the following values: 
 * - "" (empty string): an ordinary field. The "value" could be a Python 
 *      integer, str, etc.
 * - "list": The "value" is another result list, and each element is of the same
 *      "type".
 * - "dict": The "value" is another result list, and each element can be of
 *      different "types".
 * - "raw_msg/*": The value is a binary string that contains raw messages.
 */

// TODO: split this .cpp to multiple files.

// Find a field by its name in a result list.
// Return: i or -1
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

// Find a field by its name in a result list.
// Return: A new reference to the field or NULL
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

// Find an integer field by its name in a result list, and return a C int with
// the same value (overflow is ignored).
// Return: int
static int
_search_result_int(PyObject *result, const char *target) {
    PyObject *item = _search_result(result, target);
    assert(PyInt_Check(item));
    int val = (int) PyInt_AsLong(item);
    Py_DECREF(item);

    return val;
}

// Find a field in a result list and replace it with a new Python object.
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

// Find a field in a result list and replace it with an integer.
// Return: New reference to the old object
static PyObject *
_replace_result_int(PyObject *result, const char *target, int new_int) {
    PyObject *pyint = Py_BuildValue("i", new_int);
    PyObject *old_object = _replace_result(result, target, pyint);
    Py_DECREF(pyint);
    return old_object;
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

// Decode a binary string according to an array of field description (fmt[]).
// Decoded fields are appended to result
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

        case WCDMA_MEAS:
            {   // (x-256) dBm
                assert(fmt[i].len == 1);
                unsigned int ii = *((unsigned char *) p);
                decoded = Py_BuildValue("i", (int)ii-256);
                n_consumed += fmt[i].len;
                break;
            }

        case SKIP:
            n_consumed += fmt[i].len;
            break;

        case PLACEHOLDER:
            {
                assert(fmt[i].len == 0);
                decoded = Py_BuildValue("I", 0);
                break;
            }

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
_decode_umts_nas_ota(const char *b, int offset, int length,
                        PyObject *result) {
    //int start = offset;
    (void) _map_result_field_to_name(result,
                                    "Message Direction",
                                    UmtsNasOtaFmt_MessageDirection,
                                    ARRAY_SIZE(UmtsNasOtaFmt_MessageDirection, ValueName),
                                    "Unknown");

    int pdu_length = _search_result_int(result, "Message Length");
    PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, pdu_length,
                                "raw_msg/NAS");
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
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v2, Fmt),
                                    b, offset, length, result);
        break;
    case 7:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v7,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v7, Fmt),
                                    b, offset, length, result);
        break;
    case 8:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v8,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v8, Fmt),
                                    b, offset, length, result);
        break;
    case 9:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v9,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v9, Fmt),
                                    b, offset, length, result);
        break;
    default:
        printf("Unknown LTE RRC OTA packet version: %d\n", pkt_ver);
        return 0;
    }

    int pdu_number = _search_result_int(result, "PDU Number");
    int pdu_length = _search_result_int(result, "Msg Length");
    if (pdu_number > 8) {   // Hack. This is not confirmed.
        pdu_number -= 7;
    }
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
_decode_lte_rrc_mib(const char *b, int offset, int length,
                    PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 1:
        offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt_v1,
                                    ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_v1, Fmt),
                                    b, offset, length, result);
        break;
    case 2:
        offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt_v2,
                                    ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_v2, Fmt),
                                    b, offset, length, result);
        break;
    default:
        printf("Unknown LTE RRC MIB version: 0x%x\n", pkt_ver);
        return 0;
    }

    return offset - start;
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
_decode_lte_nas_esm_state(const char *b, int offset, int length,
                            PyObject *result) {

    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    switch (pkt_ver) {

        case 1:{
            offset += _decode_by_fmt(LteNasEsmStateFmt_v1,
                                        ARRAY_SIZE(LteNasEsmStateFmt_v1, Fmt),
                                        b, offset, length, result);

            break;

        }

        default:
            printf("Unknown LTE NAS ESM State version: 0x%x\n", pkt_ver);
            return 0;

    }

    return offset - start;
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
_decode_lte_ll1_pdsch_demapper_config(const char *b, int offset, int length,
                                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 23:
        {
            offset += _decode_by_fmt(LteLl1PdschDemapperConfigFmt_v23,
                                        ARRAY_SIZE(LteLl1PdschDemapperConfigFmt_v23, Fmt),
                                        b, offset, length, result);

            const unsigned int SFN_RSHIFT = 5, SFN_MASK = (1 << 10) - 1;
            const unsigned int SUBFRAME_RSHIFT = 1, SUBFRAME_MASK = (1 << 4) - 1;
            int tmp = _search_result_int(result, "System Frame Number");
            int sfn = (tmp >> SFN_RSHIFT) & SFN_MASK;
            int subframe = (tmp >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
            int serv_cell = _search_result_int(result, "Serving Cell ID");
            serv_cell += (tmp & 0x1) << 8;

            PyObject *old_object = _replace_result_int(result, "Serving Cell ID", serv_cell);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "System Frame Number", sfn);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Subframe Number", subframe);
            Py_DECREF(old_object);

            // # antennas
            tmp = _search_result_int(result, "Number of Tx Antennas(M)");
            tmp = (tmp >> 8) & 0x0f;
            int M = tmp & 0x3;
            M = (M != 3? (1 << M): -1);
            int N = (tmp >> 2) & 0x1;
            N = (1 << N);

            old_object = _replace_result_int(result, "Number of Tx Antennas(M)", M);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Number of Rx Antennas(N)", N);
            Py_DECREF(old_object);

            // modulation & ratio
            tmp = _search_result_int(result, "MCS 0");
            int mod_stream0 = (tmp >> 1) & 0x3;
            float ratio = float((tmp >> 3) & 0x1fff) / 256.0;
            tmp = _search_result_int(result, "MCS 1");
            int mod_stream1 = (tmp >> 1) & 0x3;
            int carrier_index = (tmp >> 9) & 0xf;

            old_object = _replace_result_int(result, "MCS 0", mod_stream0);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "MCS 0",
                                            LteLl1PdschDemapperConfig_v23_Modulation,
                                            ARRAY_SIZE(LteLl1PdschDemapperConfig_v23_Modulation, ValueName),
                                            "Unknown");

            PyObject *pyfloat = Py_BuildValue("f", ratio);
            old_object = _replace_result(result, "Traffic to Pilot Ratio", pyfloat);
            Py_DECREF(old_object);
            Py_DECREF(pyfloat);

            old_object = _replace_result_int(result, "MCS 1", mod_stream1);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "MCS 1",
                                            LteLl1PdschDemapperConfig_v23_Modulation,
                                            ARRAY_SIZE(LteLl1PdschDemapperConfig_v23_Modulation, ValueName),
                                            "Unknown");

            // carrier index
            old_object = _replace_result_int(result, "Carrier Index", carrier_index);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "Carrier Index",
                                            LteLl1PdschDemapperConfig_v23_Carrier_Index,
                                            ARRAY_SIZE(LteLl1PdschDemapperConfig_v23_Carrier_Index, ValueName),
                                            "Unknown");
            break;
        }

    default:
        printf("Unknown LTE LL1 PDSCH Demapper Configuration version: 0x%x\n", pkt_ver);
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
    case 4:
        {
            offset += _decode_by_fmt(LteMl1CmlifmrFmt_v4_Header,
                                        ARRAY_SIZE(LteMl1CmlifmrFmt_v4_Header, Fmt),
                                        b, offset, length, result);
            int n_neighbor_cells = _search_result_int(result, "Number of Neighbor Cells");
            int n_detected_cells = _search_result_int(result, "Number of Detected Cells");

            PyObject *t = NULL;
            PyObject *result_allcells = NULL;
            // decode "Neighbor Cells"
            result_allcells = PyList_New(0);
            for (int i = 0; i < n_neighbor_cells; i++) {
                PyObject *result_cell = PyList_New(0);
                offset += _decode_by_fmt(LteMl1CmlifmrFmt_v4_Neighbor_Cell,
                                            ARRAY_SIZE(LteMl1CmlifmrFmt_v4_Neighbor_Cell, Fmt),
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
                offset += _decode_by_fmt(LteMl1CmlifmrFmt_v4_Detected_Cell,
                                            ARRAY_SIZE(LteMl1CmlifmrFmt_v4_Detected_Cell, Fmt),
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
                        // case 19:
                        //     offset += _decode_by_fmt(LteMl1SubpktFmt_v1_Scmr_v19,
                        //                             ARRAY_SIZE(LteMl1SubpktFmt_v1_Scmr_v19, Fmt),
                        //                             b, offset, length, result_subpkt);
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
                        // printf("Unknown LTE ML1 Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
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

static int
_decode_lte_ml1_irat_cdma_subpkt(const char *b, int offset, int length,
                        PyObject *result) {

    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Subpacket count");

    switch(pkt_ver){
        case 1:
            for(int i=0; i< n_subpkt; i++)
            {

                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMl1IratSubPktFmt,
                                            ARRAY_SIZE(LteMl1IratSubPktFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                int subpkt_size = _search_result_int(result_subpkt, "Subpacket size");

                PyObject *result_subpkt_cdma = PyList_New(0);
                offset += _decode_by_fmt(LteMl1IratCDMACellFmt,
                                            ARRAY_SIZE(LteMl1IratCDMACellFmt, Fmt),
                                            b, offset, length, result_subpkt_cdma);
                int n_pilot = _search_result_int(result_subpkt_cdma, "Number of Pilots");
                int band = _search_result_int(result_subpkt_cdma, "Band");
                for(int j=0; j<n_pilot; j++){
                    PyObject *result_subpkt_cdma_pilot = PyList_New(0);
                    offset += _decode_by_fmt(LteMl1IratCDMACellPilotFmt,
                                            ARRAY_SIZE(LteMl1IratCDMACellPilotFmt, Fmt),
                                            b, offset, length, result_subpkt_cdma_pilot);
                    char name[64];
                    sprintf(name,"Pilot_%d",j);
                    PyObject *t = Py_BuildValue("(sOs)",
                                            name, result_subpkt_cdma_pilot, "dict");
                    PyList_Append(result_subpkt_cdma, t);
                    Py_DECREF(result_subpkt_cdma_pilot);
                }
                char name[64];
                sprintf(name,"Band_%d",band);
                PyObject *t = Py_BuildValue("(sOs)",
                                            "CDMA", result_subpkt_cdma, "dict");
                PyList_Append(result_subpkt, t);
                Py_DECREF(result_subpkt_cdma);

                char name2[64];
                sprintf(name2,"Subpacket_%d",i);
                PyObject *t2 = Py_BuildValue("(sOs)",
                                            name2, result_subpkt, "dict");
                PyList_Append(result, t2);
                Py_DECREF(result_subpkt);
            }
            break;

        default:
            break;
    }

    return offset - start;
}


static int
_decode_lte_ml1_irat_subpkt(const char *b, int offset, int length,
                        PyObject *result) {

    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Subpacket count");

    switch(pkt_ver){
        case 1:
            for(int i=0; i< n_subpkt; i++)
            {

                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMl1IratSubPktFmt,
                                            ARRAY_SIZE(LteMl1IratSubPktFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                int subpkt_size = _search_result_int(result_subpkt, "Subpacket size");
                switch(subpkt_id){
                    case LteMl1IratType_WCDMA:
                    {
                        PyObject *result_subpkt_wcdma = PyList_New(0);
                        offset += _decode_by_fmt(LteMl1IratWCDMAFmt,
                                            ARRAY_SIZE(LteMl1IratWCDMAFmt, Fmt),
                                            b, offset, length, result_subpkt_wcdma);
                        int n_freq = _search_result_int(result_subpkt_wcdma, "Number of frequencies");
                        for(int j=0; j<n_freq; j++){
                            PyObject *result_subpkt_wcdma_freq = PyList_New(0);
                            offset += _decode_by_fmt(LteMl1IratWCDMACellMetaFmt,
                                            ARRAY_SIZE(LteMl1IratWCDMACellMetaFmt, Fmt),
                                            b, offset, length, result_subpkt_wcdma_freq);
                            int freq = _search_result_int(result_subpkt_wcdma_freq, "Frequency");
                            int n_cell = _search_result_int(result_subpkt_wcdma_freq, "Number of cells");
                            for(int k=0; k<n_cell; k++){
                                PyObject *result_subpkt_wcdma_cell = PyList_New(0);
                                offset += _decode_by_fmt(LteMl1IratWCDMACellFmt,
                                            ARRAY_SIZE(LteMl1IratWCDMACellFmt, Fmt),
                                            b, offset, length, result_subpkt_wcdma_cell);

                                char name[64];
                                sprintf(name,"CellPacket_%d",k);
                                PyObject *t = Py_BuildValue("(sOs)",
                                                    name, result_subpkt_wcdma_cell, "dict");
                                PyList_Append(result_subpkt_wcdma_freq, t);
                                Py_DECREF(result_subpkt_wcdma_cell);
                            }
                            char name[64];
                            sprintf(name,"FrequencyPacket_%d",j);
                            PyObject *t = Py_BuildValue("(sOs)",
                                                    name, result_subpkt_wcdma_freq, "dict");
                            PyList_Append(result_subpkt_wcdma, t);
                            Py_DECREF(result_subpkt_wcdma_freq);
                        }

                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "WCDMA", result_subpkt_wcdma, "dict");
                        PyList_Append(result_subpkt, t);
                        Py_DECREF(result_subpkt_wcdma);
                        break;
                    }

                    default:
                    {
                        offset += subpkt_size - 4;
                        break;
                    }
                }

                char name[64];
                sprintf(name,"Subpacket_%d",i);
                PyObject *t = Py_BuildValue("(sOs)",
                                            name, result_subpkt, "dict");
                PyList_Append(result, t);
                Py_DECREF(result_subpkt);

            }

        default:
            break;
    }

    return offset - start;
}

static int
_decode_lte_pdcp_dl_srb_integrity_data_pdu(const char *b, int offset, int length,
                                            PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 1:
        break;
    default:
        printf("Unknown LTE PDCP DL SRB Integrity Data PDU version: %d\n", pkt_ver);
        return 0;
    }
    int pktCount = _search_result_int(result, "Num SubPkt");
    if (pktCount != 1) {
        printf ("Unsupported LTE PDCP DL SRB Integrity Data PDU Num SubPkt: %d\n", pktCount);
        return 0;
    }


    int pdu_length = _search_result_int(result, "PDU Size");

    PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, pdu_length, "raw_msg/LTE-PDCP_DL_SRB");
    PyList_Append(result, t);
    Py_DECREF(t);
    return (offset - start) + pdu_length;


}

static int
_decode_lte_pdcp_ul_srb_integrity_data_pdu(const char *b, int offset, int length,
                                            PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 1:
        break;
    default:
        printf("Unknown LTE PDCP UL SRB Integrity Data PDU version: %d\n", pkt_ver);
        return 0;
    }
    int pktCount = _search_result_int(result, "Num SubPkt");
    if (pktCount != 1) {
        printf ("Unsupported LTE PDCP UL SRB Integrity Data PDU Num SubPkt: %d\n", pktCount);
        return 0;
    }


    int pdu_length = _search_result_int(result, "PDU Size");

    PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, pdu_length, "raw_msg/LTE-PDCP_UL_SRB");
    PyList_Append(result, t);
    Py_DECREF(t);
    return (offset - start) + pdu_length;
}

//------------------------------------------------------
// Jie
static int
_decode_lte_mac_configuration_subpkt(const char *b, int offset, int length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMacConfiguration_SubpktHeader,
                                            ARRAY_SIZE(LteMacConfiguration_SubpktHeader, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                const char *type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                    ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                    subpkt_id);
                (void) _map_result_field_to_name(
                                result_subpkt,
                                "SubPacket ID",
                                LteMacConfigurationSubpkt_SubpktType,
                                ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                "Unsupported");

                if (type_name == NULL) {    // not found
                    printf("Unknown LTE MAC Configuration Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    switch (subpkt_id) {
                    case 0: //Config Type Subpacket
                        offset += _decode_by_fmt(LteMacConfigurationSubpkt_ConfigType,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_ConfigType, Fmt),
                                                b, offset, length, result_subpkt);
                        (void) _map_result_field_to_name(
                                result_subpkt,
                                "Config reason",
                                LteMacConfigurationConfigType_ConfigReason,
                                ARRAY_SIZE(LteMacConfigurationConfigType_ConfigReason, ValueName),
                                "NORMAL");
                        success = true;
                        break;
                    case 1: //DL Config Subpacket
                        offset += _decode_by_fmt(LteMacConfigurationSubpkt_DLConfig,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_DLConfig, Fmt),
                                                b, offset, length, result_subpkt);
                        success = true;
                        break;
                    case 2: //UL Config Subpacket
                        offset += _decode_by_fmt(LteMacConfigurationSubpkt_ULConfig,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_ULConfig, Fmt),
                                                b, offset, length, result_subpkt);
                        success = true;
                        break;
                    case 3: //RACH Config Subpacket
                        offset += _decode_by_fmt(LteMacConfigurationSubpkt_RACHConfig,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_RACHConfig, Fmt),
                                                b, offset, length, result_subpkt);
                        success = true;
                        break;
                    case 4: //LC Config Subpacket
                        {
                            offset += _decode_by_fmt(LteMacConfigurationSubpkt_LCConfig,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_LCConfig, Fmt),
                                                b, offset, length, result_subpkt);
                            int num_LC = _search_result_int(result_subpkt, "Number of added/modified LC");
                            int start_LC = offset;

                            for (int j = 0; j < num_LC; j++) {
                                PyObject *result_subpkt_LC = PyList_New(0);
                                offset += _decode_by_fmt(LteMacConfiguration_LCConfig_LC,
                                                ARRAY_SIZE(LteMacConfiguration_LCConfig_LC, Fmt),
                                                b, offset, length, result_subpkt_LC);
                                PyObject *t = Py_BuildValue("(sOs)",
                                                    "added/modified LC", result_subpkt_LC, "dict");
                                PyList_Append(result_subpkt, t);
                                Py_DECREF(result_subpkt_LC);
                            }
                            offset += 290 - (offset-start_LC);
                            success = true;
                            break;
                        }
                    case 13: //eMBMBS Config SubPacket
                        offset += _decode_by_fmt(LteMacConfigurationSubpkt_eMBMSConfig,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_eMBMSConfig, Fmt),
                                                b, offset, length, result_subpkt);
                        success = true;
                        break;
                    default:
                        break;
                    }
                    // TODO: replace type ID to name.

                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "Ignored", result_subpkt, "dict");
                        PyList_Append(result_allpkts, t);
                        Py_DECREF(result_subpkt);
                    } else {
                        printf("Unknown LTE MAC Configuration Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
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
        printf("Unknown LTE MAC Configuration packet version: 0x%x\n", pkt_ver);
        return 0;
    }
}

//------------------------------------------------------
// TODO: Jie
static int
_decode_lte_mac_ul_transportblock_subpkt(const char *b, int offset, int length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMacULTransportBlock_SubpktHeaderFmt,
                                            ARRAY_SIZE(LteMacULTransportBlock_SubpktHeaderFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                int subpkt_nsample = _search_result_int(result_subpkt, "Num Samples");
                const char *type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                    ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                    subpkt_id);
                (void) _map_result_field_to_name(
                                result_subpkt,
                                "SubPacket ID",
                                LteMacConfigurationSubpkt_SubpktType,
                                ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                "Unsupported");

                if (type_name == NULL) {    // not found
                    printf("Unknown LTE MAC Uplink Transport Block Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    switch (subpkt_ver) {
                    case 1: // UL Transport Block Subpacket V1
                        for (int j = 0; j < subpkt_nsample; j++) {
                            PyObject *result_subpkt_sample = PyList_New(0);
                            offset += _decode_by_fmt(LteMacULTransportBlock_SubpktV1_SampleFmt,
                                    ARRAY_SIZE(LteMacULTransportBlock_SubpktV1_SampleFmt, Fmt),
                                    b, offset, length, result_subpkt_sample);
                            (void) _map_result_field_to_name(
                                    result_subpkt_sample,
                                    "BSR event",
                                    BSREvent,
                                    ARRAY_SIZE(BSREvent, ValueName),
                                    "Unsupported");
                            (void) _map_result_field_to_name(
                                    result_subpkt_sample,
                                    "BSR trig",
                                    BSRTrig,
                                    ARRAY_SIZE(BSRTrig, ValueName),
                                    "Unsupported");
                            offset += (_search_result_int(result_subpkt_sample, "HDR LEN") + 2);
                            PyObject *t = Py_BuildValue("(sOs)",
                                    "Sample", result_subpkt_sample, "dict");
                            PyList_Append(result_subpkt, t);
                            Py_DECREF(result_subpkt_sample);
                        }
                        success = true;
                        break;
                    default:
                        break;
                    }
                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "MAC Subpacket", result_subpkt, "dict");
                        PyList_Append(result_allpkts, t);
                        Py_DECREF(result_subpkt);
                    } else {
                        printf("Unknown LTE MAC Uplink Transport Block Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
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
        printf("Unknown LTE MAC Uplink Transport Block packet version: 0x%x\n", pkt_ver);
        return 0;
    }
}

//------------------------------------------------------
// Jie
static int
_decode_lte_mac_dl_transportblock_subpkt(const char *b, int offset, int length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMacDLTransportBlock_SubpktHeaderFmt,
                                            ARRAY_SIZE(LteMacDLTransportBlock_SubpktHeaderFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                int subpkt_nsample = _search_result_int(result_subpkt, "Num Samples");
                const char *type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                    ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                    subpkt_id);
                (void) _map_result_field_to_name(
                                result_subpkt,
                                "SubPacket ID",
                                LteMacConfigurationSubpkt_SubpktType,
                                ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                "Unsupported");

                if (type_name == NULL) {    // not found
                    printf("Unknown LTE MAC Downlink Transport Block Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    switch (subpkt_ver) {
                    case 2: // DL Transport Block Subpacket
                        for (int j = 0; j < subpkt_nsample; j++) {
                            PyObject *result_subpkt_sample = PyList_New(0);
                            offset += _decode_by_fmt(LteMacDLTransportBlock_SubpktV2_SampleFmt,
                                    ARRAY_SIZE(LteMacDLTransportBlock_SubpktV2_SampleFmt, Fmt),
                                    b, offset, length, result_subpkt_sample);
                            (void) _map_result_field_to_name(
                                    result_subpkt_sample,
                                    "RNTI Type",
                                    RNTIType,
                                    ARRAY_SIZE(RNTIType, ValueName),
                                    "Unsupported");
                            offset += _search_result_int(result_subpkt_sample, "HDR LEN");

                            PyObject *t = Py_BuildValue("(sOs)",
                                                        "Sample", result_subpkt_sample, "dict");
                            PyList_Append(result_subpkt, t);
                            Py_DECREF(result_subpkt_sample);
                        }
                        success = true;
                        break;
                    default:
                        break;
                    }
                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "MAC Subpacket", result_subpkt, "dict");
                        PyList_Append(result_allpkts, t);
                        Py_DECREF(result_subpkt);
                    } else {
                        printf("Unknown LTE MAC Downlink Transport Block Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
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
        printf("Unknown LTE MAC Downlink Transport Block packet version: 0x%x\n", pkt_ver);
        return 0;
    }
}

//------------------------------------------------------
// Jie
static int
_decode_lte_mac_ul_bufferstatusinternal_subpkt(const char *b, int offset, int length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMacULBufferStatusInternal_SubpktHeaderFmt,
                                            ARRAY_SIZE(LteMacDLTransportBlock_SubpktHeaderFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                int subpkt_nsample = _search_result_int(result_subpkt, "Num Samples");
                const char *type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                    ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                    subpkt_id);
                (void) _map_result_field_to_name(
                                result_subpkt,
                                "SubPacket ID",
                                LteMacConfigurationSubpkt_SubpktType,
                                ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                "Unsupported");

                if (type_name == NULL) {    // not found
                    printf("Unknown LTE MAC Uplink Buffer Status Internel Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    switch (subpkt_ver) {
                    case 3: // UL Buffer Status SubPacket
                        for (int j = 0; j < subpkt_nsample; j++) {
                            PyObject *result_subpkt_sample = PyList_New(0);
                            offset += _decode_by_fmt(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt,
                                    ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt, Fmt),
                                    b, offset, length, result_subpkt_sample);
                            int num_active_lcid = _search_result_int(result_subpkt_sample, "Number of active LCID");
                            for (int k = 0; k < num_active_lcid; k++) {
                                PyObject *result_subpkt_sample_lcid = PyList_New(0);
                                offset += _decode_by_fmt(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt,
                                        ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt, Fmt),
                                        b, offset, length, result_subpkt_sample_lcid);
                                PyObject *t = Py_BuildValue("(sOs)",
                                                        "Sample", result_subpkt_sample_lcid, "dict");
                                PyList_Append(result_subpkt_sample, t);
                                Py_DECREF(result_subpkt_sample_lcid);
                            }

                            PyObject *t = Py_BuildValue("(sOs)",
                                                        "Sample", result_subpkt_sample, "dict");
                            PyList_Append(result_subpkt, t);
                            Py_DECREF(result_subpkt_sample);
                        }
                        success = true;
                        break;
                    default:
                        break;
                    }
                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "MAC Subpacket", result_subpkt, "dict");
                        PyList_Append(result_allpkts, t);
                        Py_DECREF(result_subpkt);
                    } else {
                        printf("Unknown LTE MAC Uplink Buffer Status Internel Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
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
        printf("Unknown LTE MAC Uplink Buffer Status Internal packet version: 0x%x\n", pkt_ver);
        return 0;
    }
}

//------------------------------------------------------
// Jie
static int
_decode_lte_mac_ul_txstatistics_subpkt(const char *b, int offset, int length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMacULTxStatistics_SubpktHeaderFmt,
                                            ARRAY_SIZE(LteMacULTxStatistics_SubpktHeaderFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                const char *type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                    ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                    subpkt_id);
                (void) _map_result_field_to_name(
                                result_subpkt,
                                "SubPacket ID",
                                LteMacConfigurationSubpkt_SubpktType,
                                ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                "Unsupported");

                if (type_name == NULL) {    // not found
                    printf("Unknown LTE MAC Uplink Tx Statistics Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    switch (subpkt_ver) {
                    case 1: // UL Tx Stats SubPacket
                        {
                            PyObject *result_subpkt_sample = PyList_New(0);
                            offset += _decode_by_fmt(LteMacULTxStatistics_ULTxStatsSubPacketFmt,
                                    ARRAY_SIZE(LteMacULTxStatistics_ULTxStatsSubPacketFmt, Fmt),
                                    b, offset, length, result_subpkt_sample);

                            PyObject *t = Py_BuildValue("(sOs)",
                                                        "Sample", result_subpkt_sample, "dict");
                            PyList_Append(result_subpkt, t);
                            Py_DECREF(result_subpkt_sample);
                            success = true;
                            break;
                        }
                    default:
                        break;
                    }
                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "MAC Subpacket", result_subpkt, "dict");
                        PyList_Append(result_allpkts, t);
                        Py_DECREF(result_subpkt);
                    } else {
                        printf("Unknown LTE MAC Uplink Tx Statistics Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
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
        printf("Unknown LTE MAC Uplink Tx Statistics packet version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_rlc_ul_config_log_packet_subpkt (const char *b,
        int offset, int length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // Decode subpacket header
                offset += _decode_by_fmt(LteRlcUlConfigLogPacket_SubpktHeader,
                        ARRAY_SIZE(LteRlcUlConfigLogPacket_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 69 && subpkt_ver == 1) {
                    // 69 means lte rlc ul config log packet
                    offset += _decode_by_fmt(
                            LteRlcUlConfigLogPacket_SubpktPayload,
                            ARRAY_SIZE(LteRlcUlConfigLogPacket_SubpktPayload,
                                Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Reason",
                            LteRlcUlConfigLogPacket_Subpkt_Reason,
                            ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_Reason,
                                ValueName),
                            "Unknown");
                    int maxSizeRbs = _search_result_int(result_subpkt,
                            "Max Size RBs");

                    // Released RB Struct
                    int start_ReleasedRBStruct = offset;
                    offset += _decode_by_fmt(
                            LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Header,
                            ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ReleasedRB = _search_result_int(result_subpkt,
                            "Number of Released RBs");
                    PyObject *result_ReleasedRB = PyList_New(0);
                    for (int j = 0; j < num_ReleasedRB; j++) {
                        PyObject *result_ReleasedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Fmt,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Fmt,
                                    Fmt),
                                b, offset, length, result_ReleasedRB_item);
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_ReleasedRB_item, "dict");
                        PyList_Append(result_ReleasedRB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_ReleasedRB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "Released RBs",
                            result_ReleasedRB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_ReleasedRB);
                    offset += 1 + maxSizeRbs * 1 -
                        (offset - start_ReleasedRBStruct);

                    // Added Modified RB Struct
                    int start_AddedModifiedRBStruct = offset;
                    offset += _decode_by_fmt(LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Header,
                            ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_AddedModifiedRB = _search_result_int(result_subpkt,
                            "Number of Added/Modified RBs");
                    PyObject *result_AddedModifiedRB = PyList_New(0);
                    for (int j = 0; j < num_AddedModifiedRB; j++) {
                        PyObject *result_AddedModifiedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt,
                                    Fmt),
                                b, offset, length, result_AddedModifiedRB_item);
                        (void) _map_result_field_to_name(result_AddedModifiedRB_item,
                                "Action",
                                LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Action,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Action,
                                    ValueName),
                                "Unknown");
                        PyObject *t2 = Py_BuildValue("(sOs)", "Ignored",
                                result_AddedModifiedRB_item, "dict");
                        PyList_Append(result_AddedModifiedRB, t2);
                        Py_DECREF(t2);
                        Py_DECREF(result_AddedModifiedRB_item);
                    }
                    PyObject *t2 = Py_BuildValue("(sOs)", "Added/Modified RBs",
                            result_AddedModifiedRB, "list");
                    PyList_Append(result_subpkt, t2);
                    Py_DECREF(t2);
                    Py_DECREF(result_AddedModifiedRB);
                    offset += 1 + maxSizeRbs * 2 -
                        (offset - start_AddedModifiedRBStruct);

                    // Active RB info
                    offset += _decode_by_fmt(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Header,
                            ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ActiveRB = _search_result_int(result_subpkt,
                            "Number of Active RBs");
                    PyObject *result_ActiveRB = PyList_New(0);
                    for (int j = 0; j < num_ActiveRB; j++) {
                        PyObject *result_ActiveRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Fmt,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Fmt,
                                    Fmt),
                                b, offset, length, result_ActiveRB_item);
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB Mode",
                                LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBMode,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBMode,
                                    ValueName),
                                "Unknown");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB Type",
                                LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBType,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBType,
                                    ValueName),
                                "Unknown");
                        PyObject *pystr = Py_BuildValue("s", "RLCUL CFG");
                        PyObject *old_object = _replace_result(result_ActiveRB_item,
                                "RLCUL CFG", pystr);
                        Py_DECREF(old_object);
                        Py_DECREF(pystr);

                        old_object = _replace_result_int(result_ActiveRB_item,
                                "SN Length", 10);
                        Py_DECREF(old_object);

                        PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                result_ActiveRB_item, "dict");
                        PyList_Append(result_ActiveRB, t3);
                        Py_DECREF(t3);
                        Py_DECREF(result_ActiveRB_item);
                    }
                    PyObject *t3 = Py_BuildValue("(sOs)", "Active RBs",
                            result_ActiveRB, "list");
                    PyList_Append(result_subpkt, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_ActiveRB);
                } else {
                    printf("Unknown LTE RLC UL Config Log Packet subpkt id"
                            "and version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("Unknown LTE RLC UL Config Log Packet version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_rlc_dl_config_log_packet_subpkt (const char *b,
        int offset, int length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // Decode subpacket header
                offset += _decode_by_fmt(LteRlcDlConfigLogPacket_SubpktHeader,
                        ARRAY_SIZE(LteRlcDlConfigLogPacket_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 64 && subpkt_ver == 1) {
                    // 64 means lte rlc dl config log packet
                    offset += _decode_by_fmt(
                            LteRlcDlConfigLogPacket_SubpktPayload,
                            ARRAY_SIZE(LteRlcDlConfigLogPacket_SubpktPayload,
                                Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Reason",
                            LteRlcDlConfigLogPacket_Subpkt_Reason,
                            ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_Reason,
                                ValueName),
                            "Unknown");
                    int maxSizeRbs = _search_result_int(result_subpkt,
                            "Max Size RBs");

                    // Released RB Struct
                    int start_ReleasedRBStruct = offset;
                    offset += _decode_by_fmt(
                            LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Header,
                            ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ReleasedRB = _search_result_int(result_subpkt,
                            "Number of Released RBs");
                    PyObject *result_ReleasedRB = PyList_New(0);
                    for (int j = 0; j < num_ReleasedRB; j++) {
                        PyObject *result_ReleasedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Fmt,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Fmt,
                                    Fmt),
                                b, offset, length, result_ReleasedRB_item);
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_ReleasedRB_item, "dict");
                        PyList_Append(result_ReleasedRB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_ReleasedRB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "Released RBs",
                            result_ReleasedRB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_ReleasedRB);
                    offset += 1 + maxSizeRbs * 1 -
                        (offset - start_ReleasedRBStruct);

                    // Added Modified RB Struct
                    int start_AddedModifiedRBStruct = offset;
                    offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Header,
                            ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_AddedModifiedRB = _search_result_int(result_subpkt,
                            "Number of Added/Modified RBs");
                    PyObject *result_AddedModifiedRB = PyList_New(0);
                    for (int j = 0; j < num_AddedModifiedRB; j++) {
                        PyObject *result_AddedModifiedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt,
                                    Fmt),
                                b, offset, length, result_AddedModifiedRB_item);
                        (void) _map_result_field_to_name(result_AddedModifiedRB_item,
                                "Action",
                                LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Action,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Action,
                                    ValueName),
                                "Unknown");
                        PyObject *t2 = Py_BuildValue("(sOs)", "Ignored",
                                result_AddedModifiedRB_item, "dict");
                        PyList_Append(result_AddedModifiedRB, t2);
                        Py_DECREF(t2);
                        Py_DECREF(result_AddedModifiedRB_item);
                    }
                    PyObject *t2 = Py_BuildValue("(sOs)", "Added/Modified RBs",
                            result_AddedModifiedRB, "list");
                    PyList_Append(result_subpkt, t2);
                    Py_DECREF(t2);
                    Py_DECREF(result_AddedModifiedRB);
                    offset += 1 + maxSizeRbs * 2 -
                        (offset - start_AddedModifiedRBStruct);

                    // Active RB info
                    offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Header,
                            ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ActiveRB = _search_result_int(result_subpkt,
                            "Number of Active RBs");
                    PyObject *result_ActiveRB = PyList_New(0);
                    for (int j = 0; j < num_ActiveRB; j++) {
                        PyObject *result_ActiveRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Fmt,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Fmt,
                                    Fmt),
                                b, offset, length, result_ActiveRB_item);
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB Mode",
                                LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBMode,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBMode,
                                    ValueName),
                                "Unknown");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB Type",
                                LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBType,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBType,
                                    ValueName),
                                "Unknown");
                        PyObject *pystr = Py_BuildValue("s", "RLCDL CFG");
                        PyObject *old_object = _replace_result(result_ActiveRB_item,
                                "RLCDL CFG", pystr);
                        Py_DECREF(old_object);
                        Py_DECREF(pystr);

                        old_object = _replace_result_int(result_ActiveRB_item,
                                "SN Length", 10);
                        Py_DECREF(old_object);

                        PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                result_ActiveRB_item, "dict");
                        PyList_Append(result_ActiveRB, t3);
                        Py_DECREF(t3);
                        Py_DECREF(result_ActiveRB_item);
                    }
                    PyObject *t3 = Py_BuildValue("(sOs)", "Active RBs",
                            result_ActiveRB, "list");
                    PyList_Append(result_subpkt, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_ActiveRB);
                } else {
                    printf("Unknown LTE RLC DL Config Log Packet subpkt id"
                            "and version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("Unknown LTE RLC DL Config Log Packet version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_rlc_ul_am_all_pdu_subpkt (const char *b, int offset,
        int length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Number of Subpackets");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                offset += _decode_by_fmt(LteRlcUlAmAllPdu_SubpktHeader,
                        ARRAY_SIZE(LteRlcUlAmAllPdu_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 70 && subpkt_ver == 3) {
                    // 70 means LTE RLC UL AM All PDU
                    offset += _decode_by_fmt(LteRlcUlAmAllPdu_SubpktPayload,
                            ARRAY_SIZE(LteRlcUlAmAllPdu_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    int rb_cfg_idx = _search_result_int(result_subpkt,
                            "RB Cfg Idx");
                    (void) _map_result_field_to_name(result_subpkt, "RB Mode",
                            LteRlcUlAmAllPdu_Subpkt_RBMode,
                            ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_RBMode,
                            ValueName), "Unknown");
                    int enabled_pdu = _search_result_int(result_subpkt,
                            "Enabled PDU Log Packets");
                    // Need to check bit by bit
                    std::string strEnabledPDU = "";
                    if ((enabled_pdu) & (1 << (1)))
                        strEnabledPDU += "RLCUL Config (0xB091), ";
                    if ((enabled_pdu) & (1 << (2)))
                        strEnabledPDU += "RLCUL AM ALL PDU (0xB092), ";
                    if ((enabled_pdu) & (1 << (3)))
                        strEnabledPDU += "RLCUL AM CONTROL PDU (0xB093), ";
                    if ((enabled_pdu) & (1 << (4)))
                        strEnabledPDU += "RLCUL AM POLLING PDU (0xB094), ";
                    if ((enabled_pdu) & (1 << (5)))
                        strEnabledPDU += "RLCUL AM SIGNALING PDU (0xB095), ";
                    if ((enabled_pdu) & (1 << (6)))
                        strEnabledPDU += "RLCUL UM DATA PDU (0xB096), ";
                    if ((enabled_pdu) & (1 << (7)))
                        strEnabledPDU += "RLCUL STATISTICS (0xB097), ";
                    if ((enabled_pdu) & (1 << (8)))
                        strEnabledPDU += "RLCUL AM STATE (0xB098), ";
                    if ((enabled_pdu) & (1 << (9)))
                        strEnabledPDU += "RLCUL UM STATE (0xB099), ";
                    if (strEnabledPDU.length() > 0) {
                        strEnabledPDU = strEnabledPDU.substr(0,
                                strEnabledPDU.length() - 2);
                        strEnabledPDU += ".";
                    }
                    PyObject *pystr = Py_BuildValue("s",
                            strEnabledPDU.c_str());
                    PyObject *old_object = _replace_result(result_subpkt,
                            "Enabled PDU Log Packets", pystr);
                    Py_DECREF(old_object);
                    Py_DECREF(pystr);

                    int n_pdu = _search_result_int(result_subpkt,
                            "Number of PDUs");
                    PyObject *result_pdu = PyList_New(0);
                    for (int j = 0; j < n_pdu; j++) {
                        PyObject *result_pdu_item = PyList_New(0);
                        offset += _decode_by_fmt(
                                LteRlcUlAmAllPdu_Subpkt_PDU_Basic, ARRAY_SIZE(
                                    LteRlcUlAmAllPdu_Subpkt_PDU_Basic, Fmt),
                                b, offset, length, result_pdu_item);
                        // Update rb_cfg_idx from previous content
                        old_object = _replace_result_int(result_pdu_item,
                                "rb_cfg_idx", rb_cfg_idx);
                        Py_DECREF(old_object);
                        int DCLookAhead = _search_result_int(result_pdu_item,
                                "D/C LookAhead");
                        int iNonDecodeFN = _search_result_int(result_pdu_item,
                                "sys_fn");
                        int iLoggedBytes = _search_result_int(result_pdu_item,
                                "logged_bytes");
                        // Handle fn
                        const unsigned int SFN_RSHIFT = 4,
                              SFN_MASK = (1 << 12) - 1;
                        const unsigned int SUBFRAME_RSHIFT = 0,
                              SUBFRAME_MASK = (1 << 4) - 1;
                        int sys_fn = (iNonDecodeFN >> SFN_RSHIFT) & SFN_MASK;
                        int sub_fn =
                            (iNonDecodeFN >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
                        old_object = _replace_result_int(result_pdu_item,
                                "sys_fn", sys_fn);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_pdu_item,
                                "sub_fn", sub_fn);
                        Py_DECREF(old_object);
                        // Decide PDU type
                        int iNonDecodeSN = _search_result_int(result_pdu_item, "SN");
                        if (DCLookAhead < 16) {
                            // Type = Control
                            pystr = Py_BuildValue("s", "RLCUL CTRL");
                            old_object = _replace_result(result_pdu_item,
                                    "PDU TYPE", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Modify ACK_SN
                            std::string strAckSN = "ACK_SN = ";
                            int iAckSN = DCLookAhead * 64 + iNonDecodeSN/4;
                            strAckSN += SSTR(iAckSN);
                            pystr = Py_BuildValue("s", strAckSN.c_str());
                            old_object = _replace_result(result_pdu_item,
                                    "SN", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Update other info
                            offset += _decode_by_fmt(
                                    LteRlcUlAmAllPdu_Subpkt_PDU_Control,
                                    ARRAY_SIZE(
                                        LteRlcUlAmAllPdu_Subpkt_PDU_Control,
                                        Fmt),
                                    b, offset, length, result_pdu_item);
                            pystr = Py_BuildValue("s", "STATUS(0)");
                            old_object = _replace_result(result_pdu_item,
                                    "cpt", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                        } else {
                            // Type = DATA
                            pystr = Py_BuildValue("s", "RLCUL DATA");
                            old_object = _replace_result(result_pdu_item,
                                    "PDU TYPE", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Update other info
                            std::string strRF = "", strP = "", strFI = "",
                                strE = "";
                            if ((DCLookAhead) & (1 << (6))) {
                                strRF += "1";
                            } else {
                                strRF += "0";
                            }
                            if ((DCLookAhead) & (1 << (5))) {
                                strP += "1";
                            } else {
                                strP += "0";
                            }
                            if ((DCLookAhead) & (1 << (4))) {
                                strFI += "1";
                            } else {
                                strFI += "0";
                            }
                            if ((DCLookAhead) & (1 << (3))) {
                                strFI += "1";
                            } else {
                                strFI += "0";
                            }
                            if ((DCLookAhead) & (1 << (2))) {
                                strE += "1";
                            } else {
                                strE += "0";
                            }
                            // update SN (need to check last two bits in
                            // DCLookAhead)
                            if ((DCLookAhead) & (1 << (1))) {
                                iNonDecodeSN += 512;
                            }
                            if ((DCLookAhead) & (1)) {
                                iNonDecodeSN += 256;
                            }
                            old_object = _replace_result_int(result_pdu_item,
                                    "SN", iNonDecodeSN);
                            Py_DECREF(old_object);

                            offset += _decode_by_fmt(
                                    LteRlcUlAmAllPdu_Subpkt_PDU_DATA,
                                    ARRAY_SIZE(
                                        LteRlcUlAmAllPdu_Subpkt_PDU_DATA, Fmt),
                                    b, offset, length, result_pdu_item);
                            pystr = Py_BuildValue("s", (strRF.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "RF", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strP.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "P", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strFI.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "FI", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strE.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "E", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);

                            if (strE == "1") {
                                // Decode extra two bytes
                                offset += _decode_by_fmt(
                                        LteRlcUlAmAllPdu_Subpkt_PDU_Extra,
                                        ARRAY_SIZE(
                                            LteRlcUlAmAllPdu_Subpkt_PDU_Extra,
                                        Fmt),
                                        b, offset, length, result_pdu_item);
                                int iNonDecodeLI = _search_result_int(
                                        result_pdu_item, "RLC DATA LI");
                                int iLeft1 = iNonDecodeLI / 4096;
                                iNonDecodeLI -= iLeft1 * 4096;
                                int iLeft2 = iNonDecodeLI / 256;
                                iNonDecodeLI -= iLeft2 * 256;
                                int iLeft3 = iNonDecodeLI / 16;
                                iNonDecodeLI -= iLeft3 * 16;
                                int iLeft4 = iNonDecodeLI;
                                int iLI =
                                    iLeft1 * 1 + iLeft3 * 256 + iLeft4 * 16;
                                old_object = _replace_result_int(
                                        result_pdu_item, "RLC DATA LI", iLI);
                                Py_DECREF(old_object);
                                iLoggedBytes = iLoggedBytes - 2;
                            }
                        }
                        PyObject *t1 = Py_BuildValue("(sOs)", ("RLCUL PDU["
                                + SSTR(i) + "]").c_str(), result_pdu_item,
                                "dict");
                        PyList_Append(result_pdu, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_pdu_item);
                        offset += iLoggedBytes - 2;
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "RLCUL PDUs",
                            result_pdu, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_pdu);
                } else {
                    printf("Unkown LTE RLC UL AM ALL PDU subpkt id and version"
                            ": 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("Unkown LTE RLC UL AM ALL PDU version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_rlc_dl_am_all_pdu_subpkt (const char *b, int offset,
        int length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Number of Subpackets");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                offset += _decode_by_fmt(LteRlcDlAmAllPdu_SubpktHeader,
                        ARRAY_SIZE(LteRlcDlAmAllPdu_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 65 && subpkt_ver == 3) {
                    // 65 means LTE RLC DL AM All PDU
                    offset += _decode_by_fmt(LteRlcDlAmAllPdu_SubpktPayload,
                            ARRAY_SIZE(LteRlcDlAmAllPdu_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    int rb_cfg_idx = _search_result_int(result_subpkt,
                            "RB Cfg Idx");
                    (void) _map_result_field_to_name(result_subpkt, "RB Mode",
                            LteRlcDlAmAllPdu_Subpkt_RBMode,
                            ARRAY_SIZE(LteRlcDlAmAllPdu_Subpkt_RBMode,
                            ValueName), "Unknown");
                    int enabled_pdu = _search_result_int(result_subpkt,
                            "Enabled PDU Log Packets");
                    // Need to check bit by bit
                    std::string strEnabledPDU = "";
                    if ((enabled_pdu) & (1 << (1)))
                        strEnabledPDU += "RLCDL Config (0xB081), ";
                    if ((enabled_pdu) & (1 << (2)))
                        strEnabledPDU += "RLCDL AM ALL PDU (0xB082), ";
                    if ((enabled_pdu) & (1 << (3)))
                        strEnabledPDU += "RLCDL AM CONTROL PDU (0xB083), ";
                    if ((enabled_pdu) & (1 << (4)))
                        strEnabledPDU += "RLCDL AM POLLING PDU (0xB084), ";
                    if ((enabled_pdu) & (1 << (5)))
                        strEnabledPDU += "RLCDL AM SIGNALING PDU (0xB085), ";
                    if ((enabled_pdu) & (1 << (6)))
                        strEnabledPDU += "RLCDL UM DATA PDU (0xB086), ";
                    if ((enabled_pdu) & (1 << (7)))
                        strEnabledPDU += "RLCDL STATISTICS (0xB087), ";
                    if ((enabled_pdu) & (1 << (8)))
                        strEnabledPDU += "RLCDL AM STATE (0xB088), ";
                    if ((enabled_pdu) & (1 << (9)))
                        strEnabledPDU += "RLCDL UM STATE (0xB089), ";
                    if (strEnabledPDU.length() > 0) {
                        strEnabledPDU = strEnabledPDU.substr(0,
                                strEnabledPDU.length() - 2);
                        strEnabledPDU += ".";
                    }
                    PyObject *pystr = Py_BuildValue("s",
                            strEnabledPDU.c_str());
                    PyObject *old_object = _replace_result(result_subpkt,
                            "Enabled PDU Log Packets", pystr);
                    Py_DECREF(old_object);
                    Py_DECREF(pystr);

                    int n_pdu = _search_result_int(result_subpkt,
                            "Number of PDUs");
                    PyObject *result_pdu = PyList_New(0);
                    for (int j = 0; j < n_pdu; j++) {
                        PyObject *result_pdu_item = PyList_New(0);
                        offset += _decode_by_fmt(
                                LteRlcDlAmAllPdu_Subpkt_PDU_Basic, ARRAY_SIZE(
                                    LteRlcDlAmAllPdu_Subpkt_PDU_Basic, Fmt),
                                b, offset, length, result_pdu_item);
                        // Update rb_cfg_idx from previous content
                        old_object = _replace_result_int(result_pdu_item,
                                "rb_cfg_idx", rb_cfg_idx);
                        Py_DECREF(old_object);
                        int DCLookAhead = _search_result_int(result_pdu_item,
                                "D/C LookAhead");
                        int iNonDecodeFN = _search_result_int(result_pdu_item,
                                "sys_fn");
                        int iLoggedBytes = _search_result_int(result_pdu_item,
                                "logged_bytes");
                        // Handle fn
                        const unsigned int SFN_RSHIFT = 4,
                              SFN_MASK = (1 << 12) - 1;
                        const unsigned int SUBFRAME_RSHIFT = 0,
                              SUBFRAME_MASK = (1 << 4) - 1;
                        int sys_fn = (iNonDecodeFN >> SFN_RSHIFT) & SFN_MASK;
                        int sub_fn =
                            (iNonDecodeFN >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
                        old_object = _replace_result_int(result_pdu_item,
                                "sys_fn", sys_fn);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_pdu_item,
                                "sub_fn", sub_fn);
                        Py_DECREF(old_object);
                        // Decide PDU type
                        int iNonDecodeSN = _search_result_int(result_pdu_item, "SN");
                        if (DCLookAhead < 16) {
                            // Type = Control
                            pystr = Py_BuildValue("s", "RLCDL CTRL");
                            old_object = _replace_result(result_pdu_item,
                                    "PDU TYPE", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", "PDU CTRL");
                            old_object = _replace_result(result_pdu_item,
                                    "Status", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Modify ACK_SN
                            std::string strAckSN = "ACK_SN = ";
                            int iAckSN = DCLookAhead * 64 + iNonDecodeSN/4;
                            strAckSN += SSTR(iAckSN);
                            pystr = Py_BuildValue("s", strAckSN.c_str());
                            old_object = _replace_result(result_pdu_item,
                                    "SN", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Update other info
                            offset += _decode_by_fmt(
                                    LteRlcDlAmAllPdu_Subpkt_PDU_Control,
                                    ARRAY_SIZE(
                                        LteRlcDlAmAllPdu_Subpkt_PDU_Control,
                                        Fmt),
                                    b, offset, length, result_pdu_item);
                            pystr = Py_BuildValue("s", "STATUS(0)");
                            old_object = _replace_result(result_pdu_item,
                                    "cpt", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                        } else {
                            // Type = DATA
                            pystr = Py_BuildValue("s", "RLCDL DATA");
                            old_object = _replace_result(result_pdu_item,
                                    "PDU TYPE", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", "PDU DATA");
                            old_object = _replace_result(result_pdu_item,
                                    "Status", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Update other info
                            std::string strRF = "", strP = "", strFI = "",
                                strE = "";
                            if ((DCLookAhead) & (1 << (6))) {
                                strRF += "1";
                            } else {
                                strRF += "0";
                            }
                            if ((DCLookAhead) & (1 << (5))) {
                                strP += "1";
                            } else {
                                strP += "0";
                            }
                            if ((DCLookAhead) & (1 << (4))) {
                                strFI += "1";
                            } else {
                                strFI += "0";
                            }
                            if ((DCLookAhead) & (1 << (3))) {
                                strFI += "1";
                            } else {
                                strFI += "0";
                            }
                            if ((DCLookAhead) & (1 << (2))) {
                                strE += "1";
                            } else {
                                strE += "0";
                            }
                            // update SN (need to check last two bits in
                            // DCLookAhead)
                            if ((DCLookAhead) & (1 << (1))) {
                                iNonDecodeSN += 512;
                            }
                            if ((DCLookAhead) & (1)) {
                                iNonDecodeSN += 256;
                            }
                            old_object = _replace_result_int(result_pdu_item,
                                    "SN", iNonDecodeSN);
                            Py_DECREF(old_object);
                            offset += _decode_by_fmt(
                                    LteRlcDlAmAllPdu_Subpkt_PDU_DATA,
                                    ARRAY_SIZE(
                                        LteRlcDlAmAllPdu_Subpkt_PDU_DATA, Fmt),
                                    b, offset, length, result_pdu_item);
                            pystr = Py_BuildValue("s", (strRF.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "RF", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strP.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "P", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strFI.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "FI", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strE.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "E", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);

                            if (strE == "1") {
                                // Decode extra two bytes
                                offset += _decode_by_fmt(
                                        LteRlcDlAmAllPdu_Subpkt_PDU_Extra,
                                        ARRAY_SIZE(
                                            LteRlcDlAmAllPdu_Subpkt_PDU_Extra,
                                        Fmt),
                                        b, offset, length, result_pdu_item);
                                int iNonDecodeLI = _search_result_int(
                                        result_pdu_item, "RLC DATA LI");
                                int iLeft1 = iNonDecodeLI / 4096;
                                iNonDecodeLI -= iLeft1 * 4096;
                                int iLeft2 = iNonDecodeLI / 256;
                                iNonDecodeLI -= iLeft2 * 256;
                                int iLeft3 = iNonDecodeLI / 16;
                                iNonDecodeLI -= iLeft3 * 16;
                                int iLeft4 = iNonDecodeLI;
                                int iLI =
                                    iLeft1 * 1 + iLeft3 * 256 + iLeft4 * 16;
                                old_object = _replace_result_int(
                                        result_pdu_item, "RLC DATA LI", iLI);
                                Py_DECREF(old_object);
                                iLoggedBytes = iLoggedBytes - 2;
                            }
                        }
                        offset += iLoggedBytes - 2;
                        PyObject *t1 = Py_BuildValue("(sOs)", ("RLCDL PDU["
                                + SSTR(i) + "]").c_str(), result_pdu_item,
                                "dict");
                        PyList_Append(result_pdu, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_pdu_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "RLCDL PDUs",
                            result_pdu, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_pdu);
                } else {
                    printf("Unkown LTE RLC DL AM ALL PDU subpkt id and version"
                            ": 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("Unkown LTE RLC DL AM ALL PDU version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------

static int _decode_lte_mac_rach_trigger_subpkt (const char *b, int offset,
        int length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Number of Subpackets");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                offset += _decode_by_fmt(LteMacRachTrigger_SubpktHeader,
                        ARRAY_SIZE(LteMacRachTrigger_SubpktHeader, Fmt), b,
                        offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 3 && subpkt_ver == 2) {
                    // RACH Config Subpacket
                    offset += _decode_by_fmt(
                            LteMacRachTrigger_RachConfigSubpktPayload,
                            ARRAY_SIZE(LteMacRachTrigger_RachConfigSubpktPayload,
                                Fmt),
                            b, offset, length, result_subpkt);
                    PyObject *old_object = _replace_result_int(result_subpkt,
                            "Preamble Format", 0);
                    Py_DECREF(old_object);
                } else if (subpkt_id == 5 && subpkt_ver == 1) {
                    offset += _decode_by_fmt(
                            LteMacRachTrigger_RachReasonSubpktPayload,
                            ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpktPayload,
                                Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Rach reason",
                            LteMacRachTrigger_RachReasonSubpkt_RachReason,
                            ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpkt_RachReason,
                                ValueName),
                            "Unknown");
                    std::string strRachContention = "Contention Based RACH procedure";
                    PyObject *pystr = Py_BuildValue("s", strRachContention.c_str());
                    PyObject *old_object = _replace_result(result_subpkt,
                            "RACH Contention", pystr);
                    Py_DECREF(old_object);
                    Py_DECREF(pystr);
                } else {
                    printf("Unknown LTE MAC RACH Trigger subpkt id and "
                            "version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("Unknown LTE MAC RACH Trigger Packet Version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------

static int _decode_lte_mac_rach_attempt_subpkt (const char *b, int offset,
        int length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Number of Subpackets");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                offset += _decode_by_fmt(LteMacRachAttempt_SubpktHeader,
                        ARRAY_SIZE(LteMacRachAttempt_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 6 && subpkt_ver == 2) {
                    offset += _decode_by_fmt(LteMacRachAttempt_SubpktPayload,
                            ARRAY_SIZE(LteMacRachAttempt_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    int iRachMsgBMasks = _search_result_int(result_subpkt,
                            "Rach msg bmasks");
                    (void) _map_result_field_to_name(result_subpkt,
                            "Rach result",
                            LteMacRachAttempt_Subpkt_RachResult,
                            ARRAY_SIZE(LteMacRachAttempt_Subpkt_RachResult,
                                ValueName),
                            "Unsuccess");
                    (void) _map_result_field_to_name(result_subpkt,
                            "Contention procedure",
                            LteMacRachAttempt_Subpkt_ContentionProcedure,
                            ARRAY_SIZE(LteMacRachAttempt_Subpkt_ContentionProcedure,
                                ValueName),
                            "Unknown");
                    if ((iRachMsgBMasks) & (1 << 2)) {
                        // Msg1
                        PyObject *result_subpkt_msg1 = PyList_New(0);
                        offset += _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg1,
                                ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg1, Fmt),
                                b, offset, length, result_subpkt_msg1);
                        PyObject *t1 = Py_BuildValue("(sOs)", "Msg1",
                                result_subpkt_msg1, "dict");
                        PyList_Append(result_subpkt, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_subpkt_msg1);
                    }
                    if ((iRachMsgBMasks) & (1 << 1)) {
                        // Msg2
                        PyObject *result_subpkt_msg2 = PyList_New(0);
                        offset += _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg2,
                                ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg2, Fmt),
                                b, offset, length, result_subpkt_msg2);
                        (void) _map_result_field_to_name(result_subpkt_msg2,
                                "Result",
                                LteMacRachAttempt_Subpkt_Msg2_Result,
                                ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg2_Result,
                                    ValueName),
                                "False");
                        PyObject *t1 = Py_BuildValue("(sOs)", "Msg2",
                                result_subpkt_msg2, "dict");
                        PyList_Append(result_subpkt, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_subpkt_msg2);
                    }
                    if ((iRachMsgBMasks) & (1)) {
                        // Msg3
                        PyObject *result_subpkt_msg3 = PyList_New(0);
                        offset += _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg3,
                                ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg3, Fmt),
                                b, offset, length, result_subpkt_msg3);
                        int iGrantBytes = _search_result_int(result_subpkt_msg3,
                                "Grant");
                        PyObject *result_MACPDUs = PyList_New(0);
                        for (int j = 0; j < iGrantBytes + 1; j++) {
                            PyObject *result_MACPDU_item = PyList_New(0);
                            offset += _decode_by_fmt(
                                    LteMacRachAttempt_Subpkt_Msg3_MACPDU,
                                    ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg3_MACPDU,
                                        Fmt),
                                    b, offset, length, result_MACPDU_item);
                            PyObject *t2 = Py_BuildValue("(sOs)", "Ignored",
                                    result_MACPDU_item, "dict");
                            PyList_Append(result_MACPDUs, t2);
                            Py_DECREF(t2);
                            Py_DECREF(result_MACPDU_item);
                        }
                        // add pdu list
                        PyObject *t2 = Py_BuildValue("(sOs)", "MAC PDUs",
                                result_MACPDUs, "list");
                        PyList_Append(result_subpkt_msg3, t2);
                        Py_DECREF(t2);
                        Py_DECREF(result_MACPDUs);
                        // add Msg3 dict
                        PyObject *t1 = Py_BuildValue("(sOs)", "Msg3",
                                result_subpkt_msg3, "dict");
                        PyList_Append(result_subpkt, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_subpkt_msg3);
                    }

                } else {
                    printf("Unknown LTE MAC RACH Attempt Subpkt id and Version"
                            ": 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored",
                        result_subpkt, "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("Unknown LTE MAC RACH Attempt Packet Version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_pdcp_dl_config_subpkt (const char *b, int offset,
        int length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num Subpkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LtePdcpDlConfig_SubpktHeader,
                        ARRAY_SIZE(LtePdcpDlConfig_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 192 && subpkt_ver == 2) {
                    // PDCP DL Config 0xC0
                    offset += _decode_by_fmt(
                            LtePdcpDlConfig_SubpktPayload,
                            ARRAY_SIZE(LtePdcpDlConfig_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Reason",
                            LtePdcpDlConfig_Subpkt_Reason,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_Reason, ValueName),
                            "Unknown");
                    (void) _map_result_field_to_name(result_subpkt, "SRB Cipher Algorithm",
                            LtePdcpDlConfig_Subpkt_CipherAlgo,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_CipherAlgo, ValueName),
                            "Unknown");
                    (void) _map_result_field_to_name(result_subpkt, "DRB Cipher Algorithm",
                            LtePdcpDlConfig_Subpkt_CipherAlgo,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_CipherAlgo, ValueName),
                            "Unknown");
                    (void) _map_result_field_to_name(result_subpkt, "SRB Integrity Algorithm",
                            LtePdcpDlConfig_Subpkt_IntegAlgo,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_IntegAlgo, ValueName),
                            "Unknown");
                    int iArraySize = _search_result_int(result_subpkt,
                            "Array size");

                    // Released RB
                    int start_ReleasedRBStruct = offset;
                    offset += _decode_by_fmt(
                            LtePdcpDlConfig_Subpkt_ReleaseRB_Header,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ReleaseRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ReleasedRB = _search_result_int(result_subpkt,
                            "Number of Released RBs");
                    PyObject *result_ReleasedRB = PyList_New(0);
                    for (int j = 0; j < num_ReleasedRB; j++) {
                        PyObject *result_ReleasedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpDlConfig_Subpkt_ReleaseRB_Fmt,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ReleaseRB_Fmt, Fmt),
                                b, offset, length, result_ReleasedRB_item);
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_ReleasedRB_item, "dict");
                        PyList_Append(result_ReleasedRB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_ReleasedRB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "Released RBs",
                            result_ReleasedRB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_ReleasedRB);
                    offset += 1 + iArraySize * 1 -
                        (offset - start_ReleasedRBStruct);

                    // Added/Modified RB
                    int start_AddedModifiedRBStruct = offset;
                    offset += _decode_by_fmt(LtePdcpDlConfig_Subpkt_AddedModifiedRB_Header,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_AddedModifiedRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_AddedModifiedRB = _search_result_int(result_subpkt,
                            "Number of Added/Modified RBs");
                    PyObject *result_AddedModifiedRB = PyList_New(0);
                    for (int j = 0; j < num_AddedModifiedRB; j++) {
                        PyObject *result_AddedModifiedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpDlConfig_Subpkt_AddedModifiedRB_Fmt,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_AddedModifiedRB_Fmt, Fmt),
                                b, offset, length, result_AddedModifiedRB_item);
                        (void) _map_result_field_to_name(result_AddedModifiedRB_item,
                                "Action",
                                LtePdcpDlConfig_Subpkt_AddedModifiedRB_Action,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_AddedModifiedRB_Action,
                                    ValueName),
                                "Unknown");
                        PyObject *t2 = Py_BuildValue("(sOs)", "Ignored",
                                result_AddedModifiedRB_item, "dict");
                        PyList_Append(result_AddedModifiedRB, t2);
                        Py_DECREF(t2);
                        Py_DECREF(result_AddedModifiedRB_item);
                    }
                    PyObject *t2 = Py_BuildValue("(sOs)", "Added/Modified RBs",
                            result_AddedModifiedRB, "list");
                    PyList_Append(result_subpkt, t2);
                    Py_DECREF(t2);
                    Py_DECREF(result_AddedModifiedRB);
                    offset += 1 + iArraySize * 2 - (offset - start_AddedModifiedRBStruct);

                    // Active RB
                    int start_ActiveRBStruct = offset;
                    offset += _decode_by_fmt(LtePdcpDlConfig_Subpkt_ActiveRB_Header,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ActiveRB = _search_result_int(result_subpkt,
                            "Number of active RBs");
                    PyObject *result_ActiveRB = PyList_New(0);
                    for (int j = 0; j < num_ActiveRB; j++) {
                        PyObject *result_ActiveRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpDlConfig_Subpkt_ActiveRB_Fmt,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_Fmt, Fmt),
                                b, offset, length, result_ActiveRB_item);
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB mode",
                                LtePdcpDlConfig_Subpkt_ActiveRB_RBmode,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_RBmode,
                                    ValueName),
                                "Unknown");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB type",
                                LtePdcpDlConfig_Subpkt_ActiveRB_RBtype,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_RBtype,
                                    ValueName),
                                "Unknown");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "Status report",
                                LtePdcpDlConfig_Subpkt_ActiveRB_StatusReport,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_StatusReport,
                                    ValueName),
                                "YES");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RoHC Enabled",
                                LtePdcpDlConfig_Subpkt_ActiveRB_RoHCEnabled,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_RoHCEnabled,
                                    ValueName),
                                "true");
                        PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                result_ActiveRB_item, "dict");
                        PyList_Append(result_ActiveRB, t3);
                        Py_DECREF(t3);
                        Py_DECREF(result_ActiveRB_item);
                    }
                    PyObject *t3 = Py_BuildValue("(sOs)", "Active RBs",
                            result_ActiveRB, "list");
                    PyList_Append(result_subpkt, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_ActiveRB);
                } else {
                    printf("Unknown LTE PDCP DL Config subpkt id and version:"
                            " 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("Unknown LTE PDCP DL Config Log Packet version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}


// ----------------------------------------------------------------------------

//Yuanjie: decode modem's internal debugging message

static int
_decode_modem_debug_msg(const char *b, int offset, int length,
                                PyObject *result) {

    int start = offset;
    int argc = _search_result_int(result, "Number of parameters");
    char version = _search_result_int(result, "Version");

    if(version=='\x79'){
        //Yuanjie: optimization for iCellular. Don't show unnecessary logs
        return length-start;
    }
    else if(version=='\x92'){

        //Yuanjie: these logs are encoded in unknown approach (signature based?)
        //Currently only ad-hoc approach is available

        // Yuanjie: for this type of debugging message, a "fingerprint" exists before the parameters
        // The fingerprint seems the **unique** identifier of one message.
        // The message is pre-stored (thus not transferred) as a database.
        //
        argc++;
        int *tmp_argv = new int[argc];

        //Get parameters
        for(int i=0; i!=argc; i++)
        {
            const char *p = b + offset;

            int ii = *((int *) p);    //a new parameter
            tmp_argv[i] = ii;

            offset += 4;    //one parameter
        }


        // if(argc>=2)
        //     printf("argc=%d argv[0]=%x argv[1]=%d\n", argc, tmp_argv[0], tmp_argv[1]);

        if(argc==2 && tmp_argv[0]==0x2e3bbbed)
        {

             // Yuanjie: for icellular only, 4G RSRP result in manual network search
             // get log "BPLMN LOG: Saved measurement results. rsrp=-121"
             // The fingerprint is tmp_argv[0]==0xedbb3b2e, tmp_argv[1]=rsrp value
             //
            std::string res = "BPLMN LOG: Saved measurement results. rsrp=";
            // res+=std::to_string(tmp_argv[1]);
            res+=patch::to_string(tmp_argv[1]);
            PyObject *t = Py_BuildValue("(ss#s)", "Msg", res.c_str(), res.size(), "");
            PyList_Append(result, t);
            Py_DECREF(t);
            return length-start;
        }
        else if(argc==10 && tmp_argv[0]==0x81700a47)
        {

             // Yuanjie: for icellular only, 3G RSRP result in manual network search
             // The fingerprint is tmp_argv[0]==0x81700a47, tmp_argv[1]=rscp value
             //
            std::string res = "Freq=%d psc=%d eng=%d filt_eng=%d rscp=%d RxAGC=%d 2*ecio=%d 2*squal=%d srxlv=%d";
            for(int i=1; i!=argc; i++)
            {

                std::size_t found = res.find("%d");
                // std::string tmp = std::to_string(tmp_argv[i]);
                std::string tmp = patch::to_string(tmp_argv[i]);
                res.replace(found,2,tmp);
            }
            PyObject *t = Py_BuildValue("(ss#s)", "Msg", res.c_str(), res.size(), "");
            PyList_Append(result, t);
            Py_DECREF(t);
            return length-start;

        }
        else{

            //Ignore other unknown messages. Don't send to MobileInsight Analyzers
            // std::string res="(Unknown debug message)";
            // PyObject *t = Py_BuildValue("(ss#s)", "Msg", res.c_str(), res.size(), "");
            // PyList_Append(result, t);
            // Py_DECREF(t);
            return length-start;

        }


    }
    return length-start;

}



bool
is_log_packet (const char *b, int length) {
    return length >= 2 && b[0] == '\x10';
}

bool
is_debug_packet (const char *b, int length) {
    return length >=2 && (b[0] ==  '\x79' || b[0] == '\x92');
    // return length >=2 && (b[0] == '\x92');  //Yuanjie: optimization for iCellular, avoid unuseful debug msg
    // return length >=2 && (b[0] ==  '\x79');
}



PyObject *
decode_log_packet (const char *b, int length, bool skip_decoding) {
    if (PyDateTimeAPI == NULL)  // import datetime module
        PyDateTime_IMPORT;


    PyObject *result = NULL;
    int offset = 0;

    // Parse Header
    result = PyList_New(0);
    offset = 0;
    offset += _decode_by_fmt(LogPacketHeaderFmt, ARRAY_SIZE(LogPacketHeaderFmt, Fmt),
                                b, offset, length, result);
    PyObject *old_result = result;
    result = PyList_GetSlice(result, 1, 4); // remove the duplicate "len1" field
    Py_DECREF(old_result);
    old_result = NULL;

    // Differentiate using type ID
    LogPacketType type_id = (LogPacketType) _map_result_field_to_name(
                                result,
                                "type_id",
                                LogPacketTypeID_To_Name,
                                ARRAY_SIZE(LogPacketTypeID_To_Name, ValueName),
                                "Unsupported");

    if (skip_decoding) {    // skip further decoding
        return result;
    }

    switch (type_id) {
    case CDMA_Paging_Channel_Message:
        // Not decoded yet.
        break;

    case _1xEV_Signaling_Control_Channel_Broadcast:
        offset += _decode_by_fmt(_1xEVSignalingFmt,
                                    ARRAY_SIZE(_1xEVSignalingFmt, Fmt),
                                    b, offset, length, result);
        break;

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

    case UMTS_NAS_MM_REG_State:
        offset += _decode_by_fmt(UmtsNasMmRegStateFmt,
                                    ARRAY_SIZE(UmtsNasMmRegStateFmt, Fmt),
                                    b, offset, length, result);
        break;

    case UMTS_NAS_OTA:
        offset += _decode_by_fmt(UmtsNasOtaFmt,
                                    ARRAY_SIZE(UmtsNasOtaFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_umts_nas_ota(b, offset, length, result);
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
        offset += _decode_lte_rrc_mib(b, offset, length, result);
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

    case LTE_NAS_ESM_State:
        offset += _decode_by_fmt(LteNasEsmStateFmt,
                                    ARRAY_SIZE(LteNasEsmStateFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_nas_esm_state(b, offset, length, result);
        break;

    case LTE_LL1_PDSCH_Demapper_Configuration:
        offset += _decode_by_fmt(LteLl1PdschDemapperConfigFmt,
                                    ARRAY_SIZE(LteLl1PdschDemapperConfigFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_ll1_pdsch_demapper_config(b, offset, length, result);
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

    case LTE_ML1_IRAT_MDB:
        offset += _decode_by_fmt(LteMl1IratFmt,
                                    ARRAY_SIZE(LteMl1IratFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_ml1_irat_subpkt(b, offset, length, result);
        break;

    case LTE_ML1_CDMA_MEAS:
        //It shares similar packet format as LTE_ML1_IRAT_MDB
        offset += _decode_by_fmt(LteMl1IratFmt,
                                    ARRAY_SIZE(LteMl1IratFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_ml1_irat_cdma_subpkt(b, offset, length, result);
        break;

    case LTE_PDCP_DL_SRB_Integrity_Data_PDU:
        offset += _decode_by_fmt(LtePdcpDlSrbIntegrityDataPduFmt,
                                    ARRAY_SIZE(LtePdcpDlSrbIntegrityDataPduFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_pdcp_dl_srb_integrity_data_pdu(b, offset, length, result);
        break;

    case LTE_PDCP_UL_SRB_Integrity_Data_PDU:
        offset += _decode_by_fmt(LtePdcpUlSrbIntegrityDataPduFmt,
                                    ARRAY_SIZE(LtePdcpUlSrbIntegrityDataPduFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_pdcp_ul_srb_integrity_data_pdu(b, offset, length, result);
        break;

    case LTE_MAC_Configuration:// Jie
        offset += _decode_by_fmt(LteMacConfigurationFmt, ARRAY_SIZE(LteMacConfigurationFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_mac_configuration_subpkt(b, offset, length, result);
        break;

    case LTE_MAC_UL_Transport_Block:// Jie
        offset += _decode_by_fmt(LteMacULTransportBlockFmt, ARRAY_SIZE(LteMacULTransportBlockFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_mac_ul_transportblock_subpkt(b, offset, length, result);
        break;

    case LTE_MAC_DL_Transport_Block:// Jie
        offset += _decode_by_fmt(LteMacDLTransportBlockFmt,
                                    ARRAY_SIZE(LteMacDLTransportBlockFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_mac_dl_transportblock_subpkt(b, offset, length, result);
        break;

    case LTE_MAC_UL_Buffer_Status_Internal:// Jie
        offset += _decode_by_fmt(LteMacULBufferStatusInternalFmt,
                                    ARRAY_SIZE(LteMacULBufferStatusInternalFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_mac_ul_bufferstatusinternal_subpkt(b, offset, length, result);
        break;
    case LTE_MAC_UL_Tx_Statistics:// Jie
        offset += _decode_by_fmt(LteMacULTxStatisticsFmt,
                                    ARRAY_SIZE(LteMacULTxStatisticsFmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_lte_mac_ul_txstatistics_subpkt(b, offset, length, result);
        break;

    case Modem_debug_message: //Yuanjie: modem debugging message
        offset += _decode_by_fmt(ModemDebug_Fmt,
                                    ARRAY_SIZE(ModemDebug_Fmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_modem_debug_msg(b, offset, length, result);
        break;

    case LTE_RLC_UL_Config_Log_Packet:
        offset += _decode_by_fmt(LteRlcUlConfigLogPacketFmt,
                ARRAY_SIZE(LteRlcUlConfigLogPacketFmt, Fmt),
                b, offset, length, result);
        offset += _decode_lte_rlc_ul_config_log_packet_subpkt(b, offset,
                length, result);
        break;

    case LTE_RLC_DL_Config_Log_Packet:
        offset += _decode_by_fmt(LteRlcDlConfigLogPacketFmt,
                ARRAY_SIZE(LteRlcDlConfigLogPacketFmt, Fmt),
                b, offset, length, result);
        offset += _decode_lte_rlc_dl_config_log_packet_subpkt(b, offset,
                length, result);
        break;

    case LTE_RLC_UL_AM_All_PDU:
        offset += _decode_by_fmt(LteRlcUlAmAllPduFmt,
                ARRAY_SIZE(LteRlcUlAmAllPduFmt, Fmt),
                b, offset, length, result);
        offset += _decode_lte_rlc_ul_am_all_pdu_subpkt(b, offset,
                length, result);
        break;

    case LTE_RLC_DL_AM_All_PDU:
        offset += _decode_by_fmt(LteRlcDlAmAllPduFmt,
                ARRAY_SIZE(LteRlcDlAmAllPduFmt, Fmt),
                b, offset, length, result);
        offset += _decode_lte_rlc_dl_am_all_pdu_subpkt(b, offset,
                length, result);
        break;

    case LTE_MAC_Rach_Trigger:
        offset += _decode_by_fmt(LteMacRachTriggerFmt,
                ARRAY_SIZE(LteMacRachTriggerFmt, Fmt),
                b, offset, length, result);
        offset += _decode_lte_mac_rach_trigger_subpkt(b, offset, length,
                result);
        break;
    case LTE_MAC_Rach_Attempt:
        offset += _decode_by_fmt(LteMacRachAttempt_Fmt,
                ARRAY_SIZE(LteMacRachAttempt_Fmt, Fmt),
                b, offset, length, result);
        offset += _decode_lte_mac_rach_attempt_subpkt(b, offset, length,
                result);
        break;
    case LTE_PDCP_DL_Config:
        offset += _decode_by_fmt(LtePdcpDlConfig_Fmt,
                ARRAY_SIZE(LtePdcpDlConfig_Fmt, Fmt),
                b, offset, length, result);
        offset += _decode_lte_pdcp_dl_config_subpkt(b, offset, length, result);
        break;

    default:
        break;
    };
    return result;
}


/*-----------------------------------------------------------------------
 * DEBUG ONLY
 */

static int
_decode_by_fmt_modem (const Fmt fmt [], int n_fmt,
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
                    // iiii = *((unsigned long long *) p);
                    iiii = 0;   //Yuanjie: quick work-around to avoid crash. For modem debug, this is timestamp
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

        case QCDM_TIMESTAMP:
            {
                const double PER_SECOND = 52428800.0;
                const double PER_USECOND = 52428800.0 / 1.0e6;
                assert(fmt[i].len == 8);
                // Convert to a Python long integer object
                // unsigned long long iiii = *((unsigned long long *) p);
                unsigned long long iiii = 0;    //Yuanjie: FIX crash on Android
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



PyObject *
decode_log_packet_modem (const char *b, int length, bool skip_decoding) {
    if (PyDateTimeAPI == NULL)  // import datetime module
        PyDateTime_IMPORT;


    PyObject *result = NULL;
    int offset = 0;

    // Parse Header
    result = PyList_New(0);
    offset = 0;
    offset += _decode_by_fmt_modem(LogPacketHeaderFmt, ARRAY_SIZE(LogPacketHeaderFmt, Fmt),
                                b, offset, length, result);

    PyObject *old_result = result;
    result = PyList_GetSlice(result, 1, 4); // remove the duplicate "len1" field
    Py_DECREF(old_result);
    old_result = NULL;


    // Differentiate using type ID
    LogPacketType type_id = (LogPacketType) _map_result_field_to_name(
                                result,
                                "type_id",
                                LogPacketTypeID_To_Name,
                                ARRAY_SIZE(LogPacketTypeID_To_Name, ValueName),
                                "Unsupported");

    if (skip_decoding) {    // skip further decoding
        return result;
    }

    switch (type_id) {

    case Modem_debug_message: //Yuanjie: modem debugging message
        offset += _decode_by_fmt_modem(ModemDebug_Fmt,
                                    ARRAY_SIZE(ModemDebug_Fmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_modem_debug_msg(b, offset, length, result); 
        break;


    default:
        break;
    };


    return result;
}
