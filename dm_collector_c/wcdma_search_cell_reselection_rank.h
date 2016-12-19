/*
 * WCDMA_Search_Cell_Reselection_Rank
 *
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt WcdmaScrr_Fmt [] = {
    {UINT, "WCDMA Cells", 1},
    {UINT, "GSM Cells", 1},
};

const Fmt WcdmaScrr_Wcdma_Cell [] = {
    {UINT, "RF Channel Frequency", 2},
    {UINT, "Primary Scrambling Code", 2},
    {UINT, "Received Signal Code Power", 1},    // x - 277
    {UINT, "Cell Ranking RSCP", 2}, // x - 65536
    {UINT, "Ec/Io", 1}, // (x - 256) * 0.5
    {UINT, "Cell Ranking Ec/Io", 2},    // x - 65536
};

static int _decode_wcdma_scrr_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int num_wcdma_cells = _search_result_int(result, "WCDMA Cells");
    // int num_gsm_cells = _search_result_int(result, "GSM Cells");

    PyObject *old_object;
    PyObject *pyfloat;
    int temp;

    PyObject *result_record = PyList_New(0);
    for (int i = 0; i < num_wcdma_cells; i++) {
        PyObject *result_record_item = PyList_New(0);
        offset += _decode_by_fmt(WcdmaScrr_Wcdma_Cell,
                ARRAY_SIZE(WcdmaScrr_Wcdma_Cell, Fmt),
                b, offset, length, result_record_item);

        temp = _search_result_int(result_record_item,
                "Received Signal Code Power");
        int iRSCP = temp - 277;
        old_object = _replace_result_int(result_record_item,
                "Received Signal Code Power", iRSCP);
        Py_DECREF(old_object);

        temp = _search_result_int(result_record_item,
                "Cell Ranking RSCP");
        int iRankingRSCP = temp - 65536;
        old_object = _replace_result_int(result_record_item,
                "Cell Ranking RSCP", iRankingRSCP);
        Py_DECREF(old_object);

        temp = _search_result_int(result_record_item,
                "Ec/Io");
        float fEcIo = (temp - 256) * 0.5;
        pyfloat = Py_BuildValue("f", fEcIo);
        old_object = _replace_result(result_record_item,
                "Ec/Io", pyfloat);
        Py_DECREF(old_object);
        Py_DECREF(pyfloat);

        temp = _search_result_int(result_record_item,
                "Cell Ranking Ec/Io");
        int iCellRankingEcIo = temp - 65536;
        old_object = _replace_result_int(result_record_item,
                "Cell Ranking Ec/Io", iCellRankingEcIo);
        Py_DECREF(old_object);

        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                result_record_item, "dict");
        PyList_Append(result_record, t1);
        Py_DECREF(t1);
        Py_DECREF(result_record_item);
    }
    PyObject *t = Py_BuildValue("(sOs)", "WCDMA Search Results",
            result_record, "list");
    PyList_Append(result, t);
    Py_DECREF(t);
    Py_DECREF(result_record);
    return offset - start;
}
