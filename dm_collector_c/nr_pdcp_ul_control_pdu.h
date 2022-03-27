/*
 * NR5G PDCP UL Control Pdu
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt NrPdcpUlControlPdu_Fmt [] = {
    {UINT, "Version", 4},
};

//Version1
const Fmt Meta_v1 [] = {
    {UINT, "Num Control PDU", 1},
    {SKIP, NULL, 3},
};
const Fmt ControlPDU_v1 [] = {
    {UINT, "RB ID", 4},//6b
    {PLACEHOLDER, "PDCP SN Length", 0},//3b string
    {PLACEHOLDER, "RB Mode", 0},//2b  string
    {PLACEHOLDER, "RB Type", 0},//2b  string
    {PLACEHOLDER, "Size", 0},//10b
    {PLACEHOLDER, "Pad Size", 0},//2b
};
const Fmt PucpSysTime_v1 [] = {
    {UINT, "Slot Number", 1},
    {SKIP, NULL, 1},    
    {UINT, "FN", 2},     
};
const Fmt others_v1 [] = {
    {UINT, "R", 1},//4b
    {PLACEHOLDER, "PDCP Type", 0},//2b
    {PLACEHOLDER, "DC Bit", 0},//2b    
    {UINT, "FMC", 4},   
    {UINT, "Pad Payload", 3},     
};


const ValueName RBMode[] = {
        {1, "AM"},

};
const ValueName RBType[] = {
        {1, "SRB"},
        {2, "DRB"},
};
const ValueName PDCPSNLength[] = {
        {2, "18"},
};
const ValueName PDCPType1[] = {
        {0, "STATUS_REPORT"},

};

uint32_t reversebytes_uint32t(uint32_t value){  
    return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |   
        (value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24;   
}  

static int _decode_nr_pdcp_ul_control_pdu_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    PyObject *old_object;


    switch (pkt_ver) {
    case 1:
        {
        	//Meta
        	PyObject *result_Meta = PyList_New(0);
            offset += _decode_by_fmt(Meta_v1,
                    ARRAY_SIZE(Meta_v1, Fmt),
                    b, offset, length, result_Meta);
	    	int num_ControlPDU = _search_result_int(result_Meta, "Num Control PDU");                    
 			//Meta > result
            PyObject *t4 = Py_BuildValue("(sOs)", "Meta",
                    result_Meta, "dict");
            PyList_Append(result, t4);
            Py_DECREF(t4);
            Py_DECREF(result_Meta);                    
                    
        	//ControlPDU                    
            PyObject *result_ControlPDU = PyList_New(0);
            for (int i = 0; i < num_ControlPDU; i++) {
            	//ControlPDUItem
                PyObject *result_ControlPDU_item = PyList_New(0);
                offset += _decode_by_fmt(ControlPDU_v1,
                        ARRAY_SIZE(ControlPDU_v1, Fmt),
                        b, offset, length, result_ControlPDU_item);
                      
                unsigned int temp1 = _search_result_uint(result_ControlPDU_item,
                        "RB ID");
                int iID = temp1 & 63;  // 6bits			   
                int iLength = (temp1 >> 6) & 7;   // 3 bits
                int iMode = (temp1 >> 9) & 3; // 2 bit	   
                int iType = (temp1 >> 11) & 3; // 2 bits
                int iSize = (temp1 >> 13) & 1023; // 10 bits
                int iPad = (temp1 >> 23) & 3;    // 2 bits                                  
                old_object = _replace_result_int(result_ControlPDU_item,
                        "RB ID", iID);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_ControlPDU_item,
                        "PDCP SN Length", iLength);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_ControlPDU_item,
                        "RB Mode", iMode);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_ControlPDU_item,
                        "RB Type", iType);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_ControlPDU_item,
                        "Size", iSize);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_ControlPDU_item,
                        "Pad Size", iPad);
                Py_DECREF(old_object);                                
                (void) _map_result_field_to_name(result_ControlPDU_item,
                        "PDCP SN Length",
                        PDCPSNLength,
                        ARRAY_SIZE(PDCPSNLength, ValueName),
                        "(MI)Unknown");
                (void) _map_result_field_to_name(result_ControlPDU_item,
                        "RB Mode",
                        RBMode,
                        ARRAY_SIZE(RBMode, ValueName),
                        "(MI)Unknown");
                (void) _map_result_field_to_name(result_ControlPDU_item,
                        "RB Type",
                        RBType,
                        ARRAY_SIZE(RBType, ValueName),
                        "(MI)Unknown");                                                
				//SysTime > ControlPDU item
                PyObject *result_SysTime = PyList_New(0);			
				offset += _decode_by_fmt(PucpSysTime_v1,
				            ARRAY_SIZE(PucpSysTime_v1, Fmt),
				            b, offset, length, result_SysTime);
                int tmp=_search_result_int(result_SysTime,"FN");
                int value=tmp&0x3ff;
                old_object = _replace_result_int(result_SysTime,
                         "FN", value);
                Py_DECREF(old_object);

				PyObject *t1 = Py_BuildValue("(sOs)", "Sys Time",
				                result_SysTime, "dict");
				PyList_Append(result_ControlPDU_item, t1);
				Py_DECREF(t1);
                Py_DECREF(result_SysTime);
				
				//Others > ControlPDU item		
				offset += _decode_by_fmt(others_v1,
				            ARRAY_SIZE(others_v1, Fmt),
				            b, offset, length, result_ControlPDU_item);
                unsigned int temp2 = _search_result_uint(result_ControlPDU_item,
                        "R");
                int iR = temp2 & 15;  // 4bits			   
                int iPDCPType1 = (temp2 >> 4) & 3;   // 2 bits
                int iDCBit = (temp2 >> 6) & 3; // 2 bit	
                
                unsigned int FMC1 = _search_result_uint(result_ControlPDU_item,
                        "FMC");                 
                int FMC2 = reversebytes_uint32t(FMC1);
                old_object = _replace_result_int(result_ControlPDU_item,
                        "FMC", FMC2);
                Py_DECREF(old_object);                                                
                old_object = _replace_result_int(result_ControlPDU_item,
                        "R", iR);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_ControlPDU_item,
                        "PDCP Type", iPDCPType1);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_ControlPDU_item,
                        "DC Bit", iDCBit);
                Py_DECREF(old_object);               
                (void) _map_result_field_to_name(result_ControlPDU_item,
                        "PDCP Type",
                        PDCPType1,
                        ARRAY_SIZE(PDCPType1, ValueName),
                        "(MI)Unknown");				

				//ControlPDU item > ControlPDU
		        char name[64];
		        sprintf(name, "Control PDU[%d]", i);
				PyObject *t3 = Py_BuildValue("(sOs)", name,
					result_ControlPDU_item, "dict");
				PyList_Append(result_ControlPDU, t3);
				Py_DECREF(t3);
				Py_DECREF(result_ControlPDU_item);	
            }

                  
 			//ControlPDU > result
            PyObject *t5 = Py_BuildValue("(sOs)", "Control PDU",
                    result_ControlPDU, "list");
            PyList_Append(result, t5);
            Py_DECREF(t5);
            Py_DECREF(result_ControlPDU);
            return offset - start; 
        }
    default:
        printf("(MI)Unknown NR5G PDCP UL Control PDU version: %d\n", pkt_ver);
        return 0;
    }
}






















