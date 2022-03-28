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

const Fmt NrMacUlPhyChannelScheduleSystime_Fmt_v2_11[] = {
    {UINT,"Slot",1},
    {UINT,"Numerology",1},
    {UINT,"Frame",2},
};

const Fmt NrMacUlPhyChannelScheduleRecords_Fmt_v2_11[] = {
    {UINT,"Num Carrier",1},
    {SKIP,NULL,3},
};

const Fmt NrMacUlPhyChannelScheduleCarriers_Fmt_v2_11[] = {
    {UINT,"Carrier ID",1},
    {PLACEHOLDER,"RNTI Type",0},
    {UINT,"Phychan Bit Mask",1},
    {PLACEHOLDER,"Dual Pol Status",0},
    {SKIP,NULL,2},
};

const Fmt NrMacUlPhyChannelSchedulePUSCH_Fmt_v2_11[] = {
    {UINT,"Is Second Phychan",1},
    {PLACEHOLDER,"Start Symbol",0},
    {UINT,"Num Symbols",1},
    {PLACEHOLDER,"HARQ ID",0},
    {UINT,"MCS",1},
    {PLACEHOLDER,"MCS Table",0},
    {PLACEHOLDER,"DMRS Add Pos",0},
    {UINT,"RB Start",1},
    {UINT,"Num RBs",2},
    {PLACEHOLDER,"RA Type",0},
    {PLACEHOLDER,"Mapping Type",0},
    {UINT,"TB Size( bytes)",2},
    {PLACEHOLDER,"DMRS Config Type",0},
    {PLACEHOLDER,"Antenna Switch Indicator",0},
    {UINT,"TX Mode",1},
    {PLACEHOLDER,"RV Index",0},
    {PLACEHOLDER,"DMRS PTRS Asoc",0},
    {PLACEHOLDER,"Beta Oddset Ind",0},
    {UINT,"TX Type",1},
    {PLACEHOLDER,"BWP Idx",0},
    {UINT,"Code Rate",2},
    {PLACEHOLDER,"UCI Request Mask",0},
    {UINT,"Num CSF P1 Bits",2},
    {UINT,"Num CSF P2 Bits",1},
    {UINT,"Num HARQ ACK Bits",1},
    {PLACEHOLDER,"Transform Precoding",0},
    {UINT,"K Prime Value",2},
    {UINT,"ZC Value",1},
    {UINT,"Num CBs",1},
    {UINT,"DMRS Symbol Bitmask",2},
    {PLACEHOLDER,"SRS Res Ind",0},
    {UINT,"CB Size",2},
    {PLACEHOLDER,"Freq Hop Flag",0},
    {UINT,"Tx Slot Offset",1},
    {UINT,"TPC Accum 0",1},
    {UINT,"TPC Accum 1",1},
    {UINT,"TPMI",1},
    {PLACEHOLDER,"Data Scrambling Selection",0},
    {PLACEHOLDER,"DMRS Ports[0]",0},
    {UINT,"DMRS Ports[1]",1},
    {UINT,"RB Start Hop",1},
    {PLACEHOLDER,"MOD_TYPE",0},
    {UINT,"RNTI Value",2},
    {UINT,"CDM Groups",1},
    {PLACEHOLDER,"DMRS Scrambing Selection",0},
    {SKIP,NULL ,3},
    {UINT,"RBG Bitmap",4},
};

const Fmt NrMacUlPhyChannelSchedulePUCCH_Fmt_v2_11[] = {
    {UINT,"Num PUCCH",1},
    {SKIP,NULL,3},
};

