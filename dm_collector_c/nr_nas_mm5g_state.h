#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"


const Fmt NrNasMm5gState_Fmt[] = {
    {UINT, "Version", 4},
};

const Fmt NrNasMm5gStateSubpkt_Fmt_v1[] = {
    {UINT,"MM5G State",1},
    {UINT,"Mm5g Deregistered Substate",2},
    {UINT_BIG_ENDIAN,"PLMN Id",4},
};

const Fmt NrNasMm5gStateGuti_Fmt_v1[] = {
    {PLACEHOLDER,"UE Id",0},
    {UINT_BIG_ENDIAN,"GUTI PLMN ID",4},
    {PLACEHOLDER,"AMF Region ID",0},
    {BYTE_STREAM,"AMF Set ID",2},
    {BYTE_STREAM,"AMF Pointer",1},
    {UINT_BIG_ENDIAN,"5G TMSI",4},
};

const Fmt NrNasMm5gStateOther_Fmt_v1[] = {
    {UINT,"MM5G Update Status",1},
//    {UINT_BIG_ENDIAN,"NR5G TAC",3},
    {UINT,"Octet[0]",1},
    {UINT,"Octet[1]",1},
    {UINT,"Octet[2]",1},        
};


const ValueName NrMM5GState_v1[] = {
    {1,"DEREGISTERED"},
    {2,"REGISTERED_INITIATED"},
    {3,"REGISTERED"},
    {4,"SERVICE_REQUEST_INITIATED"},
};

const ValueName NrMm5gDeregisteredSubstate_v1[] = {
    {0,"NORMAL_SERVICE"},
    {1,"PLMN_SEARCH"},
    {2,"NO_CELL_AVAILABLE"},
    {5,"LIMITED_SERVICE"},
};

const ValueName NrMM5GUpdateStatus_v1[] = {
    {0,"UPDATED"},
    {1,"NOT_UPDATED"},
};
const ValueName UEId_v1[] = {
    {2,"5G_GUTI"},
};

static int 
_decode_nr_nas_mm5g_state(const char* b,
    int offset, size_t length, PyObject* result)
{
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    bool success=0;
    PyObject* old_object;
    PyObject* pyfloat;
    switch (pkt_ver) {
    case 1:
    {
        offset+= _decode_by_fmt(NrNasMm5gStateSubpkt_Fmt_v1,
            ARRAY_SIZE(NrNasMm5gStateSubpkt_Fmt_v1, Fmt),
            b, offset, length, result);
        (void)_map_result_field_to_name(result,
            "MM5G State", NrMM5GState_v1,
            ARRAY_SIZE(NrMM5GState_v1, ValueName),
            "(MI)Unknown");

        (void)_map_result_field_to_name(result,
            "Mm5g Deregistered Substat", NrMm5gDeregisteredSubstate_v1,
            ARRAY_SIZE(NrMm5gDeregisteredSubstate_v1, ValueName),
            "(MI)Unknown");
        
        int tmp = _search_result_uint(result, "PLMN Id");
        char id[64];
        sprintf(id, "{0x%x,0x%x,0x%x}", (tmp >> 24)&0xff, (tmp >> 16) & 0xff,(tmp>>8) & 0xff);
        PyObject* t = Py_BuildValue("s", id);
        old_object = _replace_result(result, "PLMN Id", t);
        Py_DECREF(old_object);
        Py_DECREF(t);

        PyObject* guti_result = PyList_New(0);
        offset += _decode_by_fmt(NrNasMm5gStateGuti_Fmt_v1,
            ARRAY_SIZE(NrNasMm5gStateGuti_Fmt_v1, Fmt),
            b, offset, length, guti_result);
        char ue_id[64];
        sprintf(ue_id, "0x%x", tmp & 0xff);
        t = Py_BuildValue("s", ue_id);
        old_object = _replace_result(guti_result, "UE Id", t);
        Py_DECREF(old_object);
//        (void)_map_result_field_to_name(guti_result,
//            "UE Id", UEId_v1,
//            ARRAY_SIZE(UEId_v1, ValueName),
//            "(MI)Unknown");        

        char guti_plmn_id[64];
        tmp = _search_result_uint(guti_result, "GUTI PLMN ID");
        sprintf(guti_plmn_id, "{0x%x,0x%x,0x%x}", (tmp >> 24)&0xff, (tmp >> 16) & 0xff, (tmp >> 8) & 0xff);
        t = Py_BuildValue("s", guti_plmn_id);
        old_object = _replace_result(guti_result, "GUTI PLMN ID", t);
        Py_DECREF(old_object);

        char amf_region_id[64];
        sprintf(amf_region_id, "0x%x", tmp & 0xff);
        t = Py_BuildValue("s", amf_region_id);
        old_object = _replace_result(guti_result, "AMF Region ID", t);
        Py_DECREF(old_object);

        char tmsi[64];
        tmp = _search_result_uint(guti_result, "5G TMSI");
        sprintf(tmsi, "{0x%x,0x%x,0x%x,0x%x}", (tmp >> 24)&0xff, (tmp >> 16) & 0xff, (tmp >> 8) & 0xff,tmp&0xff);
        t = Py_BuildValue("s", tmsi);
        old_object = _replace_result(guti_result, "5G TMSI", t);
        Py_DECREF(old_object);
        t = Py_BuildValue("(sOs)", "GUTI", guti_result, "dict");
	    PyList_Append(result, t);
	    Py_DECREF(guti_result);
	    Py_DECREF(t);

        offset += _decode_by_fmt(NrNasMm5gStateOther_Fmt_v1,
            ARRAY_SIZE(NrNasMm5gStateOther_Fmt_v1, Fmt),
            b, offset, length, result);
        
        (void)_map_result_field_to_name(result,
            "MM5G Update Status", NrMM5GUpdateStatus_v1,
            ARRAY_SIZE(NrMM5GUpdateStatus_v1, ValueName),
            "(MI)Unknown");
        
//        char tac[64];
//        int tmp2 = _search_result_int(result, "NR5G TAC");
//        sprintf(tac, "{0x%x,0x%x,0x%x}", tmp2>>16, (tmp2 >> 8) & 0xff, tmp2 & 0xff);
//        t = Py_BuildValue("s", tac);
//        old_object = _replace_result(result, "NR5G TAC", t);
//        Py_DECREF(old_object);

        success = 1;
        break;
    }
    default:
        break;
    }
    if (!success) {
        printf("(MI)Unknown NR5G RLC DL Stats version:%d\n", pkt_ver);
    }
    return offset - start;
}
