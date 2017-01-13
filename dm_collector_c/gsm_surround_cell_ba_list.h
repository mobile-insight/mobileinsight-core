/*
 * GSM Surround Cell BA List
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt GsmScbl_Fmt [] = {
    {UINT, "Cell Count", 1},
};

const Fmt GsmScbl_Cell [] = {
    {UINT, "BCCH ARFCN", 2},    // 12 bits
    {PLACEHOLDER, "BCCH Band", 0},  // 4 bits
    {UINT, "RX Power", 2},  // (x - 65536) / 16.0
    {UINT, "BSIC Known", 1},
    {UINT, "BSIC-BCC", 1},  // 3 bits
    {PLACEHOLDER, "BSIC-NCC", 1},   // 3 bits
    {UINT, "Frame Number Offset", 4},
    {UINT, "Time Offset", 2},
};

static int _decode_gsm_scbl_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int num_record = _search_result_int(result, "Cell Count");

    PyObject *old_object;
    PyObject *pyfloat;
    int temp;

    PyObject *result_record = PyList_New(0);
    for (int i = 0; i < num_record; i++) {
        PyObject *result_record_item = PyList_New(0);
        offset += _decode_by_fmt(GsmScbl_Cell,
                ARRAY_SIZE(GsmScbl_Cell, Fmt),
                b, offset, length, result_record_item);

        (void)_map_result_field_to_name(result_record_item, "BSIC Known",
                ValueNameTrueOrFalse,
                ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                "(MI)Unknown");

        temp = _search_result_int(result_record_item, "BCCH ARFCN");
        int iArfcn = temp & 4095;   // 12 bits
        int iBand = (temp >> 12) & 15;  // 4 bits
        old_object = _replace_result_int(result_record_item, "BCCH ARFCN",
                iArfcn);
        Py_DECREF(old_object);
        old_object = _replace_result_int(result_record_item, "BCCH Band",
                iBand);
        Py_DECREF(old_object);
        (void) _map_result_field_to_name(result_record_item,
                "BCCH Band",
                ValueNameBandClassGSM,
                ARRAY_SIZE(ValueNameBandClassGSM, ValueName),
                "(MI)Unknown");

        temp = _search_result_int(result_record_item, "RX Power");
        float fRxPower = (temp - 65536) / 16.0;
        pyfloat = Py_BuildValue("f", fRxPower);
        old_object = _replace_result(result_record_item, "RX Power", pyfloat);
        Py_DECREF(old_object);
        Py_DECREF(pyfloat);

        temp = _search_result_int(result_record_item, "BSIC-BCC");
        int iBCC = temp & 7;    // 3 bits
        int iNCC = (temp >> 3) & 7; // 3 bits
        old_object = _replace_result_int(result_record_item, "BSIC-BCC", iBCC);
        Py_DECREF(old_object);
        old_object = _replace_result_int(result_record_item, "BSIC-NCC", iNCC);
        Py_DECREF(old_object);

        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                result_record_item, "dict");
        PyList_Append(result_record, t1);
        Py_DECREF(t1);
        Py_DECREF(result_record_item);
    }
    PyObject *t = Py_BuildValue("(sOs)", "Surrounding Cells",
            result_record, "list");
    PyList_Append(result, t);
    Py_DECREF(t);
    Py_DECREF(result_record);
    return offset - start;
}
