/*
 * NR_DCI_Message
 */
#include <iostream>
#include <iomanip>
#include <sstream>

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

// 4 bytes
const Fmt NrDciMessage_MacVersion_Fmt[] = {
        {UINT, "Minor Version", 2}, // 16 bits
        {UINT, "Major Version", 2}, // 16 bits
        {PLACEHOLDER, "Major.Minor Version", 0}, // 0 bits
        {PLACEHOLDER, "{id: 2026230 }", 0} // 0 bits
};

// 8 bytes
const Fmt NrDciMessage_LogFieldsChange_Fmt[] = {
        {UINT, "Sleep", 1}, // 8 bits
        {UINT, "Beam Change", 1}, // 8 bits
        {UINT, "Signal Change", 1}, // 8 bits
        {UINT, "DL Dynamic Cfg Change", 1}, // 8 bits
        {UINT, "DL Config", 1}, // 8 bits
        {UINT, "UL Config", 1}, // 4 bits, 1 byte = 8 bits cover UL Config and ML1 State Change, do bit masking on the 1 byte to get UL Config
        {PLACEHOLDER, "ML1 State Change", 0}, // 4 bits, do bit masking on the 1 byte to get ML1 State Change
        {SKIP, NULL, 2} // 16 bits for Reserved
};

// 4 bytes
const Fmt NrDciMessage_Version_Fmt[] = {
        {UINT, "Log Fields Change BMask",  2}, // 16 bits
        {SKIP, NULL, 1}, // 8 bits for Reserved
        {UINT, "Num Records", 1} // 8 bits
};

// 8 bytes
const Fmt NrDciMessage_Record_Fmt[] = {
        {UINT, "Slot", 1}, // 8 bits
        {UINT, "Num", 1}, // 8 bits, 1 byte = 8 bits cover Num and Reserved0 (which we don't need), do bit masking on the 8 bits to get Num
        {UINT, "Frame", 2}, // 8 bits (1 byte) + last 2?? bits of the 2nd byte. 2 bytes cover Frame and Reserved (which we don't need). Do bit masking on the 2 bytes to get the Frame.
        {UINT, "Num DCI", 1}, // 8 bits
        {SKIP, NULL, 3} // 24 bits for reserved[0], reserved[1], reserved[2], which we don't need
};

const ValueName ValueNameNrDciMessage_Num[] = {
        {0,  "15kHz"}
};

// 4 bytes
const Fmt NrDciMessage_DciInfo_Fmt[] = {
        {UINT, "Carrier ID", 4}, // 2?? bits, 4 bytes = 32 bits cover Carrier ID, RNTI Type, DCI Format, Aggregation Level, Raw DCI Included, and Reserve (not needed). do bit masking to get Carrier ID
        {PLACEHOLDER, "RNTI Type", 0}, // 6?? bits, do bit masking on the 4 bytes to get RNTI Type
        {PLACEHOLDER, "DCI Format", 0}, // 8?? bits, do bit masking on the 4 bytes to get DCI Format
        {PLACEHOLDER, "Aggregation Level", 0}, // 8 bits, do bit masking on the 4 bytes to get Aggregation Level
        {PLACEHOLDER, "Raw DCI Included", 0} // 4?? bits, do bit masking on the 4 bytes to get Raw DCI Included
};

const ValueName ValueNameNrDciMessage_RntiType[] = {
        {0, "C_RNTI"},
        {8, "RA_RNTI"}
};

const ValueName ValueNameNrDciMessage_DciFormat[] = {
        {1, "UL_0_1"},
        {2, "DL_1_0"},
        {3, "DL_1_1"}
};

const ValueName ValueNameNrDciMessage_AggregationLevel[] = {
        {0, "LEVEL_1"},
        {1, "LEVEL_2"},
        {3, "LEVEL_8"}
};

const ValueName ValueNameNrDciMessage_PruneReasonMsb[] = {
        {0, "NONE"},
        {11, "INVALID_RES_MCS"}
};

// 12 bytes
const Fmt NrDciMessage_RawDci_Fmt[] = {
        {UINT, "Raw DCI Payload[0]", 4}, // 32 bits
        {UINT, "Raw DCI Payload[1]", 4}, // 32 bits
        {UINT, "Raw DCI Payload[2]", 4}, // 32 bits
        {PLACEHOLDER, "Raw DCI[2]", 0},
        {PLACEHOLDER, "Raw DCI[1]", 0},
        {PLACEHOLDER, "Raw DCI[0]", 0}
};

