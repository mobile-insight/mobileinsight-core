/* log_packet.cpp
 * Author: Jiayao Li, Jie Zhao
 * Implements log packet message decoding.
 */

#include <Python.h>
#include <datetime.h>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <cstring>

#include "1xev_connected_state_search_info.h"
#include "1xev_connection_attempt.h"
#include "1xev_connection_release.h"
#include "1xev_rx_partial_multirlp_packet.h"
#include "1xev_signaling_control_channel_broadcast.h"
#include "1xevdo_multi_carrier_pilot_sets.h"
#include "consts.h"
#include "cdma_paging_channel_msg.h"
#include "gsm_rr_cell_information.h"
#include "gsm_rr_cell_reselection_meas.h"
#include "gsm_rr_cell_reselection_param.h"
#include "gsm_rr_signaling_message.h"
#include "gsm_surround_cell_ba_list.h"
#include "gsm_dsds_rr_cell_information.h"
#include "gsm_dsds_rr_cell_reselection_param.h"
#include "gsm_dsds_rr_signaling_message.h"
#include "log_packet.h"
#include "log_packet_helper.h"
#include "lte_pdcp_dl_cipher_data_pdu.h"
#include "lte_pdcp_ul_cipher_data_pdu.h"
#include "lte_pdsch_stat_indication.h"
#include "lte_phy_bplmn_cell_confirm.h"
#include "lte_phy_bplmn_cell_request.h"
#include "lte_phy_cdrx_events_info.h"
#include "lte_phy_idle_neighbor_cell_meas.h"
#include "lte_phy_connected_neighbor_cell_meas.h"
#include "lte_phy_pdcch_decoding_result.h"
#include "lte_phy_pdsch_decoding_result.h"
#include "lte_phy_pucch_csf.h"
#include "lte_phy_pucch_tx_report.h"
#include "lte_phy_pusch_csf.h"
#include "lte_phy_pusch_tx_report.h"
#include "lte_phy_rlm_report.h"
#include "lte_phy_serving_cell_com_loop.h"
#include "lte_phy_system_scan_results.h"
#include "srch_tng_1x_searcher_dump.h"
#include "wcdma_rrc_states.h"
#include "wcdma_search_cell_reselection_rank.h"

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

// #ifdef __ANDROID__
// #include <android/log.h>
// #define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "TAG", __VA_ARGS__);
// #endif

//Yuanjie: In-phone version cannot locate std::to_string
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm;
        stm << n ;
        return stm.str();
    }
}

/*
 * The decoding result is represented using a Python list object (called result
 * list in the source code).
 * All elements in a result list is a 3-element tuple: (name, value, type).
 * "name" is a C string, and "value" is a reference to a Python object.
 * "type" is a C string that can be one of the following values:
 * - "" (empty string): an ordinary field. The "value" could be a Python
 *      integer, str, etc.
 * - "list": The "value" is another result list, and each element is of the same
 *      "type".
 * - "dict": The "value" is another result list, and each element can be of
 *      different "types".
 * - "raw_msg/TYPE": The value is a binary string that contains raw messages.
 */

// TODO: split this .cpp to multiple files.

static int
_decode_wcdma_signaling_messages(const char *b, int offset, size_t length,
                                    PyObject *result) {
    (void)length;
    int start = offset;
    int ch_num = _search_result_int(result, "Channel Type");
    const char *ch_name = search_name(WcdmaSignalingMsgChannelType,
            ARRAY_SIZE(WcdmaSignalingMsgChannelType, ValueName),
            ch_num);

    if (ch_name == NULL) {  // not found
        printf("(MI)Unknown WCDMA Signalling Messages Channel Type: 0x%x\n", ch_num);
        return 0;
    }

    int pdu_length = _search_result_int(result, "Message Length");

    if (0 == strncmp(ch_name, "RRC_COMPLETE_SIB", 16)) {
        offset += _decode_by_fmt(WcdmaSignalingMessagesFmtExtraSIBType,
                ARRAY_SIZE(WcdmaSignalingMessagesFmtExtraSIBType, Fmt),
                b, offset, length, result);
        pdu_length--;
        int ch_num = _search_result_int(result, "Extra SIB Type");
        ch_name = search_name(ValueNameWcdmaExtraSIBType,
                ARRAY_SIZE(ValueNameWcdmaExtraSIBType, ValueName),
                ch_num);
        if (ch_name == NULL) {  // not found
            printf("(MI)Unknown WCDMA Signalling Messages RRC Complete SIB Type: 0x%x\n", ch_num);
            return 0;
        }
    } else if (0 == strncmp(ch_name, "Extension SIB", 13)) {
        offset += _decode_by_fmt(WcdmaSignalingMessagesFmtExtensionSIBType,
                ARRAY_SIZE(WcdmaSignalingMessagesFmtExtensionSIBType, Fmt),
                b, offset, length, result);
        pdu_length--;
        int ch_num = _search_result_int(result, "Extension SIB Type");
        ch_name = search_name(ValueNameWcdmaExtensionSIBType,
                ARRAY_SIZE(ValueNameWcdmaExtensionSIBType, ValueName),
                ch_num);
        if (ch_name == NULL) {  // not found
            printf("(MI)Unknown WCDMA Signalling Messages RRC Extension SIB Type: 0x%x\n", ch_num);
            return 0;
        }
    }

    std::string type_str = "raw_msg/";
    type_str += ch_name;
    PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, pdu_length, type_str.c_str());
    PyList_Append(result, t);
    Py_DECREF(t);
    return offset - start;
}

static int
_decode_umts_nas_gmm_state(const char *b, int offset, size_t length,
                            PyObject *result) {
    (void)b;
    (void)offset;
    (void)length;
    (void) _map_result_field_to_name(result,
                                        "GMM State",
                                        UmtsNasGmmState_GmmState,
                                        ARRAY_SIZE(UmtsNasGmmState_GmmState, ValueName),
                                        "(MI)Unknown");
    (void) _map_result_field_to_name(result,
                                        "GMM Substate",
                                        UmtsNasGmmState_GmmSubstate,
                                        ARRAY_SIZE(UmtsNasGmmState_GmmSubstate, ValueName),
                                        "(MI)Unknown");
    (void) _map_result_field_to_name(result,
                                        "GMM Update Status",
                                        UmtsNasGmmState_GmmUpdateStatus,
                                        ARRAY_SIZE(UmtsNasGmmState_GmmUpdateStatus, ValueName),
                                        "(MI)Unknown");
    return 0;
}

static int
_decode_umts_nas_mm_state(const char *b, int offset, size_t length,
                            PyObject *result) {
    (void)b;
    (void)offset;
    (void)length;
    (void) _map_result_field_to_name(result,
                                        "MM State",
                                        UmtsNasMmState_MmState,
                                        ARRAY_SIZE(UmtsNasMmState_MmState, ValueName),
                                        "(MI)Unknown");
    (void) _map_result_field_to_name(result,
                                        "MM Substate",
                                        UmtsNasMmState_MmSubstate,
                                        ARRAY_SIZE(UmtsNasMmState_MmSubstate, ValueName),
                                        "(MI)Unknown");
    (void) _map_result_field_to_name(result,
                                        "MM Update Status",
                                        UmtsNasMmState_MmUpdateStatus,
                                        ARRAY_SIZE(UmtsNasMmState_MmUpdateStatus, ValueName),
                                        "(MI)Unknown");
    return 0;
}

static int
_decode_umts_nas_ota(const char *b, int offset, size_t length,
                        PyObject *result) {
    (void)length;
    //int start = offset;
    (void) _map_result_field_to_name(result,
                                    "Message Direction",
                                    UmtsNasOtaFmt_MessageDirection,
                                    ARRAY_SIZE(UmtsNasOtaFmt_MessageDirection, ValueName),
                                    "(MI)Unknown");

    int pdu_length = _search_result_int(result, "Message Length");
    PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, pdu_length,
                                "raw_msg/NAS");
    PyList_Append(result, t);
    Py_DECREF(t);
    return pdu_length;
}

static int
_decode_lte_rrc_ota(const char *b, int offset, size_t length,
                    PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Pkt Version");

    switch (pkt_ver) {
    case 2:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v2,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v2, Fmt),
                                    b, offset, length, result);
        break;
    case 4:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v4,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v4, Fmt),
                                    b, offset, length, result);
        break;
    case 7:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v7,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v7, Fmt),
                                    b, offset, length, result);
        break;
    case 8:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v8,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v8, Fmt),
                                    b, offset, length, result);
        break;
    case 9:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v9,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v9, Fmt),
                                    b, offset, length, result);
        break;
    case 12:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v12,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v12, Fmt),
                                    b, offset, length, result);
        break;
    case 13:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v13,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v13, Fmt),
                                    b, offset, length, result);
        break;
    case 15:
        offset += _decode_by_fmt(LteRrcOtaPacketFmt_v15,
                                    ARRAY_SIZE(LteRrcOtaPacketFmt_v15, Fmt),
                                    b, offset, length, result);
        break;
    default:
        printf("(MI)Unknown LTE RRC OTA packet version: %d\n", pkt_ver);
        return 0;
    }

    if (pkt_ver >= 15) {
        int pdu_number = _search_result_int(result, "PDU Number");
        int pdu_length = _search_result_int(result, "Msg Length");
        const char *type_name = search_name(LteRrcOtaPduType_v15,
                                            ARRAY_SIZE(LteRrcOtaPduType_v15, ValueName),
                                            pdu_number);

        if (type_name == NULL) {    // not found
            printf("(MI)Unknown LTE RRC PDU Type: 0x%x\n", pdu_number);
            return 0;
        } else {
            std::string type_str = "raw_msg/";
            type_str += type_name;
            PyObject *t = Py_BuildValue("(ss#s)",
                                        "Msg", b + offset, pdu_length, type_str.c_str());
            PyList_Append(result, t);
            Py_DECREF(t);
            return (offset - start) + pdu_length;
        }
    } else {
        int pdu_number = _search_result_int(result, "PDU Number");
        int pdu_length = _search_result_int(result, "Msg Length");
        if (pdu_number > 8) {   // Hack. This is not confirmed.
            pdu_number -= 7;
        }
        const char *type_name = search_name(LteRrcOtaPduType,
                                            ARRAY_SIZE(LteRrcOtaPduType, ValueName),
                                            pdu_number);

        if (type_name == NULL) {    // not found
            printf("(MI)Unknown LTE RRC PDU Type: 0x%x\n", pdu_number);
            return 0;
        } else {
            std::string type_str = "raw_msg/";
            type_str += type_name;
            PyObject *t = Py_BuildValue("(ss#s)",
                                        "Msg", b + offset, pdu_length, type_str.c_str());
            PyList_Append(result, t);
            Py_DECREF(t);
            return (offset - start) + pdu_length;
        }
    }
}

static int
_decode_lte_rrc_mib(const char *b, int offset, size_t length,
                    PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 1:
        offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt_v1,
                                    ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_v1, Fmt),
                                    b, offset, length, result);
        break;
    case 2:
        offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt_v2,
                                    ARRAY_SIZE(LteRrcMibMessageLogPacketFmt_v2, Fmt),
                                    b, offset, length, result);
        break;
    default:
        printf("(MI)Unknown LTE RRC MIB version: 0x%x\n", pkt_ver);
        return 0;
    }

    return offset - start;
}

static int
_decode_lte_rrc_serv_cell_info(const char *b, int offset, size_t length,
                                PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 2:
        offset += _decode_by_fmt(LteRrcServCellInfoLogPacketFmt_v2,
                                    ARRAY_SIZE(LteRrcServCellInfoLogPacketFmt_v2, Fmt),
                                    b, offset, length, result);
        break;
    case 3:
        offset += _decode_by_fmt(LteRrcServCellInfoLogPacketFmt_v3,
                                    ARRAY_SIZE(LteRrcServCellInfoLogPacketFmt_v3, Fmt),
                                    b, offset, length, result);
        break;
    default:
        printf("(MI)Unknown LTE RRC Serving Cell Info packet version: %d\n", pkt_ver);
        return 0;
    }

    return offset - start;
}

static size_t
_decode_lte_nas_plain(const char *b, int offset, size_t length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Pkt Version");

    switch (pkt_ver) {
    case 1:
        offset += _decode_by_fmt(LteNasPlainFmt_v1,
                                    ARRAY_SIZE(LteNasPlainFmt_v1, Fmt),
                                    b, offset, length, result);
        break;
    default:
        printf("(MI)Unknown LTE NAS version: 0x%x\n", pkt_ver);
        return 0;
    }

    size_t pdu_length = length - offset;
    PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, pdu_length,
                                "raw_msg/LTE-NAS_EPS_PLAIN");
    PyList_Append(result, t);
    Py_DECREF(t);
    return length - start;
}

static int
_decode_lte_nas_esm_state(const char *b, int offset, size_t length,
                            PyObject *result) {

    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    switch (pkt_ver) {

        case 1:{
            offset += _decode_by_fmt(LteNasEsmStateFmt_v1,
                                        ARRAY_SIZE(LteNasEsmStateFmt_v1, Fmt),
                                        b, offset, length, result);

            break;

        }

        default:
            printf("(MI)Unknown LTE NAS ESM State version: 0x%x\n", pkt_ver);
            return 0;

    }

    return offset - start;
}

static int
_decode_lte_nas_emm_state(const char *b, int offset, size_t length,
                            PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 2:
        {
            offset += _decode_by_fmt(LteNasEmmStateFmt_v2,
                                        ARRAY_SIZE(LteNasEmmStateFmt_v2, Fmt),
                                        b, offset, length, result);
            int emm_state_id = _map_result_field_to_name(
                                        result,
                                        "EMM State",
                                        LteNasEmmState_v2_EmmState,
                                        ARRAY_SIZE(LteNasEmmState_v2_EmmState, ValueName),
                                        "(MI)Unknown");
            // Replace the value of "EMM Substate"
            const ValueName *table = NULL;
            int table_size = 0;
            switch (emm_state_id) {
            case 1: // EMM_DEREGISTERED
                table = LteNasEmmState_v2_EmmSubstate_Deregistered;
                table_size = ARRAY_SIZE(LteNasEmmState_v2_EmmSubstate_Deregistered, ValueName);
                break;

            case 2: // EMM_REGISTERED_INITIATED
                table = LteNasEmmState_v2_EmmSubstate_Registered_Initiated;
                table_size = ARRAY_SIZE(LteNasEmmState_v2_EmmSubstate_Registered_Initiated, ValueName);
                break;

            case 3: // EMM_REGISTERED
            case 4: // EMM_TRACKING_AREA_UPDATING_INITIATED
            case 5: // EMM_SERVICE_REQUEST_INITIATED
                table = LteNasEmmState_v2_EmmSubstate_Registered;
                table_size = ARRAY_SIZE(LteNasEmmState_v2_EmmSubstate_Registered, ValueName);
                break;

            case 0: // EMM_NULL
            case 6: // EMM_DEREGISTERED_INITIATED
            default:
                // No Substate
                break;
            }
            if (table != NULL && table_size > 0) {
                (void) _map_result_field_to_name(result, "EMM Substate", table, table_size, "(MI)Unknown");
            } else {
                PyObject *pystr = Py_BuildValue("s", "Undefined");
                PyObject *old_object = _replace_result(result, "EMM Substate", pystr);
                Py_DECREF(old_object);
                Py_DECREF(pystr);
            }
            break;
        } // End of v2

    default:
        printf("(MI)Unknown LTE NAS EMM State version: 0x%x\n", pkt_ver);
        return 0;
    }

    return offset - start;
}

static int
_decode_lte_phy_pdsch_demapper_config(const char *b, int offset, size_t length,
                                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 23:
        {
            offset += _decode_by_fmt(LtePhyPdschDemapperConfigFmt_v23,
                                        ARRAY_SIZE(LtePhyPdschDemapperConfigFmt_v23, Fmt),
                                        b, offset, length, result);

            const unsigned int SFN_RSHIFT = 5, SFN_MASK = (1 << 10) - 1;
            const unsigned int SUBFRAME_RSHIFT = 1, SUBFRAME_MASK = (1 << 4) - 1;
            int tmp = _search_result_int(result, "System Frame Number");
            int sfn = (tmp >> SFN_RSHIFT) & SFN_MASK;
            int subframe = (tmp >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
            int serv_cell = _search_result_int(result, "Serving Cell ID");
            serv_cell += (tmp & 0x1) << 8;

            PyObject *old_object = _replace_result_int(result, "Serving Cell ID", serv_cell);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "System Frame Number", sfn);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Subframe Number", subframe);
            Py_DECREF(old_object);

            // # antennas
            tmp = _search_result_int(result, "Number of Tx Antennas(M)");
            int iRNTIType = tmp & 15;
            int iSpatialRank = (tmp >> 14) & 3;
            tmp = (tmp >> 8) & 0x0f;
            int M = tmp & 0x3;
            M = (M != 3? (1 << M): -1);
            int N = (tmp >> 2) & 0x1;
            N = (1 << N);

            old_object = _replace_result_int(result, "PDSCH RNTI Type", iRNTIType);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "PDSCH RNTI Type",
                    ValueNameRNTIType,
                    ARRAY_SIZE(ValueNameRNTIType, ValueName),
                    "(MI)Unknown");

            old_object = _replace_result_int(result, "Number of Tx Antennas(M)", M);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Number of Rx Antennas(N)", N);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Spatial Rank", iSpatialRank);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "Spatial Rank",
                    ValueNameRankIndex,
                    ARRAY_SIZE(ValueNameRankIndex, ValueName),
                    "(MI)Unknown");

            tmp = _search_result_int(result, "Frequency Selective PMI");
            int iFrequencySelectivePMI = (tmp >> 1) & 3;
            int iPMIIndex = (tmp >> 4) & 15;
            tmp = _search_result_int(result, "Transmission Scheme");
            int iTransmissionScheme = tmp & 15;

            old_object = _replace_result_int(result, "Frequency Selective PMI",
                    iFrequencySelectivePMI);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "Frequency Selective PMI",
                    ValueNameFrequencySelectivePMI,
                    ARRAY_SIZE(ValueNameFrequencySelectivePMI, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "PMI Index", iPMIIndex);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Transmission Scheme",
                    iTransmissionScheme);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "Transmission Scheme",
                    ValueNameTransmissionScheme,
                    ARRAY_SIZE(ValueNameTransmissionScheme, ValueName),
                    "(MI)Unknown");

            // modulation & ratio
            tmp = _search_result_int(result, "MCS 0");
            int mod_stream0 = (tmp >> 1) & 0x3;
            float ratio = float((tmp >> 3) & 4095) / 256.0;
            tmp = _search_result_int(result, "MCS 1");
            int mod_stream1 = (tmp >> 1) & 0x3;
            int carrier_index = (tmp >> 9) & 0xf;

            old_object = _replace_result_int(result, "MCS 0", mod_stream0);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "MCS 0",
                                            LtePhyPdschDemapperConfig_v23_Modulation,
                                            ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName),
                                            "(MI)Unknown");

            PyObject *pyfloat = Py_BuildValue("f", ratio);
            //FIXME: Traffic to Pilot Ratio is incorrect. Refine its calculation
            old_object = _replace_result(result, "Traffic to Pilot Ratio", pyfloat);
            Py_DECREF(old_object);
            Py_DECREF(pyfloat);

            old_object = _replace_result_int(result, "MCS 1", mod_stream1);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "MCS 1",
                                            LtePhyPdschDemapperConfig_v23_Modulation,
                                            ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName),
                                            "(MI)Unknown");

            // carrier index
            old_object = _replace_result_int(result, "Carrier Index", carrier_index);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "Carrier Index",
                                            LtePhyPdschDemapperConfig_v23_Carrier_Index,
                                            ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Carrier_Index, ValueName),
                                            "(MI)Unknown");
            break;
        }
    case 103:
    	{
            offset += _decode_by_fmt(LtePhyPdschDemapperConfigFmt_v103,
                                        ARRAY_SIZE(LtePhyPdschDemapperConfigFmt_v103, Fmt),
                                        b, offset, length, result);

            const unsigned int SFN_RSHIFT = 5, SFN_MASK = (1 << 10) - 1;
            const unsigned int SUBFRAME_RSHIFT = 1, SUBFRAME_MASK = (1 << 4) - 1;
            int tmp = _search_result_int(result, "System Frame Number");
            int sfn = (tmp >> SFN_RSHIFT) & SFN_MASK;
            int subframe = (tmp >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
            int serv_cell = _search_result_int(result, "Serving Cell ID");
            serv_cell += (tmp & 0x1) << 8;

            PyObject *old_object = _replace_result_int(result, "Serving Cell ID", serv_cell);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "System Frame Number", sfn);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Subframe Number", subframe);
            Py_DECREF(old_object);

            // # antennas
            tmp = _search_result_int(result, "Number of Tx Antennas(M)");
            int iRNTIType = tmp & 15;
            int iSpatialRank = (tmp >> 14) & 3;
            tmp = (tmp >> 8) & 0x0f;
            int M = tmp & 0x3;
            M = (M != 3? (1 << M): -1);
            int N = (tmp >> 2) & 0x1;
            N = (1 << N);

            old_object = _replace_result_int(result, "PDSCH RNTI Type", iRNTIType);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "PDSCH RNTI Type",
                    ValueNameRNTIType,
                    ARRAY_SIZE(ValueNameRNTIType, ValueName),
                    "(MI)Unknown");

            old_object = _replace_result_int(result, "Number of Tx Antennas(M)", M);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Number of Rx Antennas(N)", N);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Spatial Rank", iSpatialRank);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "Spatial Rank",
                    ValueNameRankIndex,
                    ARRAY_SIZE(ValueNameRankIndex, ValueName),
                    "(MI)Unknown");

            tmp = _search_result_int(result, "Frequency Selective PMI");
            int iFrequencySelectivePMI = (tmp >> 1) & 3;
            int iPMIIndex = (tmp >> 4) & 15;
            tmp = _search_result_int(result, "Transmission Scheme");
            int iTransmissionScheme = tmp & 15;

            old_object = _replace_result_int(result, "Frequency Selective PMI",
                    iFrequencySelectivePMI);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "Frequency Selective PMI",
                    ValueNameFrequencySelectivePMI,
                    ARRAY_SIZE(ValueNameFrequencySelectivePMI, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "PMI Index", iPMIIndex);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Transmission Scheme",
                    iTransmissionScheme);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "Transmission Scheme",
                    ValueNameTransmissionScheme,
                    ARRAY_SIZE(ValueNameTransmissionScheme, ValueName),
                    "(MI)Unknown");

            // modulation & ratio
            tmp = _search_result_int(result, "MCS 0");
            int mod_stream0 = (tmp >> 2) & 0x3;
            float ratio = float((tmp >> 4) & 0x1fff) / 256.0;
            tmp = _search_result_int(result, "MCS 1");
            int mod_stream1 = (tmp >> 2) & 0x3;
            int carrier_index = (tmp >> 9) & 0xf;

            old_object = _replace_result_int(result, "MCS 0", mod_stream0);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "MCS 0",
                                            LtePhyPdschDemapperConfig_v23_Modulation,
                                            ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName),
                                            "(MI)Unknown");

            PyObject *pyfloat = Py_BuildValue("f", ratio);
            old_object = _replace_result(result, "Traffic to Pilot Ratio", pyfloat);
            Py_DECREF(old_object);
            Py_DECREF(pyfloat);

            old_object = _replace_result_int(result, "MCS 1", mod_stream1);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "MCS 1",
                                            LtePhyPdschDemapperConfig_v23_Modulation,
                                            ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName),
                                            "(MI)Unknown");

            // carrier index
            old_object = _replace_result_int(result, "Carrier Index", carrier_index);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "Carrier Index",
                                            LtePhyPdschDemapperConfig_v23_Carrier_Index,
                                            ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Carrier_Index, ValueName),
                                            "(MI)Unknown");
            break;
        }
    case 104:
        {
            offset += _decode_by_fmt(LtePhyPdschDemapperConfigFmt_v104,
                                        ARRAY_SIZE(LtePhyPdschDemapperConfigFmt_v104, Fmt),
                                        b, offset, length, result);

            const unsigned int SFN_RSHIFT = 5, SFN_MASK = (1 << 10) - 1;
            const unsigned int SUBFRAME_RSHIFT = 1, SUBFRAME_MASK = (1 << 4) - 1;
            int tmp = _search_result_int(result, "System Frame Number");
            int sfn = (tmp >> SFN_RSHIFT) & SFN_MASK;
            int subframe = (tmp >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
            int serv_cell = _search_result_int(result, "Serving Cell ID");
            serv_cell += (tmp & 0x1) << 8;

            PyObject *old_object = _replace_result_int(result, "Serving Cell ID", serv_cell);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "System Frame Number", sfn);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Subframe Number", subframe);
            Py_DECREF(old_object);

            // # antennas
            tmp = _search_result_int(result, "Number of Tx Antennas(M)");
            int iRNTIType = tmp & 15;
            int iSpatialRank = (tmp >> 14) & 3;
            tmp = (tmp >> 8) & 0x0f;
            int M = tmp & 0x3;
            M = (M != 3? (1 << M): -1);
            int N = (tmp >> 2) & 0x1;
            N = (1 << N);

            old_object = _replace_result_int(result, "PDSCH RNTI Type", iRNTIType);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "PDSCH RNTI Type",
                    ValueNameRNTIType,
                    ARRAY_SIZE(ValueNameRNTIType, ValueName),
                    "(MI)Unknown");

            old_object = _replace_result_int(result, "Number of Tx Antennas(M)", M);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Number of Rx Antennas(N)", N);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Spatial Rank", iSpatialRank);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "Spatial Rank",
                    ValueNameRankIndex,
                    ARRAY_SIZE(ValueNameRankIndex, ValueName),
                    "(MI)Unknown");

            tmp = _search_result_int(result, "Frequency Selective PMI");
            int iFrequencySelectivePMI = (tmp >> 1) & 3;
            int iPMIIndex = (tmp >> 4) & 15;
            tmp = _search_result_int(result, "Transmission Scheme");
            int iTransmissionScheme = tmp & 15;

            old_object = _replace_result_int(result, "Frequency Selective PMI",
                    iFrequencySelectivePMI);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "Frequency Selective PMI",
                    ValueNameFrequencySelectivePMI,
                    ARRAY_SIZE(ValueNameFrequencySelectivePMI, ValueName),
                    "(MI)Unknown");
            old_object = _replace_result_int(result, "PMI Index", iPMIIndex);
            Py_DECREF(old_object);
            old_object = _replace_result_int(result, "Transmission Scheme",
                    iTransmissionScheme);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result, "Transmission Scheme",
                    ValueNameTransmissionScheme,
                    ARRAY_SIZE(ValueNameTransmissionScheme, ValueName),
                    "(MI)Unknown");

            // modulation & ratio
            tmp = _search_result_int(result, "MCS 0");
            int mod_stream0 = (tmp >> 2) & 0x3;
            float ratio = float((tmp >> 4) & 0x1fff) / 256.0;
            tmp = _search_result_int(result, "MCS 1");
            int mod_stream1 = (tmp >> 2) & 0x3;
            int carrier_index = (tmp >> 9) & 0xf;

            old_object = _replace_result_int(result, "MCS 0", mod_stream0);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "MCS 0",
                                            LtePhyPdschDemapperConfig_v23_Modulation,
                                            ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName),
                                            "(MI)Unknown");

            PyObject *pyfloat = Py_BuildValue("f", ratio);
            old_object = _replace_result(result, "Traffic to Pilot Ratio", pyfloat);
            Py_DECREF(old_object);
            Py_DECREF(pyfloat);

            old_object = _replace_result_int(result, "MCS 1", mod_stream1);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "MCS 1",
                                            LtePhyPdschDemapperConfig_v23_Modulation,
                                            ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Modulation, ValueName),
                                            "(MI)Unknown");

            // carrier index
            old_object = _replace_result_int(result, "Carrier Index", carrier_index);
            Py_DECREF(old_object);
            (void)_map_result_field_to_name(result,
                                            "Carrier Index",
                                            LtePhyPdschDemapperConfig_v23_Carrier_Index,
                                            ARRAY_SIZE(LtePhyPdschDemapperConfig_v23_Carrier_Index, ValueName),
                                            "(MI)Unknown");
            break;
        }

    default:
        printf("(MI)Unknown LTE PHY PDSCH Demapper Configuration version: 0x%x\n", pkt_ver);
        return 0;
    }

    return offset - start;
}

