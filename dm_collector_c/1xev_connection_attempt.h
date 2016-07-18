/*
 * 1xEV Connection Attempt
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt _1xEVConnectionAttempt_Fmt [] = {
    {UINT, "Transaction Id", 1},
    {BYTE_STREAM, "Message Sequence", 1},
    {UINT, "Conn Attempt Outcome", 1},
    {PLACEHOLDER, "Stat of TCA/RTCACK Msgs", 0},
    {UINT, "Duration", 2},
    {UINT, "Success count", 4},
    {UINT, "Failure count", 4},
    {UINT, "Attempts count", 4},
    {UINT, "PN", 2},
    {BYTE_STREAM, "Sector ID (lower 16 bits)", 2},
    {BYTE_STREAM, "Sector ID (upper 8 bits)", 1},
    {UINT, "Color Code", 1},
    {UINT, "Active Set Pilot Changed Count", 1},
};
const ValueName _1xEVConnectionAttempt_Outcome [] = {
    {15, "Success"},
    {12, "Reserved"},
};
const ValueName _1xEVConnectionAttempt_Stat [] = {
    {0, "Not received either TCA or RTCACK message"},
    {2, "Received both TCA and RTCACK message"},
};

static int _decode_1xev_connection_attempt_payload (const char *b,
        int offset, size_t length, PyObject *result) {

    int iNonDecodeP = _search_result_int(result, "Conn Attempt Outcome");
    int iOutcome = iNonDecodeP & 15; // last 4 bits
    int iStat = (iNonDecodeP >> 4) & 15; // next 4 bits
    PyObject *old_object = _replace_result_int(result, "Conn Attempt Outcome",
            iOutcome);
    Py_DECREF(old_object);
    (void) _map_result_field_to_name(result, "Conn Attempt Outcome",
            _1xEVConnectionAttempt_Outcome,
            ARRAY_SIZE(_1xEVConnectionAttempt_Outcome, ValueName),
            "(MI)Unknown");
    old_object = _replace_result_int(result, "Stat of TCA/RTCACK Msgs",
            iStat);
    Py_DECREF(old_object);
    (void) _map_result_field_to_name(result, "Stat of TCA?RTCACK Msgs",
            _1xEVConnectionAttempt_Stat,
            ARRAY_SIZE(_1xEVConnectionAttempt_Stat, ValueName),
            "(MI)Unknown");
    return 0;

}
