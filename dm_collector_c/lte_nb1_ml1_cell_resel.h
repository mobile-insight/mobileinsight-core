/*
 * LTE NB1 ML1 Cell Resel
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt LteNb1Ml1CellReselFmt[] = {
    {UINT, "Version", 1},

};

const Fmt LteNb1Ml1CellReselFmt_v4[] = {
    {UINT, "Num Layers", 1},
    {SKIP, NULL, 2}, //Reserved
};

const Fmt LteNb1Ml1CellReselFmt_LayerInfo_v4[] = {
    {UINT, "Num Cells", 2},
    {UINT, "Frequency", 2}, 

};
const Fmt LteNb1Ml1CellReselFmt_CellInfo_v4[] = {
    {UINT, "PCI", 2},
    {UINT, "RANK", 2},
    {UINT, "Tresel Value", 2}, 
    {SKIP, NULL, 2}, //Reserved
};

static int _decode_lte_nb1_ml1_cell_resel_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    PyObject *old_object;

    switch (pkt_ver) {
        case 3:
        {
        	offset += _decode_by_fmt(LteNb1Ml1CellReselFmt_v4,
                    ARRAY_SIZE(LteNb1Ml1CellReselFmt_v4, Fmt),
                    b, offset, length, result);
            int num_layers = _search_result_int(result, "Num Layers");

            PyObject *result_layers = PyList_New(0);
            for (int i = 0; i < num_layers; i++) {
                offset += _decode_by_fmt(LteNb1Ml1CellReselFmt_LayerInfo_v4,
                    ARRAY_SIZE(LteNb1Ml1CellReselFmt_LayerInfo_v4, Fmt),
                    b, offset, length, result);

                PyObject *result_cells = PyList_New(0);
                for (int j = 0; j < 4; j++) {
                    PyObject *result_cells_item = PyList_New(0);
                    offset += _decode_by_fmt(LteNb1Ml1CellReselFmt_CellInfo_v4,
                            ARRAY_SIZE(LteNb1Ml1CellReselFmt_CellInfo_v4, Fmt),
                            b, offset, length, result_cells_item);

                    unsigned int iNonDecodeHSFN = _search_result_uint(result_cells_item, "NPDCCH Timing HSFN");
                    int iHSFN = iNonDecodeHSFN & 1023;          // 10 bits
                    int iSFN = (iNonDecodeHSFN >> 10) & 1023;   // 10 bits

                    old_object = _replace_result_int(result_cells_item, "SC Index",
                            iSC_I);
                    Py_DECREF(old_object);

                    PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                            result_cells_item, "dict");
                    PyList_Append(result_cells, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_cells_item);

                }
                PyObject *t1 = Py_BuildValue("(sOs)", "cells",
                        result_layers, "dict");
                PyList_Append(result_layers, t1);
                Py_DECREF(t1);
                Py_DECREF(result_layers_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "layers",
                    result_layers, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_layers);
            return offset - start;
        }
        default:
            printf("(MI)Unknown LTE NB1 ML1 GM DCI Info version: 0x%x\n", pkt_ver);
            return 0;
    }
}