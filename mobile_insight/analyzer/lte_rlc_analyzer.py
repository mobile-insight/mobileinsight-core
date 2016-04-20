#!/usr/bin/python
# Filename: lte_rlc_analyzer.py
"""
A 4G RLC analyzer to get link layer information

Author: Haotian Deng
"""


from mobile_insight.analyzer.analyzer import *
from xml.dom import minidom

__all__=["LteRlcAnalyzer"]

class LteRlcAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

        self.startThrw = None
        self.rbInfo = {}


    def set_source(self,source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)

        #Phy-layer logs
        # source.enable_log("LTE_RLC_UL_Config_Log_Packet")
        # source.enable_log("LTE_RLC_DL_Config_Log_Packet")
        source.enable_log("LTE_RLC_UL_AM_All_PDU")
        source.enable_log("LTE_RLC_DL_AM_All_PDU")

    def __msg_callback(self,msg):

        if msg.type_id == "LTE_RLC_UL_AM_All_PDU":
            log_item = msg.data.decode()
            if not self.startThrw:
                self.startThrw = log_item['timestamp']
            timeWindow = (log_item['timestamp'] \
                    - self.startThrw).total_seconds()
            if timeWindow > 5:
                # Update throughput information
                self.log_info("Downlink received throughput")
                for k, v in self.rbInfo.iteritems():
                    self.log_info("RB Cfg Idx: " + str(k) + ", " + str(v['cumulativeDLData'] / timeWindow) + " bytes/s")
                    self.rbInfo[k]['cumulativeDLData'] = 0.0
                self.log_info("-------------------------------------------------------")
                self.log_info("Uplink sent throughput")
                for k, v in self.rbInfo.iteritems():
                    self.log_info("RB Cfg Idx: " + str(k) + ", " + str(v['cumulativeULData'] / timeWindow) + " bytes/s")
                    self.rbInfo[k]['cumulativeULData'] = 0.0
                self.log_info("-------------------------------------------------------")

                self.startThrw = log_item['timestamp']

            subPkt = log_item['Subpackets'][0]
            rbConfigIdx = subPkt['RB Cfg Idx']
            if rbConfigIdx not in self.rbInfo:
                self.rbInfo[rbConfigIdx] = {}
                self.rbInfo[rbConfigIdx]['cumulativeULData'] = 0
                self.rbInfo[rbConfigIdx]['cumulativeDLData'] = 0
                self.rbInfo[rbConfigIdx]['waitingForSN'] = -1
                self.rbInfo[rbConfigIdx]['waitingForSNSince'] = None

            listPDU = subPkt['RLCUL PDUs']
            for pduItem in listPDU:
                if pduItem['PDU TYPE'] == 'RLCUL DATA':
                    self.rbInfo[rbConfigIdx]['cumulativeULData'] += \
                            int(pduItem['pdu_bytes'])
                    SN = int(pduItem['SN'])
                    if SN + 1 > self.rbInfo[rbConfigIdx]['waitingForSN']:
                        self.rbInfo[rbConfigIdx]['waitingForSN'] = SN + 1
                        self.rbInfo[rbConfigIdx]['waitingForSNSince'] = \
                                log_item['timestamp']
                    elif SN + 1 == self.rbInfo[rbConfigIdx]['waitingForSN']:
                        self.log_debug("Retransmission happened")
                        self.rbInfo[rbConfigIdx]['waitingForSNSince'] = \
                                log_item['timestamp']


            # s = msg.data.decode_xml().replace("\n", "")
            # print minidom.parseString(s).toprettyxml(" ")
            # log_item = msg.data.decode()
            # print log_item

        if msg.type_id == "LTE_RLC_DL_AM_All_PDU":
            log_item = msg.data.decode()
            if not self.startThrw:
                self.startThrw = log_item['timestamp']
            timeWindow = (log_item['timestamp'] \
                    - self.startThrw).total_seconds()
            if timeWindow > 5:
                # Update throughput information
                self.log_info("Downlink received throughput")
                for k, v in self.rbInfo.iteritems():
                    self.log_info("RB Cfg Idx: " + str(k) + ", " + str(v['cumulativeDLData'] / timeWindow) + " bytes/s")
                    self.rbInfo[k]['cumulativeDLData'] = 0.0
                self.log_info("-------------------------------------------------------")
                self.log_info("Uplink sent throughput")
                for k, v in self.rbInfo.iteritems():
                    self.log_info("RB Cfg Idx: " + str(k) + ", " + str(v['cumulativeULData'] / timeWindow) + " bytes/s")
                    self.rbInfo[k]['cumulativeULData'] = 0.0
                self.log_info("-------------------------------------------------------")

                self.startThrw = log_item['timestamp']

            subPkt = log_item['Subpackets'][0]
            rbConfigIdx = subPkt['RB Cfg Idx']
            if rbConfigIdx not in self.rbInfo:
                self.rbInfo[rbConfigIdx] = {}
                self.rbInfo[rbConfigIdx]['cumulativeULData'] = 0
                self.rbInfo[rbConfigIdx]['cumulativeDLData'] = 0
                self.rbInfo[rbConfigIdx]['waitingForSN'] = -1
                self.rbInfo[rbConfigIdx]['waitingForSNSince'] = None

            listPDU = subPkt['RLCDL PDUs']
            for pduItem in listPDU:
                if pduItem['PDU TYPE'] == 'RLCDL DATA':
                    self.rbInfo[rbConfigIdx]['cumulativeDLData'] += \
                            pduItem['pdu_bytes']
                elif pduItem['PDU TYPE'] == 'RLCDL CTRL':
                    AckSN = pduItem['SN']
                    AckSN = int(AckSN.split(" = ")[1])
                    if AckSN == self.rbInfo[rbConfigIdx]['waitingForSN']:
                        self.log_info("RB Config Idx: " + str(rbConfigIdx) + ", RTT: " +  str((log_item['timestamp'] - self.rbInfo[rbConfigIdx]['waitingForSNSince']).total_seconds()) + " seconds")
                        self.rbInfo[rbConfigIdx]['waitingForSN'] = -1
                        self.rbInfo[rbConfigIdx]['waitingForSNSince'] = None
                    else:
                        self.log_debug("[Debug] RB Config Idx: " + str(rbConfigIdx) + " Expected Ack SN: " + str(self.rbInfo[rbConfigIdx]['waitingForSN']) + " Received Ack SN: " + str(AckSN) + ", timestamp: " + str(log_item['timestamp']))
                        if AckSN > self.rbInfo[rbConfigIdx]['waitingForSN']:
                            self.rbInfo[rbConfigIdx]['waitingForSN'] = -1
                            self.rbInfo[rbConfigIdx]['waitingForSNSince'] = None
            # s = msg.data.decode_xml().replace("\n", "")
            # print minidom.parseString(s).toprettyxml(" ")
            # log_item = msg.data.decode()
            # print log_item


