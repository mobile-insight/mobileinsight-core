/*
 * NR_MAC_UL_Physical_Channel_Schedule_Report
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"
#include <string>


const Fmt NrMacVersion_Fmt [] = {
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
};

const Fmt NrMacUlSchedRecord_v2_8 [] = {
    {UINT, "Slot", 1},
    {UINT, "Numerology (kHz)", 1},
    {UINT, "Frame", 2},
    {UINT, "Num Carrier", 1},
    {SKIP, NULL,3},
};

const Fmt NrMacUlSchedCarrierRecord_v2_8 [] = {
    // {UINT, "Carrier ID", 1},   // Carrier ID + RNTI type
    // {UINT, "PhyChan BitMask", 1},
    {UINT, "PhyChan BitMask", 2},
    {SKIP, NULL,2},
};

const Fmt NrMacUlSchedPUSCHRecord_v2_8 [] = {
    {BIT_STREAM_LITTLE_ENDIAN, "PUSCH Raw Data", 40},
};

const Fmt NrMacUlSchedPUCCHRecord_v2_8 [] = {
    {BIT_STREAM_LITTLE_ENDIAN, "PUCCH Raw Data", 20},
};

const Fmt NrMacUlSchedSrsRecord_v2_8 [] = {
    {BIT_STREAM_LITTLE_ENDIAN, "SRS Raw Data", 20},
};



static int
_decode_nr_mac_ul_physical_channel_schedule_report_subpkt(const char *b, int offset, size_t length,
                       PyObject *result) {

    int start = offset;
    int major_ver = _search_result_int(result, "Major Version");
    int minor_ver = _search_result_int(result, "Minor Version");
    int n_records = _search_result_int(result, "Num Records");
    bool success = false;

    switch (major_ver) {
        case 2:{

            switch (minor_ver){
                case 8:{
                    PyObject *result_allrecords = PyList_New(0);
                    PyObject *t = NULL;

                    for(int i = 0; i < n_records; i++){
                        PyObject *result_record = PyList_New(0);
                        offset += _decode_by_fmt(NrMacUlSchedRecord_v2_8,
                                                 ARRAY_SIZE(NrMacUlSchedRecord_v2_8, Fmt),
                                                 b, offset, length, result_record);

                        int n_carriers = _search_result_int(result_record, "Num Carrier");
                        PyObject *result_allcarriers = PyList_New(0);
                        PyObject *t2 = NULL;
                        for(int j = 0; j < n_carriers; j++){

                            PyObject *result_carrier = PyList_New(0);
                            offset += _decode_by_fmt(NrMacUlSchedCarrierRecord_v2_8,
                                                     ARRAY_SIZE(NrMacUlSchedCarrierRecord_v2_8, Fmt),
                                                     b, offset, length, result_carrier);

                            int phy_bitmask = _search_result_int(result_carrier, "PhyChan BitMask");
                           
                            int carrier_id = phy_bitmask & 0x000f; // TODO: Add RNTI type
                            bool pusch_flag = phy_bitmask & 0x0080; 
                            bool pucch_flag = phy_bitmask & 0x0100;
                            bool srs_flag = phy_bitmask & 0x0200;
                            bool tav_status = phy_bitmask & 0x2000;
                            bool ccd_status = phy_bitmask & 0x2000;
                            PyObject *py_carrier = Py_BuildValue("I", carrier_id);
                            PyList_Append(result_carrier, Py_BuildValue("(sOs)", "Carrier ID", py_carrier, ""));
                            PyObject *py_pusch = Py_BuildValue("I", pusch_flag);
                            PyList_Append(result_carrier, Py_BuildValue("(sOs)", "PUSCH flag", py_pusch, ""));
                            PyObject *py_pucch = Py_BuildValue("I", pucch_flag);
                            PyList_Append(result_carrier, Py_BuildValue("(sOs)", "PUCCH flag", py_pucch, ""));
                            PyObject *py_srs = Py_BuildValue("I", srs_flag);
                            PyList_Append(result_carrier, Py_BuildValue("(sOs)", "SRS flag", py_srs, ""));
                            PyObject *py_tav = Py_BuildValue("I", tav_status);
                            PyList_Append(result_carrier, Py_BuildValue("(sOs)", "TAV status", py_tav, ""));
                            PyObject *py_ccd = Py_BuildValue("I", ccd_status);
                            PyList_Append(result_carrier, Py_BuildValue("(sOs)", "CCD status", py_ccd, ""));



                            // PUSCH data
                            if(pusch_flag){

                                // TODO: Many more fileds are not yet parsed. The current implementation focuses on the most useful ones.

                                PyObject *result_pusch = PyList_New(0);
                                offset += _decode_by_fmt(NrMacUlSchedPUSCHRecord_v2_8,
                                                     ARRAY_SIZE(NrMacUlSchedPUSCHRecord_v2_8, Fmt),
                                                     b, offset, length, result_pusch);

                                std::string py_pusch = _search_result_bytestream(result_pusch,"PUSCH Raw Data");
                                // printf("%s\n", py_pusch.c_str());
                                // _delete_result(result_pusch,"PUSCH data"); // Remove raw data

                                size_t pusch_len = py_pusch.size();

                                PyObject *pusch_field = NULL;

                                PyObject *is_second_phychan = Py_BuildValue("I", stoi(py_pusch.substr(pusch_len-4,4),0,2));
                                pusch_field = Py_BuildValue("(sOs)", "Is Second Phychan", is_second_phychan, "");
                                PyList_Append(result_pusch, pusch_field);
                                Py_DECREF(is_second_phychan);
                                Py_DECREF(pusch_field);


                                PyObject *start_symbol = Py_BuildValue("I", stoi(py_pusch.substr(pusch_len-5,1),0,2));
                                pusch_field = Py_BuildValue("(sOs)", "Start Symbol", start_symbol, "");
                                PyList_Append(result_pusch, pusch_field);
                                Py_DECREF(start_symbol);
                                Py_DECREF(pusch_field);


                                PyObject *num_symbol = Py_BuildValue("I", stoi(py_pusch.substr(pusch_len-9,4),0,2));
                                pusch_field = Py_BuildValue("(sOs)", "Num Symbols", num_symbol, "");
                                PyList_Append(result_pusch, pusch_field);
                                Py_DECREF(num_symbol);
                                Py_DECREF(pusch_field);

                                PyObject *harq_id = Py_BuildValue("I", stoi(py_pusch.substr(pusch_len-13,4),0,2));
                                pusch_field = Py_BuildValue("(sOs)", "HARQ ID", harq_id, "");
                                PyList_Append(result_pusch, pusch_field);
                                Py_DECREF(harq_id);
                                Py_DECREF(pusch_field);


                                PyObject *mcs = Py_BuildValue("I", stoi(py_pusch.substr(pusch_len-18,5),0,2));
                                pusch_field = Py_BuildValue("(sOs)", "MCS", mcs, "");
                                PyList_Append(result_pusch, pusch_field);
                                Py_DECREF(mcs);
                                Py_DECREF(pusch_field);

                                PyObject *dmrs_add_pos = Py_BuildValue("I", stoi(py_pusch.substr(pusch_len-24,6),0,2));
                                pusch_field = Py_BuildValue("(sOs)", "DMRS Add Pos", dmrs_add_pos, "");
                                PyList_Append(result_pusch, pusch_field);
                                Py_DECREF(dmrs_add_pos);
                                Py_DECREF(pusch_field);

                                PyObject *RB_start = Py_BuildValue("I", stoi(py_pusch.substr(pusch_len-32,8),0,2));
                                pusch_field = Py_BuildValue("(sOs)", "RB Start", RB_start, "");
                                PyList_Append(result_pusch, pusch_field);
                                Py_DECREF(RB_start);
                                Py_DECREF(pusch_field);

                                PyObject *num_RB = Py_BuildValue("I", stoi(py_pusch.substr(pusch_len-41,9),0,2));
                                pusch_field = Py_BuildValue("(sOs)", "Num RBs", num_RB, "");
                                PyList_Append(result_pusch, pusch_field);
                                Py_DECREF(num_RB);
                                Py_DECREF(pusch_field);

                                PyObject *ra_type = Py_BuildValue("I", stoi(py_pusch.substr(pusch_len-42,1),0,2));
                                pusch_field = Py_BuildValue("(sOs)", "RA Type", ra_type, "");
                                PyList_Append(result_pusch, pusch_field);
                                Py_DECREF(ra_type);
                                Py_DECREF(pusch_field);

                                PyObject *tb_size = Py_BuildValue("I", stoi(py_pusch.substr(pusch_len-57,14),0,2));
                                pusch_field = Py_BuildValue("(sOs)", "TB Size(bytes)", tb_size, "");
                                PyList_Append(result_pusch, pusch_field);
                                Py_DECREF(tb_size);
                                Py_DECREF(pusch_field);


                                PyObject *code_rate = Py_BuildValue("I", stoi(py_pusch.substr(pusch_len-87,11),0,2));
                                pusch_field = Py_BuildValue("(sOs)", "Code rate", code_rate, "");
                                PyList_Append(result_pusch, pusch_field);
                                Py_DECREF(code_rate);
                                Py_DECREF(pusch_field);

 
                                PyObject *pusch_data = Py_BuildValue("(sOs)", "PUSCH Data (partial results)", result_pusch, "dict");
                                PyList_Append(result_carrier, pusch_data);
                                Py_DECREF(pusch_data);
                            }
                            

                            // PUCCH data
                            if(pucch_flag){

                                // TODO: Many more fileds are not yet parsed. The current implementation focuses on the most useful ones.

                                PyObject *result_pusch = PyList_New(0);
                                offset += _decode_by_fmt(NrMacUlSchedPUCCHRecord_v2_8,
                                                     ARRAY_SIZE(NrMacUlSchedPUCCHRecord_v2_8, Fmt),
                                                     b, offset, length, result_pusch);


                                PyObject *pucch_data = Py_BuildValue("(sOs)", "PUCCH Data (coming soon)", result_pusch, "dict");
                                PyList_Append(result_carrier, pucch_data);
                                Py_DECREF(pucch_data);


                            }

                            if(srs_flag){

                                PyObject *result_srs = PyList_New(0);
                                offset += _decode_by_fmt(NrMacUlSchedSrsRecord_v2_8,
                                                     ARRAY_SIZE(NrMacUlSchedSrsRecord_v2_8, Fmt),
                                                     b, offset, length, result_srs);


                                PyObject *srs_data = Py_BuildValue("(sOs)", "SRS Data (coming soon)", result_srs, "dict");
                                PyList_Append(result_carrier, srs_data);
                                Py_DECREF(srs_data);

                            }


                            // if(prach_flag){

                            // }

                            char name_carrier[64];
                            sprintf(name_carrier, "Carrier [%d]", j);
                            t2 = Py_BuildValue("(sOs)", name_carrier, result_carrier, "dict");

                            PyList_Append(result_allcarriers, t2);
                            Py_DECREF(py_carrier);
                            Py_DECREF(py_pusch);
                            Py_DECREF(py_pucch);
                            Py_DECREF(py_srs);
                            Py_DECREF(py_tav);
                            Py_DECREF(py_ccd);
                            Py_DECREF(result_carrier);
                            Py_DECREF(t2);


                        }

                        PyObject *py_allcarriers = Py_BuildValue("(sOs)", "Carriers", result_allcarriers, "dict");
                        PyList_Append(result_record, py_allcarriers);


                        char name[64];
                        sprintf(name, "Record [%d]", i);
                        t = Py_BuildValue("(sOs)", name, result_record, "dict");

                        PyList_Append(result_allrecords, t);

                        Py_DECREF(result_allcarriers);
                        Py_DECREF(py_allcarriers);
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

        printf("(MI)Unknown 5G ML1 Serving Cell Beam Management: %d.%d\n", major_ver, minor_ver);
    }

    return offset - start;
}

