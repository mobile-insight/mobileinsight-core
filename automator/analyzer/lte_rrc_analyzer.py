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
import time


class LteRrcAnalyzer(Analyzer):

	def __init__(self):

		Analyzer.__init__(self)

		#init packet filters
		self.add_source_callback(self.rrc_filter)

		#init internal states
		self.status=LteRrcStatus()	# current cell status
		self.history={}	# cell history: timestamp -> LteRrcHistory()
		self.config={}	# cell_id -> LteRrcConfig()

	def set_source(self,source):
		Analyzer.set_source(self,source)
		#enable LTE RRC log
		source.enable_log("LTE_RRC_OTA_Packet")
		# source.enable_log("LTE_RRC_MIB_Message_Log_Packet")

	def rrc_filter(self,msg):
		"""
			Filter all RRC packets, and call functions to process it
			
			Args:
				msg: the event (message) from the trace collector

		"""

		#Convert to xml 
		log_item = msg.data
		log_item_dict = dict(log_item)

		if not log_item_dict.has_key('Msg'):
			return

		raw_msg=Event(msg.timestamp,msg.type_id,log_item_dict)
		self.callback_serv_cell(raw_msg)
		
		log_xml = ET.fromstring(log_item_dict['Msg'])
		xml_msg=Event(msg.timestamp,msg.type_id,log_xml)

		# Calllbacks triggering

		if msg.type_id == "LTE_RRC_OTA_Packet":	
			self.callback_sib_config(xml_msg)
			#TODO: callback RRC
		elif msg.type_id == "LTE_RRC_MIB_Message_Log_Packet":
			self.callback_mib_config(xml_msg)

		else: #nothing to update
			return

		# Raise event to other analyzers
		# FIXME: the timestamp is incoherent with that from the trace collector
		e = Event(time.time(),'LteRrcAnalyzer',"")
		self.send(e)


	def callback_serv_cell(self,msg):
		"""
			Update serving cell status
		"""
		if msg.data.has_key('Freq'):
			self.status.freq = msg.data['Freq']
		if msg.data.has_key('Physical Cell ID'):
			self.status.id = msg.data['Physical Cell ID']
		self.status.dump()

		

	def callback_sib_config(self,msg):
		"""
			Callbacks for LTE RRC analysis
		"""

		for field in msg.data.iter('field'):

			#TODO: use MIB, not lte-rrc.trackingAreaCode
			if field.get('name')=="lte-rrc.trackingAreaCode": #tracking area code
				self.status.tac = field.get('show')
				self.status.dump()

			#serving cell info
			if field.get('name')=="lte-rrc.cellReselectionServingFreqInfo_element": 
				field_val={}

				#Default value setting
				#FIXME: set default to those in TS36.331
				field_val['lte-rrc.cellReselectionPriority']=None
				field_val['lte-rrc.threshServingLow']=None
				field_val['lte-rrc.s_NonIntraSearch']=None

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.config.has_key(self.status.id):
					self.config[self.status.id]=LteRrcConfig()
					self.config[self.status.id].status=self.status

				self.config[self.status.id].serv_config=LteRrcSibServ(
					field_val['lte-rrc.cellReselectionPriority'],
					field_val['lte-rrc.threshServingLow'],
					field_val['lte-rrc.s_NonIntraSearch'])
			
			#intra-freq cell info
			if field.get('name')=="lte-rrc.intraFreqCellReselectionInfo_element":
				
				field_val={}

				#FIXME: set to the default value based on TS36.331
				field_val['lte-rrc.t_ReselectionEUTRA']=None
				field_val['lte-rrc.q_RxLevMin']=None
				field_val['lte-rrc.p_Max']=None
				field_val['lte-rrc.s_IntraSearch']=None

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.config.has_key(self.status.id):
					self.config[self.status.id]=LteRrcConfig()
					self.config[self.status.id].status=self.status

				self.config[self.status.id].sib.intra_freq_config\
				=LteRrcSibIntraFreqConfig(
					field_val['lte-rrc.t_ReselectionEUTRA'],
					field_val['lte-rrc.q_RxLevMin'],
					field_val['lte-rrc.p_Max'],
					field_val['lte-rrc.s_IntraSearch'])
				self.config[self.status.id].sib.intra_freq_config.dump()

			#inter-freq cell info
			if field.get('name')=="lte-rrc.interFreqCarrierFreqList":
				field_val={}

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

				if not self.config.has_key(self.status.id):
					self.config[self.status.id]=LteRrcConfig()
					self.config[self.status.id].status=self.status

				neighbor_freq=field_val['lte-rrc.dl_CarrierFreq']
				self.config[self.status.id].sib.inter_freq_config[neighbor_freq]\
				=LteRrcSibInterFreqConfig(
						field_val['lte-rrc.dl_CarrierFreq'],
						field_val['lte-rrc.t_ReselectionEUTRA'],
						field_val['lte-rrc.q_RxLevMin'],
						field_val['lte-rrc.p_Max'],
						field_val['lte-rrc.cellReselectionPriority'],
						field_val['lte-rrc.threshX_High'],
						field_val['lte-rrc.threshX_Low'])
				self.config[self.status.id].sib.inter_freq_config[neighbor_freq].dump()

			#inter-RAT cell info (3G)
			if field.get('name')=="lte-rrc.t_ReselectionUTRA":
				for config in self.config[self.status.id].sib.inter_freq_config.itervalues():
					#FIXME: how about 2G?
					if config.tReselection==None:
						config.tReselection = field.get('show')
						config.dump()
			
			if field.get('name')=="lte-rrc.CarrierFreqUTRA_FDD_element":
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


				if not self.config.has_key(self.status.id):
					self.config[self.status.id]=LteRrcConfig()
					self.config[self.status.id].status=self.status

				neighbor_freq=field_val['lte-rrc.carrierFreq'] 
				self.config[self.status.id].sib.inter_freq_config[neighbor_freq]\
				=LteRrcSibInterFreqConfig(
						field_val['lte-rrc.carrierFreq'],
						None,	#For 3G, tReselection is not in this IE
						field_val['lte-rrc.q_RxLevMin'],
						field_val['lte-rrc.p_MaxUTRA'],
						field_val['lte-rrc.cellReselectionPriority'],
						field_val['lte-rrc.threshX_High'],
						field_val['lte-rrc.threshX_Low'])

			#TODO: RRC connection status update


	def callback_mib_config(self,msg):
		#LTE RRC status update
		pass
		#Whenever the status changes, push it into the history
			# if field.get('name')=="lte-rrc.cellIdentity": #cell identity
			# 	if self.status.id != field.get('show'):
			# 		self.status.id = field.get('show')
			# 		self.history.add_history(msg.timestamp,self.status)

			# elif field.get('name')=="lte-rrc.trackingAreaCode": #tracking area code
			# 	if self.status.tac != field.get('show'):
			# 		self.status.tac = field.get('show')
			# 		self.history.add_history(msg.timestamp,self.status)

			# elif field.get('name')=="lte-rrc.carrierFreq": 
			# 	if self.status.freq != field.get('show'):
			# 		self.status.freq = field.get('show')
			# 		self.history.add_history(msg.timestamp,self.status)

	def get_cell_list(self):
		"""
			Get a complete list of cell IDs *at current location*

			FIXME: currently only return *all* cells in the LteRrcConfig
		"""
		return self.config.keys()

	def get_cell_config(self,cell_id):
		"""
			Return a cell's configuration
		"""
		return self.config[cell_id]


