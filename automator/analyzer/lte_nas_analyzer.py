#! /usr/bin/env python
"""
lte_nas_analyzer.py

A LTE NAS layer (EMM/ESM) analyzer

Author: Yuanjie Li
"""

import xml.etree.ElementTree as ET
from analyzer import *
import time

__all__=["LteNasAnalyzer"]

emm_state={0:"deregistered",1:"registered"}
emm_substate={
	0: "deregistered.normal_service",
	1: "deregistered.limited_service",
	2: "deregistered.attempting_to_attach",
	3: "deregistered.plmn_search",
	4: "deregistered.no_imsi",
	5: "deregistered.attach_needed",
	6: "deregistered.no_cell_available",
	7: "registered.normal_service",
	8: "registered.attempting_to_update",
	9: "registered.limited_service",
	10: "registered.plmn_search",
	11: "registered.updated_needed",
	12: "registered.no_cell_available",
	13: "registered.attempting_to_update_mm",
	14: "registered.imsi_detach_inited"}

esm_state={0:"disconnected",1:"connected"}

#QoS mapping: 10.5.6.5, TS24.008
mean_tput={1:100,2:200,3:500,4:1000,5:2000,6:5000,7:10000,8:20000,
		9:50000,10:100000,11:200000,12:500000,13:1000000,14:2000000,
		15:5000000,16:10000000,17:20000000,18:50000000,31:"best effort"}
delivery_order={1:"with delivery order", 2:"without delivery order"}
traffic_class={1:"conversional class", 2:"streaming class",
		3:"interactive class", 4:"background class"}
residual_ber={1:5e-2, 2:1e-2, 3:5e-3, 4:4e-3, 5:1e-3, 6:1e-4, 7:1e-5,
		8:1e-6, 9:6e-8}

def max_bitrate(val):
	"""
		Given ESM value, return MBR (Kbps)
		R.f. 10.5.6.5, TS24.008
	"""
	if val<=63:
		return val
	elif val<=127:
		return 64+(val-64)*8
	elif val<=254:
		return 576+(val-128)*64
	else:
		return 0

def max_bitrate_ext(val):
	"""
		Given ESM value, return MBR (Kbps)
		R.f. 10.5.6.5, TS24.008
	"""
	if val<=74:
		return 8600+val*100
	elif val<=186:
		return 16000+(val-74)*1000
	elif val<=250:
		return 128000+(val-186)*2000
	else:
		return None

def trans_delay(val):
	"""
		Given ESM value, return transfer delay (ms)
		R.f. 10.5.6.5, TS24.008
	"""
	if val<=15:
		return val*10
	elif val<=31:
		return 200+(val-16)*50
	elif val<=62:
		return 1000+(val-32)*100
	else:
		return None



