#!/usr/bin/python
# Filename: umts_nas_analyzer.py
"""

A UMTS NAS layer (MM/GMM/CM/SM) analyzer

Author: Yuanjie Li
"""

try: 
    import xml.etree.cElementTree as ET 
except ImportError: 
    import xml.etree.ElementTree as ET
from analyzer import *
import timeit

from profile import Profile, ProfileHierarchy

__all__=["UmtsNasAnalyzer"]


class UmtsNasAnalyzer(Analyzer):

    """
    A protocol analyzer for UMTS NAS layer (MM/GMM/CM/SM)
    """

    def __init__(self):

        Analyzer.__init__(self)
        #init packet filters
        self.add_source_callback(self.__nas_filter)

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE NAS messages.

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)
        #Enable MM/GMM/CM/SM logs
        source.enable_log("UMTS_NAS_OTA")
        source.enable_log("UMTS_NAS_GMM_State")
        source.enable_log("UMTS_NAS_MM_State")

    def __nas_filter(self,msg):

        """
        Filter all NAS(MM/GMM/CM/SM) packets, and call functions to process it

        :param msg: the event (message) from the trace collector.
        """

        if msg.type_id == "UMTS_NAS_MM_State":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            raw_msg = Event(msg.timestamp,msg.type_id,log_item_dict)
            self.__callback_mm_state(raw_msg)
    
        if msg.type_id == "UMTS_NAS_GMM_State":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            raw_msg = Event(msg.timestamp,msg.type_id,log_item_dict)
            self.__callback_gmm_state(raw_msg)


        if msg.type_id == "UMTS_NAS_OTA":
            # log_item = msg.data
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)

            # if not log_item_dict.has_key('Msg'):
            if 'Msg' not in log_item_dict:
                return

            #Convert msg to xml format
            log_xml = ET.XML(log_item_dict['Msg'])
            xml_msg=Event(msg.timestamp,msg.type_id,log_xml)
            self.__callback_nas(xml_msg)

    def __callback_mm_state(self,msg):
        """
        Given the MM message, update MM state and substate.

        :param msg: the NAS signaling message that carries MM state
        """
        self.__mm_status.state = msg.data["MM State"]
        self.__mm_status.substate = msg.data["MM Substate"]
        self.__mm_status.update_status = msg.data["MM Update Status"]



    def __callback_gmm_state(self,msg):
        """
        Given the GMM message, update GMM state and substate.

        :param msg: the NAS signaling message that carries GMM state
        """
        ''' Sample
        2015-11-14 18:06:47.446913:UMTS_NAS_GMM_State
        <dm_log_packet><pair key="type_id">UMTS_NAS_GMM_State</pair><pair key="timestamp">2015-11-15 01:49:26.380084</pair><pair key="GMM State">GMM_DEREGISTERED</pair><pair key="GMM Substate">GMM_PLMN_SEARCH</pair><pair key="GMM Update Status">GMM_GU1_UPDATED</pair></dm_log_packet>
        MsgLogger UMTS_NAS_GMM_State 3.57007980347
        '''
        for dm_log_packet in msg.data.iter('dm_log_packet'):
            for pair in dm_log_packet.iter('pair'):
                if key.get('type_id') == "UMTS_NAS_GMM_State":
                    key_val = {}
                    self.__gmm_status.timestamp = key_val['timestamp']
                    self.__gmm_status.gmm_state = key_val['GMM State']
                    self.__gmm_status.gmm_substate = key_val['GMM Substate']
                    self.__gmm_status.gmm_update_status = key_val['GMM Update Status']

    def __callback_nas(self,msg):
        """
        Extrace MM status and configurations from the NAS messages

        :param msg: the MM NAS message
        """
        ''' Sample
        2015-11-14 18:06:47.441853:UMTS_NAS_MM_State
        <dm_log_packet><pair key="type_id">UMTS_NAS_MM_State</pair><pair key="timestamp">2015-11-15 01:49:26.379322</pair><pair key="MM State">MM_IDLE</pair><pair key="MM Substate">MM_PLMN_SEARCH</pair><pair key="MM Update Status">UPDATED</pair></dm_log_packet>
        MsgLogger UMTS_NAS_MM_State 3.64398956299
        '''

        for dm_log_packet in msg.data.iter('dm_log_packet'):
            for pair in dm_log_packet.iter('pair'):
                if key.get('type_id') == "UMTS_NAS_MM_State":
                    key_val = {}
                    self.__mm_status.timestamp = key_val['timestamp']
                    self.__mm_status.mm_state = key_val['MM State']
                    self.__mm_status.mm_substate = key_val['MM Substate']
                    self.__mm_status.mm_update_status = key_val['MM Update Status']
