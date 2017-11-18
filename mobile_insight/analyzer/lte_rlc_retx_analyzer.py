#!/usr/bin/python
# Filename: lte_rlc_retx_analyzer.py

from mobile_insight.analyzer.analyzer import *
import datetime

__all__ = ["LteRlcRetxAnalyzer"]

class RadioBearerEntity():
	def __init__(self, num):
		self.idx 			= num
		self.t_reordering 	= 60
		
		self.pkt_recv 		= [] # a list of first-received packet, in ascending order
		self.pkt_disorder 	= []
		self.max_sn 		= -1

		self.nack_dict 		= {} # sn:[nack_time,timestamp]; a list of nack packet; w/o retx
		self.start_time 	= {} # sn:[loss_detected_time,timestamp]

		self.res 			= []


	def recv_rlc_data(self, pdu, timestamp):
		if 'LSF' in pdu and pdu['LSF'] == 0:
			return
		
		sys_time = pdu['sys_fn'] * 10 + pdu['sub_fn']
		sn = pdu['SN']

		if (sn > self.max_sn and sn - self.max_sn < 900) or (sn < self.max_sn and sn < 5 and self.max_sn >500):
			self.max_sn = sn
			self.pkt_recv.append([sn, sys_time, timestamp])

		else:
			if sn in self.start_time:
				if (timestamp - self.start_time[sn][1]).total_seconds() < 1:
					# self.res.append([timestamp, sn, self.start_time[sn][0], self.nack_dict[sn][0], sys_time])
					self.res.append({'timestamp':timestamp, 'sequence number':sn, 'gap detected at':self.start_time[sn][0], 'NACK sent at':self.nack_dict[sn][0], 'packet retransmission at':sys_time, 't-reordering':self.t_reordering})
				self.start_time.pop(sn)

			self.pkt_disorder.append([sn, sys_time, timestamp])


	def recv_rlc_ctrl(self, pdu, timestamp):
		lst = []
		pdu_sys_time = pdu['sys_fn'] * 10 + pdu['sub_fn']
		for nackItem in pdu['RLC CTRL NACK']:
			sn = nackItem['NACK_SN']
			lst.append(sn)
			if sn in self.nack_dict:
				if (timestamp - self.nack_dict[sn][1]).total_seconds > 1:
					self.nack_dict[sn] = [pdu_sys_time, timestamp]
					if sn in self.start_time:
						self.start_time.pop(sn)
					# continue

			else:
				self.nack_dict[sn] = [pdu_sys_time, timestamp]
				if sn in self.start_time:
					self.start_time.pop(sn)

		idx = len(self.pkt_recv) - 1

		for key in self.nack_dict.keys():
			if key not in lst:
				self.nack_dict.pop(key)
				self.start_time.pop(key, None)
				continue

			if key in self.start_time:
				continue

			while idx >= 1:
				before = self.pkt_recv[idx-1]
				after = self.pkt_recv[idx]

				if (before[0] < key and key < after[0]) or (before[0] > after[0] and (key > before[0] or key < after[0])):
					self.start_time[key] = [after[1], after[2]]
					break

				idx -= 1


		# check if retx packets are displayed before RLC NACK
		idx = -1
		for pkt in reversed(self.pkt_disorder):
			if abs((timestamp-pkt[2]).total_seconds()) > 0.5:
				idx = self.pkt_disorder.index(pkt)
				break

			# find start_time
			if pkt[0] in self.start_time:
				# self.res.append([pkt[2], pkt[0], self.start_time[pkt[0]][0], self.nack_dict[pkt[0]][0], pkt[1]])
				self.res.append({'timestamp':pkt[2], 'sequence number':pkt[0], 'gap detected at':self.start_time[pkt[0]][0], 'NACK sent at':self.nack_dict[pkt[0]][0], 'packet retransmission at':pkt[1], 't-reordering':self.t_reordering})
				self.start_time.pop(pkt[0])
				self.nack_dict.pop(pkt[0])

		if idx >= 0:
			del self.pkt_disorder[:idx + 1]



class LteRlcRetxAnalyzer(Analyzer):
	def __init__(self):
		Analyzer.__init__(self)
		self.add_source_callback(self.__msg_callback)

		self.bearer_entity 	= {}

		self.res = []

		self.t_timer = {}

		self.nack_flag 	= False # mark status report of NACK

	def set_source(self, source):
		Analyzer.set_source(self, source)
		source.enable_log("LTE_RLC_UL_AM_All_PDU")
		source.enable_log("LTE_RLC_DL_AM_All_PDU")
		# enable RLC config message:
		source.enable_log("LTE_RLC_DL_Config_Log_Packet")

	def __msg_callback(self, msg):
		if msg.type_id == "LTE_RLC_UL_AM_All_PDU":
			self.__msg_rlc_ul_callback(msg)

		if msg.type_id == "LTE_RLC_DL_AM_All_PDU":
			self.__msg_rlc_dl_callback(msg)

		if msg.type_id == "LTE_RLC_DL_Config_Log_Packet":
			self.__msg_rlccfg_callback(msg)


	def __msg_rlc_cfg_callback(self, msg):
		records = msg.data.decode()['Subpackets'][0]

		for srb in records['Active RBs']:
			if srb['RB Cfg Idx'] >= 30:
				continue
			cfg = srb['RB Cfg Idx']

			if cfg_idx not in self.bearer_entity:
				self.bearer_entity[cfg_idx] = RadioBearerEntity(cfg_idx)

			timer = srb['T Reordering (ms)']
			if timer not in self.t_timer:
				self.t_timer[timer] = 0
			self.t_timer[timer] += 1
			self.bearer_entity[cfg_idx].t_reordering = timer 


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