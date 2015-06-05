#ifndef __DM_ENDEC_C_LOG_PACKET_H__
#define __DM_ENDEC_C_LOG_PACKET_H__

enum FmtType { UINT, TIMESTAMP1, PLMN, SKIP };

struct Fmt {
    FmtType type;
    const char *field_name;
    int len;
};

const Fmt LogPacketHeaderFmt [] = {
    {UINT, "len1", 2},
    {UINT, "len2", 2},
    {UINT, "type_id", 2},
    {TIMESTAMP1, "timestamp", 8}
};

const Fmt WCDMACellIdFmt [] = {
    {UINT, "UTRA UL Absolute RF channel number", 4},
    {UINT, "UTRA DL Absolute RF channel number", 4},
    {UINT, "Cell identity (28-bits)", 4},
    {UINT, "URA to use in case of overlapping URAs", 1},
    {SKIP, NULL, 2},    // unknown yet
    {UINT, "Allowed Call Access", 1},
    {UINT, "PSC", 2},
    {PLMN, "PLMN", 6},
    {UINT, "LAC id", 4},
    {UINT, "RAC id", 4}
};

#endif  // __DM_ENDEC_C_LOG_PACKET_H__