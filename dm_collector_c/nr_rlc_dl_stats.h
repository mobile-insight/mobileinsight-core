/*
 * NR5G_RLC_DL_Stats
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt NrRlcDlStats_Fmt [] = {
    {UINT, "Version", 4},
};

const Fmt NrRlcDlStats_v4 [] = {
    {UINT, "Num RB",               1},
    {UINT, "Reason",               1},
    {UINT, "Num FC Down Commands", 4},
    {UINT, "Num FC Up Commands",   4},
    {UINT, "Num Shutdown Issued",  4},
    {UINT, "Num RLF Issued",       4},
    {UINT, "Num Level1 Down Commands", 4},
    {UINT, "Num Level1 Up Commands",   4},
    {UINT, "Num Scell TA Start",   4},
    {UINT, "Num Scell TA Stop",    4},
    {UINT, "Num Scell FC Start",   4},
    {UINT, "Num Scell FC Stop",    4},
    {UINT, "Num RF Available",     4},
    {UINT, "Num FC RX Win",        4},
    {UINT, "Last RLF",             4},  
};

//Num FC Level1 Inc Segs Dropped List[4]
const Fmt NrRlcDlStats_NumFCLevel1IncSegDrop_v4 [] = {
    {UINT, "Num FC Level1 Inc Segs Dropped[0]", 4},
    {UINT, "Num FC Level1 Inc Segs Dropped[1]", 4},
    {UINT, "Num FC Level1 Inc Segs Dropped[2]", 4},
    {UINT, "Num FC Level1 Inc Segs Dropped[3]", 4},
};

const Fmt NrRlcDlStats_Others_v4 [] = {
    {UINT, "Num FC Level1 History Index", 4},
    {UINT, "Num FC Shutdown Inc Segs Dropped", 4},
    {SKIP, NULL, 2},
};

//RB Stats Based on Num RB 
const Fmt NrRlcDlStats_RBStats_v4 [] = {
    {SKIP, NULL,               1},
    {UINT, "RB Mode",          1},
    {SKIP, NULL,               4},
    {UINT, "Num Data PDU",     4},
    {UINT, "Num Stats Rxed",   4},
    {UINT, "Num Invalid PDUS", 4},
    {UINT, "Num ReTX PDU",     4},
    {UINT, "Num Duplicate PDU",4},
    {UINT, "Num Dropped PDU",  4},
    {UINT, "Num Segment Join", 4},
    {UINT, "Num Segment Collapse", 4},
    {UINT, "Num Status Trigger",   4},
    {UINT, "Num Status Update", 4},
    {UINT, "Num Status OTA",    4},
    {UINT, "Num Status PDU Drop",  4},
    {UINT, "Num Reassembly Expired",4},
    {UINT, "Num Reassembly Start", 4},
    {UINT, "Num Missed UM PDU",    4},
    {SKIP, NULL,                 4},
    {UINT, "Num Status Trigger Ack SN Only", 4},
    {UINT, "Num Status Trigger Nack List",   4},
    {UINT, "Num Status Update Ack SN Only",  4},
    {UINT, "Num Status Update Nack List",    4},
    {UINT, "Num Status Ackd",   4},
    {UINT, "Num Status Nack Lists", 4},
    {UINT, "RLC Data Bytes",    4},
    {SKIP, NULL, 2},
};

static int _decode_nr_rlc_dl_status_payload(const char *b,
		int offset, size_t length, PyObject *result){	
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;
    PyObject *pyfloat;

   switch(pkt_ver){
   case 4:
   {
       offset += _decode_by_fmt(NrRlcDlStats_v4,
           ARRAY_SIZE(NrRlcDlStats_v4, Fmt),
           b, offset, length, result);
       int num_rb = _search_result_int(result, "Num RB");

       //Num FC Level1 Inc Segs Dropped List[4]
       PyObject* result_NumFCLevel1IncSegDrop_item = PyList_New(0);
       offset += _decode_by_fmt(NrRlcDlStats_NumFCLevel1IncSegDrop_v4,
           ARRAY_SIZE(NrRlcDlStats_NumFCLevel1IncSegDrop_v4, Fmt),
           b, offset, length, result_NumFCLevel1IncSegDrop_item);
       PyObject* t0 = Py_BuildValue("(sOs)", "Num FC Level1 Inc Segs Dropped",
           result_NumFCLevel1IncSegDrop_item, "dict");
       PyList_Append(result, t0);
       Py_DECREF(t0);
       Py_DECREF(result_NumFCLevel1IncSegDrop_item);

       offset += _decode_by_fmt(NrRlcDlStats_Others_v4,
           ARRAY_SIZE(NrRlcDlStats_Others_v4, Fmt),
           b, offset, length, result);
       PyObject* result_NrRlcDlStats_RBStats = PyList_New(0);
       for (int i = 0; i < num_rb; i++) {
           PyObject* result_NrRlcDlStats_RBStats_item = PyList_New(0);
           offset += _decode_by_fmt(NrRlcDlStats_RBStats_v4,
               ARRAY_SIZE(NrRlcDlStats_RBStats_v4, Fmt),
               b, offset, length, result_NrRlcDlStats_RBStats_item);
           PyObject* t2 = Py_BuildValue("(sOs)", "Ignored",
               result_NrRlcDlStats_RBStats_item, "dict");
           PyList_Append(result_NrRlcDlStats_RBStats, t2);
           Py_DECREF(t2);
           Py_DECREF(result_NrRlcDlStats_RBStats_item);
       }

       PyObject* t3 = Py_BuildValue("(sOs)", "RB Stats",
           result_NrRlcDlStats_RBStats, "list");
       PyList_Append(result, t3);
       Py_DECREF(t3);
       Py_DECREF(result_NrRlcDlStats_RBStats);
       break;

    }
    default:
          printf("(MI)Unknown NR5G RLC DL Stats version:%d\n", pkt_ver);
    	  return 0;
   }
   return offset-start;
}
