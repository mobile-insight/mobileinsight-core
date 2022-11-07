/*
 * NR_DCI_Message
 */
#include <iostream>
#include <sstream>

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

// 4 bytes
const Fmt NrDciMessage_MacVersion_Fmt[] = {
        {UINT_BIG_ENDIAN, "Minor Version", 2}, // 16 bits
        {UINT_BIG_ENDIAN, "Major Version", 2}, // 16 bits
        {PLACEHOLDER, "Major.Minor Version", 0}, // 0 bits
        {PLACEHOLDER, "{id: 2026230 }", 0} // 0 bits
}

// 12 bytes
const Fmt NrDciMessage_Version_Fmt[] = {
        {UINT_BIG_ENDIAN, "Sleep", 1}, // 8 bits
        {UINT_BIG_ENDIAN, "Beam Change", 1}, // 8 bits
        {UINT_BIG_ENDIAN, "Signal Change", 1}, // 8 bits
        {UINT_BIG_ENDIAN, "DL Dynamic Cfg Change", 1}, // 8 bits
        {UINT_BIG_ENDIAN, "DL Config", 1}, // 8 bits
        {UINT_BIG_ENDIAN, "UL Config", 1}, // 4 bits, 1 byte = 8 bits cover UL Config and ML1 State Change, do bit masking on the 1 byte to get UL Config
        {PLACEHOLDER, "ML1 State Change", 0}, // 4 bits, do bit masking on the 1 byte to get ML1 State Change
        {SKIP, NULL, 2}, // 16 bits for Reserved
        {UINT_BIG_ENDIAN, "Log Fields Change BMask",  2} // 16 bits
        {SKIP, NULL, 1}, // 8 bits for Reserved
        {UINT_BIG_ENDIAN, "Num Records", 1} // 8 bits
}

// 8 bytes
const Fmt NrDciMessage_Record_Fmt[] = {
        {UINT_BIG_ENDIAN, "Slot", 1}, // 8 bits
        {UINT_BIG_ENDIAN, "Num", 1}, // 8 bits, 1 byte = 8 bits cover Num and Reserved0 (which we don't need), do bit masking on the 8 bits to get Num
        {UINT_BIG_ENDIAN, "Frame" 2}, // 8 bits (1 byte) + last 2?? bits of the 2nd byte. 2 bytes cover Frame and Reserved (which we don't need). Do bit masking on the 2 bytes to get the Frame.
        {UINT_BIG_ENDIAN, "Num DCI", 1}, // 8 bits
        {SKIP, NULL, 3} // 24 bits for reserved[0], reserved[1], reserved[2], which we don't need
}

const ValueName ValueNameNrDciMessage_Num[] = {
        {0,  "15kHz"},
};

// 4 bytes
const Fmt NrDciMessage_DciInfo_Fmt[] = {
        {UINT_BIG_ENDIAN, "Carrier ID", 4}, // 2?? bits, 4 bytes = 32 bits cover Carrier ID, RNTI Type, DCI Format, Aggregation Level, Raw DCI Included, and Reserve (not needed). do bit masking to get Carrier ID
        {PLACEHOLDER, "RNTI Type", 0}, // 6?? bits, do bit masking on the 4 bytes to get RNTI Type
        {PLACEHOLDER, "DCI Format", 0}, // 8?? bits, do bit masking on the 4 bytes to get DCI Format
        {PLACEHOLDER, "Aggregation Level", 0}, // 8 bits, do bit masking on the 4 bytes to get Aggregation Level
        {PLACEHOLDER, "Raw DCI Included", 0} // 4?? bits, do bit masking on the 4 bytes to get Raw DCI Included
}

const ValueName ValueNameNrDciMessage_RntiType[] = {
        {0, "C_RNTI"},
        {8, "RA_RNTI"}
};

const ValueName ValueNameNrDciMessage_DciFormat[] = {
        {0, "DL_1_1"},
        {1, "UL_0_1"},
        {2, "DL_1_0"}
};

const ValueName ValueNameNrDciMessage_AggregationLevel[] = {
        {0, "LEVEL_1"},
        {1, "LEVEL_2"},
        {3, "LEVEL_8"}
}

const ValueName ValueNameNrDciMessage_PruneReasonMsb[] = {
        {0, "NONE"},
        {11, "INVALID_RES_MCS"}
}

