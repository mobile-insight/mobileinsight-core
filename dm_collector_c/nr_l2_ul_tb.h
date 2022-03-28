#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"
#include<vector>
#include<string>
const Fmt NrL2UlTb_Fmt[] = {
	{UINT,"Version",4},
};
const Fmt NrMeta_Fmt_v4[]={
	{UINT,"Num TTI",1},
	{PLACEHOLDER,"Is Type2 Scell",0},
	{PLACEHOLDER,"Is Type2 Other Cell",0},
	{SKIP,NULL,3},
};

const Fmt TtiInfoMeta_Fmt_v4[] = {
	{UINT,"Sys Time Slot Number",1},
	{SKIP,NULL,1},
	{UINT,"Sys Time FN",2},
	{UINT,"Num TB",1},
	{SKIP,NULL,3},
};

const Fmt TbInfo_Fmt_v4[] = {
	{UINT,"Numerology",1},
	{PLACEHOLDER,"Harq ID",0},
	{UINT,"Carrier ID",1},
	{PLACEHOLDER,"TB Type",0},
	{PLACEHOLDER,"RNTI Type",0},
	{UINT,"Start PDU Seg",1},
	{UINT,"Num Complete PDU",1},
	{PLACEHOLDER,"End PDU Seg",0},
	{UINT,"Grant Size",4},
	{UINT,"Bytes Build",4},
	{UINT,"MCE Req Bitmask",1},
	{UINT,"MCE Build Bitmask",1},
	{UINT,"PHR Reason",1},
	{UINT,"BSR Reason",1},
	{PLACEHOLDER,"BSR Type",0},
	{PLACEHOLDER,"PHR Cancel Reason",0},
	{UINT,"MCE Length",1},
	{SKIP,NULL,3},
};
const Fmt McePayload_Fmt_v4[] = {
	{UINT,"MCE Type",1},
};
const Fmt ShortBsr_Fmt_v4[] = {
	{UINT,"ShortBsr-BSR Index",1},
	{PLACEHOLDER,"ShortBsr-BSR LCG",0},
};

const Fmt SinglePhr_Fmt_v4[] = {
	{UINT,"SinglePhr-PH",1},
	{UINT,"SinglePhr-Pcmax fc",1},
	{PLACEHOLDER,"SinglePhr-R",0},
};

const ValueName  tb_type_v4[] = {
	{0b0000,"CONNECTED"},
	{0b0100,"RNTI"},
	{0b1000,"IRACH"},
};

const ValueName  rnti_type_v4[] = {
	{0b000,"C_RNTI"},
};
const ValueName logic[] = {
	{0,"false"},
	{1,"true"},
};
const ValueName MCE_bitmask_v4[] = {
	{0,"0"},
	{1,"PHR"},
	{2,"BSR"},
	{3,"PHR|BSR"},
};

const ValueName phr_reason_v4[] = {
	{0x00," "},
	{0x01,"CFG_CHANGE"},
	{0x02,"PERIODIC_EXPIRY_PHR"},
    {0x03,"PERIODIC_EXPIRY_PHR|CFG_CHANGE"},
	{0x04,"DL_PATHLOSS_CHANGE"},
	{0x05,"CFG_CHANGE|DL_PATHLOSS_CHANGE"},
	{0x06,"PERIODIC_EXPIRY_PHR|DL_PATHLOSS_CHANGE"},
};
const ValueName phr_cancel_reason_v4[] = {
	{0x00,"MIN"},
};
const ValueName bsr_reason_v4[] = {
	{0b100,"HIGH_DATA_ARRIVAL"},
	{0b101,"PERIODIC|HIGH_DATA_ARRIVAL"},
	{0b001,"PERIODIC"},
};