// 16 bytes
const Fmt NrDciMessage_UL_Fmt[] = {
        {UINT, "UL DCI Format", 4}, // 2?? bits, 4 bytes = 32 bits cover DCI Format, Carrier ID, NDI, MCS, Freq Hopping Flag, RV, HARQ ID, PUSCH TPC, UL SUL Ind, PTRS DMRS Association, Beta Offset Ind. Do bit masking on the 2 bytes to get DCI Format
        {PLACEHOLDER, "Carrier ID", 0}, // 4?? bits, do bit masking on the 4 bytes to get Carrier ID
        {PLACEHOLDER, "NDI", 0}, // 2?? bits, do bit masking on the 4 bytes to get NDI
        {PLACEHOLDER, "MCS", 0}, // ?? bits, do bit masking on the 4 bytes to get MCS
        {PLACEHOLDER, "Freq Hopping Flag", 0}, // ?? bits, do bit masking on the 4 bytes to get Freq Hopping Flag (probably 1 bit)
        {PLACEHOLDER, "RV", 0}, // ?? bits, do bit masking on the 4 bytes to get RV
        {PLACEHOLDER, "HARQ ID", 0}, // ?? bits, do bit masking on the 4 bytes to get HARQ ID
        {PLACEHOLDER, "PUSCH TPC", 0}, // ?? bits, do bit masking on the 4 bytes to get PUSCH TPC
        {PLACEHOLDER, "UL SUL Ind", 0}, // ?? bits, do bit masking on the 4 bytes to get UL SUL Ind
        {PLACEHOLDER, "Symbol Alloc Index", 0}, // ?? bits, do bit masking on the 4 bytes to get Symbol Alloc Index
        {PLACEHOLDER, "BWP Ind", 0}, // ?? bits, do bit masking on the 4 bytes to get BWP Ind
        {PLACEHOLDER, "PTRS DMRS Association", 0}, // ?? bits, do bit masking on the 4 bytes to get PTRS DMRS Association
        {PLACEHOLDER, "Beta Offset Ind", 0}, // ?? bits, do bit masking on the 4 bytes to get Beta Offset Ind
        {UINT, "RB Assignment", 4}, // ?? bits, 4 bytes = 32 bits cover RB Assignment, UL SCH Ind, DAI 1, DAI 2, SRS Resource Indication, Precoding Layer Info. Do bit masking on the 4 bytes to get RB Assignment
        {PLACEHOLDER, "UL SCH Ind", 0}, // ?? bits, do bit masking on the 4 bytes to get UL SCH Ind
        {PLACEHOLDER, "DAI 1", 0}, // ?? bits, do bit masking on the 4 bytes to get DAI 1
        {PLACEHOLDER, "DAI 2", 0}, // ?? bits, do bit masking on the 4 bytes to get DAI 2
        {PLACEHOLDER, "SRS Resource Indication", 0}, // ?? bits, do bit masking on the 4 bytes to get SRS Resource Indication
        {PLACEHOLDER, "Precoding Layer Info", 0}, // ?? bits, do bit masking on the 4 bytes to get Precoding Layer Info
        {UINT, "Antenna Ports", 1}, // ? bits, 1 byte = 8 bits cover Antenna Ports, DMRS Seq Init Flag, SRS Request. Do bit masking on the 1 byte to get Antenna Ports
        {PLACEHOLDER, "DMRS Seq Init Flag", 0}, // ? bits, do bit masking on the 1 byte to get DMRS Seq Init Flag
        {PLACEHOLDER, "SRS Request", 0}, // ? bits, do bit masking on the 1 byte to get SRS Request
        {UINT, "CSI Request", 2}, // ? bits, 2 bytes = 16 bits cover CSI Request, CBGTI, Reserved MCE Enable, RA Type. do bit masking on the 2 bytes to get CSI Request
        {PLACEHOLDER, "CBGTI", 0}, // ? bits, do bit masking on the 2 bytes to get CBGTI
        {PLACEHOLDER, "Reserved MCE Enable", 0}, // ? bits, do bit masking on the 2 bytes to get Reserved MCE Enable
        {PLACEHOLDER, "RA Type", 0}, // ? bits, do bit masking on the 2 bytes to get RA Type
        {UINT, "Prune Reason", 1}, // ? bits, 1 byte = 8 bits covers Prune Reason MSB and Reserved (not needed). Do bit masking on the 1 byte to get Prune Reason
        {UINT, "Pruned Mask", 4} // 32 bits
};

/*
CBG Transmission Info, CBG Flushing Out Info, Transmission Config Ind, SRS Request, Carrier ID are always 0 for every log I found

Bandwidth Part Indicator - bits 0-1
Time Resource Assignment - bits 2-5
TB 1 MCS = bits 6-10
TB 1 New Data Indicator - bit 11
DL Assignment Index - bits 12-15
TPC Command For Sched PUCCH - bits 16-17
PUCCH Resource Indicator - bits 18-20
PDSCH Harq Feedback Timing - bits 21-23
CBG Transmission Info - bits 24-31
CBG Flushing Out Info  - bit 32
Transmission Config Ind - 33-35
SRS Request - 36-37
Carrier ID - 38-40
bit 41 - don't know, but HARQ ID has to be bits 42-45 

HARQ ID - bits 42-45
*/
// 8 bytes
const Fmt NrDciMessage_DL_Fmt[] = {
        {UINT, "Bandwidth Part Indicator", 2}, // 2 bits, 2 bytes cover Bandwidth Part Indicator, Time Resource Assignment, TB 1 MCS, TB 1 New Data Indicator, DL Assignment Index. Do bit masking
        {PLACEHOLDER, "Time Resource Assignment", 0}, // 4 bits according to Internet
        {PLACEHOLDER, "TB 1 MCS", 0}, // 5 bits
        {PLACEHOLDER, "TB 1 New Data Indicator", 0}, // 1 bit
        {PLACEHOLDER, "DL Assignment Index", 0}, // 2 bits
        {UINT, "TPC Command For Sched PUCCH", 1}, // 2 bits, 1 byte covers TPC Command For Sched PUCCH, PUCCH Resource Indicator, PDSCH Harq Feedback Timing. Do bit masking
        {PLACEHOLDER, "PUCCH Resource Indicator", 0}, // 3 bits
        {PLACEHOLDER, "PDSCH Harq Feedback Timing", 0}, // 3 bits
        {UINT, "CBG Transmission Info", 1}, // 0, 2, 4, 6, 8 bits, assume 8 bits
        {UINT, "CBG Flushing Out Info", 2}, // 0, 1 bit, assume 1 bit, 2 bytes cover CBG Transmission Info, CBG Flushing Out, Transmission Config Ind, SRS Request, Carrier ID, HARQ ID. Do bit masking
        {PLACEHOLDER, "Transmission Config Ind", 0}, // 0, 3 bits, assume 3 bits
        {PLACEHOLDER, "SRS Request", 0}, // 2 bits
        {PLACEHOLDER, "Carrier ID", 0}, // 3 bits
        {PLACEHOLDER, "HARQ ID", 0}, // 4 bits
        {SKIP, NULL, 2} // last 2 bytes don't seem to be relevant to any field
};

