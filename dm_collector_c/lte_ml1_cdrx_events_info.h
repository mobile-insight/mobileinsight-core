/*
 * LTE ML1 CDRX Events Info
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LteMl1CdrxEventsInfo_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LteMl1CdrxEventsInfo_Payload_v1 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LteMl1CdrxEventsInfo_Record_v1 [] = {
    {UINT, "SFN", 4},   // 10 bits
    {PLACEHOLDER, "Sub-FN", 0}, // 4 bits
    {PLACEHOLDER, "CDRX Event", 0}, // 6 bits
};

const Fmt LteMl1CdrxEventsInfo_Payload_v2 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LteMl1CdrxEventsInfo_Record_v2 [] = {
    {UINT, "SFN", 4},   // 10 bits
    {PLACEHOLDER, "Sub-FN", 0}, // 4 bits
    {PLACEHOLDER, "CDRX Event", 0}, // 6 bits
    {UINT, "Internal Field Mask", 4},   // 32 bits
};

static int _decode_lte_ml1_cdrx_events_info_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;
    PyObject *pyfloat;
    int temp;

    switch (pkt_ver) {
    case 1:
        {
            offset += _decode_by_fmt(LteMl1CdrxEventsInfo_Payload_v1,
                    ARRAY_SIZE(LteMl1CdrxEventsInfo_Payload_v1, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LteMl1CdrxEventsInfo_Record_v1,
                        ARRAY_SIZE(LteMl1CdrxEventsInfo_Record_v1, Fmt),
                        b, offset, length, result_record_item);

                uint utemp = _search_result_uint(result_record_item, "SFN");
                int iSFN = utemp & 1023;
                int iSubFN = (utemp >> 10) & 15;
                int iCdrxEvent = (utemp >> 14) & 63;
                old_object = _replace_result_int(result_record_item, "SFN",
                        iSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Sub-FN",
                        iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "CDRX Event",
                        iCdrxEvent);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item, "CDRX Event",
                        ValueNameCDRXEvent,
                        ARRAY_SIZE(ValueNameCDRXEvent, ValueName),
                        "(MI)Unknown");

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
    case 2:
        {
            offset += _decode_by_fmt(LteMl1CdrxEventsInfo_Payload_v2,
                    ARRAY_SIZE(LteMl1CdrxEventsInfo_Payload_v2, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LteMl1CdrxEventsInfo_Record_v2,
                        ARRAY_SIZE(LteMl1CdrxEventsInfo_Record_v2, Fmt),
                        b, offset, length, result_record_item);

                uint utemp = _search_result_uint(result_record_item, "SFN");
                int iSFN = utemp & 1023;
                int iSubFN = (utemp >> 10) & 15;
                int iCdrxEvent = (utemp >> 14) & 63;
                old_object = _replace_result_int(result_record_item, "SFN",
                        iSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Sub-FN",
                        iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "CDRX Event",
                        iCdrxEvent);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item, "CDRX Event",
                        ValueNameCDRXEvent,
                        ARRAY_SIZE(ValueNameCDRXEvent, ValueName),
                        "(MI)Unknown");
                utemp = _search_result_uint(result_record_item, "Internal Field Mask");
                std::string strInternalFieldMask = "";
                for (int i = 1; i <= 32; i++) {
                    if ((utemp >> (i - 1)) & 1 == 1) {
                        if (i == 4) {
                            strInternalFieldMask += "INACTIVITY_TIMER";
                        } else if (i == 8) {
                            strInternalFieldMask += "PENDING_UL_RETX";
                        } else {
                            strInternalFieldMask += (":(MI)Unknown-" + SSTR(i));
                        }
                    }
                }
                PyObject *pystr = Py_BuildValue("s", strInternalFieldMask.c_str());
                old_object = _replace_result(result_record_item, "Internal Field Mask",
                        pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);

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
        printf("(MI)Unknown LTE ML1 CDRX Events Info version: 0x%x\n", pkt_ver);
        return 0;
    }
}