class LteRrcStatus:
	"""
		The metadata of a cell
	"""
	def __init__(self):
		self.id = None #cell ID
		self.tac = None #tracking area code
		self.freq = None #cell frequency
		self.bandwidth = None #cell bandwidth
		self.conn = False #connectivity status (for serving cell only)

	def dump(self):
		print self.id,self.tac,self.freq,self.bandwidth,self.conn

class LteRrcConfig:
	""" 
		Per-cell RRC configurations

		The following configurations should be supported
			- Idle-state
				- Cell reselection parameters
			- Active-state
				- PHY/MAC/PDCP/RLC configuration
				- Measurement configurations
	"""
	def __init__(self):
		self.status = LteRrcStatus() #the metadata of this cell
		self.sib=LteRrcSib()	#Idle-state: cellID->LTE_RRC_SIB_CELL
		self.active=LteRrcActive() #active-state configurations

class LteRrcSib:

	"""
		Per-cell Idle-state configurations
	"""
	def __init__(self):
		#FIXME: init based on the default value in TS36.331
		#configuration as a serving cell (LteRrcSibServ)
		self.serv_config = LteRrcSibServ(None,None,None) 
		#Intra-freq reselection config
		self.intra_freq_config = LteRrcSibIntraFreqConfig(None,None,None,None) 
		#Inter-freq/RAT reselection config. Freq/cell -> LteRrcSibInterFreqConfig
		self.inter_freq_config = {}  


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
	def __init__(self,tReselection,q_RxLevMin,p_Max,s_IntraSearch):
		#FIXME: individual cell offset
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
		print self.freq, self.tReselection, self.q_RxLevMin, self.p_Max, \
		self.priority, self.threshx_high, self.threshx_low

class LteRrcActive:
	def __init__(self):
		#TODO: initialize some containers
		pass

