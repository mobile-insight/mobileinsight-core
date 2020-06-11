#!/usr/bin/python
# Filename: umts_nas_analyzer.py
"""

A UMTS NAS layer (MM/GMM/CM/SM) analyzer

Author: Yuanjie Li
Author: Zengwen Yuan
"""

import xml.etree.ElementTree as ET
from .analyzer import *
from .state_machine import *
import timeit

from .protocol_analyzer import *
from .profile import Profile, ProfileHierarchy

from .nas_util import *
# from mobile_insight.element import *


__all__=["UmtsNasAnalyzer"]


class UmtsNasAnalyzer(ProtocolAnalyzer):

    """
    A protocol analyzer for UMTS NAS layer (MM/GMM/CM/SM)
    """

    def __init__(self):

        self.log_info("Initialing UmtsNasAnalyzer..")

        ProtocolAnalyzer.__init__(self)
        #init packet filters
        self.add_source_callback(self.__nas_filter)

        self.__mm_status = MmStatus()
        self.__gmm_status = GmmStatus()
        self.__mm_nas_status = MmNasStatus()
        self.mm_state_machine = self.create_mm_state_machine()
        self.gmm_state_machine = self.create_gmm_state_machine()
        self.cm_state_machine = self.create_cm_state_machine()

    def create_profile_hierarchy(self):
        '''
        Return a UMTS NAS ProfileHierarchy (configurations)

        :returns: ProfileHierarchy for LTE NAS
        '''
        return UmtsNasProfileHierarchy()

    def create_mm_state_machine(self):
        """
        Declare an MM state machine

        returns: a StateMachine
        """

        def to_wait_ntk(msg):
            if msg.type_id == "UMTS_NAS_MM_State" and str(msg.data["MM State"]) == 'CELL_FACH':
                return True

        def to_idle(msg):
            if msg.type_id == "UMTS_NAS_MM_State" and str(msg.data['MM State']) == 'MM_IDLE':
                return True

        def to_wait_outgoing_con(msg):
            if msg.type_id == "UMTS_NAS_MM_State" and str(msg.data['MM State']) == 'MM_WAIT_FOR_OUTGOING_MM_CONNECTION':
                return True

        def to_con_active(msg):
            if msg.type_id == "UMTS_NAS_MM_State" and str(msg.data['MM State']) == 'MM_CONNECTION_ACTIVE':
                return True

        def init_state(msg):
            if msg.type_id == "UMTS_NAS_MM_State":
                state = str(msg.data['MM State'])
                if state in ["MM_WAIT_FOR_NETWORK_COMMAND", "MM_IDLE", "MM_WAIT_FOR_OUTGOING_MM_CONNECTION", "MM_CONNECTION_ACTIVE"]:
                    return state

        state_machine={"MM_WAIT_FOR_NETWORK_COMMAND": {'MM_IDLE': to_idle, 'MM_CONNECTION_ACTIVE': to_con_active},
                       "MM_IDLE": {'MM_WAIT_FOR_OUTGOING_MM_CONNECTION': to_wait_outgoing_con},
                       "MM_WAIT_FOR_OUTGOING_MM_CONNECTION": {'MM_CONNECTION_ACTIVE': to_con_active},
                       "MM_CONNECTION_ACTIVE": {'MM_WAIT_FOR_NETWORK_COMMAND': to_wait_ntk, 'MM_WAIT_FOR_OUTGOING_MM_CONNECTION': to_wait_outgoing_con, 'MM_IDLE': to_idle}}

        return StateMachine(state_machine, init_state)

    def create_gmm_state_machine(self):
        """
        Declare a GMM state machine

        returns: a StateMachine
        """

        def to_deregistered(msg):
            if msg.type_id == "UMTS_NAS_GMM_State" and str(msg.data["GMM State"]) == 'GMM_DEREGISTERED':
                return True

        def to_registered(msg):
            if msg.type_id == "UMTS_NAS_GMM_State" and str(msg.data['GMM State']) == 'GMM_REGISTERED':
                return True

        def init_state(msg):
            if msg.type_id == "UMTS_NAS_GMM_State":
                msg_state = str(msg.data['GMM State'])
                state = msg_state if msg_state in ['GMM_DEREGISTERED', 'GMM_REGISTERED'] else None
                return state

        state_machine={"GMM_REGISTERED": {'GMM_DEREGISTERED': to_deregistered},
                       "GMM_DEREGISTERED": {'GMM_REGISTERED': to_registered}}

        return StateMachine(state_machine, init_state)


    def create_cm_state_machine(self):
        """
        Declare a GMM state machine

        returns: a StateMachine
        """

        def to_service_req(msg):
            if msg.type_id == "UMTS_NAS_OTA_Packet" and str(msg.data) == "CM Service Request":
                return True

        def to_setup(msg):
            if msg.type_id == "UMTS_NAS_OTA_Packet" and str(msg.data) == 'Setup':
                return True

        def to_call_proceeding(msg):
            if msg.type_id == "UMTS_NAS_OTA_Packet" and str(msg.data) == 'Call Proceeding':
                return True

        def to_alerting(msg):
            if msg.type_id == "UMTS_NAS_OTA_Packet" and str(msg.data) == 'Alerting':
                return True

        def to_connect(msg):
            if msg.type_id == "UMTS_NAS_OTA_Packet" and str(msg.data) == 'Connect':
                return True

        def to_connect_ack(msg):
            if msg.type_id == "UMTS_NAS_OTA_Packet" and str(msg.data) == 'Connect Acknowledge':
                return True

        def to_disconnect(msg):
            if msg.type_id == "UMTS_NAS_OTA_Packet" and str(msg.data) == 'Disconnect':
                return True

        def to_release(msg):
            if msg.type_id == "UMTS_NAS_OTA_Packet" and str(msg.data) == 'Release':
                return True

        def to_idle(msg):
            if msg.type_id == "UMTS_NAS_OTA_Packet":
                if str(msg.data) == 'Release Complete' or str(msg.data) == 'CM Service Abort':
                    return True

        def init_state(msg):
            if msg.type_id == "UMTS_NAS_OTA_Packet" and str(msg.data) == "CM Service Request":
                return "CM_SERVICE_REQUEST"

        state_machine={"CM_IDLE": {"CM_SERVICE_REQUEST": to_service_req},
                       "CM_SERVICE_REQUEST": {'CM_SETUP': to_setup, 'CM_IDLE': to_idle},
                       "CM_SETUP": {'CM_CALL_PROCEEDING': to_call_proceeding},
                       "CM_CALL_PROCEEDING": {'CM_ALERTING': to_alerting, 'CM_DISCONNET': to_disconnect},
                       "CM_ALERTING": {'CM_CONNECT': to_connect, 'CM_DISCONNET': to_disconnect},
                       "CM_CONNECT": {'CM_CONNECT_ACK': to_connect_ack, 'CM_DISCONNET': to_disconnect},
                       "CM_CONNECT_ACK": {'CM_DISCONNET': to_disconnect},
                       "CM_DISCONNET": {"CM_RELEASE": to_release},
                       "CM_RELEASE": {"CM_IDLE": to_idle}}

        return StateMachine(state_machine, init_state)

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE NAS messages.

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)
        #Enable MM/GMM/CM/SM logs
        source.enable_log("UMTS_NAS_OTA_Packet")
        source.enable_log("UMTS_NAS_GMM_State") #GMM state/GMM substate
        source.enable_log("UMTS_NAS_MM_State") #MM state/MM substate
        source.enable_log("UMTS_NAS_MM_REG_State")

    def __nas_filter(self,msg):

        """
        Filter all NAS(MM/GMM/CM/SM) packets, and call functions to process it

        :param msg: the event (message) from the trace collector.
        """

        if msg.type_id == "UMTS_NAS_MM_State":

            self.log_info("Find One UMTS_NAS_MM_State")

            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            raw_msg = Event(msg.timestamp,msg.type_id,log_item_dict)
            self.__callback_mm_state(raw_msg)
            if self.mm_state_machine.update_state(raw_msg):
                self.log_info("MM State: " + self.mm_state_machine.get_current_state())


        if msg.type_id == "UMTS_NAS_MM_REG_State":

            self.log_info("Find One UMTS_NAS_MM_REG_State")

            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            raw_msg = Event(msg.timestamp,msg.type_id,log_item_dict)
            self.__callback_mm_reg_state(raw_msg)

    
        if msg.type_id == "UMTS_NAS_GMM_State":

            self.log_info("Find One UMTS_NAS_GMM_State")

            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            raw_msg = Event(msg.timestamp,msg.type_id,log_item_dict)
            self.__callback_gmm_state(raw_msg)
            if self.gmm_state_machine.update_state(raw_msg):
                self.log_info("GMM State: " + self.gmm_state_machine.get_current_state())



        if msg.type_id == "UMTS_NAS_OTA_Packet":

            self.log_info("Find One UMTS_NAS_OTA_Packet")

            # log_item = msg.data
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)

            # if not log_item_dict.has_key('Msg'):
            if 'Msg' not in log_item_dict:
                return

            #Convert msg to xml format
            log_xml = ET.XML(log_item_dict['Msg'])
            xml_msg = Event(msg.timestamp,msg.type_id,log_xml)
            # print str(log_item_dict)

            self.__callback_nas(xml_msg)

    def __callback_mm_state(self,msg):

        """
        Given the MM message, update MM state and substate.

        :param msg: the NAS signaling message that carries MM state
        """
        self.__mm_status.state = msg.data["MM State"]
        self.__mm_status.substate = msg.data["MM Substate"]
        self.__mm_status.update_status = msg.data["MM Update Status"]

        self.log_info(self.__mm_status.dump())

        # broadcast
        mm_state = {}
        mm_state["conn state"] = self.__mm_status.state
        mm_state["conn substate"] = self.__mm_status.substate
        mm_state["update state"] = self.__mm_status.update_status
        mm_state['timestamp'] = str(msg.data["timestamp"])
        self.broadcast_info("MM_STATE", mm_state)

    def __callback_mm_reg_state(self,msg):
        """
        Given the MM message, update MM state and substate.

        :param msg: the NAS signaling message that carries MM state
        """
        self.__mm_status.plmn = msg.data["PLMN"]
        self.__mm_status.lac = msg.data["LAC"]
        self.__mm_status.rac = msg.data["RAC"]
        self.__mm_status.operation_mode = msg.data["Network operation mode"]
        self.__mm_status.service_type = msg.data["CS/PS service type"]

        self.log_info(self.__mm_status.dump())

        # broadcast
        mm_reg_state = {}
        mm_reg_state["service type"] = self.__mm_status.service_type
        mm_reg_state["operation mode"] = self.__mm_status.operation_mode

        # Bug here. without exception catch, the process will terminate here.
        # but it do works.
        try:
            self.broadcast_info("MM_REG_STATE", mm_reg_state)
        except:
            pass

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
        self.__gmm_status.state = msg.data['GMM State']
        self.__gmm_status.substate = msg.data['GMM Substate']
        self.__gmm_status.update_status = msg.data['GMM Update Status']

        #broadcast
        gmm_state = {}
        gmm_state["conn state"] = self.__gmm_status.state
        gmm_state["conn substate"] = self.__gmm_status.substate
        gmm_state['timestamp'] = str(msg.data["timestamp"])
        self.broadcast_info("GMM_STATE", gmm_state)

    def __callback_nas(self,msg):
        """
        Extrace MM status and configurations from the NAS messages

        :param msg: the MM NAS message
        """

        # for proto in msg.data.iter('proto'):
        #     if proto.get('name') == "gsm_a.dtap": #GSM A-I/F DTAP - Location Updating Request

        for proto in msg.data.iter('proto'):
            if proto.get('name') == "gsm_a.dtap":
                raw_state_name = proto.get('showname')
                raw_state = raw_state_name.split('-')[-1].split('(')[0]
                if raw_state != "" and raw_state[0] == " ":
                    raw_state = raw_state[1:]
                if raw_state != "" and raw_state[-1] == " ":
                    raw_state = raw_state[:-1]
                # print raw_state
                if self.cm_state_machine.update_state(Event(msg.timestamp, msg.type_id, raw_state)):
                    cm_state = {}
                    cm_state["state"] = self.cm_state_machine.get_current_state()
                    cm_state['timestamp'] = str(msg.timestamp)
                    self.broadcast_info("CM_STATE", cm_state)
                    self.log_info("CM State: " + self.cm_state_machine.get_current_state())


        for field in msg.data.iter('field'):
            if field.get('show') == "DRX Parameter":
                field_val = {}

                # Default value setting
                field_val["gsm_a.gm.gmm.split_pg_cycle_code"] = None
                field_val["gsm_a.gm.gmm.cn_spec_drx_cycle_len_coef"] = None
                field_val["gsm_a.gm.gmm.split_on_ccch"] = None
                field_val["gsm_a.gm.gmm.non_drx_timer"] = None

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                self.__mm_nas_status.drx.split_pg_cycle_code = field_val["gsm_a.gm.gmm.split_pg_cycle_code"]
                self.__mm_nas_status.drx.cn_spec_drx_cycle_len_coef = field_val["gsm_a.gm.gmm.cn_spec_drx_cycle_len_coef"]
                self.__mm_nas_status.drx.split_on_ccch = field_val["gsm_a.gm.gmm.split_on_ccch"]
                self.__mm_nas_status.drx.non_drx_timer = field_val["gsm_a.gm.gmm.non_drx_timer"]

            if field.get('show') == "Quality Of Service - New QoS" \
            or field.get('show') == "Quality Of Service - Negotiated QoS":
                field_val = {}

                # Default value setting
                # field_val['gsm_a.len'] = None
                # field_val["gsm_a.spare_bits"] = None
                field_val["gsm_a.gm.sm.qos.delay_cls"] = None
                field_val["gsm_a.gm.sm.qos.reliability_cls"] = None
                field_val["gsm_a.gm.sm.qos.peak_throughput"] = None
                # field_val["gsm_a.spare_bits"] = None
                field_val["gsm_a.gm.sm.qos.prec_class"] = None
                # field_val["gsm_a.spare_bits"] = None
                field_val["gsm_a.gm.sm.qos.mean_throughput"] = None
                field_val["gsm_a.gm.sm.qos.traffic_cls"] = None
                field_val["gsm_a.gm.sm.qos.del_order"] = None
                # field_val["gsm_a.gm.sm.qos.del_of_err_sdu"] = None
                # field_val["gsm_a.gm.sm.qos.max_sdu"] = None
                field_val["gsm_a.gm.sm.qos.max_bitrate_upl"] = 0
                field_val["gsm_a.gm.sm.qos.max_bitrate_downl"] = 0
                field_val["gsm_a.gm.sm.qos.ber"] = None
                # field_val["gsm_a.gm.sm.qos.sdu_err_rat"] = None
                field_val["gsm_a.gm.sm.qos.trans_delay"] = None
                field_val["gsm_a.gm.sm.qos.traff_hdl_pri"] = None
                field_val["gsm_a.gm.sm.qos.guar_bitrate_upl"] = 0
                field_val["gsm_a.gm.sm.qos.guar_bitrate_downl"] = 0
                # field_val["gsm_a.spare_bits"] = None
                # field_val["gsm_a.gm.sm.qos.signalling_ind"] = None
                # field_val["gsm_a.gm.sm.qos.source_stat_desc"] = None
                field_val["gsm_a.gm.sm.qos.max_bitrate_downl_ext"] = 0
                field_val["gsm_a.gm.sm.qos.guar_bitrate_downl_ext"] = 0

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')
                    if "Maximum SDU size" in val.get('show'):
                        field_val["gsm_a.gm.`sm.qos.max_sdu"] = val.get('value')

                # 10.5.6.5, TS24.008
                self.__mm_nas_status.qos_negotiated.delay_class = int(field_val['gsm_a.gm.sm.qos.delay_cls'])
                self.__mm_nas_status.qos_negotiated.reliability_class = int(field_val['gsm_a.gm.sm.qos.reliability_cls'])
                self.__mm_nas_status.qos_negotiated.peak_throughput = 1000 * pow(2, int(field_val["gsm_a.gm.sm.qos.peak_throughput"]) - 1)
                self.__mm_nas_status.qos_negotiated.precedence_class = int(field_val['gsm_a.gm.sm.qos.prec_class'])
                self.__mm_nas_status.qos_negotiated.mean_throughput = mean_tput[int(field_val["gsm_a.gm.sm.qos.mean_throughput"])]
                self.__mm_nas_status.qos_negotiated.traffic_class = int(field_val['gsm_a.gm.sm.qos.traffic_cls'])
                self.__mm_nas_status.qos_negotiated.delivery_order = int(field_val['gsm_a.gm.sm.qos.del_order'])
                self.__mm_nas_status.qos_negotiated.traffic_handling_priority = int(field_val['gsm_a.gm.sm.qos.traff_hdl_pri'])
                self.__mm_nas_status.qos_negotiated.residual_ber = residual_ber[int(field_val['gsm_a.gm.sm.qos.ber'])]
                self.__mm_nas_status.qos_negotiated.transfer_delay = trans_delay(int(field_val['gsm_a.gm.sm.qos.trans_delay']))
                self.__mm_nas_status.qos_negotiated.max_bitrate_ulink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_upl']))
                self.__mm_nas_status.qos_negotiated.max_bitrate_dlink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl']))
                self.__mm_nas_status.qos_negotiated.guaranteed_bitrate_ulink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_upl']))
                self.__mm_nas_status.qos_negotiated.guaranteed_bitrate_dlink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl']))
                # self.__mm_nas_status.qos_negotiated.max_bitrate_ulink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.max_bitrate_upl_ext']))
                self.__mm_nas_status.qos_negotiated.max_bitrate_dlink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl_ext']))
                # self.__mm_nas_status.qos_negotiated.guaranteed_bitrate_ulink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_upl_ext']))
                self.__mm_nas_status.qos_negotiated.guaranteed_bitrate_dlink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl_ext']))


                # self.__mm_nas_status.qos_negotiated.del_of_err_sdu = field_val["gsm_a.gm.sm.qos.del_of_err_sdu"]
                # self.__mm_nas_status.qos_negotiated.max_sdu = field_val["gsm_a.gm.sm.qos.max_sdu"]
                # self.__mm_nas_status.qos_negotiated.sdu_err_rat = field_val["gsm_a.gm.sm.qos.sdu_err_rat"]
                # self.__mm_nas_status.qos_negotiated.spare_bits = field_val["gsm_a.spare_bits"]
                # self.__mm_nas_status.qos_negotiated.signalling_ind = field_val["gsm_a.gm.sm.qos.signalling_ind"]
                # self.__mm_nas_status.qos_negotiated.source_stat_desc = field_val["gsm_a.gm.sm.qos.source_stat_desc"]

                self.log_info(self.__mm_nas_status.dump())
                # profile update for esm qos
                self.profile.update("UmtsNasProfile:"+xstr(self.__mm_status.profile_id())+".pdp.qos",
                    {
                    'delay_class':xstr(self.__mm_nas_status.qos_negotiated.delay_class),
                    'reliability_class':xstr(self.__mm_nas_status.qos_negotiated.reliability_class),
                    'precedence_class':xstr(self.__mm_nas_status.qos_negotiated.precedence_class),
                    'peak_tput':xstr(self.__mm_nas_status.qos_negotiated.peak_throughput),
                    'mean_tput':xstr(self.__mm_nas_status.qos_negotiated.mean_throughput),
                    'traffic_class':xstr(self.__mm_nas_status.qos_negotiated.traffic_class),
                    'delivery_order':xstr(self.__mm_nas_status.qos_negotiated.delivery_order),
                    'traffic_handling_priority':xstr(self.__mm_nas_status.qos_negotiated.traffic_handling_priority),
                    'residual_ber':xstr(self.__mm_nas_status.qos_negotiated.residual_ber),
                    'transfer_delay':xstr(self.__mm_nas_status.qos_negotiated.transfer_delay),
                    'max_bitrate_ulink':xstr(self.__mm_nas_status.qos_negotiated.max_bitrate_ulink),
                    'max_bitrate_dlink':xstr(self.__mm_nas_status.qos_negotiated.max_bitrate_dlink),
                    'guaranteed_bitrate_ulink':xstr(self.__mm_nas_status.qos_negotiated.guaranteed_bitrate_ulink),
                    'guaranteed_bitrate_dlink':xstr(self.__mm_nas_status.qos_negotiated.guaranteed_bitrate_dlink),
                    # 'max_bitrate_ulink_ext':xstr(self.__mm_nas_status.qos_negotiated.max_bitrate_ulink_ext),
                    'max_bitrate_dlink_ext':xstr(self.__mm_nas_status.qos_negotiated.max_bitrate_dlink_ext),
                    # 'guaranteed_bitrate_ulink_ext':xstr(self.__mm_nas_status.qos_negotiated.guaranteed_bitrate_ulink_ext),
                    'guaranteed_bitrate_dlink_ext':xstr(self.__mm_nas_status.qos_negotiated.guaranteed_bitrate_dlink_ext),
                    })

            if "Mobile Identity - TMSI/P-TMSI" in field.get('show'):
                field_val = {}

                # Default value setting
                field_val["gsm_a.len"] = None
                field_val["gsm_a.unused"] = None 
                field_val["gsm_a.oddevenind"] = None
                field_val["gsm_a.ie.mobileid.type"] = None
                field_val["gsm_a.tmsi"] = None

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                self.__mm_nas_status.tmsi.len = field_val["gsm_a.len"]
                self.__mm_nas_status.tmsi.unused = field_val["gsm_a.unused"]
                self.__mm_nas_status.tmsi.oddevenind = field_val["gsm_a.oddevenind"]
                self.__mm_nas_status.tmsi.mobileid = field_val["gsm_a.ie.mobileid.type"]
                self.__mm_nas_status.tmsi.tmsi = field_val["gsm_a.tmsi"]

            if field.get('show') == "Quality Of Service - Requested QoS":
                field_val = {}

                # Default value setting
                # field_val['gsm_a.len'] = None
                # field_val["gsm_a.spare_bits"] = None
                field_val["gsm_a.gm.sm.qos.delay_cls"] = None
                field_val["gsm_a.gm.sm.qos.reliability_cls"] = None
                field_val["gsm_a.gm.sm.qos.peak_throughput"] = None
                # field_val["gsm_a.spare_bits"] = None
                field_val["gsm_a.gm.sm.qos.prec_class"] = None
                # field_val["gsm_a.spare_bits"] = None
                field_val["gsm_a.gm.sm.qos.mean_throughput"] = 31 #best-effort by default
                field_val["gsm_a.gm.sm.qos.traffic_cls"] = None
                field_val["gsm_a.gm.sm.qos.del_order"] = None
                # field_val["gsm_a.gm.sm.qos.del_of_err_sdu"] = None
                # field_val["gsm_a.gm.sm.qos.max_sdu"] = None
                field_val["gsm_a.gm.sm.qos.max_bitrate_upl"] = None
                field_val["gsm_a.gm.sm.qos.max_bitrate_downl"] = None
                field_val["gsm_a.gm.sm.qos.ber"] = None
                # field_val["gsm_a.gm.sm.qos.sdu_err_rat"] = None
                field_val["gsm_a.gm.sm.qos.trans_delay"] = None
                field_val["gsm_a.gm.sm.qos.traff_hdl_pri"] = None
                field_val["gsm_a.gm.sm.qos.guar_bitrate_upl"] = None
                field_val["gsm_a.gm.sm.qos.guar_bitrate_downl"] = None
                # field_val["gsm_a.spare_bits"] = None
                # field_val["gsm_a.gm.sm.qos.signalling_ind"] = None
                # field_val["gsm_a.gm.sm.qos.source_stat_desc"] = None
                field_val["gsm_a.gm.sm.qos.max_bitrate_downl_ext"] = None
                field_val["gsm_a.gm.sm.qos.guar_bitrate_downl_ext"] = None

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')
                    if "Maximum SDU size" in val.get('show'):
                        field_val["gsm_a.gm.sm.qos.max_sdu"] = val.get('value')

                # 10.5.6.5, TS24.008
                self.__mm_nas_status.qos_requested.delay_class = int(field_val['gsm_a.gm.sm.qos.delay_cls'])
                self.__mm_nas_status.qos_requested.reliability_class = int(field_val['gsm_a.gm.sm.qos.reliability_cls'])
                self.__mm_nas_status.qos_requested.peak_throughput = 1000 * pow(2, int(field_val["gsm_a.gm.sm.qos.peak_throughput"]) - 1)
                self.__mm_nas_status.qos_requested.precedence_class = int(field_val['gsm_a.gm.sm.qos.prec_class'])
                self.__mm_nas_status.qos_requested.mean_throughput = mean_tput[int(field_val["gsm_a.gm.sm.qos.mean_throughput"])]
                self.__mm_nas_status.qos_requested.traffic_class = int(field_val['gsm_a.gm.sm.qos.traffic_cls'])
                self.__mm_nas_status.qos_requested.delivery_order = int(field_val['gsm_a.gm.sm.qos.del_order'])
                self.__mm_nas_status.qos_requested.traffic_handling_priority = int(field_val['gsm_a.gm.sm.qos.traff_hdl_pri'])
                self.__mm_nas_status.qos_requested.residual_ber = residual_ber[int(field_val['gsm_a.gm.sm.qos.ber'])]
                self.__mm_nas_status.qos_requested.transfer_delay = trans_delay(int(field_val['gsm_a.gm.sm.qos.trans_delay']))
                self.__mm_nas_status.qos_requested.max_bitrate_ulink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_upl']))
                self.__mm_nas_status.qos_requested.max_bitrate_dlink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl']))
                self.__mm_nas_status.qos_requested.guaranteed_bitrate_ulink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_upl']))
                self.__mm_nas_status.qos_requested.guaranteed_bitrate_dlink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl']))
                self.__mm_nas_status.qos_requested.max_bitrate_dlink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl_ext']))
                self.__mm_nas_status.qos_requested.guaranteed_bitrate_dlink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl_ext']))

                self.profile.update("UmtsNasProfile:"+xstr(self.__mm_status.profile_id())+".pdp.qos",
                    {
                    'delay_class':xstr(self.__mm_nas_status.qos_requested.delay_class),
                    'reliability_class':xstr(self.__mm_nas_status.qos_requested.reliability_class),
                    'precedence_class':xstr(self.__mm_nas_status.qos_requested.precedence_class),
                    'peak_tput':xstr(self.__mm_nas_status.qos_requested.peak_throughput),
                    'mean_tput':xstr(self.__mm_nas_status.qos_requested.mean_throughput),
                    'traffic_class':xstr(self.__mm_nas_status.qos_requested.traffic_class),
                    'delivery_order':xstr(self.__mm_nas_status.qos_requested.delivery_order),
                    'traffic_handling_priority':xstr(self.__mm_nas_status.qos_requested.traffic_handling_priority),
                    'residual_ber':xstr(self.__mm_nas_status.qos_requested.residual_ber),
                    'transfer_delay':xstr(self.__mm_nas_status.qos_requested.transfer_delay),
                    'max_bitrate_ulink':xstr(self.__mm_nas_status.qos_requested.max_bitrate_ulink),
                    'max_bitrate_dlink':xstr(self.__mm_nas_status.qos_requested.max_bitrate_dlink),
                    # 'guaranteed_bitrate_ulink':xstr(self.__mm_nas_status.qos_requested.guaranteed_bitrate_ulink),
                    'guaranteed_bitrate_dlink':xstr(self.__mm_nas_status.qos_requested.guaranteed_bitrate_dlink),
                    # 'max_bitrate_ulink_ext':xstr(self.__mm_nas_status.qos_requested.max_bitrate_ulink_ext),
                    'max_bitrate_dlink_ext':xstr(self.__mm_nas_status.qos_requested.max_bitrate_dlink_ext),
                    # 'guaranteed_bitrate_ulink_ext':xstr(self.__mm_nas_status.qos_requested.guaranteed_bitrate_ulink_ext),
                    'guaranteed_bitrate_dlink_ext':xstr(self.__mm_nas_status.qos_requested.guaranteed_bitrate_dlink_ext),
                    })
            # TODO:
            # show="MS Network Capability"
            # show="Attach Type"
            # show="MS Radio Access Capability"
            # show="GPRS Timer - Ready Timer"
            # show="P-TMSI type"
            # show="Routing Area Identification - Old routing area identification - RAI: 310-260-26281-1"