const Fmt NrMacUlPhyChannelSchedulePerPUCCH_Fmt_v2_11[] = {
    {UINT,"Is Second Phychan",1},
    {PLACEHOLDER,"PUCCH Format",0},
    {PLACEHOLDER,"UCI Request BMask",0},
    {UINT,"Start symbol",1},
    {PLACEHOLDER,"Num Symbols",0},
    {UINT,"Starting RB",2},
    {UINT,"Num RB",1},
    {UINT,"Freq Hopping Flag",1},
    {UINT,"Second Hop RB",1},
    {UINT,"Num HARQ ACK Bits",1},
    {PLACEHOLDER,"Num SR Bits",0},
    {UINT,"Num UCI P1 Bits",2},
    {UINT,"Num UCI P2 Bits",2},
    {PLACEHOLDER,"Spatial Relation Id",0},
    {PLACEHOLDER,"Antenna Switch Indictor",0},
    {UINT,"M0",1},
    {PLACEHOLDER,"Time OCC Index",0},
    {UINT,"I DMRS",1},
    {PLACEHOLDER,"DFT 0CC Length",0},
    {PLACEHOLDER,"DFT 0CC Index",0},
    {UINT,"TPC Accum 0",1},
    {UINT,"TPC Accum 1",1},
};
const Fmt NrMacUlPhyChannelSchedulePrach_Fmt_v2_11[] = {
	{UINT,"Resource Allocation",2},
	{UINT,"ZC Root Seq",1},
	{PLACEHOLDER,"Preamble Format",0},
	{UINT,"Symbol Offset",1},
	{PLACEHOLDER,"Numerology",0},
	{UINT,"PRACH Number",2},
	{UINT,"ZC Cyclic Shift",2},
};

const ValueName prach_Numerology_v2_11[] = {
	{1,"30KHZ"},
};

const ValueName PCSRSystemTimeNumerology_v2_11[] = {
	{0,"15KHZ"},
};
const ValueName NRMacULPhyCHAScheRntiType_v2_11[] = {
    {0b0000,"C_RNTI"},
    {0B0010,"T_C_RNTI"},
};
const ValueName NRMacULPhyCHASchePhychanBitMask_v2_11[] = {
    {0b000010,"PUSCH"},
    {0b000100,"PUCCH"},
    {0b000110,"PUSCH|PUCCH"},
	{0b010000,"PRACH"},
};
const ValueName NRMacULPhyCHAScheMCSTable_v2_11[] = {
    {0b0000,"64QAM"},
};
const  ValueName NRMacULPhyCHAScheMappingType_v2_11[] = {
    {1,"Type B"},
};
const ValueName NRMacULPhyCHAScheTxMode_v2_11[] = {
    {0,"SISO"},
};
const  ValueName NRMacULPhyCHAScheTXType_v2_11[] = {
    {0,"NEW_TX"},
};

const  ValueName NRMacULPhyCHAScheDataScramblingSelection_v2_11[] = {
    {0b01,"FROM_CELL_ID"},
    {0,"FROM_CFG"},
};
const  ValueName NRMacULPhyCHAScheMOD_TYPE_v2_11[] = {
    {0b010,"16QAM"},
    {0b001,"QPSK"},
};

const  ValueName NRMacULPhyCHAScheMDMRSScrambingSelection_v2_11[] = {
    {0,"NSCID_0"},
    {0b0010,"CELL_ID"},
};

const  ValueName NRMacULPhyCHASchePUCCHFormat_v2_11[] = {
    {0b1000,"PUCCH_FORMAT_F0"},
};