static int
_decode_lte_phy_cmlifmr(const char *b, int offset, size_t length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    (void) _map_result_field_to_name(
            result, "Serving Cell Index",
            ValueNameCellIndex, ARRAY_SIZE(ValueNameCellIndex, ValueName),
            "(MI)Unknown");

    switch (pkt_ver) {
    case 3:
        {
            offset += _decode_by_fmt(LtePhyCmlifmrFmt_v3_Header,
                                        ARRAY_SIZE(LtePhyCmlifmrFmt_v3_Header, Fmt),
                                        b, offset, length, result);
            int n_neighbor_cells = _search_result_int(result, "Number of Neighbor Cells");
            int n_detected_cells = _search_result_int(result, "Number of Detected Cells");

            PyObject *t = NULL;
            PyObject *result_allcells = NULL;
            // decode "Neighbor Cells"
            result_allcells = PyList_New(0);
            for (int i = 0; i < n_neighbor_cells; i++) {
                PyObject *result_cell = PyList_New(0);
                offset += _decode_by_fmt(LtePhyCmlifmrFmt_v3_Neighbor_Cell,
                                            ARRAY_SIZE(LtePhyCmlifmrFmt_v3_Neighbor_Cell, Fmt),
                                            b, offset, length, result_cell);
                t = Py_BuildValue("(sOs)", "Ignored", result_cell, "dict");
                PyList_Append(result_allcells, t);
                Py_DECREF(t);
                Py_DECREF(result_cell);
            }
            t = Py_BuildValue("(sOs)", "Neighbor Cells", result_allcells, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allcells);

            // decode "Detected Cells"
            result_allcells = PyList_New(0);
            for (int i = 0; i < n_detected_cells; i++) {
                PyObject *result_cell = PyList_New(0);
                offset += _decode_by_fmt(LtePhyCmlifmrFmt_v3_Detected_Cell,
                                            ARRAY_SIZE(LtePhyCmlifmrFmt_v3_Detected_Cell, Fmt),
                                            b, offset, length, result_cell);
                t = Py_BuildValue("(sOs)", "Ignored", result_cell, "dict");
                PyList_Append(result_allcells, t);
                Py_DECREF(t);
                Py_DECREF(result_cell);
            }
            t = Py_BuildValue("(sOs)", "Detected Cells", result_allcells, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allcells);

            return offset - start;
        }
    case 4:
        {
            offset += _decode_by_fmt(LtePhyCmlifmrFmt_v4_Header,
                                        ARRAY_SIZE(LtePhyCmlifmrFmt_v4_Header, Fmt),
                                        b, offset, length, result);
            int n_neighbor_cells = _search_result_int(result, "Number of Neighbor Cells");
            int n_detected_cells = _search_result_int(result, "Number of Detected Cells");

            PyObject *t = NULL;
            PyObject *result_allcells = NULL;
            // decode "Neighbor Cells"
            result_allcells = PyList_New(0);
            for (int i = 0; i < n_neighbor_cells; i++) {
                PyObject *result_cell = PyList_New(0);
                offset += _decode_by_fmt(LtePhyCmlifmrFmt_v4_Neighbor_Cell,
                                            ARRAY_SIZE(LtePhyCmlifmrFmt_v4_Neighbor_Cell, Fmt),
                                            b, offset, length, result_cell);
                t = Py_BuildValue("(sOs)", "Ignored", result_cell, "dict");
                PyList_Append(result_allcells, t);
                Py_DECREF(t);
                Py_DECREF(result_cell);
            }
            t = Py_BuildValue("(sOs)", "Neighbor Cells", result_allcells, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allcells);

            // decode "Detected Cells"
            result_allcells = PyList_New(0);
            for (int i = 0; i < n_detected_cells; i++) {
                PyObject *result_cell = PyList_New(0);
                offset += _decode_by_fmt(LtePhyCmlifmrFmt_v4_Detected_Cell,
                                            ARRAY_SIZE(LtePhyCmlifmrFmt_v4_Detected_Cell, Fmt),
                                            b, offset, length, result_cell);
                t = Py_BuildValue("(sOs)", "Ignored", result_cell, "dict");
                PyList_Append(result_allcells, t);
                Py_DECREF(t);
                Py_DECREF(result_cell);
            }
            t = Py_BuildValue("(sOs)", "Detected Cells", result_allcells, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allcells);

            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PHY CMLIFMR version: 0x%x\n", pkt_ver);
        return 0;
    }
}

static int
_decode_lte_phy_subpkt(const char *b, int offset, size_t length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Number of SubPackets");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LtePhySubpktFmt_v1_SubpktHeader,
                                            ARRAY_SIZE(LtePhySubpktFmt_v1_SubpktHeader, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "SubPacket Version");
                const char *type_name = search_name(LtePhySubpkt_SubpktType,
                                                    ARRAY_SIZE(LtePhySubpkt_SubpktType, ValueName),
                                                    subpkt_id);
                if (type_name == NULL) {    // not found
                    printf("(MI)Unknown LTE PHY Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    if (strcmp(type_name, "Serving_Cell_Measurement_Result") == 0) {
                        switch (subpkt_ver) {
                        case 4:
                            {
                                offset += _decode_by_fmt(
                                        LtePhySubpktFmt_v1_Scmr_v4,
                                        ARRAY_SIZE(LtePhySubpktFmt_v1_Scmr_v4,
                                            Fmt),
                                        b, offset, length, result_subpkt);
                                int temp = _search_result_int(result_subpkt,
                                        "Physical Cell ID");
                                int iPhyCellId = temp & 511;   // 9 bits
                                int iServingCellIdx = (temp >> 9) & 7; // 3 bits
                                PyObject *old_object = _replace_result_int(
                                        result_subpkt,
                                        "Physical Cell ID", iPhyCellId);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(
                                        result_subpkt,
                                        "Serving Cell Index", iServingCellIdx);
                                Py_DECREF(old_object);
                                (void) _map_result_field_to_name(
                                        result_subpkt,
                                        "Serving Cell Index",
                                        ValueNameCellIndex,
                                        ARRAY_SIZE(ValueNameCellIndex,
                                            ValueName),
                                        "(MI)Unknown");

                                temp = _search_result_int(result_subpkt,
                                        "Current SFN");
                                int iSysFN = temp & 1023;   // 10 bits
                                int iSubFN = (temp >> 10) & 15; // 4 bits
                                old_object = _replace_result_int(result_subpkt,
                                        "Current SFN", iSysFN);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(result_subpkt,
                                        "Current Subframe Number", iSubFN);
                                Py_DECREF(old_object);

                                unsigned int utemp;
                                PyObject *pyfloat;

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP Rx[0]");
                                float RSRP0 = float((utemp >> 10) & 4095);
                                RSRP0 = RSRP0 * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP0);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP Rx[1]");
                                float RSRP1 = float((utemp >> 12) & 4095);
                                RSRP1 = RSRP1 * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP1);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP");
                                float RSRP = float((utemp >> 12) & 4095);
                                RSRP = RSRP * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRQ Rx[0]");
                                float RSRQ0 = float((utemp >> 12) & 1023);
                                RSRQ0 = RSRQ0 * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ0);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRQ Rx[1]");
                                float RSRQ1 = float(utemp & 1023);
                                RSRQ1 = RSRQ1 * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ1);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float RSRQ = float((utemp >> 20) & 1023);
                                RSRQ = RSRQ * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "RSSI Rx[0]");
                                float RSSI0 = float((utemp >> 10) & 2047);
                                RSSI0 = RSSI0 * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI0);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float RSSI1 = float((utemp >> 21) & 2047);
                                RSSI1 = RSSI1 * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI1);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "RSSI");
                                float RSSI = float(utemp & 2047);
                                RSSI = RSSI * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "FTL SNR Rx[0]");
                                float SNR0 = float(utemp & 511);
                                SNR0 = SNR0 * 0.1 - 20.0;
                                pyfloat = Py_BuildValue("f", SNR0);
                                old_object = _replace_result(result_subpkt,
                                        "FTL SNR Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float SNR1 = float((utemp >> 9) & 511);
                                SNR1 = SNR1 * 0.1 - 20.0;
                                pyfloat = Py_BuildValue("f", SNR1);
                                old_object = _replace_result(result_subpkt,
                                        "FTL SNR Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                success = true;
                                break;
                            }
                        case 7:
                            {
                                offset += _decode_by_fmt(
                                        LtePhySubpktFmt_v1_Scmr_v7,
                                        ARRAY_SIZE(LtePhySubpktFmt_v1_Scmr_v7,
                                            Fmt),
                                        b, offset, length, result_subpkt);
                                int temp = _search_result_int(result_subpkt,
                                        "Physical Cell ID");
                                int iPhyCellId = temp & 511;   // 9 bits
                                int iServingCellIdx = (temp >> 9) & 7; // 3 bits
                                PyObject *old_object = _replace_result_int(
                                        result_subpkt,
                                        "Physical Cell ID", iPhyCellId);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(
                                        result_subpkt,
                                        "Serving Cell Index", iServingCellIdx);
                                Py_DECREF(old_object);
                                (void) _map_result_field_to_name(
                                        result_subpkt,
                                        "Serving Cell Index",
                                        ValueNameCellIndex,
                                        ARRAY_SIZE(ValueNameCellIndex,
                                            ValueName),
                                        "(MI)Unknown");

                                temp = _search_result_int(result_subpkt,
                                        "Current SFN");
                                int iSysFN = temp & 1023;   // 10 bits
                                int iSubFN = (temp >> 10) & 15; // 4 bits
                                old_object = _replace_result_int(result_subpkt,
                                        "Current SFN", iSysFN);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(result_subpkt,
                                        "Current Subframe Number", iSubFN);
                                Py_DECREF(old_object);

                                unsigned int utemp;
                                PyObject *pyfloat;

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP Rx[0]");
                                float RSRP0 = float((utemp >> 10) & 4095);
                                RSRP0 = RSRP0 * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP0);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP Rx[1]");
                                float RSRP1 = float((utemp >> 12) & 4095);
                                RSRP1 = RSRP1 * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP1);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP");
                                float RSRP = float((utemp >> 12) & 4095);
                                RSRP = RSRP * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRQ Rx[0]");
                                float RSRQ0 = float((utemp >> 12) & 1023);
                                RSRQ0 = RSRQ0 * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ0);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRQ Rx[1]");
                                float RSRQ1 = float(utemp & 1023);
                                RSRQ1 = RSRQ1 * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ1);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float RSRQ = float((utemp >> 20) & 1023);
                                RSRQ = RSRQ * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "RSSI Rx[0]");
                                float RSSI0 = float((utemp >> 10) & 2047);
                                RSSI0 = RSSI0 * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI0);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float RSSI1 = float((utemp >> 21) & 2047);
                                RSSI1 = RSSI1 * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI1);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "RSSI");
                                float RSSI = float(utemp & 2047);
                                RSSI = RSSI * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "FTL SNR Rx[0]");
                                float SNR0 = float(utemp & 511);
                                SNR0 = SNR0 * 0.1 - 20.0;
                                pyfloat = Py_BuildValue("f", SNR0);
                                old_object = _replace_result(result_subpkt,
                                        "FTL SNR Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float SNR1 = float((utemp >> 9) & 511);
                                SNR1 = SNR1 * 0.1 - 20.0;
                                pyfloat = Py_BuildValue("f", SNR1);
                                old_object = _replace_result(result_subpkt,
                                        "FTL SNR Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                success = true;
                                break;
                            }

                        case 19:
                            {
                                offset += _decode_by_fmt(
                                        LtePhySubpktFmt_v1_Scmr_v19,
                                        ARRAY_SIZE(LtePhySubpktFmt_v1_Scmr_v19,
                                            Fmt),
                                        b, offset, length, result_subpkt);
                                int temp = _search_result_int(result_subpkt,
                                        "Physical Cell ID");
                                int iPhyCellId = temp & 511;   // 9 bits
                                int iServingCellIdx = (temp >> 9) & 7; // 3 bits
                                int iIsServingCell = (temp >> 12) & 1;  // 1 bit
                                PyObject *old_object = _replace_result_int(
                                        result_subpkt,
                                        "Physical Cell ID", iPhyCellId);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(
                                        result_subpkt,
                                        "Serving Cell Index", iServingCellIdx);
                                Py_DECREF(old_object);
                                (void) _map_result_field_to_name(
                                        result_subpkt,
                                        "Serving Cell Index",
                                        ValueNameCellIndex,
                                        ARRAY_SIZE(ValueNameCellIndex,
                                            ValueName),
                                        "(MI)Unknown");
                                old_object = _replace_result_int(
                                        result_subpkt,
                                        "Is Serving Cell", iIsServingCell);
                                Py_DECREF(old_object);

                                temp = _search_result_int(result_subpkt,
                                        "Current SFN");
                                int iSysFN = temp & 1023;   // 10 bits
                                int iSubFN = (temp >> 10) & 15; // 4 bits
                                old_object = _replace_result_int(result_subpkt,
                                        "Current SFN", iSysFN);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(result_subpkt,
                                        "Current Subframe Number", iSubFN);
                                Py_DECREF(old_object);

                                unsigned int utemp;
                                PyObject *pyfloat;

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP Rx[0]");
                                float RSRP0 = float((utemp >> 10) & 4095);
                                RSRP0 = RSRP0 * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP0);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP Rx[1]");
                                float RSRP1 = float((utemp >> 12) & 4095);
                                RSRP1 = RSRP1 * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP1);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP");
                                float RSRP = float((utemp >> 12) & 4095);
                                RSRP = RSRP * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRQ Rx[0]");
                                float RSRQ0 = float((utemp >> 12) & 1023);
                                RSRQ0 = RSRQ0 * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ0);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRQ Rx[1]");
                                float RSRQ1 = float(utemp & 1023);
                                RSRQ1 = RSRQ1 * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ1);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float RSRQ = float((utemp >> 20) & 1023);
                                RSRQ = RSRQ * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "RSSI Rx[0]");
                                float RSSI0 = float((utemp >> 10) & 2047);
                                RSSI0 = RSSI0 * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI0);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float RSSI1 = float((utemp >> 21) & 2047);
                                RSSI1 = RSSI1 * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI1);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "RSSI");
                                float RSSI = float(utemp & 2047);
                                RSSI = RSSI * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "FTL SNR Rx[0]");
                                float SNR0 = float(utemp & 511);
                                SNR0 = SNR0 * 0.1 - 20.0;
                                pyfloat = Py_BuildValue("f", SNR0);
                                old_object = _replace_result(result_subpkt,
                                        "FTL SNR Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float SNR1 = float((utemp >> 9) & 511);
                                SNR1 = SNR1 * 0.1 - 20.0;
                                pyfloat = Py_BuildValue("f", SNR1);
                                old_object = _replace_result(result_subpkt,
                                        "FTL SNR Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "Post IC RSRQ");
                                float postICRSRQ = float(utemp * 0.0625 - 30);
                                pyfloat = Py_BuildValue("f", postICRSRQ);
                                old_object = _replace_result(result_subpkt,
                                        "Post IC RSRQ", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "Projected SIR");
                                int SIR = 0;
                                if (utemp & (1 << 31)) {
                                    SIR = utemp - 4294967296;
                                } else {
                                    SIR = utemp;
                                }
                                SIR = SIR / 16;
                                old_object = _replace_result_int(result_subpkt,
                                        "Projected SIR", SIR);
                                Py_DECREF(old_object);

                                success = true;
                                break;
                            }
                        case 22:
                            {
                                offset += _decode_by_fmt(
                                        LtePhySubpktFmt_v1_Scmr_v22,
                                        ARRAY_SIZE(LtePhySubpktFmt_v1_Scmr_v22,
                                            Fmt),
                                        b, offset, length, result_subpkt);
                                int temp = _search_result_int(result_subpkt,
                                        "Physical Cell ID");
                                int iPhyCellId = temp & 511;   // 9 bits
                                int iServingCellIdx = (temp >> 9) & 7; // 3 bits
                                int iIsServingCell = (temp >> 12) & 1;  // 1 bit
                                PyObject *old_object = _replace_result_int(
                                        result_subpkt,
                                        "Physical Cell ID", iPhyCellId);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(
                                        result_subpkt,
                                        "Serving Cell Index", iServingCellIdx);
                                Py_DECREF(old_object);
                                (void) _map_result_field_to_name(
                                        result_subpkt,
                                        "Serving Cell Index",
                                        ValueNameCellIndex,
                                        ARRAY_SIZE(ValueNameCellIndex,
                                            ValueName),
                                        "(MI)Unknown");
                                old_object = _replace_result_int(
                                        result_subpkt,
                                        "Is Serving Cell", iIsServingCell);
                                Py_DECREF(old_object);

                                temp = _search_result_int(result_subpkt,
                                        "Current SFN");
                                int iSysFN = temp & 1023;   // 10 bits
                                int iSubFN = (temp >> 10) & 15; // 4 bits
                                old_object = _replace_result_int(result_subpkt,
                                        "Current SFN", iSysFN);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(result_subpkt,
                                        "Current Subframe Number", iSubFN);
                                Py_DECREF(old_object);

                                unsigned int utemp;
                                PyObject *pyfloat;

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP Rx[0]");
                                float RSRP0 = float((utemp >> 10) & 4095);
                                RSRP0 = RSRP0 * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP0);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP Rx[1]");
                                float RSRP1 = float((utemp >> 12) & 4095);
                                RSRP1 = RSRP1 * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP1);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP");
                                float RSRP = float((utemp >> 12) & 4095);
                                RSRP = RSRP * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRQ Rx[0]");
                                float RSRQ0 = float((utemp >> 12) & 1023);
                                RSRQ0 = RSRQ0 * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ0);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRQ Rx[1]");
                                float RSRQ1 = float(utemp & 1023);
                                RSRQ1 = RSRQ1 * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ1);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float RSRQ = float((utemp >> 10) & 1023);
                                RSRQ = RSRQ * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "RSSI Rx[0]");
                                float RSSI0 = float((utemp >> 10) & 2047);
                                RSSI0 = RSSI0 * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI0);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float RSSI1 = float((utemp >> 21) & 2047);
                                RSSI1 = RSSI1 * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI1);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "RSSI");
                                float RSSI = float(utemp & 2047);
                                RSSI = RSSI * 0.0625 - 110.0;
                                pyfloat = Py_BuildValue("f", RSSI);
                                old_object = _replace_result(result_subpkt,
                                        "RSSI", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "FTL SNR Rx[0]");
                                float SNR0 = float(utemp & 511);
                                SNR0 = SNR0 * 0.1 - 20.0;
                                pyfloat = Py_BuildValue("f", SNR0);
                                old_object = _replace_result(result_subpkt,
                                        "FTL SNR Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float SNR1 = float((utemp >> 9) & 511);
                                SNR1 = SNR1 * 0.1 - 20.0;
                                pyfloat = Py_BuildValue("f", SNR1);
                                old_object = _replace_result(result_subpkt,
                                        "FTL SNR Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "Post IC RSRQ");
                                float postICRSRQ = float(utemp * 0.0625 - 30);
                                pyfloat = Py_BuildValue("f", postICRSRQ);
                                old_object = _replace_result(result_subpkt,
                                        "Post IC RSRQ", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(result_subpkt,
                                        "Projected SIR");
                                int SIR = 0;
                                if (utemp & (1 << 31)) {
                                    SIR = utemp - 4294967296;
                                } else {
                                    SIR = utemp;
                                }
                                SIR = SIR / 16;
                                old_object = _replace_result_int(result_subpkt,
                                        "Projected SIR", SIR);
                                Py_DECREF(old_object);

                                success = true;
                                break;
                            }
                            case 35:
                            {
                                offset += _decode_by_fmt(
                                        LtePhySubpktFmt_v1_Scmr_v35,
                                        ARRAY_SIZE(LtePhySubpktFmt_v1_Scmr_v35,
                                            Fmt),
                                        b, offset, length, result_subpkt);
                                int temp = _search_result_int(result_subpkt,
                                        "Physical Cell ID");
                                int iPhyCellId = temp & 511;   // 9 bits
                                int iServingCellIdx = (temp >> 9) & 7; // 3 bits
                                int iIsServingCell = (temp >> 12) & 1;  // 1 bit
                                PyObject *old_object = _replace_result_int(
                                        result_subpkt,
                                        "Physical Cell ID", iPhyCellId);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(
                                        result_subpkt,
                                        "Serving Cell Index", iServingCellIdx);
                                Py_DECREF(old_object);
                                (void) _map_result_field_to_name(
                                        result_subpkt,
                                        "Serving Cell Index",
                                        ValueNameCellIndex,
                                        ARRAY_SIZE(ValueNameCellIndex,
                                            ValueName),
                                        "(MI)Unknown");
                                old_object = _replace_result_int(
                                        result_subpkt,
                                        "Is Serving Cell", iIsServingCell);
                                Py_DECREF(old_object);

                                temp = _search_result_int(result_subpkt,
                                        "Current SFN");
                                int iSysFN = temp & 1023;   // 10 bits
                                int iSubFN = (temp >> 10) & 15; // 4 bits
                                old_object = _replace_result_int(result_subpkt,
                                        "Current SFN", iSysFN);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(result_subpkt,
                                        "Current Subframe Number", iSubFN);
                                Py_DECREF(old_object);

                                unsigned int utemp;
                                PyObject *pyfloat;

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP Rx[0]");
                                float RSRP0 = float((utemp >> 10) & 4095);
                                RSRP0 = RSRP0 * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP0);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP Rx[1]");
                                float RSRP1 = float((utemp >> 12) & 4095);
                                RSRP1 = RSRP1 * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP1);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRP");
                                float RSRP = float((utemp) & 4095);
                                RSRP = RSRP * 0.0625 - 180.0;
                                pyfloat = Py_BuildValue("f", RSRP);
                                old_object = _replace_result(result_subpkt,
                                        "RSRP", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                utemp = _search_result_uint(
                                        result_subpkt, "RSRQ Rx[0]");
                                float RSRQ0 = float((utemp) & 1023);
                                RSRQ0 = RSRQ0 * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ0);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ Rx[0]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float RSRQ1 = float((utemp >> 20) & 1023);
                                RSRQ1 = RSRQ1 * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ1);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ Rx[1]", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                float RSRQ = float((utemp) & 1023);
                                RSRQ = RSRQ * 0.0625 - 30.0;
                                pyfloat = Py_BuildValue("f", RSRQ);
                                old_object = _replace_result(result_subpkt,
                                        "RSRQ", pyfloat);
                                Py_DECREF(old_object);
                                Py_DECREF(pyfloat);

                                success = true;
                                break;
                            }
                        default:
                            break;
                        }
                    }
                    // TODO: replace type ID to name.

                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "Ignored", result_subpkt, "dict");
                        PyList_Append(result_allpkts, t);
                        Py_DECREF(result_subpkt);
                    } else {
                        printf("(MI)Unknown LTE PHY Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                    }
                }
            }
            PyObject *t = Py_BuildValue("(sOs)",
                                        "Subpackets", result_allpkts, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PHY packet version: 0x%x\n", pkt_ver);
        return 0;
    }
}

static int
_decode_lte_phy_irat_cdma_subpkt(const char *b, int offset, size_t length,
                        PyObject *result) {

    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Subpacket count");

    switch(pkt_ver){
        case 1:
            for(int i=0; i< n_subpkt; i++)
            {

                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LtePhyIratSubPktFmt,
                                            ARRAY_SIZE(LtePhyIratSubPktFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                // int subpkt_id = _search_result_int(result_subpkt, "Subpacket ID");
                // int subpkt_ver = _search_result_int(result_subpkt, "Version");
                // int subpkt_size = _search_result_int(result_subpkt, "Subpacket size");

                PyObject *result_subpkt_cdma = PyList_New(0);
                offset += _decode_by_fmt(LtePhyIratCDMACellFmt,
                                            ARRAY_SIZE(LtePhyIratCDMACellFmt, Fmt),
                                            b, offset, length, result_subpkt_cdma);
                int n_pilot = _search_result_int(result_subpkt_cdma, "Number of Pilots");
                int band = _search_result_int(result_subpkt_cdma, "Band");
                for(int j=0; j<n_pilot; j++){
                    PyObject *result_subpkt_cdma_pilot = PyList_New(0);
                    offset += _decode_by_fmt(LtePhyIratCDMACellPilotFmt,
                                            ARRAY_SIZE(LtePhyIratCDMACellPilotFmt, Fmt),
                                            b, offset, length, result_subpkt_cdma_pilot);
                    char name[64];
                    sprintf(name,"Pilot_%d",j);
                    PyObject *t = Py_BuildValue("(sOs)",
                                            name, result_subpkt_cdma_pilot, "dict");
                    PyList_Append(result_subpkt_cdma, t);
                    Py_DECREF(result_subpkt_cdma_pilot);
                }
                char name[64];
                sprintf(name,"Band_%d",band);
                PyObject *t = Py_BuildValue("(sOs)",
                                            "CDMA", result_subpkt_cdma, "dict");
                PyList_Append(result_subpkt, t);
                Py_DECREF(result_subpkt_cdma);

                char name2[64];
                sprintf(name2,"Subpacket_%d",i);
                PyObject *t2 = Py_BuildValue("(sOs)",
                                            name2, result_subpkt, "dict");
                PyList_Append(result, t2);
                Py_DECREF(result_subpkt);
            }
            break;

        default:
            break;
    }

    return offset - start;
}


static int
_decode_lte_phy_irat_subpkt(const char *b, int offset, size_t length,
                        PyObject *result) {

    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Subpacket count");

    switch(pkt_ver){
        case 1:
            for(int i=0; i< n_subpkt; i++)
            {

                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LtePhyIratSubPktFmt,
                                            ARRAY_SIZE(LtePhyIratSubPktFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "Subpacket ID");
                // int subpkt_ver = _search_result_int(result_subpkt, "Version");
                int subpkt_size = _search_result_int(result_subpkt, "Subpacket size");
                switch(subpkt_id){
                    case LtePhyIratType_WCDMA:
                    {
                        PyObject *result_subpkt_wcdma = PyList_New(0);
                        offset += _decode_by_fmt(LtePhyIratWCDMAFmt,
                                            ARRAY_SIZE(LtePhyIratWCDMAFmt, Fmt),
                                            b, offset, length, result_subpkt_wcdma);
                        int n_freq = _search_result_int(result_subpkt_wcdma, "Number of frequencies");
                        for(int j=0; j<n_freq; j++){
                            PyObject *result_subpkt_wcdma_freq = PyList_New(0);
                            offset += _decode_by_fmt(LtePhyIratWCDMACellMetaFmt,
                                            ARRAY_SIZE(LtePhyIratWCDMACellMetaFmt, Fmt),
                                            b, offset, length, result_subpkt_wcdma_freq);
                            // int freq = _search_result_int(result_subpkt_wcdma_freq, "Frequency");
                            int n_cell = _search_result_int(result_subpkt_wcdma_freq, "Number of cells");
                            for(int k=0; k<n_cell; k++){
                                PyObject *result_subpkt_wcdma_cell = PyList_New(0);
                                offset += _decode_by_fmt(LtePhyIratWCDMACellFmt,
                                            ARRAY_SIZE(LtePhyIratWCDMACellFmt, Fmt),
                                            b, offset, length, result_subpkt_wcdma_cell);

                                char name[64];
                                sprintf(name,"CellPacket_%d",k);
                                PyObject *t = Py_BuildValue("(sOs)",
                                                    name, result_subpkt_wcdma_cell, "dict");
                                PyList_Append(result_subpkt_wcdma_freq, t);
                                Py_DECREF(result_subpkt_wcdma_cell);
                            }
                            char name[64];
                            sprintf(name,"FrequencyPacket_%d",j);
                            PyObject *t = Py_BuildValue("(sOs)",
                                                    name, result_subpkt_wcdma_freq, "dict");
                            PyList_Append(result_subpkt_wcdma, t);
                            Py_DECREF(result_subpkt_wcdma_freq);
                        }

                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "WCDMA", result_subpkt_wcdma, "dict");
                        PyList_Append(result_subpkt, t);
                        Py_DECREF(result_subpkt_wcdma);
                        break;
                    }

                    default:
                    {
                        offset += subpkt_size - 4;
                        break;
                    }
                }

                char name[64];
                sprintf(name,"Subpacket_%d",i);
                PyObject *t = Py_BuildValue("(sOs)",
                                            name, result_subpkt, "dict");
                PyList_Append(result, t);
                Py_DECREF(result_subpkt);

            }

        default:
            break;
    }

    return offset - start;
}