const ValueName BSR_Type_v4[] = {
	{0b0101,"CANCELLED_BSR"},
	{0b0001,"SHORT_BSR"},
	{0b0010,"LONG_BSR"},
	{0,"NO_BSR"},
};
const ValueName MCE_Type_v4[] = {
	{0x3D,"S-BSR"},
	{0x39,"S-PHR"},
    {0x00,"None"},
};
static int
_decode_McePayload_v4(const char* b, int offset, size_t length,
	PyObject* result, std::string  lc_name) {
	int start = offset;
	PyObject* records = PyList_New(0);
	PyObject* t = NULL;
	offset += _decode_by_fmt(McePayload_Fmt_v4,
		ARRAY_SIZE(McePayload_Fmt_v4, Fmt),
		b, offset, length, records);
	int tmp = _search_result_int(records, "MCE Type");
	int type = tmp & 0x3f;
	PyObject* old_object = _replace_result_int(records, "MCE Type", type);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"MCE Type", MCE_Type_v4,
		ARRAY_SIZE(MCE_Type_v4, ValueName),
		"(MI)Unknown");
	if (type == 0x3d)//S-BSR
	{
		offset += _decode_by_fmt(ShortBsr_Fmt_v4,
			ARRAY_SIZE(ShortBsr_Fmt_v4, Fmt),
			b, offset, length, records);
		tmp = _search_result_int(records, "ShortBsr-BSR Index");
		int index = tmp & 0x1f;
		old_object = _replace_result_int(records, "ShortBsr-BSR Index", index);
		Py_DECREF(old_object);  

		int lcg = (tmp >> 5);
		old_object = _replace_result_int(records, "ShortBsr-BSR LCG", lcg);
		Py_DECREF(old_object);
	}
	else if (type == 0x39) {
		offset += _decode_by_fmt(SinglePhr_Fmt_v4,
			ARRAY_SIZE(SinglePhr_Fmt_v4, Fmt),
			b, offset, length, records);
		tmp = _search_result_int(records, "SinglePhr-PH");
		int ph = tmp & 0x3f;
		old_object = _replace_result_int(records, "SinglePhr-PH", ph);
		Py_DECREF(old_object);

		tmp= _search_result_int(records, "SinglePhr-Pcmax fc");
		int fc = tmp & 0x3f;
		old_object = _replace_result_int(records, "SinglePhr-Pcmax fc", fc);
		Py_DECREF(old_object);

		int r = tmp>>6;
		old_object = _replace_result_int(records, "SinglePhr-R", r);
		Py_DECREF(old_object);

	}
	const char* s_name = lc_name.c_str();
	t = Py_BuildValue("(sOs)", s_name, records, "dict");
	PyList_Append(result, t);
	Py_DECREF(records);
	Py_DECREF(t);
	return offset - start;

}

