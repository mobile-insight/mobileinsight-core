/*
 * LTE ******
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt Lte******_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt Lte***_Payload_v* [] = {
};

const Fmt Lte***_Record_v* [] = {
};

const Fmt Lte***_Stream_v* [] = {
};

const Fmt Lte***_EnergyMetric_v* [] = {
};

static int _decode_lte_***_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;
    PyObject *pyfloat;
    int temp;

    switch (pkt_ver) {
    case *:
        {
            offset += _decode_by_fmt(Lte***_Payload_v*,
                    ARRAY_SIZE(Lte***_Payload_v*, Fmt),
                    b, offset, length, result);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(Lte***_Record_v*,
                        ARRAY_SIZE(Lte***_Record_v*, Fmt),
                        b, offset, length, result_record_item);

                PyObject *result_record_stream = PyList_New(0);
                for (int j = 0; j < num_stream; j++) {
                    PyObject *result_record_stream_item = PyList_New(0);
                    offset += _decode_by_fmt(Lte***_Stream_v*,
                            ARRAY_SIZE(Lte***_Stream_v*, Fmt),
                            b, offset, length, result_record_stream_item);

                    PyObject *result_energy_metric = PyList_New(0);
                    for (int k = 0; k < num_energy_metric; k++) {
                        PyObject *result_energy_metric_item = PyList_New(0);
                        offset += _decode_by_fmt(Lte***_EnergyMetric_v*,
                                ARRAY_SIZE(Lte***_EnergyMetric_v*, Fmt),
                                b, offset, length, result_energy_metric_item);

                        PyObject *t5 = Py_BuildValue("(sOs)", "Ignored",
                                result_energy_metric_item, "dict");
                        PyList_Append(result_energy_metric, t5);
                        Py_DECREF(t5);
                        Py_DECREF(result_energy_metric_item);
                    }
                    offset += (13 - num_energy_metric) * 4;

                    PyObject *t4 = Py_BuildValue("(sOs)", "Energy Metrics",
                            result_energy_metric, "list");
                    PyList_Append(result_record_stream_item, t4);
                    Py_DECREF(t4);
                    Py_DECREF(result_energy_metric);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_record_stream_item, "dict");
                    PyList_Append(result_record_stream, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_record_stream_item);
                }

                PyObject *t2 = Py_BuildValue("(sOs)", "Streams",
                        result_record_stream, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_record_stream);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE *** version: %d\n", pkt_ver);
        return 0;
    }
}
