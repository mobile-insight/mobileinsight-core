#!/usr/bin/python
# Filename: lte_mac_loss_cause_analyzer.py
try:
	import xml.etree.cElementTree as ET
except ImportError:
	import xml.etree.ElementTree as ET

from protocol_analyzer import *
from profile import Profile,ProfileHierarchy
from mobile_insight.analyzer.analyzer import *
import datetime

__all__ = ["LteMacLossCauseAnalyzer"]

class LteMacLossCauseAnalyzer(Analyzer):
	def __init__(self):
		Analyzer.__init__(self)
		self.add_source_callback(self.__msg_callback)

		# for pdsch use
		self.harq_status = {} # cell:[]
		self.mac_fail = {} # corrupt_sys_time:[timestamp,{lost_time:cnt}]
		self.mac_success = {} # corrupt_sys_time:[timestamp, first_recover_time]

		# for rlc
		self.rlc_inorder_lst = []
		self.rlc_outorder_lst = []
		self.max_sn = {} # cfg:max_sn

		self.last_nack = []

		# return the basic info of MAC loss
		self.rlc_reorder_list = []


	def set_source(self, source):
		Analyzer.set_source(self, source)
		# enable physical layer information
		source.enable_log("LTE_PHY_PDSCH_Stat_Indication")
		# enable RLC uplink PDU: nack feedback
		source.enable_log("LTE_RLC_UL_AM_All_PDU")
		# enable RLC downlink PDU: 
		source.enable_log("LTE_RLC_DL_AM_All_PDU")
		# enable RRC message:
		# source.enable_log("LTE_RLC_DL_Config_Log_Packet")

	def __msg_callback(self, msg):
		if msg.type_id == "LTE_PHY_PDSCH_Stat_Indication":
			self.__msg_pdsch_callback(msg)

		if msg.type_id == "LTE_RLC_UL_AM_All_PDU":
			self.__msg_rlcul_callback(msg)

		if msg.type_id == "LTE_RLC_DL_AM_All_PDU":
			self.__msg_rlcdl_callback(msg) 

	def __msg_pdsch_callback(self, msg):
		log_item = msg.data.decode()
		timestamp = log_item['timestamp']
		for record in log_item['Records']:
			cell = record['Serving Cell Index']
			sys_time = record['Frame Num'] * 10 + record['Subframe Num']
			if cell not in self.harq_status:
				self.harq_status[cell] = [[None, None] for i in range(8)]

			blocknum = len(record['Transport Blocks'])

			for block in record['Transport Blocks']:
				if block['RNTI Type'] == 'RA-RNTI':
					self.harq_status.clear()
					self.mac_fail.clear()
					self.mac_success.clear()
					del self.rlc_outorder_lst[:]
					del self.rlc_inorder_lst[:]
					self.max_sn.clear()
					continue

				if block['RNTI Type'] != 'C-RNTI':
					continue

				tb_id = block['TB Index']
				discard = block['Discarded reTx Present']

				harq_id = block['HARQ ID']
				
				retx = block['Did Recombining']
				crc = block['CRC Result']

				if blocknum == 1 and self.harq_status[cell][harq_id][0] != None and self.harq_status[cell][harq_id][1] != None:
					# two block fail, but only retx one ==> harq failure
					entry = self.harq_status[cell][harq_id][abs(1-tb_id)]
					if entry[0] not in self.mac_fail:
						self.mac_fail[entry[0]] = [entry[1], {sys_time:1}]
						# self.log_info("harq fail: " + str(self.mac_fail[entry[0]]))
					elif abs((self.mac_fail[entry[0]][0] - entry[1]).total_seconds()) < 0.00001:
						if sys_time not in self.mac_fail[entry[0]][1]:
							self.mac_fail[entry[0]][1][sys_time] = 1
						else:
							self.mac_fail[entry[0]][1][sys_time] += 1
						# self.log_info("harq fail: " + str(self.mac_fail[entry[0]]))
					else:
						self.mac_fail.pop(entry[0])
					self.harq_status[cell][harq_id][abs(1-tb_id)] = None
				
				if self.harq_status[cell][harq_id][tb_id] != None:
					if retx == 'No':
						# retx a new block regardless of corruption ==> harq failure
						entry = self.harq_status[cell][harq_id][tb_id]
						if entry[0] not in self.mac_fail:
							self.mac_fail[entry[0]] = [entry[1], {sys_time:1}]

						elif abs((self.mac_fail[entry[0]][0] - entry[1]).total_seconds()) < 0.00001:
							if sys_time not in self.mac_fail[entry[0]][1]:
								self.mac_fail[entry[0]][1][sys_time] = 1
							else:
								self.mac_fail[entry[0]][1][sys_time] += 1

						else:
							self.mac_fail.pop(entry[0])

				if crc == 'Fail' and discard == 'None' and retx == 'No':
					self.harq_status[cell][harq_id][tb_id] = [sys_time, timestamp, harq_id]

				if crc == 'Pass':
					if retx == 'Yes':
						entry = self.harq_status[cell][harq_id][tb_id]

						if entry != None:
							self.mac_success[entry[0]] = [entry[1], sys_time]

					self.harq_status[cell][harq_id][tb_id] = None


	def __msg_rlcdl_callback(self, msg):
		log_item = msg.data.decode()
		subpkt = log_item['Subpackets'][0]
		cfg_idx = subpkt['RB Cfg Idx']

		# exclude signaling plane
		if cfg_idx > 30:
			return


		for pdu in subpkt['RLCDL PDUs']:
			if pdu['PDU TYPE'] != 'RLCDL DATA':
				continue
			
			systime = pdu['sys_fn'] * 10 + pdu['sub_fn']

			if 'LSF' in pdu and pdu['LSF'] == 0:
				continue

			if cfg_idx not in self.max_sn:
				self.max_sn[cfg_idx] = -1
			
			if self.max_sn[cfg_idx] < 0 or (pdu['SN'] > self.max_sn[cfg_idx] and pdu['SN'] - self.max_sn[cfg_idx] <= 100) or (self.max_sn[cfg_idx] - pdu['SN']) > 1015:
				self.max_sn[cfg_idx] = pdu['SN']
				self.rlc_inorder_lst.append([cfg_idx, pdu['SN'], systime, log_item['timestamp']])
			else:
				self.rlc_outorder_lst.append([cfg_idx, pdu['SN'], systime, log_item['timestamp']])


	# throw one entry: rnd, sys_time, cfg, sn, feedback-time
	# for one-on-one map
	def __msg_rlcul_callback(self, msg):
		log_item = msg.data.decode()
		timestamp = log_item['timestamp']
		subpkt = log_item['Subpackets'][0]
		cfg_idx = subpkt['RB Cfg Idx']

		if cfg_idx > 30:
			return

		for pdu in subpkt['RLCUL PDUs']:

			if pdu['PDU TYPE'] == 'RLCUL CTRL' and 'RLC CTRL NACK' in pdu:
				systime = pdu['sys_fn'] * 10 + pdu['sub_fn']
				lst = []
				for nackItem in pdu['RLC CTRL NACK']:
					lst.append(nackItem['NACK_SN'])

				for sn in lst:
					if sn in self.last_nack:
						continue

					next = None
					before = -1
					after = -1
					rlctime = None

					for item in reversed(self.rlc_inorder_lst):
						if item[0] != cfg_idx:
							continue

						if next == None:
							next = item
							continue

						if (item[1] < sn and sn < next[1]) or (item[1] > next[1] and (sn < next[1] or sn > item[1])):
							diff = (systime - item[2] + 10240) % 10240
							if diff <= 200:
								rlctime = next[-1]
								before = item[2]
								after = next[2]
							break

						next = item

					if rlctime != None:
						i = before
						while i <= after:
							if i not in self.mac_fail:
								i += 1
								continue

							if self.mac_fail[i][1] == {}:
								self.mac_fail.pop(i)
								continue

							self.log_info("Any corrupt: " + str(self.mac_fail[i]))

							# mac harq failure
							if abs((self.mac_fail[i][0] - rlctime).total_seconds()) <= 5:
								for key in self.mac_fail[i][1].keys():
									# if cfg_idx not in self.t_reordering:
									# 	self.t_reordering[cfg_idx] = 60
									# self.rlc_reorder_list.append([sn, after, key, systime, timestamp, self.t_reordering[cfg_idx]])
									self.rlc_reorder_list.append({'timestamp':timestamp, 'sequence number':sn, 'loss detected at':after, 'loss cause':'HARQ failure'})
									self.mac_fail[i][1][key] -= 1

									if self.mac_fail[i][1][key] ==0:
										self.mac_fail[i][1].pop(key)

									rlctime = None
									before = -1
									after = -1
									break

							else:
								self.mac_fail.pop(i)
							i+= 1

						# packet loss by MAC ==> HARQ failure
						if rlctime !=  None:
							i = after
							while i >= before:
								if i in self.mac_success and abs((self.mac_success[i][0]-rlctime).total_seconds()) < 5:
									# if cfg_idx not in self.t_reordering:
									# 	self.t_reordering[cfg_idx] = 60
									# self.rlc_reorder_list.append([sn, after, self.mac_success[i][1], systime, timestamp, self.t_reordering[cfg_idx]])
									self.rlc_reorder_list.append({'timestamp':timestamp, 'sequence number':sn, 'loss detected at':after, 'loss cause':'MAC loss'})
									self.log_info("loss and corruption: {},{}".format(sn, after, timestamp))
									break
								i -= 1

							if i<before:
								# if cfg_idx not in self.t_reordering:
								# 	self.t_reordering[cfg_idx] = 60
								# self.rlc_reorder_list.append([sn, after, after, systime, timestamp, self.t_reordering[cfg_idx]])
								self.rlc_reorder_list.append({'timestamp':timestamp, 'sequence number':sn, 'loss detected at':after, 'loss cause':'MAC loss'})

				self.last_nack = lst