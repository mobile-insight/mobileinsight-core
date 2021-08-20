/*
 * NR_L2_UL_TB
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt NrL2UlTbFmt [] = {
    {UINT, "Version", 4},
};

const Fmt NrL2UlTbMeta_v4 [] = {
    {UINT, "Num TTI", 1},
    {SKIP, NULL, 3},
};

const Fmt NrTTIMeta_v4 [] = {
    {UINT, "Slot Number", 1},
    {SKIP, NULL, 1},
    {UINT, "FN", 2},
    {UINT, "Num TB", 1},
    {SKIP, NULL, 3},

};

const Fmt NrTbInfo_v4 [] = {
    {BIT_STREAM_LITTLE_ENDIAN, "Raw TB Info", 20},
};

const Fmt NrMcePayload_v4 [] = {
    {BIT_STREAM_LITTLE_ENDIAN, "Raw MCE Info", 2},
};

static int
_decode_nr_l2_ul_tb_subpkt(const char *b, int offset, size_t length,
                       PyObject *result) {

    int start = offset;
    int version = _search_result_int(result, "Version");
    
    bool success = false;


    switch (version) {
        case 4:{

            // Parse meta header
            PyObject *result_meta = PyList_New(0);
            offset += _decode_by_fmt(NrL2UlTbMeta_v4,
                                    ARRAY_SIZE(NrL2UlTbMeta_v4, Fmt),
                                    b, offset, length, result_meta);
            PyObject *t = Py_BuildValue("(sOs)", "Meta", result_meta, "dict");

            int n_tti = _search_result_int(result_meta, "Num TTI");

            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_meta);

            PyObject *result_all_tti = PyList_New(0);

            for(int i = 0; i < n_tti; i++){

                PyObject *result_record = PyList_New(0);
                PyObject *tti_meta = PyList_New(0);
                offset += _decode_by_fmt(NrTTIMeta_v4,
                                    ARRAY_SIZE(NrTTIMeta_v4, Fmt),
                                    b, offset, length, tti_meta);

                int num_tb = _search_result_int(tti_meta, "Num TB");

                t = Py_BuildValue("(sOs)", "TTI Info Meta", tti_meta, "dict");
                PyList_Append(result_record, t);
                Py_DECREF(t);
                Py_DECREF(tti_meta);

                
                PyObject *tb_all_info = PyList_New(0);

                for(int j = 0; j < num_tb; j++){

                    PyObject *tb_info = PyList_New(0);
                    offset += _decode_by_fmt(NrTbInfo_v4,
                                    ARRAY_SIZE(NrTbInfo_v4, Fmt),
                                    b, offset, length, tb_info);


                    std::string str_tb_info = _search_result_bytestream(tb_info,"Raw TB Info");
                    
                    // _delete_result(tb_info,"Raw TB Info"); // Remove raw data

                    size_t tb_info_len = str_tb_info.size();
                    PyObject *tb_info_field = NULL;

                    // printf("%s\n", str_tb_info.c_str());

                    PyObject *numerology = Py_BuildValue("I", stoi(str_tb_info.substr(tb_info_len-3,3),0,2));
                    tb_info_field = Py_BuildValue("(sOs)", "Numerology", numerology, "");
                    PyList_Append(tb_info, tb_info_field);
                    Py_DECREF(numerology);
                    Py_DECREF(tb_info_field);

                    PyObject *harq_id = Py_BuildValue("I", stoi(str_tb_info.substr(tb_info_len-8,5),0,2));
                    tb_info_field = Py_BuildValue("(sOs)", "HARQ ID", harq_id, "");
                    PyList_Append(tb_info, tb_info_field);
                    Py_DECREF(harq_id);
                    Py_DECREF(tb_info_field);

                    PyObject *start_pdu_seg = Py_BuildValue("I", stoi(str_tb_info.substr(tb_info_len-18,1),0,2));
                    tb_info_field = Py_BuildValue("(sOs)", "Start PDU Seg", start_pdu_seg, "");
                    PyList_Append(tb_info, tb_info_field);
                    Py_DECREF(start_pdu_seg);
                    Py_DECREF(tb_info_field);


                    PyObject *num_complete_pdu = Py_BuildValue("I", stoi(str_tb_info.substr(tb_info_len-27,10),0,2));
                    tb_info_field = Py_BuildValue("(sOs)", "Num Complete PDU", num_complete_pdu, "");
                    PyList_Append(tb_info, tb_info_field);
                    Py_DECREF(num_complete_pdu);
                    Py_DECREF(tb_info_field);


                    PyObject *end_pdu_seg = Py_BuildValue("I", stoi(str_tb_info.substr(tb_info_len-28,1),0,2));
                    tb_info_field = Py_BuildValue("(sOs)", "End PDU Seg", end_pdu_seg, "");
                    PyList_Append(tb_info, tb_info_field);
                    Py_DECREF(end_pdu_seg);
                    Py_DECREF(tb_info_field);


                    PyObject *grant_size = Py_BuildValue("I", stol(str_tb_info.substr(tb_info_len-64,32),0,2));
                    tb_info_field = Py_BuildValue("(sOs)", "Grant Size", grant_size, "");
                    PyList_Append(tb_info, tb_info_field);
                    Py_DECREF(grant_size);
                    Py_DECREF(tb_info_field);

                    PyObject *bytes_built = Py_BuildValue("I", stol(str_tb_info.substr(tb_info_len-96,32),0,2));
                    tb_info_field = Py_BuildValue("(sOs)", "Bytes Built", bytes_built, "");
                    PyList_Append(tb_info, tb_info_field);
                    Py_DECREF(bytes_built);
                    Py_DECREF(tb_info_field);

                    // MCE Req Bitmask: https://www.sharetechnote.com/html/5G/5G_MAC_CE.html
                    int mce_req_bitmask_i = stol(str_tb_info.substr(tb_info_len-104,8),0,2);
                    std::string mce_req_bitmask_s="";
                    if (mce_req_bitmask_i & 0x1){
                        mce_req_bitmask_s += "PHR;";
                    }
                    if (mce_req_bitmask_i & 0x2){
                        mce_req_bitmask_s += "BSR;";
                    }
                    PyObject *mce_req_bitmask = Py_BuildValue("s", mce_req_bitmask_s.c_str());
                    tb_info_field = Py_BuildValue("(sOs)", "MAC CE Req Bitmask", mce_req_bitmask, "");
                    PyList_Append(tb_info, tb_info_field);
                    Py_DECREF(mce_req_bitmask);
                    Py_DECREF(tb_info_field);



                    int mce_built_bitmask_i = stol(str_tb_info.substr(tb_info_len-112,8),0,2);
                    std::string mce_built_bitmask_s="";
                    if (mce_built_bitmask_i & 0x1){
                        mce_built_bitmask_s += "PHR;";
                    }
                    if (mce_built_bitmask_i & 0x2){
                        mce_built_bitmask_s += "BSR;";
                    }
                    PyObject *mce_built_bitmask = Py_BuildValue("s", mce_built_bitmask_s.c_str());
                    tb_info_field = Py_BuildValue("(sOs)", "MAC CE Built Bitmask", mce_built_bitmask, "");
                    PyList_Append(tb_info, tb_info_field);
                    Py_DECREF(mce_built_bitmask);
                    Py_DECREF(tb_info_field);


                    int bsr_reason_i = stol(str_tb_info.substr(32,8),0,2);
                    std::string bsr_reason_s="";
                    if (bsr_reason_i & 0x1){
                        bsr_reason_s += "High_data_arrival;";
                    }
                    if (bsr_reason_i & 0x2){
                        bsr_reason_s += "Periodic;";
                    }
                    PyObject *bsr_reason = Py_BuildValue("s", bsr_reason_s.c_str());
                    tb_info_field = Py_BuildValue("(sOs)", "BSR Reason", bsr_reason, "");
                    PyList_Append(tb_info, tb_info_field);
                    Py_DECREF(bsr_reason);
                    Py_DECREF(tb_info_field);


                    // NOTE: MCE payload is not always available. It would affect the message length!
                    int mce_length_i = stol(str_tb_info.substr(28,4),0,2);
                    PyObject *mce_length = Py_BuildValue("I", mce_length_i);
                    tb_info_field = Py_BuildValue("(sOs)", "MAC CE Length", mce_length, "");
                    PyList_Append(tb_info, tb_info_field);
                    Py_DECREF(mce_length);
                    Py_DECREF(tb_info_field);

                    if(mce_length_i){
                        // Non-empty MCE

                        offset += mce_length_i; // TODO: variable length: To be parsed later

                        // PyObject *mce_payload = PyList_New(0);
                        // offset += _decode_by_fmt(NrMcePayload_v4,
                        //                 ARRAY_SIZE(NrMcePayload_v4, Fmt),
                        //                 b, offset, length, mce_payload);

                        // tb_info_field = Py_BuildValue("(sOs)", "McePayload", mce_payload, "");
                        // PyList_Append(tb_info, tb_info_field);
                        // Py_DECREF(mce_payload);
                        // Py_DECREF(tb_info_field);
                    }

                    char name_tb[64];
                    sprintf(name_tb, "TB Info [%d]", j);
                    t = Py_BuildValue("(sOs)", name_tb, tb_info, "dict");
                    PyList_Append(tb_all_info, t);
                    Py_DECREF(t);
                    Py_DECREF(tb_info);
                }

                t = Py_BuildValue("(sOs)", "TB Info", tb_all_info, "dict");
                PyList_Append(result_record, t);
                Py_DECREF(t);
                Py_DECREF(tb_all_info);


                char name_tti[64];
                sprintf(name_tti, "TTI Info [%d]", i);

                t = Py_BuildValue("(sOs)", name_tti, result_record, "dict");
                PyList_Append(result_all_tti, t);
                Py_DECREF(t);
                Py_DECREF(result_record);


            }

            t = Py_BuildValue("(sOs)", "TTI Info", result_all_tti, "dict");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_all_tti);



            success = true;
            break;
        }
        default:
            break;

    }

    if(!success){

        printf("(MI)Unknown 5G NR L2 UL TB: %d\n", version);
    }

    return offset - start;

}
