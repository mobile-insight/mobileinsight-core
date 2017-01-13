/*
 * LTE PHY Serving Cell COM Loop
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePhyServingCellComLoop_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePhyServingCellComLoop_Payload_v41 [] = {
    {UINT, "Carrier Index", 2}, // 4 bits
    {PLACEHOLDER, "Cell ID", 0},    // 9 bits
    {UINT, "Number of Records", 1}, // right shift 1 bit, 5 bits
};
const Fmt LtePhyServingCellComLoop_Payload_v22 [] = {
    {UINT, "Carrier Index", 2}, // 4 bits
    {PLACEHOLDER, "Number of Records", 0}, // 5 bits
    {SKIP, NULL, 1},
};

const ValueName LtePhyServingCellComLoop_CarrierIndex [] = {
    {0, "PCC"},
    {1, "SCC"},
};

const Fmt LtePhyServingCellComLoop_Record_v41 [] = {
    {UINT, "System Frame Number", 2},   // 10 bits
    {PLACEHOLDER, "Subframe Number", 0},    // 4 bits
    {PLACEHOLDER, "Enabled", 0},    // 1 bit
    {UINT, "COM IIR Alpha", 2}, // x/32765.0
    {UINT, "COM Adjustment[0]", 4}, // x/64.0 or
    {UINT, "COM Adjustment[1]", 4}, // (x-4294967295)/64.0
    // depends on if greater or less than 2147483648
    {UINT, "COM Unwrap Base[0]", 4},
    {UINT, "COM Unwrap Base[1]", 4},
    {BYTE_STREAM, "COM Acc Stat[0]", 8},
    {BYTE_STREAM, "COM Acc Stat[1]", 8},
    {SKIP, NULL, 16},
};
const ValueName LtePhyServingCellComLoop_Record_Enabled [] = {
    {0, "Disabled"},
    {1, "Enabled"},
};

static int _decode_lte_phy_serving_cell_com_loop_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 22:
        {
            offset += _decode_by_fmt(LtePhyServingCellComLoop_Payload_v22,
                    ARRAY_SIZE(LtePhyServingCellComLoop_Payload_v22, Fmt),
                    b, offset, length, result);
            int iNonDecodeP1 = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = iNonDecodeP1 & 15;  // last 4 bits
            int num_record = (iNonDecodeP1 >> 4) & 31;    // next 5 bits
            PyObject *old_object = _replace_result_int(result, "Carrier Index",
                    iCarrierIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Carrier Index",
                    LtePhyServingCellComLoop_CarrierIndex,
                    ARRAY_SIZE(LtePhyServingCellComLoop_CarrierIndex, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyServingCellComLoop_Record_v41,
                        ARRAY_SIZE(LtePhyServingCellComLoop_Record_v41, Fmt),
                        b, offset, length, result_record_item);
                int iNonDecodeP3_1 = _search_result_int(result_record_item,
                        "System Frame Number");
                int iSysFN = iNonDecodeP3_1 & 1023;
                int iSubFN = (iNonDecodeP3_1 >> 10) & 15;
                int iEnabled = (iNonDecodeP3_1 >> 14) & 1;
                old_object = _replace_result_int(result_record_item,
                        "System Frame Number", iSysFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Subframe Number", iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Enabled", iEnabled);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item, "Enabled",
                        LtePhyServingCellComLoop_Record_Enabled,
                        ARRAY_SIZE(LtePhyServingCellComLoop_Record_Enabled,
                            ValueName),
                        "(MI)Unknown");
                int iCIA = _search_result_int(result_record_item, "COM IIR Alpha");
                float fCIA = iCIA / 32765.0;
                PyObject *pyfloat = Py_BuildValue("f", fCIA);
                old_object = _replace_result(result_record_item, "COM IIR Alpha", pyfloat);
                Py_DECREF(old_object);

                int iCA0 = _search_result_uint(result_record_item, "COM Adjustment[0]");
                float ratio = 64.0;
                float fCA0 = iCA0 / ratio;
                pyfloat = Py_BuildValue("f", fCA0);
                old_object = _replace_result(result_record_item, "COM Adjustment[0]", pyfloat);
                Py_DECREF(old_object);

                int iCA1 = _search_result_uint(result_record_item, "COM Adjustment[1]");
                ratio = 64.0;
                float fCA1 = iCA1 / ratio;
                pyfloat = Py_BuildValue("f", fCA1);
                old_object = _replace_result(result_record_item, "COM Adjustment[1]", pyfloat);
                Py_DECREF(old_object);

                unsigned int iCNB0 = _search_result_uint(result_record_item, "COM Unwrap Base[0]");
                float fCNB0 = iCNB0 / 64.0;
                pyfloat = Py_BuildValue("f", fCNB0);
                old_object = _replace_result(result_record_item, "COM Unwrap Base[0]", pyfloat);
                Py_DECREF(old_object);

                unsigned int iCNB1 = _search_result_uint(result_record_item, "COM Unwrap Base[1]");
                float fCNB1 = iCNB1 / 64.0;
                pyfloat = Py_BuildValue("f", fCNB1);
                old_object = _replace_result(result_record_item, "COM Unwrap Base[1]", pyfloat);
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

    case 41:
        {
            offset += _decode_by_fmt(LtePhyServingCellComLoop_Payload_v41,
                    ARRAY_SIZE(LtePhyServingCellComLoop_Payload_v41, Fmt),
                    b, offset, length, result);
            int iNonDecodeP1 = _search_result_int(result, "Carrier Index");
            int iCarrierIndex = iNonDecodeP1 & 15;  // last 4 bits
            int iCellId = (iNonDecodeP1 >> 4) & 511;    // next 9 bits
            PyObject *old_object = _replace_result_int(result, "Carrier Index",
                    iCarrierIndex);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(result, "Carrier Index",
                    LtePhyServingCellComLoop_CarrierIndex,
                    ARRAY_SIZE(LtePhyServingCellComLoop_CarrierIndex, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "Cell ID", iCellId);
            Py_DECREF(old_object);
            int iNonDecodeP2 = _search_result_int(result, "Number of Records");
            int num_record = (iNonDecodeP2 >> 1) & 31;
            old_object = _replace_result_int(result, "Number of Records",
                    num_record);
            Py_DECREF(old_object);

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyServingCellComLoop_Record_v41,
                        ARRAY_SIZE(LtePhyServingCellComLoop_Record_v41, Fmt),
                        b, offset, length, result_record_item);
                int iNonDecodeP3_1 = _search_result_int(result_record_item,
                        "System Frame Number");
                int iSysFN = iNonDecodeP3_1 & 1023;
                int iSubFN = (iNonDecodeP3_1 >> 10) & 15;
                int iEnabled = (iNonDecodeP3_1 >> 14) & 1;
                old_object = _replace_result_int(result_record_item,
                        "System Frame Number", iSysFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Subframe Number", iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Enabled", iEnabled);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item, "Enabled",
                        LtePhyServingCellComLoop_Record_Enabled,
                        ARRAY_SIZE(LtePhyServingCellComLoop_Record_Enabled,
                            ValueName),
                        "(MI)Unknown");
                unsigned int iCIA = _search_result_uint(result_record_item, "COM IIR Alpha");
                float fCIA = iCIA / 32765.0;
                PyObject *pyfloat = Py_BuildValue("f", fCIA);
                old_object = _replace_result(result_record_item, "COM IIR Alpha", pyfloat);
                Py_DECREF(old_object);

                int iCA0 = _search_result_uint(result_record_item, "COM Adjustment[0]");
                float ratio = 64.0;
                float fCA0 = iCA0 / ratio;
                pyfloat = Py_BuildValue("f", fCA0);
                old_object = _replace_result(result_record_item, "COM Adjustment[0]", pyfloat);
                Py_DECREF(old_object);

                int iCA1 = _search_result_uint(result_record_item, "COM Adjustment[1]");
                ratio = 64.0;
                float fCA1 = iCA1 / ratio;
                pyfloat = Py_BuildValue("f", fCA1);
                old_object = _replace_result(result_record_item, "COM Adjustment[1]", pyfloat);
                Py_DECREF(old_object);

                unsigned int iCNB0 = _search_result_uint(result_record_item, "COM Unwrap Base[0]");
                float fCNB0 = iCNB0 / 64.0;
                pyfloat = Py_BuildValue("f", fCNB0);
                old_object = _replace_result(result_record_item, "COM Unwrap Base[0]", pyfloat);
                Py_DECREF(old_object);

                unsigned int iCNB1 = _search_result_uint(result_record_item, "COM Unwrap Base[1]");
                float fCNB1 = iCNB1 / 64.0;
                pyfloat = Py_BuildValue("f", fCNB1);
                old_object = _replace_result(result_record_item, "COM Unwrap Base[1]", pyfloat);
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
        printf("(MI)Unknown LTE PHY Serving Cell COM Loop version: 0x%x\n", pkt_ver);
        return 0;
    }
}
