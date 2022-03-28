#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"
#include <string>
const Fmt NrMacRachTriggerFmt[] = {
    {UINT,"MAC version Minor",2},
    {UINT,"MAC version Major",2},
    
};
const Fmt NrMacRachTriggerSubpkt_Fmt_v2_1[] = {
    {UINT,"Sleep",        1},
    {SKIP,NULL,           2},
    {UINT,"DL Dynamic Cfg Change",1},
    {UINT,"DL Config",    1},
    {UINT,"ML1 State Change", 1},
    {PLACEHOLDER,"UL config",0},
    {SKIP,NULL,           2},
    {UINT,"Log Fields Change Bmask",2},
    {UINT,"Sub Id",       1},
    {UINT,"Num Records",  1},
    {UINT,"CRNTI",        2},
    {UINT,"Rach Reason",  1},
    {UINT,"Carrier Id",   1},
    {UINT,"First Active UL BWP",1},
    {UINT,"RACH Contention",1},
    {UINT,"RA ID",        1},
    {UINT,"MSG3 SIZE",    1},
};
const Fmt NrMacRachTriggerFmt_ForMsglist_v2_1[] = {
    {UINT,"MSG3",1},
};
const ValueName NrMACRachTriggerReason_v2_1[] = {
        {0x00,"Connection_Request"},
        {0x01,"RADIO_LINK_FAILURE"},
        {0x02,"HANDOVER"},
        {0x03,"UL_DATA_ARRIVAL"},
        {0x06,"Beam_Failure",}
};
const ValueName NrMACRachTriggerRachContention_v2_1[] = {
        {0x00,"CONT_FREE"},
        {0x01,"DL_MCE"},
        {0x02,"UL_Grant"},
        {0x03,"PDCCH_CRNTI"},
};
static int
_decode_nr_mac_rach_trigger(const char* b, int offset, size_t length,
    PyObject* result) {
    int start = offset;
    int Major = _search_result_int(result, "MAC version Major");
    int Minor = _search_result_int(result, "MAC version Minor");
    bool success = 0;
    switch (Major)
    {
    case 2: {
        switch (Minor)
        {
        case 1:
        {
            offset += _decode_by_fmt(NrMacRachTriggerSubpkt_Fmt_v2_1,
                ARRAY_SIZE(NrMacRachTriggerSubpkt_Fmt_v2_1, Fmt),
                b, offset, length, result);
            (void)_map_result_field_to_name(result,
                "Rach Reason", NrMACRachTriggerReason_v2_1,
                ARRAY_SIZE(NrMACRachTriggerReason_v2_1, ValueName),
                "(MI)Unknown");
            (void)_map_result_field_to_name(result,
                "RACH Contention", NrMACRachTriggerRachContention_v2_1,
                ARRAY_SIZE(NrMACRachTriggerRachContention_v2_1, ValueName),
                "(MI)Unknown");

            int tmp = _search_result_int(result, "ML1 State Change");
            int UL_config = tmp & 0x0F;
            int ml1_state_change = tmp >> 4;
            PyObject* old_object = _replace_result_int(result, "ML1 State Change", ml1_state_change);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "UL config", UL_config);
            Py_DECREF(old_object);

            std::string msg_list = "{ ";
            for (int i = 0; i < 6; i++) {
                PyObject* result_record_item = PyList_New(0);
                offset += _decode_by_fmt(NrMacRachTriggerFmt_ForMsglist_v2_1,
                    ARRAY_SIZE(NrMacRachTriggerFmt_ForMsglist_v2_1, Fmt),
                    b, offset, length, result_record_item);
                int tmp = _search_result_int(result_record_item, "MSG3");
                if (i == 0) {
                    msg_list += std::to_string(tmp);
                }
                else {
                    msg_list += "," + std::to_string(tmp);
                }
            }
            msg_list += " }";
            const char* msg_list_str=msg_list.c_str();
            PyObject* t = Py_BuildValue("(sss)", "MSG3 LIST",
                msg_list_str, "");
            PyList_Append(result, t);
            Py_DECREF(t);
            success = 1;
            break;
        }
        default:
            break;
        }
    default:
        break;
    }
    }
    if (!success) {
        printf("(MI)Unknown 5G NR MAC RACH Trigger version: %d.%d\n", Major, Minor);
    }
    return offset - start;
}
