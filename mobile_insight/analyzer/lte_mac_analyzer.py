#!/usr/bin/python
# Filename: lte_mac_analyzer.py
"""
A 4G MAC-layer analyzer with the following functions

  - UL grant utilization analysis

  - TBA

Author: Yuanjie Li
"""


from mobile_insight.analyzer.analyzer import *
import datetime

__all__ = ["LteMacAnalyzer"]


class LteMacAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)
        self.last_bytes = {} # LACI -> bytes <int> Last remaining bytes in MAC UL buffer
        self.buffer = {} # LCID -> [(sys_fn, sun_fn), packet_bytes] buffered mac ul packets
        self.ctrl_pkt_sfn = {} # LCID -> [sys_fn, sun_fn] when last mac ul control packet comes
        self.cur_fn = None # Record current [sys_fn, sub_fn] for mac ul buffer
        self.cell_id = {} # cell_name -> idx Keep index for each type of cell
        self.idx = 0 # current recorded cell idx
        self.failed_harq = [0] * 8 * 3 * 2
        self.queue_length = 0
        # store every failed_harq by ['timestamp', 'cell_idx', 'harq_id', 'tb_idx', 'tb_size', 'retx_succeed', 'retx_cnt', 'trigger_rlc_retx', 'sn_sfn', 'delay']
        # self.mac_retx = []  # for each retx, get [timestamp, fn_sfn, time, delay]

    def set_source(self, source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self, source)

        # Phy-layer logs
        source.enable_log("LTE_MAC_UL_Tx_Statistics")
        source.enable_log("LTE_MAC_UL_Buffer_Status_Internal")
        source.enable_log("LTE_PHY_PDSCH_Stat_Indication")

    def __msg_callback(self, msg):

        if msg.type_id == "LTE_MAC_UL_Tx_Statistics":
            log_item = msg.data.decode()

            grant_received = 0
            grant_utilized = 0
            grant_utilization = 0

            for i in range(0, len(log_item['Subpackets'])):
                grant_received += log_item['Subpackets'][i]['Sample']['Grant received']
                grant_utilized += log_item['Subpackets'][i]['Sample']['Grant utilized']

            if grant_received != 0:
                grant_utilization = round(
                    100.0 * grant_utilized / grant_received, 2)
                bcast_dict = {}
                bcast_dict['timestamp'] = str(log_item['timestamp'])
                bcast_dict['received'] = str(grant_received)
                bcast_dict['used'] = str(grant_utilized)
                bcast_dict['utilization'] = str(grant_utilization)
                self.broadcast_info("MAC_UL_GRANT", bcast_dict)
                self.log_info(str(log_item['timestamp']) +
                              " MAC UL grant: received=" +
                              str(grant_received) +
                              " bytes" +
                              " used=" +
                              str(grant_utilized) +
                              " bytes" +
                              " utilization=" +
                              str(grant_utilization) +
                              "%")

        elif msg.type_id == "LTE_MAC_UL_Buffer_Status_Internal":
            log_item = msg.data.decode()
            if 'Subpackets' in log_item:
                for i in range(0, len(log_item['Subpackets'])):
                    if 'Samples' in log_item['Subpackets'][i]:
                        # print log_item
                        for sample in log_item['Subpackets'][i]['Samples']:
                            sub_fn = int(sample['Sub FN'])
                            sys_fn = int(sample['Sys FN'])
                            # Incorrect sys_fn and sub_fn are normally 1023 and 15
                            if not (sys_fn >= 1023 and sub_fn >= 9): # if the sys_fn and sub_fn are valid, update
                                if self.cur_fn:
                                    # reset historical data if time lag is bigger than 2ms
                                    lag = sys_fn * 10 + sub_fn - self.cur_fn[0] * 10 - self.cur_fn[1]
                                    if lag > 2 or -10238 < lag < 0:

                                        self.last_bytes = {}
                                        self.buffer = {}
                                        self.ctrl_pkt_sfn = {}
                                self.cur_fn = [sys_fn, sub_fn]
                            elif self.cur_fn: # if invalid and inited, add current sfn
                                self.cur_fn[1] += 1
                                if self.cur_fn[1] == 10:
                                    self.cur_fn[1] = 0
                                    self.cur_fn[0] += 1
                                if self.cur_fn[0] == 1024:
                                    self.cur_fn = [0, 0]
                            if not self.cur_fn:
                                break

                            for lcid in sample['LCIDs']:
                                try:
                                    idx = lcid['Ld Id']
                                    new_bytes = int(lcid['New Compressed Bytes'])
                                    ctrl_bytes = int(lcid['Ctrl bytes'])
                                    total_bytes = int(lcid['Total Bytes'])
                                except KeyError:
                                    continue

                                if idx not in self.buffer:
                                    self.buffer[idx] = []
                                if idx not in self.last_bytes:
                                    self.last_bytes[idx] = 0
                                if idx not in self.ctrl_pkt_sfn:
                                    self.ctrl_pkt_sfn[idx] = None

                                # add new packet to buffer
                                if not new_bytes == 0:
                                    # TODO: Need a better way to decided if it is a new packet or left packet
                                    if new_bytes > self.last_bytes[idx]:
                                        new_bytes = new_bytes - self.last_bytes[idx]
                                        self.buffer[idx].append([(self.cur_fn[0], self.cur_fn[1]), new_bytes])

                                if not ctrl_bytes == 0:
                                    total_bytes -= 2
                                    if not self.ctrl_pkt_sfn[idx]:
                                        self.ctrl_pkt_sfn[idx] = (self.cur_fn[0], self.cur_fn[1])
                                else:
                                    if self.ctrl_pkt_sfn[idx]:
                                        ctrl_pkt_delay = self.cur_fn[0] * 10 + self.cur_fn[1] \
                                                         - self.ctrl_pkt_sfn[idx][0] * 10 - self.ctrl_pkt_sfn[idx][1]
                                        ctrl_pkt_delay += 10240 if ctrl_pkt_delay < 0 else 0
                                        self.ctrl_pkt_sfn[idx] = None
                                        self.log_info(str(log_item['timestamp']) + " UL_CTRL_PKT_DELAY: " + str(ctrl_pkt_delay))
                                        bcast_dict = {}
                                        bcast_dict['timestamp'] = str(log_item['timestamp'])
                                        bcast_dict['delay'] = str(ctrl_pkt_delay)
                                        self.broadcast_info("UL_CTRL_PKT_DELAY", bcast_dict)

                                if self.last_bytes[idx] > total_bytes:
                                    sent_bytes = self.last_bytes[idx] - total_bytes
                                    while len(self.buffer[idx]) > 0 and sent_bytes > 0:
                                        pkt = self.buffer[idx][0]
                                        if pkt[1] <= sent_bytes:
                                            pkt_delay = self.cur_fn[0] * 10 + self.cur_fn[1] \
                                                             - pkt[0][0] * 10 - pkt[0][1]
                                            pkt_delay += 10240 if pkt_delay < 0 else 0
                                            self.buffer[idx].pop(0)
                                            sent_bytes -= pkt[1]
                                            self.log_info(str(log_item['timestamp']) + " UL_PKT_DELAY: " + str(pkt_delay))
                                            bcast_dict = {}
                                            bcast_dict['timestamp'] = str(log_item['timestamp'])
                                            bcast_dict['delay'] = str(pkt_delay)
                                            self.broadcast_info("UL_PKT_DELAY", bcast_dict)
                                        else:
                                            pkt[1] -= sent_bytes
                                self.last_bytes[idx] = total_bytes
                            queue_length = 0
                            for idx in self.last_bytes:
                                queue_length += self.last_bytes[idx]
                                if queue_length > 0 and queue_length != self.queue_length:
                                    self.queue_length = queue_length
                                    self.log_info(str(log_item['timestamp']) + " UL_QUEUE_LENGTH: " + str(queue_length))
                                    bcast_dict = {}
                                    bcast_dict['timestamp'] = str(log_item['timestamp'])
                                    bcast_dict['length'] = str(queue_length)
                                    self.broadcast_info("UL_QUEUE_LENGTH", bcast_dict)
                                
        elif msg.type_id == "LTE_PHY_PDSCH_Stat_Indication":
            self.__msg_callback_pdsch_stat(msg)

    def __msg_callback_pdsch_stat(self, msg):
        log_item = msg.data.decode()
        timestamp = str(log_item['timestamp'])
        # two_tb_flag = False # if a record has 'Serving Cell Index' key, two tb share the same cell idx
        if 'Records' in log_item:
            for i in range(0, len(log_item['Records'])):
                record = log_item['Records'][i]
                if 'Transport Blocks' in record:
                    if 'Serving Cell Index' in record:
                        # two_tb_flag = True
                        cell_id_str = record['Serving Cell Index']
                        if cell_id_str not in self.cell_id:
                            self.cell_id[cell_id_str] = self.idx
                            cell_idx = self.idx
                            self.idx += 1
                        else:
                            cell_idx = self.cell_id[cell_id_str]
                        sn = int(record['Frame Num'])
                        sfn = int(record['Subframe Num'])
                        sn_sfn = sn * 10 + sfn
                    for blocks in log_item['Records'][i]['Transport Blocks']:
                        # if not two_tb_flag:
                        harq_id = int(blocks['HARQ ID'])
                        tb_idx = int(blocks['TB Index'])
                        is_retx = True if blocks['Did Recombining'][-2:] == "es" else False
                        crc_check = True if blocks['CRC Result'][-2:] == "ss" else False
                        tb_size = int(blocks['TB Size'])
                        rv_value = int(blocks['RV'])
                        rlc_retx = 0

                        id = harq_id + cell_idx * 8 + tb_idx * 24

                        # print crc_check #self.failed_harq

                        if not crc_check:  # add retx instance or add retx time for existing instance
                            cur_fail = [timestamp, cell_idx, harq_id, tb_idx, tb_size, False, 0, False, sn_sfn]
                            # print cur_fail, rv_value
                            if self.failed_harq[id] != 0:
                                if rv_value > 0:
                                    self.failed_harq[id][6] += 1
                                else:
                                    self.failed_harq[id][-2] = True
                                    # rlc_retx += 1
                                    delay = sn_sfn - self.failed_harq[id][-1]
                                    bcast_dict = {}
                                    bcast_dict['pkt size'] = self.failed_harq[id][4]
                                    # bcast_dict['cell index'] = self.failed_harq[id][1]
                                    bcast_dict['timestamp'] = timestamp
                                    bcast_dict['delay'] = delay
                                    self.broadcast_info('RLC_RETX', bcast_dict)
                                    self.log_info('RLC_RETX: ' + str(bcast_dict))
                                    self.failed_harq[id] = 0
                            elif rv_value == 0:
                                self.failed_harq[id] = cur_fail
                            # print self.failed_harq

                        else:  # check if it trigger rlc_retx or mark as retx_succeed
                            if self.failed_harq[id] != 0:
                                if rv_value > 0 or is_retx:
                                    self.failed_harq[id][6] += 1
                                    self.failed_harq[id][-4] = True
                                    delay = sn_sfn - self.failed_harq[id][-1]
                                    # self.mac_retx.append(self.failed_harq[id] + [delay])
                                    bcast_dict = {}
                                    bcast_dict['pkt size'] = self.failed_harq[id][4]
                                    # bcast_dict['cell index'] = self.failed_harq[id][1]
                                    bcast_dict['timestamp'] = timestamp
                                    bcast_dict['delay'] = delay
                                    self.broadcast_info('MAC_RETX', bcast_dict)
                                    self.log_info('MAC_RETX: ' + str(bcast_dict))
                                else:
                                    self.failed_harq[id][-2] = True
                                    delay = sn_sfn - self.failed_harq[id][-1]
                                    # self.mac_retx.append(self.failed_harq[id] + [delay])
                                    bcast_dict = {}
                                    bcast_dict['pkt size'] = self.failed_harq[id][4]
                                    # bcast_dict['cell index'] = self.failed_harq[id][1]
                                    bcast_dict['timestamp'] = timestamp
                                    bcast_dict['delay'] = delay
                                    self.broadcast_info('RLC_RETX', bcast_dict)
                                    self.log_info('RLC_RETX: ' + str(bcast_dict))
                                self.failed_harq[id] = 0