class GmmStatus:
    """
    An abstraction to maintain the GMM status.
    """
    def __init__(self):
        self.state = None
        self.substate = None
        self.update_status = None

class MmStatus:
    """
    An abstraction to maintain the MM status.
    """
    def __init__(self):
        self.state = None
        self.substate = None
        self.update_status = None
        self.plmn=None
        self.lac=None
        self.rac=None
        self.operation_mode=None
        self.service_type=None

    def profile_id(self):
        """
        Return a globally unique id (MCC-MNC-MMEGI-MMEC) for profiling
        """
        if not self.plmn or not self.lac or not self.rac:
            return None
        else:
            return (str(self.plmn)
                + '-' + str(self.lac)
                + '-' + str(self.rac))

    def dump(self):
        """
        Report the MM status

        :returns: a string that encodes MM status
        """

        return (self.__class__.__name__
            + ' MM.state='+xstr(self.state) 
            + ' MM.substate='+xstr(self.substate)
            + ' MM.update_status='+xstr(self.update_status)
            + ' PLMN=' + xstr(self.plmn)
            + ' LAC=' + xstr(self.lac)
            + ' RAC=' + xstr(self.rac)
            + ' Network_operation_mode=' + xstr(self.operation_mode)
            + ' CS/PS_service_type=' + xstr(self.service_type))


