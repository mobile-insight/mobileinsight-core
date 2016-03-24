#!/usr/bin/python
# Filename: umts_nas_analyzer.py
"""

A UMTS NAS layer (MM/GMM/CM/SM) analyzer

Author: Yuanjie Li
Author: Zengwen Yuan
"""

try: 
    import xml.etree.cElementTree as ET 
except ImportError: 
    import xml.etree.ElementTree as ET
from analyzer import *
import timeit

from profile import Profile, ProfileHierarchy

__all__=["UmtsNasAnalyzer"]


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

class UmtsNasAnalyzer(Analyzer):

    """
    A protocol analyzer for UMTS NAS layer (MM/GMM/CM/SM)
    """

    def __init__(self):

        Analyzer.__init__(self)
        #init packet filters
        self.add_source_callback(self.__nas_filter)

        self.__mm_status = MmStatus()
        self.__gmm_status = GmmStatus()
        self.__mm_nas_status = MmNasStatus()

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
            xml_msg = Event(msg.timestamp,msg.type_id,log_xml)
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
        self.__gmm_status.state = msg.data['GMM State']
        self.__gmm_status.substate = msg.data['GMM Substate']
        self.__gmm_status.update_status = msg.data['GMM Update Status']

    def __callback_nas(self,msg):
        """
        Extrace MM status and configurations from the NAS messages

        :param msg: the MM NAS message
        """

        # for proto in msg.data.iter('proto'):
        #     if proto.get('name') == "gsm_a.dtap": #GSM A-I/F DTAP - Location Updating Request
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

            if field.get('show') == "Quality Of Service - Negotiated QoS":
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
                self.__mm_nas_status.qos_negotiated.residual_ber = residual_ber(int(field_val['gsm_a.gm.sm.qos.ber']))
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
                field_val["gsm_a.gm.sm.qos.mean_throughput"] = None
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
                self.__mm_nas_status.qos_requested.residual_ber = residual_ber(int(field_val['gsm_a.gm.sm.qos.ber']))
                self.__mm_nas_status.qos_requested.transfer_delay = trans_delay(int(field_val['gsm_a.gm.sm.qos.trans_delay']))
                self.__mm_nas_status.qos_requested.max_bitrate_ulink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_upl']))
                self.__mm_nas_status.qos_requested.max_bitrate_dlink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl']))
                self.__mm_nas_status.qos_requested.guaranteed_bitrate_ulink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_upl']))
                self.__mm_nas_status.qos_requested.guaranteed_bitrate_dlink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl']))
                # self.__mm_nas_status.qos_requested.max_bitrate_ulink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.max_bitrate_upl_ext']))
                self.__mm_nas_status.qos_requested.max_bitrate_dlink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl_ext']))
                # self.__mm_nas_status.qos_requested.guaranteed_bitrate_ulink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_upl_ext']))
                self.__mm_nas_status.qos_requested.guaranteed_bitrate_dlink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl_ext']))

                # self.__mm_nas_status.qos_requested.len = field_val["gsm_a.len"]
                # self.__mm_nas_status.qos_requested.spare_bits = field_val["gsm_a.spare_bits"]
                # self.__mm_nas_status.qos_requested.delay_cls = field_val["gsm_a.gm.sm.qos.delay_cls"]
                # self.__mm_nas_status.qos_requested.reliability_cls = field_val["gsm_a.gm.sm.qos.reliability_cls"]
                # self.__mm_nas_status.qos_requested.peak_throughput = field_val["gsm_a.gm.sm.qos.peak_throughput"]
                # self.__mm_nas_status.qos_requested.spare_bits = field_val["gsm_a.spare_bits"]
                # self.__mm_nas_status.qos_requested.prec_class = field_val["gsm_a.gm.sm.qos.prec_class"]
                # self.__mm_nas_status.qos_requested.spare_bits = field_val["gsm_a.spare_bits"]
                # self.__mm_nas_status.qos_requested.mean_throughput = field_val["gsm_a.gm.sm.qos.mean_throughput"]
                # self.__mm_nas_status.qos_requested.traffic_cls = field_val["gsm_a.gm.sm.qos.traffic_cls"]
                # self.__mm_nas_status.qos_requested.del_order = field_val["gsm_a.gm.sm.qos.del_order"]
                # self.__mm_nas_status.qos_requested.del_of_err_sdu = field_val["gsm_a.gm.sm.qos.del_of_err_sdu"]
                # self.__mm_nas_status.qos_requested.max_sdu = field_val["gsm_a.gm.sm.qos.max_sdu"]
                # self.__mm_nas_status.qos_requested.max_bitrate_upl = field_val["gsm_a.gm.sm.qos.max_bitrate_upl"]
                # self.__mm_nas_status.qos_requested.max_bitrate_downl = field_val["gsm_a.gm.sm.qos.max_bitrate_downl"]
                # self.__mm_nas_status.qos_requested.ber = field_val["gsm_a.gm.sm.qos.ber"]
                # self.__mm_nas_status.qos_requested.sdu_err_rat = field_val["gsm_a.gm.sm.qos.sdu_err_rat"]
                # self.__mm_nas_status.qos_requested.trans_delay = field_val["gsm_a.gm.sm.qos.trans_delay"]
                # self.__mm_nas_status.qos_requested.traff_hdl_pri = field_val["gsm_a.gm.sm.qos.traff_hdl_pri"]
                # self.__mm_nas_status.qos_requested.guar_bitrate_upl = field_val["gsm_a.gm.sm.qos.guar_bitrate_upl"]
                # self.__mm_nas_status.qos_requested.guar_bitrate_downl = field_val["gsm_a.gm.sm.qos.guar_bitrate_downl"]
                # self.__mm_nas_status.qos_requested.spare_bits = field_val["gsm_a.spare_bits"]
                # self.__mm_nas_status.qos_requested.signalling_ind = field_val["gsm_a.gm.sm.qos.signalling_ind"]
                # self.__mm_nas_status.qos_requested.source_stat_desc = field_val["gsm_a.gm.sm.qos.source_stat_desc"]
                # self.__mm_nas_status.qos_requested.max_bitrate_downl_ext = field_val["gsm_a.gm.sm.qos.max_bitrate_downl_ext"]
                # self.__mm_nas_status.qos_requested.guar_bitrate_downl_ext = field_val["gsm_a.gm.sm.qos.guar_bitrate_downl_ext"]

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

        

        
        