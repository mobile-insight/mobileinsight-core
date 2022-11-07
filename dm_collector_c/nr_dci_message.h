/*
 * NR_DCI_Message
 */

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

// 12 bytes
const Fmt NrDciMessage_RawDci_Fmt[] = {
        {UINT_BIG_ENDIAN, "Raw DCI[0]", 4}, // 32 bits
        {UINT_BIG_ENDIAN, "Raw DCI[1]", 4}, // 32 bits
        {UINT_BIG_ENDIAN, "Raw DCI[2]", 4} // 32 bits
}

// 16 bytes
const Fmt NrDciMessage_UL_0_1_Fmt[] = {
        {UINT_BIG_ENDIAN, "DCI Format", 4}, // 2?? bits, 4 bytes = 32 bits cover DCI Format, Carrier ID, NDI, MCS, Freq Hopping Flag, RV, HARQ ID, PUSCH TPC, UL SUL Ind, PTRS DMRS Association, Beta Offset Ind. Do bit masking on the 2 bytes to get DCI Format
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
        {UINT_BIG_ENDIAN, "Antenna Ports", 2}, // ? bits, 2 byte = 16 bits cover Antenna Ports, DMRS Seq Init Flag, SRS Request, CSI Request, CBGTI, Reserved MCE Enable, RA Type. Do bit masking on the 2 bytes to get Antenna Ports
        {PLACEHOLDER, "DMRS Seq Init Flag", 0}, // ? bits, do bit masking on the 2 bytes to get DMRS Seq Init Flag
        {PLACEHOLDER, "SRS Request", 0}, // ? bits, do bit masking on the 2 bytes to get SRS Request
        {PLACEHOLDER, "CSI Request", 0}, // ? bits, do bit masking on the 2 bytes to get CSI Request
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
        unsigned int numDcis = _search_result_uint(result, "Num DCI");
        for (unsigned int dciIndex = 0; dciIndex < numDcis; dciIndex++) {
            PyObject *dci = PyList_New(0);

            // decode individual DCI
            offset += _decode_by_fmt(NrDciMessage_DciInfo_Fmt,
                ARRAY_SIZE(NrDciMessage_DciInfo_Fmt, Fmt),
                b, offset, length, dci);
        
            /*
            todo:

            figure out how many bits each field is in general
            populate Carrier ID, RNTI Type, DCI Format, Aggregation Level, Raw DCI Included fields by doing bit masking
            look at value of Raw DCI Included field, if it's 1 then decode NrDciMessage_RawDci_Fmt
            look at value of RNTI Type and DCI Format - use those values to figure out which format (DL_1_0, DL_1_1, UL_0_1, UL_1_0, etc) to decode
            */
        }
    }
}