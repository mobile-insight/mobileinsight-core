/*
 * 5G_NR_ML1_Serving_Cell_Beam_Management
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt NrMl1ServCellBeamMngt_Fmt [] = {
    {UINT, "Minor Version",                 2},
    {UINT, "Major Version",                 2},
    {UINT, "PCI",                 2},
    {SKIP,           NULL,                   2},
    {UINT, "SSB Periodicity Serv Cell (ms)",                 1},
    {UINT, "Serving Beam SSB Index",                 1},
    {SKIP,           NULL,                   2},
    {UINT, "RSRP Filtered",                 4},
    {UINT, "RSRQ Filtered",                 4},
    {SKIP,           NULL,                   8},
    {UINT, "Frequency Offset",                 4},
    {UINT, "Time Offset",                 4},
    {UINT, "Num Detected Beams",                 1},
    {SKIP,           NULL,                   3},
};

const Fmt NrMl1ServCellBeamRecord_v_2_1 [] = {
    {UINT, "Tx Beam Index",                 2},
    {SKIP,           NULL,                   2},
    {UINT, "RSRP Filtered",                 4},
    {UINT, "RSRQ Filtered",                 4},
    {SKIP,           NULL,                   4},

};

static int
_decode_nr_ml1_serv_cell_beam_mngt_subpkt(const char *b, int offset, size_t length,
                       PyObject *result) {


    int start = offset;
    int major_ver = _search_result_int(result, "Major Version");
    int minor_ver = _search_result_int(result, "Minor Version");
    int n_beam = _search_result_int(result, "Num Detected Beams");
    bool success = false;

    _convert_nr_rsrp(result,"RSRP Filtered");
    _convert_nr_rsrq(result,"RSRQ Filtered");

    unsigned int ssb_index = _search_result_int(result, "Serving Beam SSB Index");
    ssb_index = (ssb_index>>12) | (ssb_index>>4);
    PyObject *py_ssb_index = Py_BuildValue("I", ssb_index);
    _replace_result(result, "Serving Beam SSB Index", py_ssb_index);

    switch (major_ver) {
        case 2:{

            switch (minor_ver){
                case 1:{
                    PyObject *result_allrecords = PyList_New(0);
                    PyObject *t = NULL;

                    for(int i = 0; i < n_beam; i++){
                        PyObject *result_record = PyList_New(0);
                        offset += _decode_by_fmt(NrMl1ServCellBeamRecord_v_2_1,
                                                 ARRAY_SIZE(NrMl1ServCellBeamRecord_v_2_1, Fmt),
                                                 b, offset, length, result_record);


                        _convert_nr_rsrp(result_record,"RSRP Filtered");
                        _convert_nr_rsrq(result_record,"RSRQ Filtered");

                        char name[64];
                        sprintf(name, "Detected Beam[%d]", i);
                        t = Py_BuildValue("(sOs)", name, result_record, "dict");

                        PyList_Append(result_allrecords, t);

                        Py_DECREF(result_record);
                        Py_DECREF(t);
                    }

                    t = Py_BuildValue("(sOs)", "Detected Beams", result_allrecords, "list");
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

        printf("(MI)Unknown 5G ML1 Serving Cell Beam Management: %d.%d\n", major_ver, minor_ver);
    }

    return offset - start;

}


