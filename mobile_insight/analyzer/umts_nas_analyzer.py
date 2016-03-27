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

from protocol_analyzer import *
from profile import Profile, ProfileHierarchy

from nas_util import *


__all__=["UmtsNasAnalyzer"]


class UmtsNasAnalyzer(ProtocolAnalyzer):

    """
    A protocol analyzer for UMTS NAS layer (MM/GMM/CM/SM)
    """

    def __init__(self):

        ProtocolAnalyzer.__init__(self)
        #init packet filters
        self.add_source_callback(self.__nas_filter)

        self.__mm_status = MmStatus()
        self.__gmm_status = GmmStatus()
        self.__mm_nas_status = MmNasStatus()

    def create_profile_hierarchy(self):
        '''
        Return a UMTS NAS ProfileHierarchy (configurations)

        :returns: ProfileHierarchy for LTE NAS
        '''
        return UmtsNasProfileHierarchy()

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
        source.enable_log("UMTS_NAS_MM_REG_State")

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


        if msg.type_id == "UMTS_NAS_MM_REG_State":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            raw_msg = Event(msg.timestamp,msg.type_id,log_item_dict)
            self.__callback_mm_reg_state(raw_msg)

    
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

        self.logger.info(self.__mm_status.dump())

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

        self.logger.info(self.__mm_status.dump())

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

                self.logger.info(self.__mm_nas_status.dump())
                # profile update for esm qos
                self.profile.update("UmtsNasProfile:"+self.__mm_status.profile_id()+".pdp.qos",
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
                self.__mm_nas_status.qos_requested.residual_ber = residual_ber(int(field_val['gsm_a.gm.sm.qos.ber']))
                self.__mm_nas_status.qos_requested.transfer_delay = trans_delay(int(field_val['gsm_a.gm.sm.qos.trans_delay']))
                self.__mm_nas_status.qos_requested.max_bitrate_ulink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_upl']))
                self.__mm_nas_status.qos_requested.max_bitrate_dlink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl']))
                self.__mm_nas_status.qos_requested.guaranteed_bitrate_ulink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_upl']))
                self.__mm_nas_status.qos_requested.guaranteed_bitrate_dlink = max_bitrate(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl']))
                self.__mm_nas_status.qos_requested.max_bitrate_dlink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.max_bitrate_downl_ext']))
                self.__mm_nas_status.qos_requested.guaranteed_bitrate_dlink_ext = max_bitrate_ext(int(field_val['gsm_a.gm.sm.qos.guar_bitrate_downl_ext']))

                self.profile.update("UmtsNasProfile:"+self.__mm_status.profile_id()+".pdp.qos",
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
                    'max_bitrate_ulink_ext':xstr(self.__mm_nas_status.qos_requested.max_bitrate_ulink_ext),
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
    qos.add('max_bitrate_ulink_ext',False)
    qos.add('max_bitrate_dlink_ext',False)
    qos.add('guaranteed_bitrate_ulink_ext',False)
    qos.add('guaranteed_bitrate_dlink_ext',False)
    qos.add('residual_ber',False)

    return profile_hierarchy

        

        
        