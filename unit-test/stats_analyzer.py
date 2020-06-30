#!/usr/bin/python
# Filename: stats_analyzer.py
"""
Get stats of meaning full message contents

Author: Haotian Deng
"""


from mobile_insight.analyzer.analyzer import *
from xml.dom import minidom
import datetime
import os
import pandas as pd

__all__=["StatsAnalyzer"]


# key:[nRB,TBS] -> value:Itbs


class StatsAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)
        self.mi2log = ""
        self.log_pdsch = {\
                'time':[],\
                'interval':[],\
                'servingCellId':[],\
                'rntiType':[],\
                'numTxAntennas':[],\
                'numRxAntennas':[],\
                'spatialRank':[],\
                'RB0':[],\
                'RB1':[],\
                'frequencySelectivePMI':[],\
                'pmiIndex':[],\
                'transmissionScheme':[],\
                'Modulation0':[],\
                'Modulation1':[],\
                'bitTBS0':[],\
                'bitTBS1':[],\
                'carrierIndex':[],\
                }
        self.log_pdschStats = {\
                'time':[],\
                'interval':[],\
                'numRecords':[],\
                'numRBs':[],\
                'numLayers':[],\
                'numTBPresent':[],\
                'servingCellIndex':[],\
                'hsicEnabled':[],\
                'harqId':[],\
                'rv':[],\
                'ndi':[],\
                'crcResult':[],\
                'rntiType':[],\
                'tbIndex':[],\
                'discardedReTxPresent':[],\
                'didRecombining':[],\
                'tbSize':[],\
                'mcs':[],\
                'tbNumRBs':[],\
                'ackNackDecision':[],\
                'pmchId':[],\
                'areaId':[],\
                }
        self.log_cmifm = {\
                'time':[],\
                'interval':[],\
                'earfcn':[],\
                'servingPhysicalCellId':[],\
                'servingRsrp':[],\
                'servingRsrq':[],\
                'numNeighborCells':[],\
                'numDetectedCells':[],\
                'neighborPhysicalCellId':[],\
                'neighborRsrp':[],\
                'neighborRsrq':[],\
                'detectedPhysicalCellId':[],\
                'detectedSSSCorrValue':[],\
                'detectedReferenceTime':[],\
               }

        self.log_rlm = {\
                'time':[],\
                'interval':[],\
                'numRecords':[],\
                'outofSyncBler':[],\
                'inSyncBler':[],\
                'outofSyncCount':[],\
                'inSyncCount':[],\
                't310TimerStatus':[],\
                }

        self.log_puschcsf = {\
                'time':[],\
                'interval':[],\
                'puschReportingMode':[],\
                'rankIndex':[],\
                'csiMeasSetIndex':[],\
                'numSubBands':[],\
                'widebandCQICW0':[],\
                'widebandCQICW1':[],\
                'subBandSize':[],\
                'singleWBPMI':[],\
                'singleMBPMI':[],\
                'csfTxMode':[],\
                'carrierIndex':[],\
                'numCSIrsPorts':[],\
               }

        self.log_pktname = {\
                'time':[],\
                'interval':[],\
                'key00':[],\
                'key01':[],\
                'key02':[],\
                'key03':[],\
                'key04':[],\
                'key05':[],\
                'key06':[],\
                'key07':[],\
                'key08':[],\
                'key09':[],\
                'key10':[],\
                'key11':[],\
                'key12':[],\
                'key13':[],\
                'key14':[],\
                'key15':[],\
                'key16':[],\
                'key17':[],\
                'key18':[],\
                'key19':[],\
                'key20':[],\
                'key21':[],\
                'key22':[],\
                'key23':[],\
                'key24':[],\
                'key25':[],\
                'key26':[],\
                'key27':[],\
                'key28':[],\
                'key29':[],\
                }

    def close(self):
        if len(self.log_pdsch['time']) > 0:
            data = pd.DataFrame(dict([ (k, pd.Series(v)) for k, v in list(self.log_pdsch.items()) ]))
            print("PDSCH Demapper Configuration")
            print("---------------------------------------------------------------")
            print("PDSCH Packet: interval")
            print((data['interval'].describe()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: Serving Cell ID")
            print((data['servingCellId'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: RNTI Type")
            print((data['rntiType'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: Num Tx Antennas")
            print((data['numTxAntennas'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: Num Rx Antennas")
            print((data['numRxAntennas'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: Spatial Rank")
            print((data['spatialRank'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: Number of RB 0")
            print((data['RB0'].describe()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: Number of RB 1")
            print((data['RB1'].describe()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: Frequency Selective PMI")
            print((data['frequencySelectivePMI'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: PMI Index")
            print((data['pmiIndex'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: Transmission Scheme")
            print((data['transmissionScheme'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: Modulation 0")
            print((data['Modulation0'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: Modulation 1")
            print((data['Modulation1'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: TBS 0")
            print((data['bitTBS0'].describe()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: TBS 1")
            print((data['bitTBS1'].describe()))
            print("---------------------------------------------------------------")
            print("PDSCH Packet: Carrier Index")
            print((data['carrierIndex'].value_counts()))
            print("###############################################################")

        if len(self.log_pdschStats['time']) > 0:
            data = pd.DataFrame(dict([ (k, pd.Series(v)) for k, v in list(self.log_pdschStats.items()) ]))
            print("PDSCH Stat Indication")
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Interval")
            print((data['interval'].describe()))
            print("PDSCH Stat: ")
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Number of Records")
            print((data['numRecords'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Number of RBs")
            print((data['numRBs'].describe()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Number of Layers")
            print((data['numLayers'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Number of Transport Blocks Present")
            print((data['numTBPresent'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Serving Cell Index")
            print((data['servingCellIndex'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: HSIC Enabled")
            print((data['hsicEnabled'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: HARQ ID")
            print((data['harqId'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: RV")
            print((data['rv'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: NDI")
            print((data['ndi'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: CRC Result")
            print((data['crcResult'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: RNTI Type")
            print((data['rntiType'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Transport Block Index")
            print((data['tbIndex'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Discarded ReTx Present")
            print((data['discardedReTxPresent'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Did Recombining")
            print((data['didRecombining'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Transport Block Size")
            print((data['tbSize'].describe()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: MCS")
            print((data['mcs'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Number of RBs in this Transport Block")
            print((data['tbNumRBs'].describe()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: ACK/NACK Decision")
            print((data['ackNackDecision'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: PMCH ID")
            print((data['pmchId'].value_counts()))
            print("---------------------------------------------------------------")
            print("PDSCH Stat: Area ID")
            print((data['areaId'].value_counts()))
            print("###############################################################")

        if len(self.log_cmifm['time']) > 0:
            data = pd.DataFrame(dict([ (k, pd.Series(v)) for k, v in list(self.log_cmifm.items()) ]))
            print("LTE ML1 Connected Mode LTE Intra-Freq Meas Results")
            print("---------------------------------------------------------------")
            print((data['interval'].describe()))
            print("Connected Mode LTE Intra-Freq Meas Results: Interval")
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: E-ARFCN")
            print((data['earfcn'].value_counts()))
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: Serving Physical Cell ID")
            print((data['servingPhysicalCellId'].value_counts()))
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: Serving RSRP")
            print((data['servingRsrp'].describe()))
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: Serving RSRQ")
            print((data['servingRsrq'].describe()))
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: Number of Neighbor Cells")
            print((data['numNeighborCells'].value_counts()))
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: Number of Detected Cells")
            print((data['numDetectedCells'].value_counts()))
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: Neighbor Physical Cell ID")
            print((data['neighborPhysicalCellId'].value_counts()))
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: Neighbor Cell RSRP")
            print((data['neighborRsrp'].describe()))
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: Neighbor Cell RSRQ")
            print((data['neighborRsrq'].describe()))
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: Detected Physical Cell ID")
            print((data['detectedPhysicalCellId'].value_counts()))
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: Detected SSS Corr Value")
            print((data['detectedSSSCorrValue'].describe()))
            print("---------------------------------------------------------------")
            print("Connected Mode LTE Intra-Freq Meas Results: Detected Reference Time")
            print((data['detectedReferenceTime'].describe()))
            print("###############################################################")

        if len(self.log_rlm['time']) > 0:
            data = pd.DataFrame(dict([ (k, pd.Series(v)) for k, v in list(self.log_rlm.items()) ]))
            print("LTE ML1 RLM Report")
            print("---------------------------------------------------------------")
            print("RLM Report: Interval")
            print((data['interval'].describe()))
            print("---------------------------------------------------------------")
            print("RLM Report: Number of Records")
            print((data['numRecords'].describe()))
            print("---------------------------------------------------------------")
            print("RLM Report: Out of Sync BLER")
            print((data['outofSyncBler'].describe()))
            print("---------------------------------------------------------------")
            print("RLM Report: In Sync BLER")
            print((data['inSyncBler'].describe()))
            print("---------------------------------------------------------------")
            print("RLM Report: Out of Sync Count")
            print((data['outofSyncCount'].describe()))
            print("---------------------------------------------------------------")
            print("RLM Report: In Sync Count")
            print((data['inSyncCount'].describe()))
            print("---------------------------------------------------------------")
            print("RLM Report: T310 Timer Status (in second)")
            print((data['t310TimerStatus'].value_counts()))
            print("###############################################################")

        if len(self.log_puschcsf['time']) > 0:
            data = pd.DataFrame(dict([ (k, pd.Series(v)) for k, v in list(self.log_puschcsf.items()) ]))
            print("LTE LL1 PUSCH CSF")
            print("---------------------------------------------------------------")
            print("PUSCH CSF: Interval")
            print((data['interval'].describe()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: PUSCH Reporting Mode")
            print((data['puschReportingMode'].value_counts()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: Rank Index")
            print((data['rankIndex'].value_counts()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: CSI Meas Set Index")
            print((data['csiMeasSetIndex'].value_counts()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: Number of Sub-Bands")
            print((data['numSubBands'].value_counts()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: Wide Band CQI CW0")
            print((data['widebandCQICW0'].value_counts()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: Wide Band CQI CW1")
            print((data['widebandCQICW1'].value_counts()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: Sub-Band Size")
            print((data['subBandSize'].value_counts()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: Single WB PMI")
            print((data['singleWBPMI'].value_counts()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: Single MB PMI")
            print((data['singleMBPMI'].value_counts()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: CSF Tx Mode")
            print((data['csfTxMode'].value_counts()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: Carrier Index")
            print((data['carrierIndex'].value_counts()))
            print("---------------------------------------------------------------")
            print("PUSCH CSF: Num CSI-RS Ports")
            print((data['numCSIrsPorts'].value_counts()))
            print("###############################################################")




    def set_mi2log(self, path):
        self.mi2log = path

    def set_source(self,source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)

        # source.enable_log("LTE_RRC_OTA_Packet")
        # source.enable_log("LTE_RRC_Serv_Cell_Info")
        # source.enable_log("LTE_RRC_MIB_Packet")
        # source.enable_log("LTE_RRC_MIB_Message_Log_Packet")
        # source.enable_log("LTE_NAS_ESM_State")
        # source.enable_log("LTE_NAS_ESM_OTA_Incoming_Packet")
        # source.enable_log("LTE_NAS_ESM_OTA_Outgoing_Packet")
        # source.enable_log("LTE_NAS_EMM_State")
        # source.enable_log("LTE_NAS_EMM_OTA_Incoming_Packet")
        # source.enable_log("LTE_NAS_EMM_OTA_Outgoing_Packet")

        # source.enable_log("LTE_PDCP_DL_Config")
        # source.enable_log("LTE_PDCP_UL_Config")
        # source.enable_log("LTE_PDCP_UL_Data_PDU")
        # source.enable_log("LTE_PDCP_DL_Ctrl_PDU")
        # source.enable_log("LTE_PDCP_UL_Ctrl_PDU")
        # source.enable_log("LTE_PDCP_DL_Stats")
        # source.enable_log("LTE_PDCP_UL_Stats")
        # source.enable_log("LTE_PDCP_DL_SRB_Integrity_Data_PDU")
        # source.enable_log("LTE_PDCP_UL_SRB_Integrity_Data_PDU")

        # source.enable_log("LTE_RLC_UL_Config_Log_Packet")
        # source.enable_log("LTE_RLC_DL_Config_Log_Packet")
        # source.enable_log("LTE_RLC_UL_AM_All_PDU")
        # source.enable_log("LTE_RLC_DL_AM_All_PDU")
        # source.enable_log("LTE_RLC_UL_Stats")
        # source.enable_log("LTE_RLC_DL_Stats")

        # source.enable_log("LTE_MAC_Configuration")
        # source.enable_log("LTE_MAC_UL_Transport_Block")
        # source.enable_log("LTE_MAC_DL_Transport_Block")
        # source.enable_log("LTE_MAC_UL_Buffer_Status_Internal")
        # source.enable_log("LTE_MAC_UL_Tx_Statistics")
        # source.enable_log("LTE_MAC_Rach_Trigger")
        # source.enable_log("LTE_MAC_Rach_Attempt")

        source.enable_log("LTE_PHY_PDSCH_Packet")
        # source.enable_log("LTE_PHY_Serv_Cell_Measurement")
        source.enable_log("LTE_PHY_Connected_Mode_Intra_Freq_Meas")
        # source.enable_log("LTE_PHY_Inter_RAT_Measurement")
        # source.enable_log("LTE_PHY_Inter_RAT_CDMA_Measurement")

        # source.enable_log("LTE_PUCCH_Power_Control")
        # source.enable_log("LTE_PUSCH_Power_Control")
        # source.enable_log("LTE_PDCCH_PHICH_Indication_Report")
        source.enable_log("LTE_PHY_PDSCH_Stat_Indication")
        # source.enable_log("LTE_PHY_System_Scan_Results")
        # source.enable_log("LTE_PHY_BPLMN_Cell_Request")
        # source.enable_log("LTE_PHY_BPLMN_Cell_Confirm")
        # source.enable_log("LTE_PHY_Serving_Cell_COM_Loop")
        # source.enable_log("LTE_PHY_PDCCH_Decoding_Result")
        # source.enable_log("LTE_PHY_PDSCH_Decoding_Result")
        # source.enable_log("LTE_PHY_PUSCH_Tx_Report")
        source.enable_log("LTE_PHY_RLM_Report")
        source.enable_log("LTE_PHY_PUSCH_CSF")


        # source.enable_log("1xEV_Rx_Partial_MultiRLP_Packet")
        # source.enable_log("1xEV_Connected_State_Search_Info")
        # # source.enable_log("1xEV_Signaling_Control_Channel_Broadcast")
        # source.enable_log("1xEV_Connection_Attempt")
        # source.enable_log("1xEV_Connection_Release")

        # source.enable_log("WCDMA_RRC_OTA_Packet")
        # source.enable_log("WCDMA_RRC_Serv_Cell_Info")

        # source.enable_log("UMTS_NAS_OTA_Packet")
        # source.enable_log("UMTS_NAS_GMM_State")
        # source.enable_log("UMTS_NAS_MM_State")
        # source.enable_log("UMTS_NAS_MM_REG_State")

    def __msg_callback(self,msg):
        # s = msg.data.decode_xml().replace("\n", "")
        # print minidom.parseString(s).toprettyxml(" ")
        # if "Unsupported" in s or "unsupported" in s or "(MI)Unknown" in s:
        #     print "Unsupported message or unknown message field appears.\n"
        #     print minidom.parseString(s).toprettyxml(" ")
        #     print str(self.mi2log) + "\n"
        if msg.type_id == "LTE_PHY_PDSCH_Packet":
            log_item = msg.data.decode()
            time = datetime.datetime.strptime(str(log_item['timestamp']).split()[1], '%H:%M:%S.%f')
            if len(self.log_pdsch['time']) > 0:
                interval = (time - self.log_pdsch['time'][-1]).total_seconds()
                self.log_pdsch['interval'].append(interval)
            else:
                pass
            self.log_pdsch['time'].append(time)
            servingCellId = log_item['Serving Cell ID']
            rntiType = log_item['PDSCH RNTI Type']
            numTxAntennas = log_item['Number of Tx Antennas(M)']
            numRxAntennas = log_item['Number of Rx Antennas(N)']
            spatialRank = log_item['Spatial Rank']
            frequencySelectivePMI = log_item['Frequency Selective PMI']
            pmiIndex = log_item['PMI Index']
            transmissionScheme = log_item['Transmission Scheme']
            bitTBS0 = log_item['TBS 0']
            bitTBS1 = log_item['TBS 1']
            RBSlot00 = bin(int(str(log_item['RB Allocation Slot 0[0]']), 0)).count("1")
            RBSlot01 = bin(int(str(log_item['RB Allocation Slot 0[1]']), 0)).count("1")
            RBSlot10 = bin(int(str(log_item['RB Allocation Slot 1[0]']), 0)).count("1")
            RBSlot11 = bin(int(str(log_item['RB Allocation Slot 1[1]']), 0)).count("1")
            RB0 = RBSlot00 + RBSlot01
            RB1 = RBSlot10 + RBSlot11
            Modulation0 = log_item['MCS 0']
            Modulation1 = log_item['MCS 1']
            carrierIndex = log_item['Carrier Index']
            # print msg.type_id + "\t" + time + "\t" + rntiType + "\t" + RB0 + "\t" + RB1 + "\t" + Modulation0 + "\t" + Modulation1 + "\t" + bitTBS0 + "\t" + bitTBS1
            self.log_pdsch['servingCellId'].append(servingCellId)
            self.log_pdsch['rntiType'].append(rntiType)
            self.log_pdsch['numTxAntennas'].append(numTxAntennas)
            self.log_pdsch['numRxAntennas'].append(numRxAntennas)
            self.log_pdsch['spatialRank'].append(spatialRank)
            self.log_pdsch['frequencySelectivePMI'].append(frequencySelectivePMI)
            self.log_pdsch['pmiIndex'].append(pmiIndex)
            self.log_pdsch['transmissionScheme'].append(transmissionScheme)
            self.log_pdsch['RB0'].append(RB0)
            self.log_pdsch['RB1'].append(RB1)
            self.log_pdsch['Modulation0'].append(Modulation0)
            self.log_pdsch['Modulation1'].append(Modulation1)
            self.log_pdsch['bitTBS0'].append(bitTBS0)
            self.log_pdsch['bitTBS1'].append(bitTBS1)
            self.log_pdsch['carrierIndex'].append(carrierIndex)

        elif msg.type_id == "LTE_PDSCH_Stat_Indication":
            log_item = msg.data.decode()
            time = datetime.datetime.strptime(str(log_item['timestamp']).split()[1], '%H:%M:%S.%f')
            if len(self.log_pdschStats['time']) > 0:
                interval = (time - self.log_pdschStats['time'][-1]).total_seconds()
                self.log_pdschStats['interval'].append(interval)
            else:
                pass
            self.log_pdschStats['time'].append(time)
            numRecords = log_item['Num Records']
            self.log_pdschStats['numRecords'].append(numRecords)
            for record in log_item['Records']:
                numRBs = record['Num RBs']
                numLayers = record['Num Layers']
                numTBPresent = record['Num Transport Blocks Present']
                servingCellIndex = record['Serving Cell Index']
                hsicEnabled = record['HSIC Enabled']
                pmchId = record['PMCH ID']
                areaId = record['Area ID']
                self.log_pdschStats['numRBs'].append(numRBs)
                self.log_pdschStats['numLayers'].append(numLayers)
                self.log_pdschStats['numTBPresent'].append(numTBPresent)
                self.log_pdschStats['servingCellIndex'].append(servingCellIndex)
                self.log_pdschStats['hsicEnabled'].append(hsicEnabled)
                self.log_pdschStats['pmchId'].append(pmchId)
                self.log_pdschStats['areaId'].append(areaId)

                for tb in record['Transport Blocks']:
                    harqId = tb['HARQ ID']
                    rv = tb['RV']
                    ndi = tb['NDI']
                    crcResult = tb['CRC Result']
                    rntiType = tb['RNTI Type']
                    tbIndex = tb['TB Index']
                    discardedReTxPresent = tb['Discarded reTx Present']
                    didRecombining = tb['Did Recombining']
                    tbSize = tb['TB Size']
                    mcs = tb['MCS']
                    tbNumRBs = tb['Num RBs']
                    ackNackDecision = tb['ACK/NACK Decision']
                    self.log_pdschStats['harqId'].append(harqId)
                    self.log_pdschStats['rv'].append(rv)
                    self.log_pdschStats['ndi'].append(ndi)
                    self.log_pdschStats['crcResult'].append(crcResult)
                    self.log_pdschStats['rntiType'].append(rntiType)
                    self.log_pdschStats['tbIndex'].append(tbIndex)
                    self.log_pdschStats['discardedReTxPresent'].append(discardedReTxPresent)
                    self.log_pdschStats['didRecombining'].append(didRecombining)
                    self.log_pdschStats['tbSize'].append(tbSize)
                    self.log_pdschStats['mcs'].append(mcs)
                    self.log_pdschStats['tbNumRBs'].append(tbNumRBs)
                    self.log_pdschStats['ackNackDecision'].append(ackNackDecision)

        elif msg.type_id == "LTE_PHY_Connected_Mode_Intra_Freq_Meas":
            log_item = msg.data.decode()
            time = datetime.datetime.strptime(str(log_item['timestamp']).split()[1], '%H:%M:%S.%f')
            if len(self.log_cmifm['time']) > 0:
                interval = (time - self.log_cmifm['time'][-1]).total_seconds()
                self.log_cmifm['interval'].append(interval)
            else:
                pass
            self.log_cmifm['time'].append(time)

            earfcn = log_item['E-ARFCN']
            servingPhysicalCellId = log_item['Serving Physical Cell ID']
            servingRsrp = log_item['RSRP(dBm)']
            servingRsrq = log_item['RSRQ(dB)']
            numNeighborCells = log_item['Number of Neighbor Cells']
            numDetectedCells = log_item['Number of Detected Cells']

            self.log_cmifm['earfcn'].append(earfcn)
            self.log_cmifm['servingPhysicalCellId'].append(servingPhysicalCellId)
            self.log_cmifm['servingRsrp'].append(servingRsrp)
            self.log_cmifm['servingRsrq'].append(servingRsrq)
            self.log_cmifm['numNeighborCells'].append(numNeighborCells)
            self.log_cmifm['numDetectedCells'].append(numDetectedCells)

            for nc in log_item['Neighbor Cells']:
                neighborPhysicalCellId = nc['Physical Cell ID']
                neighborRsrp = nc['RSRP(dBm)']
                neighborRsrq = nc['RSRQ(dB)']
                self.log_cmifm['neighborPhysicalCellId'].append(neighborPhysicalCellId)
                self.log_cmifm['neighborRsrp'].append(neighborRsrp)
                self.log_cmifm['neighborRsrq'].append(neighborRsrq)

            for dc in log_item['Detected Cells']:
                detectedPhysicalCellId = dc['Physical Cell ID']
                detectedSSSCorrValue = dc['SSS Corr Value']
                detectedReferenceTime = dc['Reference Time']
                self.log_cmifm['detectedPhysicalCellId'].append(detectedPhysicalCellId)
                self.log_cmifm['detectedSSSCorrValue'].append(detectedSSSCorrValue)
                self.log_cmifm['detectedReferenceTime'].append(detectedReferenceTime)

        elif msg.type_id == "LTE_PHY_RLM_Report":
            log_item = msg.data.decode()
            time = datetime.datetime.strptime(str(log_item['timestamp']).split()[1], '%H:%M:%S.%f')
            if len(self.log_rlm['time']) > 0:
                interval = (time - self.log_rlm['time'][-1]).total_seconds()
                self.log_rlm['interval'].append(interval)
            else:
                pass
            self.log_rlm['time'].append(time)

            numRecords = log_item['Number of Records']
            self.log_rlm['numRecords'].append(numRecords)

            for record in log_item['Records']:
                outofSyncBler = record['Out of Sync BLER (%)']
                inSyncBler = record['In Sync BLER (%)']
                outofSyncCount = record['Out of Sync Count']
                inSyncCount = record['In Sync Count']
                t310TimerStatus = record['T310 Timer Status']
                self.log_rlm['outofSyncBler'].append(outofSyncBler)
                self.log_rlm['inSyncBler'].append(inSyncBler)
                self.log_rlm['outofSyncCount'].append(outofSyncCount)
                self.log_rlm['inSyncCount'].append(inSyncCount)
                self.log_rlm['t310TimerStatus'].append(t310TimerStatus)

        elif msg.type_id == "LTE_PHY_PUSCH_CSF":
            log_item = msg.data.decode()
            time = datetime.datetime.strptime(str(log_item['timestamp']).split()[1], '%H:%M:%S.%f')
            if len(self.log_puschcsf['time']) > 0:
                interval = (time - self.log_puschcsf['time'][-1]).total_seconds()
                self.log_puschcsf['interval'].append(interval)
            else:
                pass
            self.log_puschcsf['time'].append(time)

            puschReportingMode = log_item['PUSCH Reporting Mode']
            rankIndex = log_item['Rank Index']
            csiMeasSetIndex = log_item['Csi Meas Set Index']
            numSubBands = log_item['Number of Subbands']
            widebandCQICW0 = log_item['WideBand CQI CW0']
            widebandCQICW1 = log_item['WideBand CQI CW1']
            subBandSize = log_item['SubBand Size (k)']
            singleWBPMI = log_item['Single WB PMI']
            singleMBPMI = log_item['Single MB PMI']
            csfTxMode = log_item['CSF Tx Mode']
            carrierIndex = log_item['Carrier Index']
            numCSIrsPorts = log_item['Num CSIrs Ports']
            self.log_puschcsf['puschReportingMode'].append(puschReportingMode)
            self.log_puschcsf['rankIndex'].append(rankIndex)
            self.log_puschcsf['csiMeasSetIndex'].append(csiMeasSetIndex)
            self.log_puschcsf['numSubBands'].append(numSubBands)
            self.log_puschcsf['widebandCQICW0'].append(widebandCQICW0)
            self.log_puschcsf['widebandCQICW1'].append(widebandCQICW1)
            self.log_puschcsf['subBandSize'].append(subBandSize)
            self.log_puschcsf['singleWBPMI'].append(singleWBPMI)
            self.log_puschcsf['singleMBPMI'].append(singleMBPMI)
            self.log_puschcsf['csfTxMode'].append(csfTxMode)
            self.log_puschcsf['carrierIndex'].append(carrierIndex)
            self.log_puschcsf['numCSIrsPorts'].append(numCSIrsPorts)

        else:
            pass
