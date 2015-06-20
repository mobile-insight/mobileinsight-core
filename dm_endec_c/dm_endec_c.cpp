#include <Python.h>

#include "consts.h"

PyObject *dm_endec_c_decode_log_packet (PyObject *self, PyObject *args);
PyObject *dm_endec_c_encode_log_config (PyObject *self, PyObject *args);

static PyMethodDef DmEndecCMethods[] = {
    {"decode_log_packet", dm_endec_c_decode_log_packet , METH_VARARGS,
        "hahaha"},
    {"encode_log_config", dm_endec_c_encode_log_config , METH_VARARGS,
        "hahaha"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

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
