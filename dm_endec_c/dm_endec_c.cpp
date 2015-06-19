#include <Python.h>

#include <utility>
#include <algorithm>
#include <vector>

#include "consts.h"

typedef std::pair<char*, int> BinaryBuffer;

PyObject *dm_endec_c_decode_log_packet (PyObject *self, PyObject *args);
static PyObject * dm_endec_c_encode_log_config (PyObject *self, PyObject *args);

static PyMethodDef DmEndecCMethods[] = {
    {"decode_log_packet",  dm_endec_c_decode_log_packet , METH_VARARGS,
        "hahaha"},
    {"encode_log_config",  dm_endec_c_encode_log_config , METH_VARARGS,
        "hahaha"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static BinaryBuffer
_encode_log_config (LogConfigOp op, const std::vector<int>& type_ids) {
    const int EQUIP_ID_MASK = 0x0000F000;
    const int ITEM_ID_MASK = 0x00000FFF;

    BinaryBuffer buf;
    buf.first = NULL;
    buf.second = 0;
    switch (op) {
    case DISABLE:
        buf.second = sizeof(char) * 4 + sizeof(int);
        buf.first = new char[buf.second];
        buf.first[0] = 115;
        buf.first[1] = 0;
        buf.first[2] = 0;
        buf.first[3] = 0;
        *((int *)(buf.first + 4)) = (int) op;
        break;

    case SET_MASK:
        {
            int equip_id = -1;
            int highest = -1;
            for (int id : type_ids) {
                int e = (id & EQUIP_ID_MASK) >> 12;
                if (equip_id == -1) {
                    equip_id = e;
                    highest = std::max(highest, id & ITEM_ID_MASK);
                } else if (equip_id == e) {
                    highest = std::max(highest, id & ITEM_ID_MASK);
                } else {
                    equip_id = -1;
                    break;
                }
            }
            if (equip_id == -1) {
                ;   // fail to extract an unique equid id
            } else {
                int mask_len = (highest / 8) + 1;
                buf.second = sizeof(char) * 4 + sizeof(int) * 3 + mask_len;
                buf.first = new char[buf.second];
                buf.first[0] = 115;
                buf.first[1] = 0;
                buf.first[2] = 0;
                buf.first[3] = 0;
                *((int *)(buf.first + 4)) = (int) op;
                *((int *)(buf.first + 8)) = equip_id;
                *((int *)(buf.first + 12)) = highest + 1;
                char *mask = buf.first + 16;
                for (int i = 0; i < mask_len; i++)
                    mask[i] = 0;
                for (int id : type_ids) {
                    int i = id & ITEM_ID_MASK;
                    mask[i / 8] |= 1 << (i % 8);
                }
            }
            break;
        }

    case GET_RANGE:
    case GET_MASK:
    default:
        break;
    }
    return buf;
}

static PyObject *
dm_endec_c_encode_log_config (PyObject *self, PyObject *args) {
    std::vector<int> type_ids;
    BinaryBuffer buf;
    LogConfigOp op = DISABLE;
    const char * op_name = NULL;
    int n;
    PyObject *sequence = NULL;
    PyObject *retstr = NULL;

    if (!PyArg_ParseTuple(args, "sO", &op_name, &sequence))
        return NULL;
    Py_INCREF(sequence);

    // Check arguments
    if (!PySequence_Check(sequence))
        goto fail;
    n = PySequence_Length(sequence);
    if (strcmp(op_name, "DISABLE") == 0)
        op = DISABLE;
    else if (strcmp(op_name, "GET_RANGE") == 0)
        op = GET_RANGE;
    else if (strcmp(op_name, "SET_MASK") == 0)
        op = SET_MASK;
    else if (strcmp(op_name, "GET_MASK") == 0)
        op = GET_MASK;
    else
        goto fail;

    for (int i = 0; i < n; i++) {
        PyObject *item = PySequence_GetItem(sequence, i);
        if (item == NULL) 
            goto fail;
        if (PyInt_Check(item))
            type_ids.push_back(int(PyInt_AsLong(item)));
        Py_DECREF(item);    // Discard reference ownership
    }
    Py_DECREF(sequence);
    buf = _encode_log_config(op, type_ids);
    if (buf.first == NULL || buf.second == 0)
        goto fail;
    retstr = Py_BuildValue("s#", buf.first, buf.second);
    delete buf.first;
    return retstr;

    fail:
        Py_DECREF(sequence);
        Py_RETURN_NONE;
}

PyMODINIT_FUNC
initdm_endec_c(void)
{
    PyObject *dm_endec_c = Py_InitModule("dm_endec_c", DmEndecCMethods);

    int n_types = sizeof(LogPacketTypes) / sizeof(const char *);
    PyObject *log_packet_types = PyTuple_New(n_types);
    for (int i = 0; i < n_types; i++) {
        PyTuple_SetItem(log_packet_types, i, Py_BuildValue("s", LogPacketTypes[i]));
    }
    PyObject_SetAttrString(dm_endec_c, "log_packet_types", log_packet_types);
}