// 12 bytes
const Fmt NrDciMessage_RawDci_Fmt[] = {
        {UINT_BIG_ENDIAN, "Raw DCI Payload[0]", 4}, // 32 bits
        {UINT_BIG_ENDIAN, "Raw DCI Payload[1]", 4}, // 32 bits
        {UINT_BIG_ENDIAN, "Raw DCI Payload[2]", 4}, // 32 bits
        {PLACEHOLDER, "Raw DCI[2]", 0},
        {PLACEHOLDER, "Raw DCI[1]", 0},
        {PLACEHOLDER, "Raw DCI[0]", 0}
}

// 16 bytes
const Fmt NrDciMessage_UL_0_1_Fmt[] = {
        {UINT_BIG_ENDIAN, "UL DCI Format", 4}, // 2?? bits, 4 bytes = 32 bits cover DCI Format, Carrier ID, NDI, MCS, Freq Hopping Flag, RV, HARQ ID, PUSCH TPC, UL SUL Ind, PTRS DMRS Association, Beta Offset Ind. Do bit masking on the 2 bytes to get DCI Format
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
        {UINT_BIG_ENDIAN, "RB Assignment", 4}, // ?? bits, 4 bytes = 32 bits cover RB Assignment, UL SCH Ind, DAI 1, DAI 2, SRS Resource Indication, Precoding Layer Info. Do bit masking on the 4 bytes to get RB Assignment
        {PLACEHOLDER, "UL SCH Ind", 0}, // ?? bits, do bit masking on the 4 bytes to get UL SCH Ind
        {PLACEHOLDER, "DAI 1", 0}, // ?? bits, do bit masking on the 4 bytes to get DAI 1
        {PLACEHOLDER, "DAI 2", 0}, // ?? bits, do bit masking on the 4 bytes to get DAI 2
        {PLACEHOLDER, "SRS Resource Indication", 0}, // ?? bits, do bit masking on the 4 bytes to get SRS Resource Indication
        {PLACEHOLDER, "Precoding Layer Info", 0}, // ?? bits, do bit masking on the 4 bytes to get Precoding Layer Info
        {UINT_BIG_ENDIAN, "Antenna Ports", 1}, // ? bits, 1 byte = 8 bits cover Antenna Ports, DMRS Seq Init Flag, SRS Request. Do bit masking on the 1 byte to get Antenna Ports
        {PLACEHOLDER, "DMRS Seq Init Flag", 0}, // ? bits, do bit masking on the 1 byte to get DMRS Seq Init Flag
        {PLACEHOLDER, "SRS Request", 0}, // ? bits, do bit masking on the 1 byte to get SRS Request
        {PLACEHOLDER, "CSI Request", 2}, // ? bits, 2 bytes = 16 bits cover CSI Request, CBGTI, Reserved MCE Enable, RA Type. do bit masking on the 2 bytes to get CSI Request
        {PLACEHOLDER, "CBGTI", 0}, // ? bits, do bit masking on the 2 bytes to get CBGTI
        {PLACEHOLDER, "Reserved MCE Enable", 0}, // ? bits, do bit masking on the 2 bytes to get Reserved MCE Enable
        {PLACEHOLDER, "RA Type", 0}, // ? bits, do bit masking on the 2 bytes to get RA Type
        {UINT_BIG_ENDIAN, "Prune Reason", 1}, // ? bits, 1 byte = 8 bits covers Prune Reason MSB and Reserved (not needed). Do bit masking on the 1 byte to get Prune Reason
        {UINT_BIG_ENDIAN, "Pruned Mask", 4} // 32 bits
}

// todo - don't know how many bytes each field is (no video)
/*
const Fmt NrDciMessage_DL_Fmt[] = {
        {PLACEHOLDER, "Bandwidth Part Indicator", 0},
        {PLACEHOLDER, "Time Resource Assignment", 0},
        {PLACEHOLDER, "TB 1 MCS", 0},
        {PLACEHOLDER, "TB 1 New Data Indicator", 0},
        {PLACEHOLDER, "DL Assignment Index", 0},
        {PLACEHOLDER, "TPC Command For Sched PUCCH", 0},
        {PLACEHOLDER, "PUCCH Resource Indicator", 0},
        {PLACEHOLDER, "PDSCH Harq Feedback Timing", 0},
        {PLACEHOLDER, "CBG Transmission Info", 0},
        {PLACEHOLDER, "CBG Flushing Out Info", 0},
        {PLACEHOLDER, "Transmission Config Ind", 0},
        {PLACEHOLDER, "SRS Request", 0},
        {PLACEHOLDER, "Carrier ID", 0},
        {PLACEHOLDER, "HARQ ID", 0}
}
*/

