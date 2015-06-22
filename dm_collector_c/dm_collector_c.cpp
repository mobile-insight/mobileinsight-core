#include <Python.h>

#include "consts.h"
#include "hdlc.h"
#include "log_config.h"
#include "log_packet.h"

#include <string>
#include <vector>
#include <algorithm>


static PyObject *dm_collector_c_disable_logs (PyObject *self, PyObject *args);
static PyObject *dm_collector_c_enable_logs (PyObject *self, PyObject *args);
static PyObject *dm_collector_c_feed_binary (PyObject *self, PyObject *args);
static PyObject *dm_collector_c_receive_log_packet (PyObject *self, PyObject *args);

static PyMethodDef DmCollectorCMethods[] = {
    {"disable_logs", dm_collector_c_disable_logs, METH_VARARGS,
        "hahaha"},
    {"enable_logs", dm_collector_c_enable_logs, METH_VARARGS,
        "hahaha"},
    {"feed_binary", dm_collector_c_feed_binary, METH_VARARGS,
        "hahaha"},
    {"receive_log_packet", dm_collector_c_receive_log_packet, METH_VARARGS,
        "hahaha"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static void
sort_type_ids(IdVector& type_ids, std::vector<IdVector>& out_vectors) {
    sort(type_ids.begin(), type_ids.end());
    auto itr = std::unique(type_ids.begin(), type_ids.end());
    type_ids.resize(std::distance(type_ids.begin(), itr));

    out_vectors.clear();
    int last_equip_id = -1;
    size_t i = 0, j = 0;
    for (j = 0; j < type_ids.size(); j++) {
        if (j != 0 && last_equip_id != get_equip_id(type_ids[j])) {
            out_vectors.push_back(IdVector(type_ids.begin() + i, 
                                            type_ids.begin() + j));
            i = j;
        }
        last_equip_id = get_equip_id(type_ids[j]);
    }
    if (i != j) {
        out_vectors.push_back(IdVector(type_ids.begin() + i, 
                                        type_ids.begin() + j));
    }
    return;
}

static bool
check_serial_port (PyObject *o) {
    return PyObject_HasAttrString(o, "read");
}

static bool
send_msg (PyObject *serial_port, const char *b, int length) {
    std::string frame = encode_hdlc_frame(b, length);
    PyObject *o = PyObject_CallMethod(serial_port,
                                        (char *) "write",
                                        (char *) "s#", frame.c_str(), frame.size());
    Py_DECREF(o);
    return true;
}

static PyObject *
dm_collector_c_disable_logs (PyObject *self, PyObject *args) {
    IdVector empty;
    BinaryBuffer buf;
    PyObject *serial_port = NULL;
    if (!PyArg_ParseTuple(args, "O", &serial_port)) {
        return NULL;
    }
    Py_INCREF(serial_port);

    // Check arguments
    if (!check_serial_port(serial_port)) {
        PyErr_SetString(PyExc_TypeError, "\'port\' is not a serial port.");
        goto raise_exception;
    }

    buf = encode_log_config(DISABLE, empty);
    if (buf.first == NULL || buf.second == 0) {
        Py_DECREF(serial_port);
        Py_RETURN_FALSE;
    }
    send_msg(serial_port, buf.first, buf.second);
    Py_DECREF(serial_port);
    Py_RETURN_TRUE;

    raise_exception:
        Py_DECREF(serial_port);
        return NULL;
}

// Return: successful or not
static PyObject *
dm_collector_c_enable_logs (PyObject *self, PyObject *args) {
    IdVector type_ids;
    std::vector<IdVector> type_id_vectors;
    BinaryBuffer buf;
    PyObject *serial_port = NULL;
    PyObject *sequence = NULL;
    int n;
    if (!PyArg_ParseTuple(args, "OO", &serial_port, &sequence)) {
        return NULL;
    }
    Py_INCREF(serial_port);
    Py_INCREF(sequence);

    // Check arguments
    if (!check_serial_port(serial_port)) {
        PyErr_SetString(PyExc_TypeError, "\'port\' is not a serial port.");
        goto raise_exception;
    }
    if (!PySequence_Check(sequence)) {
        PyErr_SetString(PyExc_TypeError, "\'type_names\' is not a sequence.");
        goto raise_exception;
    }

    n = PySequence_Length(sequence);
    for (int i = 0; i < n; i++) {
        PyObject *item = PySequence_GetItem(sequence, i);
        if (PyString_Check(item)) {
            std::string name(PyString_AsString(item));
            if (LogPacketType_To_ID.count(name) > 0) {
                int id = (int) LogPacketType_To_ID.at(name);
                type_ids.push_back(id);
            } else {
                PyErr_SetString(PyExc_RuntimeError, "Wrong type names.");
                Py_DECREF(item);
                goto raise_exception;
            }
        } else {
            // ignore non-strings
        }
        if (item != NULL)
            Py_DECREF(item);    // Discard reference ownership
    }
    Py_DECREF(sequence); sequence = NULL;

    // send log config messages
    sort_type_ids(type_ids, type_id_vectors);
    for (const IdVector& v: type_id_vectors) {
        buf = encode_log_config(SET_MASK, v);
        if (buf.first == NULL || buf.second == 0) {
            Py_DECREF(serial_port);
            Py_RETURN_FALSE;
        }
        send_msg(serial_port, buf.first, buf.second);
    }
    Py_DECREF(serial_port);
    Py_RETURN_TRUE;

    raise_exception:
        Py_DECREF(sequence);
        Py_DECREF(serial_port);
        return NULL;
}

static PyObject *
dm_collector_c_feed_binary (PyObject *self, PyObject *args) {
    const char *b;
    int length;
    if (!PyArg_ParseTuple(args, "s#", &b, &length))
        return NULL;
    feed_binary(b, length);
    Py_RETURN_NONE;
}

static PyObject *
dm_collector_c_receive_log_packet (PyObject *self, PyObject *args) {
    std::string frame;
    bool crc_correct;
    bool success = get_next_frame(frame, crc_correct);
    if (success && crc_correct && is_log_packet(frame.c_str(), frame.size())) {
        const char *s = frame.c_str();
        return decode_log_packet(s + 2, frame.size() - 2);  // skip first two bytes
    } else {
        Py_RETURN_NONE;
    }
}

PyMODINIT_FUNC
initdm_collector_c(void)
{
    PyObject *dm_collector_c = Py_InitModule("dm_collector_c", DmCollectorCMethods);

    int n_types = LogPacketType_To_ID.size();
    PyObject *log_packet_types = PyTuple_New(n_types);
    int i = 0;
    for (auto& pair: LogPacketType_To_ID) {
        // There is no leak here, because PyTuple_SetItem steals reference
        PyTuple_SetItem(log_packet_types, i, Py_BuildValue("s", pair.first.c_str()));
        i++;
    }
    PyObject_SetAttrString(dm_collector_c, "log_packet_types", log_packet_types);
}