static int _decode_nr_DCI (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;

    PyObject *macVersionList = PyList_New(0);

    offset += _decode_by_fmt(NrDciMessage_MacVersion_Fmt,
                            ARRAY_SIZE(NrDciMessage_MacVersion_Fmt, Fmt),
                            b, offset, length, macVersionList);
    // std::cout << "offset after decoding mac version: " << offset << std::endl;

    // get values of minor and major versions, populate Major.Minor Version and {id: 2026230 } fields with those values
    unsigned int minVersion = _search_result_uint(macVersionList, "Minor Version");
    unsigned int majVersion = _search_result_uint(macVersionList, "Major Version");
//     std::cout << "Minor Version: " << minVersion << std::endl;
//     std::cout << "Major Version: " << majVersion << std::endl;
    PyObject *old_object = _replace_result_int(macVersionList, "Major.Minor Version", majVersion);
    Py_DECREF(old_object);
    old_object = _replace_result_int(macVersionList, "{id: 2026230 }", minVersion);
    Py_DECREF(old_object);

    PyObject *macVersion = Py_BuildValue("(sOs)", "MacVersion", macVersionList, "dict");
    PyList_Append(result, macVersion);
    Py_DECREF(macVersion);
    Py_DECREF(macVersionList);

    PyObject *logFieldsChangeList = PyList_New(0);

    // decode NrDciMessage_LogFieldsChange_Fmt
    offset += _decode_by_fmt(NrDciMessage_LogFieldsChange_Fmt,
                ARRAY_SIZE(NrDciMessage_LogFieldsChange_Fmt, Fmt),
                b, offset, length, logFieldsChangeList);
//     std::cout << "offset after decoding version: " << offset << std::endl;
    
    // populate UL Config and ML1 State Change fields with their values by bit masking
    unsigned int utemp = _search_result_uint(logFieldsChangeList, "UL Config");

    unsigned int ulConfig = utemp & 0xF; // last 4 bits is ulConfig
//     std::cout << "UL Config: " << ulConfig << std::endl;
    unsigned int ml1StateChange = (utemp >> 4) & 0xF; // shift 4 bits to the right to get rid of ulConfig bits, last 4 bits of that result is ml1StateChange
//     std::cout << "ML1 State Change: " << ml1StateChange << std::endl;
    old_object = _replace_result_int(logFieldsChangeList, "UL Config", ulConfig);
    Py_DECREF(old_object);
    old_object = _replace_result_int(logFieldsChangeList, "ML1 State Change", ml1StateChange);
    Py_DECREF(old_object);

    PyObject *logFieldsChange = Py_BuildValue("(sOs)", "Log Fields Change", logFieldsChangeList, "dict");

    PyObject *version131077List = PyList_New(0);
    PyList_Append(version131077List, logFieldsChange);
    Py_DECREF(logFieldsChange);
    Py_DECREF(logFieldsChangeList);

    // decode NrDciMessage_Version_Fmt
    offset += _decode_by_fmt(NrDciMessage_Version_Fmt,
                ARRAY_SIZE(NrDciMessage_Version_Fmt, Fmt),
                b, offset, length, version131077List);

    PyObject *version131077 = Py_BuildValue("(sOs)", "Version 131077", version131077List, "dict");
    PyList_Append(result, version131077);
    Py_DECREF(version131077);
    Py_DECREF(version131077List);

    // build a list of records (there may be > 1 records, so we need to loop through each individual record to build the list)
    PyObject *recordList = PyList_New(0);
    unsigned int numRecords = _search_result_uint(version131077List, "Num Records");
//     std::cout << "Num Records: " << numRecords << std::endl;
    for (unsigned int recordIndex = 0; recordIndex < numRecords; recordIndex++) {
        PyObject *record = PyList_New(0);

        // decode individual record
        offset += _decode_by_fmt(NrDciMessage_Record_Fmt,
                ARRAY_SIZE(NrDciMessage_Record_Fmt, Fmt),
                b, offset, length, record);
        // std::cout << "offset after decoding record: " << offset << std::endl;
        
        // populate Num field by bit masking
        utemp = _search_result_uint(record, "Num");
        unsigned int num = utemp & 0xF; // last 4 bits is num
        // std::cout << "Num: " << num << std::endl;
        old_object = _replace_result_int(record, "Num", num);
        Py_DECREF(old_object);
        (void) _map_result_field_to_name(record, "Num",
                                             ValueNameNrDciMessage_Num,
                                             ARRAY_SIZE(ValueNameNrDciMessage_Num, ValueName),
                                             "Num Unknown");
        
        utemp = _search_result_uint(record, "Frame");
        unsigned int frame = utemp & 0x3FF;
        // std::cout << "Frame: " << frame << std::endl;
        old_object = _replace_result_int(record, "Frame", frame);
        Py_DECREF(old_object);

        // for each record, build a list of DCIs (there may be > 1 DCIs in one record, so we need to loop through each individual DCI to build the list)
        PyObject *dciList = PyList_New(0);
        unsigned int numDcis = _search_result_uint(record, "Num DCI");
        // std::cout << "Num DCI: " << numDcis << std::endl;
        for (unsigned int dciIndex = 0; dciIndex < numDcis; dciIndex++) {
            PyObject *dci = PyList_New(0);

            // decode individual DCI
            offset += _decode_by_fmt(NrDciMessage_DciInfo_Fmt,
                ARRAY_SIZE(NrDciMessage_DciInfo_Fmt, Fmt),
                b, offset, length, dci);
        //     std::cout << "offset after decoding dci info: " << offset << std::endl;
        
            /*
            --
            After _decode_by_fmt():
            0  0  0  0    0  0  0  0    0  0  0  0    0  0  0  0
            15 14 13 12   11 10 9  8    7  6  5  4    3  2  1  0

            

            Aggregation Level - bits 8-15
              bit 9, 10, 11, 12, 14, 15 cannot be ruled out
              bit 8 and 13 can be ruled out
                level 8 - 00x0 110x
                level 1 - 00x0 000x
                level 2 - 00x0 010x
            
            DCI Format - bits 4-11
              bit 4, 5, 6, 7, 8 cannot be ruled out
              bit 10, 11 can be ruled out
        
            Carrier ID - bits 0-3
              bit 0, 1, 2, 3 cannot be ruled out

            RNTI Type - bits 0-7
              bit 0, 1, 2, 3, 4, 5, 6, 7 cannot be ruled out
        
            Raw DCI Included - bits 12-15
              bit 13 is Raw DCI Included
            --
        
            Raw DCI Included is bit 13.
            Since Aggregation Level bits 10, 11 cannot be ruled out, and DCI Format bits 10, 11 can be ruled out, then bits 10, 11 are for Aggregation Level.
            Since DCI Format bit 8 cannot be ruled out, and Aggregation Level bit 8 can be ruled out, then bit 8 is for DCI Format.

            So now:
            Aggregation Level - bit 9, 12, 14, 15 cannot be ruled out. Bit 10, 11 are confirmed.
            DCI Format - bit 4, 5, 6, 7, 9 cannot be ruled out. Bit 8 is confirmed.
            Carrier ID - bit 0, 1, 2, 3 cannot be ruled out.
            RNTI Type - bit 0, 1, 2, 3, 4, 5, 6, 7 cannot be ruled out.
            Raw DCI Included - bit 13 is confirmed, Raw DCI Included is completed.

            There's 8 different DCI Formats, so it probably just needs 3 bits. Let us assume bits 7, 8, 9 (only 8 is confirmed) is the DCI Format field. This seems ok, since DCI Format has to span 2 bytes.
            There are 13 different RNTI Types, so it probably just needs 4 bits. Let us assume bits 3, 4, 5, 6 is the RNTI Type field. This seems ok, since RNTI Type has to span 2 bytes.
            According to Google, Carrier ID's bit length is either 0 or 3 bits. Let us assume bits 0, 1, 2 is the Carrier ID field. This seems ok, since Carrier ID has to span 1 byte only.
            According to Google, Aggregation Levels go from 1 to 8. Let us assume we need 3 bits for the Aggregation Level field. Let us assume bit 10, 11, 12 is the Aggregation Level field. This seems ok,
            since bit 10, 11 are confirmed, and Aggregation Level has to span 2 bytes.
            We know Raw DCI Included is bit 13.
            That just leaves bit 14, 15 as reserved.

            */
            utemp = _search_result_uint(dci, "Carrier ID");
            unsigned int carrierId = utemp & 0x7;
        //     std::cout << "Carrier ID: " << carrierId << std::endl;
            old_object = _replace_result_int(dci, "Carrier ID", carrierId);
            Py_DECREF(old_object);
            unsigned int rntiType = (utemp & 0x78) >> 3;
        //     std::cout << "RNTI Type: " << rntiType << std::endl;
            old_object = _replace_result_int(dci, "RNTI Type", rntiType);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(dci, "RNTI Type",
                                             ValueNameNrDciMessage_RntiType,
                                             ARRAY_SIZE(ValueNameNrDciMessage_RntiType, ValueName),
                                             "RNTI Type Unknown");
            unsigned int dciFormat = (utemp & 0x380) >> 7;
        //     std::cout << "DCI Format: " << dciFormat << std::endl;
            old_object = _replace_result_int(dci, "DCI Format", dciFormat);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(dci, "DCI Format",
                                             ValueNameNrDciMessage_DciFormat,
                                             ARRAY_SIZE(ValueNameNrDciMessage_DciFormat, ValueName),
                                             "DCI Format Unknown");
            unsigned int aggregationLevel = (utemp & 0x1C00) >> 10;
        //     std::cout << "Aggregation Level: " << aggregationLevel << std::endl;
            old_object = _replace_result_int(dci, "Aggregation Level", aggregationLevel);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(dci, "Aggregation Level",
                                             ValueNameNrDciMessage_AggregationLevel,
                                             ARRAY_SIZE(ValueNameNrDciMessage_AggregationLevel, ValueName),
                                             "Aggregation Level Unknown");
            unsigned int rawDciIncluded = (utemp & 0x2000) >> 13;
        //     std::cout << "Raw DCI Included: " << rawDciIncluded << std::endl;
            old_object = _replace_result_int(dci, "Raw DCI Included", rawDciIncluded);
            Py_DECREF(old_object);

            // decode Raw DCI if needed
            if (rawDciIncluded > 0) {
                // decode raw dci
                offset += _decode_by_fmt(NrDciMessage_RawDci_Fmt,
                        ARRAY_SIZE(NrDciMessage_RawDci_Fmt, Fmt),
                        b, offset, length, dci);
                // std::cout << "offset after decoding raw dci: " << offset << std::endl;
                unsigned int rawDci0 = _search_result_uint(dci, "Raw DCI Payload[0]");
                // std::cout << "Raw DCI Payload[0]: " << rawDci0 << std::endl;
                unsigned int rawDci1 = _search_result_uint(dci, "Raw DCI Payload[1]");
                // std::cout << "Raw DCI Payload[1]: " << rawDci1 << std::endl;
                unsigned int rawDci2 = _search_result_uint(dci, "Raw DCI Payload[2]");
                // std::cout << "Raw DCI Payload[2]: " << rawDci2 << std::endl;

                // populate Raw DCI[2], Raw DCI[1], Raw DCI[0] fields as strings that show the hexadecimal values of the fields
                std::stringstream stream2;
                stream2 << "0x" << std::setfill('0') << std::uppercase << std::setw(8) << std::hex << rawDci2;
                old_object = _replace_result_string(dci, "Raw DCI[2]", stream2.str());
                Py_DECREF(old_object);
                std::stringstream stream1;
                stream1 << "0x" << std::setfill('0') << std::uppercase << std::setw(8) << std::hex << rawDci1;
                old_object = _replace_result_string(dci, "Raw DCI[1]", stream1.str());
                Py_DECREF(old_object);
                std::stringstream stream0;
                stream0 << "0x" << std::setfill('0') << std::uppercase << std::setw(8) << std::hex << rawDci0;
                old_object = _replace_result_string(dci, "Raw DCI[0]", stream0.str());
                Py_DECREF(old_object);
            }

            // depending on the specific DCI Format, do specific decoding for that DCI Format
            if (0 == strcmp("UL_0_1", _search_result_bytestream(dci, "DCI Format"))) {
                offset += _decode_by_fmt(NrDciMessage_UL_Fmt,
                        ARRAY_SIZE(NrDciMessage_UL_Fmt, Fmt),
                        b, offset, length, dci);
                // std::cout << "offset after decoding UL_0_1: " << offset << std::endl;

                /*
                Let us have our prior assumptions that DCI Format and Carrier ID are each 3 bits long.
                Let bits 0, 1, 2 be DCI Format.
                Let bits 3, 4, 5 be Carrier ID. This is ok since Carrier ID always = 0, and our splitting keeps it at 0 still for the log in the video.
                Let bit 6 be NDI (New Data Indicator according to Internet, is also 1 bit long). This is ok since in the video it's 1, and our splitting gives 1.
                Let bits 7, 8, 9, 10, 11 be MCS (Modulation Coding Scheme according to Internet, is also 5 bits long). This is ok since the value of MCS in the video is 20, which is 10100, and our splitting matches this.
                Freq Hopping Flag (Frequency Hopping Flag, is 1 bit). Let bit 12 be Freq Hopping Flag.
                RV (Redundancy Version, is 2 bits). Let bits 13-14 be RV.
                HARQ ID (Hybrid Automatic Repeat Request, seems to be variable bit length). Let bits 15-18 be HARQ ID.
                PUSCH TPC (TPC command for scheduled PUSCH, is 2 bits). Let bits 19-20 be PUSCH TPC.
                UL SUL Ind (UL/SUL Indicator, either 0 or 1 bits according to the DCI format). Let bit 21 be UL SUL Ind.
                Symbol Alloc Index. Let bits 22-26 be Symbol Alloc Index (not sure about if it should be this many bits).
                BWP Ind (Bandwidth Part Indicator, can be 0, 1, or 2 bits for UL_0_1). Let bits 27-28 be BWP Ind.
                PTRS DMRS Association (can be 0 or 2 bits for UL_0_1). Let bits 29-30 be PTRS DMRS Association
                Beta Offset Ind (Beta Offset Indicator, can be 0 or 2 bits for UL_0_1). Let bit 31 be Beta Offset Ind (here I have it as 1 bit, not 2 apparently? This might be ok since most Indicators are 1 bit long?).
                */
                utemp = _search_result_uint(dci, "UL DCI Format");
                unsigned int dciFormat2 = utemp & 0x7;
                // std::cout << "UL DCI Format: " << dciFormat2 << std::endl;
                old_object = _replace_result_int(dci, "UL DCI Format", dciFormat);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(dci, "UL DCI Format",
                                                ValueNameNrDciMessage_DciFormat,
                                                ARRAY_SIZE(ValueNameNrDciMessage_DciFormat, ValueName),
                                                "UL DCI Format Unknown");
                unsigned int carrierId2 = (utemp & 0x38) >> 3;
                // std::cout << "Carrier ID: " << carrierId2 << std::endl;
                old_object = _replace_result_int(dci, "Carrier ID", carrierId2);
                Py_DECREF(old_object);
                unsigned int ndi = (utemp & 0x40) >> 6;
                // std::cout << "NDI: " << ndi << std::endl;
                old_object = _replace_result_int(dci, "NDI", ndi);
                Py_DECREF(old_object);
                unsigned int mcs = (utemp & 0xF80) >> 7;
                // std::cout << "mcs: " << mcs << std::endl;
                old_object = _replace_result_int(dci, "MCS", mcs);
                Py_DECREF(old_object);
                unsigned int freqHoppingFlag = (utemp & 0x1000) >> 12;
                // std::cout << "Freq Hopping Flag: " << freqHoppingFlag << std::endl;
                old_object = _replace_result_int(dci, "Freq Hopping Flag", freqHoppingFlag);
                Py_DECREF(old_object);
                unsigned int rv = (utemp & 0x6000) >> 13;
                // std::cout << "RV: " << rv << std::endl;
                old_object = _replace_result_int(dci, "RV", rv);
                Py_DECREF(old_object);
                unsigned int harqId = (utemp & 0x78000) >> 15;
                // std::cout << "HARQ ID: " << harqId << std::endl;
                old_object = _replace_result_int(dci, "HARQ ID", harqId);
                Py_DECREF(old_object);
                unsigned int puschTpc = (utemp & 0x180000) >> 19;
                // std::cout << "PUSCH TPC: " << puschTpc << std::endl;
                old_object = _replace_result_int(dci, "PUSCH TPC", puschTpc);
                Py_DECREF(old_object);
                unsigned int ulSulInd = (utemp & 0x200000) >> 21;
                // std::cout << "UL SUL Ind: " << ulSulInd << std::endl;
                old_object = _replace_result_int(dci, "UL SUL Ind", ulSulInd);
                Py_DECREF(old_object);
                unsigned int symbolAllocIndex = (utemp & 0x7C00000) >> 22;
                // std::cout << "Symbol Alloc Index: " << symbolAllocIndex << std::endl;
                old_object = _replace_result_int(dci, "Symbol Alloc Index", symbolAllocIndex);
                Py_DECREF(old_object);
                unsigned int bwpInd = (utemp & 0x18000000) >> 27;
                // std::cout << "BWP Ind: " << bwpInd << std::endl;
                old_object = _replace_result_int(dci, "BWP Ind", bwpInd);
                Py_DECREF(old_object);
                unsigned int ptrsDmrsAssociation = (utemp & 0x60000000) >> 29;
                // std::cout << "PTRS DMRS Association: " << ptrsDmrsAssociation << std::endl;
                old_object = _replace_result_int(dci, "PTRS DMRS Association", ptrsDmrsAssociation);
                Py_DECREF(old_object);
                unsigned int betaOffsetInd = (utemp & 0x80000000) >> 31;
                // std::cout << "Beta Offset Ind: " << betaOffsetInd << std::endl;
                old_object = _replace_result_int(dci, "Beta Offset Ind", betaOffsetInd);
                Py_DECREF(old_object);

                /*
                RB Assignment (Resource Block?? Assignment). In the video, it is equal to 150, which is 0x96, but more than 0x96 is highlighted in the video: the whole 00 byte is highlighted and the C byte is. Let us assume bits 0-17 is RB Assignment. This ok since it spans 3 bytes and goes into the C byte.
                UL SCH Ind (UL SCH Indicator, 1 bit). Let bit 18 be UL SCH Indicator. This is okay since the value in the video is 1, which matches our splitting.
                DAI 1 (1st Downlink Assignment Index, 1 or 2 bits). Let bits 19-20 be DAI 1. This is okay since the value in the video is 3, which matches our splitting.
                DAI 2 (2nd Downlink Assignment Index, 0 or 2 bits). Let bits 21-22 be DAI 2. This is okay since the Internet said it could be 2 bits.
                SRS Resource Indication (variable bits). Let bits 23-25 be SRS Resource Indication.
                Precoding Layer Info (0, 2, 3, 4, 5, 6 bits). Let Precoding Layer Info be bits 26-31. This is okay since the Internet said it could be 6 bits. The value in the video is 63, which is 0x3F, corresponding exactly to our splitting.
                */
                utemp = _search_result_uint(dci, "RB Assignment");
                unsigned int rbAssignment = utemp & 0x3FFFF;
                // std::cout << "RB Assignment: " << rbAssignment << std::endl;
                old_object = _replace_result_int(dci, "RB Assignment", rbAssignment);
                Py_DECREF(old_object);
                unsigned int ulSchInd = (utemp & 0x40000) >> 18;
                // std::cout << "UL SCH Ind: " << ulSchInd << std::endl;
                old_object = _replace_result_int(dci, "UL SCH Ind", ulSchInd);
                Py_DECREF(old_object);
                unsigned int dai1 = (utemp & 0x1F0000) >> 19;
                // std::cout << "DAI 1: " << dai1 << std::endl;
                old_object = _replace_result_int(dci, "DAI 1", dai1);
                Py_DECREF(old_object);
                unsigned int dai2 = (utemp & 0x600000) >> 21;
                // std::cout << "DAI 2: " << dai2 << std::endl;
                old_object = _replace_result_int(dci, "DAI 2", dai2);
                Py_DECREF(old_object);
                unsigned int srsResourceIndication = (utemp & 0x3800000) >> 23;
                // std::cout << "SRS Resource Indication: " << srsResourceIndication << std::endl;
                old_object = _replace_result_int(dci, "SRS Resource Indication", srsResourceIndication);
                Py_DECREF(old_object);
                unsigned int precodingLayerInfo = (utemp & 0xFC000000) >> 26;
                // std::cout << "Precoding Layer Info: " << precodingLayerInfo << std::endl;
                old_object = _replace_result_int(dci, "Precoding Layer Info", precodingLayerInfo);
                Py_DECREF(old_object);

                /*
                Antenna Ports (2, 3, 4, 5 bits). Let bits 0-4 be Antenna Port.
                DMRS Seq Init Flag (0 or 1 bit). Let bit 5 bit DMRS Seq Init Flag.
                SRS Request (2 bits). Let bits 6-7 SRS Request.
                */
                utemp = _search_result_uint(dci, "Antenna Ports");
                unsigned int antennaPorts = utemp & 0x1F;
                // std::cout << "Antenna Ports: " << antennaPorts << std::endl;
                old_object = _replace_result_int(dci, "Antenna Ports", antennaPorts);
                Py_DECREF(old_object);
                unsigned int dmrsSeqInitFlag = (utemp & 0x20) >> 5;
                // std::cout << "DMRS Seq Init Flag: " << dmrsSeqInitFlag << std::endl;
                old_object = _replace_result_int(dci, "DMRS Seq Init Flag", dmrsSeqInitFlag);
                Py_DECREF(old_object);
                unsigned int srsRequest = (utemp & 0xC0) >> 6;
                // std::cout << "SRS Request: " << srsRequest << std::endl;
                old_object = _replace_result_int(dci, "SRS Request", srsRequest);
                Py_DECREF(old_object);

                /* Note - we have 16 bits in total.
                CSI Request (0, 1, 2, 3, 4, 5, 6 bits). Let bits 0-5 be CSI Request. We have 16 - 6 = 10 bits left to assign.
                CBGTI (CBG Transmission Information, 0, 2, 4, 6, 8 bits). Let bits 6-13 be CBGTI. We have 10 - 8 = 2 bits left to assign.
                Reserved MCE Enable. Let bit 14 be Reserved MCE Enable. We have 2 - 1 bit left to assign.
                RA Type. Let bit 15 be RA Type. This is confirmed, since its value is 1 in the video, and the only 1 to appear in the whole 4 bytes is bit 15. We have 1 - 1 = 0 bits left to assign.
                */
                utemp = _search_result_uint(dci, "CSI Request");
                unsigned int csiRequest = utemp & 0x3F;
                // std::cout << "CSI Request: " << csiRequest << std::endl;
                old_object = _replace_result_int(dci, "CSI Request", csiRequest);
                Py_DECREF(old_object);
                unsigned int cbgti = (utemp & 0x3FC0) >> 6;
                // std::cout << "CBGTI: " << cbgti << std::endl;
                old_object = _replace_result_int(dci, "CBGTI", cbgti);
                Py_DECREF(old_object);
                unsigned int reservedMceEnable = (utemp & 0x4000) >> 14;
                // std::cout << "Reserved MCE Enable: " << reservedMceEnable << std::endl;
                old_object = _replace_result_int(dci, "Reserved MCE Enable", reservedMceEnable);
                Py_DECREF(old_object);
                unsigned int raType = (utemp & 0x8000) >> 15;
                // std::cout << "RA Type: " << raType << std::endl;
                old_object = _replace_result_int(dci, "RA Type", raType);
                Py_DECREF(old_object);

                /*
                Prune Reason MSB. Either NONE (0000 0000) or INVALID_RES_MCS (0000 1011) in the log files. Assume bits 0-5 are Prune Reason MSB (this might be wrong, might just be 0-4 or 0-6, I don't know how to check for this given our logs).
                Reserved. Let bits 6-7 be Reserved.
                */
                utemp = _search_result_uint(dci, "Prune Reason");
                unsigned int pruneReason = (utemp & 0x3F);
                // std::cout << "Prune Reason: " << pruneReason << std::endl;
                old_object = _replace_result_int(dci, "Prune Reason", pruneReason);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(dci, "Prune Reason",
                                                ValueNameNrDciMessage_PruneReasonMsb,
                                                ARRAY_SIZE(ValueNameNrDciMessage_PruneReasonMsb, ValueName),
                                                "Prune Reason Unknown");

                // populate Pruned Mask field as a string that shows the hexadecimal value of the field
                utemp = _search_result_uint(dci, "Pruned Mask");
                // std::cout << "Pruned Mask: " << utemp << std::endl;
                std::stringstream prunedMastStream;
                prunedMastStream << "0x" << std::setfill('0') << std::setw(8) << std::hex << utemp;
                old_object = _replace_result_string(dci, "Pruned Mask", prunedMastStream.str());
                Py_DECREF(old_object);
            }
            else if ((0 == strcmp("DL_1_0", _search_result_bytestream(dci, "DCI Format"))) || (0 == strcmp("DL_1_1", _search_result_bytestream(dci, "DCI Format")))) {
                offset += _decode_by_fmt(NrDciMessage_DL_Fmt,
                        ARRAY_SIZE(NrDciMessage_DL_Fmt, Fmt),
                        b, offset, length, dci);

                utemp = _search_result_uint(dci, "Bandwidth Part Indicator");
                unsigned int bandwidthPartIndicator = utemp & 0x3;
                // std::cout << "Bandwidth Part Indicator: " << bandwidthPartIndicator << std::endl;
                old_object = _replace_result_int(dci, "Bandwidth Part Indicator", bandwidthPartIndicator);
                Py_DECREF(old_object);
                unsigned int timeResourceAssignment = (utemp & 0x3C) >> 2;
                // std::cout << "Time Resource Assignment: " << timeResourceAssignment << std::endl;
                old_object = _replace_result_int(dci, "Time Resource Assignment", timeResourceAssignment);
                Py_DECREF(old_object);
                unsigned int tb1Mcs = (utemp & 0x7C0) >> 6;
                // std::cout << "TB 1 MCS: " << tb1Mcs << std::endl;
                old_object = _replace_result_int(dci, "TB 1 MCS", tb1Mcs);
                Py_DECREF(old_object);
                unsigned int tb1NewDataIndicator = (utemp & 0x800) >> 11;
                // std::cout << "TB 1 New Data Indicator: " << tb1NewDataIndicator << std::endl;
                old_object = _replace_result_int(dci, "TB 1 New Data Indicator", tb1NewDataIndicator);
                Py_DECREF(old_object);
                unsigned int dlAssignmentIndex = (utemp & 0xF000) >> 12;
                // std::cout << "DL Assignment Index: " << dlAssignmentIndex << std::endl;
                old_object = _replace_result_int(dci, "DL Assignment Index", dlAssignmentIndex);
                Py_DECREF(old_object);

                utemp = _search_result_uint(dci, "TPC Command For Sched PUCCH");
                unsigned int tpcCommandForSchedPucch = utemp & 0x3;
                // std::cout << "TPC Command for Sched PUCCH: " << tpcCommandForSchedPucch << std::endl;
                old_object = _replace_result_int(dci, "TPC Command For Sched PUCCH", tpcCommandForSchedPucch);
                Py_DECREF(old_object);
                unsigned int pucchResourceIndicator = (utemp & 0x1C) >> 2;
                // std::cout << "PUCCH Resource Indicator: " << pucchResourceIndicator << std::endl;
                old_object = _replace_result_int(dci, "PUCCH Resource Indicator", pucchResourceIndicator);
                Py_DECREF(old_object);
                unsigned int pdschHarqFeedbackTiming = (utemp & 0xE0) >> 5;
                // std::cout << "PDSCH Harq Feedback Timing: " << pdschHarqFeedbackTiming << std::endl;
                old_object = _replace_result_int(dci, "PDSCH Harq Feedback Timing", pdschHarqFeedbackTiming);
                Py_DECREF(old_object);

                utemp = _search_result_uint(dci, "CBG Flushing Out Info");
                unsigned int cbgFlushingOutInfo = utemp & 0x1;
                // std::cout << "CBG Flushing Out Info: " << cbgFlushingOutInfo << std::endl;
                old_object = _replace_result_int(dci, "CBG Flushing Out Info", cbgFlushingOutInfo);
                Py_DECREF(old_object);
                unsigned int transmissionConfigInd = (utemp & 0xE) >> 1;
                // std::cout << "Transmission Config Ind: " << transmissionConfigInd << std::endl;
                old_object = _replace_result_int(dci, "Transmission Config Ind", transmissionConfigInd);
                Py_DECREF(old_object);
                unsigned int srsRequest2 = (utemp & 0x30) >> 4;
                // std::cout << "SRS Request: " << srsRequest2 << std::endl;
                old_object = _replace_result_int(dci, "SRS Request", srsRequest2);
                Py_DECREF(old_object);
                unsigned int carrierId3 = (utemp & 0x1C0) >> 6;
                // std::cout << "Carrier ID: " << carrierId3 << std::endl;
                old_object = _replace_result_int(dci, "Carrier ID", carrierId3);
                Py_DECREF(old_object);
                unsigned int harqId2 = (utemp & 0x3C00) >> 10;
                // std::cout << "HARQ ID: " << harqId2 << std::endl;
                old_object = _replace_result_int(dci, "HARQ ID", harqId2);
                Py_DECREF(old_object);
            }
            
            // make "DCI Info[index]" string
            char index_dci_string[32];
            sprintf(index_dci_string, "[%d]", dciIndex);
            char dciInfoString[64] = "DCI Info";
            strcat(dciInfoString, index_dci_string);

            // build DCI Info[index] object
            PyObject *dciInfoIndex = Py_BuildValue("(sOs)", dciInfoString, dci, "dict"); // might need "list" or "dict" instead of ""?

            // append DCI Info[index] object to list of DCIs
            PyList_Append(dciList, dciInfoIndex);
            Py_DECREF(dciInfoIndex);
            Py_DECREF(dci);

                // PyList_Append(dciList, dci);
                // Py_DECREF(dci);
        }

        // build DCI Info object - made up of the DCI Info[index] objects
        PyObject *dciInfo = Py_BuildValue("(sOs)", "DCI Info", dciList, "list");

        // append DCI list to each record
        PyList_Append(record, dciInfo);
        Py_DECREF(dciInfo);

        // make "Records[index]" string
        char index_record_string[32];
        sprintf(index_record_string, "[%d]", recordIndex);
        char recordsString[64] = "Records";
        strcat(recordsString, index_record_string);

        // build Records[index] object - made up of the record object
        PyObject *recordIndexObj = Py_BuildValue("(sOs)", recordsString, record, "dict"); // might need "list" or "dict" instead of ""?

        // append Records[index] object to list of records
        PyList_Append(recordList, recordIndexObj);
        Py_DECREF(recordIndexObj);
        Py_DECREF(record);

        // PyList_Append(recordList, record);
        // Py_DECREF(record);
    }

    // build Records object - made up of the recordList object
    PyObject *records = Py_BuildValue("(sOs)", "Records", recordList, "list"); // might need "list" or "dict" instead of ""?

    // append Records object to result
    PyList_Append(result, records);
    Py_DECREF(records);
    Py_DECREF(recordList);

//     PyList_Append(result, recordList);
//     Py_DECREF(recordList);

    // return offset - start to return however many bytes we have left remaining
    return offset - start;
}