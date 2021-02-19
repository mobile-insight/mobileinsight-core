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
        case 4:
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
                int num_cells = _search_result_int(result, "Num Cells");

                PyObject *result_cells = PyList_New(0);
                for (int j = 0; j < num_cells; j++) {
                    PyObject *result_cells_item = PyList_New(0);
                    offset += _decode_by_fmt(LteNb1Ml1CellReselFmt_CellInfo_v4,
                            ARRAY_SIZE(LteNb1Ml1CellReselFmt_CellInfo_v4, Fmt),
                            b, offset, length, result_cells_item);

                    int temp = _search_result_int(result_cells_item, "RANK");
                    int iRANK = ((temp & 0xff) << 24) >> 24;
                    old_object = _replace_result_int(result_cells_item, "RANK",
                        iRANK);
                    Py_DECREF(old_object);

                    PyObject *t2 = Py_BuildValue("(sOs)", "Ignored",
                            result_cells_item, "dict");
                    PyList_Append(result_cells, t2);
                    Py_DECREF(t2);
                    Py_DECREF(result_cells_item);

                }
                PyObject *t1 = Py_BuildValue("(sOs)", "cells",
                        result_cells, "dict");
                PyList_Append(result_layers, t1);
                Py_DECREF(t1);
                Py_DECREF(result_cells);
            }
            PyObject *t = Py_BuildValue("(sOs)", "layers",
                    result_layers, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_layers);
            return offset - start;
        }
        default:
            printf("(MI)Unknown LTE NB1 ML1 Cell Resel Info version: 0x%x\n", pkt_ver);
            return 0;
    }
}