static int
_decode_lte_pdcp_dl_srb_integrity_data_pdu(const char *b, int offset, size_t length,
                                            PyObject *result) {
    (void)length;
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 1:
        break;
    default:
        printf("(MI)Unknown LTE PDCP DL SRB Integrity Data PDU version: %d\n", pkt_ver);
        return 0;
    }
    int pktCount = _search_result_int(result, "Num SubPkt");
    if (pktCount != 1) {
        printf ("(MI)Unknown LTE PDCP DL SRB Integrity Data PDU Num SubPkt: %d\n", pktCount);
        return 0;
    }


    int pdu_length = _search_result_int(result, "PDU Size");

    PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, pdu_length, "raw_msg/LTE-PDCP_DL_SRB");
    PyList_Append(result, t);
    Py_DECREF(t);
    return (offset - start) + pdu_length;


}

static int
_decode_lte_pdcp_ul_srb_integrity_data_pdu(const char *b, int offset, size_t length,
                                            PyObject *result) {
    (void)length;
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 1:
        break;
    default:
        printf("(MI)Unknown LTE PDCP UL SRB Integrity Data PDU version: %d\n", pkt_ver);
        return 0;
    }
    int pktCount = _search_result_int(result, "Num SubPkt");
    if (pktCount != 1) {
        printf ("(MI)Unknown LTE PDCP UL SRB Integrity Data PDU Num SubPkt: %d\n", pktCount);
        return 0;
    }


    int pdu_length = _search_result_int(result, "PDU Size");

    PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, pdu_length, "raw_msg/LTE-PDCP_UL_SRB");
    PyList_Append(result, t);
    Py_DECREF(t);
    return (offset - start) + pdu_length;
}

//------------------------------------------------------
// Jie
static int
_decode_lte_mac_configuration_subpkt(const char *b, int offset, size_t length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                int start_subpkt = offset;
                offset += _decode_by_fmt(LteMacConfiguration_SubpktHeader,
                                            ARRAY_SIZE(LteMacConfiguration_SubpktHeader, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                int subpkt_size = _search_result_int(result_subpkt, "SubPacket Size");
                const char *type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                    ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                    subpkt_id);
                (void) _map_result_field_to_name(
                                result_subpkt,
                                "SubPacket ID",
                                LteMacConfigurationSubpkt_SubpktType,
                                ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                "(MI)Unknown");

                if (type_name == NULL) {    // not found
                    printf("(MI)Unknown LTE MAC Configuration Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    switch (subpkt_id) {
                    case 0: //Config Type Subpacket
                        offset += _decode_by_fmt(LteMacConfigurationSubpkt_ConfigType,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_ConfigType, Fmt),
                                                b, offset, length, result_subpkt);
                        (void) _map_result_field_to_name(
                                result_subpkt,
                                "Config reason",
                                LteMacConfigurationConfigType_ConfigReason,
                                ARRAY_SIZE(LteMacConfigurationConfigType_ConfigReason, ValueName),
                                "NORMAL");
                        success = true;
                        break;
                    case 1: //DL Config Subpacket
                        offset += _decode_by_fmt(LteMacConfigurationSubpkt_DLConfig,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_DLConfig, Fmt),
                                                b, offset, length, result_subpkt);
                        success = true;
                        break;
                    case 2: //UL Config Subpacket
                        offset += _decode_by_fmt(LteMacConfigurationSubpkt_ULConfig,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_ULConfig, Fmt),
                                                b, offset, length, result_subpkt);
                        success = true;
                        break;
                    case 3: //RACH Config Subpacket
                        offset += _decode_by_fmt(LteMacConfigurationSubpkt_RACHConfig,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_RACHConfig, Fmt),
                                                b, offset, length, result_subpkt);
                        success = true;
                        break;
                    case 4: //LC Config Subpacket
                        {
                            offset += _decode_by_fmt(LteMacConfigurationSubpkt_LCConfig,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_LCConfig, Fmt),
                                                b, offset, length, result_subpkt);
                            int num_LC = _search_result_int(result_subpkt, "Number of added/modified LC");
                            int start_LC = offset;

                            for (int j = 0; j < num_LC; j++) {
                                PyObject *result_subpkt_LC = PyList_New(0);
                                offset += _decode_by_fmt(LteMacConfiguration_LCConfig_LC,
                                                ARRAY_SIZE(LteMacConfiguration_LCConfig_LC, Fmt),
                                                b, offset, length, result_subpkt_LC);
                                PyObject *t = Py_BuildValue("(sOs)",
                                                    "added/modified LC", result_subpkt_LC, "dict");
                                PyList_Append(result_subpkt, t);
                                Py_DECREF(result_subpkt_LC);
                            }
                            offset += 290 - (offset-start_LC);
                            success = true;
                            break;
                        }
                    case 13: //eMBMBS Config SubPacket
                        offset += _decode_by_fmt(LteMacConfigurationSubpkt_eMBMSConfig,
                                                ARRAY_SIZE(LteMacConfigurationSubpkt_eMBMSConfig, Fmt),
                                                b, offset, length, result_subpkt);
                        success = true;
                        break;
                    default:
                        break;
                    }
                    // TODO: replace type ID to name.

                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "Ignored", result_subpkt, "dict");
                        PyList_Append(result_allpkts, t);
                        Py_DECREF(result_subpkt);
                    } else {
                        printf("(MI)Unknown LTE MAC Configuration Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                    }
                }
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)",
                                        "Subpackets", result_allpkts, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE MAC Configuration packet version: 0x%x\n", pkt_ver);
        return 0;
    }
}

//------------------------------------------------------
// TODO: Jie
static int
_decode_lte_mac_ul_transportblock_subpkt(const char *b, int offset, size_t length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMacULTransportBlock_SubpktHeaderFmt,
                                            ARRAY_SIZE(LteMacULTransportBlock_SubpktHeaderFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                int subpkt_nsample = _search_result_int(result_subpkt, "Num Samples");
                const char *type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                    ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                    subpkt_id);
                (void) _map_result_field_to_name(
                                result_subpkt,
                                "SubPacket ID",
                                LteMacConfigurationSubpkt_SubpktType,
                                ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                "(MI)Unknown");

                if (type_name == NULL) {    // not found
                    printf("(MI)Unknown LTE MAC Uplink Transport Block Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    PyObject *result_sample_list = PyList_New(0);
                    switch (subpkt_ver) {
                        case 1:
                            {
                                // UL Transport Block Subpacket V1
                                for (int j = 0; j < subpkt_nsample; j++) {
                                    PyObject *result_subpkt_sample = PyList_New(0);
                                    offset += _decode_by_fmt(LteMacULTransportBlock_SubpktV1_SampleFmt,
                                            ARRAY_SIZE(LteMacULTransportBlock_SubpktV1_SampleFmt, Fmt),
                                            b, offset, length, result_subpkt_sample);
                                    (void) _map_result_field_to_name(
                                            result_subpkt_sample,
                                            "BSR event",
                                            BSREvent,
                                            ARRAY_SIZE(BSREvent, ValueName),
                                            "(MI)Unknown");
                                    (void) _map_result_field_to_name(
                                            result_subpkt_sample,
                                            "BSR trig",
                                            BSRTrig,
                                            ARRAY_SIZE(BSRTrig, ValueName),
                                            "(MI)Unknown");
                                    (void) _map_result_field_to_name(
                                            result_subpkt_sample,
                                            "RNTI Type",
                                            ValueNameRNTIType,
                                            ARRAY_SIZE(ValueNameRNTIType,
                                                ValueName),
                                            "(MI)Unknown");
                                    int temp = _search_result_int(
                                            result_subpkt_sample, "Sub-FN");
                                    int iSubFN = temp & 15; // 4 bits
                                    int iSFN = (temp >> 4);
                                    PyObject *old_object = _replace_result_int(
                                            result_subpkt_sample,
                                            "Sub-FN", iSubFN);
                                    Py_DECREF(old_object);
                                    old_object = _replace_result_int(
                                            result_subpkt_sample,
                                            "SFN", iSFN);
                                    Py_DECREF(old_object);
                                    offset += _search_result_int(result_subpkt_sample,
                                                "HDR LEN");
                                    PyObject *t = Py_BuildValue("(sOs)",
                                            "Ignored", result_subpkt_sample, "dict");
                                    PyList_Append(result_sample_list, t);
                                    Py_DECREF(result_subpkt_sample);
                                    Py_DECREF(t);
                                }
                                success = true;
                                break;
                            }
                        case 2:
                            {
                                // UL Transport Block Subpacket V2
                                for (int j = 0; j < subpkt_nsample; j++) {
                                    PyObject *result_subpkt_sample = PyList_New(0);
                                    offset += _decode_by_fmt(LteMacULTransportBlock_SubpktV2_SampleFmt,
                                            ARRAY_SIZE(LteMacULTransportBlock_SubpktV2_SampleFmt, Fmt),
                                            b, offset, length, result_subpkt_sample);
                                    (void) _map_result_field_to_name(
                                            result_subpkt_sample,
                                            "BSR event",
                                            BSREvent,
                                            ARRAY_SIZE(BSREvent, ValueName),
                                            "(MI)Unknown");
                                    (void) _map_result_field_to_name(
                                            result_subpkt_sample,
                                            "BSR trig",
                                            BSRTrig,
                                            ARRAY_SIZE(BSRTrig, ValueName),
                                            "(MI)Unknown");
                                    (void) _map_result_field_to_name(
                                            result_subpkt_sample,
                                            "RNTI Type",
                                            ValueNameRNTIType,
                                            ARRAY_SIZE(ValueNameRNTIType,
                                                ValueName),
                                            "(MI)Unknown");
                                    int temp = _search_result_int(
                                            result_subpkt_sample, "Sub-FN");
                                    int iSubFN = temp & 15; // 4 bits
                                    int iSFN = (temp >> 4);
                                    PyObject *old_object = _replace_result_int(
                                            result_subpkt_sample,
                                            "Sub-FN", iSubFN);
                                    Py_DECREF(old_object);
                                    old_object = _replace_result_int(
                                            result_subpkt_sample,
                                            "SFN", iSFN);
                                    Py_DECREF(old_object);
                                    offset += _search_result_int(
                                            result_subpkt_sample, "HDR LEN");
                                    PyObject *t = Py_BuildValue("(sOs)",
                                            "Ignored", result_subpkt_sample, "dict");
                                    PyList_Append(result_sample_list, t);
                                    Py_DECREF(result_subpkt_sample);
                                    Py_DECREF(t);
                                }
                                success = true;
                                break;
                            }
                    default:
                        break;
                    }
                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                "Samples", result_sample_list, "list");
                        PyList_Append(result_subpkt, t);
                        Py_DECREF(result_sample_list);
                        Py_DECREF(t);
                    } else {
                        printf("(MI)Unknown LTE MAC Uplink Transport Block Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                    }
                    PyObject *t = Py_BuildValue("(sOs)",
                            "Ignored", result_subpkt, "dict");
                    PyList_Append(result_allpkts, t);
                    Py_DECREF(result_subpkt);
                    Py_DECREF(t);
                }
            }
            PyObject *t = Py_BuildValue("(sOs)",
                                        "Subpackets", result_allpkts, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE MAC Uplink Transport Block packet version: 0x%x\n", pkt_ver);
        return 0;
    }
}

//------------------------------------------------------
// Jie
static int
_decode_lte_mac_dl_transportblock_subpkt(const char *b, int offset, size_t length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMacDLTransportBlock_SubpktHeaderFmt,
                                            ARRAY_SIZE(LteMacDLTransportBlock_SubpktHeaderFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                int subpkt_nsample = _search_result_int(result_subpkt, "Num Samples");
                const char *type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                    ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                    subpkt_id);
                (void) _map_result_field_to_name(
                                result_subpkt,
                                "SubPacket ID",
                                LteMacConfigurationSubpkt_SubpktType,
                                ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                "(MI)Unknown");

                if (type_name == NULL) {    // not found
                    printf("(MI)Unknown LTE MAC Downlink Transport Block Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    PyObject *result_sample_list = PyList_New(0);
                    switch (subpkt_ver) {
                    case 2:
                        {
                            // DL Transport Block Subpacket
                            for (int j = 0; j < subpkt_nsample; j++) {
                                PyObject *result_subpkt_sample = PyList_New(0);
                                offset += _decode_by_fmt(LteMacDLTransportBlock_SubpktV2_SampleFmt,
                                        ARRAY_SIZE(LteMacDLTransportBlock_SubpktV2_SampleFmt, Fmt),
                                        b, offset, length, result_subpkt_sample);
                                (void) _map_result_field_to_name(
                                        result_subpkt_sample,
                                        "RNTI Type",
                                        ValueNameRNTIType,
                                        ARRAY_SIZE(ValueNameRNTIType, ValueName),
                                        "(MI)Unknown");
                                int temp = _search_result_int(
                                        result_subpkt_sample, "Sub-FN");
                                int iSubFN = temp & 15; // 4 bits
                                int iSFN = (temp >> 4);
                                PyObject *old_object = _replace_result_int(
                                        result_subpkt_sample,
                                        "Sub-FN", iSubFN);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(
                                        result_subpkt_sample,
                                        "SFN", iSFN);
                                Py_DECREF(old_object);
                                offset += _search_result_int(result_subpkt_sample, "HDR LEN");
                                PyObject *t = Py_BuildValue("(sOs)",
                                        "Ignored",
                                        result_subpkt_sample, "dict");
                                PyList_Append(result_sample_list, t);
                                Py_DECREF(result_subpkt_sample);
                                Py_DECREF(t);
                            }
                            success = true;
                            break;
                        }
                    case 4:
                        {
                            // DL Transport Block Subpacket
                            for (int j = 0; j < subpkt_nsample; j++) {
                                PyObject *result_subpkt_sample = PyList_New(0);
                                offset += _decode_by_fmt(LteMacDLTransportBlock_SubpktV4_SampleFmt,
                                        ARRAY_SIZE(LteMacDLTransportBlock_SubpktV4_SampleFmt, Fmt),
                                        b, offset, length, result_subpkt_sample);
                                (void) _map_result_field_to_name(
                                        result_subpkt_sample,
                                        "RNTI Type",
                                        ValueNameRNTIType,
                                        ARRAY_SIZE(ValueNameRNTIType, ValueName),
                                        "(MI)Unknown");
                                int temp = _search_result_int(
                                        result_subpkt_sample, "Sub-FN");
                                int iSubFN = temp & 15; // 4 bits
                                int iSFN = (temp >> 4);
                                PyObject *old_object = _replace_result_int(
                                        result_subpkt_sample,
                                        "Sub-FN", iSubFN);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(
                                        result_subpkt_sample,
                                        "SFN", iSFN);
                                Py_DECREF(old_object);
                                offset += _search_result_int(result_subpkt_sample, "HDR LEN");
                                PyObject *t = Py_BuildValue("(sOs)",
                                        "Ignored",
                                        result_subpkt_sample, "dict");
                                PyList_Append(result_sample_list, t);
                                Py_DECREF(result_subpkt_sample);
                                Py_DECREF(t);
                            }
                            success = true;
                            break;
                        }

                    default:
                        break;
                    }
                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                "Samples", result_sample_list, "list");
                        PyList_Append(result_subpkt, t);
                        Py_DECREF(result_sample_list);
                        Py_DECREF(t);
                    } else {
                        printf("(MI)Unknown LTE MAC Downlink Transport Block Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                    }
                    PyObject *t = Py_BuildValue("(sOs)",
                            "Ignored", result_subpkt, "dict");
                    PyList_Append(result_allpkts, t);
                    Py_DECREF(result_subpkt);
                    Py_DECREF(t);
                }
            }
            PyObject *t = Py_BuildValue("(sOs)",
                                        "Subpackets", result_allpkts, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE MAC Downlink Transport Block packet version: 0x%x\n", pkt_ver);
        return 0;
    }
}

//------------------------------------------------------
// Jie
static int
_decode_lte_mac_ul_bufferstatusinternal_subpkt(const char *b, int offset, size_t length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    PyObject *old_object;
    switch (pkt_ver) {
        case 1:{
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMacULBufferStatusInternal_SubpktHeaderFmt,
                                            ARRAY_SIZE(LteMacDLTransportBlock_SubpktHeaderFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                int subpkt_nsample = _search_result_int(result_subpkt, "Num Samples");
                const char *type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                    ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                    subpkt_id);
                (void) _map_result_field_to_name(
                                result_subpkt,
                                "SubPacket ID",
                                LteMacConfigurationSubpkt_SubpktType,
                                ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                "(MI)Unknown Subpkt Type");

                if (type_name == NULL) {    // not found
                    printf("(MI)Unknown LTE MAC Uplink Buffer Status Internel Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    switch (subpkt_ver) {
                        case 3: {
                            // UL Buffer Status SubPacket v3
                            PyObject *result_subpkt_allsamples = PyList_New(0);

                            for (int j = 0; j < subpkt_nsample; j++) {
                                PyObject *result_subpkt_sample = PyList_New(0);
                                offset += _decode_by_fmt(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt,
                                        ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt, Fmt),
                                        b, offset, length, result_subpkt_sample);
                                int num_active_lcid = _search_result_int(result_subpkt_sample, "Number of active LCID");
                                int temp = _search_result_int(result_subpkt_sample, "Sub FN");
                                int iSubFN = temp & 15;
                                int iSysFN = (temp >> 4) & 1023;
                                old_object = _replace_result_int(result_subpkt_sample,
                                        "Sub FN", iSubFN);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(result_subpkt_sample,
                                        "Sys FN", iSysFN);
                                Py_DECREF(old_object);

                                PyObject *result_subpkt_sample_alllcids = PyList_New(0);

                                for (int k = 0; k < num_active_lcid; k++) {
                                    PyObject *result_subpkt_sample_lcid = PyList_New(0);
                                    offset += _decode_by_fmt(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt,
                                            ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt, Fmt),
                                            b, offset, length, result_subpkt_sample_lcid);
                                    PyObject *t4 = Py_BuildValue("(sOs)",
                                                            "Ignored", result_subpkt_sample_lcid, "dict");
                                    PyList_Append(result_subpkt_sample_alllcids, t4);
                                    Py_DECREF(t4);
                                    Py_DECREF(result_subpkt_sample_lcid);
                                }
                                PyObject *t3 = Py_BuildValue("(sOs)",
                                        "LCIDs",
                                        result_subpkt_sample_alllcids,
                                        "list");
                                PyList_Append(result_subpkt_sample, t3);
                                Py_DECREF(t3);
                                Py_DECREF(result_subpkt_sample_alllcids);

                                PyObject *t2 = Py_BuildValue("(sOs)",
                                        "Ignored",
                                        result_subpkt_sample,
                                        "dict");
                                PyList_Append(result_subpkt_allsamples, t2);
                                Py_DECREF(t2);
                                Py_DECREF(result_subpkt_sample);
                            }
                            PyObject *t1 = Py_BuildValue("(sOs)",
                                    "Samples",
                                    result_subpkt_allsamples, "list");
                            PyList_Append(result_subpkt, t1);
                            Py_DECREF(t1);
                            Py_DECREF(result_subpkt_allsamples);

                            success = true;
                            break;
                        }
                        case 24:{
                            PyObject *result_subpkt_allsamples = PyList_New(0);
                            for (int j = 0; j < subpkt_nsample; j++) {
                                PyObject *result_subpkt_sample = PyList_New(0);
                                offset += _decode_by_fmt(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt_v24,
                                        ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_SampleFmt_v24, Fmt),
                                        b, offset, length, result_subpkt_sample);
                                int num_active_lcid = _search_result_int(result_subpkt_sample, "Number of active LCID");
                                int temp = _search_result_int(result_subpkt_sample, "Sub FN");
                                int iSubFN = temp & 15;
                                int iSysFN = (temp >> 4) & 1023;
                                old_object = _replace_result_int(result_subpkt_sample,
                                        "Sub FN", iSubFN);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(result_subpkt_sample,
                                        "Sys FN", iSysFN);
                                Py_DECREF(old_object);

                                PyObject *result_subpkt_sample_alllcids = PyList_New(0);
                                for (int k = 0; k < num_active_lcid; k++) {
                                    PyObject *result_subpkt_sample_lcid = PyList_New(0);
                                    offset += _decode_by_fmt(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt_v24,
                                            ARRAY_SIZE(LteMacULBufferStatusInternal_ULBufferStatusSubPacket_LCIDFmt_v24, Fmt),
                                            b, offset, length, result_subpkt_sample_lcid);
                                    unsigned int iNewUncompressedBytes = _search_result_uint(result_subpkt_sample_lcid, "New Uncompressed Bytes");
                                    unsigned int iNewCompressedBytes = _search_result_uint(result_subpkt_sample_lcid, "New Compressed Bytes");
                                    unsigned int iRetxBytes = _search_result_uint(result_subpkt_sample_lcid, "Retx bytes");
                                    int iCtrlBytes = _search_result_int(result_subpkt_sample_lcid, "Ctrl bytes");
                                    int iTotalBytes = (int)iNewUncompressedBytes + (int)iNewCompressedBytes + (int)iRetxBytes + iCtrlBytes;
                                    old_object = _replace_result_int(result_subpkt_sample_lcid,
                                            "Total Bytes", iTotalBytes);
                                    Py_DECREF(old_object);
                                    PyObject *t4 = Py_BuildValue("(sOs)",
                                                            "Ignored", result_subpkt_sample_lcid, "dict");
                                    PyList_Append(result_subpkt_sample_alllcids, t4);
                                    Py_DECREF(t4);
                                    Py_DECREF(result_subpkt_sample_lcid);
                                }
                                PyObject *t3 = Py_BuildValue("(sOs)",
                                        "LCIDs",
                                        result_subpkt_sample_alllcids,
                                        "list");
                                PyList_Append(result_subpkt_sample, t3);
                                Py_DECREF(t3);
                                Py_DECREF(result_subpkt_sample_alllcids);

                                PyObject *t2 = Py_BuildValue("(sOs)",
                                        "Ignored",
                                        result_subpkt_sample,
                                        "dict");
                                PyList_Append(result_subpkt_allsamples, t2);
                                Py_DECREF(t2);
                                Py_DECREF(result_subpkt_sample);
                            }
                            PyObject *t1 = Py_BuildValue("(sOs)",
                                    "Samples",
                                    result_subpkt_allsamples, "list");
                            PyList_Append(result_subpkt, t1);
                            Py_DECREF(t1);
                            Py_DECREF(result_subpkt_allsamples);

                            success = true;
                            break;
                        }
                        default:
                            break;
                    }
                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "MAC Subpacket", result_subpkt, "dict");
                        PyList_Append(result_allpkts, t);
                        Py_DECREF(t);
                    } else {
                        printf("(MI)Unknown LTE MAC Uplink Buffer Status Internel Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                    }
                }

                Py_DECREF(result_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)",
                                        "Subpackets", result_allpkts, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
        default:
            printf("(MI)Unknown LTE MAC Uplink Buffer Status Internal packet version: 0x%x\n", pkt_ver);
            return 0;
    }

    return 0;
}