class MmNasStatusDrx:
    def __init__(self):
        self.split_pg_cycle_code = None
        self.cn_spec_drx_cycle_len_coef = None
        self.split_on_ccch = None
        self.non_drx_timer = None

class MmNasQosNegotiated:
    def __init__(self):
        self.elay_class = None
        self.reliability_class = None
        self.peak_throughput = None
        self.precedence_class = None
        self.mean_throughput = None
        self.traffic_class = None
        self.delivery_order = None
        self.traffic_handling_priority = None
        self.residual_ber = None
        self.transfer_delay = None
        self.max_bitrate_ulink = None
        self.max_bitrate_dlink = None
        self.guaranteed_bitrate_ulink = None
        self.guaranteed_bitrate_dlink = None
        self.max_bitrate_dlink_ext = None
        self.guaranteed_bitrate_dlink_ext = None

    def dump_rate(self):
        """
        Report the data rate profile in ESM QoS, including the peak/mean throughput,
        maximum downlink/uplink data rate, guaranteed downlink/uplink data rate, etc.

        :returns: a string that encodes all the data rate 
        :rtype: string
        """
        return (self.__class__.__name__ 
            + ' peak_tput=' + xstr(self.peak_throughput) + ' mean_tput=' + xstr(self.mean_throughput)
            + ' max_bitrate_ulink=' + xstr(self.max_bitrate_ulink) + ' max_bitrate_dlink=' + xstr(self.max_bitrate_dlink)
            + ' guaranteed_birate_ulink=' + xstr(self.guaranteed_bitrate_ulink) + ' guaranteed_birate_dlink=' + xstr(self.guaranteed_bitrate_dlink)
            + ' max_bitrate_dlink_ext=' + xstr(self.max_bitrate_dlink_ext)
            + ' guaranteed_birate_dlink_ext=' + xstr(self.guaranteed_bitrate_dlink_ext))

    def dump_delivery(self):
        """
        Report the delivery profile in ESM QoS, including delivery order guarantee,
        traffic class, delay class, transfer delay, etc.

        :returns: a string that encodes all the data rate, or None if not ready 
        :rtype: string
        """
        if self.delivery_order:
            order = delivery_order[self.delivery_order]
        else:
            order = None
        if self.traffic_class:
            tra_class = traffic_class[self.traffic_class]
        else:
            tra_class = None
        return (self.__class__.__name__
            + ' delivery_order=' + xstr(order)
            + ' traffic_class=' + xstr(tra_class)
            + ' transfer_delay=' + xstr(self.transfer_delay) + ' residual_BER=' + xstr(self.residual_ber))

