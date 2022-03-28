/*
 * NR5G L2 UL BSR
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt NRL2ULBSR_Fmt [] = {
    {UINT, "Version", 4},
};


//Version2
const Fmt Meta_v2 [] = {
    {UINT, "Num TTI", 4},//4bit
    {PLACEHOLDER, "LCID Prio Bitmask", 0},//16bit
    {PLACEHOLDER, "Reserved1", 0}, //12bit
};
const Fmt TTIInfo_v2 [] = {
    {UINT, "Hard ID", 1},		//4bit  //insert systime
    {PLACEHOLDER, "Carrier", 0},	//4bit
    {UINT, "BSR Trigger Reason", 4}, 	//2bit
    {PLACEHOLDER, "BSR Type", 0},		//4bit		
    {PLACEHOLDER, "BSR Length", 0},		//4bit
    {PLACEHOLDER, "LCG Bitmask", 0},	//8bit
    {PLACEHOLDER, "Num LCIDs", 0},		//4bit
    {PLACEHOLDER, "Reserved3", 0},    	//2bit
    {PLACEHOLDER, "Reserved4", 0},
    {UINT, "Reserved5", 3},   //appendQOS LCID
 		
};
const Fmt SysTime_v2 [] = {
    {UINT, "Slot Number", 1},
    {UINT, "Reserved6", 1},
    //{SKIP, NULL, 1},    		
    {UINT, "FN", 2}, 				//10bit
    {PLACEHOLDER, "Reserved2", 0},  	//6bit
};
const Fmt QOSLCID_v2 [] = {
    {UINT, "QOS LCID", 1},
};
const Fmt Skip1_v2 [] = {
    {UINT, "Reserved11", 4},
};
const Fmt Skip2_v2 [] = {
    {UINT, "Reserved12", 8},
};
const ValueName BSRTriggerReason[] = {
        {1, "T_PERIODIC_EXPIRY_BSR"},
        {4, "HIGH_DATA_ARRIVAL"},
        {5, "T_PERIODIC_EXPIRY_BSR:HIGH_DATA_ARRIVAL"}
};
const ValueName BSRType[] = {
        {1, "SHORT_BSR"},
        {2, "LONG_BSR"}
};
static int _decode_nr_l2_ul_bsr_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");   
    PyObject *old_object;

    switch (pkt_ver) {
    case 2:
        {

		//Meta
		PyObject *result_Meta = PyList_New(0);			    
		offset += _decode_by_fmt(Meta_v2,
		    ARRAY_SIZE(Meta_v2, Fmt),
		    b, offset, length, result_Meta);
			//划分bit
        unsigned int temp4 = _search_result_uint(result_Meta, "Num TTI");
        int iNumTTI = temp4 & 15; // 4 bits
        int iLCIDPrioBitmask = (temp4 >> 4) & 65535;    // 16 bits	
        int iReserved1 = (temp4 >> 20) & 4095;    // 12 bits
        old_object = _replace_result_int(result_Meta,
                "Num TTI", iNumTTI);
        Py_DECREF(old_object);  
        old_object = _replace_result_int(result_Meta,
                "LCID Prio Bitmask", iLCIDPrioBitmask);
        Py_DECREF(old_object);  
        old_object = _replace_result_int(result_Meta,
                "Reserved1", iReserved1);
        Py_DECREF(old_object);                        	        	
		//Meta加入result
		PyObject *t1 = Py_BuildValue("(sOs)", "Meta",
			result_Meta, "dict");
		PyList_Append(result, t1);
		Py_DECREF(t1);
		Py_DECREF(result_Meta);		
				    
		//TTI Info
		int num_TTI = iNumTTI;
        //unsigned int num_TTI = _search_result_uint(result, "Num TTI");		   
    	//int num_TTI = _search_result_int(result, "Num TTI");		    
        PyObject *result_TTI = PyList_New(0);
        for (int i = 0; i < num_TTI; i++) {
			//SysTime
			PyObject *result_SysTime = PyList_New(0);			    
			offset += _decode_by_fmt(SysTime_v2,
				ARRAY_SIZE(SysTime_v2, Fmt),
				b, offset, length, result_SysTime);
			//SysTime内部划分bit
		    unsigned int temp5 = _search_result_uint(result_SysTime, "FN");
		    int iFN = temp5 & 1023; // 10 bits
		    int iReserved2 = (temp5 >> 10) & 63;    // 6 bits	
		    old_object = _replace_result_int(result_SysTime,"FN", iFN);
		    Py_DECREF(old_object);  
		    old_object = _replace_result_int(result_SysTime,"Reserved2", iReserved2);
		    Py_DECREF(old_object); 
		    //TTI Item                    				        
            PyObject *result_TTI_item = PyList_New(0);
            offset += _decode_by_fmt(TTIInfo_v2,
                    ARRAY_SIZE(TTIInfo_v2, Fmt),
                    b, offset, length, result_TTI_item);		    
			//SysTime插入TTIItem
			PyObject *t2 = Py_BuildValue("(sOs)", "SysTime",
				result_SysTime, "dict");
			PyList_Insert(result_TTI_item, 0, t2);
			Py_DECREF(t2);
			Py_DECREF(result_SysTime);				    
		    
		    
		    //TTI Info内部划分bit
		    unsigned int temp6 = _search_result_uint(result_TTI_item, "Hard ID");
		    int iHardID = temp6 & 15; // 4 bits
		    int iCarrier = (temp6 >> 4) & 15;    // 4 bits	
		    old_object = _replace_result_int(result_TTI_item,"Hard ID", iHardID);
		    Py_DECREF(old_object);  
		    old_object = _replace_result_int(result_TTI_item,"Carrier", iCarrier);
		    Py_DECREF(old_object); 		    
		    unsigned int temp7 = _search_result_uint(result_TTI_item, "BSR Trigger Reason");
		    int iBSRTriggerReason = temp7 & 7; 	// 3 bits
		    int iBSRType = (temp7 >> 3) & 3;    // 3 bits	
		    int iBSRLength = (temp7 >> 6) & 15; 	// 4 bits
		    int iLCGBitmask = (temp7 >> 10) & 255;// 8 bits	
		    int iNumLCIDs = (temp7 >> 18) & 15; 	// 4 bits
		    int iReserved3 = (temp7 >> 22) & 3;   // 2 bits		
		    int iReserved4 = (temp7 >> 24) & 255;   // 8 bits	    		    
		    old_object = _replace_result_int(result_TTI_item,"BSR Trigger Reason", iBSRTriggerReason);
		    Py_DECREF(old_object);  
	        (void) _map_result_field_to_name(result_TTI_item,
	                "BSR Trigger Reason",
	                BSRTriggerReason,
	                ARRAY_SIZE(BSRTriggerReason, ValueName),
	                "(MI)Unknown");		    
		    //BSR Trigger Reason和BSR Type要转换字符串
		    old_object = _replace_result_int(result_TTI_item,"BSR Type", iBSRType);
		    Py_DECREF(old_object); 
	        (void) _map_result_field_to_name(result_TTI_item,
	                "BSR Type",
	                BSRType,
	                ARRAY_SIZE(BSRType, ValueName),
	                "(MI)Unknown");				    	
		    old_object = _replace_result_int(result_TTI_item,"BSR Length", iBSRLength);
		    Py_DECREF(old_object); 	
		    old_object = _replace_result_int(result_TTI_item,"LCG Bitmask", iLCGBitmask);
		    Py_DECREF(old_object); 	
		    old_object = _replace_result_int(result_TTI_item,"Num LCIDs", iNumLCIDs);
		    Py_DECREF(old_object); 	
		    old_object = _replace_result_int(result_TTI_item,"Reserved3", iReserved3);
		    Py_DECREF(old_object); 	
		    old_object = _replace_result_int(result_TTI_item,"Reserved4", iReserved4);
		    Py_DECREF(old_object); 			    		    		    		    		    		    
		    		    
		    //QOS LCID		    
			int num_LCIDs = _search_result_int(result_TTI_item, "Num LCIDs");		    
		    PyObject *result_LCIDs = PyList_New(0);
		    for (int i = 0; i < num_LCIDs; i++) {
		        PyObject *result_LCIDs_item = PyList_New(0);
		        offset += _decode_by_fmt(QOSLCID_v2,
		                ARRAY_SIZE(QOSLCID_v2, Fmt),
		                b, offset, length, result_LCIDs_item);
		        char name[64];
                int tmp=_search_result_int(result_LCIDs_item,"QOS LCID");
		        sprintf(name, "QOS LCID[%d]", i);
				PyObject *t3 = Py_BuildValue("(sIs)", name,
					tmp,"");		                
				PyList_Append(result_LCIDs, t3);
				Py_DECREF(t3);
				Py_DECREF(result_LCIDs_item);			               		    
				}	
			//QOS LCID insert TTIItem
			PyObject *t4 = Py_BuildValue("(sOs)", "QOS LCID",
				result_LCIDs, "dict");
			PyList_Append(result_TTI_item, t4);
			Py_DECREF(t4);
			Py_DECREF(result_LCIDs);	
			
			//Skip
			PyObject *result_Skip = PyList_New(0);
		    int count = 0;
		    for (int i = 0; i < 8; i++) {
		        if ((iLCGBitmask&(1<<i))==(1<<i)) {
		            count++;
		        }
		    }
		    switch(count){
		    	case 1:	
					offset += _decode_by_fmt(Skip1_v2,
						ARRAY_SIZE(Skip1_v2, Fmt),
						b, offset, length, result_Skip);
					break;
				case 2:	
					offset += _decode_by_fmt(Skip2_v2,
						ARRAY_SIZE(Skip2_v2, Fmt),
						b, offset, length, result_Skip);
					break;
				default:
					offset += _decode_by_fmt(Skip1_v2,
						ARRAY_SIZE(Skip1_v2, Fmt),
						b, offset, length, result_Skip);
				}				
									    					
		
			PyObject *t7 = Py_BuildValue("(sOs)", "Skip",
				result_Skip, "dict");
			PyList_Append(result_TTI_item, t7);
			Py_DECREF(t7);
			Py_DECREF(result_Skip);

			//TTI Item add TTI
            char name[64];
            sprintf(name, "TTI Info[%d]", i);
			PyObject *t5 = Py_BuildValue("(sOs)", name,
				result_TTI_item, "dict");
			PyList_Append(result_TTI, t5);
			Py_DECREF(t5);
			Py_DECREF(result_TTI_item);						    
		    }		    
		//TTI add result
		PyObject *t6 = Py_BuildValue("(sOs)", "TTI Infos",
			result_TTI, "dict");
		PyList_Append(result, t6);
		Py_DECREF(t6);
		Py_DECREF(result_TTI);	
		return offset - start;	
		}	    
    default:
        printf("(MI)Unknown NR L2 UL BSR Version: 0x%x\n", pkt_ver);
        return 0;
    }
}	    
		    
		    
		    
		    
		    
		    
		    
		    
		    
