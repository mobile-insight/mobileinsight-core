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

        if msg.type_id == "UMTS_NAS_OTA" \
        or msg.type_id == "UMTS_NAS_GMM_State" \
        or msg.type_id == "UMTS_NAS_MM_State":
            # log_item = msg.data
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)

            # if not log_item_dict.has_key('Msg'):
            if 'Msg' not in log_item_dict:
                return

            #Convert msg to xml format
            log_xml = ET.XML(log_item_dict['Msg'])
            xml_msg=Event(msg.timestamp,msg.type_id,log_xml)

            self.__callback_mm(xml_msg)
            self.__callback_gmm(xml_msg)
            self.__callback_nas(xml_msg)



    def __callback_mm_state(self,msg):
        """
        Given the MM message, update MM state and substate.

        :param msg: the NAS signaling message that carries MM state
        """
        pass

    def __callback_gmm_state(self,msg):
        """
        Given the GMM message, update GMM state and substate.

        :param msg: the NAS signaling message that carries GMM state
        """
        pass

    def __callback_nas(self,msg):
        """
        Extrace MM status and configurations from the NAS messages

        :param msg: the MM NAS message
        """
        pass

