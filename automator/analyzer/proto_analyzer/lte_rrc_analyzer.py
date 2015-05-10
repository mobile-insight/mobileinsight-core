#! /usr/bin/env python
"""
lte_rrc_analyzer.py

A LTE_RRC analyzer
	- Maintain configurations from the SIB
	- Connection setup/release statistics
	- RRCReconfiguration statistics
	- ...

Author: Yuanjie Li
"""

import xml.etree.ElementTree as ET
from ..msg_filter import *

class LTE_RRC_Analyzer(MsgFilter):

	def __init__(self):
		self.filter_f = lambda msg : msg.type_id=="LTE_RRC_OTA_Packet"
		self.callback_list = [self.callback_sib_config]
		self.status=LTE_RRC_Status()	# current cell status
		self.config=LTE_RRC_Config()	# cell configurations
		self.history=LTE_RRC_History()	# cell history: timestamp -> cell status

	#Callbacks for LTE RRC analysis
	#FIXME: a single callback for all rrc messages, or per-msg callback? 
	def callback_sib_config(self,msg):

		for field in msg.data.iter('field'):
		#LTE RRC status update
		#Whenever the status changes, push it into the history
		#FIXME: redundant update
			if field.get('name')=="lte-rrc.cellIdentity": #cell identity
				if self.status.ID != field.get('show'):
					self.status.ID = field.get('show')
					self.history.add_history(msg.timestamp,self.status)

			elif field.get('name')=="lte-rrc.trackingAreaCode": #tracking area code
				if self.status.TAC != field.get('show'):
					self.status.TAC = field.get('show')
					self.history.add_history(msg.timestamp,self.status)

			#FIXME: is it serving cell freq?
			# elif field.get('name')=="lte-rrc.carrierFreq": 
			# 	if self.status.freq != field.get('show'):
			# 		self.status.freq = field.get('show')
			# 		self.history.add_history(msg.timestamp,self.status)

		#TODO: RRC connection status update

		#LTE RRC SIB config update
			#serving cell info
			elif field.get('name')=="lte-rrc.cellReselectionServingFreqInfo_element": 
				field_val={}
				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')
	
				if not self.config.sib.has_key(self.status.ID):
					#FIXME: can cells in different freq/TAC have the same ID?
					self.config.sib[self.status.ID]=LTE_RRC_SIB_CELL()
				self.config.sib[self.status.ID].serv_config=LTE_RRC_SIB_SERV(
					field_val['lte-rrc.cellReselectionPriority'],
					field_val['lte-rrc.threshServingLow'],
					field_val['lte-rrc.s_NonIntraSearch'])
			
			#intra-freq cell info
			elif field.get('name')=="lte-rrc.intraFreqCellReselectionInfo_element":
				field_val={}
				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.config.sib.has_key(self.status.ID):
					#FIXME: can cells in different freq/TAC have the same ID?
					self.config.sib[self.status.ID]=LTE_RRC_SIB_CELL()
				self.config.sib[self.status.ID].intra_freq_config=LTE_RRC_SIB_INTRA_FREQ_CONFIG(
					field_val['lte-rrc.t_ReselectionEUTRA'],
					field_val['lte-rrc.q_RxLevMin'],
					field_val['lte-rrc.p_Max'],
					field_val['lte-rrc.s_IntraSearch'])

			#inter-freq cell info
			elif field.get('name')=="lte-rrc.interFreqCarrierFreqList":
				field_val={}
				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.config.sib.has_key(self.status.ID):
					#FIXME: can cells in different freq/TAC have the same ID?
					self.config.sib[self.status.ID]=LTE_RRC_SIB_CELL()
				self.config.sib[self.status.ID].inter_freq_config[field_val['lte-rrc.dl_CarrierFreq']]\
					=LTE_RRC_SIB_INTER_FREQ_CONFIG(
						field_val['lte-rrc.dl_CarrierFreq'],
						field_val['lte-rrc.t_ReselectionEUTRA'],
						field_val['lte-rrc.q_RxLevMin'],
						field_val['lte-rrc.p_Max'],
						field_val['lte-rrc.cellReselectionPriority'],
						field_val['lte-rrc.threshX_High'],
						field_val['lte-rrc.threshX_Low'])
				self.config.sib[self.status.ID].inter_freq_config[field_val['lte-rrc.dl_CarrierFreq']].dump()

			#inter-RAT cell info (3G)
			elif field.get('name')=="lte-rrc.t_ReselectionUTRA":
				for config in self.config.sib[self.status.ID].inter_freq_config.itervalues():
					#FIXME: how about 2G?
					if config.tReselection==None:
						config.tReselection = field.get('show')
						config.dump()
			elif field.get('name')=="lte-rrc.CarrierFreqUTRA_FDD_element":
				field_val={}
				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.config.sib.has_key(self.status.ID):
					#FIXME: can cells in different freq/TAC have the same ID?
					self.config.sib[self.status.ID]=LTE_RRC_SIB_CELL()
				self.config.sib[self.status.ID].inter_freq_config[field_val['lte-rrc.carrierFreq']]\
					=LTE_RRC_SIB_INTER_FREQ_CONFIG(
						field_val['lte-rrc.carrierFreq'],
						None,	#For 3G, tReselection is not in this IE
						field_val['lte-rrc.q_RxLevMin'],
						field_val['lte-rrc.p_MaxUTRA'],
						field_val['lte-rrc.cellReselectionPriority'],
						field_val['lte-rrc.threshX_High'],
						field_val['lte-rrc.threshX_Low'])


			

