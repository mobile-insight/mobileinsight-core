#include <Python.h>
#include <datetime.h>

#include "consts.h"
#include "log_packet.h"

static int
_decode_by_fmt (const Fmt fmt [], int n_fmt,
                const char *b, int offset, int length,
                PyObject *result) {
    assert(PyList_Check(result));
    int n_consumed = 0;

    Py_INCREF(result);
    for (int i = 0; i < n_fmt; i++) {
        PyObject *decoded = NULL;
        switch (fmt[i].type) {
        case UINT:
            {
                unsigned int ii = 0;
                switch (fmt[i].len) {
                case 1:
                    ii = *((unsigned char *)(b + offset + n_consumed));
                    break;
                case 2:
                    ii = *((unsigned short *)(b + offset + n_consumed));
                    break;
                case 4:
                    ii = *((unsigned int *)(b + offset + n_consumed));
                    break;
                case 8: // TODO: Not handled
                default:
                    assert(false);
                    break;
                }
                // Convert to a Python integer object or a Python long integer object
                decoded = Py_BuildValue("I", ii);
                n_consumed += fmt[i].len;
                break;
            }

        case PLMN:
            {
                assert(fmt[i].len == 6);
                const char *plmn = b + offset + n_consumed;
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

        case TIMESTAMP1:
            {
                const double PER_SECOND = 52428800.0;
                const double PER_USECOND = 52428800.0 / 1.0e6;
                assert(fmt[i].len == 8);
                // Convert to a Python long integer object
                unsigned long long iiii = *((unsigned long long *)(b + offset + n_consumed));
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

PyObject *
dm_endec_c_decode_log_packet (PyObject *self, PyObject *args)
{
    if (PyDateTimeAPI == NULL)
        PyDateTime_IMPORT;
    const char *b;
    int offset = 0;
    int length;
    PyObject *result = NULL;
    PyObject *item = NULL;

    if (!PyArg_ParseTuple(args, "s#", &b, &length))
        return NULL;

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
        offset += _decode_by_fmt(WCDMACellIdFmt, sizeof(WCDMACellIdFmt) / sizeof(Fmt),
                                    b, offset, length, result);
        break;
    default:
        break;
    };
    return result;
}
