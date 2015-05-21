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

#Q-offset range mapping (6.3.4, TS36.331)
q_offset_range={
	0:-24, 1:-22, 2:-20, 3:-18, 4:-16, 5:-14,
	6:-12, 7:-10, 8:-8, 9:-6, 10:-5, 11:-4, 
	12:-3, 13:-2, 14:-1, 15:0, 16:1, 17:2, 
	18:3, 19:4, 20:5, 21:6, 22:8, 23:10, 24:12, 
	25:14, 26:16, 27:18, 28:20, 29:22, 30:24
}

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

		# Raise event to other analyzers
		# FIXME: the timestamp is incoherent with that from the trace collector
		e = Event(time.time(),self.__class__.__name__,"")
		self.send(e)


	def __callback_serv_cell(self,msg):

		"""
			Update current cell status
		"""
		if not self.__status.inited():
			if msg.data.has_key('Freq'):
				self.__status.freq = msg.data['Freq']
			if msg.data.has_key('Physical Cell ID'):
				self.__status.id = msg.data['Physical Cell ID']
		else:
			if msg.data.has_key('Freq') and self.__status.freq != msg.data['Freq']:
				self.__status=LteRrcStatus()
				self.__status.freq=msg.data['Freq']
				self.__history[msg.timestamp]=self.__status
			if msg.data.has_key('Physical Cell ID') \
			and self.__status.id != msg.data['Physical Cell ID']:
				self.__status=LteRrcStatus()
				self.__status.id=msg.data['Physical Cell ID']
				self.__history[msg.timestamp]=self.__status

		

	def __callback_sib_config(self,msg):
		"""
			Callbacks for LTE RRC analysis
		"""

		for field in msg.data.iter('field'):

			#TODO: use MIB, not lte-rrc.trackingAreaCode
			if field.get('name')=="lte-rrc.trackingAreaCode": #tracking area code
				self.__status.tac = field.get('show')
				# self.__status.dump()

			#serving cell and intra-frequency reselection info
			if field.get('name')=="lte-rrc.sib3_element":
				field_val={}

				#Default value setting
				#FIXME: set default to those in TS36.331
				field_val['lte-rrc.cellReselectionPriority']=None #mandatory
				field_val['lte-rrc.threshServingLow']=None #mandatory
				field_val['lte-rrc.s_NonIntraSearch']=float("inf")
				field_val['lte-rrc.q_Hyst']=0
				field_val['lte-rrc.q_RxLevMin']=None #mandatory
				field_val['lte-rrc.p_Max']=23 #default value for UE category 3
				field_val['lte-rrc.s_IntraSearch']=float('inf')
				field_val['lte-rrc.t_ReselectionEUTRA']=None

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.__config.has_key(self.__status.id):
					self.__config[self.__status.id]=LteRrcConfig()
					self.__config[self.__status.id].status=self.__status

				self.__config[self.__status.id].sib.serv_config=LteRrcSibServ(
					float(field_val['lte-rrc.cellReselectionPriority']),
					float(field_val['lte-rrc.threshServingLow'])*2,
					float(field_val['lte-rrc.s_NonIntraSearch'])*2,
					float(field_val['lte-rrc.q_Hyst']))

				self.__config[self.__status.id].sib.intra_freq_config\
				=LteRrcSibIntraFreqConfig(
					float(field_val['lte-rrc.t_ReselectionEUTRA']),
					float(field_val['lte-rrc.q_RxLevMin'])*2,
					float(field_val['lte-rrc.p_Max']),
					float(field_val['lte-rrc.s_IntraSearch'])*2) 


			#inter-frequency (LTE)
			if field.get('name')=="lte-rrc.interFreqCarrierFreqList":
				field_val={}

				#FIXME: set to the default value based on TS36.331
				field_val['lte-rrc.dl_CarrierFreq']=None #mandatory
				field_val['lte-rrc.t_ReselectionEUTRA']=None #mandatory
				field_val['lte-rrc.q_RxLevMin']=None #mandatory
				field_val['lte-rrc.p_Max']=23 #optional, r.f. 36.101
				field_val['lte-rrc.cellReselectionPriority']=None #mandatory
				field_val['lte-rrc.threshX_High']=None #mandatory
				field_val['lte-rrc.threshX_Low']=None #mandatory
				field_val['lte-rrc.q_OffsetFreq']=0

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.__config.has_key(self.__status.id):
					self.__config[self.__status.id]=LteRrcConfig()
					self.__config[self.__status.id].status=self.__status

				neighbor_freq=int(field_val['lte-rrc.dl_CarrierFreq'])
				self.__config[self.__status.id].sib.inter_freq_config[neighbor_freq]\
				=LteRrcSibInterFreqConfig(
						"LTE",
						neighbor_freq,
						float(field_val['lte-rrc.t_ReselectionEUTRA']),
						float(field_val['lte-rrc.q_RxLevMin'])*2,
						float(field_val['lte-rrc.p_Max']),
						float(field_val['lte-rrc.cellReselectionPriority']),
						float(field_val['lte-rrc.threshX_High'])*2,
						float(field_val['lte-rrc.threshX_Low'])*2,
						float(field_val['lte-rrc.q_OffsetFreq']))

			#inter-RAT (UTRA)
			if field.get('name')=="lte-rrc.CarrierFreqUTRA_FDD_element":
				field_val={}

				#Default value setting
				#FIXME: set to default based on TS25.331
				field_val['lte-rrc.carrierFreq']=None #mandatory
				field_val['lte-rrc.q_RxLevMin']=None #mandatory
				field_val['lte-rrc.p_MaxUTRA']=None #mandatory
				field_val['lte-rrc.cellReselectionPriority']=None #mandatory
				field_val['lte-rrc.threshX_High']=None #mandatory
				field_val['lte-rrc.threshX_High']=None #mandatory

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')


				if not self.__config.has_key(self.__status.id):
					self.__config[self.__status.id]=LteRrcConfig()
					self.__config[self.__status.id].status=self.__status

				neighbor_freq=int(field_val['lte-rrc.carrierFreq']) 
				self.__config[self.__status.id].sib.inter_freq_config[neighbor_freq]\
				=LteRrcSibInterFreqConfig(
						"UTRA",
						neighbor_freq,
						None,	#For 3G, tReselection is not in this IE
						float(field_val['lte-rrc.q_RxLevMin'])*2,
						float(field_val['lte-rrc.p_MaxUTRA']),
						float(field_val['lte-rrc.cellReselectionPriority']),
						float(field_val['lte-rrc.threshX_High'])*2,
						float(field_val['lte-rrc.threshX_Low'])*2,
						0)	#inter-RAT has no freq-offset

			if field.get('name')=="lte-rrc.t_ReselectionUTRA":
				for config in self.__config[self.__status.id].sib.inter_freq_config.itervalues():
					if config.rat=="UTRA":
						config.tReselection = float(field.get('show'))

			#TODO: inter-RAT (GERAN): lte-rrc.CarrierFreqsInfoGERAN_element
			if field.get('name')=="lte-rrc.CarrierFreqsInfoGERAN_element":
				field_val={}

				#Default value setting
				#FIXME: set to default based on TS25.331
				field_val['lte-rrc.startingARFCN']=None #mandatory
				field_val['lte-rrc.q_RxLevMin']=None #mandatory
				field_val['lte-rrc.lte-rrc.p_MaxGERAN']=None #mandatory
				field_val['lte-rrc.cellReselectionPriority']=None #mandatory
				field_val['lte-rrc.threshX_High']=None #mandatory
				field_val['lte-rrc.threshX_High']=None #mandatory

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')


				if not self.__config.has_key(self.__status.id):
					self.__config[self.__status.id]=LteRrcConfig()
					self.__config[self.__status.id].status=self.__status

				neighbor_freq=int(field_val['lte-rrc.startingARFCN']) 
				self.__config[self.__status.id].sib.inter_freq_config[neighbor_freq]\
				=LteRrcSibInterFreqConfig(
						"GERAN",
						neighbor_freq,
						None,	#For 3G, tReselection is not in this IE
						float(field_val['lte-rrc.q_RxLevMin'])*2,
						float(field_val['lte-rrc.p_MaxGERAN']),
						float(field_val['lte-rrc.cellReselectionPriority']),
						float(field_val['lte-rrc.threshX_High'])*2,
						float(field_val['lte-rrc.threshX_Low'])*2,
						0)	#inter-RAT has no freq-offset

			#FIXME: t_ReselectionGERAN appears BEFORE config, so this code does not work!
			if field.get('name')=="lte-rrc.t_ReselectionGERAN":
				for config in self.__config[self.__status.id].sib.inter_freq_config.itervalues():
					if config.rat=="GERAN":
						config.tReselection = float(field.get('show'))


			#intra-frequency cell offset
			if field.get('name')=="lte-rrc.IntraFreqNeighCellInfo_element":
				field_val={}

				field_val['lte-rrc.physCellId']=None #mandatory
				field_val['lte-rrc.q_OffsetCell']=None #mandatory

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.__config.has_key(self.__status.id):
					self.__config[self.__status.id]=LteRrcConfig()
					self.__config[self.__status.id].status=self.__status

				cell_id=int(field_val['lte-rrc.physCellId'])
				offset=int(field_val['lte-rrc.q_OffsetCell'])
				self.__config[self.__status.id].sib.intra_freq_cell_config[cell_id]=q_offset_range[int(offset)]

			if field.get('name')=="lte-rrc.InterFreqNeighCellInfo_element":
				field_val={}

				field_val['lte-rrc.physCellId']=None #mandatory
				field_val['lte-rrc.q_OffsetCell']=None #mandatory

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.__config.has_key(self.__status.id):
					self.__config[self.__status.id]=LteRrcConfig()
					self.__config[self.__status.id].status=self.__status

				cell_id=int(field_val['lte-rrc.physCellId'])
				offset=int(field_val['lte-rrc.q_OffsetCell'])
				self.__config[self.__status.id].sib.inter_freq_cell_config[cell_id]=q_offset_range[int(offset)]




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
		print self.__class__.__name__,self.id,self.freq

	def inited(self):
		return (self.id!=None and self.freq!=None)

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
		print self.__class__.__name__
		self.status.dump()
		self.sib.dump()
		self.active.dump()


	def get_cell_reselection_config(self,cell,freq):
		"""
			Given a cell ID, return its resel configurations in serving cell (self.status)
		"""
		if freq==self.status.freq:
			#intra-frequency 
			offset_cell = 0
			if self.sib.intra_freq_cell_config.has_key(cell):
				offset_cell = self.sib.intra_freq_cell_config[cell]
			return LteRrcReselectionConfig(cell,freq,self.status.priority,offset_cell,None,None)
		else:
			#inter-frequency/RAT
			if not self.sib.inter_freq_config.has_key(freq):
				return None
			freq_config = self.sib.inter_freq_config[freq]
			offset_cell = 0
			if self.sib.inter_freq_cell_config.has_key(cell):
				offset_cell = self.sib.inter_freq_cell_config[cell]
			return LteRrcReselectionConfig(cell,freq,freq_config.priority,\
				freq_config.q_offset_freq+offset_cell, \
				freq_config.threshX_High,freq_config.threshX_Low)


