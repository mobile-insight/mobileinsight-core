/*
 * 1xEV Connection Release
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt _1xEVConnectionRelease_Fmt [] = {
    {UINT, "Reason", 1},
};
const ValueName _1xEVConnectionRelease_Reason [] = {
    {0, "AN Connection Close"},
};

static int _decode_1xev_connection_release_payload (const char *b,
        int offset, size_t length, PyObject *result) {

    (void) _map_result_field_to_name(result, "Reason",
            _1xEVConnectionRelease_Reason,
            ARRAY_SIZE(_1xEVConnectionRelease_Reason, ValueName),
            "(MI)Unknown");
    return 0;
}
