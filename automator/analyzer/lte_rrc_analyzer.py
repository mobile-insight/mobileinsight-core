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
from analyzer import *
from msg_dump import *

class LteRrcAnalyzer(Analyzer):

	def __init__(self):
		Analyzer.__init__(self)

		#Declare analyzers included
		self.dumper=MsgDump()
		self.include_analyzer(self.dumper,[])

		#init packet filters
		self.add_source_callback(self.rrc_filter)

		#init internal states
		self.status=LteRrcStatus()	# current cell status
		self.history=LteRrcHistory()	# cell history: timestamp -> cell status
		self.config=LteRrcConfig()	# cell configurations

	def rrc_filter(self,msg):
		"""
			Filter all RRC packets, and call functions to process it
		"""
		if msg.type_id != "LTE_RRC_OTA_Packet":
			return

		log_item = msg.data
		log_item_dict = dict(log_item)
		if not log_item_dict.has_key('Msg'):
			return
		
		log_xml = ET.fromstring(log_item_dict['Msg'])
		xml_msg=Event(msg.timestamp,msg.type_id,log_xml)
		self.callback_sib_config(xml_msg)

	#Callbacks for LTE RRC analysis
	#FIXME: a single callback for all rrc messages, or per-msg callback? 
	def callback_sib_config(self,msg):

		for field in msg.data.iter('field'):
		#LTE RRC status update
		#Whenever the status changes, push it into the history
		#FIXME: redundant update
			if field.get('name')=="lte-rrc.cellIdentity": #cell identity
				if self.status.id != field.get('show'):
					self.status.id = field.get('show')
					self.history.add_history(msg.timestamp,self.status)

			elif field.get('name')=="lte-rrc.trackingAreaCode": #tracking area code
				if self.status.tac != field.get('show'):
					self.status.tac = field.get('show')
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

				#Default value setting
				#FIXME: set default to those in TS36.331
				field_val['lte-rrc.cellReselectionPriority']=None
				field_val['lte-rrc.threshServingLow']=None
				field_val['lte-rrc.s_NonIntraSearch']=None

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')
	
				if not self.config.sib.has_key(self.status.id):
					#FIXME: can cells in different freq/TAC have the same ID?
					self.config.sib[self.status.id]=LteRrcSibCell()
				self.config.sib[self.status.id].serv_config=LteRrcSibServ(
					field_val['lte-rrc.cellReselectionPriority'],
					field_val['lte-rrc.threshServingLow'],
					field_val['lte-rrc.s_NonIntraSearch'])
			
			#intra-freq cell info
			elif field.get('name')=="lte-rrc.intraFreqCellReselectionInfo_element":
				field_val={}

				#Default value setting
				#FIXME: set to the default value based on TS36.331
				field_val['lte-rrc.t_ReselectionEUTRA']=None
				field_val['lte-rrc.q_RxLevMin']=None
				field_val['lte-rrc.p_Max']=None
				field_val['lte-rrc.s_IntraSearch']=None

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.config.sib.has_key(self.status.id):
					#FIXME: can cells in different freq/TAC have the same ID?
					self.config.sib[self.status.id]=LteRrcSibCell()	
				self.config.sib[self.status.id].intra_freq_config=LteRrcSibIntraFreqConfig(
					field_val['lte-rrc.t_ReselectionEUTRA'],
					field_val['lte-rrc.q_RxLevMin'],
					field_val['lte-rrc.p_Max'],
					field_val['lte-rrc.s_IntraSearch'])
				self.config.sib[self.status.id].intra_freq_config.dump()

			#inter-freq cell info
			elif field.get('name')=="lte-rrc.interFreqCarrierFreqList":
				field_val={}

				#Default value setting
				#FIXME: set to the default value based on TS36.331
				field_val['lte-rrc.dl_CarrierFreq']=None
				field_val['lte-rrc.t_ReselectionEUTRA']=None
				field_val['lte-rrc.q_RxLevMin']=None
				field_val['lte-rrc.p_Max']=None
				field_val['lte-rrc.cellReselectionPriority']=None
				field_val['lte-rrc.threshX_High']=None
				field_val['lte-rrc.threshX_Low']=None

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.config.sib.has_key(self.status.id):
					#FIXME: can cells in different freq/TAC have the same ID?
					self.config.sib[self.status.id]=LteRrcSibCell()
				self.config.sib[self.status.id].inter_freq_config[field_val['lte-rrc.dl_CarrierFreq']]\
					=LteRrcSibInterFreqConfig(
						field_val['lte-rrc.dl_CarrierFreq'],
						field_val['lte-rrc.t_ReselectionEUTRA'],
						field_val['lte-rrc.q_RxLevMin'],
						field_val['lte-rrc.p_Max'],
						field_val['lte-rrc.cellReselectionPriority'],
						field_val['lte-rrc.threshX_High'],
						field_val['lte-rrc.threshX_Low'])
				self.config.sib[self.status.id].inter_freq_config[field_val['lte-rrc.dl_CarrierFreq']].dump()

			#inter-RAT cell info (3G)
			elif field.get('name')=="lte-rrc.t_ReselectionUTRA":
				for config in self.config.sib[self.status.id].inter_freq_config.itervalues():
					#FIXME: how about 2G?
					if config.tReselection==None:
						config.tReselection = field.get('show')
						config.dump()
			elif field.get('name')=="lte-rrc.CarrierFreqUTRA_FDD_element":
				field_val={}

				#Default value setting
				#FIXME: set to default based on TS25.331
				field_val['lte-rrc.carrierFreq']=None
				field_val['lte-rrc.q_RxLevMin']=None
				field_val['lte-rrc.p_MaxUTRA']=None
				field_val['lte-rrc.cellReselectionPriority']=None
				field_val['lte-rrc.threshX_High']=None
				field_val['lte-rrc.threshX_High']=None

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')


				if not self.config.sib.has_key(self.status.id):
					#FIXME: can cells in different freq/TAC have the same ID?
					self.config.sib[self.status.id]=LteRrcSibCell()
				self.config.sib[self.status.id].inter_freq_config[field_val['lte-rrc.carrierFreq']]\
					=LteRrcSibInterFreqConfig(
						field_val['lte-rrc.carrierFreq'],
						None,	#For 3G, tReselection is not in this IE
						field_val['lte-rrc.q_RxLevMin'],
						field_val['lte-rrc.p_MaxUTRA'],
						field_val['lte-rrc.cellReselectionPriority'],
						field_val['lte-rrc.threshX_High'],
						field_val['lte-rrc.threshX_Low'])


			

