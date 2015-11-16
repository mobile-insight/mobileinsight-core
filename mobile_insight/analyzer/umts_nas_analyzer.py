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
            xml_msg = Event(msg.timestamp, msg.type_id, log_xml)

            self.__callback_mm(xml_msg)
            self.__callback_gmm(xml_msg)
            self.__callback_nas(xml_msg)

    def __callback_mm_state(self,msg):
        """
        Given the MM message, update MM state and substate.

        :param msg: the NAS signaling message that carries MM state
        """
        ''' Sample
        2015-11-14 18:06:57.054602:UMTS_NAS_OTA
        <dm_log_packet><pair key="type_id">UMTS_NAS_OTA</pair><pair key="timestamp">2015-11-15 01:49:28.068908</pair><pair key="Message Direction">FROM_UE</pair><pair key="Message Length">21</pair><pair key="Msg" type="list"><msg>
        <packet>
          <proto name="frame" pos="0" showname="Frame 0: 29 bytes on wire (232 bits), 29 bytes captured (232 bits)" size="29">
            <field name="frame.encap_type" pos="0" show="46" showname="Encapsulation type: USER 1 (46)" size="0" />
            <field name="frame.number" pos="0" show="0" showname="Frame Number: 0" size="0" />
            <field name="frame.len" pos="0" show="29" showname="Frame Length: 29 bytes (232 bits)" size="0" />
            <field name="frame.cap_len" pos="0" show="29" showname="Capture Length: 29 bytes (232 bits)" size="0" />
            <field name="frame.marked" pos="0" show="0" showname="Frame is marked: False" size="0" />
            <field name="frame.ignored" pos="0" show="0" showname="Frame is ignored: False" size="0" />
            <field name="frame.protocols" pos="0" show="user_dlt:aww:gsm_a.dtap" showname="Protocols in frame: user_dlt:aww:gsm_a.dtap" size="0" />
          </proto>
          <proto name="user_dlt" pos="0" showname="DLT: 148, Payload: aww (Automator Wireshark Wrapper)" size="29" />
          <proto name="aww" pos="0" showname="Automator Wireshark Wrapper" size="29">
            <field name="aww.proto" pos="0" show="190" showname="Protocol: 190" size="4" value="000000be" />
            <field name="aww.data_len" pos="4" show="21" showname="Data length: 21" size="4" value="00000015" />
          </proto>
          <proto name="gsm_a.dtap" pos="8" showname="GSM A-I/F DTAP - Location Updating Request" size="21">
            <field name="gsm_a.dtap.protocol_discriminator" pos="8" show="5" showname="Protocol Discriminator: Mobility Management messages (5)" size="1" value="05">
              <field name="gsm_a.L3_protocol_discriminator" pos="8" show="5" showname=".... 0101 = Protocol discriminator: Mobility Management messages (0x05)" size="1" unmaskedvalue="05" value="5" />
              <field name="gsm_a.skip.ind" pos="8" show="0" showname="0000 .... = Skip Indicator: No indication of selected PLMN (0)" size="1" unmaskedvalue="05" value="0" />
            </field>
            <field name="gsm_a.dtap.seq_no" pos="9" show="0" showname="00.. .... = Sequence number: 0" size="1" unmaskedvalue="08" value="0" />
            <field name="gsm_a.dtap.msg_mm_type" pos="9" show="8" showname="..00 1000 = DTAP Mobility Management Message Type: Location Updating Request (0x08)" size="1" unmaskedvalue="08" value="8" />
            <field name="" pos="10" show="Ciphering Key Sequence Number" size="1" value="02">
              <field name="gsm_a.spare_bits" pos="10" show="0" showname="0... .... = Spare bit(s): 0" size="1" value="02" />
              <field name="gsm_a.dtap.ciphering_key_sequence_number" pos="10" show="0" showname=".000 .... = Ciphering Key Sequence Number: 0" size="1" unmaskedvalue="02" value="0" />
            </field>
            <field name="" pos="10" show="Location Updating Type - IMSI attach" size="1" value="02">
              <field name="gsm_a.dtap.follow_on_request" pos="10" show="0" showname=".... 0... = Follow-On Request (FOR): No follow-on request pending" size="1" unmaskedvalue="02" value="0" />
              <field name="gsm_a.spare_bits" pos="10" show="0" showname=".... .0.. = Spare bit(s): 0" size="1" value="02" />
              <field name="gsm_a.dtap.updating_type" pos="10" show="2" showname=".... ..10 = Updating Type: IMSI attach (2)" size="1" unmaskedvalue="02" value="2" />
            </field>
            <field name="" pos="11" show="Location Area Identification (LAI)" size="5" value="13006266a9">
              <field name="" pos="11" show="Location Area Identification (LAI) - 310/260/26281" size="5" value="13006266a9">
                <field name="e212.mcc" pos="11" show="310" showname="Mobile Country Code (MCC): United States of America (310)" size="2" value="1300" />
                <field name="e212.mnc" pos="12" show="260" showname="Mobile Network Code (MNC): T-Mobile USA (260)" size="2" value="0062" />
                <field name="gsm_a.lac" pos="14" show="26281" showname="Location Area Code (LAC): 0x66a9 (26281)" size="2" value="66a9" />
              </field>
            </field>
            <field name="" pos="16" show="Mobile Station Classmark 1" size="1" value="57">
              <field name="" pos="16" show="Mobile Station Classmark 1" size="1" value="57">
                <field name="gsm_a.spareb8" pos="16" show="0" showname="0... .... = Spare: 0" size="1" unmaskedvalue="57" value="0" />
                <field name="gsm_a.MSC_rev" pos="16" show="2" showname=".10. .... = Revision Level: Used by mobile stations supporting R99 or later versions of the protocol (2)" size="1" unmaskedvalue="57" value="2" />
                <field name="gsm_a.ES_IND" pos="16" show="1" showname="...1 .... = ES IND: Controlled Early Classmark Sending option is implemented in the MS" size="1" unmaskedvalue="57" value="1" />
                <field name="gsm_a.A5_1_algorithm_sup" pos="16" show="0" showname=".... 0... = A5/1 algorithm supported: encryption algorithm A5/1 available" size="1" unmaskedvalue="57" value="0" />
                <field name="gsm_a.RF_power_capability" pos="16" show="7" showname=".... .111 = RF Power Capability: RF Power capability is irrelevant in this information element (7)" size="1" unmaskedvalue="57" value="7" />
              </field>
            </field>
            <field name="" pos="17" show="Mobile Identity - TMSI/P-TMSI (0x965e0419)" size="6" value="05f4965e0419">
              <field name="gsm_a.len" pos="17" show="5" showname="Length: 5" size="1" value="05" />
              <field name="gsm_a.unused" pos="18" show="15" showname="1111 .... = Unused: 0x0f" size="1" unmaskedvalue="f4" value="F" />
              <field name="gsm_a.oddevenind" pos="18" show="0" showname=".... 0... = Odd/even indication: Even number of identity digits" size="1" unmaskedvalue="f4" value="0" />
              <field name="gsm_a.ie.mobileid.type" pos="18" show="4" showname=".... .100 = Mobile Identity Type: TMSI/P-TMSI/M-TMSI (4)" size="1" unmaskedvalue="f4" value="4" />
              <field name="gsm_a.tmsi" pos="19" show="2522743833" showname="TMSI/P-TMSI: 0x965e0419" size="4" value="965e0419" />
            </field>
            <field name="" pos="23" show="Mobile Station Classmark 2 - Mobile station classmark for UMTS" size="5" value="33035758a6">
              <field name="gsm_a.common.elem_id" pos="23" show="51" showname="Element ID: 0x33" size="1" value="33" />
              <field name="gsm_a.len" pos="24" show="3" showname="Length: 3" size="1" value="03" />
              <field name="gsm_a.spareb8" pos="25" show="0" showname="0... .... = Spare: 0" size="1" unmaskedvalue="57" value="0" />
              <field name="gsm_a.MSC_rev" pos="25" show="2" showname=".10. .... = Revision Level: Used by mobile stations supporting R99 or later versions of the protocol (2)" size="1" unmaskedvalue="57" value="2" />
              <field name="gsm_a.ES_IND" pos="25" show="1" showname="...1 .... = ES IND: Controlled Early Classmark Sending option is implemented in the MS" size="1" unmaskedvalue="57" value="1" />
              <field name="gsm_a.A5_1_algorithm_sup" pos="25" show="0" showname=".... 0... = A5/1 algorithm supported: encryption algorithm A5/1 available" size="1" unmaskedvalue="57" value="0" />
              <field name="gsm_a.RF_power_capability" pos="25" show="7" showname=".... .111 = RF Power Capability: RF Power capability is irrelevant in this information element (7)" size="1" unmaskedvalue="57" value="7" />
              <field name="gsm_a.spareb8" pos="26" show="0" showname="0... .... = Spare: 0" size="1" unmaskedvalue="58" value="0" />
              <field name="gsm_a.ps_sup_cap" pos="26" show="1" showname=".1.. .... = PS capability (pseudo-synchronization capability): PS capability present" size="1" unmaskedvalue="58" value="1" />
              <field name="gsm_a.SS_screening_indicator" pos="26" show="1" showname="..01 .... = SS Screening Indicator: Capability of handling of ellipsis notation and phase 2 error handling  (1)" size="1" unmaskedvalue="58" value="1" />
              <field name="gsm_a.SM_cap" pos="26" show="1" showname=".... 1... = SM capability (MT SMS pt to pt capability): Mobile station supports mobile terminated point to point SMS" size="1" unmaskedvalue="58" value="1" />
              <field name="gsm_a.VBS_notification_rec" pos="26" show="0" showname=".... .0.. = VBS notification reception: no VBS capability or no notifications wanted" size="1" unmaskedvalue="58" value="0" />
              <field name="gsm_a.VGCS_notification_rec" pos="26" show="0" showname=".... ..0. = VGCS notification reception: no VGCS capability or no notifications wanted" size="1" unmaskedvalue="58" value="0" />
              <field name="gsm_a.FC_frequency_cap" pos="26" show="0" showname=".... ...0 = FC Frequency Capability: The MS does not support the E-GSM or R-GSM band" size="1" unmaskedvalue="58" value="0" />
              <field name="gsm_a.CM3" pos="27" show="1" showname="1... .... = CM3: The MS supports options that are indicated in classmark 3 IE" size="1" unmaskedvalue="a6" value="1" />
              <field name="gsm_a.spareb7" pos="27" show="0" showname=".0.. .... = Spare: 0" size="1" unmaskedvalue="a6" value="0" />
              <field name="gsm_a.LCS_VA_cap" pos="27" show="1" showname="..1. .... = LCS VA capability (LCS value added location request notification capability): LCS value added location request notification capability supported" size="1" unmaskedvalue="a6" value="1" />
              <field name="gsm_a.UCS2_treatment" pos="27" show="0" showname="...0 .... = UCS2 treatment: the ME has a preference for the default alphabet" size="1" unmaskedvalue="a6" value="0" />
              <field name="gsm_a.SoLSA" pos="27" show="0" showname=".... 0... = SoLSA: The ME does not support SoLSA" size="1" unmaskedvalue="a6" value="0" />
              <field name="gsm_a.CMSP" pos="27" show="1" showname=".... .1.. = CMSP: CM Service Prompt: Network initiated MO CM connection request supported for at least one CM protocol" size="1" unmaskedvalue="a6" value="1" />
              <field name="gsm_a.A5_3_algorithm_sup" pos="27" show="1" showname=".... ..1. = A5/3 algorithm supported: encryption algorithm A5/3 available" size="1" unmaskedvalue="a6" value="1" />
              <field name="gsm_a.A5_2_algorithm_sup" pos="27" show="0" showname=".... ...0 = A5/2 algorithm supported: encryption algorithm A5/2 not available" size="1" unmaskedvalue="a6" value="0" />
            </field>
            <field name="" pos="28" show="Extraneous Data" size="1" value="e1" />
          </proto>
        </packet>
        '''


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