class LTE_RRC_Status:
	def __init__(self):
		self.ID = None #cell ID
		self.TAC = None #tracking area code
		self.freq = None #cell frequency
		self.conn = False #connectivity status

class LTE_RRC_History:
	def __init__(self):
		self.status_history=[] # a list of (timestamp,LTE_RRC_STATUS) pair

	def add_history(self,timestamp,status):
		self.status_history.append((timestamp,status))

	def dump(self):
		for item in self.status_history:
			print item[0],item[1].ID,item[1].TAC,item[1].freq,item[1].conn

class LTE_RRC_Config:
	""" 
		A database for cell configurations

		TODO: following configurations should be supported
			- Idle-state
				- Cell reselection parameters
			- Active-state
				- PHY/MAC/PDCP/RLC configuration
				- Measurement configurations
	"""
	def __init__(self):
		# self.sib=LTE_RRC_SIB()	#Idle-state: SIB configurations
		self.sib={}	#Idle-state: cellID->LTE_RRC_SIB_CELL
		self.active=LTE_RRC_ACTIVE() #active-state configurations

class LTE_RRC_SIB_CELL:
	"""
		Per-cell SIB configuration
	"""
	def __init__(self):
		self.serv_config = None #LTE_RRC_SIB_SERV()
		self.intra_freq_config = None #LTE_RRC_SIB_INTRA_FREQ_CONFIG()
		self.inter_freq_config = {} #freq -> LTE_RRC_SIB_INTER_FREQ_CONFIG

class LTE_RRC_SIB_SERV:
	"""
		Serving cell's cell-reselection configurations
	"""
	def __init__(self,priority,thresh_serv, s_nonintrasearch):
		self.priority = priority #cell reselection priority
		self.threshserv_low = thresh_serv #cell reselection threshold
		self.s_nonintrasearch = s_nonintrasearch #threshold for searching other frequencies

	def dump(self):
		print self.priority,self.threshx_low,self.s_nonintrasearch

class LTE_RRC_SIB_INTRA_FREQ_CONFIG:
	"""
		Intra-frequency cell-reselection configurations
	"""
	#FIXME: individual cell offset
	def __init__(self,tReselection,q_RxLevMin,p_Max,s_IntraSearch):
		self.tReselection = tReselection
		self.q_RxLevMin = q_RxLevMin
		self.p_Max=p_Max
		self.s_IntraSearch = s_IntraSearch

	def dump(self):
		print self.tReselection,self.q_RxLevMin,self.p_Max,self.s_IntraSearch

class LTE_RRC_SIB_INTER_FREQ_CONFIG:
	"""
		Inter-frequency cell-reselection configurations
	"""	
	#FIXME: the current list is incomplete
	#FIXME: individual cell offset
	def __init__(self,freq,tReselection,q_RxLevMin,p_Max,priority,threshx_high,threshx_low):
		self.freq = freq
		self.tReselection = tReselection
		self.q_RxLevMin = q_RxLevMin
		self.p_Max = p_Max
		self.priority = priority
		self.threshx_high = threshx_high
		self.threshx_low = threshx_low

	def dump(self):
		print self.freq, self.tReselection, self.q_RxLevMin, self.p_Max, self.priority, self.threshx_high, self.threshx_low

class LTE_RRC_ACTIVE:
	def __init__(self):
		#TODO: initialize some containers
		self.a=1
