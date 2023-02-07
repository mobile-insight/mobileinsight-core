/*
 * NR5G PDCP DL Data Pdu
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

// 4 bytes
const Fmt NrPdcpDlDataPdu_Version_Fmt[] = {
        {UINT, "Version", 4}
};

// 0 bytes
const Fmt NrPdcpDlDataPdu_MajorMinorVersion_Fmt[] = {
        {PLACEHOLDER, "Minor", 0},
        {PLACEHOLDER, "Major", 0},
        {PLACEHOLDER, "Major.Minor Version", 0},
        {PLACEHOLDER, "{id: 2003420 }", 0}
};

// 6 bytes
const Fmt NrPdcpDlDataPdu_MetaRbNumAndReserved_Fmt[] = {
        {UINT, "Number of Meta", 2},
        {UINT, "Number of RB", 2},
        {SKIP, NULL, 2}
};

// 16 bytes
const Fmt NrPdcpDlDataPdu_PdcpState_Fmt[] = {
        {UINT, "RB Cfg Index", 4},
        {UINT, "RX Deliv", 4},
        {UINT, "RX Next", 4},
        {UINT, "Next Count", 4}
};

// 4 bytes
const Fmt NrPdcpDlDataPdu_SystemTime_Fmt[] = {
        {UINT, "Slot", 1},
        {SKIP, NULL, 1},
        {UINT, "Frame", 2},
};

// 10 bytes
const Fmt NrPdcpDlDataPdu_RxTimetick_Fmt[] = {
        {UINT, "RX Timetick Raw", 8},
        {PLACEHOLDER, "RX Timetick", 0},
        {SKIP, NULL, 2}, // skip 0x0104 bits, seem to not be used in decoding at all
};

// 1 byte
const Fmt NrPdcpDlDataPdu_KeyIndex_Fmt[] = {
        {UINT, "Key Index", 1},
};

// 1 byte
const Fmt NrPdcpDlDataPdu_RlcPath_Fmt[] = {
        {UINT, "RLC Path", 1},
};

const ValueName ValueNameNrPdcpDlDataPdu_RlcPath[] = {
        {0, "NR"}
};

// 1 byte
const Fmt NrPdcpDlDataPdu_RouteStatus_Fmt[] = {
        {UINT, "Route Status", 1},
};

const ValueName ValueNameNrPdcpDlDataPdu_RouteStatus[] = {
        {1, "DELIV_DIRECT"}
};

// 2 bytes
const Fmt NrPdcpDlDataPdu_IpPacketHeaders_Fmt[] = {
        {UINT, "IP Packet Header[0]", 1}, // need to represent in hex
        {UINT, "IP Packet Header[1]", 1}, // need to represent in hex
};

// 4 bytes
const Fmt NrPdcpDlDataPdu_StartCount_Fmt[] = {
        {UINT, "Start Count", 4},
};

// 4 bytes
const Fmt NrPdcpDlDataPdu_EndCount_Fmt[] = {
        {UINT, "End Count", 4},
};

// 4 bytes
const Fmt NrPdcpDlDataPdu_RlcEndSn_Fmt[] = {
        {UINT, "RLC end SN", 4},
};

// 8 bytes
const Fmt NrPdcpDlDataPdu_IpPacketNums_Fmt[] = {
        {UINT, "Number IP Pkts", 4},
        {UINT, "Number IP bytes", 4}
};

static int _decode_nr_pdcp_dl_data_pdu (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;

    // ---------- decode MajorMinorVersion structure -------------
    offset += _decode_by_fmt(NrPdcpDlDataPdu_Version_Fmt,
                            ARRAY_SIZE(NrPdcpDlDataPdu_Version_Fmt, Fmt),
                            b, offset, length, result); // this function doesn't increase result's reference count
    unsigned int versionNumber = _search_result_uint(result, "Version");

    PyObject *majorMinorVersionList = PyList_New(0); // reference count of majorMinorVersionList = 1

    offset += _decode_by_fmt(NrPdcpDlDataPdu_MajorMinorVersion_Fmt,
                            ARRAY_SIZE(NrPdcpDlDataPdu_MajorMinorVersion_Fmt, Fmt),
                            b, offset, length, majorMinorVersionList);

    // get values of minor and major versions, populate Major.Minor Version and {id: 2003420 } fields with those values
    unsigned int minVersion = versionNumber & 0x00FF;
    unsigned int majVersion = (versionNumber & 0xFF00) >> 8;

    PyObject *old_object = _replace_result_int(majorMinorVersionList, "Minor", minVersion); // this function doesn't increase majorMinorVersionList's reference count
    Py_DECREF(old_object);
    old_object = _replace_result_int(majorMinorVersionList, "Major", majVersion);
    Py_DECREF(old_object);
    old_object = _replace_result_int(majorMinorVersionList, "Major.Minor Version", majVersion);
    Py_DECREF(old_object);
    old_object = _replace_result_int(majorMinorVersionList, "{id: 2003420 }", minVersion);
    Py_DECREF(old_object);

    PyObject *majorMinorVersion = Py_BuildValue("(sOs)", "MajorMinorVersion", majorMinorVersionList, "dict"); // THIS FUNCTION INCREASES majorMinorVersionList's REFERENCE COUNT! Also, majorMinorVersions's reference count = 1
    Py_DECREF(majorMinorVersionList);

    PyList_Append(result, majorMinorVersion); // increases majorMinorVersion's reference count by 1
    Py_DECREF(majorMinorVersion);
    // ---------- end decode MajorMinorVersion structure -------------

    // ---------- decode Versions structure -------------
    // ---------- decode Version 5 structure -------------
    PyObject *versionList = PyList_New(0);

    offset += _decode_by_fmt(NrPdcpDlDataPdu_MetaRbNumAndReserved_Fmt,
                            ARRAY_SIZE(NrPdcpDlDataPdu_MetaRbNumAndReserved_Fmt, Fmt),
                            b, offset, length, versionList);

    unsigned int numMeta = _search_result_uint(versionList, "Number of Meta"); // doesn't increase reference count of versionList
    unsigned int numRb = _search_result_uint(versionList, "Number of RB");

    // ---------- decode PDCP State structure -------------
    PyObject *pdcpStatesList = PyList_New(0);

    for (unsigned int i = 0; i < numRb; i++)
    {
        // ---------- decode PDCP State[] structure -------------
        PyObject *pdcpStateList = PyList_New(0);
        offset += _decode_by_fmt(NrPdcpDlDataPdu_PdcpState_Fmt,
                        ARRAY_SIZE(NrPdcpDlDataPdu_PdcpState_Fmt, Fmt),
                        b, offset, length, pdcpStateList);

        std::string pdcpStateString = "PDCP State[" + std::to_string(i) + "]";
        PyObject *pdcpState = Py_BuildValue("(sOs)", pdcpStateString.c_str(), pdcpStateList, "dict");
        Py_DECREF(pdcpStateList);

        PyList_Append(pdcpStatesList, pdcpState);
        Py_DECREF(pdcpState);
        // ---------- end decode PDCP State[] structure -------------
    }
    PyObject *pdcpStates = Py_BuildValue("(sOs)", "PDCP State", pdcpStatesList, "list");
    Py_DECREF(pdcpStatesList);
    // ---------- end decode PDCP State structure -------------
//     PyObject_Print(pdcpStates, stdout, 0); // testing
//     std::cout << "ref cnt: " << versionList->ob_refcnt << std::endl; // testing

    PyList_Append(versionList, pdcpStates);
    Py_DECREF(pdcpStates);

    // ---------- decode Meta Log Buffer structure -------------
    PyObject *metaLogBufferList = PyList_New(0);
    for (unsigned int i = 0; i < numMeta; i++)
    {
        // ---------- decode Meta Log Buffer[] structure -------------
        PyObject *metaLogBufferElementList = PyList_New(0);

        // ---------- decode SystemTime structure -------------
        PyObject *systemTimeList = PyList_New(0);

        offset += _decode_by_fmt(NrPdcpDlDataPdu_SystemTime_Fmt,
                        ARRAY_SIZE(NrPdcpDlDataPdu_SystemTime_Fmt, Fmt),
                        b, offset, length, systemTimeList);

        unsigned int frame = _search_result_uint(systemTimeList, "Frame");
        frame = frame & 0x0FFF; // lower 12 bits of the 16 are bits for the Frame field
        old_object = _replace_result_int(systemTimeList, "Frame", frame);
        Py_DECREF(old_object);

        PyObject *systemTime = Py_BuildValue("(sOs)", "System Time", systemTimeList, "dict");
        Py_DECREF(systemTimeList);

        PyList_Append(metaLogBufferElementList, systemTime);
        Py_DECREF(systemTime);
        // ---------- end decode SystemTime structure -------------

        // ---------- decode Rx Timetick structure -------------
        offset += _decode_by_fmt(NrPdcpDlDataPdu_RxTimetick_Fmt,
                        ARRAY_SIZE(NrPdcpDlDataPdu_RxTimetick_Fmt, Fmt),
                        b, offset, length, metaLogBufferElementList);

        unsigned long rxTimetickRaw = _search_result_ulongint(metaLogBufferElementList, "RX Timetick Raw"); // don't think this function increases reference count
        // populate RX Timetick Raw fields as string that show the hexadecimal values of the field
        std::stringstream stream1;
        stream1 << "0x" << std::setfill('0') << std::uppercase << std::setw(16) << std::hex << rxTimetickRaw;
        old_object = _replace_result_string(metaLogBufferElementList, "RX Timetick Raw", stream1.str()); // this function might increase reference count of metaLogBufferElementList
        Py_DECREF(old_object);
        
        // todo - calculate Rx Timetick (float) - need to build a Python float object, currently it is PLACEHOLDER (a Python integer object), but I need to create a new case for a float in log_packet_helper.h...
        // ---------- end decode Rx Timetick structure -------------

        // ---------- decode Key Index structure -------------
        offset += _decode_by_fmt(NrPdcpDlDataPdu_KeyIndex_Fmt,
                        ARRAY_SIZE(NrPdcpDlDataPdu_KeyIndex_Fmt, Fmt),
                        b, offset, length, metaLogBufferElementList);
        // ---------- end decode Key Index structure -------------

        // ---------- decode RLC Path structure -------------
        offset += _decode_by_fmt(NrPdcpDlDataPdu_RlcPath_Fmt,
                        ARRAY_SIZE(NrPdcpDlDataPdu_RlcPath_Fmt, Fmt),
                        b, offset, length, metaLogBufferElementList);
        (void) _map_result_field_to_name(metaLogBufferElementList, "RLC Path",
                                                ValueNameNrPdcpDlDataPdu_RlcPath,
                                                ARRAY_SIZE(ValueNameNrPdcpDlDataPdu_RlcPath, ValueName),
                                                "RLC Path Unknown"); // don't think this function increases reference count of metaLogBufferElementList
        // ---------- end decode RLC Path structure -------------

        // ---------- decode Route Status structure -------------
        offset += _decode_by_fmt(NrPdcpDlDataPdu_RouteStatus_Fmt,
                        ARRAY_SIZE(NrPdcpDlDataPdu_RouteStatus_Fmt, Fmt),
                        b, offset, length, metaLogBufferElementList);
        (void) _map_result_field_to_name(metaLogBufferElementList, "Route Status",
                                                ValueNameNrPdcpDlDataPdu_RouteStatus,
                                                ARRAY_SIZE(ValueNameNrPdcpDlDataPdu_RouteStatus, ValueName),
                                                "Route Status Unknown");
        // ---------- end decode Route Status structure -------------

        // ---------- decode IP Packet Header[] structure -------------
        offset += _decode_by_fmt(NrPdcpDlDataPdu_IpPacketHeaders_Fmt,
                        ARRAY_SIZE(NrPdcpDlDataPdu_IpPacketHeaders_Fmt, Fmt),
                        b, offset, length, metaLogBufferElementList);
        
        unsigned int ipPacketHeader0 = _search_result_uint(metaLogBufferElementList, "IP Packet Header[0]");
        unsigned int ipPacketHeader1 = _search_result_uint(metaLogBufferElementList, "IP Packet Header[1]");
        
        // populate IP Packet Header[0], IP Packet Header[1] fields as strings that show the hexadecimal values of the fields
        std::stringstream stream2;
        stream2 << "0x" << std::setfill('0') << std::uppercase << std::setw(2) << std::hex << ipPacketHeader0;
        old_object = _replace_result_string(metaLogBufferElementList, "IP Packet Header[0]", stream2.str()); // this function might increase reference count of metaLogBufferElementList
        Py_DECREF(old_object);

        std::stringstream stream3;
        stream3 << "0x" << std::setfill('0') << std::uppercase << std::setw(2) << std::hex << ipPacketHeader1;
        old_object = _replace_result_string(metaLogBufferElementList, "IP Packet Header[1]", stream3.str());
        Py_DECREF(old_object);
        // ---------- end decode IP Packet Header[] structure -------------

        // ---------- decode Start Count structure -------------
        offset += _decode_by_fmt(NrPdcpDlDataPdu_StartCount_Fmt,
                        ARRAY_SIZE(NrPdcpDlDataPdu_StartCount_Fmt, Fmt),
                        b, offset, length, metaLogBufferElementList);
        // ---------- end decode Start Count structure -------------

        // ---------- decode End Count structure -------------
        offset += _decode_by_fmt(NrPdcpDlDataPdu_EndCount_Fmt,
                        ARRAY_SIZE(NrPdcpDlDataPdu_EndCount_Fmt, Fmt),
                        b, offset, length, metaLogBufferElementList);
        // ---------- end decode Start Count structure -------------

        // ---------- decode RLC end SN structure -------------
        offset += _decode_by_fmt(NrPdcpDlDataPdu_RlcEndSn_Fmt,
                        ARRAY_SIZE(NrPdcpDlDataPdu_RlcEndSn_Fmt, Fmt),
                        b, offset, length, metaLogBufferElementList);
        // ---------- end decode RLC end SN structure -------------

        // ---------- decode IP Packet Numbers structure -------------
        offset += _decode_by_fmt(NrPdcpDlDataPdu_IpPacketNums_Fmt,
                        ARRAY_SIZE(NrPdcpDlDataPdu_IpPacketNums_Fmt, Fmt),
                        b, offset, length, metaLogBufferElementList);
        // ---------- end decode IP Packet Numbers structure -------------

        // ---------- end decode Meta Log Buffer[] structure ------------- 
        std::string metaLogBufferStr = "Meta Log Buffer[" + std::to_string(i) + "]";
        PyObject *metaLogBuffers = Py_BuildValue("(sOs)", metaLogBufferStr.c_str(), metaLogBufferElementList, "dict");
        Py_DECREF(metaLogBufferElementList);

        PyList_Append(metaLogBufferList, metaLogBuffers);
        Py_DECREF(metaLogBuffers);
    }

    PyObject *metaLogBuffer = Py_BuildValue("(sOs)", "Meta Log Buffer", metaLogBufferList, "list");
    Py_DECREF(metaLogBufferList);
    // ---------- end decode Meta Log Buffer structure -------------

    PyList_Append(versionList, metaLogBuffer);
    Py_DECREF(metaLogBuffer);

    std::string versionStr = "Version " + std::to_string(minVersion);
    PyObject *version = Py_BuildValue("(sOs)", versionStr.c_str(), versionList, "dict");
//     PyList_Append(version, versionList);
    Py_DECREF(versionList);

// //     PyObject *versions = Py_BuildValue("(sOs)", "Versions", version, "dict");
//     PyObject *versions = Py_BuildValue("(sOs)", "Versions", version, "list");
//     PyList_Append(versions, version);
//     Py_DECREF(version);

//     PyList_Append(result, versions);
//     Py_DECREF(versions);

    PyList_Append(result, version);
    Py_DECREF(version);

    // return offset - start to return however many bytes we have left remaining
    return offset - start;
}




















