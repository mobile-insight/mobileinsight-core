/*
 * CDMA_Paging_Channel_Message
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt CdmaPagingChannelMsg_Fmt [] = {
    {UINT, "Message Length", 1},
    {UINT, "Message ID", 1},
};

const Fmt CdmaPagingChannelMsg_SystemParameters [] = {
    // pilot_pn: 9 bits
    // config_msg_seq: 6 bits
    // sid: 15 bits
    // nid 16 bits
    {UINT_BIG_ENDIAN, "SID", 4},
    // reg_zone: 12 bits
    // total_zones: 3 bits
    // zone_timer: 3 bits
    {UINT_BIG_ENDIAN, "NID", 4},
    // mult_sids: 1 bit
    // mult_nids: 1 bit
    // base_id: 16 bits
    // base_class 6 bits
    // page_channel: 1 bit
    // max_slot_cycle_index: 3 bits
    // hame_reg: 1 bit
    // for_sid_reg: 1 bit
    // for_nid_reg: 1 bit
    // power_up_reg: 1 bit
    {UINT_BIG_ENDIAN, "Base ID", 4},
    // power_down_reg: 1 bit
    // parameter_reg: 1 bit
    // reg_prd: 7 bits
    // base_lat: 22 bits
    // base_long: 23 bits - reamaining 22 bits
    {UINT_BIG_ENDIAN, "Base Latitude", 4},
    // reg_dist: 11 bits - reamining 1 bit
    {UINT_BIG_ENDIAN, "Base Longitude", 4},
    // srch_win_a: 4 bits
    // srch_win_n: 4 bits
    // srch_win_r: 4 bits
    // nghbr_max_age: 4 bits
    // pwr_rep_thresh: 5 bits
    // pwr_rep_frames: 4 bits
    // pwr_thresh_enable: 1 bit
    // pwr_period_enable: 1 bit
    // pwr_rep_delay: 5 bits - remaining 1 bit
    {SKIP, NULL, 4},
    // rescah: 1 bit
    // t_add: 6 bits
    // t_drop: 6 bits
    // t_comp: 4 bits
    // t_tdrop: 4 bits
    {UINT_BIG_ENDIAN, "T_Add", 4},
    {PLACEHOLDER, "T_Drop", 0},
    {PLACEHOLDER, "T_Comp", 0},
    {PLACEHOLDER, "T_TDrop", 0},
};

static int _decode_cdma_paging_channel_msg (const char *b,
        int offset, size_t length, PyObject *result) {
    int iMsgLength = _search_result_int(result, "Message Length");
    int iMessageID = _search_result_int(result, "Message ID");
    int finaloffset = offset + iMsgLength - 2;

    if (iMessageID != 1) {
        return offset;
    }

    PyObject *old_object;

    // Only support for System Parameters Msg
    offset += _decode_by_fmt(CdmaPagingChannelMsg_SystemParameters,
            ARRAY_SIZE(CdmaPagingChannelMsg_SystemParameters, Fmt),
            b, offset, length, result);

    unsigned int utemp = _search_result_uint(result, "SID");
    int iSID = (utemp >> 2) & 32767;
    int iNID = (utemp & 3) * 16384;
    utemp = _search_result_uint(result, "NID");
    iNID += (utemp >> 18) & 16383;

    old_object = _replace_result_int(result, "SID", iSID);
    Py_DECREF(old_object);
    old_object = _replace_result_int(result, "NID", iNID);
    Py_DECREF(old_object);

    utemp = _search_result_uint(result, "Base ID");
    int iBaseID = (utemp >> 14) & 65535;
    old_object = _replace_result_int(result, "Base ID", iBaseID);
    Py_DECREF(old_object);

    utemp = _search_result_uint(result, "Base Latitude");
    int iBaseLatitude = (utemp >> 1) & 4194303;
    if (((iBaseLatitude >> 21) & 1) == 1) {
        iBaseLatitude = iBaseLatitude - 4194304;
    }
    float fBaseLatitude = iBaseLatitude * 1.0 / 14400;
    PyObject *pyfloat = Py_BuildValue("f", fBaseLatitude);
    old_object = _replace_result(result, "Base Latitude", pyfloat);
    Py_DECREF(old_object);
    Py_DECREF(pyfloat);

    int iBaseLongitude = (utemp & 1) * 4194304;
    utemp = _search_result_uint(result, "Base Longitude");
    iBaseLongitude += (utemp >> 10) & 4194303;
    if (((iBaseLongitude >> 22) & 1) == 1) {
        iBaseLongitude = iBaseLongitude - 8388608;
    }
    float fBaseLongitude = iBaseLongitude * 1.0 / 14400;
    pyfloat = Py_BuildValue("f", fBaseLongitude);
    old_object = _replace_result(result, "Base Longitude", pyfloat);
    Py_DECREF(old_object);
    Py_DECREF(pyfloat);

    utemp = _search_result_uint(result, "T_Add");
    int iTAdd = (utemp >> 24) & 63;
    int iTDrop = (utemp >> 18) & 63;
    int iTComp = (utemp >> 14) & 15;
    int iTTDrop = (utemp >> 10) & 15;
    old_object = _replace_result_int(result, "T_Add", iTAdd);
    Py_DECREF(old_object);
    old_object = _replace_result_int(result, "T_Drop", iTDrop);
    Py_DECREF(old_object);
    old_object = _replace_result_int(result, "T_Comp", iTComp);
    Py_DECREF(old_object);
    old_object = _replace_result_int(result, "T_TDrop", iTTDrop);
    Py_DECREF(old_object);

    offset = finaloffset;
    return offset;
}