class LteRrcSib:

	"""
		Per-cell Idle-state configurations
	"""
	def __init__(self):
		#FIXME: init based on the default value in TS36.331
		#configuration as a serving cell (LteRrcSibServ)
		self.serv_config = LteRrcSibServ(None,None,None,None) 

		#Per-frequency configurations
		#Intra-freq reselection config
		self.intra_freq_config = LteRrcSibIntraFreqConfig(None,None,None,None) 
		#Inter-freq/RAT reselection config. Freq -> LteRrcSibInterFreqConfig
		self.inter_freq_config = {}  

		#TODO: add intra_cell_config and inter_cell config, which maps individual cell offset
		self.intra_freq_cell_config = {} # cell -> offset
		self.inter_freq_cell_config = {} # cell -> offset

	def dump(self):
		self.serv_config.dump()
		self.intra_freq_config.dump()
		for item in self.inter_freq_config:
			self.inter_freq_config[item].dump()
		for item in self.intra_freq_cell_config:
			print "Intra-freq offset: ",item,self.intra_freq_cell_config[item]
		for item in self.inter_freq_cell_config:
			print "Inter-freq offset: ",item,self.inter_freq_cell_config[item]

class LteRrcReselectionConfig:
	def __init__(self,cell_id,freq,priority,offset,threshX_High,threshX_Low):
		self.id = cell_id
		self.freq = freq
		self.priority = priority
		self.offset = offset #adjusted offset by considering freq/cell-specific offsets
		self.threshX_High = threshX_High
		self.threshX_Low = threshX_Low