const  ValueName NRMacULPhyCHAScheUCIRequestBMask_v2_11[] = {
    {0,"ACK_NACK_PRT"},
};
const  ValueName NRMacULPhyCHAScheMFreqHoppingFlag_v2_11[] = {
    {0,"HOP_MODE_NEITHER"},
};
static int
_decode_nr_mac_ul_pcsr_system_time_v2_11(const char* b, int offset, size_t length,
	PyObject* result, const char* lc_name) {
	int start = offset;
	PyObject* records = PyList_New(0);
	PyObject* t = NULL;
	offset += _decode_by_fmt(NrMacUlPhyChannelScheduleSystime_Fmt_v2_11,
		ARRAY_SIZE(NrMacUlPhyChannelScheduleSystime_Fmt_v2_11, Fmt),
		b, offset, length, records);
	int tmp = _search_result_int(records, "Numerology");
	int num = tmp & 0x0f;
	PyObject* old_object = _replace_result_int(records, "Numerology", num);
	Py_DECREF(old_object);
    (void)_map_result_field_to_name(records,
		"Numerology", PCSRSystemTimeNumerology_v2_11,
		ARRAY_SIZE(PCSRSystemTimeNumerology_v2_11, ValueName),
		"(MI)Unknown");
	tmp = _search_result_int(records, "Frame");
	int frame = tmp & 0x3ff;
	old_object = _replace_result_int(records, "Frame", frame);
	Py_DECREF(old_object);

	t = Py_BuildValue("(sOs)", lc_name, records, "dict");
	PyList_Append(result, t);
	Py_DECREF(records);
	Py_DECREF(t);
	return offset - start;
}

