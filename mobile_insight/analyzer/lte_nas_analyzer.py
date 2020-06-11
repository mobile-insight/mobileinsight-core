#!/usr/bin/python
# Filename: lte_nas_analyzer.py
"""

A LTE NAS layer (EMM/ESM) analyzer

Author: Yuanjie Li
        Zengwen Yuan
"""

import xml.etree.ElementTree as ET
from .analyzer import *
from .state_machine import *
import timeit

from .protocol_analyzer import *
from .profile import Profile, ProfileHierarchy

from .nas_util import *

__all__=["LteNasAnalyzer"]

#EMM registeration state 
emm_state={0:"deregistered",1:"registered"}

#EMM registeration substate 
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

#ESM session connection state
esm_state={0:"disconnected", 1:"connected", -1:"disconnected"}

# class LteNasAnalyzer(Analyzer):
class LteNasAnalyzer(ProtocolAnalyzer):

    """
    A protocol analyzer for LTE NAS messages (EMM and ESM)
    """

    def __init__(self):

        ProtocolAnalyzer.__init__(self)
        #init packet filters
        self.add_source_callback(self.__nas_filter)
        #EMM/ESM status initialization
        self.__emm_status = EmmStatus()
        self.__esm_status = {} #EPS ID -> EsmStatus()
        #####use EPS bearer ID or EPS bearer state????
        self.__cur_eps_id = None
        # self.__esm_status = EsmStatus()
        self.esm_state_machine = self.create_esm_state_machine()
        self.emm_state_machine = self.create_emm_state_machine()
        self.callflow_state_machine = self.create_callflow_state_machine()

    def create_profile_hierarchy(self):
        '''
        Return a Lte NAS ProfileHierarchy (configurations)

        :returns: ProfileHierarchy for LTE NAS
        '''
        return LteNasProfileHierarchy()

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE NAS messages.

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)
        #Enable EMM/ESM logs
        source.enable_log("LTE_NAS_ESM_OTA_Incoming_Packet")
        source.enable_log("LTE_NAS_ESM_OTA_Outgoing_Packet")
        source.enable_log("LTE_NAS_EMM_OTA_Incoming_Packet")
        source.enable_log("LTE_NAS_EMM_OTA_Outgoing_Packet")
        source.enable_log("LTE_NAS_EMM_State")
        source.enable_log("LTE_NAS_ESM_State")

    def create_callflow_state_machine(self):
        """
        Declare a callflow state machine

        returns: a StateMachine
        """

        def idle_to_csfb(msg):
            if msg.type_id == 'LTE_NAS_EMM_State':
                return
            extended_service_req_flag = False
            for field in msg.data.iter('field'):
                if field.get('name') == 'nas_eps.nas_msg_emm_type' and field.get('value') == '4c':
                    extended_service_req_flag = True
                elif extended_service_req_flag and field.get('name') == 'nas_eps.emm.service_type' \
                        and (field.get('show') == '0' or field.get('show') == '1'):
                    return True


        def idle_to_volte(msg):
            if msg.type_id == 'LTE_NAS_EMM_State':
                return
            act_bearer_flag = False
            for proto in msg.data.iter('proto'):
                if proto.get('name') == 'nas-eps':
                    for field in proto.iter('field'):
                        # print ET.dump(field)
                        if field.get('name') == 'nas_eps.nas_msg_esm_type' and field.get('value') == 'c5':
                            act_bearer_flag = True
                        elif act_bearer_flag and field.get('show') == 'EPS quality of service':
                            # print ET.dump(field)
                            for val in field.iter('field'):
                                if val.get('name') == 'nas_eps.emm.qci' and val.get('show') == '1':
                                    # print ET.dump(val)
                                    return True


        def con_to_volte(msg):
            if msg.type_id == 'LTE_NAS_EMM_State':
                return
            deact_bearer_flag = False
            for proto in msg.data.iter('proto'):
                if proto.get('name') == 'nas-eps':
                    for field in proto.iter('field'):
                        if field.get('value') == 'cd':  # Deactivate EPS bearer context request (0xcd)
                            deact_bearer_flag = True
                        elif deact_bearer_flag and field.get('show') == 'ESM cause':
                            for val in field.iter('field'):
                                if val.get('name') == 'nas_eps.esm.cause' and val.get('show') == '36':
                                    return True


        def csfb_to_idle(msg):
            if msg.type_id == 'LTE_NAS_EMM_State' and msg.data["EMM Substate"] == 'EMM_REGISTERED_NORMAL_SERVICE':
                return True

        def volte_to_con(msg):
            if msg.type_id == 'LTE_NAS_EMM_State':
                return
            act_bearer_flag = False
            for proto in msg.data.iter('proto'):
                if proto.get('name') == 'nas-eps':
                    # print ET.dump(proto)
                    for field in proto.iter('field'):
                        if field.get('value') == 'c6': # Activate dedicated EPS bearer context accept (0xc6)
                            act_bearer_flag = True
                            # TODO: add check on bearer id
                            return True

        def volte_to_idle(msg):
            if msg.type_id == 'LTE_NAS_EMM_State':
                return
            deact_bearer_flag = False
            for proto in msg.data.iter('proto'):
                if proto.get('name') == 'nas-eps':
                    for field in proto.iter('field'):
                        if field.get('value') == 'ce':  # Deactivate EPS bearer context accept (0xce)
                            deact_bearer_flag = True
                            # TODO: add check on bearer id
                            return True

        def init_state(msg):
            return 'IDLE'

        state_machine={'IDLE': {'CALL_FALLBACK': idle_to_csfb, 'VoLTE_PROCESSING': idle_to_volte},
                       'CONNECTED': {'VoLTE_PROCESSING': con_to_volte},
                       'CALL_FALLBACK': {'IDLE': csfb_to_idle},
                       'VoLTE_PROCESSING': {'CONNECTED': volte_to_con, 'IDLE': volte_to_idle}}

        return StateMachine(state_machine, init_state)


    def create_esm_state_machine(self):
        """
        Declare a ESM state machine
        returns: a StateMachine
        """

        def con_to_discon(msg):
            if int(msg.data["EPS bearer state"]) - 1 == 0:
                return True

        def discon_to_con(msg):
            if int(msg.data["EPS bearer state"]) - 1 == 1:
                return True

        def init_state(msg):
            if "EPS bearer state" not in msg.data:
                return "ESM_DISCON"
            elif int(msg.data["EPS bearer state"]) - 1 == 0:
                return 'ESM_DISCON'
            elif int(msg.data["EPS bearer state"]) - 1 == 1:
                return 'ESM_CON'

        state_machine = {'ESM_CON': {'ESM_DISCON': con_to_discon},
                         'ESM_DISCON': {'ESM_CON': discon_to_con}}

        return StateMachine(state_machine, init_state)


    def create_emm_state_machine(self):
        """
        Declare a EMM state machine

        returns: a StateMachine
        """

        def to_deregister(msg):
            if msg.data["EMM State"] == 'EMM_DEREGISTERED':
                return True

        def to_deregister_init(msg):
            if msg.data["EMM State"] == 'EMM_DEREGISTERED_INITIATED':
                return True

        def to_register(msg):
            if msg.data["EMM State"] == 'EMM_REGISTERED':
                return True

        def to_register_init(msg):
            if msg.data["EMM State"] == 'EMM_REGISTERED_INITIATED':
                return True

        def init_state(msg):
            if msg.data["EMM State"] in ['EMM_REGISTERED', 'EMM_REGISTERED_INITIATED', 'EMM_DEREGISTERED',
                       'EMM_DEREGISTERED_INITIATED']:
                return msg.data["EMM State"]

        state_machine={'EMM_REGISTERED': {'EMM_DEREGISTERED': to_deregister, 'EMM_DEREGISTERED_INITIATED': to_deregister_init},
                       'EMM_REGISTERED_INITIATED': {'EMM_REGISTERED': to_register, 'EMM_DEREGISTERED': to_deregister},
                       'EMM_DEREGISTERED': {'EMM_REGISTERED_INITIATED': to_register_init},
                       'EMM_DEREGISTERED_INITIATED': {'EMM_DEREGISTERED': to_deregister}}

        return StateMachine(state_machine, init_state)

    def __nas_filter(self,msg):
        """
        Filter all NAS(EMM/ESM) packets, and call functions to process it

        :param msg: the event (message) from the trace collector.
        """

        if msg.type_id == "LTE_NAS_ESM_OTA_Incoming_Packet" \
        or msg.type_id == "LTE_NAS_ESM_OTA_Outgoing_Packet" \
        or msg.type_id == "LTE_NAS_EMM_OTA_Incoming_Packet" \
        or msg.type_id == "LTE_NAS_EMM_OTA_Outgoing_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)

            # if not log_item_dict.has_key('Msg'):
            if 'Msg' not in log_item_dict:
                return

            #Convert msg to xml format
            # log_xml = ET.fromstring(log_item_dict['Msg'])
            log_xml = ET.XML(log_item_dict['Msg'])
            xml_msg=Event(msg.timestamp,msg.type_id,log_xml)

            # print log_item_dict['Msg']

            # self.__callback_emm_state(xml_msg)
            self.__callback_emm(xml_msg)
            self.__callback_esm(xml_msg)

            self.send(msg)

        if msg.type_id == "LTE_NAS_EMM_State":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)

            raw_msg = Event(msg.timestamp,msg.type_id,log_item_dict)
            self.__callback_emm_state(raw_msg)
            if self.emm_state_machine.update_state(raw_msg):
                self.log_info("EMM state: " + str(self.emm_state_machine.get_current_state()))

            self.send(msg)

        if msg.type_id == "LTE_NAS_ESM_State":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            raw_msg = Event(msg.timestamp,msg.type_id,log_item_dict)
            self.__callback_esm_state(raw_msg)
            if self.esm_state_machine.update_state(raw_msg):
                self.log_info("ESM state: " + self.esm_state_machine.get_current_state())

            self.send(msg)

    def __callback_emm_state(self,msg):
        """
        Given the EMM message, update EMM state and substate.

        :param msg: the NAS signaling message that carries EMM state
        """
        self.__emm_status.state = msg.data["EMM State"]
        self.__emm_status.substate = msg.data["EMM Substate"]
        tmp = msg.data["PLMN"].split('-')
        self.__emm_status.guti.mcc = tmp[0]
        self.__emm_status.guti.mnc = tmp[1]
        self.__emm_status.guti.mme_group_id = msg.data["GUTI MME Group ID"]
        self.__emm_status.guti.mme_code = msg.data["GUTI MME Code"]
        self.__emm_status.guti.m_tmsi = msg.data["GUTI M-TMSI"]
        self.log_info(self.__emm_status.dump())

        #broadcast
        state = {
            'conn state': self.__emm_status.state,
            'conn substate': self.__emm_status.substate,
        }

        # self.log_info('EMM_STATE', str(state))
        self.broadcast_info('EMM_STATE', state)

        if self.callflow_state_machine.update_state(msg):
            self.log_info("Call flow status: " + str(self.callflow_state_machine.get_current_state()))



    def __callback_esm_state(self,msg):
        """
        Given the ESM message, update ESM state

        :param msg: the NAS signaling message that carries EMM state
        """
        self.__cur_eps_id = msg.data["EPS bearer ID"]

        if self.__cur_eps_id not in self.__esm_status:
            self.__esm_status[self.__cur_eps_id] = EsmStatus()

        # print self.__cur_eps_id, str(self.__esm_status), str(bearer_type), msg.data["EPS bearer type"], str(msg.data['timestamp'])

        self.__esm_status[self.__cur_eps_id].eps_id = int(msg.data["EPS bearer ID"])
        self.__esm_status[self.__cur_eps_id].type = int(msg.data["EPS bearer type"])
        if self.__esm_status[self.__cur_eps_id].type == 255:
            self.__esm_status[self.__cur_eps_id].type = 1
        self.__esm_status[self.__cur_eps_id].qos.qci = msg.data["QCI"]
        self.__esm_status[self.__cur_eps_id].qos.max_bitrate_ulink = msg.data["UL MBR"]
        self.__esm_status[self.__cur_eps_id].qos.max_bitrate_dlink = msg.data["DL MBR"]
        self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_ulink=msg.data["UL GBR"]
        self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_dlink=msg.data["DL MBR"]
        self.__esm_status[self.__cur_eps_id].qos.max_bitrate_ulink_ext=msg.data["UL MBR ext"]
        self.__esm_status[self.__cur_eps_id].qos.max_bitrate_dlink_ext=msg.data["DL MBR ext"]
        self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_ulink_ext=msg.data["UL GBR ext"]
        self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_dlink_ext=msg.data["DL MBR ext"]

        self.__esm_status[self.__cur_eps_id].timestamp = msg.data["timestamp"]

        self.log_info(self.__esm_status[self.__cur_eps_id].dump())

        self.profile.update("LteNasProfile:"+self.__emm_status.profile_id()+".eps.qos:"+bearer_type[self.__esm_status[self.__cur_eps_id].type],
                    {'qci':self.__esm_status[self.__cur_eps_id].qos.qci,
                     'max_bitrate_ulink':self.__esm_status[self.__cur_eps_id].qos.max_bitrate_ulink,
                     'max_bitrate_dlink':self.__esm_status[self.__cur_eps_id].qos.max_bitrate_dlink,
                     'guaranteed_bitrate_ulink':self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_ulink,
                     'guaranteed_bitrate_dlink':self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_dlink,
                     'max_bitrate_ulink_ext':self.__esm_status[self.__cur_eps_id].qos.max_bitrate_ulink_ext,
                     'max_bitrate_dlink_ext':self.__esm_status[self.__cur_eps_id].qos.max_bitrate_dlink_ext,
                     'guaranteed_bitrate_ulink_ext':self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_ulink_ext,
                     'guaranteed_bitrate_dlink_ext':self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_dlink_ext,
                     })

        # broadcast
        state = {
            'conn state': esm_state[int(msg.data["EPS bearer state"]) - 1],
            'timestamp' : str(msg.data['timestamp'])
        }
        # self.log_info(str(state))
        self.broadcast_info('ESM_STATE', state)

    def __callback_emm(self,msg):
        """
        Extract EMM status and configurations from the NAS messages

        :param msg: the EMM NAS message
        """

        for field in msg.data.iter('field'):

            if field.get('show') == "UE network capability":
                # print str(ET.dump(field))
                for val in field.iter('field'):
                    if val.get('name') == 'nas_eps.emm.acc_csfb_cap':
                        csfb_cap = True if val.get('show') == '1' else False
                        self.log_info("CSFB Capbility: " + str(csfb_cap))
                        bcast_dict = {}
                        bcast_dict['cafb cap'] = str(csfb_cap)
                        bcast_dict['timestamp'] = str(msg.timestamp)
                        self.broadcast_info('CSFB_CAP', bcast_dict)


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
        Extract ESM status and configurations from the NAS messages

        :param msg: the ESM NAS message
        """

        if self.callflow_state_machine.update_state(msg):
            self.log_info("Call flow status: " + str(self.callflow_state_machine.get_current_state()))

        for field in msg.data.iter('field'):

            if field.get('name')=="nas_eps.bearer_id":
                self.__cur_eps_id = int(field.get('show'))
                if self.__cur_eps_id not in self.__esm_status:
                    self.__esm_status[self.__cur_eps_id]=EsmStatus()

            if field.get('name')=="nas_eps.emm.qci":
                self.__esm_status[self.__cur_eps_id].qos.qci=int(field.get('show'))

            if field.get('show')=="Quality Of Service - Negotiated QoS" \
            or field.get('show')=="Quality Of Service - New QoS" \
            or field.get('show')=="Quality Of Service - Requested QoS":

                field_val={}

                for val in field.iter('field'):
                    field_val[val.get('name')]=val.get('show')

                self.__esm_status[self.__cur_eps_id].eps_id = int(self.__cur_eps_id)
                if 'gsm_a.gm.sm.qos.delay_cls' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.delay_class=int(field_val['gsm_a.gm.sm.qos.delay_cls'])

                if 'gsm_a.gm.sm.qos.reliability_cls' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.reliability_class=int(field_val['gsm_a.gm.sm.qos.reliability_cls'])

                if 'gsm_a.gm.sm.qos.prec_class' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.precedence_class=int(field_val['gsm_a.gm.sm.qos.prec_class'])

                if 'gsm_a.gm.sm.qos.peak_throughput' in field_val:
                    #10.5.6.5, TS24.008
                    self.__esm_status[self.__cur_eps_id].qos.peak_tput=1000*pow(2,int(field_val['gsm_a.gm.sm.qos.peak_throughput'])-1)

                if 'gsm_a.gm.sm.qos.mean_throughput' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.mean_tput=mean_tput[int(field_val['gsm_a.gm.sm.qos.mean_throughput'])]

                if 'gsm_a.gm.sm.qos.traffic_cls' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.traffic_class=int(field_val['gsm_a.gm.sm.qos.traffic_cls'])

                if 'gsm_a.gm.sm.qos.del_order' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.delivery_order=int(field_val['gsm_a.gm.sm.qos.del_order'])

                if 'gsm_a.gm.sm.qos.traff_hdl_pri' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.traffic_handling_priority=int(field_val['gsm_a.gm.sm.qos.traff_hdl_pri'])

                if 'gsm_a.gm.sm.qos.ber' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.residual_ber=residual_ber[int(field_val['gsm_a.gm.sm.qos.ber'])]

                if 'gsm_a.gm.sm.qos.trans_delay' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.transfer_delay=trans_delay(int(field_val['gsm_a.gm.sm.qos.trans_delay']))

                if 'gsm_a.gm.sm.qos.max_bitrate_upl' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.max_bitrate_ulink=max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_upl']))

                if 'gsm_a.gm.sm.qos.max_bitrate_downl' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.max_bitrate_dlink=max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl']))

                if 'gsm_a.gm.sm.qos.guar_bitrate_upl' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_ulink=max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_upl']))

                if 'gsm_a.gm.sm.qos.guar_bitrate_downl' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_dlink=max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl']))

                if 'gsm_a.gm.sm.qos.max_bitrate_upl_ext' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.max_bitrate_ulink_ext=max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.max_bitrate_upl_ext']))

                if 'gsm_a.gm.sm.qos.max_bitrate_downl_ext' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.max_bitrate_dlink_ext=max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl_ext']))
                if 'gsm_a.gm.sm.qos.guar_bitrate_upl_ext' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_ulink_ext=max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_upl_ext']))
                if 'gsm_a.gm.sm.qos.guar_bitrate_downl_ext' in field_val:
                    self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_dlink_ext=max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl_ext']))

                self.log_info("EPS_Id="+str(self.__cur_eps_id)+self.__esm_status[self.__cur_eps_id].dump())

                # profile update for esm qos
                self.profile.update("LteNasProfile:"+xstr(self.__emm_status.profile_id())+".eps.qos:"+bearer_type[self.__esm_status[self.__cur_eps_id].type],
                    {
                    'delay_class':xstr(self.__esm_status[self.__cur_eps_id].qos.delay_class),
                    'reliability_class':xstr(self.__esm_status[self.__cur_eps_id].qos.reliability_class),
                    'precedence_class':xstr(self.__esm_status[self.__cur_eps_id].qos.precedence_class),
                    'peak_tput':xstr(self.__esm_status[self.__cur_eps_id].qos.peak_tput),
                    'mean_tput':xstr(self.__esm_status[self.__cur_eps_id].qos.mean_tput),
                    'traffic_class':xstr(self.__esm_status[self.__cur_eps_id].qos.traffic_class),
                    'delivery_order':xstr(self.__esm_status[self.__cur_eps_id].qos.delivery_order),
                    'traffic_handling_priority':xstr(self.__esm_status[self.__cur_eps_id].qos.traffic_handling_priority),
                    'residual_ber':xstr(self.__esm_status[self.__cur_eps_id].qos.residual_ber),
                    'transfer_delay':xstr(self.__esm_status[self.__cur_eps_id].qos.transfer_delay),
                    'max_bitrate_ulink':xstr(self.__esm_status[self.__cur_eps_id].qos.max_bitrate_ulink),
                    'max_bitrate_dlink':xstr(self.__esm_status[self.__cur_eps_id].qos.max_bitrate_dlink),
                    'guaranteed_bitrate_ulink':xstr(self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_ulink),
                    'guaranteed_bitrate_dlink':xstr(self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_dlink),
                    'max_bitrate_ulink_ext':xstr(self.__esm_status[self.__cur_eps_id].qos.max_bitrate_ulink_ext),
                    'max_bitrate_dlink_ext':xstr(self.__esm_status[self.__cur_eps_id].qos.max_bitrate_dlink_ext),
                    'guaranteed_bitrate_ulink_ext':xstr(self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_ulink_ext),
                    'guaranteed_bitrate_dlink_ext':xstr(self.__esm_status[self.__cur_eps_id].qos.guaranteed_bitrate_dlink_ext),
                    })
        if self.__cur_eps_id in self.__esm_status:
            self.log_info(str(self.__esm_status[self.__cur_eps_id].qos.dump_rate()))
            self.log_info(str(self.__esm_status[self.__cur_eps_id].qos.dump_delivery()))
            self.broadcast_info('QOS_DELIVERY', self.__esm_status[self.__cur_eps_id].qos.dump_delivery_dict())
            self.broadcast_info('QOS_RATE', self.__esm_status[self.__cur_eps_id].qos.dump_rate_dict())


    def getTimeInterval(self, preTime, curTime):
        # preTime_parse = dt.strptime(preTime, '%Y-%m-%d %H:%M:%S.%f')
        # curTime_parse = dt.strptime(curTime, '%Y-%m-%d %H:%M:%S.%f')
        return (curTime - preTime).total_seconds() * 1000000.0

    def get_qos(self):
        # return self.__esm_status.qos
        if self.__cur_eps_id in self.__esm_status:
            return self.__esm_status[self.__cur_eps_id].qos
        else:
            #Check if QoS profile exists in data base
            return None

    def get_profiled_qos(self,plmn):
        """
        Get QoS from the profile (if any)
        """
        if plmn:
            tmp = self.profile.query("LteNasProfile:"+xstr(plmn)+".eps.qos:default")
            #     tmp = self.profile.query("LteNasProfile:"+xstr(self.__emm_status.profile_id())+".eps.qos:"+bearer_type[self.__esm_status[self.__cur_eps_id].type])
            if not tmp:
                return None
            f_qos_int = lambda x: int(x) if x and x!="unknown" else None
            f_qos_float = lambda x: float(x) if x and x!="unknown" else None
            res = EsmQos()
            res.qci=f_qos_int(tmp['qci'])
            res.delay_class=f_qos_int(tmp['delay_class'])
            res.reliability_class=f_qos_int(tmp['reliability_class'])
            res.precedence_class=f_qos_int(tmp['precedence_class'])
            res.peak_tput=f_qos_int(tmp['peak_tput'])
            res.mean_tput=tmp['mean_tput']
            res.traffic_class=f_qos_int(tmp['traffic_class'])
            res.delivery_order=f_qos_int(tmp['delivery_order'])
            res.transfer_delay=f_qos_int(tmp['transfer_delay'])
            res.traffic_handling_priority=f_qos_int(tmp['traffic_handling_priority'])
            res.max_bitrate_ulink=f_qos_int(tmp['max_bitrate_ulink'])
            res.max_bitrate_dlink=f_qos_int(tmp['max_bitrate_dlink'])
            res.guaranteed_bitrate_ulink=f_qos_int(tmp['guaranteed_bitrate_ulink'])
            res.guaranteed_bitrate_dlink=f_qos_int(tmp['guaranteed_bitrate_dlink'])
            res.max_bitrate_ulink_ext=f_qos_int(tmp['max_bitrate_ulink_ext'])
            res.max_bitrate_dlink_ext=f_qos_int(tmp['max_bitrate_dlink_ext'])
            res.guaranteed_bitrate_ulink_ext=f_qos_int(tmp['guaranteed_bitrate_ulink_ext'])
            res.guaranteed_bitrate_dlink_ext=f_qos_int(tmp['guaranteed_bitrate_dlink_ext'])
            res.residual_ber=f_qos_float(tmp['residual_ber'])
            return res
        else:
            return None

        # if self.__cur_eps_id:
        #     tmp = self.profile.query("LteNasProfile:"+xstr(self.__emm_status.profile_id())+".eps.qos:"+bearer_type[self.__esm_status[self.__cur_eps_id].type])
        #     print tmp
        # else:
        #     return None

class EmmStatus:
    """
    An abstraction to maintain the EMM status, including the registeration states,
    temporary IDs (GUTI), security options, etc.
    """
    def __init__(self):
        self.state = "null"
        self.substate = "null"
        self.guti = Guti()
        self.ciphering = None
        self.integrity = None
        self.timestamp = None

    def inited(self):
        return (self.state and self.substate and self.guti.inited())

    def profile_id(self):
        """
        Return a globally unique id (MCC-MNC-MMEGI-MMEC) for profiling
        """
        if not self.guti.inited():
            return None
        else:
            return (str(self.guti.mcc)
                + '-' + str(self.guti.mnc)
                )

            # return (str(self.guti.mcc)
            #     + '-' + str(self.guti.mnc)
            #     + '-' + str(int(self.guti.mme_group_id,0))
            #     + '-' + str(int(self.guti.mme_code,0)))

    def dump(self):
        """
        Report the EMM status

        :returns: a string that encodes EMM status
        """

        return (self.__class__.__name__
            + ' EMM.state='+xstr(self.state) + ' EMM.substate='+xstr(self.substate)
            + ' MCC=' + xstr(self.guti.mcc) + ' MNC=' + xstr(self.guti.mnc)
            + ' MMEGI=' + xstr(self.guti.mme_group_id) + ' MMEC=' + xstr(self.guti.mme_code)
            + ' TMSI=' + xstr(self.guti.m_tmsi))


class Guti:
    """
    An abstraction to maintain Globally Unique Temporary ID (GUTI)
    """
    def __init__(self):
        self.mcc=None
        self.mnc=None
        self.mme_group_id=None
        self.mme_code=None
        self.m_tmsi=None

    def inited(self):
        """
        Return true if all GUTI fileds are initialized
        """
        return (self.mcc and self.mnc and self.mme_group_id \
            and self.mme_code and self.m_tmsi)


class EsmStatus:
    """
    An abstraction to maintain the ESM status
    """
    def __init__(self):
        self.eps_id = None
        self.type = 0    #default or dedicated
        self.qos=EsmQos()
        self.timestamp = None

    def dump(self):
        # TODO: deal with potential KeyError here
        dump_text = ' EPS_ID=' + xstr(self.eps_id) + ' type=' + xstr(bearer_type[self.type]) \
            + ":\n\t"+self.qos.dump_rate()+'\n\t'+self.qos.dump_delivery()
        return dump_text

class EsmQos:
    """
    An abstraction for ESM QoS profiles
    """
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
        """
        Report the data rate profile in ESM QoS, including the peak/mean throughput,
        maximum downlink/uplink data rate, guaranteed downlink/uplink data rate, etc.

        :returns: a string that encodes all the data rate
        :rtype: string
        """
        return (self.__class__.__name__
            + ' peak_tput=' + xstr(self.peak_tput) + ' mean_tput=' + xstr(self.mean_tput)
            + ' max_bitrate_ulink=' + xstr(self.max_bitrate_ulink) + ' max_bitrate_dlink=' + xstr(self.max_bitrate_dlink)
            + ' guaranteed_birate_ulink=' + xstr(self.guaranteed_bitrate_ulink) + ' guaranteed_birate_dlink=' + xstr(self.guaranteed_bitrate_dlink)
            + ' max_bitrate_ulink_ext=' + xstr(self.max_bitrate_ulink_ext) + ' max_bitrate_dlink_ext=' + xstr(self.max_bitrate_dlink_ext)
            + ' guaranteed_birate_ulink_ext=' + xstr(self.guaranteed_bitrate_ulink_ext) + ' guaranteed_birate_dlink_ext=' + xstr(self.guaranteed_bitrate_dlink_ext))

    def dump_rate_dict(self):
        """
        Report the data rate profile in ESM QoS, including the peak/mean throughput,
        maximum downlink/uplink data rate, guaranteed downlink/uplink data rate, etc.

        :returns: a dict that encodes all the data rate
        :rtype: dict
        """
        res = {}
        res['peak_tput'] = xstr(self.peak_tput)
        res['mean_tput'] = xstr(self.mean_tput)
        res['max_bitrate_ulink'] = xstr(self.peak_tput)
        res['max_bitrate_dlink'] = xstr(self.max_bitrate_dlink)
        res['guaranteed_birate_ulink'] = xstr(self.guaranteed_bitrate_ulink)
        res['guaranteed_birate_dlink'] = xstr(self.guaranteed_bitrate_dlink)
        res['max_bitrate_ulink_ext'] = xstr(self.max_bitrate_ulink_ext)
        res['max_bitrate_dlink_ext'] = xstr(self.max_bitrate_dlink_ext)
        res['guaranteed_birate_ulink_ext'] = xstr(self.guaranteed_bitrate_ulink_ext)
        res['guaranteed_birate_dlink_ext'] = xstr(self.guaranteed_bitrate_dlink_ext)
        return res

    def dump_delivery(self):
        """
        Report the delivery profile in ESM QoS, including delivery order guarantee,
        traffic class, QCI, delay class, transfer delay, etc.

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
            + ' QCI=' + xstr(self.qci) + ' delay_class=' + xstr(self.delay_class)
            + ' transfer_delay=' + xstr(self.transfer_delay) + ' residual_BER=' + xstr(self.residual_ber))

    def dump_delivery_dict(self):
        """
        Report the delivery profile in ESM QoS, including delivery order guarantee,
        traffic class, QCI, delay class, transfer delay, etc.

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
        res = {}
        res['delivery_order'] = xstr(order)
        res['traffic_class'] = xstr(tra_class)
        res['QCI'] = xstr(self.qci)
        res['delay_class'] = xstr(self.delay_class)
        res['transfer_delay'] = xstr(self.transfer_delay)
        res['residual_BER'] = xstr(self.residual_ber)
        return res

def LteNasProfileHierarchy():
    '''
    Return a Lte Nas ProfileHierarchy (configurations)

    :returns: ProfileHierarchy for LTE NAS
    '''

    profile_hierarchy = ProfileHierarchy('LteNasProfile')
    root = profile_hierarchy.get_root()
    eps = root.add('eps',False)

    qos = eps.add('qos',True) #Active-state configurations (indexed by EPS type: default or dedicated)

    #QoS parameters
    qos.add('qci',False)
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
    qos.add('max_bitrate_ulink_ext',False)
    qos.add('max_bitrate_dlink_ext',False)
    qos.add('guaranteed_bitrate_ulink_ext',False)
    qos.add('guaranteed_bitrate_dlink_ext',False)
    qos.add('residual_ber',False)

    return profile_hierarchy