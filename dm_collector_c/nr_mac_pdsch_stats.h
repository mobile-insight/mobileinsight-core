/*
 * NR_MAC_PDSCH_Stats
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt NrMacPdschStats_Fmt [] = {
    {UINT, "Minor Version",                 2},
    {UINT, "Major Version",                 2},
    {UINT, "Sleep",        1},
    {UINT, "Beam Change",        1},
    {UINT, "Signal Change",        1},
    {UINT, "DL Dynamic Cfg Change",        1},
    {UINT, "DL Config", 1},
    {UINT, "UL Config", 1},
    {SKIP, NULL,                   2},
    {UINT, "Log Fields Change BMask", 2},
    {SKIP, NULL,                   1},
    {UINT, "Num Records", 1},
    {SKIP, NULL,                   12}, // TODO: Some less important fields are skipped
};

const Fmt NrMacPdschStatsRecord_v2_2 [] = {
    {UINT, "Carrier ID", 4},
    {UINT, "Num Slots Elapsed", 4},
    {UINT, "Num PDSCH Decode", 4},
    {UINT, "Num CRC Pass TB", 4},
    {UINT, "Num CRC Fail TB", 4},
    {UINT, "Num ReTx", 4},
    {UINT, "ACK as NACK", 4},
    {UINT, "HARQ Failure", 4},
    {UINT, "CRC Pass TB Bytes", 8},
    {UINT, "CRC Fail TB Bytes", 8},
    {UINT, "TB Bytes", 8},
    {UINT, "Padding Bytes", 8},
    {UINT, "ReTx Bytes", 8},

};

static int
_decode_nr_mac_pdsch_stats_subpkt(const char *b, int offset, size_t length,
                       PyObject *result) {


    int start = offset;
    int major_ver = _search_result_int(result, "Major Version");
    int minor_ver = _search_result_int(result, "Minor Version");
    int n_record = _search_result_int(result, "Num Records");
    bool success = false;

    switch (major_ver) {
        case 2:{

            switch (minor_ver){
                case 2:{
                    PyObject *result_allrecords = PyList_New(0);
                    PyObject *t = NULL;

                    for(int i = 0; i < n_record; i++){
                        PyObject *result_record = PyList_New(0);
                        offset += _decode_by_fmt(NrMacPdschStatsRecord_v2_2,
                                                 ARRAY_SIZE(NrMacPdschStatsRecord_v2_2, Fmt),
                                                 b, offset, length, result_record);

                        // Add BLER estimation

                        int success_tb = _search_result_int(result_record, "Num CRC Pass TB");
                        int fail_tb = _search_result_int(result_record, "Num CRC Fail TB");
                        float bler = 100.0*fail_tb/(fail_tb+success_tb);
                        int n = PySequence_Length(result_record);
                        PyObject *pyfloat = Py_BuildValue("f", bler);

                        PyList_Insert(result_record, n , Py_BuildValue("(sOs)", "BLER (%)", pyfloat, ""));


                        char name[64];
                        sprintf(name, "Records[%d]", i);
                        t = Py_BuildValue("(sOs)", name, result_record, "dict");

                        PyList_Append(result_allrecords, t);

                        Py_DECREF(result_record);
                        Py_DECREF(t);
                    }

                    t = Py_BuildValue("(sOs)", "Records", result_allrecords, "list");
                    PyList_Append(result, t);
                    Py_DECREF(t);
                    Py_DECREF(result_allrecords);


                    success = true;
                    break;
                }
                default:
                    break;

            }

            
        }
        default:
            break;
    }

    if(!success){

        printf("(MI)Unknown 5G NR MAC UL TB Stats: %d.%d\n", major_ver, minor_ver);
    }

    return offset - start;

}