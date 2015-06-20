#include <Python.h>

PyObject *dm_collector_c_encode_hdlc_frame (PyObject *self, PyObject *args);

static PyMethodDef DmCollectorCMethods[] = {
    {"encode_hdlc_frame", dm_collector_c_encode_hdlc_frame, METH_VARARGS,
        "hahaha"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initdm_collector_c(void)
{
    (void) Py_InitModule("dm_collector_c", DmCollectorCMethods);
}