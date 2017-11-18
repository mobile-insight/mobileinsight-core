#!/usr/bin/python
# Filename: lte_mac_corruption_analyzer.py

from mobile_insight.analyzer.analyzer import *
import datetime

__all__ = ["LteMacCorruptAnalyzer"]

class LteMacCorruptAnalyzer(Analyzer):
	def __init__(self):
		Analyzer.__init__(self)
		self.add_source_callback(self.__msg_callback)

		self.total_blocks 		= {'PCell':0, 'SCell':0}
		self.corrupt_blocks 	= {'PCell':0, 'SCell':0}
		self.total_new_blocks 	= {'PCell':0, 'SCell':0}
		self.loss_blocks 		= {'PCell':0, 'SCell':0}

		self.mac_retx_delay 	= {'PCell':[], 'SCell':[]}

		self.last_status = {}

	def set_source(self, source):
		Analyzer.set_source(self, source)
		source.enable_log("LTE_PHY_PDSCH_Stat_Indication")

	def __msg_callback(self, msg):
		if msg.type_id == "LTE_PHY_PDSCH_Stat_Indication":
			self.__msg_pdsch_callback(msg)

	def __msg_pdsch_callback(self, msg):
		log_item = msg.data.decode()
		for record in log_item['Records']:
			cell = 'PCell'
			if record['Serving Cell Index'] != 'PCell':
				cell = 'SCell'

			if record['Serving Cell Index'] not in self.last_status:
				self.last_status[record['Serving Cell Index']] = [['Pass', 'Pass'] for i in range(8)]	

			status = self.last_status[record['Serving Cell Index']]

			sys_time = record['Subframe Num'] + record['Frame Num'] * 10
			for block in record['Transport Blocks']:
				if block['RNTI Type'] != 'C-RNTI':
					continue

				harq_id = block['HARQ ID']
				tb_id = block['TB Index']
				crc = block['CRC Result'] 		# 'Pass', 'Fail'
				old = block['Did Recombining'] 	# 'Yes', 'No'

				if block['Discarded reTx Present'] == 'Present':
					status[harq_id][tb_id] = None
					continue

				self.total_blocks[cell] += 1

				harq_id = block['HARQ ID']
				tb_id = block['TB Index']
				crc = block['CRC Result'] 		# 'Pass', 'Fail'
				old = block['Did Recombining'] 	# 'Yes', 'No'

				if crc == 'Fail':
					self.corrupt_blocks[cell] += 1

				if old == 'No':
					self.total_new_blocks[cell] += 1

				if old == 'Yes' and crc == 'Pass' and status[harq_id][tb_id] != None:
					self.mac_retx_delay[cell].append[(sys_time - status[harq_id][tb_id] + 10240) % 10240]

				if old == 'No':
					if status[harq_id][tb_id] != None:
						self.loss_blocks[cell] += 1

				if old == 'No' and crc == 'Fail':
					status[harq_id][tb_id] = sys_time

				if crc == 'Pass':
					status[harq_id][tb_id] = None