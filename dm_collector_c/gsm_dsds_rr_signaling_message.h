/*
 * GSM_DSDS_RR_Signaling_Message
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt GsmDsdsRrSignalingMsg_Fmt [] = {
    {UINT, "Subscription ID", 1},
    {UINT, "Channel Type", 1},      // 7 bits
    {PLACEHOLDER, "Direction", 0},  // 1 bits
    {UINT, "Message Type", 1},
    {UINT, "Message Length in bytes", 1},
};

static int _decode_gsm_dsds_rr_signaling_msg_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int temp;
    PyObject *old_object;

    temp = _search_result_int(result, "Channel Type");
    int iChannelType = temp & 127;
    int iDirection = (temp >> 7) & 1;
    old_object = _replace_result_int(result, "Channel Type", iChannelType);
    Py_DECREF(old_object);
    (void) _map_result_field_to_name(result, "Channel Type",
            ValueNameGSMChannelType,
            ARRAY_SIZE(ValueNameGSMChannelType, ValueName),
            "(MI)Unknown");
    old_object = _replace_result_int(result, "Direction", iDirection);
    Py_DECREF(old_object);
    (void) _map_result_field_to_name(result, "Direction",
            ValueNameDirection,
            ARRAY_SIZE(ValueNameDirection, ValueName),
            "(MI)Unknown");
    int iMessageType = _search_result_int(result, "Message Type");
    (void) _map_result_field_to_name(result, "Message Type",
            ValueNameGSMSignalingMessageType,
            ARRAY_SIZE(ValueNameGSMSignalingMessageType, ValueName),
            "(MI)Unknown");
    switch(iMessageType) {
        case 7: // System Information Type 2quater
            break;
        default:
            break;
    }

    return offset;
}