class LteRrcStatus:
	def __init__(self):
		self.id = None #cell ID
		self.tac = None #tracking area code
		self.freq = None #cell frequency
		self.conn = False #connectivity status

class LteRrcHistory:
	def __init__(self):
		self.status_history=[] # a list of (timestamp,LTE_RRC_STATUS) pair

	def add_history(self,timestamp,status):
		self.status_history.append((timestamp,status))

	def dump(self):
		for item in self.status_history:
			print item[0],item[1].id,item[1].tac,item[1].freq,item[1].conn

class LteRrcConfig:
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
		self.active=LteRrcActive() #active-state configurations

class LteRrcSibCell:
	"""
		Per-cell SIB configuration
	"""
	def __init__(self):
		self.serv_config = None #LTE_RRC_SIB_SERV()
		self.intra_freq_config = None #LTE_RRC_SIB_INTRA_FREQ_CONFIG()
		self.inter_freq_config = {} #freq -> LTE_RRC_SIB_INTER_FREQ_CONFIG

class LteRrcSibServ:
	"""
		Serving cell's cell-reselection configurations
	"""
	def __init__(self,priority,thresh_serv, s_nonintrasearch):
		self.priority = priority #cell reselection priority
		self.threshserv_low = thresh_serv #cell reselection threshold
		self.s_nonintrasearch = s_nonintrasearch #threshold for searching other frequencies

	def dump(self):
		print self.priority,self.threshx_low,self.s_nonintrasearch

class LteRrcSibIntraFreqConfig:
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

class LteRrcSibInterFreqConfig:
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

class LteRrcActive:
	def __init__(self):
		#TODO: initialize some containers
		pass
