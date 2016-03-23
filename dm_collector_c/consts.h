/* consts.h
 * Author: Jiayao Li
 * Defines constants general to the whole module.
 */

#ifndef __DM_COLLECTOR_C_CONSTS_H__
#define __DM_COLLECTOR_C_CONSTS_H__

#include "utils.h"

const int EXPOSE_INTERNAL_LOGS=1;  //Yuanjie: 1 means expose all logs, 0 means only public logs are exposed

// All supported type IDs
enum LogPacketType {
    CDMA_Paging_Channel_Message = 0x1007,
    _1xEV_Signaling_Control_Channel_Broadcast = 0x107C,
    WCDMA_CELL_ID = 0x4127,
    WCDMA_Signaling_Messages = 0x412F,
    UMTS_NAS_GMM_State = 0x7130,
    UMTS_NAS_MM_State = 0x7131,
    UMTS_NAS_OTA = 0x713A,
    LTE_RRC_OTA_Packet = 0xB0C0,
    LTE_RRC_MIB_Message_Log_Packet = 0xB0C1,
    LTE_RRC_Serv_Cell_Info_Log_Packet = 0xB0C2,
    LTE_NAS_ESM_Plain_OTA_Incoming_Message = 0xB0E2,
    LTE_NAS_ESM_Plain_OTA_Outgoing_Message = 0xB0E3,
    LTE_NAS_EMM_Plain_OTA_Incoming_Message = 0xB0EC,
    LTE_NAS_EMM_Plain_OTA_Outgoing_Message = 0xB0ED,
    LTE_NAS_EMM_State = 0xB0EE,
    LTE_LL1_PDSCH_Demapper_Configuration = 0xB126,
    LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results = 0xB179,
    LTE_ML1_IRAT_Measurement_Request = 0xB187,
    LTE_ML1_Serving_Cell_Measurement_Result = 0xB193,
    LTE_ML1_Connected_Mode_Neighbor_Meas_Req_Resp = 0xB195,
    LTE_PDCP_DL_SRB_Integrity_Data_PDU = 0xB0A5,
    LTE_PDCP_UL_SRB_Integrity_Data_PDU = 0xB0B5,
    LTE_MAC_Configuration = 0xB060,
    LTE_MAC_DL_Transport_Block = 0xB063,
    LTE_MAC_UL_Transport_Block = 0xB064,
    LTE_MAC_UL_Buffer_Status_Internal = 0xB066,
    LTE_MAC_UL_Tx_Statistics = 0xB067,
    Modem_debug_message = 0x1FEB,
};

// Mapping type IDs to names
const ValueName LogPacketTypeID_To_Name [] = {
    {CDMA_Paging_Channel_Message,
        "CDMA_Paging_Channel_Message",false},
    {_1xEV_Signaling_Control_Channel_Broadcast,
        "1xEV_Signaling_Control_Channel_Broadcast",false},
    {WCDMA_CELL_ID, // WCDMA cell status
        "WCDMA_CELL_ID",false},
    {WCDMA_Signaling_Messages,  // WCDMA signaling messages
        "WCDMA_Signaling_Messages",true},
    {UMTS_NAS_GMM_State,
        "UMTS_NAS_GMM_State",false},
    {UMTS_NAS_MM_State,
        "UMTS_NAS_MM_State",false},
    {UMTS_NAS_OTA,
        "UMTS_NAS_OTA",true},
    {LTE_RRC_OTA_Packet,    // LTE RRC signaling messages
        "LTE_RRC_OTA_Packet",true},
    {LTE_RRC_MIB_Message_Log_Packet,    // LTE RRC MIB packet
        "LTE_RRC_MIB_Message_Log_Packet",true},
    {LTE_RRC_Serv_Cell_Info_Log_Packet, // LTE RRC cell status
        "LTE_RRC_Serv_Cell_Info_Log_Packet",false},
    {LTE_NAS_ESM_Plain_OTA_Incoming_Message,    // LTE ESM signaling message (merge incoming/outgoing)
        "LTE_NAS_ESM_Plain_OTA_Incoming_Message",true},
    {LTE_NAS_ESM_Plain_OTA_Outgoing_Message,    // LTE ESM signaling message (merge incoming/outgoing)
        "LTE_NAS_ESM_Plain_OTA_Outgoing_Message",true},
    {LTE_NAS_EMM_Plain_OTA_Incoming_Message,    // LTE EMM signaling message (merge incoming/outgoing)
        "LTE_NAS_EMM_Plain_OTA_Incoming_Message",true},
    {LTE_NAS_EMM_Plain_OTA_Outgoing_Message,    // LTE EMM signaling message (merge incoming/outgoing)
        "LTE_NAS_EMM_Plain_OTA_Outgoing_Message",true},
    {LTE_NAS_EMM_State,
        "LTE_NAS_EMM_State",false},
    {LTE_LL1_PDSCH_Demapper_Configuration,
        "LTE_LL1_PDSCH_Demapper_Configuration",false},
    {LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results,    // LTE infra-frequency measurements (connected mode)
        "LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results",false},
    {LTE_ML1_IRAT_Measurement_Request,  // LTE inter-RAT measurement request
        "LTE_ML1_IRAT_Measurement_Request",false},
    {LTE_ML1_Serving_Cell_Measurement_Result,   // LTE serving cell measurement result
        "LTE_ML1_Serving_Cell_Measurement_Result",false},
    {LTE_ML1_Connected_Mode_Neighbor_Meas_Req_Resp, // LTE neighbor measurement request (connected mode)
        "LTE_ML1_Connected_Mode_Neighbor_Meas_Req_Resp",false},
    {LTE_PDCP_DL_SRB_Integrity_Data_PDU,
        "LTE_PDCP_DL_SRB_Integrity_Data_PDU",false},
    {LTE_PDCP_UL_SRB_Integrity_Data_PDU,
        "LTE_PDCP_UL_SRB_Integrity_Data_PDU",false},
    {LTE_MAC_Configuration,
        "LTE_MAC_Configuration",false},
    {LTE_MAC_UL_Transport_Block,
        "LTE_MAC_UL_Transport_Block",true},
    {LTE_MAC_DL_Transport_Block,
        "LTE_MAC_DL_Transport_Block",true},
    {LTE_MAC_UL_Buffer_Status_Internal,
        "LTE_MAC_UL_Buffer_Status_Internal",false},
    {LTE_MAC_UL_Tx_Statistics,
        "LTE_MAC_UL_Tx_Statistics",false},
    {Modem_debug_message,
        "Modem_debug_message",false},
};

#endif	// __DM_COLLECTOR_C_CONSTS_H__