//------------------------------------------------------
// Jie
static int
_decode_lte_mac_ul_txstatistics_subpkt(const char *b, int offset, size_t length,
                        PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
        case 1:{
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                // Decode subpacket header
                offset += _decode_by_fmt(LteMacULTxStatistics_SubpktHeaderFmt,
                                            ARRAY_SIZE(LteMacULTxStatistics_SubpktHeaderFmt, Fmt),
                                            b, offset, length, result_subpkt);
                // Decode payload
                int subpkt_id = _search_result_int(result_subpkt, "SubPacket ID");
                int subpkt_ver = _search_result_int(result_subpkt, "Version");
                const char *type_name = search_name(LteMacConfigurationSubpkt_SubpktType,
                                                    ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                                    subpkt_id);
                (void) _map_result_field_to_name(
                                result_subpkt,
                                "SubPacket ID",
                                LteMacConfigurationSubpkt_SubpktType,
                                ARRAY_SIZE(LteMacConfigurationSubpkt_SubpktType, ValueName),
                                "(MI)Unknown Subpkt Type");

                if (type_name == NULL) {    // not found
                    printf("(MI)Unknown LTE MAC Uplink Tx Statistics Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    switch (subpkt_ver) {
                        case 1: // UL Tx Stats SubPacket version 1
                            {
                                PyObject *result_subpkt_sample = PyList_New(0);
                                offset += _decode_by_fmt(LteMacULTxStatistics_ULTxStatsSubPacketFmt,
                                        ARRAY_SIZE(LteMacULTxStatistics_ULTxStatsSubPacketFmt, Fmt),
                                        b, offset, length, result_subpkt_sample);

                                PyObject *t = Py_BuildValue("(sOs)",
                                                            "Sample", result_subpkt_sample, "dict");
                                PyList_Append(result_subpkt, t);
                                Py_DECREF(result_subpkt_sample);
                                success = true;
                                break;
                            }
                        case 2: // UL Tx Stats SubPacket version 2
                            {
                                PyObject *result_subpkt_sample = PyList_New(0);
                                offset += _decode_by_fmt(LteMacULTxStatistics_ULTxStatsSubPacketFmtV2,
                                        ARRAY_SIZE(LteMacULTxStatistics_ULTxStatsSubPacketFmtV2, Fmt),
                                        b, offset, length, result_subpkt_sample);

                                PyObject *t = Py_BuildValue("(sOs)",
                                                            "Sample", result_subpkt_sample, "dict");
                                PyList_Append(result_subpkt, t);
                                Py_DECREF(result_subpkt_sample);
                                success = true;
                                break;
                            }

                        default:
                            break;
                    }
                    if (success) {
                        PyObject *t = Py_BuildValue("(sOs)",
                                                    "MAC Subpacket", result_subpkt, "dict");
                        PyList_Append(result_allpkts, t);
                        Py_DECREF(t);
                        Py_DECREF(result_subpkt);
                    } else {
                        Py_DECREF(result_subpkt);
                        printf("(MI)Unknown LTE MAC Uplink Tx Statistics Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                    }


                }
            }
            PyObject *t = Py_BuildValue("(sOs)",
                                        "Subpackets", result_allpkts, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
        default:
            printf("(MI)Unknown LTE MAC Uplink Tx Statistics packet version: 0x%x\n", pkt_ver);
            return 0;
    }
}


// ----------------------------------------------------------------------------
static int _decode_lte_rlc_ul_config_log_packet_subpkt (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // Decode subpacket header
                offset += _decode_by_fmt(LteRlcUlConfigLogPacket_SubpktHeader,
                        ARRAY_SIZE(LteRlcUlConfigLogPacket_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 69 && subpkt_ver == 1) {
                    // 69 means lte rlc ul config log packet
                    offset += _decode_by_fmt(
                            LteRlcUlConfigLogPacket_SubpktPayload,
                            ARRAY_SIZE(LteRlcUlConfigLogPacket_SubpktPayload,
                                Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Reason",
                            LteRlcUlConfigLogPacket_Subpkt_Reason,
                            ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_Reason,
                                ValueName),
                            "(MI)Unknown");
                    int maxSizeRbs = _search_result_int(result_subpkt,
                            "Max Size RBs");

                    // Released RB Struct
                    int start_ReleasedRBStruct = offset;
                    offset += _decode_by_fmt(
                            LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Header,
                            ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ReleasedRB = _search_result_int(result_subpkt,
                            "Number of Released RBs");
                    PyObject *result_ReleasedRB = PyList_New(0);
                    for (int j = 0; j < num_ReleasedRB; j++) {
                        PyObject *result_ReleasedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Fmt,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ReleasedRB_Fmt,
                                    Fmt),
                                b, offset, length, result_ReleasedRB_item);
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_ReleasedRB_item, "dict");
                        PyList_Append(result_ReleasedRB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_ReleasedRB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "Released RBs",
                            result_ReleasedRB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_ReleasedRB);
                    offset += 1 + maxSizeRbs * 1 -
                        (offset - start_ReleasedRBStruct);

                    // Added Modified RB Struct
                    int start_AddedModifiedRBStruct = offset;
                    offset += _decode_by_fmt(LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Header,
                            ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_AddedModifiedRB = _search_result_int(result_subpkt,
                            "Number of Added/Modified RBs");
                    PyObject *result_AddedModifiedRB = PyList_New(0);
                    for (int j = 0; j < num_AddedModifiedRB; j++) {
                        PyObject *result_AddedModifiedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt,
                                    Fmt),
                                b, offset, length, result_AddedModifiedRB_item);
                        (void) _map_result_field_to_name(result_AddedModifiedRB_item,
                                "Action",
                                LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Action,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_AddedModifiedRB_Action,
                                    ValueName),
                                "(MI)Unknown");
                        PyObject *t2 = Py_BuildValue("(sOs)", "Ignored",
                                result_AddedModifiedRB_item, "dict");
                        PyList_Append(result_AddedModifiedRB, t2);
                        Py_DECREF(t2);
                        Py_DECREF(result_AddedModifiedRB_item);
                    }
                    PyObject *t2 = Py_BuildValue("(sOs)", "Added/Modified RBs",
                            result_AddedModifiedRB, "list");
                    PyList_Append(result_subpkt, t2);
                    Py_DECREF(t2);
                    Py_DECREF(result_AddedModifiedRB);
                    offset += 1 + maxSizeRbs * 2 -
                        (offset - start_AddedModifiedRBStruct);

                    // Active RB info
                    offset += _decode_by_fmt(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Header,
                            ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ActiveRB = _search_result_int(result_subpkt,
                            "Number of Active RBs");
                    PyObject *result_ActiveRB = PyList_New(0);
                    for (int j = 0; j < num_ActiveRB; j++) {
                        PyObject *result_ActiveRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Fmt,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_Fmt,
                                    Fmt),
                                b, offset, length, result_ActiveRB_item);
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB Mode",
                                LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBMode,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBMode,
                                    ValueName),
                                "(MI)Unknown");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB Type",
                                LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBType,
                                ARRAY_SIZE(LteRlcUlConfigLogPacket_Subpkt_ActiveRB_RBType,
                                    ValueName),
                                "(MI)Unknown");
                        PyObject *pystr = Py_BuildValue("s", "RLCUL CFG");
                        PyObject *old_object = _replace_result(result_ActiveRB_item,
                                "RLCUL CFG", pystr);
                        Py_DECREF(old_object);
                        Py_DECREF(pystr);

                        old_object = _replace_result_int(result_ActiveRB_item,
                                "SN Length", 10);
                        Py_DECREF(old_object);

                        PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                result_ActiveRB_item, "dict");
                        PyList_Append(result_ActiveRB, t3);
                        Py_DECREF(t3);
                        Py_DECREF(result_ActiveRB_item);
                    }
                    PyObject *t3 = Py_BuildValue("(sOs)", "Active RBs",
                            result_ActiveRB, "list");
                    PyList_Append(result_subpkt, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_ActiveRB);
                } else {
                    printf("(MI)Unknown LTE RLC UL Config Log Packet subpkt id"
                            "and version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE RLC UL Config Log Packet version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_rlc_dl_config_log_packet_subpkt (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num SubPkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // Decode subpacket header
                offset += _decode_by_fmt(LteRlcDlConfigLogPacket_SubpktHeader,
                        ARRAY_SIZE(LteRlcDlConfigLogPacket_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 64 && subpkt_ver == 1) {
                    // 64 means lte rlc dl config log packet
                    offset += _decode_by_fmt(
                            LteRlcDlConfigLogPacket_SubpktPayload,
                            ARRAY_SIZE(LteRlcDlConfigLogPacket_SubpktPayload,
                                Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Reason",
                            LteRlcDlConfigLogPacket_Subpkt_Reason,
                            ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_Reason,
                                ValueName),
                            "(MI)Unknown");
                    int maxSizeRbs = _search_result_int(result_subpkt,
                            "Max Size RBs");

                    // Released RB Struct
                    int start_ReleasedRBStruct = offset;
                    offset += _decode_by_fmt(
                            LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Header,
                            ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ReleasedRB = _search_result_int(result_subpkt,
                            "Number of Released RBs");
                    PyObject *result_ReleasedRB = PyList_New(0);
                    for (int j = 0; j < num_ReleasedRB; j++) {
                        PyObject *result_ReleasedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Fmt,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ReleasedRB_Fmt,
                                    Fmt),
                                b, offset, length, result_ReleasedRB_item);
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_ReleasedRB_item, "dict");
                        PyList_Append(result_ReleasedRB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_ReleasedRB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "Released RBs",
                            result_ReleasedRB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_ReleasedRB);
                    offset += 1 + maxSizeRbs * 1 -
                        (offset - start_ReleasedRBStruct);

                    // Added Modified RB Struct
                    int start_AddedModifiedRBStruct = offset;
                    offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Header,
                            ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_AddedModifiedRB = _search_result_int(result_subpkt,
                            "Number of Added/Modified RBs");
                    PyObject *result_AddedModifiedRB = PyList_New(0);
                    for (int j = 0; j < num_AddedModifiedRB; j++) {
                        PyObject *result_AddedModifiedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Fmt,
                                    Fmt),
                                b, offset, length, result_AddedModifiedRB_item);
                        (void) _map_result_field_to_name(result_AddedModifiedRB_item,
                                "Action",
                                LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Action,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_AddedModifiedRB_Action,
                                    ValueName),
                                "(MI)Unknown");
                        PyObject *t2 = Py_BuildValue("(sOs)", "Ignored",
                                result_AddedModifiedRB_item, "dict");
                        PyList_Append(result_AddedModifiedRB, t2);
                        Py_DECREF(t2);
                        Py_DECREF(result_AddedModifiedRB_item);
                    }
                    PyObject *t2 = Py_BuildValue("(sOs)", "Added/Modified RBs",
                            result_AddedModifiedRB, "list");
                    PyList_Append(result_subpkt, t2);
                    Py_DECREF(t2);
                    Py_DECREF(result_AddedModifiedRB);
                    offset += 1 + maxSizeRbs * 2 -
                        (offset - start_AddedModifiedRBStruct);

                    // Active RB info
                    offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Header,
                            ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ActiveRB = _search_result_int(result_subpkt,
                            "Number of Active RBs");
                    PyObject *result_ActiveRB = PyList_New(0);
                    for (int j = 0; j < num_ActiveRB; j++) {
                        PyObject *result_ActiveRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Fmt,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_Fmt,
                                    Fmt),
                                b, offset, length, result_ActiveRB_item);
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB Mode",
                                LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBMode,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBMode,
                                    ValueName),
                                "(MI)Unknown");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB Type",
                                LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBType,
                                ARRAY_SIZE(LteRlcDlConfigLogPacket_Subpkt_ActiveRB_RBType,
                                    ValueName),
                                "(MI)Unknown");
                        PyObject *pystr = Py_BuildValue("s", "RLCDL CFG");
                        PyObject *old_object = _replace_result(result_ActiveRB_item,
                                "RLCDL CFG", pystr);
                        Py_DECREF(old_object);
                        Py_DECREF(pystr);

                        old_object = _replace_result_int(result_ActiveRB_item,
                                "SN Length", 10);
                        Py_DECREF(old_object);

                        PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                result_ActiveRB_item, "dict");
                        PyList_Append(result_ActiveRB, t3);
                        Py_DECREF(t3);
                        Py_DECREF(result_ActiveRB_item);
                    }
                    PyObject *t3 = Py_BuildValue("(sOs)", "Active RBs",
                            result_ActiveRB, "list");
                    PyList_Append(result_subpkt, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_ActiveRB);
                } else {
                    printf("(MI)Unknown LTE RLC DL Config Log Packet subpkt id"
                            "and version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE RLC DL Config Log Packet version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_rlc_ul_am_all_pdu_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Number of Subpackets");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                offset += _decode_by_fmt(LteRlcUlAmAllPdu_SubpktHeader,
                        ARRAY_SIZE(LteRlcUlAmAllPdu_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 70 && subpkt_ver == 3) {
                    // 70 means LTE RLC UL AM All PDU
                    offset += _decode_by_fmt(LteRlcUlAmAllPdu_SubpktPayload,
                            ARRAY_SIZE(LteRlcUlAmAllPdu_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    int rb_cfg_idx = _search_result_int(result_subpkt,
                            "RB Cfg Idx");
                    (void) _map_result_field_to_name(result_subpkt, "RB Mode",
                            LteRlcUlAmAllPdu_Subpkt_RBMode,
                            ARRAY_SIZE(LteRlcUlAmAllPdu_Subpkt_RBMode,
                            ValueName), "(MI)Unknown");
                    int enabled_pdu = _search_result_int(result_subpkt,
                            "Enabled PDU Log Packets");
                    // Need to check bit by bit
                    std::string strEnabledPDU = "";
                    if ((enabled_pdu) & (1 << (1)))
                        strEnabledPDU += "RLCUL Config (0xB091), ";
                    if ((enabled_pdu) & (1 << (2)))
                        strEnabledPDU += "RLCUL AM ALL PDU (0xB092), ";
                    if ((enabled_pdu) & (1 << (3)))
                        strEnabledPDU += "RLCUL AM CONTROL PDU (0xB093), ";
                    if ((enabled_pdu) & (1 << (4)))
                        strEnabledPDU += "RLCUL AM POLLING PDU (0xB094), ";
                    if ((enabled_pdu) & (1 << (5)))
                        strEnabledPDU += "RLCUL AM SIGNALING PDU (0xB095), ";
                    if ((enabled_pdu) & (1 << (6)))
                        strEnabledPDU += "RLCUL UM DATA PDU (0xB096), ";
                    if ((enabled_pdu) & (1 << (7)))
                        strEnabledPDU += "RLCUL STATISTICS (0xB097), ";
                    if ((enabled_pdu) & (1 << (8)))
                        strEnabledPDU += "RLCUL AM STATE (0xB098), ";
                    if ((enabled_pdu) & (1 << (9)))
                        strEnabledPDU += "RLCUL UM STATE (0xB099), ";
                    if (strEnabledPDU.length() > 0) {
                        strEnabledPDU = strEnabledPDU.substr(0,
                                strEnabledPDU.length() - 2);
                        strEnabledPDU += ".";
                    }
                    PyObject *pystr = Py_BuildValue("s",
                            strEnabledPDU.c_str());
                    PyObject *old_object = _replace_result(result_subpkt,
                            "Enabled PDU Log Packets", pystr);
                    Py_DECREF(old_object);
                    Py_DECREF(pystr);

                    int n_pdu = _search_result_int(result_subpkt,
                            "Number of PDUs");
                    PyObject *result_pdu = PyList_New(0);
                    for (int j = 0; j < n_pdu; j++) {
                        PyObject *result_pdu_item = PyList_New(0);
                        offset += _decode_by_fmt(
                                LteRlcUlAmAllPdu_Subpkt_PDU_Basic, ARRAY_SIZE(
                                    LteRlcUlAmAllPdu_Subpkt_PDU_Basic, Fmt),
                                b, offset, length, result_pdu_item);
                        // Update rb_cfg_idx from previous content
                        old_object = _replace_result_int(result_pdu_item,
                                "rb_cfg_idx", rb_cfg_idx);
                        Py_DECREF(old_object);
                        int DCLookAhead = _search_result_int(result_pdu_item,
                                "D/C LookAhead");
                        int iNonDecodeFN = _search_result_int(result_pdu_item,
                                "sys_fn");
                        int iLoggedBytes = _search_result_int(result_pdu_item,
                                "logged_bytes");
                        // D/C LookAhead and SN (or Ack_SN) has already been parsed.
                        iLoggedBytes -= 2;

                        // Handle fn
                        const unsigned int SFN_RSHIFT = 4,
                              SFN_MASK = (1 << 12) - 1;
                        const unsigned int SUBFRAME_RSHIFT = 0,
                              SUBFRAME_MASK = (1 << 4) - 1;
                        int sys_fn = (iNonDecodeFN >> SFN_RSHIFT) & SFN_MASK;
                        int sub_fn =
                            (iNonDecodeFN >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
                        old_object = _replace_result_int(result_pdu_item,
                                "sys_fn", sys_fn);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_pdu_item,
                                "sub_fn", sub_fn);
                        Py_DECREF(old_object);
                        // Decide PDU type
                        int iNonDecodeSN = _search_result_int(result_pdu_item, "SN");
                        if (DCLookAhead < 16) {
                            // Type = Control
                            pystr = Py_BuildValue("s", "RLCUL CTRL");
                            old_object = _replace_result(result_pdu_item,
                                    "PDU TYPE", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Modify ACK_SN
                            std::string strAckSN = "ACK_SN = ";
                            int iAckSN = DCLookAhead * 64 + iNonDecodeSN/4;
                            strAckSN += SSTR(iAckSN);
                            int iHeadFromPadding = (iNonDecodeSN & 1) * 512;
                            pystr = Py_BuildValue("s", strAckSN.c_str());
                            old_object = _replace_result(result_pdu_item,
                                    "SN", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Update other info
                            offset += _decode_by_fmt(
                                    LteRlcUlAmAllPdu_Subpkt_PDU_Control,
                                    ARRAY_SIZE(
                                        LteRlcUlAmAllPdu_Subpkt_PDU_Control,
                                        Fmt),
                                    b, offset, length, result_pdu_item);
                            pystr = Py_BuildValue("s", "STATUS(0)");
                            old_object = _replace_result(result_pdu_item,
                                    "cpt", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);

                            if (iLoggedBytes > 0) {
                                // Decode NACK
                                int numNack = iLoggedBytes / 1.5;
                                int iAllign = 0;
                                int iHeadFromAllign = 0;

                                PyObject *result_pdu_nack = PyList_New(0);
                                for (int indexNack = 0; indexNack < numNack; indexNack++) {
                                    PyObject *result_pdu_nack_item = PyList_New(0);
                                    if (iAllign == 0) {
                                        iAllign = 1;
                                        offset += _decode_by_fmt(
                                                LteRlcUlAmAllPdu_Subpkt_PDU_NACK_ALLIGN,
                                                ARRAY_SIZE(
                                                    LteRlcUlAmAllPdu_Subpkt_PDU_NACK_ALLIGN,
                                                Fmt),
                                                b, offset, length, result_pdu_nack_item);
                                        iLoggedBytes -= 2;
                                        int iNonDecodeNACK = _search_result_int(
                                                result_pdu_nack_item, "NACK_SN");
                                        int iPart3 = iNonDecodeNACK / 4096;
                                        int iPart4 = (iNonDecodeNACK - iPart3 * 4096) / 256;
                                        int iPart1 = (iNonDecodeNACK - iPart3 * 4096 - iPart4 * 256) / 16;
                                        int iPart2 = iNonDecodeNACK - iPart3 * 4096 - iPart4 * 256 - iPart1 * 16;
                                        int iNack = iPart1 * 32 + iPart2 * 2 + iPart3 / 8 + iHeadFromPadding;
                                        iHeadFromAllign = iPart4 + (iPart3 & 1) * 16;
                                        int iE2 = iPart3 & 2;
                                        if (iE2 == 2) {
                                            indexNack = numNack - 1;
                                        }
                                        old_object = _replace_result_int(
                                                result_pdu_nack_item, "NACK_SN", iNack);
                                        Py_DECREF(old_object);
                                    } else {
                                        iAllign = 0;
                                        offset += _decode_by_fmt(
                                                LteRlcUlAmAllPdu_Subpkt_PDU_NACK_PADDING,
                                                ARRAY_SIZE(
                                                    LteRlcUlAmAllPdu_Subpkt_PDU_NACK_PADDING,
                                                Fmt),
                                                b, offset, length, result_pdu_nack_item);
                                        iLoggedBytes -= 1;
                                        int iNonDecodeNACK = _search_result_int(
                                                result_pdu_nack_item, "NACK_SN");
                                        int iNack = iHeadFromAllign * 32 + iNonDecodeNACK / 8;
                                        int iE2 = iNonDecodeNACK & 2;
                                        if (iE2 == 2) {
                                            indexNack = numNack - 1;
                                        }
                                        iHeadFromPadding = (iNonDecodeNACK & 1) * 512;
                                        old_object = _replace_result_int(
                                                result_pdu_nack_item, "NACK_SN", iNack);
                                        Py_DECREF(old_object);
                                    }
                                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                            result_pdu_nack_item, "dict");
                                    PyList_Append(result_pdu_nack, t3);
                                    Py_DECREF(t3);
                                    Py_DECREF(result_pdu_nack_item);
                                }
                                PyObject *t2 = Py_BuildValue("(sOs)", "RLC CTRL NACK",
                                        result_pdu_nack, "list");
                                PyList_Append(result_pdu_item, t2);
                                Py_DECREF(t2);
                                Py_DECREF(result_pdu_nack);
                            }
                        } else {
                            // Type = DATA
                            pystr = Py_BuildValue("s", "RLCUL DATA");
                            old_object = _replace_result(result_pdu_item,
                                    "PDU TYPE", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Update other info
                            std::string strRF = "", strP = "", strFI = "",
                                strE = "";
                            if ((DCLookAhead) & (1 << (6))) {
                                strRF += "1";
                            } else {
                                strRF += "0";
                            }
                            if ((DCLookAhead) & (1 << (5))) {
                                strP += "1";
                            } else {
                                strP += "0";
                            }
                            if ((DCLookAhead) & (1 << (4))) {
                                strFI += "1";
                            } else {
                                strFI += "0";
                            }
                            if ((DCLookAhead) & (1 << (3))) {
                                strFI += "1";
                            } else {
                                strFI += "0";
                            }
                            if ((DCLookAhead) & (1 << (2))) {
                                strE += "1";
                            } else {
                                strE += "0";
                            }
                            // update SN (need to check last two bits in
                            // DCLookAhead)
                            if ((DCLookAhead) & (1 << (1))) {
                                iNonDecodeSN += 512;
                            }
                            if ((DCLookAhead) & (1)) {
                                iNonDecodeSN += 256;
                            }
                            old_object = _replace_result_int(result_pdu_item,
                                    "SN", iNonDecodeSN);
                            Py_DECREF(old_object);

                            offset += _decode_by_fmt(
                                    LteRlcUlAmAllPdu_Subpkt_PDU_DATA,
                                    ARRAY_SIZE(
                                        LteRlcUlAmAllPdu_Subpkt_PDU_DATA, Fmt),
                                    b, offset, length, result_pdu_item);
                            pystr = Py_BuildValue("s", (strRF.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "RF", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strP.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "P", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strFI.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "FI", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strE.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "E", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);

                            if (strRF == "1") {
                                // decode LSF and SO
                                iLoggedBytes -= 2;

                                offset += _decode_by_fmt(
                                        LteRlcUlAmAllPdu_Subpkt_PDU_LSF_SO,
                                        ARRAY_SIZE(
                                            LteRlcUlAmAllPdu_Subpkt_PDU_LSF_SO,
                                            Fmt),
                                        b, offset, length, result_pdu_item);
                                int temp = _search_result_int(result_pdu_item,
                                        "LSF");
                                int iLSF = temp >> 7;
                                int iSO = _search_result_int(result_pdu_item,
                                        "SO");
                                iSO += (temp & 127) * 256;
                                old_object = _replace_result_int(
                                        result_pdu_item, "LSF", iLSF);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(
                                        result_pdu_item, "SO", iSO);
                                Py_DECREF(old_object);
                            }

                            if (strE == "1") {
                                // Decode LI
                                int numLI = iLoggedBytes / 1.5;
                                iLoggedBytes = 0;
                                int iAllign = 0;
                                int iHeadFromAllign = 0;

                                PyObject *result_pdu_li = PyList_New(0);
                                for (int indexLI = 0; indexLI < numLI; indexLI++) {
                                    PyObject *result_pdu_li_item = PyList_New(0);
                                    if (iAllign == 0) {
                                        iAllign = 1;
                                        offset += _decode_by_fmt(
                                                LteRlcUlAmAllPdu_Subpkt_PDU_LI_ALLIGN,
                                                ARRAY_SIZE(
                                                    LteRlcUlAmAllPdu_Subpkt_PDU_LI_ALLIGN,
                                                Fmt),
                                                b, offset, length, result_pdu_li_item);
                                        int iNonDecodeLI = _search_result_int(
                                                result_pdu_li_item, "LI");
                                        int iPart3 = iNonDecodeLI / 4096;
                                        int iPart4 = (iNonDecodeLI - iPart3 * 4096) / 256;
                                        int iPart1 = (iNonDecodeLI - iPart3 * 4096 - iPart4 * 256) / 16;
                                        int iPart2 = iNonDecodeLI - iPart3 * 4096 - iPart4 * 256 - iPart1 * 16;
                                        int iLI = (iPart1 % 8) * 256 + iPart2 * 16 + iPart3;
                                        iHeadFromAllign = (iPart4 % 8) * 256;
                                        old_object = _replace_result_int(
                                                result_pdu_li_item, "LI", iLI);
                                        Py_DECREF(old_object);
                                    } else {
                                        iAllign = 0;
                                        offset += _decode_by_fmt(
                                                LteRlcUlAmAllPdu_Subpkt_PDU_LI_PADDING,
                                                ARRAY_SIZE(
                                                    LteRlcUlAmAllPdu_Subpkt_PDU_LI_PADDING,
                                                Fmt),
                                                b, offset, length, result_pdu_li_item);
                                        int iNonDecodeLI = _search_result_int(
                                                result_pdu_li_item, "LI");
                                        int iLI = iHeadFromAllign + iNonDecodeLI;
                                        old_object = _replace_result_int(
                                                result_pdu_li_item, "LI", iLI);
                                        Py_DECREF(old_object);
                                    }
                                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                            result_pdu_li_item, "dict");
                                    PyList_Append(result_pdu_li, t3);
                                    Py_DECREF(t3);
                                    Py_DECREF(result_pdu_li_item);
                                }
                                PyObject *t2 = Py_BuildValue("(sOs)", "RLC DATA LI",
                                        result_pdu_li, "list");
                                PyList_Append(result_pdu_item, t2);
                                Py_DECREF(t2);
                                Py_DECREF(result_pdu_li);
                            }
                        }
                        offset += iLoggedBytes;
                        PyObject *t1 = Py_BuildValue("(sOs)", ("RLCUL PDU["
                                + SSTR(i) + "]").c_str(), result_pdu_item,
                                "dict");
                        PyList_Append(result_pdu, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_pdu_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "RLCUL PDUs",
                            result_pdu, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_pdu);
                } else {
                    printf("Unkown LTE RLC UL AM ALL PDU subpkt id and version"
                            ": 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("Unkown LTE RLC UL AM ALL PDU version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_rlc_dl_am_all_pdu_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Number of Subpackets");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                offset += _decode_by_fmt(LteRlcDlAmAllPdu_SubpktHeader,
                        ARRAY_SIZE(LteRlcDlAmAllPdu_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 65 && subpkt_ver == 3) {
                    // 65 means LTE RLC DL AM All PDU
                    offset += _decode_by_fmt(LteRlcDlAmAllPdu_SubpktPayload,
                            ARRAY_SIZE(LteRlcDlAmAllPdu_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    int rb_cfg_idx = _search_result_int(result_subpkt,
                            "RB Cfg Idx");
                    (void) _map_result_field_to_name(result_subpkt, "RB Mode",
                            LteRlcDlAmAllPdu_Subpkt_RBMode,
                            ARRAY_SIZE(LteRlcDlAmAllPdu_Subpkt_RBMode,
                            ValueName), "(MI)Unknown");
                    int enabled_pdu = _search_result_int(result_subpkt,
                            "Enabled PDU Log Packets");
                    // Need to check bit by bit
                    std::string strEnabledPDU = "";
                    if ((enabled_pdu) & (1 << (1)))
                        strEnabledPDU += "RLCDL Config (0xB081), ";
                    if ((enabled_pdu) & (1 << (2)))
                        strEnabledPDU += "RLCDL AM ALL PDU (0xB082), ";
                    if ((enabled_pdu) & (1 << (3)))
                        strEnabledPDU += "RLCDL AM CONTROL PDU (0xB083), ";
                    if ((enabled_pdu) & (1 << (4)))
                        strEnabledPDU += "RLCDL AM POLLING PDU (0xB084), ";
                    if ((enabled_pdu) & (1 << (5)))
                        strEnabledPDU += "RLCDL AM SIGNALING PDU (0xB085), ";
                    if ((enabled_pdu) & (1 << (6)))
                        strEnabledPDU += "RLCDL UM DATA PDU (0xB086), ";
                    if ((enabled_pdu) & (1 << (7)))
                        strEnabledPDU += "RLCDL STATISTICS (0xB087), ";
                    if ((enabled_pdu) & (1 << (8)))
                        strEnabledPDU += "RLCDL AM STATE (0xB088), ";
                    if ((enabled_pdu) & (1 << (9)))
                        strEnabledPDU += "RLCDL UM STATE (0xB089), ";
                    if (strEnabledPDU.length() > 0) {
                        strEnabledPDU = strEnabledPDU.substr(0,
                                strEnabledPDU.length() - 2);
                        strEnabledPDU += ".";
                    }
                    PyObject *pystr = Py_BuildValue("s",
                            strEnabledPDU.c_str());
                    PyObject *old_object = _replace_result(result_subpkt,
                            "Enabled PDU Log Packets", pystr);
                    Py_DECREF(old_object);
                    Py_DECREF(pystr);

                    int n_pdu = _search_result_int(result_subpkt,
                            "Number of PDUs");
                    PyObject *result_pdu = PyList_New(0);
                    for (int j = 0; j < n_pdu; j++) {
                        PyObject *result_pdu_item = PyList_New(0);
                        offset += _decode_by_fmt(
                                LteRlcDlAmAllPdu_Subpkt_PDU_Basic, ARRAY_SIZE(
                                    LteRlcDlAmAllPdu_Subpkt_PDU_Basic, Fmt),
                                b, offset, length, result_pdu_item);
                        // Update rb_cfg_idx from previous content
                        old_object = _replace_result_int(result_pdu_item,
                                "rb_cfg_idx", rb_cfg_idx);
                        Py_DECREF(old_object);
                        int DCLookAhead = _search_result_int(result_pdu_item,
                                "D/C LookAhead");
                        int iNonDecodeFN = _search_result_int(result_pdu_item,
                                "sys_fn");
                        int iLoggedBytes = _search_result_int(result_pdu_item,
                                "logged_bytes");
                        // D/C LookAhead and SN (or Ack_SN) has already been parsed.
                        iLoggedBytes -= 2;

                        // Handle fn
                        const unsigned int SFN_RSHIFT = 4,
                              SFN_MASK = (1 << 12) - 1;
                        const unsigned int SUBFRAME_RSHIFT = 0,
                              SUBFRAME_MASK = (1 << 4) - 1;
                        int sys_fn = (iNonDecodeFN >> SFN_RSHIFT) & SFN_MASK;
                        int sub_fn =
                            (iNonDecodeFN >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
                        old_object = _replace_result_int(result_pdu_item,
                                "sys_fn", sys_fn);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_pdu_item,
                                "sub_fn", sub_fn);
                        Py_DECREF(old_object);
                        // Decide PDU type
                        int iNonDecodeSN = _search_result_int(result_pdu_item, "SN");
                        if (DCLookAhead < 16) {
                            // Type = Control
                            pystr = Py_BuildValue("s", "RLCDL CTRL");
                            old_object = _replace_result(result_pdu_item,
                                    "PDU TYPE", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", "PDU CTRL");
                            old_object = _replace_result(result_pdu_item,
                                    "Status", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Modify ACK_SN
                            std::string strAckSN = "ACK_SN = ";
                            int iAckSN = DCLookAhead * 64 + iNonDecodeSN/4;
                            strAckSN += SSTR(iAckSN);
                            int iHeadFromPadding = (iNonDecodeSN & 1) * 512;
                            pystr = Py_BuildValue("s", strAckSN.c_str());
                            old_object = _replace_result(result_pdu_item,
                                    "SN", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Update other info
                            offset += _decode_by_fmt(
                                    LteRlcDlAmAllPdu_Subpkt_PDU_Control,
                                    ARRAY_SIZE(
                                        LteRlcDlAmAllPdu_Subpkt_PDU_Control,
                                        Fmt),
                                    b, offset, length, result_pdu_item);
                            pystr = Py_BuildValue("s", "STATUS(0)");
                            old_object = _replace_result(result_pdu_item,
                                    "cpt", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);

                            if (iLoggedBytes > 0) {
                                // Decode NACK
                                int numNack = iLoggedBytes / 1.5;
                                int iAllign = 0;
                                int iHeadFromAllign = 0;

                                PyObject *result_pdu_nack = PyList_New(0);
                                for (int indexNack = 0; indexNack < numNack; indexNack++) {
                                    PyObject *result_pdu_nack_item = PyList_New(0);
                                    if (iAllign == 0) {
                                        iAllign = 1;
                                        offset += _decode_by_fmt(
                                                LteRlcDlAmAllPdu_Subpkt_PDU_NACK_ALLIGN,
                                                ARRAY_SIZE(
                                                    LteRlcDlAmAllPdu_Subpkt_PDU_NACK_ALLIGN,
                                                Fmt),
                                                b, offset, length, result_pdu_nack_item);
                                        iLoggedBytes -= 2;
                                        int iNonDecodeNACK = _search_result_int(
                                                result_pdu_nack_item, "NACK_SN");
                                        int iPart3 = iNonDecodeNACK / 4096;
                                        int iPart4 = (iNonDecodeNACK - iPart3 * 4096) / 256;
                                        int iPart1 = (iNonDecodeNACK - iPart3 * 4096 - iPart4 * 256) / 16;
                                        int iPart2 = iNonDecodeNACK - iPart3 * 4096 - iPart4 * 256 - iPart1 * 16;
                                        int iNack = iPart1 * 32 + iPart2 * 2 + iPart3 / 8 + iHeadFromPadding;
                                        iHeadFromAllign = iPart4 + (iPart3 & 1) * 16;
                                        int iE2 = iPart3 & 2;
                                        if (iE2 == 2) {
                                            indexNack = numNack - 1;
                                        }
                                        old_object = _replace_result_int(
                                                result_pdu_nack_item, "NACK_SN", iNack);
                                        Py_DECREF(old_object);
                                    } else {
                                        iAllign = 0;
                                        offset += _decode_by_fmt(
                                                LteRlcDlAmAllPdu_Subpkt_PDU_NACK_PADDING,
                                                ARRAY_SIZE(
                                                    LteRlcDlAmAllPdu_Subpkt_PDU_NACK_PADDING,
                                                Fmt),
                                                b, offset, length, result_pdu_nack_item);
                                        iLoggedBytes -= 1;
                                        int iNonDecodeNACK = _search_result_int(
                                                result_pdu_nack_item, "NACK_SN");
                                        int iNack = iHeadFromAllign * 32 + iNonDecodeNACK / 8;
                                        int iE2 = iNonDecodeNACK & 2;
                                        if (iE2 == 2) {
                                            indexNack = numNack - 1;
                                        }
                                        iHeadFromPadding = (iNonDecodeNACK & 1) * 512;
                                        old_object = _replace_result_int(
                                                result_pdu_nack_item, "NACK_SN", iNack);
                                        Py_DECREF(old_object);
                                    }
                                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                            result_pdu_nack_item, "dict");
                                    PyList_Append(result_pdu_nack, t3);
                                    Py_DECREF(t3);
                                    Py_DECREF(result_pdu_nack_item);
                                }
                                PyObject *t2 = Py_BuildValue("(sOs)", "RLC CTRL NACK",
                                        result_pdu_nack, "list");
                                PyList_Append(result_pdu_item, t2);
                                Py_DECREF(t2);
                                Py_DECREF(result_pdu_nack);
                            }
                        } else {
                            // Type = DATA
                            pystr = Py_BuildValue("s", "RLCDL DATA");
                            old_object = _replace_result(result_pdu_item,
                                    "PDU TYPE", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", "PDU DATA");
                            old_object = _replace_result(result_pdu_item,
                                    "Status", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            // Update other info
                            std::string strRF = "", strP = "", strFI = "",
                                strE = "";
                            if ((DCLookAhead) & (1 << (6))) {
                                strRF += "1";
                            } else {
                                strRF += "0";
                            }
                            if ((DCLookAhead) & (1 << (5))) {
                                strP += "1";
                            } else {
                                strP += "0";
                            }
                            if ((DCLookAhead) & (1 << (4))) {
                                strFI += "1";
                            } else {
                                strFI += "0";
                            }
                            if ((DCLookAhead) & (1 << (3))) {
                                strFI += "1";
                            } else {
                                strFI += "0";
                            }
                            if ((DCLookAhead) & (1 << (2))) {
                                strE += "1";
                            } else {
                                strE += "0";
                            }
                            // update SN (need to check last two bits in
                            // DCLookAhead)
                            if ((DCLookAhead) & (1 << (1))) {
                                iNonDecodeSN += 512;
                            }
                            if ((DCLookAhead) & (1)) {
                                iNonDecodeSN += 256;
                            }
                            old_object = _replace_result_int(result_pdu_item,
                                    "SN", iNonDecodeSN);
                            Py_DECREF(old_object);
                            offset += _decode_by_fmt(
                                    LteRlcDlAmAllPdu_Subpkt_PDU_DATA,
                                    ARRAY_SIZE(
                                        LteRlcDlAmAllPdu_Subpkt_PDU_DATA, Fmt),
                                    b, offset, length, result_pdu_item);
                            pystr = Py_BuildValue("s", (strRF.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "RF", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strP.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "P", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strFI.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "FI", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);
                            pystr = Py_BuildValue("s", (strE.c_str()));
                            old_object = _replace_result(result_pdu_item,
                                    "E", pystr);
                            Py_DECREF(old_object);
                            Py_DECREF(pystr);

                            if (strRF == "1") {
                                // decode LSF and SO
                                iLoggedBytes -= 2;

                                offset += _decode_by_fmt(
                                        LteRlcDlAmAllPdu_Subpkt_PDU_LSF_SO,
                                        ARRAY_SIZE(
                                            LteRlcDlAmAllPdu_Subpkt_PDU_LSF_SO,
                                            Fmt),
                                        b, offset, length, result_pdu_item);
                                int temp = _search_result_int(result_pdu_item,
                                        "LSF");
                                int iLSF = temp >> 7;
                                int iSO = _search_result_int(result_pdu_item,
                                        "SO");
                                iSO += (temp & 127) * 256;
                                old_object = _replace_result_int(
                                        result_pdu_item, "LSF", iLSF);
                                Py_DECREF(old_object);
                                old_object = _replace_result_int(
                                        result_pdu_item, "SO", iSO);
                                Py_DECREF(old_object);
                            }

                            if (strE == "1") {
                                // Decode LI
                                int numLI = iLoggedBytes / 1.5;
                                iLoggedBytes = 0;
                                int iAllign = 0;
                                int iHeadFromAllign = 0;

                                PyObject *result_pdu_li = PyList_New(0);
                                for (int indexLI = 0; indexLI < numLI; indexLI++) {
                                    PyObject *result_pdu_li_item = PyList_New(0);
                                    if (iAllign == 0) {
                                        iAllign = 1;
                                        offset += _decode_by_fmt(
                                                LteRlcDlAmAllPdu_Subpkt_PDU_LI_ALLIGN,
                                                ARRAY_SIZE(
                                                    LteRlcDlAmAllPdu_Subpkt_PDU_LI_ALLIGN,
                                                Fmt),
                                                b, offset, length, result_pdu_li_item);
                                        int iNonDecodeLI = _search_result_int(
                                                result_pdu_li_item, "LI");
                                        int iPart3 = iNonDecodeLI / 4096;
                                        int iPart4 = (iNonDecodeLI - iPart3 * 4096) / 256;
                                        int iPart1 = (iNonDecodeLI - iPart3 * 4096 - iPart4 * 256) / 16;
                                        int iPart2 = iNonDecodeLI - iPart3 * 4096 - iPart4 * 256 - iPart1 * 16;
                                        int iLI = (iPart1 % 8) * 256 + iPart2 * 16 + iPart3;
                                        iHeadFromAllign = (iPart4 % 8) * 256;
                                        old_object = _replace_result_int(
                                                result_pdu_li_item, "LI", iLI);
                                        Py_DECREF(old_object);
                                    } else {
                                        iAllign = 0;
                                        offset += _decode_by_fmt(
                                                LteRlcDlAmAllPdu_Subpkt_PDU_LI_PADDING,
                                                ARRAY_SIZE(
                                                    LteRlcDlAmAllPdu_Subpkt_PDU_LI_PADDING,
                                                Fmt),
                                                b, offset, length, result_pdu_li_item);
                                        int iNonDecodeLI = _search_result_int(
                                                result_pdu_li_item, "LI");
                                        int iLI = iHeadFromAllign + iNonDecodeLI;
                                        old_object = _replace_result_int(
                                                result_pdu_li_item, "LI", iLI);
                                        Py_DECREF(old_object);
                                    }
                                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                            result_pdu_li_item, "dict");
                                    PyList_Append(result_pdu_li, t3);
                                    Py_DECREF(t3);
                                    Py_DECREF(result_pdu_li_item);
                                }
                                PyObject *t2 = Py_BuildValue("(sOs)", "RLC DATA LI",
                                        result_pdu_li, "list");
                                PyList_Append(result_pdu_item, t2);
                                Py_DECREF(t2);
                                Py_DECREF(result_pdu_li);
                            }
                        }
                        offset += iLoggedBytes;
                        PyObject *t1 = Py_BuildValue("(sOs)", ("RLCDL PDU["
                                + SSTR(i) + "]").c_str(), result_pdu_item,
                                "dict");
                        PyList_Append(result_pdu, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_pdu_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "RLCDL PDUs",
                            result_pdu, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_pdu);
                } else {
                    printf("Unkown LTE RLC DL AM ALL PDU subpkt id and version"
                            ": 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("Unkown LTE RLC DL AM ALL PDU version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------

static int _decode_lte_mac_rach_trigger_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Number of Subpackets");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                offset += _decode_by_fmt(LteMacRachTrigger_SubpktHeader,
                        ARRAY_SIZE(LteMacRachTrigger_SubpktHeader, Fmt), b,
                        offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 3 && subpkt_ver == 2) {
                    // RACH Config Subpacket
                    offset += _decode_by_fmt(
                            LteMacRachTrigger_RachConfigSubpktPayload,
                            ARRAY_SIZE(LteMacRachTrigger_RachConfigSubpktPayload,
                                Fmt),
                            b, offset, length, result_subpkt);
                    PyObject *old_object = _replace_result_int(result_subpkt,
                            "Preamble Format", 0);
                    Py_DECREF(old_object);
                } else if (subpkt_id == 5 && subpkt_ver == 1) {
                    offset += _decode_by_fmt(
                            LteMacRachTrigger_RachReasonSubpktPayload,
                            ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpktPayload,
                                Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Rach reason",
                            LteMacRachTrigger_RachReasonSubpkt_RachReason,
                            ARRAY_SIZE(LteMacRachTrigger_RachReasonSubpkt_RachReason,
                                ValueName),
                            "(MI)Unknown");
                    std::string strRachContention = "Contention Based RACH procedure";
                    PyObject *pystr = Py_BuildValue("s", strRachContention.c_str());
                    PyObject *old_object = _replace_result(result_subpkt,
                            "RACH Contention", pystr);
                    Py_DECREF(old_object);
                    Py_DECREF(pystr);
                } else {
                    printf("(MI)Unknown LTE MAC RACH Trigger subpkt id and "
                            "version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE MAC RACH Trigger Packet Version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------

static int _decode_lte_mac_rach_attempt_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Number of Subpackets");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                offset += _decode_by_fmt(LteMacRachAttempt_SubpktHeader,
                        ARRAY_SIZE(LteMacRachAttempt_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 6 && subpkt_ver == 2) {
                    offset += _decode_by_fmt(LteMacRachAttempt_SubpktPayload,
                            ARRAY_SIZE(LteMacRachAttempt_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    int iRachMsgBMasks = _search_result_int(result_subpkt,
                            "Rach msg bmasks");
                    (void) _map_result_field_to_name(result_subpkt,
                            "Rach result",
                            LteMacRachAttempt_Subpkt_RachResult,
                            ARRAY_SIZE(LteMacRachAttempt_Subpkt_RachResult,
                                ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_subpkt,
                            "Contention procedure",
                            LteMacRachAttempt_Subpkt_ContentionProcedure,
                            ARRAY_SIZE(LteMacRachAttempt_Subpkt_ContentionProcedure,
                                ValueName),
                            "(MI)Unknown");
                    if ((iRachMsgBMasks) & (1 << 2)) {
                        // Msg1
                        PyObject *result_subpkt_msg1 = PyList_New(0);
                        offset += _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg1,
                                ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg1, Fmt),
                                b, offset, length, result_subpkt_msg1);
                        PyObject *t1 = Py_BuildValue("(sOs)", "Msg1",
                                result_subpkt_msg1, "dict");
                        PyList_Append(result_subpkt, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_subpkt_msg1);
                    }
                    if ((iRachMsgBMasks) & (1 << 1)) {
                        // Msg2
                        PyObject *result_subpkt_msg2 = PyList_New(0);
                        offset += _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg2,
                                ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg2, Fmt),
                                b, offset, length, result_subpkt_msg2);
                        (void) _map_result_field_to_name(result_subpkt_msg2,
                                "Result",
                                LteMacRachAttempt_Subpkt_Msg2_Result,
                                ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg2_Result,
                                    ValueName),
                                "False");
                        PyObject *t1 = Py_BuildValue("(sOs)", "Msg2",
                                result_subpkt_msg2, "dict");
                        PyList_Append(result_subpkt, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_subpkt_msg2);
                    }
                    if ((iRachMsgBMasks) & (1)) {
                        // Msg3
                        PyObject *result_subpkt_msg3 = PyList_New(0);
                        offset += _decode_by_fmt(LteMacRachAttempt_Subpkt_Msg3,
                                ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg3, Fmt),
                                b, offset, length, result_subpkt_msg3);
                        int iGrantBytes = _search_result_int(result_subpkt_msg3,
                                "Grant");
                        PyObject *result_MACPDUs = PyList_New(0);
                        for (int j = 0; j < iGrantBytes + 1; j++) {
                            PyObject *result_MACPDU_item = PyList_New(0);
                            offset += _decode_by_fmt(
                                    LteMacRachAttempt_Subpkt_Msg3_MACPDU,
                                    ARRAY_SIZE(LteMacRachAttempt_Subpkt_Msg3_MACPDU,
                                        Fmt),
                                    b, offset, length, result_MACPDU_item);
                            PyObject *t2 = Py_BuildValue("(sOs)", "Ignored",
                                    result_MACPDU_item, "dict");
                            PyList_Append(result_MACPDUs, t2);
                            Py_DECREF(t2);
                            Py_DECREF(result_MACPDU_item);
                        }
                        // add pdu list
                        PyObject *t2 = Py_BuildValue("(sOs)", "MAC PDUs",
                                result_MACPDUs, "list");
                        PyList_Append(result_subpkt_msg3, t2);
                        Py_DECREF(t2);
                        Py_DECREF(result_MACPDUs);
                        // add Msg3 dict
                        PyObject *t1 = Py_BuildValue("(sOs)", "Msg3",
                                result_subpkt_msg3, "dict");
                        PyList_Append(result_subpkt, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_subpkt_msg3);
                    }

                } else {
                    printf("(MI)Unknown LTE MAC RACH Attempt Subpkt id and Version"
                            ": 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored",
                        result_subpkt, "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE MAC RACH Attempt Packet Version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_pdcp_dl_config_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num Subpkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LtePdcpDlConfig_SubpktHeader,
                        ARRAY_SIZE(LtePdcpDlConfig_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 192 && subpkt_ver == 2) {
                    // PDCP DL Config 0xC0
                    offset += _decode_by_fmt(
                            LtePdcpDlConfig_SubpktPayload,
                            ARRAY_SIZE(LtePdcpDlConfig_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Reason",
                            LtePdcpDlConfig_Subpkt_Reason,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_Reason, ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_subpkt, "SRB Cipher Algorithm",
                            LtePdcpDlConfig_Subpkt_CipherAlgo,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_CipherAlgo, ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_subpkt, "DRB Cipher Algorithm",
                            LtePdcpDlConfig_Subpkt_CipherAlgo,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_CipherAlgo, ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_subpkt, "SRB Integrity Algorithm",
                            LtePdcpDlConfig_Subpkt_IntegAlgo,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_IntegAlgo, ValueName),
                            "(MI)Unknown");
                    int iArraySize = _search_result_int(result_subpkt,
                            "Array size");

                    // Released RB
                    int start_ReleasedRBStruct = offset;
                    offset += _decode_by_fmt(
                            LtePdcpDlConfig_Subpkt_ReleaseRB_Header,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ReleaseRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ReleasedRB = _search_result_int(result_subpkt,
                            "Number of Released RBs");
                    PyObject *result_ReleasedRB = PyList_New(0);
                    for (int j = 0; j < num_ReleasedRB; j++) {
                        PyObject *result_ReleasedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpDlConfig_Subpkt_ReleaseRB_Fmt,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ReleaseRB_Fmt, Fmt),
                                b, offset, length, result_ReleasedRB_item);
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_ReleasedRB_item, "dict");
                        PyList_Append(result_ReleasedRB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_ReleasedRB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "Released RBs",
                            result_ReleasedRB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_ReleasedRB);
                    offset += 1 + iArraySize * 1 -
                        (offset - start_ReleasedRBStruct);

                    // Added/Modified RB
                    int start_AddedModifiedRBStruct = offset;
                    offset += _decode_by_fmt(LtePdcpDlConfig_Subpkt_AddedModifiedRB_Header,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_AddedModifiedRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_AddedModifiedRB = _search_result_int(result_subpkt,
                            "Number of Added/Modified RBs");
                    PyObject *result_AddedModifiedRB = PyList_New(0);
                    for (int j = 0; j < num_AddedModifiedRB; j++) {
                        PyObject *result_AddedModifiedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpDlConfig_Subpkt_AddedModifiedRB_Fmt,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_AddedModifiedRB_Fmt, Fmt),
                                b, offset, length, result_AddedModifiedRB_item);
                        (void) _map_result_field_to_name(result_AddedModifiedRB_item,
                                "Action",
                                LtePdcpDlConfig_Subpkt_AddedModifiedRB_Action,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_AddedModifiedRB_Action,
                                    ValueName),
                                "(MI)Unknown");
                        PyObject *t2 = Py_BuildValue("(sOs)", "Ignored",
                                result_AddedModifiedRB_item, "dict");
                        PyList_Append(result_AddedModifiedRB, t2);
                        Py_DECREF(t2);
                        Py_DECREF(result_AddedModifiedRB_item);
                    }
                    PyObject *t2 = Py_BuildValue("(sOs)", "Added/Modified RBs",
                            result_AddedModifiedRB, "list");
                    PyList_Append(result_subpkt, t2);
                    Py_DECREF(t2);
                    Py_DECREF(result_AddedModifiedRB);
                    offset += 1 + iArraySize * 2 - (offset - start_AddedModifiedRBStruct);

                    // Active RB
                    // int start_ActiveRBStruct = offset;
                    offset += _decode_by_fmt(LtePdcpDlConfig_Subpkt_ActiveRB_Header,
                            ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ActiveRB = _search_result_int(result_subpkt,
                            "Number of active RBs");
                    PyObject *result_ActiveRB = PyList_New(0);
                    for (int j = 0; j < num_ActiveRB; j++) {
                        PyObject *result_ActiveRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpDlConfig_Subpkt_ActiveRB_Fmt,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_Fmt, Fmt),
                                b, offset, length, result_ActiveRB_item);
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB mode",
                                LtePdcpDlConfig_Subpkt_ActiveRB_RBmode,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_RBmode,
                                    ValueName),
                                "(MI)Unknown");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB type",
                                LtePdcpDlConfig_Subpkt_ActiveRB_RBtype,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_RBtype,
                                    ValueName),
                                "(MI)Unknown");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "Status report",
                                LtePdcpDlConfig_Subpkt_ActiveRB_StatusReport,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_StatusReport,
                                    ValueName),
                                "YES");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RoHC Enabled",
                                LtePdcpDlConfig_Subpkt_ActiveRB_RoHCEnabled,
                                ARRAY_SIZE(LtePdcpDlConfig_Subpkt_ActiveRB_RoHCEnabled,
                                    ValueName),
                                "true");
                        PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                result_ActiveRB_item, "dict");
                        PyList_Append(result_ActiveRB, t3);
                        Py_DECREF(t3);
                        Py_DECREF(result_ActiveRB_item);
                    }
                    PyObject *t3 = Py_BuildValue("(sOs)", "Active RBs",
                            result_ActiveRB, "list");
                    PyList_Append(result_subpkt, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_ActiveRB);
                } else {
                    printf("(MI)Unknown LTE PDCP DL Config subpkt id and version:"
                            " 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PDCP DL Config Log Packet version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_pdcp_ul_config_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num Subpkt");
    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LtePdcpUlConfig_SubpktHeader,
                        ARRAY_SIZE(LtePdcpUlConfig_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 193 && subpkt_ver == 2) {
                    // PDCP UL Config 0xC1
                    offset += _decode_by_fmt(
                            LtePdcpUlConfig_SubpktPayload,
                            ARRAY_SIZE(LtePdcpUlConfig_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Reason",
                            LtePdcpUlConfig_Subpkt_Reason,
                            ARRAY_SIZE(LtePdcpUlConfig_Subpkt_Reason, ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_subpkt, "SRB Cipher Algorithm",
                            LtePdcpUlConfig_Subpkt_CipherAlgo,
                            ARRAY_SIZE(LtePdcpUlConfig_Subpkt_CipherAlgo, ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_subpkt, "DRB Cipher Algorithm",
                            LtePdcpUlConfig_Subpkt_CipherAlgo,
                            ARRAY_SIZE(LtePdcpUlConfig_Subpkt_CipherAlgo, ValueName),
                            "(MI)Unknown");
                    (void) _map_result_field_to_name(result_subpkt, "SRB Integrity Algorithm",
                            LtePdcpUlConfig_Subpkt_IntegAlgo,
                            ARRAY_SIZE(LtePdcpUlConfig_Subpkt_IntegAlgo, ValueName),
                            "(MI)Unknown");
                    int iArraySize = _search_result_int(result_subpkt,
                            "Array size");

                    // Released RB
                    int start_ReleasedRBStruct = offset;
                    offset += _decode_by_fmt(
                            LtePdcpUlConfig_Subpkt_ReleaseRB_Header,
                            ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ReleaseRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ReleasedRB = _search_result_int(result_subpkt,
                            "Number of Released RBs");
                    PyObject *result_ReleasedRB = PyList_New(0);
                    for (int j = 0; j < num_ReleasedRB; j++) {
                        PyObject *result_ReleasedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpUlConfig_Subpkt_ReleaseRB_Fmt,
                                ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ReleaseRB_Fmt, Fmt),
                                b, offset, length, result_ReleasedRB_item);
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_ReleasedRB_item, "dict");
                        PyList_Append(result_ReleasedRB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_ReleasedRB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "Released RBs",
                            result_ReleasedRB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_ReleasedRB);
                    offset += 1 + iArraySize * 1 -
                        (offset - start_ReleasedRBStruct);

                    // Added/Modified RB
                    int start_AddedModifiedRBStruct = offset;
                    offset += _decode_by_fmt(LtePdcpUlConfig_Subpkt_AddedModifiedRB_Header,
                            ARRAY_SIZE(LtePdcpUlConfig_Subpkt_AddedModifiedRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_AddedModifiedRB = _search_result_int(result_subpkt,
                            "Number of Added/Modified RBs");
                    PyObject *result_AddedModifiedRB = PyList_New(0);
                    for (int j = 0; j < num_AddedModifiedRB; j++) {
                        PyObject *result_AddedModifiedRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpUlConfig_Subpkt_AddedModifiedRB_Fmt,
                                ARRAY_SIZE(LtePdcpUlConfig_Subpkt_AddedModifiedRB_Fmt, Fmt),
                                b, offset, length, result_AddedModifiedRB_item);
                        (void) _map_result_field_to_name(result_AddedModifiedRB_item,
                                "Action",
                                LtePdcpUlConfig_Subpkt_AddedModifiedRB_Action,
                                ARRAY_SIZE(LtePdcpUlConfig_Subpkt_AddedModifiedRB_Action,
                                    ValueName),
                                "(MI)Unknown");
                        PyObject *t2 = Py_BuildValue("(sOs)", "Ignored",
                                result_AddedModifiedRB_item, "dict");
                        PyList_Append(result_AddedModifiedRB, t2);
                        Py_DECREF(t2);
                        Py_DECREF(result_AddedModifiedRB_item);
                    }
                    PyObject *t2 = Py_BuildValue("(sOs)", "Added/Modified RBs",
                            result_AddedModifiedRB, "list");
                    PyList_Append(result_subpkt, t2);
                    Py_DECREF(t2);
                    Py_DECREF(result_AddedModifiedRB);
                    offset += 1 + iArraySize * 2 - (offset - start_AddedModifiedRBStruct);

                    // Active RB
                    // int start_ActiveRBStruct = offset;
                    offset += _decode_by_fmt(LtePdcpUlConfig_Subpkt_ActiveRB_Header,
                            ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_ActiveRB = _search_result_int(result_subpkt,
                            "Number of active RBs");
                    PyObject *result_ActiveRB = PyList_New(0);
                    for (int j = 0; j < num_ActiveRB; j++) {
                        PyObject *result_ActiveRB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpUlConfig_Subpkt_ActiveRB_Fmt,
                                ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_Fmt, Fmt),
                                b, offset, length, result_ActiveRB_item);
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB mode",
                                LtePdcpUlConfig_Subpkt_ActiveRB_RBmode,
                                ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_RBmode,
                                    ValueName),
                                "(MI)Unknown");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RB type",
                                LtePdcpUlConfig_Subpkt_ActiveRB_RBtype,
                                ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_RBtype,
                                    ValueName),
                                "(MI)Unknown");
                        (void) _map_result_field_to_name(result_ActiveRB_item,
                                "RoHC Enabled",
                                LtePdcpUlConfig_Subpkt_ActiveRB_RoHCEnabled,
                                ARRAY_SIZE(LtePdcpUlConfig_Subpkt_ActiveRB_RoHCEnabled,
                                    ValueName),
                                "true");
                        PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                                result_ActiveRB_item, "dict");
                        PyList_Append(result_ActiveRB, t3);
                        Py_DECREF(t3);
                        Py_DECREF(result_ActiveRB_item);
                    }
                    PyObject *t3 = Py_BuildValue("(sOs)", "Active RBs",
                            result_ActiveRB, "list");
                    PyList_Append(result_subpkt, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_ActiveRB);
                } else {
                    printf("(MI)Unknown LTE PDCP UL Config subpkt id and version:"
                            " 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PDCP UL Config Log Packet version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_pdcp_ul_data_pdu_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num Subpkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LtePdcpUlDataPdu_SubpktHeader,
                        ARRAY_SIZE(LtePdcpUlDataPdu_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 194 && subpkt_ver == 1) {
                    // PDCP UL Data PDU 0xC2
                    offset += _decode_by_fmt(
                            LtePdcpUlDataPdu_SubpktPayload,
                            ARRAY_SIZE(LtePdcpUlDataPdu_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Mode",
                            LtePdcpUlDataPdu_Subpkt_Mode,
                            ARRAY_SIZE(LtePdcpUlDataPdu_Subpkt_Mode, ValueName),
                            "(MI)Unknown");

                    // PDU
                    // int start_PDU = offset;
                    offset += _decode_by_fmt(
                            LtePdcpUlDataPdu_Subpkt_PDU_Header,
                            ARRAY_SIZE(LtePdcpUlDataPdu_Subpkt_PDU_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_PDU = _search_result_int(result_subpkt,
                            "Num PDUs");
                    PyObject *result_PDU = PyList_New(0);
                    for (int j = 0; j < num_PDU; j++) {
                        PyObject *result_PDU_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpUlDataPdu_Subpkt_PDU_Fmt,
                                ARRAY_SIZE(LtePdcpUlDataPdu_Subpkt_PDU_Fmt, Fmt),
                                b, offset, length, result_PDU_item);
                        int iNonDecodeFN = _search_result_int(result_PDU_item,
                                "System Frame Number");
                        // Handle fn
                        const unsigned int SFN_RSHIFT = 4,
                              SFN_MASK = (1 << 12) - 1;
                        const unsigned int SUBFRAME_RSHIFT = 0,
                              SUBFRAME_MASK = (1 << 4) - 1;
                        int sys_fn = (iNonDecodeFN >> SFN_RSHIFT) & SFN_MASK;
                        int sub_fn =
                            (iNonDecodeFN >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
                        PyObject *old_object = _replace_result_int(result_PDU_item,
                                "System Frame Number", sys_fn);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_PDU_item,
                                "Subframe Number", sub_fn);
                        Py_DECREF(old_object);

                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_PDU_item, "dict");
                        PyList_Append(result_PDU, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_PDU_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "PDCP UL Data PDU",
                            result_PDU, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_PDU);
                } else {
                    printf("(MI)Unknown LTE PDCP UL Data PDU subpkt id and version:"
                            " 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PDCP UL Data PDU version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_pdcp_dl_stats_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num Subpkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LtePdcpDlStats_SubpktHeader,
                        ARRAY_SIZE(LtePdcpDlStats_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 196 && subpkt_ver == 2) {
                    // PDCP DL Stats: 0xC2
                    offset += _decode_by_fmt(
                            LtePdcpDlStats_SubpktPayload,
                            ARRAY_SIZE(LtePdcpDlStats_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    // RBs
                    int num_RB = _search_result_int(result_subpkt,
                            "Num RBs");
                    PyObject *result_RB = PyList_New(0);
                    for (int j = 0; j < num_RB; j++) {
                        PyObject *result_RB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpDlStats_Subpkt_RB_Fmt,
                                ARRAY_SIZE(LtePdcpDlStats_Subpkt_RB_Fmt, Fmt),
                                b, offset, length, result_RB_item);
                        (void) _map_result_field_to_name(result_RB_item,
                                "Mode",
                                LtePdcpDlStats_Subpkt_RB_Mode,
                                ARRAY_SIZE(LtePdcpDlStats_Subpkt_RB_Mode, ValueName),
                                "(MI)Unknown");
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_RB_item, "dict");
                        PyList_Append(result_RB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_RB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "RBs",
                            result_RB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_RB);
                } else {
                    printf("(MI)Unknown LTE PDCP DL Stats subpkt id and version:"
                            " 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PDCP DL Stats version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_pdcp_ul_stats_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num Subpkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LtePdcpUlStats_SubpktHeader,
                        ARRAY_SIZE(LtePdcpUlStats_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 197 && subpkt_ver == 1) {
                    // PDCP UL Stats: 0xC5
                    offset += _decode_by_fmt(
                            LtePdcpUlStats_SubpktPayload_v1,
                            ARRAY_SIZE(LtePdcpUlStats_SubpktPayload_v1, Fmt),
                            b, offset, length, result_subpkt);
                    // RBs
                    int num_RB = _search_result_int(result_subpkt,
                            "Num RBs");
                    PyObject *result_RB = PyList_New(0);
                    for (int j = 0; j < num_RB; j++) {
                        PyObject *result_RB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpUlStats_Subpkt_RB_Fmt_v1,
                                ARRAY_SIZE(LtePdcpUlStats_Subpkt_RB_Fmt_v1, Fmt),
                                b, offset, length, result_RB_item);
                        (void) _map_result_field_to_name(result_RB_item,
                                "Mode",
                                LtePdcpUlStats_Subpkt_RB_Mode,
                                ARRAY_SIZE(LtePdcpUlStats_Subpkt_RB_Mode, ValueName),
                                "(MI)Unknown");
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_RB_item, "dict");
                        PyList_Append(result_RB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_RB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "RBs",
                            result_RB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_RB);
                } else if (subpkt_id == 197 && subpkt_ver == 2) {
                    // PDCP UL Stats: 0xC5
                    offset += _decode_by_fmt(
                            LtePdcpUlStats_SubpktPayload_v2,
                            ARRAY_SIZE(LtePdcpUlStats_SubpktPayload_v2, Fmt),
                            b, offset, length, result_subpkt);
                    // RBs
                    int num_RB = _search_result_int(result_subpkt,
                            "Num RBs");
                    PyObject *result_RB = PyList_New(0);
                    for (int j = 0; j < num_RB; j++) {
                        PyObject *result_RB_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpUlStats_Subpkt_RB_Fmt_v2,
                                ARRAY_SIZE(LtePdcpUlStats_Subpkt_RB_Fmt_v2, Fmt),
                                b, offset, length, result_RB_item);
                        (void) _map_result_field_to_name(result_RB_item,
                                "Mode",
                                LtePdcpUlStats_Subpkt_RB_Mode,
                                ARRAY_SIZE(LtePdcpUlStats_Subpkt_RB_Mode, ValueName),
                                "(MI)Unknown");
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_RB_item, "dict");
                        PyList_Append(result_RB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_RB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "RBs",
                            result_RB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_RB);
                } else {
                    printf("(MI)Unknown LTE PDCP UL Stats subpkt id and version:"
                            " 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PDCP UL Stats version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}


// ----------------------------------------------------------------------------
static int _decode_lte_rlc_ul_stats_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num Subpkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LteRlcUlStats_SubpktHeader,
                        ARRAY_SIZE(LteRlcUlStats_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 71 && subpkt_ver == 1) {
                    // LTE RLC UL Stats: 0x47
                    offset += _decode_by_fmt(
                            LteRlcUlStats_SubpktPayload,
                            ARRAY_SIZE(LteRlcUlStats_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    // RBs
                    int num_RB = _search_result_int(result_subpkt,
                            "Num RBs");
                    PyObject *result_RB = PyList_New(0);
                    for (int j = 0; j < num_RB; j++) {
                        PyObject *result_RB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcUlStats_Subpkt_RB_Fmt,
                                ARRAY_SIZE(LteRlcUlStats_Subpkt_RB_Fmt, Fmt),
                                b, offset, length, result_RB_item);
                        (void) _map_result_field_to_name(result_RB_item,
                                "Mode",
                                LteRlcUlStats_Subpkt_RB_Mode,
                                ARRAY_SIZE(LteRlcUlStats_Subpkt_RB_Mode, ValueName),
                                "(MI)Unknown");
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_RB_item, "dict");
                        PyList_Append(result_RB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_RB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "RBs",
                            result_RB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_RB);
                } else {
                    printf("(MI)Unknown LTE RLC UL Stats subpkt id and version:"
                            " 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE RLC UL Stats version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_rlc_dl_stats_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num Subpkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LteRlcDlStats_SubpktHeader,
                        ARRAY_SIZE(LteRlcDlStats_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 66 && subpkt_ver == 3) {
                    // LTE RLC DL Stats: 0x42
                    offset += _decode_by_fmt(
                            LteRlcDlStats_SubpktPayload_v3,
                            ARRAY_SIZE(LteRlcDlStats_SubpktPayload_v3, Fmt),
                            b, offset, length, result_subpkt);
                    // RBs
                    int num_RB = _search_result_int(result_subpkt,
                            "Num RBs");
                    PyObject *result_RB = PyList_New(0);
                    for (int j = 0; j < num_RB; j++) {
                        PyObject *result_RB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcDlStats_Subpkt_RB_Fmt_v3,
                                ARRAY_SIZE(LteRlcDlStats_Subpkt_RB_Fmt_v3, Fmt),
                                b, offset, length, result_RB_item);
                        (void) _map_result_field_to_name(result_RB_item,
                                "Mode",
                                LteRlcDlStats_Subpkt_RB_Mode,
                                ARRAY_SIZE(LteRlcDlStats_Subpkt_RB_Mode, ValueName),
                                "(MI)Unknown");
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_RB_item, "dict");
                        PyList_Append(result_RB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_RB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "RBs",
                            result_RB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_RB);
                } else if (subpkt_id == 66 && subpkt_ver == 2) {
                    // LTE RLC DL Stats: 0x42
                    offset += _decode_by_fmt(
                            LteRlcDlStats_SubpktPayload_v2,
                            ARRAY_SIZE(LteRlcDlStats_SubpktPayload_v2, Fmt),
                            b, offset, length, result_subpkt);
                    // RBs
                    int num_RB = _search_result_int(result_subpkt,
                            "Num RBs");
                    PyObject *result_RB = PyList_New(0);
                    for (int j = 0; j < num_RB; j++) {
                        PyObject *result_RB_item = PyList_New(0);
                        offset += _decode_by_fmt(LteRlcDlStats_Subpkt_RB_Fmt_v2,
                                ARRAY_SIZE(LteRlcDlStats_Subpkt_RB_Fmt_v2, Fmt),
                                b, offset, length, result_RB_item);
                        (void) _map_result_field_to_name(result_RB_item,
                                "Mode",
                                LteRlcDlStats_Subpkt_RB_Mode,
                                ARRAY_SIZE(LteRlcDlStats_Subpkt_RB_Mode, ValueName),
                                "(MI)Unknown");
                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_RB_item, "dict");
                        PyList_Append(result_RB, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_RB_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "RBs",
                            result_RB, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_RB);
                } else {
                    printf("(MI)Unknown LTE RLC DL Stats subpkt id and version:"
                            " 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE RLC DL Stats version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_pdcp_dl_ctrl_pdu_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num Subpkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LtePdcpDlCtrlPdu_SubpktHeader,
                        ARRAY_SIZE(LtePdcpDlCtrlPdu_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 194 && subpkt_ver == 1) {
                    // PDCP DL Ctrl PDU 0xC2
                    offset += _decode_by_fmt(
                            LtePdcpDlCtrlPdu_SubpktPayload,
                            ARRAY_SIZE(LtePdcpDlCtrlPdu_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Mode",
                            LtePdcpDlCtrlPdu_Subpkt_Mode,
                            ARRAY_SIZE(LtePdcpDlCtrlPdu_Subpkt_Mode, ValueName),
                            "(MI)Unknown");

                    // PDU
                    // int start_PDU = offset;
                    offset += _decode_by_fmt(
                            LtePdcpDlCtrlPdu_Subpkt_PDU_Header,
                            ARRAY_SIZE(LtePdcpDlCtrlPdu_Subpkt_PDU_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_PDU = _search_result_int(result_subpkt,
                            "Num PDUs");
                    PyObject *result_PDU = PyList_New(0);
                    for (int j = 0; j < num_PDU; j++) {
                        PyObject *result_PDU_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpDlCtrlPdu_Subpkt_PDU_Fmt,
                                ARRAY_SIZE(LtePdcpDlCtrlPdu_Subpkt_PDU_Fmt, Fmt),
                                b, offset, length, result_PDU_item);
                        int iNonDecodeFN = _search_result_int(result_PDU_item,
                                "System Frame Number");
                        // Handle fn
                        const unsigned int SFN_RSHIFT = 4,
                              SFN_MASK = (1 << 12) - 1;
                        const unsigned int SUBFRAME_RSHIFT = 0,
                              SUBFRAME_MASK = (1 << 4) - 1;
                        int sys_fn = (iNonDecodeFN >> SFN_RSHIFT) & SFN_MASK;
                        int sub_fn =
                            (iNonDecodeFN >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
                        PyObject *old_object = _replace_result_int(result_PDU_item,
                                "System Frame Number", sys_fn);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_PDU_item,
                                "Subframe Number", sub_fn);
                        Py_DECREF(old_object);
                        // type = STATUS REPORT
                        std::string strType = "STATUS REPORT";
                        PyObject *pystr = Py_BuildValue("s", strType.c_str());
                        old_object = _replace_result(result_PDU_item,
                                "type", pystr);
                        Py_DECREF(old_object);
                        Py_DECREF(pystr);

                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_PDU_item, "dict");
                        PyList_Append(result_PDU, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_PDU_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "PDCP DL Ctrl PDU",
                            result_PDU, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_PDU);
                } else {
                    printf("(MI)Unknown LTE PDCP DL Ctrl PDU subpkt id and version:"
                            " 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PDCP DL Ctrl PDU version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_pdcp_ul_ctrl_pdu_subpkt (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");
    int n_subpkt = _search_result_int(result, "Num Subpkt");

    switch (pkt_ver) {
    case 1:
        {
            PyObject *result_allpkts = PyList_New(0);
            for (int i = 0; i < n_subpkt; i++) {
                PyObject *result_subpkt = PyList_New(0);
                int start_subpkt = offset;
                // decode subpacket header
                offset += _decode_by_fmt(LtePdcpUlCtrlPdu_SubpktHeader,
                        ARRAY_SIZE(LtePdcpUlCtrlPdu_SubpktHeader, Fmt),
                        b, offset, length, result_subpkt);
                int subpkt_id = _search_result_int(result_subpkt,
                        "Subpacket ID");
                int subpkt_ver = _search_result_int(result_subpkt,
                        "Subpacket Version");
                int subpkt_size = _search_result_int(result_subpkt,
                        "Subpacket Size");
                if (subpkt_id == 194 && subpkt_ver == 1) {
                    // PDCP UL Ctrl PDU 0xC2
                    offset += _decode_by_fmt(
                            LtePdcpUlCtrlPdu_SubpktPayload,
                            ARRAY_SIZE(LtePdcpUlCtrlPdu_SubpktPayload, Fmt),
                            b, offset, length, result_subpkt);
                    (void) _map_result_field_to_name(result_subpkt, "Mode",
                            LtePdcpUlCtrlPdu_Subpkt_Mode,
                            ARRAY_SIZE(LtePdcpUlCtrlPdu_Subpkt_Mode, ValueName),
                            "(MI)Unknown");

                    // PDU
                    // int start_PDU = offset;
                    offset += _decode_by_fmt(
                            LtePdcpUlCtrlPdu_Subpkt_PDU_Header,
                            ARRAY_SIZE(LtePdcpUlCtrlPdu_Subpkt_PDU_Header,
                                Fmt),
                            b, offset, length, result_subpkt);
                    int num_PDU = _search_result_int(result_subpkt,
                            "Num PDUs");
                    PyObject *result_PDU = PyList_New(0);
                    for (int j = 0; j < num_PDU; j++) {
                        PyObject *result_PDU_item = PyList_New(0);
                        offset += _decode_by_fmt(LtePdcpUlCtrlPdu_Subpkt_PDU_Fmt,
                                ARRAY_SIZE(LtePdcpUlCtrlPdu_Subpkt_PDU_Fmt, Fmt),
                                b, offset, length, result_PDU_item);
                        int iNonDecodeFN = _search_result_int(result_PDU_item,
                                "System Frame Number");
                        // Handle fn
                        const unsigned int SFN_RSHIFT = 4,
                              SFN_MASK = (1 << 12) - 1;
                        const unsigned int SUBFRAME_RSHIFT = 0,
                              SUBFRAME_MASK = (1 << 4) - 1;
                        int sys_fn = (iNonDecodeFN >> SFN_RSHIFT) & SFN_MASK;
                        int sub_fn =
                            (iNonDecodeFN >> SUBFRAME_RSHIFT) & SUBFRAME_MASK;
                        PyObject *old_object = _replace_result_int(result_PDU_item,
                                "System Frame Number", sys_fn);
                        Py_DECREF(old_object);
                        old_object = _replace_result_int(result_PDU_item,
                                "Subframe Number", sub_fn);
                        Py_DECREF(old_object);
                        // type = STATUS REPORT
                        std::string strType = "STATUS REPORT";
                        PyObject *pystr = Py_BuildValue("s", strType.c_str());
                        old_object = _replace_result(result_PDU_item,
                                "type", pystr);
                        Py_DECREF(old_object);
                        Py_DECREF(pystr);

                        PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                                result_PDU_item, "dict");
                        PyList_Append(result_PDU, t1);
                        Py_DECREF(t1);
                        Py_DECREF(result_PDU_item);
                    }
                    PyObject *t1 = Py_BuildValue("(sOs)", "PDCP DL Ctrl PDU",
                            result_PDU, "list");
                    PyList_Append(result_subpkt, t1);
                    Py_DECREF(t1);
                    Py_DECREF(result_PDU);
                } else {
                    printf("(MI)Unknown LTE PDCP UL Ctrl PDU subpkt id and version:"
                            " 0x%x - %d\n", subpkt_id, subpkt_ver);
                }
                PyObject *t = Py_BuildValue("(sOs)", "Ignored", result_subpkt,
                        "dict");
                PyList_Append(result_allpkts, t);
                Py_DECREF(t);
                Py_DECREF(result_subpkt);
                offset += subpkt_size - (offset - start_subpkt);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Subpackets", result_allpkts,
                    "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_allpkts);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PDCP UL Ctrl PDU version: 0x%x\n",
                pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_pucch_power_control_payload (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 4:
        {
            offset += _decode_by_fmt(LtePucchPowerControl_Fmt_v4,
                    ARRAY_SIZE(LtePucchPowerControl_Fmt_v4, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Number of Records");
            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePucchPowerControl_Record_Fmt_v4,
                        ARRAY_SIZE(LtePucchPowerControl_Record_Fmt_v4, Fmt),
                        b, offset, length, result_record_item);
                unsigned int iNonDecodeSFN = _search_result_uint(result_record_item,
                        "SFN");
                int iSFN = iNonDecodeSFN & 1023; // last 10 bits
                int iSubFN = (iNonDecodeSFN >> 10) & 15; // next 4 bits
                int iPower = (iNonDecodeSFN >> 14) & 255; // next 8 bits
                int iDCI = (iNonDecodeSFN >> 22) & 15; // next 4 bits
                int iPUCCH = (iNonDecodeSFN >> 26) & 7; // next 3 bits
                int iN_HARQ = (iNonDecodeSFN >> 29) & 1; // next 1 bit
                PyObject *old_object = _replace_result_int(result_record_item,
                        "SFN", iSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Sub-FN",
                        iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PUCCH Tx Power (dBm)", iPower);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DCI Format", iDCI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "DCI Format",
                        LtePucchPowerControl_Record_v4_DCI_Format,
                        ARRAY_SIZE(LtePucchPowerControl_Record_v4_DCI_Format,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "PUCCH Format", iPUCCH);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "PUCCH Format",
                        LtePucchPowerControl_Record_v4_PUCCH_Format,
                        ARRAY_SIZE(LtePucchPowerControl_Record_v4_PUCCH_Format,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "N_HARQ", iN_HARQ);
                Py_DECREF(old_object);

                unsigned int iNonDecodeTPC = _search_result_uint(result_record_item,
                        "TPC Command");
                int iTPC = iNonDecodeTPC & 63; // last 6 bits
                int iN_CQI = (iNonDecodeTPC >> 6) & 31; // next 5 bits
                int iLoss = (iNonDecodeTPC >> 11) & 127; // next 8 bits
                old_object = _replace_result_int(result_record_item,
                        "TPC Command", iTPC);
                Py_DECREF(old_object);
                if (iTPC == 31 || iTPC == 63) {
                    (void) _map_result_field_to_name(result_record_item,
                            "TPC Command",
                            LtePucchPowerControl_Record_v4_TPC,
                            ARRAY_SIZE(LtePucchPowerControl_Record_v4_TPC,
                                ValueName),
                            "(MI)Unknown");
                }
                old_object = _replace_result_int(result_record_item,
                        "N_CQI", iN_CQI);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DL Pass Loss", iLoss);
                Py_DECREF(old_object);

                int iNonDecodeGi = _search_result_int(result_record_item,
                        "g(i)");
                int iGi = iNonDecodeGi - 65535;
                old_object = _replace_result_int(result_record_item,
                        "g(i)", iGi);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PUCCH Power Control version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_pusch_power_control_payload (const char *b, int offset,
        size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 5:
        {
            offset += _decode_by_fmt(LtePuschPowerControl_Fmt_v5,
                    ARRAY_SIZE(LtePuschPowerControl_Fmt_v5, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Number of Records");
            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePuschPowerControl_Record_Fmt_v5,
                        ARRAY_SIZE(LtePuschPowerControl_Record_Fmt_v5, Fmt),
                        b, offset, length, result_record_item);
                unsigned int iNonDecodeSFN = _search_result_uint(result_record_item,
                        "SFN");
                int iSFN = iNonDecodeSFN & 1023; // last 10 bits
                int iSubFN = (iNonDecodeSFN >> 10) & 15; // next 4 bits
                int iPower = (iNonDecodeSFN >> 14) & 255; // next 8 bits
                int iDCI = (iNonDecodeSFN >> 22) & 15; // next 4 bits
                int iTxType = (iNonDecodeSFN >> 26) & 3; // next 3 bits
                PyObject *old_object = _replace_result_int(result_record_item,
                        "SFN", iSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Sub-FN",
                        iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Tx Power (dBm)", iPower);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DCI Format", iDCI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "DCI Format",
                        LtePuschPowerControl_Record_v5_DCI_Format,
                        ARRAY_SIZE(LtePuschPowerControl_Record_v5_DCI_Format,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Tx Type", iTxType);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Tx Type",
                        LtePuschPowerControl_Record_v5_TxType,
                        ARRAY_SIZE(LtePuschPowerControl_Record_v5_TxType,
                            ValueName),
                        "(MI)Unknown");

                unsigned int iNonDecodeNRB = _search_result_uint(result_record_item,
                        "Num RBs");
                int iNRB = iNonDecodeNRB & 255; // last 8 bits
                int iTBS = (iNonDecodeNRB >> 8) & 16383; // next 14 bits
                int iLoss = (iNonDecodeNRB >> 22) & 255; // next 8 bits
                old_object = _replace_result_int(result_record_item,
                        "Num RBs", iNRB);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Transport Block Size", iTBS);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DL Path Loss", iLoss);
                Py_DECREF(old_object);

                unsigned int iNonDecodeFi = _search_result_uint(result_record_item,
                        "F(i)");
                int iFi = iNonDecodeFi & 1023; // last 10 bits
                if (iFi >= 512) {
                    iFi -= 1024;
                }
                int iTPC = (iNonDecodeFi >> 10) & 31; // next 5 bits
                int iActualPower = (iNonDecodeFi >> 15) & 255;
                old_object = _replace_result_int(result_record_item,
                        "F(i)", iFi);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "TPC", iTPC);
                if (iTPC == 15 || iTPC == 31) {
                    (void) _map_result_field_to_name(result_record_item,
                            "TPC",
                            LtePuschPowerControl_Record_v5_TPC,
                            ARRAY_SIZE(LtePuschPowerControl_Record_v5_TPC,
                                ValueName),
                            "(MI)Unknown");
                }
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Actual Tx Power", iActualPower);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    case 4:
        {
            offset += _decode_by_fmt(LtePuschPowerControl_Fmt_v4,
                    ARRAY_SIZE(LtePuschPowerControl_Fmt_v4, Fmt),
                    b, offset, length, result);
            int num_record = _search_result_int(result, "Number of Records");
            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePuschPowerControl_Record_Fmt_v4,
                        ARRAY_SIZE(LtePuschPowerControl_Record_Fmt_v4, Fmt),
                        b, offset, length, result_record_item);
                unsigned int iNonDecodeSFN = _search_result_uint(result_record_item,
                        "SFN");
                int iSFN = iNonDecodeSFN & 1023; // last 10 bits
                int iSubFN = (iNonDecodeSFN >> 10) & 15; // next 4 bits
                int iPower = (iNonDecodeSFN >> 14) & 255; // next 8 bits
                int iDCI = (iNonDecodeSFN >> 22) & 15; // next 4 bits
                int iTxType = (iNonDecodeSFN >> 26) & 3; // next 3 bits
                PyObject *old_object = _replace_result_int(result_record_item,
                        "SFN", iSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item, "Sub-FN",
                        iSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Tx Power (dBm)", iPower);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DCI Format", iDCI);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "DCI Format",
                        LtePuschPowerControl_Record_v5_DCI_Format,
                        ARRAY_SIZE(LtePuschPowerControl_Record_v5_DCI_Format,
                            ValueName),
                        "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "Tx Type", iTxType);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "Tx Type",
                        LtePuschPowerControl_Record_v5_TxType,
                        ARRAY_SIZE(LtePuschPowerControl_Record_v5_TxType,
                            ValueName),
                        "(MI)Unknown");

                unsigned int iNonDecodeTBS = _search_result_uint(result_record_item,
                        "Transport Block Size");
                int iTBS = iNonDecodeTBS & 16383; // last 14 bits
                int iLoss = (iNonDecodeTBS >> 14) & 255; // next 8 bits
                int iFi = (iNonDecodeTBS >> 22) & 1023; // next 10 bits
                if (iFi >= 512) {
                    iFi -= 1024;
                }
                old_object = _replace_result_int(result_record_item,
                        "Transport Block Size", iTBS);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "DL Path Loss", iLoss);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "F(i)", iFi);
                Py_DECREF(old_object);

                unsigned int iNonDecodeTPC = _search_result_uint(result_record_item,
                        "TPC");
                int iTPC = (iNonDecodeTPC) & 31; // last 5 bits
                int iActualPower = (iNonDecodeTPC >> 5) & 255; // next 8 bits
                old_object = _replace_result_int(result_record_item,
                        "TPC", iTPC);
                if (iTPC == 15 || iTPC == 31) {
                    (void) _map_result_field_to_name(result_record_item,
                            "TPC",
                            LtePuschPowerControl_Record_v5_TPC,
                            ARRAY_SIZE(LtePuschPowerControl_Record_v5_TPC,
                                ValueName),
                            "(MI)Unknown");
                }
                old_object = _replace_result_int(result_record_item,
                        "PUSCH Actual Tx Power", iActualPower);
                Py_DECREF(old_object);

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    default:
        printf("(MI)Unknown LTE PUSCH Power Control version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------
static int _decode_lte_pdcch_phich_indication_report_payload (const char *b,
        int offset, size_t length, PyObject *result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    switch (pkt_ver) {
    case 5:
        {
            offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Fmt_v5,
                    ARRAY_SIZE(LtePdcchPhichIndicationReport_Fmt_v5, Fmt),
                    b, offset, length, result);
            int iNonDecodeDuplexMode = _search_result_int(result,
                    "Duplex Mode");
            int iDuplexMode = iNonDecodeDuplexMode & 3; // last 2 bits
            PyObject *old_object = _replace_result_int(result, "Duplex Mode",
                    iDuplexMode);
            Py_DECREF(old_object);
            int num_record = _search_result_int(result, "Number of Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v5_p1,
                        ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_p1, Fmt),
                        b, offset, length, result_record_item);
                unsigned int iNonDecodeP1_1 = _search_result_uint(result_record_item,
                        "Num PDCCH Results");
                int iNumPdcch = iNonDecodeP1_1 & 7; // last 3 bits
                int iPdcchSFN = (iNonDecodeP1_1 >> 3) & 1023; // next 10 bits
                int iPdcchSubFN = (iNonDecodeP1_1 >> 13) & 15; // next 4 bits
                int iPhichIncluded = (iNonDecodeP1_1 >> 17) & 1; // next 1 bit
                int iPhich1Included = (iNonDecodeP1_1 >> 18) & 1; // next 1 bit
                int iPhichSFN = (iNonDecodeP1_1 >> 19) & 1023; // next 10 bits
                // reserved 3 bits
                unsigned int iNonDecodeP1_2 = _search_result_uint(result_record_item,
                        "PHICH Timing Sub-FN");
                int iPhichSubFN = iNonDecodeP1_2 & 15; // next 4 bits
                int iPhichValue = (iNonDecodeP1_2 >> 4) & 1; // next 1 bit
                int iPhich1Value = (iNonDecodeP1_2 >> 5) & 1; // next 1 bit
                old_object = _replace_result_int(result_record_item,
                        "Num PDCCH Results", iNumPdcch);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PDCCH Timing SFN", iPdcchSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PDCCH Timing Sub-FN", iPdcchSubFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PHICH Included", iPhichIncluded);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "PHICH Included",
                        LtePdcchPhichIndicationReport_Record_v5_Included,
                        ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_Included,
                            ValueName), "(MI)Unknown");
                old_object = _replace_result_int(result_record_item,
                        "PHICH 1 Included", iPhich1Included);
                Py_DECREF(old_object);
                (void) _map_result_field_to_name(result_record_item,
                        "PHICH 1 Included",
                        LtePdcchPhichIndicationReport_Record_v5_Included,
                        ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_Included,
                            ValueName), "(MI)Unknown");
                if (iPhichIncluded == 1) {
                    old_object = _replace_result_int(result_record_item,
                            "PHICH Timing SFN", iPhichSFN);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item,
                            "PHICH Timing Sub-FN", iPhichSubFN);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_record_item,
                            "PHICH Value", iPhichValue);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item,
                            "PHICH Value",
                            LtePdcchPhichIndicationReport_Record_v5_Value,
                            ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_Value,
                                ValueName), "(MI)Unknown");
                }
                if (iPhich1Included == 1) {
                    old_object = _replace_result_int(result_record_item,
                            "PHICH 1 Value", iPhich1Value);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_record_item,
                            "PHICH 1 Value",
                            LtePdcchPhichIndicationReport_Record_v5_Value,
                            ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_Value,
                                ValueName), "(MI)Unknown");
                }

                PyObject *result_pdcch = PyList_New(0);
                for (int j = 0; j < iNumPdcch; j++) {
                    PyObject *result_pdcch_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v5_p2,
                            ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_p2,
                                Fmt),
                            b, offset, length, result_pdcch_item);
                    int iNonDecodeP2_1 = _search_result_int(result_pdcch_item,
                            "Serv Cell Idx");
                    int iServCellIdx = iNonDecodeP2_1 & 7; // last 3 bits
                    int iRNTI = (iNonDecodeP2_1 >> 3) & 15; // next 4 bits
                    int iPayloadSize = (iNonDecodeP2_1 >> 7) & 127; // next 7 bits
                    int iAggLv = (iNonDecodeP2_1 >> 14) & 3; // next 2 bits
                    old_object = _replace_result_int(result_pdcch_item,
                            "Serv Cell Idx", iServCellIdx);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_pdcch_item,
                            "RNTI Type", iRNTI);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "RNTI Type",
                            RNTIType,
                            ARRAY_SIZE(RNTIType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_pdcch_item,
                            "Payload Size", iPayloadSize);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_pdcch_item,
                            "Aggregation Level", iAggLv);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "Aggregation Level",
                            LtePdcchPhichIndicationReport_Record_v5_AggLv,
                            ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_AggLv,
                                ValueName),
                            "(MI)Unknown");

                    int iNonDecodeP2_2 = _search_result_int(result_pdcch_item,
                            "Search Space");
                    int iSearchSpace = iNonDecodeP2_2 & 1; // last 1 bit
                    // SPS Grant Type should take this 3 bits
                    int iNewDLTx = (iNonDecodeP2_2 >> 4) & 1; // next 1 bit
                    int iNumDLTrblks = (iNonDecodeP2_2 >> 5) & 3; // next 2 bits
                    old_object = _replace_result_int(result_pdcch_item,
                            "Search Space", iSearchSpace);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "Search Space",
                            LtePdcchPhichIndicationReport_Record_v5_SS,
                            ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_SS,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_pdcch_item,
                            "New DL Tx", iNewDLTx);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "New DL Tx",
                            LtePdcchPhichIndicationReport_Record_v5_NewDLTx,
                            ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_NewDLTx,
                                ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_pdcch_item,
                            "Num DL Trblks", iNumDLTrblks);
                    Py_DECREF(old_object);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_pdcch_item, "dict");
                    PyList_Append(result_pdcch, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_pdcch_item);
                }
                PyObject *t2 = Py_BuildValue("(sOs)", "PDCCH Info",
                        result_pdcch, "list");
                PyList_Append(result_record_item, t2);
                Py_DECREF(t2);
                Py_DECREF(result_pdcch);

                for (int k = 0; k < 8 - iNumPdcch; k++) {
                    offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v5_p3,
                            ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v5_p3,
                                Fmt),
                            b, offset, length, result_record_item);
                }

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    case 25:
        {
            offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Fmt_v25,
                    ARRAY_SIZE(LtePdcchPhichIndicationReport_Fmt_v25, Fmt),
                    b, offset, length, result);
            int iNonDecodeDuplexMode = _search_result_int(result,
                    "Duplex Mode");
            int iDuplexMode = iNonDecodeDuplexMode & 3; // last 2 bits
            PyObject *old_object = _replace_result_int(result, "Duplex Mode",
                    iDuplexMode);
            Py_DECREF(old_object);
            int num_record = _search_result_int(result, "Number of Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v25_p1,
                        ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v25_p1, Fmt),
                        b, offset, length, result_record_item);
                unsigned int iNonDecodeP1_1 = _search_result_uint(result_record_item,
                        "Num PDCCH Results");
                int iNumPdcch = iNonDecodeP1_1 & 7; // last 3 bits
                int iNumPhich = (iNonDecodeP1_1 >> 3) & 7;  // next 3 bits
                int iPdcchSFN = (iNonDecodeP1_1 >> 6) & 1023; // next 10 bits
                int iPdcchSubFN = (iNonDecodeP1_1 >> 16) & 15; // next 4 bits
                old_object = _replace_result_int(result_record_item,
                        "Num PDCCH Results", iNumPdcch);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num PHICH Results", iNumPhich);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PDCCH Timing SFN", iPdcchSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PDCCH Timing Sub-FN", iPdcchSubFN);
                Py_DECREF(old_object);

                PyObject *result_phich = PyList_New(0);
                for (int j = 0; j < iNumPhich; j++) {
                    PyObject *result_phich_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v25_phich,
                            ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v25_phich,
                                Fmt),
                            b, offset, length, result_phich_item);
                    unsigned int iNonDecode_phich = _search_result_uint(
                            result_phich_item, "Cell Index");
                    int iCellIndex = iNonDecode_phich & 7;    // 3 bits
                    int iPhichIncluded = (iNonDecode_phich >> 3) & 1; // next 1 bit
                    int iPhich1Included = (iNonDecode_phich >> 4) & 1; // next 1 bit
                    int iPhichValue = (iNonDecode_phich >> 5) & 1; // next 1 bit
                    int iPhich1Value = (iNonDecode_phich >> 6) & 1; // next 1 bit

                    old_object = _replace_result_int(result_phich_item,
                            "Cell Index", iCellIndex);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_phich_item,
                            "PHICH Included", iPhichIncluded);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_phich_item,
                            "PHICH Included",
                            ValueNameYesOrNo,
                            ARRAY_SIZE(ValueNameYesOrNo, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_phich_item,
                            "PHICH 1 Included", iPhich1Included);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_phich_item,
                            "PHICH 1 Included",
                            ValueNameYesOrNo,
                            ARRAY_SIZE(ValueNameYesOrNo, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_phich_item,
                            "PHICH Value", iPhichValue);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_phich_item,
                            "PHICH Value",
                            ValueNameAckOrNack,
                            ARRAY_SIZE(ValueNameAckOrNack, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_phich_item,
                            "PHICH 1 Value", iPhich1Value);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_phich_item,
                            "PHICH 1 Value",
                            ValueNameAckOrNack,
                            ARRAY_SIZE(ValueNameAckOrNack, ValueName),
                            "(MI)Unknown");

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_phich_item, "dict");
                    PyList_Append(result_phich, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_phich_item);
                }
                PyObject *t2_phich = Py_BuildValue("(sOs)", "PHICH",
                        result_phich, "list");
                PyList_Append(result_record_item, t2_phich);
                Py_DECREF(t2_phich);
                Py_DECREF(result_phich);

                // totally 12 bytes for all phich
                offset += 12 - iNumPhich * 4;

                PyObject *result_pdcch = PyList_New(0);
                for (int j = 0; j < iNumPdcch; j++) {
                    PyObject *result_pdcch_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v25_pdcch,
                            ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v25_pdcch,
                                Fmt),
                            b, offset, length, result_pdcch_item);
                    int iNonDecodePdcch_1 = _search_result_int(result_pdcch_item,
                            "Serv Cell Idx");
                    int iServCellIdx = iNonDecodePdcch_1 & 7; // last 3 bits
                    int iRNTI = (iNonDecodePdcch_1 >> 3) & 15; // next 4 bits
                    int iPayloadSize = (iNonDecodePdcch_1 >> 7) & 127; // next 7 bits
                    int iAggLv = (iNonDecodePdcch_1 >> 14) & 3; // next 2 bits
                    old_object = _replace_result_int(result_pdcch_item,
                            "Serv Cell Idx", iServCellIdx);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_pdcch_item,
                            "RNTI Type", iRNTI);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "RNTI Type",
                            ValueNameRNTIType,
                            ARRAY_SIZE(ValueNameRNTIType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_pdcch_item,
                            "Payload Size", iPayloadSize);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_pdcch_item,
                            "Aggregation Level", iAggLv);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "Aggregation Level",
                            ValueNameAggregationLevel,
                            ARRAY_SIZE(ValueNameAggregationLevel, ValueName),
                            "(MI)Unknown");

                    int iNonDecodePdcch_2 = _search_result_int(result_pdcch_item,
                            "Search Space");
                    int iSearchSpace = iNonDecodePdcch_2 & 1; // last 1 bit
                    // SPS Grant Type should take this 3 bits
                    int iNewDLTx = (iNonDecodePdcch_2 >> 4) & 1; // next 1 bit
                    int iNumDLTrblks = (iNonDecodePdcch_2 >> 5) & 3; // next 2 bits
                    old_object = _replace_result_int(result_pdcch_item,
                            "Search Space", iSearchSpace);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "Search Space",
                            ValueNameSearchSpaceType,
                            ARRAY_SIZE(ValueNameSearchSpaceType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_pdcch_item,
                            "New DL Tx", iNewDLTx);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "New DL Tx",
                            ValueNameTrueOrFalse,
                            ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_pdcch_item,
                            "Num DL Trblks", iNumDLTrblks);
                    Py_DECREF(old_object);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_pdcch_item, "dict");
                    PyList_Append(result_pdcch, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_pdcch_item);
                }
                PyObject *t2_pdcch = Py_BuildValue("(sOs)", "PDCCH Info",
                        result_pdcch, "list");
                PyList_Append(result_record_item, t2_pdcch);
                Py_DECREF(t2_pdcch);
                Py_DECREF(result_pdcch);

                // at most 64 bytes for all pdcch
                offset += 64 - iNumPdcch * 8;

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }
    case 33:
        {
            offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Fmt_v33,
                    ARRAY_SIZE(LtePdcchPhichIndicationReport_Fmt_v33, Fmt),
                    b, offset, length, result);
            int iNonDecodeDuplexMode = _search_result_int(result,
                    "Duplex Mode");
            int iDuplexMode = iNonDecodeDuplexMode & 3; // last 2 bits
            PyObject *old_object = _replace_result_int(result, "Duplex Mode",
                    iDuplexMode);
            Py_DECREF(old_object);
            int num_record = _search_result_int(result, "Number of Records");

            PyObject *result_record = PyList_New(0);
            for (int i = 0; i < num_record; i++) {
                PyObject *result_record_item = PyList_New(0);
                offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v33_p1,
                        ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v33_p1, Fmt),
                        b, offset, length, result_record_item);
                unsigned int iNonDecodeP1_1 = _search_result_uint(result_record_item,
                        "Num PDCCH Results");
                int iNumPdcch = iNonDecodeP1_1 & 7; // last 3 bits
                int iNumPhich = (iNonDecodeP1_1 >> 3) & 7;  // next 3 bits
                int iPdcchSFN = (iNonDecodeP1_1 >> 6) & 1023; // next 10 bits
                int iPdcchSubFN = (iNonDecodeP1_1 >> 16) & 15; // next 4 bits
                old_object = _replace_result_int(result_record_item,
                        "Num PDCCH Results", iNumPdcch);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "Num PHICH Results", iNumPhich);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PDCCH Timing SFN", iPdcchSFN);
                Py_DECREF(old_object);
                old_object = _replace_result_int(result_record_item,
                        "PDCCH Timing Sub-FN", iPdcchSubFN);
                Py_DECREF(old_object);

                PyObject *result_phich = PyList_New(0);
                for (int j = 0; j < iNumPhich; j++) {
                    PyObject *result_phich_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v25_phich,
                            ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v25_phich,
                                Fmt),
                            b, offset, length, result_phich_item);
                    unsigned int iNonDecode_phich = _search_result_uint(
                            result_phich_item, "Cell Index");
                    int iCellIndex = iNonDecode_phich & 7;    // 3 bits
                    int iPhichIncluded = (iNonDecode_phich >> 3) & 1; // next 1 bit
                    int iPhich1Included = (iNonDecode_phich >> 4) & 1; // next 1 bit
                    int iPhichValue = (iNonDecode_phich >> 5) & 1; // next 1 bit
                    int iPhich1Value = (iNonDecode_phich >> 6) & 1; // next 1 bit

                    old_object = _replace_result_int(result_phich_item,
                            "Cell Index", iCellIndex);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_phich_item,
                            "PHICH Included", iPhichIncluded);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_phich_item,
                            "PHICH Included",
                            ValueNameYesOrNo,
                            ARRAY_SIZE(ValueNameYesOrNo, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_phich_item,
                            "PHICH 1 Included", iPhich1Included);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_phich_item,
                            "PHICH 1 Included",
                            ValueNameYesOrNo,
                            ARRAY_SIZE(ValueNameYesOrNo, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_phich_item,
                            "PHICH Value", iPhichValue);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_phich_item,
                            "PHICH Value",
                            ValueNameAckOrNack,
                            ARRAY_SIZE(ValueNameAckOrNack, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_phich_item,
                            "PHICH 1 Value", iPhich1Value);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_phich_item,
                            "PHICH 1 Value",
                            ValueNameAckOrNack,
                            ARRAY_SIZE(ValueNameAckOrNack, ValueName),
                            "(MI)Unknown");

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_phich_item, "dict");
                    PyList_Append(result_phich, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_phich_item);
                }
                PyObject *t2_phich = Py_BuildValue("(sOs)", "PHICH",
                        result_phich, "list");
                PyList_Append(result_record_item, t2_phich);
                Py_DECREF(t2_phich);
                Py_DECREF(result_phich);

                // totally 20 bytes for all phich
                offset += 20 - iNumPhich * 4;

                PyObject *result_pdcch = PyList_New(0);
                for (int j = 0; j < iNumPdcch; j++) {
                    PyObject *result_pdcch_item = PyList_New(0);
                    offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Record_v33_pdcch,
                            ARRAY_SIZE(LtePdcchPhichIndicationReport_Record_v33_pdcch,
                                Fmt),
                            b, offset, length, result_pdcch_item);
                    int iNonDecodePdcch_1 = _search_result_int(result_pdcch_item,
                            "Serv Cell Idx");
                    int iServCellIdx = iNonDecodePdcch_1 & 7; // last 3 bits
                    int iRNTI = (iNonDecodePdcch_1 >> 3) & 15; // next 4 bits
                    int iPayloadSize = (iNonDecodePdcch_1 >> 7) & 127; // next 7 bits
                    int iAggLv = (iNonDecodePdcch_1 >> 14) & 3; // next 2 bits
                    old_object = _replace_result_int(result_pdcch_item,
                            "Serv Cell Idx", iServCellIdx);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_pdcch_item,
                            "RNTI Type", iRNTI);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "RNTI Type",
                            ValueNameRNTIType,
                            ARRAY_SIZE(ValueNameRNTIType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_pdcch_item,
                            "Payload Size", iPayloadSize);
                    Py_DECREF(old_object);
                    old_object = _replace_result_int(result_pdcch_item,
                            "Aggregation Level", iAggLv);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "Aggregation Level",
                            ValueNameAggregationLevel,
                            ARRAY_SIZE(ValueNameAggregationLevel, ValueName),
                            "(MI)Unknown");

                    int iNonDecodePdcch_2 = _search_result_int(result_pdcch_item,
                            "Search Space");
                    int iSearchSpace = iNonDecodePdcch_2 & 1; // last 1 bit
                    // SPS Grant Type should take this 3 bits
                    int iNewDLTx = (iNonDecodePdcch_2 >> 4) & 1; // next 1 bit
                    int iNumDLTrblks = (iNonDecodePdcch_2 >> 5) & 3; // next 2 bits
                    old_object = _replace_result_int(result_pdcch_item,
                            "Search Space", iSearchSpace);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "Search Space",
                            ValueNameSearchSpaceType,
                            ARRAY_SIZE(ValueNameSearchSpaceType, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_pdcch_item,
                            "New DL Tx", iNewDLTx);
                    Py_DECREF(old_object);
                    (void) _map_result_field_to_name(result_pdcch_item,
                            "New DL Tx",
                            ValueNameTrueOrFalse,
                            ARRAY_SIZE(ValueNameTrueOrFalse, ValueName),
                            "(MI)Unknown");
                    old_object = _replace_result_int(result_pdcch_item,
                            "Num DL Trblks", iNumDLTrblks);
                    Py_DECREF(old_object);

                    PyObject *t3 = Py_BuildValue("(sOs)", "Ignored",
                            result_pdcch_item, "dict");
                    PyList_Append(result_pdcch, t3);
                    Py_DECREF(t3);
                    Py_DECREF(result_pdcch_item);
                }
                PyObject *t2_pdcch = Py_BuildValue("(sOs)", "PDCCH Info",
                        result_pdcch, "list");
                PyList_Append(result_record_item, t2_pdcch);
                Py_DECREF(t2_pdcch);
                Py_DECREF(result_pdcch);

                // at most 64 bytes for all pdcch
                offset += 64 - iNumPdcch * 8;

                PyObject *t1 = Py_BuildValue("(sOs)", "Ignored",
                        result_record_item, "dict");
                PyList_Append(result_record, t1);
                Py_DECREF(t1);
                Py_DECREF(result_record_item);
            }
            PyObject *t = Py_BuildValue("(sOs)", "Records",
                    result_record, "list");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(result_record);
            return offset - start;
        }

    default:
        printf("(MI)Unknown LTE PDCCH-PHICH Indication Report version: 0x%x\n", pkt_ver);
        return 0;
    }
}