class LteNasAnalyzer(Analyzer):

	def __init__(self):
		Analyzer.__init__(self)

		#init packet filters
		self.add_source_callback(self.__nas_filter)

		#EMM/ESM status initialization
		self.__emm_status = EmmStatus()
		self.__esm_status = EsmStatus()

	def set_source(self,source):
		Analyzer.set_source(self,source)

		#Enable EMM/ESM logs
		source.enable_log("LTE_NAS_ESM_Plain_OTA_Incoming_Message")
		source.enable_log("LTE_NAS_ESM_Plain_OTA_Outgoing_Message")
		source.enable_log("LTE_NAS_EMM_Plain_OTA_Incoming_Message")
		source.enable_log("LTE_NAS_EMM_Plain_OTA_Outgoing_Message")

	def __nas_filter(self,msg):
		"""
			Filter all NAS(EMM/ESM) packets, and call functions to process it
			
			Args:
				msg: the event (message) from the trace collector

		"""

		log_item = msg.data
		log_item_dict = dict(log_item)

		if not log_item_dict.has_key('Msg'):
			return

		#Convert msg to xml format
		log_xml = ET.fromstring(log_item_dict['Msg'])
		xml_msg=Event(msg.timestamp,msg.type_id,log_xml)

		if msg.type_id == "LTE_NAS_ESM_Plain_OTA_Incoming_Message" \
		or msg.type_id == "LTE_NAS_ESM_Plain_OTA_Outgoing_Message" \
		or msg.type_id == "LTE_NAS_EMM_Plain_OTA_Incoming_Message" \
		or msg.type_id == "LTE_NAS_EMM_Plain_OTA_Outgoing_Message":	
			self.__callback_emm_state(xml_msg)
			self.__callback_emm(xml_msg)
			self.__callback_esm(xml_msg)


		e = Event(time.time(),self.__class__.__name__,"")
		self.send(e)

	def __callback_emm_state(self,msg):
		"""
			Update EMM state and substate
		"""
		for field in msg.data.iter('field'):

			if field.get('showname')=="NAS EPS Mobility Management Message Type: Attach request (0x41)":
				self.__emm_status.state="deregistered"
				self.__emm_status.substate="registered.attempting_to_update"

			if field.get('showname')=="NAS EPS Mobility Management Message Type: Attach complete (0x43)":
				self.__emm_status.state="registered"
				self.__emm_status.substate="registered.normal_service"

			if field.get('showname')=="NAS EPS Mobility Management Message Type: Detach request (0x45)":
				self.__emm_status.state="deregistered-initiated"
				self.__emm_status.substate="null"

			if field.get('showname')=="NAS EPS Mobility Management Message Type: Detach accept (0x45)":
				self.__emm_status.state="deregistered"
				self.__emm_status.substate="deregistered.attach_needed"

		self.__emm_status.dump()



	def __callback_emm(self,msg):
		"""
			Update EMM status
		"""

		for field in msg.data.iter('field'):

			if field.get('show')=="EPS mobile identity - GUTI":
				
				field_val={}

				field_val['e212.mcc']=None
				field_val['e212.mnc']=None
				field_val['nas_eps.emm.mme_grp_id']=None
				field_val['nas_eps.emm.mme_code']=None
				field_val['nas_eps.emm.m_tmsi']=None

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				self.__emm_status.guti.mcc=field_val['e212.mcc']
				self.__emm_status.guti.mnc=field_val['e212.mnc']
				self.__emm_status.guti.mme_group_id=field_val['nas_eps.emm.mme_grp_id']
				self.__emm_status.guti.mme_code=field_val['nas_eps.emm.mme_code']
				self.__emm_status.guti.m_tmsi=field_val['nas_eps.emm.m_tmsi']

			



	def __callback_esm(self,msg):

		"""
			Update ESM status
		"""
		for field in msg.data.iter('field'):

			if field.get('name')=="nas_eps.emm.qci":
				self.__esm_status.qos.qci=int(field.get('show'))

			if field.get('show')=="Quality Of Service - Negotiated QoS": 

				field_val={}

				for val in field.iter('field'):
					field_val[val.get('name')]=val.get('show')

				
				self.__esm_status.qos.delay_class=int(field_val['gsm_a.gm.sm.qos.delay_cls'])
				self.__esm_status.qos.reliability_class=int(field_val['gsm_a.gm.sm.qos.reliability_cls'])
				self.__esm_status.qos.precedence_class=int(field_val['gsm_a.gm.sm.qos.prec_class'])
				#10.5.6.5, TS24.008
				self.__esm_status.qos.peak_tput=1000*pow(2,int(field_val['gsm_a.gm.sm.qos.peak_throughput'])-1)
				self.__esm_status.qos.mean_tput=mean_tput[int(field_val['gsm_a.gm.sm.qos.mean_throughput'])]
				self.__esm_status.qos.traffic_class=int(field_val['gsm_a.gm.sm.qos.traffic_cls'])
				self.__esm_status.qos.delivery_order=int(field_val['gsm_a.gm.sm.qos.del_order'])
				self.__esm_status.qos.traffic_handling_priority=int(field_val['gsm_a.gm.sm.qos.traff_hdl_pri'])
				self.__esm_status.qos.residual_ber=residual_ber[int(field_val['gsm_a.gm.sm.qos.ber'])]

				self.__esm_status.qos.transfer_delay=trans_delay(int(field_val['gsm_a.gm.sm.qos.trans_delay']))

				self.__esm_status.qos.max_bitrate_ulink=max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_upl']))
				self.__esm_status.qos.max_bitrate_dlink=max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl']))
				self.__esm_status.qos.guaranteed_bitrate_ulink=max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_upl']))
				self.__esm_status.qos.guaranteed_bitrate_dlink=max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl']))
				
				self.__esm_status.qos.max_bitrate_ulink_ext=max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.max_bitrate_upl_ext']))
				self.__esm_status.qos.max_bitrate_dlink_ext=max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl_ext']))
				self.__esm_status.qos.guaranteed_bitrate_ulink_ext=max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_upl_ext']))
				self.__esm_status.qos.guaranteed_bitrate_dlink_ext=max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl_ext']))

				self.__esm_status.qos.dump_rate()
				self.__esm_status.qos.dump_delay()

	


class EmmStatus:

	def __init__(self):
		self.state = "null"
		self.substate = "null"
		self.guti = Guti()
		self.ciphering = None
		self.integrity = None

	def dump(self):
		print self.__class__.__name__,self.state,self.substate, \
		self.guti.mcc,self.guti.mnc,self.guti.mme_group_id, \
		self.guti.mme_code,self.guti.m_tmsi,self.ciphering,self.integrity

class Guti:
	def __init__(self):
		self.mcc=None
		self.mnc=None
		self.mme_group_id=None
		self.mme_code=None
		self.m_tmsi=None

	def inited(self):
		return (self.mcc and self.mnc and self.mme_group_id \
			and self.mme_code and self.m_tmsi)


class EsmStatus:

	def __init__(self):
		self.qos=EsmQos()

class EsmQos:
	def __init__(self):
		self.qci=None
		self.delay_class=None
		self.reliability_class=None
		self.precedence_class=None
		self.peak_tput=None
		self.mean_tput=None
		self.traffic_class=None
		self.delivery_order=None
		self.transfer_delay=None
		self.traffic_handling_priority=None
		self.max_bitrate_ulink=None
		self.max_bitrate_dlink=None
		self.guaranteed_bitrate_ulink=None
		self.guaranteed_bitrate_dlink=None
		self.max_bitrate_ulink_ext=None
		self.max_bitrate_dlink_ext=None
		self.guaranteed_bitrate_ulink_ext=None
		self.guaranteed_bitrate_dlink_ext=None
		self.residual_ber=None

	def dump_rate(self):
		print self.__class__.__name__,"Throughput(Kbps):",self.peak_tput,self.mean_tput, \
		self.max_bitrate_ulink, self.max_bitrate_dlink, \
		self.guaranteed_bitrate_ulink, self.guaranteed_bitrate_dlink, \
		self.max_bitrate_ulink_ext, self.max_bitrate_dlink_ext, \
		self.guaranteed_bitrate_ulink_ext, self.guaranteed_bitrate_dlink_ext

	def dump_delay(self):
		print self.__class__.__name__,"delay:",delivery_order[self.delivery_order], \
		traffic_class[self.traffic_class],self.qci,self.delay_class,self.transfer_delay, \
		self.residual_ber



