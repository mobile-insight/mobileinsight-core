/*
 * gsm_rr_cell_reselection_param.h
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt GsmRrCellResParm_Fmt [] = {
    {UINT, "Cell Reselection Hysteresis", 1},
    {UINT, "MS_TXPWR_MAX_CCH", 1},
    {UINT, "RxLev Access Min", 1},
    {UINT, "Power Offset", 1},
    {UINT, "Power Offset Valid", 1},
    {UINT, "New Establishment Cause Indicator", 1},
    {UINT, "Additional Reselect Parameter Indicator", 1},
    {UINT, "Optional Reselect Parameter Indicator", 1},
    {UINT, "Cell Bar Qualify", 1},
    {UINT, "Cell Reselection Offset (dB)", 1},
    {UINT, "Temporary Offset (dB)", 1},
    {UINT, "Penalty Time", 1},
};

static int _decode_gsm_rcrp_payload (const char *b,
        int offset, size_t length, PyObject *result) {

    (void) _map_result_field_to_name(result, "RxLev Access Min",
            ValueNameGSMRxLevMin,
            ARRAY_SIZE(ValueNameGSMRxLevMin, ValueName),
            "(MI)Unknown");

    (void) _map_result_field_to_name(result, "Power Offset Valid",
            ValueNameTrueOrFalse,
            ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
            "(MI)Unknown");

    (void) _map_result_field_to_name(result, "New Establishment Cause Indicator",
            ValueNameSupportedOrNot,
            ARRAY_SIZE(ValueNameSupportedOrNot, ValueName),
            "(MI)Unknown");

    (void) _map_result_field_to_name(result, "Additional Reselect Parameter Indicator",
            ValueNameGSMAdditionalParam,
            ARRAY_SIZE(ValueNameGSMAdditionalParam, ValueName),
            "(MI)Unknown");

    (void) _map_result_field_to_name(result, "Optional Reselect Parameter Indicator",
            ValueNameYesOrNo,
            ARRAY_SIZE(ValueNameYesOrNo, ValueName),
            "(MI)Unknown");

    (void) _map_result_field_to_name(result, "Penalty Time",
            ValueNameGSMPenaltyTime,
            ARRAY_SIZE(ValueNameGSMPenaltyTime, ValueName),
            "(MI)Unknown");

    return offset;
}
