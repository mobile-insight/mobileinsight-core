/*
 * 5G NR ML1 Search Measurement Database update
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt NrMl1SearchMeasDatabaseUpdate_Fmt [] = {
    {UINT, "Minor Version",                 2},
    {UINT, "Major Version",                 2},
    {UINT, "Num Layers",                 1},
    {UINT,"SSB Periodicity Serv Cell",   1},
    {SKIP,           NULL,                   2},
};

const Fmt NrMl1SearchMeasCarrierList_v2_6 [] = {
    {UINT, "Raster ARFCN",                 4},
    {UINT, "Num Cells",                 1},
    {UINT, "Serving Cell Index",                 1},
    {UINT, "Serving Cell PCI",                 2},
    {UINT, "Serving SSB",                 1},
    {SKIP,           NULL,                   3},
    {SKIP, "ServingRsrpRx23",                 8}, // TODO: Currently NA, to be refined
    {UINT, "Serving Rx Beam 0",                 2}, 
    {UINT, "Serving Rx Beam 1",                 2}, 
    {UINT, "Serving RFIC ID",                 2}, 
    {SKIP,           NULL,                   2},
    {UINT, "Serving Subarray ID 0",                 2},
    {UINT, "Serving Subarray ID 1",                 2},
};

const Fmt NrMl1SearchMeasCellList_v2_6 [] = {
    {UINT, "PCI",                 2},
    {UINT, "PBCH SFN",            2},
    {UINT, "Num Beams",           1},
    {SKIP, NULL,                  3},
    {UINT, "CellQualityRsrp",     4},
    {UINT, "CellQualityRsrq",     4},
};

const Fmt NrMl1SearchMeasDetectedBeam_v2_6 [] = {
    {UINT, "SSB Index",           2},
    {SKIP, NULL,                  2},
    {UINT, "Rx Beam ID 0",           2},
    {UINT, "Rx Beam ID 1",           2},
    {SKIP, NULL,                  4},
    {UINT, "SSB Ref Timing 1",           4},
    {UINT, "SSB Ref Timing 2",           4},
    {UINT, "RSRP 0",           4},
    {UINT, "RSRP 1",           4},
    {UINT, "NR2NR Filtered Tx Beam RSRP L3",           4},
    {UINT, "NR2NR Filtered Tx Beam RSRQ L3",           4},
    {UINT, "L2 NR Filtered Tx Beam RSRP",           4},
    {UINT, "L2 NR Filtered Tx Beam RSRQ",           4},
};
const Fmt NrMl1SearchMeasFmtsubpkt_v2_7[] = {
    {UINT,"Frequency Offset",4},
    {UINT,"Timing Offset",4},
};
const Fmt NrMl1SearchMeasCarrierList_v2_7[] = {
    {UINT,"Raster ARFCN",4},
    {UINT,"Num Cells",1},
    {UINT,"Serving Cell Index",1},
    {UINT,"Serving Cell PCI",2},
    {UINT,"Serving SSB",1},
    {SKIP,NULL,3},
    {UINT,"ServingRsrpRx23[0]",4},
    {UINT,"ServingRsrpRx23[1]",4},
    {UINT,"Serving RX Beam[0]",2},
    {UINT,"Serving RX Beam[1]",2},
    {UINT,"Serving RFIC ID",2},
    {SKIP,NULL,2},
    {UINT,"ServingSubarrayId[0]",2},
    {UINT,"ServingSubarrayId[1]",2},
};
const Fmt NrMl1SearchMeasCellList_v2_7[] = {
    {UINT,"PCI",2},
    {UINT,"PBCH SFN",2},
    {UINT,"Num Beams",1},
    {SKIP,NULL,3},
    {UINT,"Cell Quality Rsrp",4},
    {UINT,"Cell Quality Rsrq",4},
};

const Fmt NrMl1SearchMeasDetectedBeam_v2_7[] = {
    {UINT,"SSB Index",2},
    {SKIP,NULL,2},
    {UINT,"Rx Beam Id[0]",2},
    {UINT,"Rx Beam Id[1]",2},
    {SKIP,NULL,4},
    {BYTE_STREAM_LITTLE_ENDIAN,"SSB Ref Timing ",8},
    {UINT,"RX Beam Info-RSRPs[0]",4},
    {UINT,"RX Beam Info-RSRPs[1]",4},
    {UINT,"Nr2NrFilteredBeamRsrpL3",4},
    {UINT,"Nr2NrFilteredBeamRsrqL3",4},
    {UINT,"L2NrFilteredTxBeamRsrpL3",4},
    {UINT,"L2NrFilteredTxBeamRsrqL3",4},

};

static void
_convert_nr_rsrp_rsrq_v2_7(PyObject * obj, const char* rsrp_field) {
    int utemp = _search_result_uint(obj, rsrp_field);
    if(utemp==0){
    PyObject * pyfloat = Py_BuildValue("s", "NA");
    PyObject * old_object = _replace_result(obj, rsrp_field, pyfloat);
    Py_DECREF(old_object);
    Py_DECREF(pyfloat);
    return;
    }

    int integer = (utemp >> 7) & 0xff;
    int frac = utemp & 0x7f;
    float r = (((integer^0xff)+1)*(-1)) + frac * 0.0078125;
    char  quality[64];
    sprintf(quality,"%.3f",r);
    PyObject * pyfloat = Py_BuildValue("s", quality);
    PyObject * old_object = _replace_result(obj, rsrp_field, pyfloat);
    Py_DECREF(old_object);
    Py_DECREF(pyfloat);
}

static void
_judge_na_v2_7(PyObject* obj, const char* field,int standard) {
    int tmp = _search_result_int(obj, field);
    if (tmp == standard) {
        std:: string na="NA";
        PyObject* old_object = _replace_result_string(obj, field, na);
        Py_DECREF(old_object);
    }
}



static int
_decode_nr_ml1_search_meas_subpkt(const char *b, int offset, size_t length,
                       PyObject *result) {


    int start = offset;
    int major_ver = _search_result_int(result, "Major Version");
    int minor_ver = _search_result_int(result, "Minor Version");
    int n_layer = _search_result_int(result, "Num Layers");
    bool success = false;

    switch (major_ver) {
        case 2:{

            switch (minor_ver){
                case 6:{
                    PyObject *result_allcarriers = PyList_New(0);
                    PyObject *t = NULL;
                    _delete_result(result, "SSB Periodicity Serv Cell");
         
                    for(int i = 0; i < n_layer; i++){
                        PyObject *result_layer = PyList_New(0);
                        offset += _decode_by_fmt(NrMl1SearchMeasCarrierList_v2_6,
                                                 ARRAY_SIZE(NrMl1SearchMeasCarrierList_v2_6, Fmt),
                                                 b, offset, length, result_layer);

                        int n_cells = _search_result_int(result_layer, "Num Cells");
                        PyObject *result_allcells = PyList_New(0);

                        for(int j = 0; j < n_cells; j++){

                            PyObject *result_cell = PyList_New(0);
                            offset += _decode_by_fmt(NrMl1SearchMeasCellList_v2_6,
                                                 ARRAY_SIZE(NrMl1SearchMeasCellList_v2_6, Fmt),
                                                 b, offset, length, result_cell);

                            _convert_nr_rsrp(result_cell, "CellQualityRsrp");
                            _convert_nr_rsrp(result_cell, "CellQualityRsrq");


                            int n_beams = _search_result_int(result_cell, "Num Beams");

                            for(int k = 0; k < n_beams; k++){
                                PyObject *result_beam = PyList_New(0);
                                offset += _decode_by_fmt(NrMl1SearchMeasDetectedBeam_v2_6,
                                                 ARRAY_SIZE(NrMl1SearchMeasDetectedBeam_v2_6, Fmt),
                                                 b, offset, length, result_beam);

                                _convert_nr_rsrp(result_beam, "RSRP 0");
                                _convert_nr_rsrp(result_beam, "RSRP 1");

                                _convert_nr_rsrp(result_beam, "NR2NR Filtered Tx Beam RSRP L3");
                                _convert_nr_rsrq(result_beam, "NR2NR Filtered Tx Beam RSRQ L3");

                                _convert_nr_rsrp(result_beam, "L2 NR Filtered Tx Beam RSRP");
                                _convert_nr_rsrq(result_beam, "L2 NR Filtered Tx Beam RSRQ");


                                char name_beam[64];
                                sprintf(name_beam, "Beams[%d]", k);
                                PyObject *t_beam = Py_BuildValue("(sOs)", name_beam, result_beam, "dict");
                                PyList_Append(result_cell,t_beam);

                                Py_DECREF(t_beam);
                                Py_DECREF(result_beam);

                            }

                            char name_cell[64];
                            sprintf(name_cell, "Cells[%d]", j);
                            PyObject *t_cell = Py_BuildValue("(sOs)", name_cell, result_cell, "dict");
                            PyList_Append(result_allcells,t_cell);

                            Py_DECREF(t_cell);
                            Py_DECREF(result_cell);
                        }

                        PyObject *t_allcells = Py_BuildValue("(sOs)", "Cells", result_allcells, "dict");

                        PyList_Append(result_layer,t_allcells);

                        char name[64];
                        sprintf(name, "Component_Carrier List[%d]", i);
                        t = Py_BuildValue("(sOs)", name, result_layer, "dict");

                        PyList_Append(result_allcarriers, t);

                        Py_DECREF(result_allcells);
                        Py_DECREF(t);
                        Py_DECREF(t_allcells);
                        Py_DECREF(result_layer);
                    }

                    t = Py_BuildValue("(sOs)", "Component_Carrier List", result_allcarriers, "list");
                    PyList_Append(result, t);
                    Py_DECREF(t);
                    Py_DECREF(result_allcarriers);


                    success = true;
                    break;
                }
                case 7:
                {
                    PyObject* result_allcarriers = PyList_New(0);
                    PyObject* t = NULL;
                    _judge_na_v2_7(result,"SSB Periodicity Serv Cell",0x00);
                    offset += _decode_by_fmt(NrMl1SearchMeasFmtsubpkt_v2_7,
                        ARRAY_SIZE(NrMl1SearchMeasFmtsubpkt_v2_7, Fmt),
                        b, offset, length, result);
                    int temp = _search_result_int(result, "Frequency Offset");
                    float t_offset = temp * 0.0009766;
                    char f_offset[64];
                    sprintf(f_offset,"%.3f PPM",t_offset);
                    PyObject* pyfloat = Py_BuildValue("s", f_offset);
                    PyObject* old_object = _replace_result(result, "Frequency Offset", pyfloat);
                    Py_DECREF(old_object);
                    Py_DECREF(pyfloat);
                    for (int i = 0; i < n_layer; i++) {
                        PyObject* result_layer = PyList_New(0);
                        offset += _decode_by_fmt(NrMl1SearchMeasCarrierList_v2_7,
                            ARRAY_SIZE(NrMl1SearchMeasCarrierList_v2_7, Fmt),
                            b, offset, length, result_layer);

                        int n_cells = _search_result_int(result_layer, "Num Cells");
                        PyObject* result_allcells = PyList_New(0);
                        _convert_nr_rsrp_rsrq_v2_7(result_layer, "ServingRsrpRx23[0]");
                        _convert_nr_rsrp_rsrq_v2_7(result_layer, "ServingRsrpRx23[1]");

                        _judge_na_v2_7(result_layer, "Serving SSB", 0xff);
                        _judge_na_v2_7(result_layer, "Serving RX Beam[0]", 0xffff);
                        _judge_na_v2_7(result_layer, "Serving RX Beam[1]", 0xffff);
                        _judge_na_v2_7(result_layer, "Serving RFIC ID", 0xffff);
                        _judge_na_v2_7(result_layer, "ServingSubarrayId[0]", 0xffff);
                        _judge_na_v2_7(result_layer, "ServingSubarrayId[1]", 0xffff);
                        for (int j = 0; j < n_cells; j++) {

                            PyObject* result_cell = PyList_New(0);
                            offset += _decode_by_fmt(NrMl1SearchMeasCellList_v2_7,
                                ARRAY_SIZE(NrMl1SearchMeasCellList_v2_7, Fmt),
                                b, offset, length, result_cell);
                            _convert_nr_rsrp_rsrq_v2_7(result_cell, "Cell Quality Rsrp");
                            _convert_nr_rsrp_rsrq_v2_7(result_cell, "Cell Quality Rsrq");
                            int n_beams = _search_result_int(result_cell, "Num Beams");
                            for (int k = 0; k < n_beams; k++) {
                                PyObject* result_beam = PyList_New(0);
                                offset += _decode_by_fmt(NrMl1SearchMeasDetectedBeam_v2_7,
                                    ARRAY_SIZE(NrMl1SearchMeasDetectedBeam_v2_7, Fmt),
                                    b, offset, length, result_beam);
                                _judge_na_v2_7(result_beam,"Rx Beam Id[0]",0x0000);
                                _judge_na_v2_7(result_beam,"Rx Beam Id[1]",0x0000);
                                _convert_nr_rsrp_rsrq_v2_7(result_beam, "RX Beam Info-RSRPs[0]");
                                _convert_nr_rsrp_rsrq_v2_7(result_beam, "RX Beam Info-RSRPs[1]");
                                _convert_nr_rsrp_rsrq_v2_7(result_beam, "Nr2NrFilteredBeamRsrpL3");
                                _convert_nr_rsrp_rsrq_v2_7(result_beam, "Nr2NrFilteredBeamRsrqL3");
                                _convert_nr_rsrp_rsrq_v2_7(result_beam, "L2NrFilteredTxBeamRsrpL3");
                                _convert_nr_rsrp_rsrq_v2_7(result_beam, "L2NrFilteredTxBeamRsrqL3");
                                char name_beam[64];
                                sprintf(name_beam, "Beams[%d]", k);
                                PyObject* t_beam = Py_BuildValue("(sOs)", name_beam, result_beam, "dict");
                                PyList_Append(result_cell, t_beam);

                                Py_DECREF(t_beam);
                                Py_DECREF(result_beam);
                            }
                            char name_cell[64];
                            sprintf(name_cell, "Cells[%d]", j);
                            PyObject* t_cell = Py_BuildValue("(sOs)", name_cell, result_cell, "dict");
                            PyList_Append(result_allcells, t_cell);

                            Py_DECREF(t_cell);
                            Py_DECREF(result_cell);
                        }
                        PyObject* t_allcells = Py_BuildValue("(sOs)", "Cells", result_allcells, "list");

                        PyList_Append(result_layer, t_allcells);

                        char name[64];
                        sprintf(name, "Component_Carrier List[%d]", i);
                        t = Py_BuildValue("(sOs)", name, result_layer, "dict");

                        PyList_Append(result_allcarriers, t);

                        Py_DECREF(result_allcells);
                        Py_DECREF(t);
                        Py_DECREF(t_allcells);
                        Py_DECREF(result_layer);
                    }
                    t = Py_BuildValue("(sOs)", "Component_Carrier List", result_allcarriers, "list");
                    PyList_Append(result, t);
                    Py_DECREF(t);
                    Py_DECREF(result_allcarriers);
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

        printf("(MI)Unknown 5G NR ML1 Search Meas Database Update: %d.%d\n", major_ver, minor_ver);
    }

    return offset - start;

}