class LteRrcSibServ:
	"""
		Serving cell's cell-reselection configurations
	"""
	def __init__(self,priority,thresh_serv, s_nonintrasearch,q_hyst):
		self.priority = priority #cell reselection priority
		self.threshserv_low = thresh_serv #cell reselection threshold
		self.s_nonintrasearch = s_nonintrasearch #threshold for searching other frequencies
		self.q_hyst = q_hyst

	def dump(self):
		print self.__class__.__name__, self.priority, \
		self.threshserv_low,self.s_nonintrasearch

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
		print self.__class__.__name__,self.tReselection,self.q_RxLevMin,\
		self.p_Max,self.s_IntraSearch

class LteRrcSibInterFreqConfig:
	"""
		Inter-frequency cell-reselection configurations
	"""	
	#FIXME: the current list is incomplete
	#FIXME: individual cell offset
	def __init__(self,rat,freq,tReselection,q_RxLevMin,p_Max,priority,threshx_high,threshx_low,q_offset_freq):
		self.rat = rat
		self.freq = freq
		self.tReselection = tReselection
		self.q_RxLevMin = q_RxLevMin
		self.p_Max = p_Max
		self.priority = priority
		self.threshx_high = threshx_high
		self.threshx_low = threshx_low
		self.q_offset_freq = q_offset_freq

	def dump(self):
		print self.__class__.__name__,self.rat,self.freq, self.tReselection,\
		self.q_RxLevMin, self.p_Max, self.priority, \
		self.threshx_high, self.threshx_low

class LteRrcActive:
	def __init__(self):
		#TODO: initialize some containers
		pass

	def dump(self):
		pass

