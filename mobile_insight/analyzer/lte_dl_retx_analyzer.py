#!/usr/bin/python
# Filename: lte_dl_retx_analyzer.py

"""
Function: Monitor downlink MAC retransmission delay and RLC retransmission delay
Author: Qianru Li
"""

from mobile_insight.analyzer.analyzer import *
import datetime
import sys

__all__ = ["LteDlRetxAnalyzer"]

def comp_seq_num(s1, s2):
	if s1 == s2:
		return 0
	if (s2 - s1 + 1024) % 1024 <= 150:
		return -1
	return 1

class RadioBearerEntity():
	def __init__(self, num):
		self.__idx 			= num

		self.__pkt_recv 		= [] # a list of first-received packet, in ascending order
		self.__pkt_disorder 	= []
		self.__max_sn 		= -1
		self.__nack_dict 		= {} # sn:[nack_time,timestamp]; a list of nack packet; w/o retx
		self.__loss_detected_time 	= {} # sn:[loss_detected_time,timestamp]

		self.mac_retx = []
		self.rlc_retx = []


	def recv_rlc_data(self, pdu, timestamp):
		if 'LSF' in pdu and pdu['LSF'] == 0:
			return
		
		sys_time = pdu['sys_fn'] * 10 + pdu['sub_fn']
		sn = pdu['SN']

		# Received packet with higher sequence number
		if 'LSF' not in pdu and (self.__max_sn == -1 or comp_seq_num(self.__max_sn, sn) == -1):
			self.__max_sn = sn
			self.__pkt_recv.append([sn, sys_time, timestamp])

		else:
			# rlc retx packet
			if sn in self.__loss_detected_time:
				if (timestamp - self.__loss_detected_time[sn][1]).total_seconds() < 1:
					self.rlc_retx.append({'timestamp': timestamp, 'sn':sn, 'rlc_retx':(sys_time - self.__loss_detected_time[sn][0] + 10240) % 10240})
				self.__loss_detected_time.pop(sn)

			# mac retx packet
			else:
				for i in range(len(self.__pkt_recv) - 1, 1, -1):
					after = self.__pkt_recv[i]
					before = self.__pkt_recv[i - 1]
					if (timestamp - after[2]).total_seconds() > 0.2:
						break
					if comp_seq_num(before[0], sn) == -1 and comp_seq_num(sn, after[0]) == -1:
						delay = (sys_time - after[1] + 10240) % 10240
						if delay > 0 and delay < 200:
							self.mac_retx.append({'timestamp': timestamp, 'sn':sn, 'mac_retx':delay})
						break

			self.__pkt_disorder.append([sn, sys_time, timestamp])


	def recv_rlc_ctrl(self, pdu, timestamp):
		lst = []
		pdu_sys_time = pdu['sys_fn'] * 10 + pdu['sub_fn']
		for nackItem in pdu['RLC CTRL NACK']:
			sn = nackItem['NACK_SN']
			lst.append(sn)
			if sn in self.__nack_dict:
				if (timestamp - self.__nack_dict[sn][1]).total_seconds() > 1:
					self.__nack_dict[sn] = [pdu_sys_time, timestamp]
					if sn in self.__loss_detected_time:
						self.__loss_detected_time.pop(sn)

			else:
				self.__nack_dict[sn] = [pdu_sys_time, timestamp]
				if sn in self.__loss_detected_time:
					self.__loss_detected_time.pop(sn)

		idx = len(self.__pkt_recv) - 1

		original_keys = list(self.__nack_dict)
		for key in original_keys:
			if key not in lst:
				self.__nack_dict.pop(key)
				self.__loss_detected_time.pop(key, None)
				continue

			if key in self.__loss_detected_time:
				continue

			while idx >= 1:
				before = self.__pkt_recv[idx-1]
				after = self.__pkt_recv[idx]

				if (before[0] < key and key < after[0]) or (before[0] > after[0] and (key > before[0] or key < after[0])):
					self.__loss_detected_time[key] = [after[1], after[2]]
					break

				idx -= 1

		# check if retx packets are displayed before RLC NACK
		idx = -1
		for pkt in reversed(self.__pkt_disorder):
			if abs((timestamp-pkt[2]).total_seconds()) > 0.5:
				idx = self.__pkt_disorder.index(pkt)
				break

			if pkt[0] in self.__loss_detected_time:
				self.rlc_retx.append({'timestamp': pkt[2], 'sn':sn, 'rlc_retx':(pkt[1] - self.__loss_detected_time[pkt[0]][0] + 10240) % 10240})
				self.__loss_detected_time.pop(pkt[0])
				self.__nack_dict.pop(pkt[0])

		if idx >= 0:
			del self.__pkt_disorder[:idx + 1]


class LteDlRetxAnalyzer(Analyzer):
	def __init__(self):
		Analyzer.__init__(self)
		self.add_source_callback(self.__msg_callback)

		self.bearer_entity 	= {}

	def set_source(self, source):
		Analyzer.set_source(self, source)
		source.enable_log("LTE_RLC_UL_AM_All_PDU")
		source.enable_log("LTE_RLC_DL_AM_All_PDU")

	def __msg_callback(self, msg):
		if msg.type_id == "LTE_RLC_UL_AM_All_PDU":
			self.__msg_rlc_ul_callback(msg)

		if msg.type_id == "LTE_RLC_DL_AM_All_PDU":
			self.__msg_rlc_dl_callback(msg)

	def __msg_rlc_ul_callback(self, msg):
		log_item = msg.data.decode()
		subpkt = log_item['Subpackets'][0]
		cfg_idx = subpkt['RB Cfg Idx']
		timestamp = log_item['timestamp']
		if cfg_idx >= 30:
			return

		if cfg_idx not in self.bearer_entity:
			self.bearer_entity[cfg_idx] = RadioBearerEntity(cfg_idx)

		for pdu in subpkt['RLCUL PDUs']:
			if pdu['PDU TYPE'] == 'RLCUL CTRL' and 'RLC CTRL NACK' in pdu:
				self.bearer_entity[cfg_idx].recv_rlc_ctrl(pdu, timestamp)

	def __msg_rlc_dl_callback(self, msg):
		log_item = msg.data.decode()
		subpkt = log_item['Subpackets'][0]
		cfg_idx = subpkt['RB Cfg Idx']
		if cfg_idx >= 30:
			return

		timestamp = log_item['timestamp']

		if cfg_idx not in self.bearer_entity:
			self.bearer_entity[cfg_idx] = RadioBearerEntity(cfg_idx)

		records = subpkt['RLCDL PDUs']
		for pdu in records:
			if pdu['PDU TYPE'] == 'RLCDL DATA':
				self.bearer_entity[cfg_idx].recv_rlc_data(pdu, timestamp)