static int
_decode_nr_mac_ul_pcsr_pusch_v2_11(const char* b, int offset, size_t length,
	PyObject* result, const char* lc_name) {
	int start = offset;
	PyObject* records = PyList_New(0);
	PyObject* t = NULL;
	offset += _decode_by_fmt(NrMacUlPhyChannelSchedulePUSCH_Fmt_v2_11,
		ARRAY_SIZE(NrMacUlPhyChannelSchedulePUSCH_Fmt_v2_11, Fmt),
		b, offset, length, records);

	int tmp = _search_result_int(records, "Is Second Phychan");
	int value = tmp & 0b1;
	PyObject* old_object = _replace_result_int(records, "Is Second Phychan", value);
	Py_DECREF(old_object);

	value = (tmp >> 1) & 0x0f;
	old_object = _replace_result_int(records, "Start Symbol", value);
	Py_DECREF(old_object);

	int tmp2 = _search_result_int(records, "Num Symbols");
	value = ((tmp2 & 0b1) << 3) + ((tmp >> 5) & 0b111);
	old_object = _replace_result_int(records, "Num Symbols", value);
	Py_DECREF(old_object);

	value = ((tmp2 >> 1) & 0x0f);
	old_object = _replace_result_int(records, "HARQ ID", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "MCS");
	value = ((tmp & 0b1) << 3) + ((tmp2 >> 5) & 0b111);
	old_object = _replace_result_int(records, "MCS", value);
	Py_DECREF(old_object);

	value = (tmp >> 1) & 0x0f;
	old_object = _replace_result_int(records, "MCS Table", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"MCS Table", NRMacULPhyCHAScheMCSTable_v2_11,
		ARRAY_SIZE(NRMacULPhyCHAScheMCSTable_v2_11, ValueName),
		"(MI)Unknown");

	value = (tmp >> 5) & 0b11;
	old_object = _replace_result_int(records, "DMRS Add Pos", value);
	Py_DECREF(old_object);

	tmp2 = _search_result_int(records, "RB Start");
	value = (tmp2 << 1) + (tmp >> 7);
	old_object = _replace_result_int(records, "RB Start", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "Num RBs");
	value = tmp & 0x1ff;
	old_object = _replace_result_int(records, "Num RBs", value);
	Py_DECREF(old_object);

	value = (tmp >> 9) & 0b1;
	old_object = _replace_result_int(records, "RA Type", value);
	Py_DECREF(old_object);

	value = (tmp >> 10) & 0b1;
	old_object = _replace_result_int(records, "Mapping Type", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"Mapping Type", NRMacULPhyCHAScheMappingType_v2_11,
		ARRAY_SIZE(NRMacULPhyCHAScheMappingType_v2_11, ValueName),
		"(MI)Unknown");

	tmp2 = _search_result_int(records, "TB Size( bytes)");
	value = ((tmp2 & 0x1fff) << 5) + (tmp >> 3);
	old_object = _replace_result_int(records, "TB Size( bytes)", value);
	Py_DECREF(old_object);

	value = (tmp2 >> 13) & 0b11;
	old_object = _replace_result_int(records, "DMRS Config Type", value);
	Py_DECREF(old_object);

	value = (tmp2 >> 15);
	old_object = _replace_result_int(records, "Antenna Switch Indicator", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "TX Mode");
	value = tmp & 0b11;
	old_object = _replace_result_int(records, "TX Mode", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"TX Mode", NRMacULPhyCHAScheTxMode_v2_11,
		ARRAY_SIZE(NRMacULPhyCHAScheTxMode_v2_11, ValueName),
		"(MI)Unknown");

	value = (tmp >> 2) & 0b111;
	old_object = _replace_result_int(records, "RV Index", value);
	Py_DECREF(old_object);

	value = (tmp >> 5) & 0b1;
	old_object = _replace_result_int(records, "DMRS PTRS Asoc", value);
	Py_DECREF(old_object);

	value = (tmp >> 6) & 0b11;
	old_object = _replace_result_int(records, "Beta Oddset Ind", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "TX Type");
	value = tmp & 0b11;
	old_object = _replace_result_int(records, "TX Type", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"TX Type", NRMacULPhyCHAScheTXType_v2_11,
		ARRAY_SIZE(NRMacULPhyCHAScheTXType_v2_11, ValueName),
		"(MI)Unknown");
	value = (tmp >> 2) & 0b11;
	old_object = _replace_result_int(records, "BWP Idx", value);
	Py_DECREF(old_object);

	tmp2 = _search_result_int(records, "Code Rate");
	value = ((tmp2 & 0xfff) << 4) + (tmp >> 4);
	old_object = _replace_result_int(records, "Code Rate", value);
	Py_DECREF(old_object);

	value = tmp2 >> 12;
	old_object = _replace_result_int(records, "UCI Request Mask", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "Num CSF P1 Bits");
	value = tmp & 0x3ff;
	old_object = _replace_result_int(records, "Num CSF P1 Bits", value);
	Py_DECREF(old_object);

	tmp2 = _search_result_int(records, "Num CSF P2 Bits");
	value = (tmp2 << 6) + (tmp >> 10);
	old_object = _replace_result_int(records, "Num CSF P2 Bits", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "Num HARQ ACK Bits");
	value = tmp & 0x3f;
	old_object = _replace_result_int(records, "Num HARQ ACK Bits", value);
	Py_DECREF(old_object);

	value = (tmp >> 6) & 0b11;
	old_object = _replace_result_int(records, "Transform Precoding", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "K Prime Value");
	value = tmp & 0x7fff;
	old_object = _replace_result_int(records, "K Prime Value", value);
	Py_DECREF(old_object);

	tmp2 = _search_result_int(records, "ZC Value");
	value = (tmp2 << 1) + (tmp >> 15);
	old_object = _replace_result_int(records, "ZC Value", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "DMRS Symbol Bitmask");
	value = tmp & 0x3fff;
	old_object = _replace_result_int(records, "DMRS Symbol Bitmask", value);
	Py_DECREF(old_object);

	value = tmp >> 14;
	old_object = _replace_result_int(records, "SRS Res Ind", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "CB Size");
	value = tmp & 0x7fff;
	old_object = _replace_result_int(records, "CB Size", value);
	Py_DECREF(old_object);

	value = tmp >> 15;
	old_object = _replace_result_int(records, "Freq Hop Flag", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "Tx Slot Offset");
	value = tmp & 0x3f;
	old_object = _replace_result_int(records, "Tx Slot Offset", value);
	Py_DECREF(old_object);

	tmp2 = _search_result_int(records, "TPC Accum 0");
	value = ((tmp2 & 0x3f) << 2) + (tmp >> 6);
	old_object = _replace_result_int(records, "TPC Accum 0", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "TPC Accum 1");
	value = ((tmp & 0x3f) << 2) + (tmp2 >> 6);
	old_object = _replace_result_int(records, "TPC Accum 1", value);
	Py_DECREF(old_object);

	tmp2 = _search_result_int(records, "TPMI");
	value = ((tmp2 & 0b111) << 2) + (tmp >> 6);
	old_object = _replace_result_int(records, "TPMI", value);
	Py_DECREF(old_object);

	value = (tmp2 >> 3) & 0b11;
	old_object = _replace_result_int(records, "Data Scrambling Selection", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"Data Scrambling Selection", NRMacULPhyCHAScheDataScramblingSelection_v2_11,
		ARRAY_SIZE(NRMacULPhyCHAScheDataScramblingSelection_v2_11, ValueName),
		"(MI)Unknown");

	value = (tmp2 >> 5) & 0b111;
	old_object = _replace_result_int(records, "DMRS Ports[0]", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "DMRS Ports[1]");
	value = tmp & 0x0f;
	old_object = _replace_result_int(records, "DMRS Ports[1]", value);
	Py_DECREF(old_object);

	tmp2 = _search_result_int(records, "RB Start Hop");
	value = ((tmp2 & 0x1f) << 4) + (tmp >> 4);
	old_object = _replace_result_int(records, "RB Start Hop", value);
	Py_DECREF(old_object);

	value = tmp2 >> 5;
	old_object = _replace_result_int(records, "MOD_TYPE", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"MOD_TYPE", NRMacULPhyCHAScheMOD_TYPE_v2_11,
		ARRAY_SIZE(NRMacULPhyCHAScheMOD_TYPE_v2_11, ValueName),
		"(MI)Unknown");

	tmp = _search_result_int(records, "CDM Groups");
	value = tmp & 0b11;
	old_object = _replace_result_int(records, "CDM Groups", value);
	Py_DECREF(old_object);

	value = (tmp >> 2) & 0x0f;
	old_object = _replace_result_int(records, "DMRS Scrambing Selection", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"DMRS Scrambing Selection", NRMacULPhyCHAScheMDMRSScrambingSelection_v2_11,
		ARRAY_SIZE(NRMacULPhyCHAScheMDMRSScrambingSelection_v2_11, ValueName),
		"(MI)Unknown");

	tmp = _search_result_int(records, "RBG Bitmap");
	if (tmp == 0) {
		std::string tmp_value = "NA";
		old_object = _replace_result_string(records, "RBG Bitmap", tmp_value);
		Py_DECREF(old_object);
	}

	t = Py_BuildValue("(sOs)", lc_name, records, "dict");
	PyList_Append(result, t);
	Py_DECREF(records);
	Py_DECREF(t);
	return offset - start;
}
static int
_decode_nr_mac_ul_pcsr_per_pucch_v2_11(const char* b, int offset, size_t length,
	PyObject* result, std::string lc_name) {
	int start = offset;
	PyObject* records = PyList_New(0);
	PyObject* t = NULL;
	offset += _decode_by_fmt(NrMacUlPhyChannelSchedulePerPUCCH_Fmt_v2_11,
		ARRAY_SIZE(NrMacUlPhyChannelSchedulePerPUCCH_Fmt_v2_11, Fmt),
		b, offset, length, records);
	int tmp = _search_result_int(records, "Is Second Phychan");
	int value = tmp & 0b1;
	PyObject* old_object = _replace_result_int(records, "Is Second Phychan", value);
	Py_DECREF(old_object);

	value = (tmp >> 1) & 0x0f;
	old_object = _replace_result_int(records, "PUCCH Format", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"PUCCH Format", NRMacULPhyCHASchePUCCHFormat_v2_11,
		ARRAY_SIZE(NRMacULPhyCHASchePUCCHFormat_v2_11, ValueName),
		"(MI)Unknown");

	value = (tmp >> 5) & 0x0f;
	old_object = _replace_result_int(records, "UCI Request BMask", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"UCI Request BMask", NRMacULPhyCHAScheUCIRequestBMask_v2_11,
		ARRAY_SIZE(NRMacULPhyCHAScheUCIRequestBMask_v2_11, ValueName),
		"(MI)Unknown");

	tmp = _search_result_int(records, "Start symbol");
	value = tmp & 0x0f;
	old_object = _replace_result_int(records, "Start symbol", value);
	Py_DECREF(old_object);

	value = (tmp >> 4) & 0x0f;
	old_object = _replace_result_int(records, "Num Symbols", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "Starting RB");
	value = tmp & 0x3ff;
	old_object = _replace_result_int(records, "Starting RB", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "Freq Hopping Flag");
	value = tmp & 0b111;
	old_object = _replace_result_int(records, "Freq Hopping Flag", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"Freq Hopping Flag", NRMacULPhyCHAScheMFreqHoppingFlag_v2_11,
		ARRAY_SIZE(NRMacULPhyCHAScheMFreqHoppingFlag_v2_11, ValueName),
		"(MI)Unknown");

	int tmp2 = _search_result_int(records, "Second Hop RB");
	value = ((tmp2 & 0x0f) << 5) + (tmp >> 3);
	old_object = _replace_result_int(records, "Second Hop RB", value);
	Py_DECREF(old_object);

	if (value == 0x1ff) {
		std::string tmp_value = "NA";
		old_object = _replace_result_string(records, "Second Hop RB", tmp_value);
		Py_DECREF(old_object);
	}

	tmp = _search_result_int(records, "Num HARQ ACK Bits");
	value = ((tmp & 0b11) << 4) + (tmp2 >> 4);
	old_object = _replace_result_int(records, "Num HARQ ACK Bits", value);
	Py_DECREF(old_object);

	value = tmp >> 2;
	old_object = _replace_result_int(records, "Num SR Bits", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "Num UCI P1 Bits");
	value = tmp & 0x3ff;
	old_object = _replace_result_int(records, "Num UCI P1 Bits", value);
	Py_DECREF(old_object);

	tmp2 = _search_result_int(records, "Num UCI P2 Bits");
	value = ((tmp2 & 0x3ff) << 6) + (tmp >> 10);
	old_object = _replace_result_int(records, "Num UCI P2 Bits", value);
	Py_DECREF(old_object);

	value = (tmp2 >> 10) & 0x1f;
	old_object = _replace_result_int(records, "Spatial Relation Id", value);
	Py_DECREF(old_object);

	value = (tmp2 >> 15);
	old_object = _replace_result_int(records, "Antenna Switch Indictor", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "M0");
	value = tmp & 0x0f;
	old_object = _replace_result_int(records, "M0", value);
	Py_DECREF(old_object);

	value = (tmp >> 4) & 0b11;
	old_object = _replace_result_int(records, "Time OCC Index", value);
	Py_DECREF(old_object);

	tmp2 = _search_result_int(records, "I DMRS");
	value = ((tmp2 & 0b11) << 2) + (tmp >> 6);
	old_object = _replace_result_int(records, "I DMRS", value);
	Py_DECREF(old_object);

	value = (tmp2 >> 2) & 0b111;
	old_object = _replace_result_int(records, "DFT 0CC Length", value);
	Py_DECREF(old_object);

	value = (tmp2 >> 5) & 0b111;
	old_object = _replace_result_int(records, "DFT 0CC Index", value);
	Py_DECREF(old_object);

	const char* s_name = lc_name.c_str();
	t = Py_BuildValue("(sOs)", s_name, records, "dict");
	PyList_Append(result, t);
	Py_DECREF(records);
	Py_DECREF(t);
	return offset - start;
}
static int
_decode_nr_mac_ul_pcsr_pucch_v2_11(const char* b, int offset, size_t length,
	PyObject* result, const char* lc_name) {
	int start = offset;
	PyObject* records = PyList_New(0);
	PyObject* t = NULL;
	offset += _decode_by_fmt(NrMacUlPhyChannelSchedulePUCCH_Fmt_v2_11,
		ARRAY_SIZE(NrMacUlPhyChannelSchedulePUCCH_Fmt_v2_11, Fmt),
		b, offset, length, records);

	int tmp = _search_result_int(records, "Num PUCCH");
	PyObject* records_list = PyList_New(0);
	for (int i = 0; i < tmp; i++) {
		std::string name = "Per PUCCH Data[" + std::to_string(i) + "]";
		offset += _decode_nr_mac_ul_pcsr_per_pucch_v2_11(b, offset, length, records_list, lc_name);
	}
	t = Py_BuildValue("(sOs)", "Per PUCCH Data", records_list, "list");
	PyList_Append(records, t);
	Py_DECREF(records_list);
	Py_DECREF(t);

	t = Py_BuildValue("(sOs)", lc_name, records, "dict");
	PyList_Append(result, t);
	Py_DECREF(records);
	Py_DECREF(t);
	return offset - start;


}
static int
_decode_nr_mac_ul_pcsr_PRACH_v2_11(const char* b, int offset, size_t length,
	PyObject* result, const char* lc_name) {
	int start = offset;
	PyObject* records = PyList_New(0);
	PyObject* t = NULL;
	offset += _decode_by_fmt(NrMacUlPhyChannelSchedulePrach_Fmt_v2_11,
		ARRAY_SIZE(NrMacUlPhyChannelSchedulePrach_Fmt_v2_11, Fmt),
		b, offset, length, records);
	int tmp = _search_result_int(records, "Resource Allocation");
	int value = tmp & 0x1ff;
	PyObject* old_object = _replace_result_int(records, "Resource Allocation", value);
	Py_DECREF(old_object);

	int tmp2 = _search_result_int(records, "ZC Root Seq");
	value = ((tmp2 & 0b11) << 7) + (tmp >> 9);
	old_object = _replace_result_int(records, "ZC Root Seq", value);
	Py_DECREF(old_object);

	value = tmp2 >> 2;
	old_object = _replace_result_int(records, "Preamble Format", value);
	Py_DECREF(old_object);

	tmp = _search_result_int(records, "Symbol Offset");
	value = tmp & 0x0f;
	old_object = _replace_result_int(records, "Symbol Offset", value);
	Py_DECREF(old_object);

	value = (tmp >> 4) & 0b11;
	old_object = _replace_result_int(records, "Numerology", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"Numerology", prach_Numerology_v2_11,
		ARRAY_SIZE(prach_Numerology_v2_11, ValueName),
		"(MI)Unknown");
	tmp = _search_result_int(records, "PRACH Number");
	value = tmp & 0x1ff;
	old_object = _replace_result_int(records, "PRACH Number", value);
	Py_DECREF(old_object);

	tmp2 = _search_result_int(records, "ZC Cyclic Shift");
	value = ((tmp2 & 0x1ff) << 7) + (tmp >> 9);
	old_object = _replace_result_int(records, "ZC Cyclic Shift", value);
	Py_DECREF(old_object);

	t = Py_BuildValue("(sOs)", lc_name, records, "dict");
	PyList_Append(result, t);
	Py_DECREF(records);
	Py_DECREF(t);
	return offset - start;


}