static int 
_decode_TB_Info_v4(const char* b, int offset, size_t length,
	PyObject* result, std::string  lc_name) {
	int start = offset;
	PyObject* records = PyList_New(0);
	PyObject* t = NULL;
	offset += _decode_by_fmt(TbInfo_Fmt_v4,
		ARRAY_SIZE(TbInfo_Fmt_v4, Fmt),
		b, offset, length, records);
	
    int tmp = _search_result_int(records, "Numerology");
	int num = tmp & 0b111;
	PyObject* old_object = _replace_result_int(records, "Numerology", num);
	Py_DECREF(old_object);

	int harq = (tmp >> 3) & 0xf;
	old_object = _replace_result_int(records, "Harq ID", harq);
	Py_DECREF(old_object);

	int tmp2 = _search_result_int(records, "Carrier ID");
	int carrier_id = ((tmp2 & 0b1) << 1) + ((tmp >> 7)&0b1);
	old_object = _replace_result_int(records, "Carrier ID", carrier_id);
	Py_DECREF(old_object);

	int tb_type = (tmp2 >> 1) & 0b1111;
	old_object = _replace_result_int(records, "TB Type", tb_type);
	Py_DECREF(old_object);

	(void)_map_result_field_to_name(records,
		"TB Type", tb_type_v4,
		ARRAY_SIZE(tb_type_v4, ValueName),
		"(MI)Unknown");
	int rnti = tmp2 >> 5;
	old_object = _replace_result_int(records, "RNTI Type", rnti);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"RNTI Type", rnti_type_v4,
		ARRAY_SIZE(rnti_type_v4, ValueName),
		"(MI)Unknown");

	tmp = _search_result_int(records, "Start PDU Seg");
	int s_pdu = tmp & 0b1;
	old_object = _replace_result_int(records, "Start PDU Seg", s_pdu);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"Start PDU Seg", logic,
		ARRAY_SIZE(logic, ValueName),
		"(MI)Unknown");
	tmp2 = _search_result_int(records, "Num Complete PDU");
	int num_pdu = ((tmp2 & 0b111) << 7) + (tmp >> 1);
	old_object = _replace_result_int(records, "Num Complete PDU", num_pdu);
	Py_DECREF(old_object);
	
	int e_pdu = (tmp2 >> 3) & 0b1;
	old_object = _replace_result_int(records, "End PDU Seg", e_pdu);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"End PDU Seg", logic,
		ARRAY_SIZE(logic, ValueName),
		"(MI)Unknown");
    int MCE_Bitmask = _search_result_int(records, "MCE Build Bitmask");

	(void)_map_result_field_to_name(records,
		"MCE Req Bitmask", MCE_bitmask_v4,
		ARRAY_SIZE(MCE_bitmask_v4, ValueName),
		"(MI)Unknown");
	(void)_map_result_field_to_name(records,
		"MCE Build Bitmask", MCE_bitmask_v4,
		ARRAY_SIZE(MCE_bitmask_v4, ValueName),
		"(MI)Unknown");
	tmp = _search_result_int(records, "PHR Reason");
    tmp2 = _search_result_int(records, "BSR Reason");
    if (MCE_Bitmask & 0b1)//PHR
	{
		int phr_reason = tmp & 0x3f;
		old_object = _replace_result_int(records, "PHR Reason", phr_reason);
		Py_DECREF(old_object);
		(void)_map_result_field_to_name(records,
			"PHR Reason", phr_reason_v4,
			ARRAY_SIZE(phr_reason_v4, ValueName),
			"(MI)Unknown");
		int phr_c = (tmp >> 5) & 0b11;
		old_object = _replace_result_int(records, "PHR Cancel Reason", phr_c);
		Py_DECREF(old_object);
		(void)_map_result_field_to_name(records,
			"PHR Cancel Reason", phr_cancel_reason_v4,
			ARRAY_SIZE(phr_cancel_reason_v4, ValueName),
			"(MI)Unknown");
	}
	else {
		std::string s = "";
		old_object = _replace_result_string(records, "PHR Reason", s);
		Py_DECREF(old_object);
		old_object = _replace_result_string(records, "PHR Cancel Reason", s);
		Py_DECREF(old_object);
	}
	if (MCE_Bitmask & 0b10) {
		int bsr_reason = ((tmp2 & 0b1) << 2) + ((tmp >> 6) & 0b11);
		old_object = _replace_result_int(records, "BSR Reason", bsr_reason);
		Py_DECREF(old_object);
		int bsr_type = (tmp2 >> 1) & 0b1111;
		old_object = _replace_result_int(records, "BSR Type", bsr_type);
		Py_DECREF(old_object);
		(void)_map_result_field_to_name(records,
			"BSR Reason", bsr_reason_v4,
			ARRAY_SIZE(bsr_reason_v4, ValueName),
			"(MI)Unknown");

		(void)_map_result_field_to_name(records,
			"BSR Type", BSR_Type_v4,
			ARRAY_SIZE(BSR_Type_v4, ValueName),
			"(MI)Unknown");
	}
	else {
		std::string s = "";
		old_object = _replace_result_string(records, "BSR Reason", s);
		Py_DECREF(old_object);
		old_object = _replace_result_string(records, "BSR Type", s);
		Py_DECREF(old_object);
	}
    
	tmp = _search_result_int(records, "MCE Length");
	int mce_l = tmp & 0x3f;
	old_object = _replace_result_int(records, "MCE Length", mce_l);
	Py_DECREF(old_object);

	int index_mce = 0;
	PyObject* records_list = PyList_New(0);
    //printf("%d",mce_l);

    while (mce_l>0) {
		std::string name = "McePayload[" + std::to_string(index_mce) + "]";
		int mce_length = _decode_McePayload_v4(b, offset, length, records_list, name);
    //    printf("%d:%d\n",mce_l,mce_length);
        mce_l =mce_l-mce_length;
		offset += mce_length;
		index_mce++;
	}
    
	t = Py_BuildValue("(sOs)", "McePayload", records_list, "list");
	PyList_Append(records, t);
	Py_DECREF(records_list);
	Py_DECREF(t);
    
    const char* s_name = lc_name.c_str();
	t = Py_BuildValue("(sOs)", s_name, records, "dict");
	PyList_Append(result, t);
	Py_DECREF(records);
	Py_DECREF(t);
	return offset - start;
}
static int
_decode_TTI_Info_v4(const char* b, int offset, size_t length,
	PyObject* result, std::string  lc_name) {
	int start = offset;
	PyObject* records = PyList_New(0);
	PyObject* t = NULL;
	offset += _decode_by_fmt(TtiInfoMeta_Fmt_v4,
		ARRAY_SIZE(TtiInfoMeta_Fmt_v4, Fmt),
		b, offset, length, records);
	int tmp = _search_result_int(records, "Sys Time FN");
	int fn = tmp & 0x3ff;
	PyObject* old_object = _replace_result_int(records, "Sys Time FN", fn);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "Num TB");
	int tb = tmp & 0x0f;
	old_object = _replace_result_int(records, "Num TB", tb);
	Py_DECREF(old_object);

	
    PyObject* records_list = PyList_New(0);
	for (int i = 0; i < tb; i++) {
		std::string name = "TB info[" + std::to_string(i) + "]";
		const char* char_name = name.c_str();
		offset += _decode_TB_Info_v4(b, offset, length, records_list, char_name);
	}
	t = Py_BuildValue("(sOs)", "TB Info", records_list, "list");
	PyList_Append(records, t);
	Py_DECREF(records_list);
	Py_DECREF(t);
    
    const char* s_name = lc_name.c_str();
	t = Py_BuildValue("(sOs)", s_name, records, "dict");
	PyList_Append(result, t);
	Py_DECREF(records);
	Py_DECREF(t);
	return offset - start;

}
static int
_decode_nr_l2_ul_tb_subpkt(const char* b, int offset, size_t length,
	PyObject* result) {
	int start = offset;
	int pkt_ver = _search_result_int(result, "Version");
	int tmp;
	int value;
	int tti_num = 0;
	bool success = 0;
	PyObject* tmp_py = PyList_New(0);
	PyObject* t = NULL;
	switch (pkt_ver)
	{
	case 4:
		offset += _decode_by_fmt(NrMeta_Fmt_v4,
			ARRAY_SIZE(NrMeta_Fmt_v4, Fmt),
			b, offset, length, result);
		tmp = _search_result_int(result, "Num TTI");
		tti_num = tmp;
		value = tmp & 0x0f;
		t = _replace_result_int(result, "Num TTI", value);
		Py_DECREF(t);

		value = (tmp >> 4) & 0b1;
		t = _replace_result_int(result, "Is Type2 Scell", value);
		Py_DECREF(t);

		value = (tmp >> 5) & 0b1;
		t = _replace_result_int(result, "Is Type2 Other Cell", value);
		Py_DECREF(t);
		for (int i = 0; i < tti_num; i++) {
			std::string s = "TTI Info [" + std::to_string(i) + "]";
			offset += _decode_TTI_Info_v4(b, offset, length, tmp_py, s);
		}
		t = Py_BuildValue("(sOs)", "TTI Info", tmp_py, "list");
		PyList_Append(result, t);
		Py_DECREF(tmp_py);
		Py_DECREF(t);
		success = 1;
		break;

	default:
		break;
	}
	if (!success) {
		
		printf("(MI)Unknown 5G NR L2 UL TB: %d\n", pkt_ver);
	}

	return offset - start;
}


