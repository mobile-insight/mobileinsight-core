/*
 * LTE PHY RLM Report
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePhyRlmReport_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePhyRlmReport_Payload_v1 [] = {
    {SKIP, NULL, 2},
    {UINT, "Number of Records", 1},
};

const Fmt LtePhyRlmReport_Record_v1 [] = {
    {UINT, "System Frame Number", 2},   // 10 bits
    {PLACEHOLDER, "Sub-frame Number", 0},   // 4 bits
    {UINT, "Out of Sync BLER (%)", 2},
    {UINT, "In Sync BLER (%)", 2},
    {UINT, "Out of Sync Count", 2}, // 5 bits
    {PLACEHOLDER, "In Sync Count", 0},  // 5 bits
    {UINT, "T310 Timer Status", 1},
    {SKIP, NULL, 3},
};

static int _decode_lte_phy_rlm_report_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;
    PyObject *pyfloat;
    int temp;

    switch (pkt_ver) {
    case 1:
        {
            offset += _decode_by_fmt(LtePhyRlmReport_Payload_v1,
                    ARRAY_SIZE(LtePhyRlmReport_Payload_v1, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Number of Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyRlmReport_Record_v1,
                        ARRAY_SIZE(LtePhyRlmReport_Record_v1, Fmt),
                        b, offset, length, result_record_item);
                temp = _search_result_int(result_record_item, "System Frame Number");
                int iSFN = temp & 1023;
                int iSubFN = (temp >> 10) & 15;
                int iOutofSyncBler = _search_result_int(result_record_item, "Out of Sync BLER (%)");
                float fOutofSyncBler = iOutofSyncBler * 100.0 / 32768;
                int iInSyncBler = _search_result_int(result_record_item, "In Sync BLER (%)");
                float fInSyncBler = iInSyncBler * 100.0 / 32768;
                temp = _search_result_int(result_record_item, "Out of Sync Count");
                int iOutofSyncCount = temp & 31;
                int iInSyncCount = (temp >> 5) & 31;

                old_object = _replace_result_int(result_record_item,
                        "System Frame Number", iSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Sub-frame Number", iSubFN);
                Py_DECREF(old_object);
                pyfloat = Py_BuildValue("f", fOutofSyncBler);
                old_object = _replace_result(result_record_item,
                        "Out of Sync BLER (%)", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);
                pyfloat = Py_BuildValue("f", fInSyncBler);
                old_object = _replace_result(result_record_item,
                        "In Sync BLER (%)", pyfloat);
                Py_DECREF(old_object);
                Py_DECREF(pyfloat);
                old_object = _replace_result_int(result_record_item,
                        "Out of Sync Count", iOutofSyncCount);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "In Sync Count", iInSyncCount);
                Py_DECREF(old_object);

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
        printf("(MI)Unknown LTE PHY RLM Report version: 0x%x\n", pkt_ver);
        return 0;
    }
}
