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

__all__=["LteRrcAnalyzer"]

class LteRrcAnalyzer(Analyzer):

	def __init__(self):

		Analyzer.__init__(self)

		#init packet filters
		self.add_source_callback(self.__rrc_filter)

		#init internal states
		self.__status=LteRrcStatus()	# current cell status
		self.__history={}	# cell history: timestamp -> LteRrcStatus()
		self.__config={}	# cell_id -> LteRrcConfig()

	def set_source(self,source):
		Analyzer.set_source(self,source)
		#enable LTE RRC log
		source.enable_log("LTE_RRC_OTA_Packet")
		source.enable_log("LTE_RRC_Serv_Cell_Info_Log_Packet")

	def __rrc_filter(self,msg):
		
		"""
			Filter all RRC packets, and call functions to process it
			
			Args:
				msg: the event (message) from the trace collector

		"""

		log_item = msg.data
		log_item_dict = dict(log_item)

		#Convert msg to dictionary format
		raw_msg=Event(msg.timestamp,msg.type_id,log_item_dict)
		self.__callback_serv_cell(raw_msg)

		if not log_item_dict.has_key('Msg'):
			return
		
		#Convert msg to xml format
		log_xml = ET.fromstring(log_item_dict['Msg'])
		xml_msg=Event(msg.timestamp,msg.type_id,log_xml)

		# Calllbacks triggering

		if msg.type_id == "LTE_RRC_OTA_Packet":	
			self.__callback_sib_config(xml_msg)
			#TODO: callback RRC

		else: #nothing to update
			return

		# Raise event to other analyzers
		# FIXME: the timestamp is incoherent with that from the trace collector
		e = Event(time.time(),'LteRrcAnalyzer',"")
		self.send(e)


	def __callback_serv_cell(self,msg):
		"""
			Update serving cell status
		"""
		if msg.data.has_key('Freq'):
			self.__status.freq = msg.data['Freq']
		if msg.data.has_key('Physical Cell ID'):
			self.__status.id = msg.data['Physical Cell ID']
		# self.status.dump()

		

	def __callback_sib_config(self,msg):
		"""
			Callbacks for LTE RRC analysis
		"""

		for field in msg.data.iter('field'):

			#TODO: use MIB, not lte-rrc.trackingAreaCode
			if field.get('name')=="lte-rrc.trackingAreaCode": #tracking area code
				self.__status.tac = field.get('show')
				self.__status.dump()

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

				if not self.__config.has_key(self.__status.id):
					self.__config[self.__status.id]=LteRrcConfig()
					self.__config[self.__status.id].status=self.__status

				self.__config[self.__status.id].serv_config=LteRrcSibServ(
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

				if not self.__config.has_key(self.__status.id):
					self.__config[self.__status.id]=LteRrcConfig()
					self.__config[self.__status.id].status=self.__status

				self.__config[self.__status.id].sib.intra_freq_config\
				=LteRrcSibIntraFreqConfig(
					field_val['lte-rrc.t_ReselectionEUTRA'],
					field_val['lte-rrc.q_RxLevMin'],
					field_val['lte-rrc.p_Max'],
					field_val['lte-rrc.s_IntraSearch'])
				self.__config[self.__status.id].sib.intra_freq_config.dump()

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

				if not self.__config.has_key(self.__status.id):
					self.__config[self.__status.id]=LteRrcConfig()
					self.__config[self.__status.id].status=self.__status

				neighbor_freq=field_val['lte-rrc.dl_CarrierFreq']
				self.__config[self.__status.id].sib.inter_freq_config[neighbor_freq]\
				=LteRrcSibInterFreqConfig(
						field_val['lte-rrc.dl_CarrierFreq'],
						field_val['lte-rrc.t_ReselectionEUTRA'],
						field_val['lte-rrc.q_RxLevMin'],
						field_val['lte-rrc.p_Max'],
						field_val['lte-rrc.cellReselectionPriority'],
						field_val['lte-rrc.threshX_High'],
						field_val['lte-rrc.threshX_Low'])
				self.__config[self.__status.id].sib.inter_freq_config[neighbor_freq].dump()

			#inter-RAT cell info (3G)
			if field.get('name')=="lte-rrc.t_ReselectionUTRA":
				for config in self.__config[self.__status.id].sib.inter_freq_config.itervalues():
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


				if not self.__config.has_key(self.__status.id):
					self.__config[self.__status.id]=LteRrcConfig()
					self.__config[self.__status.id].status=self.__status

				neighbor_freq=field_val['lte-rrc.carrierFreq'] 
				self.__config[self.__status.id].sib.inter_freq_config[neighbor_freq]\
				=LteRrcSibInterFreqConfig(
						field_val['lte-rrc.carrierFreq'],
						None,	#For 3G, tReselection is not in this IE
						field_val['lte-rrc.q_RxLevMin'],
						field_val['lte-rrc.p_MaxUTRA'],
						field_val['lte-rrc.cellReselectionPriority'],
						field_val['lte-rrc.threshX_High'],
						field_val['lte-rrc.threshX_Low'])

			#TODO: RRC connection status update

	def get_cell_list(self):
		"""
			Get a complete list of cell IDs *at current location*

			FIXME: currently only return *all* cells in the LteRrcConfig
		"""
		return self.__config.keys()

	def get_cell_config(self,cell_id):
		"""
			Return a cell's configuration
		"""
		if self.__config.has_key(cell_id):
			return self.__config[cell_id]
		else:
			return None

	def get_cur_cell(self):
		"""
			Return a cell's configuration
			TODO: if no current cell (inactive), return None
		"""
		return self.__status

	def get_cur_cell_config(self):
		if self.__config.has_key(self.__status.id):
			return self.__config[self.__status.id]
		else:
			return None


class LteRrcStatus:
	"""
		The metadata of a cell
	"""
	def __init__(self):
		self.id = None #cell ID
		self.freq = None #cell frequency
		self.tac = None #tracking area code
		self.bandwidth = None #cell bandwidth
		self.conn = False #connectivity status (for serving cell only)

	def dump(self):
		print "cellID=",self.id," Freq=",self.freq

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

	def dump(self):
		self.status.dump()
		self.sib.dump()
		self.active.dump()

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
		print self.priority,self.threshserv_low,self.s_nonintrasearch

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

	def dump(self):
		pass