class MmNasQosRequested:
    def __init__(self):
        self.delay_class = None
        self.reliability_class = None
        self.peak_throughput = None
        self.precedence_class = None
        self.mean_throughput = None
        self.traffic_class = None
        self.delivery_order = None
        self.traffic_handling_priority = None
        self.residual_ber = None
        self.transfer_delay = None
        self.max_bitrate_ulink = None
        self.max_bitrate_dlink = None
        self.guaranteed_bitrate_ulink = None
        self.guaranteed_bitrate_dlink = None
        self.max_bitrate_dlink_ext = None
        self.guaranteed_bitrate_dlink_ext = None

    def dump_rate(self):
        """
        Report the data rate profile in ESM QoS, including the peak/mean throughput,
        maximum downlink/uplink data rate, guaranteed downlink/uplink data rate, etc.

        :returns: a string that encodes all the data rate 
        :rtype: string
        """
        return (self.__class__.__name__ 
            + ' peak_tput=' + xstr(self.peak_throughput) + ' mean_tput=' + xstr(self.mean_throughput)
            + ' max_bitrate_ulink=' + xstr(self.max_bitrate_ulink) + ' max_bitrate_dlink=' + xstr(self.max_bitrate_dlink)
            + ' guaranteed_birate_ulink=' + xstr(self.guaranteed_bitrate_ulink) + ' guaranteed_birate_dlink=' + xstr(self.guaranteed_bitrate_dlink)
            + ' max_bitrate_dlink_ext=' + xstr(self.max_bitrate_dlink_ext)
            + ' guaranteed_birate_dlink_ext=' + xstr(self.guaranteed_bitrate_dlink_ext))

    def dump_delivery(self):
        """
        Report the delivery profile in ESM QoS, including delivery order guarantee,
        traffic class, delay class, transfer delay, etc.

        :returns: a string that encodes all the data rate, or None if not ready 
        :rtype: string
        """
        if self.delivery_order:
            order = delivery_order[self.delivery_order]
        else:
            order = None
        if self.traffic_class:
            tra_class = traffic_class[self.traffic_class]
        else:
            tra_class = None
        return (self.__class__.__name__
            + ' delivery_order=' + xstr(order)
            + ' traffic_class=' + xstr(tra_class)
            + ' delay_class=' + xstr(self.delay_class)
            + ' transfer_delay=' + xstr(self.transfer_delay) + ' residual_BER=' + xstr(self.residual_ber))

