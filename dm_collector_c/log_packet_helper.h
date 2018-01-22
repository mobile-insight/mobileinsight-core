/* log_packet_helper.h */

#ifndef __DM_COLLECTOR_C_LOG_PACKET_HELPER_H__
#define __DM_COLLECTOR_C_LOG_PACKET_HELPER_H__

#include <Python.h>
#include <datetime.h>
#include "consts.h"
#include "log_packet.h"

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>

#ifdef __ANDROID__
#include <android/log.h>
#define printf(fmt,args...) __android_log_print(ANDROID_LOG_INFO, "python", fmt, ##args);
#endif

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

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
static int _search_result_int(
        PyObject *result,
        const char *target)
    __attribute__ ((unused));
static int
_search_result_int(PyObject *result, const char *target) {
    PyObject *item = _search_result(result, target);
    assert(PyInt_Check(item));
    int val = (int) PyInt_AsLong(item);
    Py_DECREF(item);

    return val;
}

// This function should be called when the value is 4 bytes long.
// Return: unsigned int
static unsigned int _search_result_uint(
        PyObject *result,
        const char *target)
    __attribute__ ((unused));
static unsigned int
_search_result_uint(PyObject *result, const char *target) {
    PyObject *item = _search_result(result, target);
    assert(PyInt_Check(item));
    unsigned int val = (unsigned int) PyInt_AsUnsignedLongMask(item);
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
static PyObject * _replace_result_int(
        PyObject *result,
        const char *target,
        int new_int)
    __attribute__ ((unused));
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
static int _map_result_field_to_name(
        PyObject *result,
        const char *target,
        const ValueName mapping [],
        int n,
        const char *not_found)
    __attribute__ ((unused));
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

static unsigned int _decode_by_bit (
        int start,
        int bitlength,
        const char *b)
    __attribute__ ((unused));
static unsigned int
_decode_by_bit (int start, int bitlength, const char *b) {
    unsigned int rt = 0;
    int temp = 0;
    assert(bitlength <= 32);
    const char *p = b + start / 8;
    int usedByte = (start + bitlength + 8 - 1) / 8 - start / 8;
    unsigned char buffer[5] = {0};
    memcpy(buffer, p, sizeof(char) * usedByte);

    int left_remaining = 8 - start % 8;
    int right_drop = 8 - (start + bitlength) % 8;
    if (right_drop == 8) {
        right_drop = 0;
    }

    if (usedByte > 1) {
        temp = (int)buffer[0] & ((int)std::pow(2, left_remaining) - 1);
        rt += temp * std::pow(2, bitlength - left_remaining);
        for (int i = 1; i < usedByte - 1; i++) {
            temp = (int)buffer[i];
            rt += temp * pow(2, bitlength - left_remaining - 8 * i);
        }
        temp = (int)buffer[usedByte - 1] >> right_drop;
        rt += temp;
    } else {
        temp = ((int)buffer[0] >> right_drop);
        temp = temp & ((int)std::pow(2, bitlength) - 1);
        rt = temp;
    }
    return rt;
}

// Decode a binary string according to an array of field description (fmt[]).
// Decoded fields are appended to result
static int _decode_by_fmt (
        const Fmt fmt [],
        int n_fmt,
        const char *b,
        int offset,
        int length,
        PyObject *result)
    __attribute__ ((unused));
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
                unsigned long long iiii = -1LL;
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
                {
                    // iiii = *((unsigned long long *) p);
                    unsigned char buffer64[256] = {0};
                    memcpy(buffer64, p, sizeof(unsigned long long));
                    iiii = *reinterpret_cast<unsigned long long *>(buffer64);
                    break;
                }
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
        case UINT_BIG_ENDIAN:
            {
                unsigned int ii = 0;
                unsigned long long iiii = -1LL;
                char p_reverse[8];
                for (int j = 0; j < fmt[i].len; j++) {
                    p_reverse[j] = p[fmt[i].len - 1 - j];
                }
                switch (fmt[i].len) {
                case 1:
                    ii = *((unsigned char *) p_reverse);
                    break;
                case 2:
                    ii = *((unsigned short *) p_reverse);
                    break;
                case 4:
                    ii = *((unsigned int *) p_reverse);
                    break;
                case 8:
                {
                    // iiii = *((unsigned long long *) p);
                    unsigned char buffer64[256] = {0};
                    memcpy(buffer64, p, sizeof(unsigned long long));
                    iiii = *reinterpret_cast<unsigned long long *>(buffer64);
                    break;
                }
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

        case BYTE_STREAM_LITTLE_ENDIAN:
            {
                assert(fmt[i].len > 0);
                char hex[10] = {};
                std::string ascii_data = "0x";
                for (int k = fmt[i].len - 1; k >= 0; k--) {
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

#endif // __DM_COLLECTOR_C_LOG_PACKET_HELPER_H__
