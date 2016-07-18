/*
 * 1xEV Connected State Search Info
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt _1xEVConnectedStateSearchInfo_Fmt [] = {
    {UINT, "Version", 1},
};

const Fmt _1xEVConnectedStateSearchInfo_Payload_v0 [] = {
    {UINT, "Serving Sector Handoffs Since Reset", 4},
    {UINT, "ASET Pilot Add Events Since Reset", 4},
    {UINT, "ASET Pilot Drop Events Since Reset", 4},
    {UINT, "RSET To CSET Pilot Add Events Since Reset", 4},
    {UINT, "NSET To RSET Pilot Drop Events Since Reset", 4},
    {UINT, "Drop Timer Start Events Since Reset", 4},
    {UINT, "Drop Timer Cancel Events Since Reset", 4},
    {UINT, "Drop Timer Expired Events Since Reset", 4},
    {UINT, "Off Frequency Searches Since Reset", 4},
    {UINT, "ASET Pilots For All Triages Since Reset", 4},
    {UINT, "CSET Pilots For All Triages Since Reset", 4},
    {UINT, "NSET Pilots For All Triages Since Reset", 4},
    {UINT, "Assigned Fingers For All Triages Since Reset", 4},
    {UINT, "Locked Fingers For All Triages Since Reset", 4},
    {UINT, "Triage Cycles Since Reset", 4},
    {UINT, "Serving Sector Handoffs Since Last Call", 4},
    {UINT, "ASET Pilot Add Events Since Last Call", 4},
    {UINT, "ASET Pilot Drop Events Since Last Call", 4},
    {UINT, "RSET To CSET Pilot Add Events Since Last Call", 4},
    {UINT, "NSET To RSET Pilot Drop Events Since Last Call", 4},
    {UINT, "Drop Timer Start Events Since Last Call", 4},
    {UINT, "Drop Timer Cancel Events Since Last Call", 4},
    {UINT, "Drop Timer Expired Events Since Last Call", 4},
    {UINT, "Off Frequency Searchs Since Last Call", 4},
    {UINT, "ASET Pilots For All Triages Since Last Call", 4},
    {UINT, "CSET Pilots For All Triages Since Last Call", 4},
    {UINT, "NSET Pilots For All Triages Since Last Call", 4},
    {UINT, "Assigned Fingers For All Triages Since Last Call", 4},
    {UINT, "Locked Fingers For All Triages Since Last Call", 4},
    {UINT, "Triage Cycles Since Last Call", 4},
    {UINT, "Current RSSI (dBm)", 1},
    {UINT, "Current Data Level Indicator", 1},
};

static int _decode_1xev_connected_state_search_info_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 0:
        {
            offset += _decode_by_fmt(_1xEVConnectedStateSearchInfo_Payload_v0,
                    ARRAY_SIZE(_1xEVConnectedStateSearchInfo_Payload_v0, Fmt),
                    b, offset, length, result);
            int iRSSI = _search_result_int(result, "Current RSSI (dBm)");
            iRSSI = 0 - iRSSI;
            PyObject *old_object = _replace_result_int(result,
                    "Current RSSI (dBm)", iRSSI);
            Py_DECREF(old_object);
            return offset - start;
        }
    default:
        printf("(MI)Unknown 1xEV Connected State Search Info version: 0x%x\n", pkt_ver);
        return 0;
    }
}