// assuming before we call this function, we have already decoded NrDciMessage_MacVersion_Fmt, which will be passed to this function in the result variable
static int _decode_nr_DCI (const char *b,
        int offset, size_t length, PyObject *result) {            
    int start = offset;

    // get values of minor and major versions, populate Major.Minor Version and {id: 2026230 } fields with those values
    unsigned int minVersion = _search_result_uint(result, "Minor Version");
    unsigned int majVersion = _search_result_uint(result, "Major Version");
    old_object = _replace_result_int(result, "Major.Minor Version", majVersion);
    Py_DECREF(old_object);
    old_object = _replace_result_int(result, "{id: 2026230 }", minVersion);
    Py_DECREF(old_object);

    // decode NrDciMessage_Version_Fmt
    offset += _decode_by_fmt(NrDciMessage_Version_Fmt,
                ARRAY_SIZE(NrDciMessage_Version_Fmt, Fmt),
                b, offset, length, result);
    
    // populate UL Config and ML1 State Change fields with their values by bit masking
    unsigned int utemp = _search_result_uint(result, "UL Config");
    unsigned int ulConfig = utemp & 0xF; // last 4 bits is ulConfig
    unsigned int ml1StateChange = (utemp >> 4) & 0xF; // shift 4 bits to the right to get rid of ulConfig bits, last 4 bits of that result is ml1StateChange
    old_object = _replace_result_int(result, "UL Config", ulConfig);
    Py_DECREF(old_object);
    old_object = _replace_result_int(result, "ML1 State Change", ml1StateChange);
    Py_DECREF(old_object);

    // build a list of records (there may be > 1 records, so we need to loop through each individual record to build the list)
    PyObject *recordList = PyList_New(0);
    unsigned int numRecords = _search_result_uint(result, "Num Records");
    for (unsigned int recordIndex = 0; recordIndex < numRecords; recordIndex++) {
        PyObject *record = PyList_New(0);

        // decode individual record
        offset += _decode_by_fmt(NrDciMessage_Record_Fmt,
                ARRAY_SIZE(NrDciMessage_Record_Fmt, Fmt),
                b, offset, length, record);
        
        // puplate Num field by bit masking
        utemp = _search_result_uint(record, "Num");
        unsigned int num = utemp & 0xF; // last 4 bits is num
        old_object = _replace_result_int(record, "Num", num);
        Py_DECREF(old_object);

        // for each record, build a list of DCIs (there may be > 1 DCIs in one record, so we need to loop through each individual DCI to build the list)
        PyObject *dciList = PyList_New(0);
        unsigned int numDcis = _search_result_uint(record, "Num DCI");
        for (unsigned int dciIndex = 0; dciIndex < numDcis; dciIndex++) {
            PyObject *dci = PyList_New(0);

            // decode individual DCI
            offset += _decode_by_fmt(NrDciMessage_DciInfo_Fmt,
                ARRAY_SIZE(NrDciMessage_DciInfo_Fmt, Fmt),
                b, offset, length, dci);
        
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
            old_object = _replace_result_int(dci, "Carrier ID", carrierId);
            Py_DECREF(old_object);
            unsigned int rntiType = (utemp & 0x78) >> 3;
            old_object = _replace_result_int(dci, "RNTI Type", rntiType);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(dci, "RNTI Type",
                                             ValueNameNrDciMessage_RntiType,
                                             ARRAY_SIZE(ValueNameNrDciMessage_RntiType, ValueName),
                                             "RNTI Type Unknown");
            unsigned int dciFormat = (utemp & 0x380) >> 7;
            old_object = _replace_result_int(dci, "DCI Format", dciFormat);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(dci, "DCI Format",
                                             ValueNameNrDciMessage_DciFormat,
                                             ARRAY_SIZE(ValueNameNrDciMessage_DciFormat, ValueName),
                                             "DCI Format Unknown");
            unsigned int aggregationLevel = (utemp & 0x1C00) >> 10;
            old_object = _replace_result_int(dci, "Aggregation Level", aggregationLevel);
            Py_DECREF(old_object);
            (void) _map_result_field_to_name(dci, "Aggregation Level",
                                             ValueNameNrDciMessage_AggregationLevel,
                                             ARRAY_SIZE(ValueNameNrDciMessage_AggregationLevel, ValueName),
                                             "Aggregation Level Unknown");
            unsigned int rawDciIncluded = (utemp & 0x2000) >> 13;
            old_object = _replace_result_int(dci, "Raw DCI Included", rawDciIncluded);
            Py_DECREF(old_object);

            // decode Raw DCI if needed
            if (rawDciIncluded > 0) {
                // decode raw dci
                offset += _decode_by_fmt(NrDciMessage_RawDci_Fmt,
                        ARRAY_SIZE(NrDciMessage_RawDci_Fmt, Fmt),
                        b, offset, length, dci);
                unsigned int rawDci0 = _search_result_uint(dci, "Raw DCI Payload[0]");
                unsigned int rawDci1 = _search_result_uint(dci, "Raw DCI Payload[1]");
                unsigned int rawDci2 = _search_result_uint(dci, "Raw DCI Payload[2]");

                // populate Raw DCI[2], Raw DCI[1], Raw DCI[0] fields as strings that show the hexadecimal values of the fields
                std::stringstream stream2;
                stream2 << "0x" << std::setfill('0') << std::setw(8) << std::hex << rawDci2;
                old_object = _replace_result_string(dci, "Raw DCI[2]", stream2.str());
                Py_DECREF(old_object);
                std::stringstream stream1;
                stream1 << "0x" << std::setfill('0') << std::setw(8) << std::hex << rawDci1;
                old_object = _replace_result_string(dci, "Raw DCI[1]", stream1.str());
                Py_DECREF(old_object);
                std::stringstream stream0;
                stream0 << "0x" << std::setfill('0') << std::setw(8) << std::hex << rawDci0;
                old_object = _replace_result_string(dci, "Raw DCI[0]", stream0.str());
                Py_DECREF(old_object);
            }

            // depending on the specific DCI Format, do specific decoding for that DCI Format
            // todo decode more formats other than UL_0_1
            if (0 == strcmp("UL_0_1", _search_result_bytestream(dci, "DCI Format"))) {
                offset += _decode_by_fmt(NrDciMessage_UL_0_1_Fmt,
                        ARRAY_SIZE(NrDciMessage_UL_0_1_Fmt, Fmt),
                        b, offset, length, dci);

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
                old_object = _replace_result_int(dci, "UL DCI Format", dciFormat);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(dci, "UL DCI Format",
                                                ValueNameNrDciMessage_DciFormat,
                                                ARRAY_SIZE(ValueNameNrDciMessage_DciFormat, ValueName),
                                                "UL DCI Format Unknown");
                unsigned int carrierId2 = (utemp & 0x38) >> 3;
                old_object = _replace_result_int(dci, "Carrier ID", carrierId2);
                Py_DECREF(old_object);
                unsigned int ndi = (utemp & 0x40) >> 6;
                old_object = _replace_result_int(dci, "NDI", ndi);
                Py_DECREF(old_object);
                unsigned int mcs = (utemp & 0xF80) >> 7;
                old_object = _replace_result_int(dci, "MCS", mcs);
                Py_DECREF(old_object);
                unsigned int freqHoppingFlag = (utemp & 0x1000) >> 12;
                old_object = _replace_result_int(dci, "Freq Hopping Flag", freqHoppingFlag);
                Py_DECREF(old_object);
                unsigned int rv = (utemp & 0x6000) >> 13;
                old_object = _replace_result_int(dci, "RV", rv);
                Py_DECREF(old_object);
                unsigned int harqId = (utemp & 0x78000) >> 15;
                old_object = _replace_result_int(dci, "HARQ ID", harqId);
                Py_DECREF(old_object);
                unsigned int puschTpc = (utemp & 0x180000) >> 19;
                old_object = _replace_result_int(dci, "PUSCH TPC", puschTpc);
                Py_DECREF(old_object);
                unsigned int ulSulInd = (utemp & 0x200000) >> 21;
                old_object = _replace_result_int(dci, "UL SUL Ind", ulSulInd);
                Py_DECREF(old_object);
                unsigned int symbolAllocIndex = (utemp & 0x7C00000) >> 22;
                old_object = _replace_result_int(dci, "Symbol Alloc Index", symbolAllocIndex);
                Py_DECREF(old_object);
                unsigned int bwpInd = (utemp & 0x18000000) >> 27;
                old_object = _replace_result_int(dci, "BWP Ind", bwpInd);
                Py_DECREF(old_object);
                unsigned int ptrsDmrsAssociation = (utemp & 0x60000000) >> 29;
                old_object = _replace_result_int(dci, "PTRS DMRS Association", ptrsDmrsAssociation);
                Py_DECREF(old_object);
                unsigned int betaOffsetInd = (utemp & 0x80000000) >> 31;
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
                old_object = _replace_result_int(dci, "RB Assignment", rbAssignment);
                Py_DECREF(old_object);
                unsigned int ulSchInd = (utemp & 0x40000) >> 18;
                old_object = _replace_result_int(dci, "UL SCH Ind", ulSchInd);
                Py_DECREF(old_object);
                unsigned int dai1 = (utemp & 0x1F0000) >> 19;
                old_object = _replace_result_int(dci, "DAI 1", dai1);
                Py_DECREF(old_object);
                unsigned int dai2 = (utemp & 0x600000) >> 21;
                old_object = _replace_result_int(dci, "DAI 2", dai2);
                Py_DECREF(old_object);
                unsigned int srsResourceIndication = (utemp & 0x3800000) >> 23;
                old_object = _replace_result_int(dci, "SRS Resource Indication", srsResourceIndication);
                Py_DECREF(old_object);
                unsigned int precodingLayerInfo = (utemp & 0xFC000000) >> 26;
                old_object = _replace_result_int(dci, "Precoding Layer Info", precodingLayerInfo);
                Py_DECREF(old_object);

                /*
                Antenna Ports (2, 3, 4, 5 bits). Let bits 0-4 be Antenna Port.
                DMRS Seq Init Flag (0 or 1 bit). Let bit 5 bit DMRS Seq Init Flag.
                SRS Request (2 bits). Let bits 6-7 SRS Request.
                */
                utemp = _search_result_uint(dci, "Antenna Ports");
                unsigned int antennaPorts = utemp & 0x1F;
                old_object = _replace_result_int(dci, "Antenna Ports", antennaPorts);
                Py_DECREF(old_object);
                unsigned int dmrsSeqInitFlag = (utemp & 0x20) >> 5;
                old_object = _replace_result_int(dci, "DMRS Seq Init Flag", dmrsSeqInitFlag);
                Py_DECREF(old_object);
                unsigned int srsRequest = (utemp & 0xC0) >> 6;
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
                old_object = _replace_result_int(dci, "CSI Request", csiRequest);
                Py_DECREF(old_object);
                unsigned int cbgti = (utemp & 0x3FC0) >> 6;
                old_object = _replace_result_int(dci, "CBGTI", cbgti);
                Py_DECREF(old_object);
                unsigned int reservedMceEnable = (utemp & 0x4000) >> 14;
                old_object = _replace_result_int(dci, "Reserved MCE Enable", reservedMceEnable);
                Py_DECREF(old_object);
                unsigned int raType = (utemp & 0x8000) >> 15;
                old_object = _replace_result_int(dci, "RA Type", raType);
                Py_DECREF(old_object);

                /*
                Prune Reason MSB. Either NONE (0000 0000) or INVALID_RES_MCS (0000 1011) in the log files. Assume bits 0-5 are Prune Reason MSB (this might be wrong, might just be 0-4 or 0-6, I don't know how to check for this given our logs).
                Reserved. Let bits 6-7 be Reserved.
                */
                utemp = _search_result_uint(dci, "Prune Reason");
                unsigned int pruneReason = (utemp & 0x3F);
                old_object = _replace_result_int(dci, "Prune Reason", pruneReason);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(dci, "Prune Reason",
                                                ValueNameNrDciMessage_PruneReasonMsb,
                                                ARRAY_SIZE(ValueNameNrDciMessage_PruneReasonMsb, ValueName),
                                                "Prune Reason Unknown");

                // populate Pruned Mask field as a string that shows the hexadecimal value of the field
                utemp = _search_result_uint(dci, "Pruned Mask");
                std::stringstream prunedMastStream;
                prunedMastStream << "0x" << std::setfill('0') << std::setw(8) << std::hex << utemp;
                old_object = _replace_result_string(dci, "Pruned Mask", prunedMastStream.str());
                Py_DECREF(old_object);
            }
        }
    }
}