static int
_decode_nr_mac_ul_pcsr_carriers_v2_11(const char* b, int offset, size_t length,
	PyObject* result, std::string lc_name) {
	int start = offset;
	PyObject* records = PyList_New(0);
	PyObject* t = NULL;
	offset += _decode_by_fmt(NrMacUlPhyChannelScheduleCarriers_Fmt_v2_11,
		ARRAY_SIZE(NrMacUlPhyChannelScheduleCarriers_Fmt_v2_11, Fmt),
		b, offset, length, records);
	int tmp = _search_result_int(records, "Carrier ID");
	int value = tmp & 0b11;
	PyObject* old_object = _replace_result_int(records, "Carrier ID", value);
	Py_DECREF(old_object);

	value = (tmp >> 2) & 0x0f;
	old_object = _replace_result_int(records, "RNTI Type", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"RNTI Type", NRMacULPhyCHAScheRntiType_v2_11,
		ARRAY_SIZE(NRMacULPhyCHAScheRntiType_v2_11, ValueName),
		"(MI)Unknown");

	int tmp2 = _search_result_int(records, "Phychan Bit Mask");
	value = ((tmp2 & 0x0f) << 2) + ((tmp >> 6) & 0b11);
	int ch = value;
	old_object = _replace_result_int(records, "Phychan Bit Mask", value);
	Py_DECREF(old_object);
	(void)_map_result_field_to_name(records,
		"Phychan Bit Mask", NRMacULPhyCHASchePhychanBitMask_v2_11,
		ARRAY_SIZE(NRMacULPhyCHASchePhychanBitMask_v2_11, ValueName),
		"(MI)Unknown");

	value = ((tmp2 >> 4) & 0b11);
	old_object = _replace_result_int(records, "Dual Pol Status", value);
	Py_DECREF(old_object);
    
	if (ch & 0b010)//PUSCH 
	{
		offset += _decode_nr_mac_ul_pcsr_pusch_v2_11(b, offset, length, records, "PUSCH");
	}
    
	if (ch & 0b100)//PUCCH
	{
		offset += _decode_nr_mac_ul_pcsr_pucch_v2_11(b, offset, length, records, "Per PUCCH Data");
	}
    if(ch&0b010000)//PRACH
	{
       offset += _decode_nr_mac_ul_pcsr_PRACH_v2_11(b, offset, length, records, "PRACH");
    }
	const char* s_name = lc_name.c_str();
	t = Py_BuildValue("(sOs)", s_name, records, "dict");
	PyList_Append(result, t);
	Py_DECREF(records);
	Py_DECREF(t);
	return offset - start;
}
static int
_decode_nr_mac_ul_pcsr_records_v2_11(const char* b, int offset, size_t length,
	PyObject* result, std::string lc_name) {
	int start = offset;
	PyObject* records = PyList_New(0);
	PyObject* t = NULL;
	offset += _decode_nr_mac_ul_pcsr_system_time_v2_11(b, offset, length, records, "System time");
	offset += _decode_by_fmt(NrMacUlPhyChannelScheduleRecords_Fmt_v2_11,
		ARRAY_SIZE(NrMacUlPhyChannelScheduleRecords_Fmt_v2_11, Fmt),
		b, offset, length, records);
	int tmp = _search_result_int(records, "Num Carrier");
    
	PyObject* records_list = PyList_New(0);
	for (int i = 0; i < tmp; i++) {
		std::string s_name = "Carriers[" + std::to_string(i) + "]";
		offset += _decode_nr_mac_ul_pcsr_carriers_v2_11(b, offset, length, records_list, lc_name);
	}
	t = Py_BuildValue("(sOs)", "Carriers", records_list, "list");
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
_decode_nr_mac_ul_physical_channel_schedule_report_subpkt(const char *b, int offset, size_t length,
                       PyObject *result) {

    int start = offset;
    int major_ver = _search_result_int(result, "Major Version");
    int minor_ver = _search_result_int(result, "Minor Version");
    int n_records = _search_result_int(result, "Num Records");
    bool success = false;
	PyObject* tmp_py = PyList_New(0);
	PyObject* t = NULL;
    int tmp;
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
                
                case 11: {
					tmp = _search_result_int(result, "Num Records");
					for (int i = 0; i < tmp; i++) {
						std::string name = "Records[" + std::to_string(i) + "]";
						offset += _decode_nr_mac_ul_pcsr_records_v2_11(b, offset, length, tmp_py, name);
					}
					t = Py_BuildValue("(sOs)", "Records", tmp_py, "list");
					PyList_Append(result, t);
					Py_DECREF(tmp_py);
					Py_DECREF(t);
					success = 1;
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

        printf("(MI)Unknown 5G_NR_MAC_UL_Physical_Channel_Schedule_Report: %d.%d\n", major_ver, minor_ver);
    }

    return offset - start;
}


