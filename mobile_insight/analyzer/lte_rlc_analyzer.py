#!/usr/bin/python
# Filename: lte_rlc_analyzer.py
"""
A 4G RLC analyzer to get link layer information

Author: Haotian Deng
"""


from mobile_insight.analyzer.analyzer import *
from xml.dom import minidom

__all__ = ["LteRlcAnalyzer"]


class LteRlcAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

        self.startThrw = None
        self.rbInfo = {}

    def set_source(self, source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self, source)

        # Phy-layer logs
        source.enable_log("LTE_RLC_UL_Config_Log_Packet")
        source.enable_log("LTE_RLC_DL_Config_Log_Packet")
        source.enable_log("LTE_RLC_UL_AM_All_PDU")
        source.enable_log("LTE_RLC_DL_AM_All_PDU")

    def __msg_callback(self, msg):

        if msg.type_id == "LTE_RLC_UL_Config_Log_Packet" or msg.type_id == "LTE_RLC_DL_Config_Log_Packet":
            log_item = msg.data.decode()
            # print log_item
            subPkt = log_item['Subpackets'][0]
            if 'Released RBs' in subPkt:
                for releasedRBItem in subPkt['Released RBs']:
                    rbConfigIdx = releasedRBItem['Released RB Cfg Index']
                    if rbConfigIdx in self.rbInfo:
                        self.rbInfo.pop(rbConfigIdx)
            rb_num = 0
            # print subPkt
            for subpacket in subPkt['Active RBs']:
                # print subpacket
                rb_num += 1
                lc_id = subpacket['LC ID']
                ack_mode = subpacket['RB Mode']
                rb_type = subpacket['RB Type']
                bcast_dict = {}
                bcast_dict['lcid'] = lc_id
                bcast_dict['ack mode'] = ack_mode
                bcast_dict['rb type'] = rb_type
                bcast_dict['timstamp'] = str(log_item['timestamp'])
                if msg.type_id == "LTE_RLC_UL_Config_Log_Packet":
                    self.broadcast_info('RLC_UL_RB_SETTING', bcast_dict)
                    self.log_info('RLC_UL_RB_SETTING: ' + str(bcast_dict))
                else:
                    self.broadcast_info('RLC_DL_RB_SETTING', bcast_dict)
                    self.log_info('RLC_DL_RB_SETTING: ' + str(bcast_dict))
            bcast_dict = {}
            bcast_dict['number'] = str(rb_num)
            bcast_dict['timstamp'] = str(log_item['timestamp'])
            if msg.type_id == "LTE_RLC_UL_Config_Log_Packet":
                self.broadcast_info('RLC_UL_RB_NUMBER', bcast_dict)
                self.log_info('RLC_UL_RB_NUMBER: ' + str(bcast_dict))
            else:
                self.broadcast_info('RLC_DL_RB_NUMBER', bcast_dict)
                self.log_info('RLC_DL_RB_NUMBER: ' + str(bcast_dict))

        if msg.type_id == "LTE_RLC_UL_AM_All_PDU":
            log_item = msg.data.decode()
            # if not self.startThrw:
            #     self.startThrw = log_item['timestamp']
            # timeWindow = (log_item['timestamp'] \
            #         - self.startThrw).total_seconds()
            # if timeWindow > 1:
            #     # Update throughput information
            #     print "-------------------------------------------------------"
            #     print "Downlink received throughput"
            #     for k, v in self.rbInfo.iteritems():
            #         print "RB Cfg Idx: " + str(k) + ", " + str(v['cumulativeDLData'] / timeWindow) + " bytes/s"
            #         self.rbInfo[k]['cumulativeDLData'] = 0.0
            #     print "Uplink sent throughput"
            #     for k, v in self.rbInfo.iteritems():
            #         print "RB Cfg Idx: " + str(k) + ", " + str(v['cumulativeULData'] / timeWindow) + " bytes/s"
            #         self.rbInfo[k]['cumulativeULData'] = 0.0
            # print "-------------------------------------------------------"

            #     self.startThrw = log_item['timestamp']

            subPkt = log_item['Subpackets'][0]
            rbConfigIdx = subPkt['RB Cfg Idx']
            if rbConfigIdx not in self.rbInfo:
                self.rbInfo[rbConfigIdx] = {}
                self.rbInfo[rbConfigIdx]['cumulativeULData'] = 0
                self.rbInfo[rbConfigIdx]['cumulativeDLData'] = 0
                self.rbInfo[rbConfigIdx]['UL'] = {}
                self.rbInfo[rbConfigIdx]['DL'] = {}
                self.rbInfo[rbConfigIdx]['UL']['listSN'] = []
                self.rbInfo[rbConfigIdx]['UL']['listAck'] = []
                self.rbInfo[rbConfigIdx]['DL']['listSN'] = []
                self.rbInfo[rbConfigIdx]['DL']['listAck'] = []

            listPDU = subPkt['RLCUL PDUs']
            maxSys_fn = 0
            maxSub_fn = 0
            minSys_fn = 1024
            minSub_fn = 9

            for pduItem in listPDU:
                if pduItem['PDU TYPE'] == 'RLCUL DATA':
                    self.rbInfo[rbConfigIdx]['cumulativeULData'] += \
                        int(pduItem['pdu_bytes'])
                    SN = int(pduItem['SN'])
                    sys_fn = int(pduItem['sys_fn'])
                    sub_fn = int(pduItem['sub_fn'])
                    if sys_fn > maxSys_fn or (
                            sys_fn == maxSys_fn and sub_fn > maxSub_fn):
                        maxSys_fn = sys_fn
                        maxSub_fn = sub_fn
                    if sys_fn < minSys_fn or (
                            sys_fn == minSys_fn and sub_fn < minSub_fn):
                        minSys_fn = sys_fn
                        minSub_fn = sub_fn
                    alreadyAcked = False
                    for i, ackItem in enumerate(
                            self.rbInfo[rbConfigIdx]['UL']['listAck']):
                        if SN + 1 == ackItem['ack_sn']:
                            if sys_fn == ackItem['sys_fn']:
                                diff_ms = (ackItem['sub_fn'] - sub_fn) * 1
                            else:
                                diff_ms = (
                                    ackItem['sys_fn'] - sys_fn - 1) * 10 + (10 - sub_fn) + (ackItem['sub_fn'])
                            if diff_ms > 0:
                                self.log_info("[Frame cost]\tUL Data PDU Ack (frame): " +
                                              str(diff_ms) +
                                              " ms\tRB Config Index: " +
                                              str(rbConfigIdx) +
                                              "\tAckSN: " +
                                              str(ackItem['ack_sn']) +
                                              "\tTime cost: " +
                                              str((ackItem['time_stamp'] -
                                                   log_item['timestamp']).total_seconds()) +
                                              "s\tData TimeStamp: " +
                                              str(log_item['timestamp']) +
                                              "\tAck TimeStamp: " +
                                              str(ackItem['time_stamp']))
                            alreadyAcked = True
                            self.rbInfo[rbConfigIdx]['UL']['listAck'].pop(i)
                            break
                    if alreadyAcked:
                        self.rbInfo[rbConfigIdx]['UL']['listSN'] = []
                    else:
                        self.rbInfo[rbConfigIdx]['UL']['listSN'].append(
                            {
                                'sn': SN,
                                'sys_fn': pduItem['sys_fn'],
                                'sub_fn': pduItem['sub_fn'],
                                'time_stamp': log_item['timestamp']})
                elif pduItem['PDU TYPE'] == 'RLCUL CTRL':
                    self.rbInfo[rbConfigIdx]['cumulativeULData'] += \
                        int(pduItem['pdu_bytes'])
                    AckSN = pduItem['SN']
                    AckSN = int(AckSN.split(" = ")[1])
                    sys_fn = int(pduItem['sys_fn'])
                    sub_fn = int(pduItem['sub_fn'])
                    if sys_fn > maxSys_fn or (
                            sys_fn == maxSys_fn and sub_fn > maxSub_fn):
                        maxSys_fn = sys_fn
                        maxSub_fn = sub_fn
                    if sys_fn < minSys_fn or (
                            sys_fn == minSys_fn and sub_fn < minSub_fn):
                        minSys_fn = sys_fn
                        minSub_fn = sub_fn
                    alreadyAcked = False
                    indexAcked = -1
                    for i, snItem in enumerate(
                            self.rbInfo[rbConfigIdx]['DL']['listSN']):
                        if AckSN == snItem['sn'] + 1:
                            if sys_fn == snItem['sys_fn']:
                                diff_ms = (sub_fn - snItem['sub_fn']) * 1
                            else:
                                diff_ms = (
                                    sys_fn - snItem['sys_fn'] - 1) * 10 + (10 - snItem['sub_fn']) + (sub_fn)
                            if diff_ms > 0:
                                self.log_info("[Frame cost]\tDL Data PDU Ack (frame): " +
                                              str(diff_ms) +
                                              " ms\tRB Config Index: " +
                                              str(rbConfigIdx) +
                                              "\tAckSN: " +
                                              str(AckSN) +
                                              "\tTime cost: " +
                                              str((log_item['timestamp'] -
                                                   snItem['time_stamp']).total_seconds()) +
                                              "s\tData TimeStamp: " +
                                              str(snItem['time_stamp']) +
                                              "\tAck TimeStamp: " +
                                              str(log_item['timestamp']))

                            alreadyAcked = True
                            indexAcked = i
                            break
                    if alreadyAcked:
                        if indexAcked + \
                                1 < len(self.rbInfo[rbConfigIdx]['DL']['listSN']):
                            self.rbInfo[rbConfigIdx]['DL']['listSN'] = self.rbInfo[rbConfigIdx]['DL']['listSN'][indexAcked + 1:]
                        else:
                            self.rbInfo[rbConfigIdx]['DL']['listSN'] = []
                    else:
                        self.rbInfo[rbConfigIdx]['DL']['listAck'].append(
                            {
                                'ack_sn': AckSN,
                                'sys_fn': pduItem['sys_fn'],
                                'sub_fn': pduItem['sub_fn'],
                                'time_stamp': log_item['timestamp']})

            if minSys_fn == maxSys_fn:
                diff_ms = (maxSub_fn - minSub_fn) * 1
            else:
                diff_ms = (maxSys_fn - minSys_fn - 1) * 10 + \
                    (10 - minSub_fn) + (maxSub_fn)
            if diff_ms < 100 and diff_ms > 0:
                self.log_info("[Intantaneous UL Throughput]\t" +
                              str(self.rbInfo[rbConfigIdx]['cumulativeULData'] /
                                  (diff_ms *
                                   1.0)) +
                              " Bytes/ms\tRB Config Index: " +
                              str(rbConfigIdx) +
                              "\tTime Stamp: " +
                              str(log_item['timestamp']))
            self.rbInfo[rbConfigIdx]['cumulativeULData'] = 0

            # s = msg.data.decode_xml().replace("\n", "")
            # print minidom.parseString(s).toprettyxml(" ")
            # log_item = msg.data.decode()
            # print log_item

        if msg.type_id == "LTE_RLC_DL_AM_All_PDU":
            log_item = msg.data.decode()
            # if not self.startThrw:
            #     self.startThrw = log_item['timestamp']
            # timeWindow = (log_item['timestamp'] \
            #         - self.startThrw).total_seconds()
            # if timeWindow > 1:
            #     # Update throughput information
            #     print "-------------------------------------------------------"
            #     print "Downlink received throughput"
            #     for k, v in self.rbInfo.iteritems():
            #         print "RB Cfg Idx: " + str(k) + ", " + str(v['cumulativeDLData'] / timeWindow) + " bytes/s"
            #         self.rbInfo[k]['cumulativeDLData'] = 0.0
            #     print "Uplink sent throughput"
            #     for k, v in self.rbInfo.iteritems():
            #         print "RB Cfg Idx: " + str(k) + ", " + str(v['cumulativeULData'] / timeWindow) + " bytes/s"
            #         self.rbInfo[k]['cumulativeULData'] = 0.0
            # print "-------------------------------------------------------"

            #     self.startThrw = log_item['timestamp']

            subPkt = log_item['Subpackets'][0]
            rbConfigIdx = subPkt['RB Cfg Idx']
            if rbConfigIdx not in self.rbInfo:
                self.rbInfo[rbConfigIdx] = {}
                self.rbInfo[rbConfigIdx]['cumulativeULData'] = 0
                self.rbInfo[rbConfigIdx]['cumulativeDLData'] = 0
                self.rbInfo[rbConfigIdx]['UL'] = {}
                self.rbInfo[rbConfigIdx]['DL'] = {}
                self.rbInfo[rbConfigIdx]['UL']['listSN'] = []
                self.rbInfo[rbConfigIdx]['UL']['listAck'] = []
                self.rbInfo[rbConfigIdx]['DL']['listSN'] = []
                self.rbInfo[rbConfigIdx]['DL']['listAck'] = []

            listPDU = subPkt['RLCDL PDUs']
            maxSys_fn = 0
            maxSub_fn = 0
            minSys_fn = 1024
            minSub_fn = 9

            for pduItem in listPDU:
                if pduItem['PDU TYPE'] == 'RLCDL DATA':
                    self.rbInfo[rbConfigIdx]['cumulativeDLData'] += \
                        int(pduItem['pdu_bytes'])
                    SN = int(pduItem['SN'])
                    sys_fn = int(pduItem['sys_fn'])
                    sub_fn = int(pduItem['sub_fn'])
                    if sys_fn > maxSys_fn or (
                            sys_fn == maxSys_fn and sub_fn > maxSub_fn):
                        maxSys_fn = sys_fn
                        maxSub_fn = sub_fn
                    if sys_fn < minSys_fn or (
                            sys_fn == minSys_fn and sub_fn < minSub_fn):
                        minSys_fn = sys_fn
                        minSub_fn = sub_fn
                    alreadyAcked = False
                    for i, ackItem in enumerate(
                            self.rbInfo[rbConfigIdx]['DL']['listAck']):
                        if SN + 1 == ackItem['ack_sn']:
                            if sys_fn == ackItem['sys_fn']:
                                diff_ms = (ackItem['sub_fn'] - sub_fn) * 1
                            else:
                                diff_ms = (
                                    ackItem['sys_fn'] - sys_fn - 1) * 10 + (10 - sub_fn) + (ackItem['sub_fn'])
                            if diff_ms > 0:
                                self.log_info("[Frame cost]\tDL Data PDU Ack (frame): " +
                                              str(diff_ms) +
                                              " ms\tRB Config Index: " +
                                              str(rbConfigIdx) +
                                              "\tAckSN: " +
                                              str(ackItem['ack_sn']) +
                                              "\tTime cost: " +
                                              str((ackItem['time_stamp'] -
                                                   log_item['timestamp']).total_seconds()) +
                                              "s\tData TimeStamp: " +
                                              str(log_item['timestamp']) +
                                              "\tAck TimeStamp: " +
                                              str(ackItem['time_stamp']))
                            alreadyAcked = True
                            self.rbInfo[rbConfigIdx]['DL']['listAck'].pop(i)
                            break
                    if alreadyAcked:
                        self.rbInfo[rbConfigIdx]['DL']['listSN'] = []
                    else:
                        self.rbInfo[rbConfigIdx]['DL']['listSN'].append(
                            {
                                'sn': SN,
                                'sys_fn': pduItem['sys_fn'],
                                'sub_fn': pduItem['sub_fn'],
                                'time_stamp': log_item['timestamp']})

                elif pduItem['PDU TYPE'] == 'RLCDL CTRL':
                    self.rbInfo[rbConfigIdx]['cumulativeDLData'] += int(
                        pduItem['pdu_bytes'])
                    AckSN = pduItem['SN']
                    AckSN = int(AckSN.split(" = ")[1])
                    sys_fn = int(pduItem['sys_fn'])
                    sub_fn = int(pduItem['sub_fn'])
                    if sys_fn > maxSys_fn or (
                            sys_fn == maxSys_fn and sub_fn > maxSub_fn):
                        maxSys_fn = sys_fn
                        maxSub_fn = sub_fn
                    if sys_fn < minSys_fn or (
                            sys_fn == minSys_fn and sub_fn < minSub_fn):
                        minSys_fn = sys_fn
                        minSub_fn = sub_fn
                    alreadyAcked = False
                    indexAcked = -1
                    for i, snItem in enumerate(
                            self.rbInfo[rbConfigIdx]['UL']['listSN']):
                        if AckSN == snItem['sn'] + 1:
                            if sys_fn == snItem['sys_fn']:
                                diff_ms = (sub_fn - snItem['sub_fn']) * 1
                            else:
                                diff_ms = (
                                    sys_fn - snItem['sys_fn'] - 1) * 10 + (10 - snItem['sub_fn']) + (sub_fn)
                            if diff_ms > 0:
                                self.log_info("[Frame cost]\tUL Data PDU Ack (frame): " +
                                              str(diff_ms) +
                                              " ms\tRB Config Index: " +
                                              str(rbConfigIdx) +
                                              "\tAckSN: " +
                                              str(AckSN) +
                                              "\tTime cost: " +
                                              str((log_item['timestamp'] -
                                                   snItem['time_stamp']).total_seconds()) +
                                              "s\tData TimeStamp: " +
                                              str(snItem['time_stamp']) +
                                              "\tAck TimeStamp: " +
                                              str(log_item['timestamp']))
                            alreadyAcked = True
                            indexAcked = i
                            break
                    if alreadyAcked:
                        if indexAcked + \
                                1 < len(self.rbInfo[rbConfigIdx]['UL']['listSN']):
                            self.rbInfo[rbConfigIdx]['UL']['listSN'] = self.rbInfo[rbConfigIdx]['UL']['listSN'][indexAcked + 1:]
                        else:
                            self.rbInfo[rbConfigIdx]['UL']['listSN'] = []
                    else:
                        self.rbInfo[rbConfigIdx]['UL']['listAck'].append(
                            {
                                'ack_sn': AckSN,
                                'sys_fn': pduItem['sys_fn'],
                                'sub_fn': pduItem['sub_fn'],
                                'time_stamp': log_item['timestamp']})
            if minSys_fn == maxSys_fn:
                diff_ms = (maxSub_fn - minSub_fn) * 1
            else:
                diff_ms = (maxSys_fn - minSys_fn - 1) * 10 + \
                    (10 - minSub_fn) + (maxSub_fn)
            if diff_ms < 100 and diff_ms > 0:
                self.log_info("[Intantaneous DL Throughput]\t" +
                              str(self.rbInfo[rbConfigIdx]['cumulativeDLData'] /
                                  (diff_ms *
                                   1.0)) +
                              " Bytes/ms\tRB Config Index: " +
                              str(rbConfigIdx) +
                              "\tTime Stamp: " +
                              str(log_item['timestamp']))
            self.rbInfo[rbConfigIdx]['cumulativeDLData'] = 0

            # s = msg.data.decode_xml().replace("\n", "")
            # print minidom.parseString(s).toprettyxml(" ")
            # log_item = msg.data.decode()
            # print log_item