class MmNasTmsi:
    def __init__(self):
        self.len = None
        self.unused = None
        self.oddevenind = None
        self.mobileid = None
        self.tmsi = None


class MmNasStatus:
    """
    An abstraction to maintain the MM NAS status.
    """
    def __init__(self):
        self.drx = MmNasStatusDrx()
        self.qos_negotiated = MmNasQosNegotiated ()
        self.qos_requested= MmNasQosRequested()
        self.tmsi = MmNasTmsi()

    def dump(self):
        return (self.__class__.__name__      
            + ":\n\t"+self.qos_negotiated.dump_rate()+'\n\t'+self.qos_negotiated.dump_delivery())



def UmtsNasProfileHierarchy():
    '''
    Return a Lte Nas ProfileHierarchy (configurations)

    :returns: ProfileHierarchy for LTE NAS
    '''

    profile_hierarchy = ProfileHierarchy('UmtsNasProfile')
    root = profile_hierarchy.get_root()
    eps = root.add('pdp',False)
    
    qos = eps.add('qos',False) #Active-state configurations (indexed by EPS type: default or dedicated)

    #QoS parameters
    qos.add('delay_class',False)
    qos.add('reliability_class',False)
    qos.add('precedence_class',False)
    qos.add('peak_tput',False)
    qos.add('mean_tput',False)
    qos.add('traffic_class',False)
    qos.add('delivery_order',False)
    qos.add('transfer_delay',False)
    qos.add('traffic_handling_priority',False)
    qos.add('max_bitrate_ulink',False)
    qos.add('max_bitrate_dlink',False)
    qos.add('guaranteed_bitrate_ulink',False)
    qos.add('guaranteed_bitrate_dlink',False)
    # qos.add('max_bitrate_ulink_ext',False)
    qos.add('max_bitrate_dlink_ext',False)
    qos.add('guaranteed_bitrate_ulink_ext',False)
    qos.add('guaranteed_bitrate_dlink_ext',False)
    qos.add('residual_ber',False)

    return profile_hierarchy