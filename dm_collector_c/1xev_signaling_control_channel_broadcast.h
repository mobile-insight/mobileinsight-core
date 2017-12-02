/*
 * 1xev_signaling_control_channel_broadcast.h
 */

#include <Python.h>
#include <datetime.h>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <cstring>

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt _1xEVSignalingFmt [] = {
    {UINT, "Band", 1},
    {UINT, "Channel Number", 2},
    {UINT, "Pilot PN", 2},
    {UINT, "HSTR", 2},
    {SKIP, NULL, 4}, // Unknown
    {UINT, "Protocol Type", 1},
    {UINT, "Message ID", 1},
};

const Fmt _1xEVSignaling_SectorParameters [] = {
    {UINT_BIG_ENDIAN, "Country Code", 2}, // 12 bits
    // 4 bits remaining
    {UINT_BIG_ENDIAN, "Subnet ID", 4},
    {UINT_BIG_ENDIAN, "Subnet ID[2]", 4},
    {UINT_BIG_ENDIAN, "Subnet ID[3]", 4},
    {UINT_BIG_ENDIAN, "Sector ID", 4}, // 4 bits for subnet ID, 24 bits for sector id
    // 4 bits remaining
    {UINT_BIG_ENDIAN, "Subnet Mask", 1}, // 4+4 bits
    // 4 bits remaining
    {UINT_BIG_ENDIAN, "Sector Signature", 2}, // 4 + 12 bits
    // 4 bits remaining
    {UINT_BIG_ENDIAN, "Latitude", 4},    // 4 + 18 bits
    // 14 bits remaining
    {UINT_BIG_ENDIAN, "Longitude", 2},  // 14 + 9 bits
    // 7 bits remaining
};

static int _decode_1xev_signaling_control_channel_broadcast (const char *b,
        int offset, size_t length, PyObject *result) {
    int iMessageID = _search_result_int(result, "Message ID");
    int iProtocolType = _search_result_int(result, "Protocol Type");

    if (iProtocolType == 15 && iMessageID == 1) {
        // Sector Parameters

        // Only support for System Parameters Msg
        offset += _decode_by_fmt(_1xEVSignaling_SectorParameters,
                ARRAY_SIZE(_1xEVSignaling_SectorParameters, Fmt),
                b, offset, length, result);

        PyObject *old_object;
        unsigned int utemp = _search_result_int(result, "Country Code");
        int iCountryCode = ((utemp >> 12) & 15) * 100;
        iCountryCode += ((utemp >> 8) & 15) * 10;
        iCountryCode += ((utemp >> 4) & 15) * 1;
        old_object = _replace_result_int(result, "Country Code", iCountryCode);
        Py_DECREF(old_object);

        char hex[10] = {};
        std::string strSubnetID = "0x";
        sprintf(hex, "%02x", utemp & 15);
        strSubnetID += hex;
        utemp = _search_result_uint(result, "Subnet ID");
        for (int k = 0; k < 4; k++) {
            sprintf(hex, "%02x", (utemp >> ((3-k) * 8)) & 255);
            strSubnetID += hex;
        }
        utemp = _search_result_uint(result, "Subnet ID[2]");
        for (int k = 0; k < 4; k++) {
            sprintf(hex, "%02x", (utemp >> ((3-k) * 8)) & 255);
            strSubnetID += hex;
        }
        utemp = _search_result_uint(result, "Subnet ID[3]");
        for (int k = 0; k < 4; k++) {
            sprintf(hex, "%02x", (utemp >> ((3-k) * 8)) & 255);
            strSubnetID += hex;
        }
        utemp = _search_result_uint(result, "Sector ID");
        sprintf(hex, "%02x", (utemp >> 28) & 15);
        strSubnetID += hex;
        PyObject *pystr = Py_BuildValue("s", strSubnetID.c_str());
        old_object = _replace_result(result, "Subnet ID", pystr);
        Py_DECREF(old_object);
        Py_DECREF(pystr);

        int iSectorID = (utemp >> 4) & 16777215;
        old_object = _replace_result_int(result, "Sector ID", iSectorID);
        Py_DECREF(old_object);

        int iSubnetMask = (utemp & 15) * 16;
        utemp = _search_result_int(result, "Subnet Mask");
        iSubnetMask += (utemp >> 4) & 15;
        old_object = _replace_result_int(result, "Subnet Mask", iSubnetMask);
        Py_DECREF(old_object);

        int iSectorSignature = (utemp & 15) * 4096;
        utemp = _search_result_int(result, "Sector Signature");
        iSectorSignature += (utemp >> 4) & 4095;
        old_object = _replace_result_int(result, "Sector Signature", iSectorSignature);
        Py_DECREF(old_object);

        int iLatitude = (utemp & 15) * 262144;
        utemp = _search_result_uint(result, "Latitude");
        iLatitude += (utemp >> 14) & 262143;
        if (((iLatitude >> 21) & 1) == 1) {
            iLatitude = iLatitude - 4194304;
        }
        float fLatitude = iLatitude * 1.0 / 14400;
        PyObject *pyfloat = Py_BuildValue("f", fLatitude);
        old_object = _replace_result(result, "Latitude", pyfloat);
        Py_DECREF(old_object);
        Py_DECREF(pyfloat);

        int iLongitude = (utemp & 16383) * 512;
        utemp = _search_result_uint(result, "Longitude");
        iLongitude += (utemp >> 7) & 511;
        if (((iLongitude >> 22) & 1) == 1) {
            iLongitude = iLongitude - 8388608;
        }
        float fLongitude = iLongitude * 1.0 / 14400;
        pyfloat = Py_BuildValue("f", fLongitude);
        old_object = _replace_result(result, "Longitude", pyfloat);
        Py_DECREF(old_object);
        Py_DECREF(pyfloat);

        return offset;
    } else {
        return offset;
    }

}
