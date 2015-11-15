#!/usr/bin/python
# Filename: lte_nas_analyzer.py
"""

A LTE NAS layer (EMM/ESM) analyzer

Author: Yuanjie Li
        Zengwen Yuan
"""

try: 
    import xml.etree.cElementTree as ET 
except ImportError: 
    import xml.etree.ElementTree as ET
from analyzer import *
import timeit

from profile import Profile, ProfileHierarchy

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

def xstr(val):
    '''
    Return a string for valid value, or empty string for Nontype

    :param val: a value
    :returns: a string if val is not none, otherwise an empty string
    '''

    if val:
        return str(val)
    else:
        return "unknown"


def max_bitrate(val):
    '''
    Given ESM value, return maximum bit rate (Kbps).
    Please refer to 10.5.6.5, TS24.008 for more details.

    :param val: the value encoded in the ESM NAS message
    '''
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
    Given ESM value, return extended maximum bit rate (Kbps).
    Please refer to 10.5.6.5, TS24.008 for more details.

    :param val: the value encoded in the ESM NAS message
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
    Given ESM value, return transfer delay (ms).
    Please refer to 10.5.6.5, TS24.008 for more details.

    :param val: the value encoded in the ESM NAS message
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

    """
    A protocol analyzer for LTE NAS messages (EMM and ESM)
    """

    def __init__(self):
        Analyzer.__init__(self)
        #init packet filters
        self.add_source_callback(self.__nas_filter)
        #EMM/ESM status initialization
        self.__emm_status = EmmStatus()
        self.__esm_status = EsmStatus()

        #define LTE NAS profile hierarchy
        self.__eps_id = None
        self.__profile = Profile(LteNasProfileHierarchy())

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE NAS messages.

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)
        #Enable EMM/ESM logs
        source.enable_log("LTE_NAS_ESM_Plain_OTA_Incoming_Message")
        source.enable_log("LTE_NAS_ESM_Plain_OTA_Outgoing_Message")
        source.enable_log("LTE_NAS_EMM_Plain_OTA_Incoming_Message")
        source.enable_log("LTE_NAS_EMM_Plain_OTA_Outgoing_Message")

    def __nas_filter(self,msg):
        """
        Filter all NAS(EMM/ESM) packets, and call functions to process it

        :param msg: the event (message) from the trace collector.
        """
        # log_item = msg.data
        log_item = msg.data.decode()
        log_item_dict = dict(log_item)

        # if not log_item_dict.has_key('Msg'):
        if 'Msg' not in log_item_dict:
            return

        #Convert msg to xml format
        # log_xml = ET.fromstring(log_item_dict['Msg'])
        log_xml = ET.XML(log_item_dict['Msg'])
        xml_msg=Event(msg.timestamp,msg.type_id,log_xml)

        if msg.type_id == "LTE_NAS_ESM_Plain_OTA_Incoming_Message" \
        or msg.type_id == "LTE_NAS_ESM_Plain_OTA_Outgoing_Message" \
        or msg.type_id == "LTE_NAS_EMM_Plain_OTA_Incoming_Message" \
        or msg.type_id == "LTE_NAS_EMM_Plain_OTA_Outgoing_Message":    
            self.__callback_emm_state(xml_msg)
            self.__callback_emm(xml_msg)
            self.__callback_esm(xml_msg)


        e = Event(timeit.default_timer(),self.__class__.__name__,"")
        self.send(e)

    def __callback_emm_state(self,msg):
        """
        Given the EMM message, update EMM state and substate.

        :param msg: the NAS signaling message that carries EMM state
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

        self.logger.info(self.__emm_status.dump())
        # self.__emm_status.dump()

    def __callback_emm(self,msg):
        """
        Extrace EMM status and configurations from the NAS messages

        :param msg: the EMM NAS message
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
        Extrace EMM status and configurations from the NAS messages
                 ^
        Zengwen: is it ESM ??

        :param msg: the ESM NAS message
        """

        self.__eps_id = xstr(self.__emm_status.guti.mme_group_id)
        # self.__eps_id = xstr(self.__emm_status.guti.mme_group_id)+xstr(self.__emm_status.mme_code)

        for field in msg.data.iter('field'):

            if field.get('name')=="nas_eps.emm.qci":
                self.__esm_status.qos.qci=int(field.get('show'))
                self.logger.info(self.__esm_status.qos.dump_rate())
                self.logger.info(self.__esm_status.qos.dump_delivery())

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

                self.logger.info(self.__esm_status.qos.dump_rate())
                self.logger.info(self.__esm_status.qos.dump_delivery())
                # self.__esm_status.qos.dump_rate()
                # self.__esm_status.qos.dump_delay()

                # profile update for esm qos
                self.__profile.update("LteNasProfile:"+str(self.__eps_id),
                    {
                    'delay_class':xstr(self.__esm_status.qos.delay_class),
                    'reliability_class':xstr(self.__esm_status.qos.reliability_class),
                    'precedence_class':xstr(self.__esm_status.qos.precedence_class),
                    'peak_tput':xstr(self.__esm_status.qos.peak_tput),
                    'mean_tput':xstr(self.__esm_status.qos.mean_tput),
                    'traffic_class':xstr(self.__esm_status.qos.traffic_class),
                    'delivery_order':xstr(self.__esm_status.qos.delivery_order),
                    'traffic_handling_priority':xstr(self.__esm_status.qos.traffic_handling_priority),
                    'residual_ber':xstr(self.__esm_status.qos.residual_ber),
                    'transfer_delay':xstr(self.__esm_status.qos.transfer_delay),
                    'max_bitrate_ulink':xstr(self.__esm_status.qos.max_bitrate_ulink),
                    'max_bitrate_dlink':xstr(self.__esm_status.qos.max_bitrate_dlink),
                    'guaranteed_bitrate_ulink':xstr(self.__esm_status.qos.guaranteed_bitrate_ulink),
                    'guaranteed_bitrate_dlink':xstr(self.__esm_status.qos.guaranteed_bitrate_dlink),
                    'max_bitrate_ulink_ext':xstr(self.__esm_status.qos.max_bitrate_ulink_ext),
                    'max_bitrate_dlink_ext':xstr(self.__esm_status.qos.max_bitrate_dlink_ext),
                    'guaranteed_bitrate_ulink_ext':xstr(self.__esm_status.qos.guaranteed_bitrate_ulink_ext),
                    'guaranteed_bitrate_dlink_ext':xstr(self.__esm_status.qos.guaranteed_bitrate_dlink_ext),
                    })

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

    def inited(self):
        return (self.state and self.substate and self.guti.inited())

    def dump(self):
        """
        Report the EMM status

        :returns: a string that encodes EMM status
        """
        # print self.__class__.__name__,self.state,self.substate, \
        # self.guti.mcc,self.guti.mnc,self.guti.mme_group_id, \
        # self.guti.mme_code,self.guti.m_tmsi,self.ciphering,self.integrity

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
        self.qos=EsmQos()

        # # test profile
        # self.id=None

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
        # print self.__class__.__name__,"Throughput(Kbps):",self.peak_tput,self.mean_tput, \
        # self.max_bitrate_ulink, self.max_bitrate_dlink, \
        # self.guaranteed_bitrate_ulink, self.guaranteed_bitrate_dlink, \
        # self.max_bitrate_ulink_ext, self.max_bitrate_dlink_ext, \
        # self.guaranteed_bitrate_ulink_ext, self.guaranteed_bitrate_dlink_ext
        return (self.__class__.__name__ 
            + ' peak_tput=' + xstr(self.peak_tput) + ' mean_tput=' + xstr(self.mean_tput)
            + ' max_bitrate_ulink=' + xstr(self.max_bitrate_ulink) + ' max_bitrate_dlink=' + xstr(self.max_bitrate_dlink)
            + ' guaranteed_birate_ulink=' + xstr(self.guaranteed_bitrate_ulink) + ' guaranteed_birate_dlink=' + xstr(self.guaranteed_bitrate_dlink)
            + ' max_bitrate_ulink_ext=' + xstr(self.max_bitrate_ulink_ext) + ' max_bitrate_dlink_ext=' + xstr(self.max_bitrate_dlink_ext)
            + ' guaranteed_birate_ulink_ext=' + xstr(self.guaranteed_bitrate_ulink_ext) + ' guaranteed_birate_dlink_ext=' + xstr(self.guaranteed_bitrate_dlink_ext))

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

def LteNasProfileHierarchy():
    '''
    Return a Lte Nas ProfileHierarchy (configurations)

    :returns: ProfileHierarchy for LTE NAS
    '''

    profile_hierarchy = ProfileHierarchy('LteNasProfile')
    root = profile_hierarchy.get_root()
    eps = root.add('eps',False)
    
    eps.add('eps_id', False);
    qos = eps.add('qos',False) #Active-state configurations

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