// ----------------------------------------------------------------------------

//Yuanjie: decode modem's internal debugging message

static size_t
_decode_modem_debug_msg(const char *b, int offset, size_t length,
                                PyObject *result) {

    int start = offset;
    int argc = _search_result_int(result, "Number of parameters");
    char version = _search_result_int(result, "Version");

    if(version=='\x79'){
        //Yuanjie: optimization for iCellular. Don't show unnecessary logs
        return length-start;
    }
    else if(version=='\x92'){

        //Yuanjie: these logs are encoded in unknown approach (signature based?)
        //Currently only ad-hoc approach is available

        // Yuanjie: for this type of debugging message, a "fingerprint" exists before the parameters
        // The fingerprint seems the **unique** identifier of one message.
        // The message is pre-stored (thus not transferred) as a database.
        //
        argc++;
        long *tmp_argv = new long[argc];

        // printf("%d\n",argc); 

        //Get parameters
        for(int i=0; i!=argc; i++)
        {
            // const char *p = b + offset;

            // int ii = *((int *) p);    //a new parameter

            long ii = *((long *) (b + offset));

            tmp_argv[i] = ii;

            offset += 4;    //one parameter
        }


        // if(argc>=2)
        //     printf("argc=%d argv[0]=%x argv[1]=%d\n", argc, tmp_argv[0], tmp_argv[1]);

        if(argc==2 && tmp_argv[0]==0x2e3bbbed)
        {

             // Yuanjie: for icellular only, 4G RSRP result in manual network search
             // get log "BPLMN LOG: Saved measurement results. rsrp=-121"
             // The fingerprint is tmp_argv[0]==0xedbb3b2e, tmp_argv[1]=rsrp value
             //
            std::string res = "BPLMN LOG: Saved measurement results. rsrp=";
            // res+=std::to_string(tmp_argv[1]);
            res+=patch::to_string(tmp_argv[1]);
            PyObject *t = Py_BuildValue("(ss#s)", "Msg", res.c_str(), res.size(), "");
            PyList_Append(result, t);
            Py_DECREF(t);

            delete [] tmp_argv;
            return length-start;
        }
        else if(argc==10 && tmp_argv[0]==0x81700a47)
        {

             // Yuanjie: for icellular only, 3G RSRP result in manual network search
             // The fingerprint is tmp_argv[0]==0x81700a47, tmp_argv[1]=rscp value
             //
            std::string res = "Freq=%d psc=%d eng=%d filt_eng=%d rscp=%d RxAGC=%d 2*ecio=%d 2*squal=%d srxlv=%d";
            for(int i=1; i!=argc; i++)
            {

                std::size_t found = res.find("%d");
                // std::string tmp = std::to_string(tmp_argv[i]);
                std::string tmp = patch::to_string(tmp_argv[i]);
                res.replace(found,2,tmp);
            }
            PyObject *t = Py_BuildValue("(ss#s)", "Msg", res.c_str(), res.size(), "");
            PyList_Append(result, t);
            Py_DECREF(t);

            delete [] tmp_argv;
            return length-start;

        }
        else{

            //Ignore other unknown messages. Don't send to MobileInsight Analyzers
            // std::string res="(Unknown debug message)";
            // PyObject *t = Py_BuildValue("(ss#s)", "Msg", res.c_str(), res.size(), "");
            // PyList_Append(result, t);
            // Py_DECREF(t);

            delete [] tmp_argv;
            return length-start;

        }


    }
    return length-start;

}

