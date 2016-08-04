/*
 * LTE ******
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt WcdmaRrcStates_Payload [] = {
    {UINT, "RRC State", 1},
};

static int _decode_wcdma_rrc_states_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    offset += _decode_by_fmt(WcdmaRrcStates_Payload,
            ARRAY_SIZE(WcdmaRrcStates_Payload, Fmt),
            b, offset, length, result);

    (void) _map_result_field_to_name(result, "RRC State",
            ValueNameWcdmaRrcStates,
            ARRAY_SIZE(ValueNameWcdmaRrcStates, ValueName),
            "(MI)Unknown");

    return offset - start;
}
