#! /usr/bin/env python
"""
wcdma_rrc_analyzer.py

A WCDMA_RRC analyzer
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


class WcdmaRrcAnalyzer(Analyzer):

	def __init__(self):

		Analyzer.__init__(self)

		#init packet filters
		self.add_source_callback(self.rrc_filter)

		#init internal states
		self.status=WcdmaRrcStatus()	# current cell status
		self.history={}	# cell history: timestamp -> WcdmaRrcStatus()
		self.config={}	# cell_id -> WcdmaRrcConfig()

	def set_source(self,source):
		Analyzer.set_source(self,source)
		#enable WCDMA RRC log
		source.enable_log("WCDMA_Signaling_Messages")
		source.enable_log("WCDMA_CELL_ID")

	def rrc_filter(self,msg):
		
		"""
			Filter all RRC packets, and call functions to process it
			
			Args:
				msg: the event (message) from the trace collector
		"""

		log_item = msg.data
		log_item_dict = dict(log_item)

		#Convert msg to dictionary format
		raw_msg=Event(msg.timestamp,msg.type_id,log_item_dict)
		self.callback_serv_cell(raw_msg)

		log_xml = None
		if log_item_dict.has_key('Msg'):
			log_xml = ET.fromstring(log_item_dict['Msg'])
		else:
			return

		#Convert msg to xml format
		log_xml = ET.fromstring(log_item_dict['Msg'])
		xml_msg=Event(msg.timestamp,msg.type_id,log_xml)


		if msg.type_id == "WCDMA_Signaling_Messages":	
			self.callback_sib_config(xml_msg)
			#TODO: callback RRC

		else: #nothing to update
			return

		# Raise event to other analyzers
		# FIXME: the timestamp is incoherent with that from the trace collector
		e = Event(time.time(),'WcdmaRrcAnalyzer',"")
		self.send(e)


	def callback_serv_cell(self,msg):
		"""
			Update serving cell status
		"""
		if msg.data.has_key('UTRA DL Absolute RF channel number'):
			self.status.freq = msg.data['UTRA DL Absolute RF channel number']
		if msg.data.has_key('Cell identity (28-bits)'):
			self.status.id = msg.data['Cell identity (28-bits)']
		if msg.data.has_key('LAC id'):
			self.status.lac = msg.data['LAC id']
		if msg.data.has_key('RAC id'):
			self.status.rac = msg.data['RAC id']
		# self.status.dump()

		
	def callback_sib_config(self,msg):
		"""
			Callbacks for LTE RRC analysis
		"""

		for field in msg.data.iter('field'):

			#serving cell info
			if field.get('name')=="rrc.utra_ServingCell_element": 
				field_val={}

				#Default value setting
				#FIXME: set default to those in TS25.331
				field_val['rrc.priority']=None
				field_val['rrc.threshServingLow']=None
				field_val['rrc.s_PrioritySearch1']=None

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.config.has_key(self.status.id):
					self.config[self.status.id]=WcdmaRrcConfig()
					self.config[self.status.id].status=self.status

				self.config[self.status.id].serv_config=WcdmaRrcSibServ(
					field_val['rrc.priority'],
					field_val['rrc.threshServingLow'],
					field_val['rrc.s_PrioritySearch1'])

				self.config[self.status.id].serv_config.dump()

			#inter-RAT cell info (LTE)
			if field.get('name')=="rrc.EUTRA_FrequencyAndPriorityInfo_element":
				field_val={}

				#FIXME: set to the default value based on TS36.331
				field_val['rrc.earfcn']=None
				field_val['lte-rrc.t_ReselectionEUTRA']=None
				field_val['rrc.qRxLevMinEUTRA']=None
				field_val['lte-rrc.p_Max']=None
				field_val['rrc.priority']=None
				field_val['rrc.threshXhigh']=None
				field_val['rrc.threshXlow']=None

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				if not self.config.has_key(self.status.id):
					self.config[self.status.id]=WcdmaRrcConfig()
					self.config[self.status.id].status=self.status

				neighbor_freq=field_val['rrc.earfcn']
				self.config[self.status.id].sib.inter_freq_config[neighbor_freq]\
				=WcdmaRrcSibInterFreqConfig(
						field_val['rrc.earfcn'],
						field_val['lte-rrc.t_ReselectionEUTRA'],
						field_val['rrc.qRxLevMinEUTRA'],
						field_val['lte-rrc.p_Max'],
						field_val['rrc.priority'],
						field_val['rrc.threshXhigh'],
						field_val['rrc.threshXlow'])
				self.config[self.status.id].sib.inter_freq_config[neighbor_freq].dump()

			#TODO: RRC connection status update

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
		if self.config.has_key(cell_id):
			return self.config[cell_id]
		else:
			return None

	def get_cur_cell(self):
		"""
			Return a cell's configuration
			TODO: if no current cell (inactive), return None
		"""
		return self.status



class WcdmaRrcStatus:
	"""
		The metadata of a cell
	"""
	def __init__(self):
		self.id = None #cell ID
		self.freq = None #cell frequency
		self.rac = None #routing area code
		self.lac = None #location area code
		self.bandwidth = None #cell bandwidth
		self.conn = False #connectivity status (for serving cell only)

	def dump(self):
		print "WCDMA Cell Info: cellID=",self.id," Freq=",self.freq,\
			" RAC=",self.rac, " LAC=",self.lac

class WcdmaRrcConfig:
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
		self.status = WcdmaRrcStatus() #the metadata of this cell
		self.sib=WcdmaRrcSib()	#Idle-state
		self.active=WcdmaRrcActive() #active-state configurations

class WcdmaRrcSib:

	"""
		Per-cell Idle-state configurations
	"""
	def __init__(self):
		#FIXME: init based on the default value in TS25.331
		#configuration as a serving cell (LteRrcSibServ)
		self.serv_config = WcdmaRrcSibServ(None,None,None) 
		#Intra-freq reselection config
		self.intra_freq_config = WcdmaRrcSibIntraFreqConfig(None,None,None,None) 
		#Inter-freq/RAT reselection config. Freq/cell -> WcdmaRrcSibInterFreqConfig
		self.inter_freq_config = {}  


class WcdmaRrcSibServ:
	"""
		Serving cell's cell-reselection configurations
	"""
	def __init__(self,priority,thresh_serv, s_priority_search1):
		self.priority = priority #cell reselection priority
		self.threshserv_low = thresh_serv #cell reselection threshold
		self.s_priority_search1 = s_priority_search1 #threshold for searching other frequencies

	def dump(self):
		print "WcdmaRrcSibServ: ",self.priority,self.threshserv_low,self.s_priority_search1

class WcdmaRrcSibIntraFreqConfig:
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

class WcdmaRrcSibInterFreqConfig:
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

class WcdmaRrcActive:
	def __init__(self):
		#TODO: initialize some containers
		pass

