/*
 * LTE PHY CDRX Events Info
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LtePhyCdrxEventsInfo_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt LtePhyCdrxEventsInfo_Payload_v1 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePhyCdrxEventsInfo_Record_v1 [] = {
    {UINT, "SFN", 4},   // 10 bits
    {PLACEHOLDER, "Sub-FN", 0}, // 4 bits
    {PLACEHOLDER, "CDRX Event", 0}, // 6 bits
};

const Fmt LtePhyCdrxEventsInfo_Payload_v2 [] = {
    {UINT, "Num Records", 1},
    {SKIP, NULL, 2},
};

const Fmt LtePhyCdrxEventsInfo_Record_v2 [] = {
    {UINT, "SFN", 4},   // 10 bits
    {PLACEHOLDER, "Sub-FN", 0}, // 4 bits
    {PLACEHOLDER, "CDRX Event", 0}, // 6 bits
    {UINT, "Internal Field Mask", 4},   // 32 bits
};

static int _decode_lte_phy_cdrx_events_info_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;

    switch (pkt_ver) {
    case 1:
        {
            offset += _decode_by_fmt(LtePhyCdrxEventsInfo_Payload_v1,
                    ARRAY_SIZE(LtePhyCdrxEventsInfo_Payload_v1, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyCdrxEventsInfo_Record_v1,
                        ARRAY_SIZE(LtePhyCdrxEventsInfo_Record_v1, Fmt),
                        b, offset, length, result_record_item);

                unsigned int utemp = _search_result_uint(result_record_item, "SFN");
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
            offset += _decode_by_fmt(LtePhyCdrxEventsInfo_Payload_v2,
                    ARRAY_SIZE(LtePhyCdrxEventsInfo_Payload_v2, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Num Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePhyCdrxEventsInfo_Record_v2,
                        ARRAY_SIZE(LtePhyCdrxEventsInfo_Record_v2, Fmt),
                        b, offset, length, result_record_item);

                unsigned int utemp = _search_result_uint(result_record_item, "SFN");
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
                std::string strInternalFieldMask = "|";
                int count = 0;
                if (((utemp >> (1 - 1)) & 1) == 1) {
                    strInternalFieldMask += "CYCLE_START|";
                    count ++;
                }
                if (((utemp >> (3 - 1)) & 1) == 1) {
                    strInternalFieldMask += "ON_DURATION_TIMER|";
                    count ++;
                }
                if (((utemp >> (4 - 1)) & 1) == 1) {
                    strInternalFieldMask += "INACTIVITY_TIMER|";
                    count ++;
                }
                if (((utemp >> (5 - 1)) & 1) == 1) {
                    strInternalFieldMask += "DRX_RETX_TIMER|";
                    count ++;
                }
                if (((utemp >> (6 - 1)) & 1) == 1) {
                    strInternalFieldMask += "MISSING_CYCLE_TIMER|";
                    count ++;
                }
                if (((utemp >> (7 - 1)) & 1) == 1) {
                    strInternalFieldMask += "PENDING_SR|";
                    count ++;
                }
                if (((utemp >> (8 - 1)) & 1) == 1) {
                    strInternalFieldMask += "PENDING_UL_RETX|";
                    count ++;
                }
                if (((utemp >> (9 - 1)) & 1) == 1) {
                    strInternalFieldMask += "RACH|";
                    count ++;
                }
                if (((utemp >> (12 - 1)) & 1) == 1) {
                    strInternalFieldMask += "T310|";
                    count ++;
                }
                if (((utemp >> (16 - 1)) & 1) == 1) {
                    strInternalFieldMask += "WAKEUP_MISSED_CYCLE_TIMER|";
                    count ++;
                }
                if (((utemp >> (17 - 1)) & 1) == 1) {
                    strInternalFieldMask += "CATM1_UL_RETX_TIMER|";
                    count ++;
                }
                int check = 0;
                for (int i = 0; i < 32; i++) {
                    if (((utemp >> i) & 1) == 1) {
                        check ++;
                    }
                }
                if (check != count) {
                    if(check == 0){
                        strInternalFieldMask += "NO_EVENTS|";
                    }
                    else strInternalFieldMask += "(MI)Unknown|";
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
        printf("(MI)Unknown LTE PHY CDRX Events Info version: 0x%x\n", pkt_ver);
        return 0;
    }
}