bool
is_log_packet (const char *b, size_t length) {
    return length >= 2 && b[0] == '\x10';
}

bool
is_debug_packet (const char *b, size_t length) {
    return length >=2 && (b[0] ==  '\x79' || b[0] == '\x92');
    // return length >=2 && (b[0] == '\x92');  //Yuanjie: optimization for iCellular, avoid unuseful debug msg
    // return length >=2 && (b[0] ==  '\x79');
}

void 
on_demand_decode (const char *b, size_t length, LogPacketType type_id, PyObject* result)
{
    int offset = 0;
    switch (type_id) {
        case CDMA_Paging_Channel_Message:
            // Not fully support.
            offset += _decode_by_fmt(CdmaPagingChannelMsg_Fmt,
                    ARRAY_SIZE(CdmaPagingChannelMsg_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_cdma_paging_channel_msg(b, offset, length, result);
            break;

        // Yuanjie: Incomplete support. Disable it temporarily
        case _1xEV_Signaling_Control_Channel_Broadcast:
            offset += _decode_by_fmt(_1xEVSignalingFmt,
                    ARRAY_SIZE(_1xEVSignalingFmt, Fmt),
                    b, offset, length, result);
            offset += _decode_1xev_signaling_control_channel_broadcast(
                    b, offset, length, result);
            break;

        case WCDMA_CELL_ID:
            offset += _decode_by_fmt(WcdmaCellIdFmt,
                                        ARRAY_SIZE(WcdmaCellIdFmt, Fmt),
                                        b, offset, length, result);
            break;

        case WCDMA_Signaling_Messages:
            offset += _decode_by_fmt(WcdmaSignalingMessagesFmt,
                                        ARRAY_SIZE(WcdmaSignalingMessagesFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_wcdma_signaling_messages(b, offset, length, result);
            break;

        case UMTS_NAS_GMM_State:
            offset += _decode_by_fmt(UmtsNasGmmStateFmt,
                                        ARRAY_SIZE(UmtsNasGmmStateFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_umts_nas_gmm_state(b, offset, length, result);
            break;

        case UMTS_NAS_MM_State:
            offset += _decode_by_fmt(UmtsNasMmStateFmt,
                                        ARRAY_SIZE(UmtsNasMmStateFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_umts_nas_mm_state(b, offset, length, result);
            break;

        case UMTS_NAS_MM_REG_State:
            offset += _decode_by_fmt(UmtsNasMmRegStateFmt,
                                        ARRAY_SIZE(UmtsNasMmRegStateFmt, Fmt),
                                        b, offset, length, result);
            break;

        case UMTS_NAS_OTA:
            offset += _decode_by_fmt(UmtsNasOtaFmt,
                                        ARRAY_SIZE(UmtsNasOtaFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_umts_nas_ota(b, offset, length, result);
            break;

        case LTE_RRC_OTA_Packet:
            offset += _decode_by_fmt(LteRrcOtaPacketFmt,
                                        ARRAY_SIZE(LteRrcOtaPacketFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_rrc_ota(b, offset, length, result);
            break;

        case LTE_RRC_MIB_Message_Log_Packet:
            offset += _decode_by_fmt(LteRrcMibMessageLogPacketFmt,
                                        ARRAY_SIZE(LteRrcMibMessageLogPacketFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_rrc_mib(b, offset, length, result);
            break;

        case LTE_RRC_Serv_Cell_Info_Log_Packet:
            offset += _decode_by_fmt(LteRrcServCellInfoLogPacketFmt,
                                        ARRAY_SIZE(LteRrcServCellInfoLogPacketFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_rrc_serv_cell_info(b, offset, length, result);
            break;

        case LTE_NAS_ESM_Plain_OTA_Incoming_Message:
        case LTE_NAS_ESM_Plain_OTA_Outgoing_Message:
        case LTE_NAS_EMM_Plain_OTA_Incoming_Message:
        case LTE_NAS_EMM_Plain_OTA_Outgoing_Message:
            offset += _decode_by_fmt(LteNasPlainFmt,
                                        ARRAY_SIZE(LteNasPlainFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_nas_plain(b, offset, length, result);
            break;

        case LTE_NAS_EMM_State:
            offset += _decode_by_fmt(LteNasEmmStateFmt,
                                        ARRAY_SIZE(LteNasEmmStateFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_nas_emm_state(b, offset, length, result);
            break;

        case LTE_NAS_ESM_State:
            offset += _decode_by_fmt(LteNasEsmStateFmt,
                                        ARRAY_SIZE(LteNasEsmStateFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_nas_esm_state(b, offset, length, result);
            break;

        case LTE_PHY_PDSCH_Demapper_Configuration:
            offset += _decode_by_fmt(LtePhyPdschDemapperConfigFmt,
                                        ARRAY_SIZE(LtePhyPdschDemapperConfigFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_phy_pdsch_demapper_config(b, offset, length, result);
            break;

        case LTE_PHY_Connected_Mode_LTE_Intra_Freq_Meas_Results:
            offset += _decode_by_fmt(LtePhyCmlifmrFmt,
                                        ARRAY_SIZE(LtePhyCmlifmrFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_phy_cmlifmr(b, offset, length, result);
            break;

        case LTE_PHY_Serving_Cell_Measurement_Result:
            offset += _decode_by_fmt(LtePhySubpktFmt,
                                        ARRAY_SIZE(LtePhySubpktFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_phy_subpkt(b, offset, length, result);
            break;

        case LTE_PHY_IRAT_MDB:
            offset += _decode_by_fmt(LtePhyIratFmt,
                                        ARRAY_SIZE(LtePhyIratFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_phy_irat_subpkt(b, offset, length, result);
            break;

        case LTE_PHY_CDMA_MEAS:
            //It shares similar packet format as LTE_PHY_IRAT_MDB
            offset += _decode_by_fmt(LtePhyIratFmt,
                                        ARRAY_SIZE(LtePhyIratFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_phy_irat_cdma_subpkt(b, offset, length, result);
            break;

        case LTE_PDCP_DL_SRB_Integrity_Data_PDU:
            offset += _decode_by_fmt(LtePdcpDlSrbIntegrityDataPduFmt,
                                        ARRAY_SIZE(LtePdcpDlSrbIntegrityDataPduFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_pdcp_dl_srb_integrity_data_pdu(b, offset, length, result);
            break;

        case LTE_PDCP_UL_SRB_Integrity_Data_PDU:
            offset += _decode_by_fmt(LtePdcpUlSrbIntegrityDataPduFmt,
                                        ARRAY_SIZE(LtePdcpUlSrbIntegrityDataPduFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_pdcp_ul_srb_integrity_data_pdu(b, offset, length, result);
            break;

        case LTE_MAC_Configuration:// Jie
            offset += _decode_by_fmt(LteMacConfigurationFmt, ARRAY_SIZE(LteMacConfigurationFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_mac_configuration_subpkt(b, offset, length, result);
            break;

        case LTE_MAC_UL_Transport_Block:// Jie
            offset += _decode_by_fmt(LteMacULTransportBlockFmt, ARRAY_SIZE(LteMacULTransportBlockFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_mac_ul_transportblock_subpkt(b, offset, length, result);
            break;

        case LTE_MAC_DL_Transport_Block:// Jie
            offset += _decode_by_fmt(LteMacDLTransportBlockFmt,
                                        ARRAY_SIZE(LteMacDLTransportBlockFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_mac_dl_transportblock_subpkt(b, offset, length, result);
            break;

        case LTE_MAC_UL_Buffer_Status_Internal:// Jie
            offset += _decode_by_fmt(LteMacULBufferStatusInternalFmt,
                                        ARRAY_SIZE(LteMacULBufferStatusInternalFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_mac_ul_bufferstatusinternal_subpkt(b, offset, length, result);
            break;
        case LTE_MAC_UL_Tx_Statistics:// Jie
            offset += _decode_by_fmt(LteMacULTxStatisticsFmt,
                                        ARRAY_SIZE(LteMacULTxStatisticsFmt, Fmt),
                                        b, offset, length, result);
            offset += _decode_lte_mac_ul_txstatistics_subpkt(b, offset, length, result);
            break;

        case LTE_RLC_UL_Config_Log_Packet:
            offset += _decode_by_fmt(LteRlcUlConfigLogPacketFmt,
                    ARRAY_SIZE(LteRlcUlConfigLogPacketFmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_rlc_ul_config_log_packet_subpkt(b, offset,
                    length, result);
            break;

        case LTE_RLC_DL_Config_Log_Packet:
            offset += _decode_by_fmt(LteRlcDlConfigLogPacketFmt,
                    ARRAY_SIZE(LteRlcDlConfigLogPacketFmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_rlc_dl_config_log_packet_subpkt(b, offset,
                    length, result);
            break;

        case LTE_RLC_UL_AM_All_PDU:
            offset += _decode_by_fmt(LteRlcUlAmAllPduFmt,
                    ARRAY_SIZE(LteRlcUlAmAllPduFmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_rlc_ul_am_all_pdu_subpkt(b, offset,
                    length, result);
            break;

        case LTE_RLC_DL_AM_All_PDU:
            offset += _decode_by_fmt(LteRlcDlAmAllPduFmt,
                    ARRAY_SIZE(LteRlcDlAmAllPduFmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_rlc_dl_am_all_pdu_subpkt(b, offset,
                    length, result);
            break;

        case LTE_MAC_Rach_Trigger:
            offset += _decode_by_fmt(LteMacRachTriggerFmt,
                    ARRAY_SIZE(LteMacRachTriggerFmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_mac_rach_trigger_subpkt(b, offset, length,
                    result);
            break;
        case LTE_MAC_Rach_Attempt:
            offset += _decode_by_fmt(LteMacRachAttempt_Fmt,
                    ARRAY_SIZE(LteMacRachAttempt_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_mac_rach_attempt_subpkt(b, offset, length,
                    result);
            break;
        case LTE_PDCP_DL_Config:
            offset += _decode_by_fmt(LtePdcpDlConfig_Fmt,
                    ARRAY_SIZE(LtePdcpDlConfig_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pdcp_dl_config_subpkt(b, offset, length, result);
            break;
        case LTE_PDCP_UL_Config:
            offset += _decode_by_fmt(LtePdcpUlConfig_Fmt,
                    ARRAY_SIZE(LtePdcpUlConfig_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pdcp_ul_config_subpkt(b, offset, length, result);
            break;
        case LTE_PDCP_UL_Data_PDU:
            offset += _decode_by_fmt(LtePdcpUlDataPdu_Fmt,
                    ARRAY_SIZE(LtePdcpUlDataPdu_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pdcp_ul_data_pdu_subpkt(b, offset, length, result);
            break;
        case LTE_PDCP_DL_Stats:
            offset += _decode_by_fmt(LtePdcpDlStats_Fmt,
                    ARRAY_SIZE(LtePdcpDlStats_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pdcp_dl_stats_subpkt(b, offset, length, result);
            break;
        case LTE_PDCP_UL_Stats:
            offset += _decode_by_fmt(LtePdcpUlStats_Fmt,
                    ARRAY_SIZE(LtePdcpUlStats_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pdcp_ul_stats_subpkt(b, offset, length, result);
            break;
        case LTE_RLC_UL_Stats:
            offset += _decode_by_fmt(LteRlcUlStats_Fmt,
                    ARRAY_SIZE(LteRlcUlStats_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_rlc_ul_stats_subpkt(b, offset, length, result);
            break;
        case LTE_RLC_DL_Stats:
            offset += _decode_by_fmt(LteRlcDlStats_Fmt,
                    ARRAY_SIZE(LteRlcDlStats_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_rlc_dl_stats_subpkt(b, offset, length, result);
            break;
        case LTE_PDCP_DL_Ctrl_PDU:
            offset += _decode_by_fmt(LtePdcpDlCtrlPdu_Fmt,
                    ARRAY_SIZE(LtePdcpDlCtrlPdu_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pdcp_dl_ctrl_pdu_subpkt(b, offset, length, result);
            break;
        case LTE_PDCP_UL_Ctrl_PDU:
            offset += _decode_by_fmt(LtePdcpDlCtrlPdu_Fmt,
                    ARRAY_SIZE(LtePdcpDlCtrlPdu_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pdcp_ul_ctrl_pdu_subpkt(b, offset, length, result);
            break;
        case LTE_PUCCH_Power_Control:
            offset += _decode_by_fmt(LtePucchPowerControl_Fmt,
                    ARRAY_SIZE(LtePucchPowerControl_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pucch_power_control_payload(b, offset, length, result);
            break;
        case LTE_PUSCH_Power_Control:
            offset += _decode_by_fmt(LtePuschPowerControl_Fmt,
                    ARRAY_SIZE(LtePuschPowerControl_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pusch_power_control_payload(b, offset, length, result);
            break;
        case LTE_PDCCH_PHICH_Indication_Report:
            offset += _decode_by_fmt(LtePdcchPhichIndicationReport_Fmt,
                    ARRAY_SIZE(LtePdcchPhichIndicationReport_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pdcch_phich_indication_report_payload(b, offset, length, result);
            break;
        case _1xEV_Rx_Partial_MultiRLP_Packet:
            offset += _decode_by_fmt(_1xEVRxPartialMultiRLPPacket_Fmt,
                    ARRAY_SIZE(_1xEVRxPartialMultiRLPPacket_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_1xev_rx_partial_multirlp_packet_payload(b, offset, length, result);
            break;
        case _1xEV_Connected_State_Search_Info:
            offset += _decode_by_fmt(_1xEVConnectedStateSearchInfo_Fmt,
                    ARRAY_SIZE(_1xEVConnectedStateSearchInfo_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_1xev_connected_state_search_info_payload(b, offset, length, result);
            break;
        case _1xEV_Connection_Attempt:
            offset += _decode_by_fmt(_1xEVConnectionAttempt_Fmt,
                    ARRAY_SIZE(_1xEVConnectionAttempt_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_1xev_connection_attempt_payload(b, offset, length, result);
            break;
        case _1xEV_Connection_Release:
            offset += _decode_by_fmt(_1xEVConnectionRelease_Fmt,
                    ARRAY_SIZE(_1xEVConnectionRelease_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_1xev_connection_release_payload(b, offset, length, result);
            break;
        case LTE_PDSCH_Stat_Indication:
            offset += _decode_by_fmt(LtePdschStatIndication_Fmt,
                    ARRAY_SIZE(LtePdschStatIndication_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pdsch_stat_indication_payload(b, offset, length, result);
            break;
        case LTE_PHY_System_Scan_Results:
            offset += _decode_by_fmt(LtePhySystemScanResults_Fmt,
                    ARRAY_SIZE(LtePhySystemScanResults_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_system_scan_results_payload(b, offset, length, result);
            break;
        case LTE_PHY_BPLMN_Cell_Request:
            offset += _decode_by_fmt(LtePhyBplmnCellRequest_Fmt,
                    ARRAY_SIZE(LtePhyBplmnCellRequest_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_bplmn_cell_request_payload(b, offset, length, result);
            break;
        case LTE_PHY_BPLMN_Cell_Confirm:
            offset += _decode_by_fmt(LtePhyBplmnCellConfirm_Fmt,
                    ARRAY_SIZE(LtePhyBplmnCellConfirm_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_bplmn_cell_confirm_payload(b, offset, length, result);
            break;
        case LTE_PHY_Serving_Cell_COM_Loop:
            offset += _decode_by_fmt(LtePhyServingCellComLoop_Fmt,
                    ARRAY_SIZE(LtePhyServingCellComLoop_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_serving_cell_com_loop_payload(b, offset, length, result);
            break;
        case LTE_PHY_PDCCH_Decoding_Result:
            offset += _decode_by_fmt(LtePhyPdcchDecodingResult_Fmt,
                    ARRAY_SIZE(LtePhyPdcchDecodingResult_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_pdcch_decoding_result_payload(b, offset, length, result);
            break;
        case LTE_PHY_PDSCH_Decoding_Result:
            offset += _decode_by_fmt(LtePhyPdschDecodingResult_Fmt,
                    ARRAY_SIZE(LtePhyPdschDecodingResult_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_pdsch_decoding_result_payload(b, offset, length, result);
            break;
        case LTE_PHY_PUSCH_Tx_Report:
            offset += _decode_by_fmt(LtePhyPuschTxReport_Fmt,
                    ARRAY_SIZE(LtePhyPuschTxReport_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_pusch_tx_report_payload(b, offset, length, result);
            break;
        case LTE_PHY_RLM_Report:
            offset += _decode_by_fmt(LtePhyRlmReport_Fmt,
                    ARRAY_SIZE(LtePhyRlmReport_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_rlm_report_payload(b, offset, length, result);
            break;
        case LTE_PHY_PUSCH_CSF:
            offset += _decode_by_fmt(LtePhyPuschCsf_Fmt,
                    ARRAY_SIZE(LtePhyPuschCsf_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_pusch_csf_payload(b, offset, length, result);
            break;
        case LTE_PHY_CDRX_Events_Info:
            offset += _decode_by_fmt(LtePhyCdrxEventsInfo_Fmt,
                    ARRAY_SIZE(LtePhyCdrxEventsInfo_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_cdrx_events_info_payload(b, offset, length, result);
            break;
        case WCDMA_RRC_States:
            offset += _decode_wcdma_rrc_states_payload(b, offset, length, result);
            break;
        case LTE_PHY_Idle_Neighbor_Cell_Meas:
            offset += _decode_by_fmt(LtePhyIncm_Fmt,
                    ARRAY_SIZE(LtePhyIncm_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_idle_neighbor_cell_meas_payload(b, offset, length, result);
            break;
        case WCDMA_Search_Cell_Reselection_Rank:
            offset += _decode_by_fmt(WcdmaScrr_Fmt,
                    ARRAY_SIZE(WcdmaScrr_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_wcdma_scrr_payload(b, offset, length, result);
            break;
        case GSM_RR_Cell_Information:
            offset += _decode_by_fmt(GsmRrCellInfo_Fmt,
                    ARRAY_SIZE(GsmRrCellInfo_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_gsm_rci_payload(b, offset, length, result);
            break;
        case GSM_RR_Cell_Reselection_Parameters:
            offset += _decode_by_fmt(GsmRrCellResParm_Fmt,
                    ARRAY_SIZE(GsmRrCellResParm_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_gsm_rcrp_payload(b, offset, length, result);
            break;
        case GSM_Surround_Cell_BA_List:
            offset += _decode_by_fmt(GsmScbl_Fmt,
                    ARRAY_SIZE(GsmScbl_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_gsm_scbl_payload(b, offset, length, result);
            break;
        case GSM_RR_Cell_Reselection_Meas:
            offset += _decode_by_fmt(GsmRrCellResMeas_Fmt,
                    ARRAY_SIZE(GsmRrCellResMeas_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_gsm_rcrm_payload(b, offset, length, result);
            break;
        case GSM_RR_Signaling_Message:
            offset += _decode_by_fmt(GsmRrSignalingMsg_Fmt,
                    ARRAY_SIZE(GsmRrSignalingMsg_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_gsm_rr_signaling_msg_payload(
                    b, offset, length, result);
            break;
        case GSM_DSDS_RR_Cell_Information:
            offset += _decode_by_fmt(GsmDsdsRrCellInfo_Fmt,
                    ARRAY_SIZE(GsmDsdsRrCellInfo_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_gsm_drci_payload(b, offset, length, result);
            break;
        case GSM_DSDS_RR_Cell_Reselection_Parameters:
            offset += _decode_by_fmt(GsmDsdsRrCellResParm_Fmt,
                    ARRAY_SIZE(GsmDsdsRrCellResParm_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_gsm_drcrp_payload(b, offset, length, result);
            break;
        case GSM_DSDS_RR_Signaling_Message:
            offset += _decode_by_fmt(GsmDsdsRrSignalingMsg_Fmt,
                    ARRAY_SIZE(GsmDsdsRrSignalingMsg_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_gsm_dsds_rr_signaling_msg_payload(
                    b, offset, length, result);
            break;

        case Srch_TNG_1x_Searcher_Dump:
            offset += _decode_by_fmt(SrchTng1xsd_Fmt,
                    ARRAY_SIZE(SrchTng1xsd_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_srch_tng_1xsd_payload(b, offset, length, result);
            break;
        case _1xEVDO_Multi_Carrier_Pilot_Sets:
            offset += _decode_by_fmt(_1xEvdoMcps_Fmt,
                    ARRAY_SIZE(_1xEvdoMcps_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_1xevdo_mcps_payload(b, offset, length, result);
            break;
        case LTE_PHY_PUCCH_Tx_Report:
            offset += _decode_by_fmt(LtePhyPucchTxReport_Fmt,
                    ARRAY_SIZE(LtePhyPucchTxReport_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_pucch_tx_report_payload(b, offset, length, result);
            break;
        case LTE_PDCP_DL_Cipher_Data_PDU:
            offset += _decode_by_fmt(LtePdcpDlCipherDataPdu_Fmt,
                    ARRAY_SIZE(LtePdcpDlCipherDataPdu_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pdcp_dl_cipher_data_pdu_payload(b, offset, length, result);
            break;
        case LTE_PDCP_UL_Cipher_Data_PDU:
            offset += _decode_by_fmt(LtePdcpUlCipherDataPdu_Fmt,
                    ARRAY_SIZE(LtePdcpUlCipherDataPdu_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_pdcp_ul_cipher_data_pdu_payload(b, offset, length, result);
            break;
        case LTE_PHY_PUCCH_CSF:
            offset += _decode_by_fmt(LtePhyPucchCsf_Fmt,
                    ARRAY_SIZE(LtePhyPucchCsf_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_pucch_csf_payload(b, offset, length, result);
            break;
        case LTE_PHY_Connected_Mode_Neighbor_Meas_Req_Resp:
            offset += _decode_by_fmt(LtePhyCncm_Fmt,
                    ARRAY_SIZE(LtePhyCncm_Fmt, Fmt),
                    b, offset, length, result);
            offset += _decode_lte_phy_connected_neighbor_cell_meas_payload(b, offset, length, result);
            break;
        default:
            break;
    };

}

PyObject *
decode_log_packet (const char *b, size_t length, bool skip_decoding) {

    if (PyDateTimeAPI == NULL)  // import datetime module
        PyDateTime_IMPORT;

    PyObject *result = NULL;
    int offset = 0;

    // Parse Header
    result = PyList_New(0);
    offset = 0;
    offset += _decode_by_fmt(LogPacketHeaderFmt, ARRAY_SIZE(LogPacketHeaderFmt, Fmt),
                                b, offset, length, result);
    // PyObject *old_result = result;
    // result = PyList_GetSlice(result, 1, 4); // remove the duplicate "len1" field
    // Py_DECREF(old_result);
    // old_result = NULL;

    // Differentiate using type ID
    LogPacketType type_id = (LogPacketType) _map_result_field_to_name(
                                result,
                                "type_id",
                                LogPacketTypeID_To_Name,
                                ARRAY_SIZE(LogPacketTypeID_To_Name, ValueName),
                                "Unsupported");

    if (skip_decoding) {    // skip further decoding

        PyObject *t = Py_BuildValue("(ss#s)",
                                "Msg", b + offset, length-offset,
                                "raw_msg/skip_decoding");
        PyList_Append(result, t);
        Py_DECREF(t);
        return result;
    }

    on_demand_decode(b+offset, length-offset, type_id, result);

    return result;
}

/*-----------------------------------------------------------------------
 * DEBUG ONLY
 */

static int
_decode_by_fmt_modem (const Fmt fmt [], int n_fmt,
                const char *b, int offset, size_t length,
                PyObject *result) {
    (void)length;
    assert(PyList_Check(result));
    int n_consumed = 0;

    Py_INCREF(result);
    for (int i = 0; i < n_fmt; i++) {
        PyObject *decoded = NULL;
        const char *p = b + offset + n_consumed;
        switch (fmt[i].type) {
        case UINT:
            {
                unsigned int ii = 0;
                unsigned long long iiii = 0;
                switch (fmt[i].len) {
                case 1:
                    ii = *((unsigned char *) p);
                    break;
                case 2:
                    ii = *((unsigned short *) p);
                    break;
                case 4:
                    ii = *((unsigned int *) p);
                    break;
                case 8:
                    // iiii = *((unsigned long long *) p);
                    iiii = 0;   //Yuanjie: quick work-around to avoid crash. For modem debug, this is timestamp
                    break;
                default:
                    assert(false);
                    break;
                }
                // Convert to a Python integer object or a Python long integer object
                // TODO: make it little endian
                if (fmt[i].len <= 4)
                    decoded = Py_BuildValue("I", ii);
                else
                    decoded = Py_BuildValue("K", iiii);
                n_consumed += fmt[i].len;
                break;
            }

        case QCDM_TIMESTAMP:
            {
                const double PER_SECOND = 52428800.0;
                const double PER_USECOND = 52428800.0 / 1.0e6;
                assert(fmt[i].len == 8);
                // Convert to a Python long integer object
                // unsigned long long iiii = *((unsigned long long *) p);
                unsigned long long iiii = 0;    //Yuanjie: FIX crash on Android
                int seconds = int(double(iiii) / PER_SECOND);
                int useconds = (double(iiii) / PER_USECOND) - double(seconds) * 1.0e6;
                PyObject *epoch = PyDateTime_FromDateAndTime(1980, 1, 6, 0, 0, 0, 0);
                PyObject *delta = PyDelta_FromDSU(0, seconds, useconds);
                decoded = PyNumber_Add(epoch, delta);
                n_consumed += fmt[i].len;
                Py_DECREF(epoch);
                Py_DECREF(delta);
                break;
            }

        case SKIP:
            n_consumed += fmt[i].len;
            break;

        default:
            assert(false);
            break;
        }

        if (decoded != NULL) {
            PyObject *t = Py_BuildValue("(sOs)",
                                        fmt[i].field_name, decoded, "");
            PyList_Append(result, t);
            Py_DECREF(t);
            Py_DECREF(decoded);
        }
    }
    Py_DECREF(result);
    return n_consumed;
}



PyObject *
decode_log_packet_modem (const char *b, size_t length, bool skip_decoding) {
    if (PyDateTimeAPI == NULL)  // import datetime module
        PyDateTime_IMPORT;

    PyObject *result = NULL;
    int offset = 0;

    // Parse Header
    result = PyList_New(0);
    offset = 0;
    offset += _decode_by_fmt_modem(LogPacketHeaderFmt, ARRAY_SIZE(LogPacketHeaderFmt, Fmt),
                                b, offset, length, result);

    PyObject *old_result = result;
    result = PyList_GetSlice(result, 1, 4); // remove the duplicate "len1" field
    Py_DECREF(old_result);
    old_result = NULL;


    // Differentiate using type ID
    LogPacketType type_id = (LogPacketType) _map_result_field_to_name(
                                result,
                                "type_id",
                                LogPacketTypeID_To_Name,
                                ARRAY_SIZE(LogPacketTypeID_To_Name, ValueName),
                                "Unsupported");

    if (skip_decoding) {    // skip further decoding
        return result;
    }

    switch (type_id) {

    case Modem_debug_message: //Yuanjie: modem debugging message
        offset += _decode_by_fmt_modem(ModemDebug_Fmt,
                                    ARRAY_SIZE(ModemDebug_Fmt, Fmt),
                                    b, offset, length, result);
        offset += _decode_modem_debug_msg(b, offset, length, result); 
        break;


    default:
        break;
    };


    return result;
}
