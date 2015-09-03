#ifndef __DM_COLLECTOR_C_CONSTS_H__
#define __DM_COLLECTOR_C_CONSTS_H__

#include "utils.h"

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
    LTE_ML1_Connected_Mode_Neighbor_Meas_Req_Resp = 0xB195
};

const ValueName LogPacketTypeID_To_Name [] = {
    {CDMA_Paging_Channel_Message,
        "CDMA_Paging_Channel_Message"},
    {_1xEV_Signaling_Control_Channel_Broadcast,
        "1xEV_Signaling_Control_Channel_Broadcast"},
    {WCDMA_CELL_ID, // WCDMA cell status
        "WCDMA_CELL_ID"},
    {WCDMA_Signaling_Messages,  // WCDMA signaling messages
        "WCDMA_Signaling_Messages"},
    {UMTS_NAS_GMM_State,
        "UMTS_NAS_GMM_State"},
    {UMTS_NAS_MM_State,
        "UMTS_NAS_MM_State"},
    {UMTS_NAS_OTA,
        "UMTS_NAS_OTA"},
    {LTE_RRC_OTA_Packet,    // LTE RRC signaling messages
        "LTE_RRC_OTA_Packet"},
    {LTE_RRC_MIB_Message_Log_Packet,    // LTE RRC MIB packet
        "LTE_RRC_MIB_Message_Log_Packet"},
    {LTE_RRC_Serv_Cell_Info_Log_Packet, // LTE RRC cell status
        "LTE_RRC_Serv_Cell_Info_Log_Packet"},
    {LTE_NAS_ESM_Plain_OTA_Incoming_Message,    // LTE ESM signaling message (merge incoming/outgoing)
        "LTE_NAS_ESM_Plain_OTA_Incoming_Message"},
    {LTE_NAS_ESM_Plain_OTA_Outgoing_Message,    // LTE ESM signaling message (merge incoming/outgoing)
        "LTE_NAS_ESM_Plain_OTA_Outgoing_Message"},
    {LTE_NAS_EMM_Plain_OTA_Incoming_Message,    // LTE EMM signaling message (merge incoming/outgoing)
        "LTE_NAS_EMM_Plain_OTA_Incoming_Message"},
    {LTE_NAS_EMM_Plain_OTA_Outgoing_Message,    // LTE EMM signaling message (merge incoming/outgoing)
        "LTE_NAS_EMM_Plain_OTA_Outgoing_Message"},
    {LTE_NAS_EMM_State,
        "LTE_NAS_EMM_State"},
    {LTE_LL1_PDSCH_Demapper_Configuration,
        "LTE_LL1_PDSCH_Demapper_Configuration"},
    {LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results,    // LTE infra-frequency measurements (connected mode)
        "LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results"},
    {LTE_ML1_IRAT_Measurement_Request,  // LTE inter-RAT measurement request
        "LTE_ML1_IRAT_Measurement_Request"},
    {LTE_ML1_Serving_Cell_Measurement_Result,   // LTE serving cell measurement result
        "LTE_ML1_Serving_Cell_Measurement_Result"},
    {LTE_ML1_Connected_Mode_Neighbor_Meas_Req_Resp, // LTE neighbor measurement request (connected mode)
        "LTE_ML1_Connected_Mode_Neighbor_Meas_Req_Resp"}
};

#endif	// __DM_COLLECTOR_C_CONSTS_H__
