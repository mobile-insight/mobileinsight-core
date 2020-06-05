#!/usr/bin/python
# Filename: lte_handover_prediction_analyzer.py
"""
A file that count the occurrences of handover triggered by different events

Author: Yuanjie Li
"""

try: 
    import xml.etree.cElementTree as ET 
except ImportError: 
    import xml.etree.ElementTree as ET

from ..analyzer import *
from .kpi_analyzer import KpiAnalyzer

import copy

import pickle
# 
__all__=["LteHandoverPredictionAnalyzer"]

import time
import datetime

# CONDITION_INTRA = ""


def string2timestamp(s):
    # dt=datetime.datetime.strptime(s, "%Y-%m-%d %H:%M:%S.%f")
    # return time.mktime(dt.timetuple()) + (dt.microsecond / 1000000.0)
    return time.mktime(s.timetuple()) + s.microsecond/1000000.0

class LteHandoverPredictor():
    def __init__(self):
        self.pred_cnt    = 0
        self.ho_cnt      = 0
        self.fp_cnt      = 0  # false positive
        self.fn_cnt      = 0  # false negative

        self.report_state = 0 # 0: initial state; 1: waiting state; 2: true state
        self.pred_flag   = True
        self.report_time = 0
        self.current_time = 0

        self.a3a5_cnt = 0

        self.freq = ''


    def setCurrentTime(self, current_datetime):
        if type(current_datetime) == type('str'):
            current_datetime = datetime.datetime.strptime(current_datetime, '%Y-%m-%d %H:%M:%S.%f')
        self.current_time = current_datetime.microsecond / 1e3 + time.mktime(current_datetime.timetuple()) * 1e3


    def isExpired(self):
        if self.pred_flag and (1000+self.report_time) < self.current_time:
            self.pred_flag = False
            self.fp_cnt += 1
            self.report_state = 0

    def recRrcRelease(self):
        if self.pred_flag:
            self.fp_cnt += 1
            # measReportTraces    = []
            self.pred_flag           = False
            self.report_state        = 0

            self.a3a5_cnt = 0


    # if log_item['field'] == 'measReport':
    def recMeasReport(self, log_item):
        if log_item['event'] == 'a3' or log_item['event'] == 'a5':
            self.a3a5_cnt += 1

        if log_item['event'] == 'a3':
            if self.pred_flag == False:
                self.pred_cnt += 1
                self.pred_flag = True
                self.report_time = self.current_time
                self.report_state = 2
                # print "{},{},{}".format(log_item['Timestamp'], current_time,'pred at a3')

        else:
            if self.report_state == 0:
                if log_item['event'] == 'a2' and log_item['Current freq'] == log_item['freq']:
                    self.report_state    = 1
                        # print "{},{}".format(log_item['Timestamp'],'intra a2')

            elif self.report_state == 1:
                if log_item['event'] == 'a5' and log_item['Current freq'] != None and log_item['freq'] != log_item['Current freq']:
                    if self.pred_flag == False:
                        self.pred_cnt    += 1
                        self.pred_flag       = True
                        self.report_time     = self.current_time
                        self.report_state    = 2
                        # print "{},{}".format(log_item['Timestamp'],'pred at a5')
                elif log_item['event'] == 'a1' and log_item['Current freq'] == log_item['freq']:
                    report_state    = 0


    # if log_item['field'] == 'measControl':
    def recMeasControl(self):
        if self.a3a5_cnt > 0:
            self.ho_cnt += 1
            if self.pred_flag == False:
                self.fn_cnt  += 1

        self.a3a5_cnt = 0
        self.pred_flag   = False
        self.report_state = 0 # 0: initial state; 1: waiting state; 2: true state

    # print "{},{},{},{}".format(ho_cnt,pred_cnt,fp_cnt,tn_cnt)

    # def uploadKpi(self, analyzer, ho_cond, ho_occur):
    #     kpi = {}
    #     # self.attributes = ["predict_condition", "handover_occurence"]
    #     kpi['predict_condition'] = ho_cond
    #     kpi['handover_occurence'] = ho_occur
        # analyzer.upload_kpi("KPI.Mobility.HANDOVER_LATENCY", kpi)


class LteHandoverPredictionAnalyzer(KpiAnalyzer):
    """
    A function analyzer that models mobility management. 
    It offers two functions

        (1) Mobility event notification
        
        (2) A complete mobility history and configuration query

        (3) A handoff rule inference module
    """    

    """
    Development plan:
        Step 1: make it work in single cell, LTE only
        Step 2: support cell change with loading/saving state machine for different freq under LTE
        Step 3: support in-RAT
    """
    def __init__(self):

        KpiAnalyzer.__init__(self)

        self.handoverMsg = []

        self.__handoff_sample = HandoffSample()
        self.__mobility_state_machine = MobilityStateMachine()


        self.__b_prediction = True #handoff prediction is disabled by default
        self.__predict_target = None #predicted target cell

        # self.__print = False

        #include analyzers
        # self.include_analyzer("WcdmaRrcAnalyzer",[self.__on_wcdma_rrc_msg])
        self.include_analyzer("LteRrcAnalyzer",[self.__on_lte_rrc_msg])
        self.include_analyzer("TrackCellInfoAnalyzer",[])
        # self.include_analyzer("LteNasAnalyzer",[self.__on_lte_nas_msg])
        # self.include_analyzer("UmtsNasAnalyzer",[self.__on_umts_nas_msg])

        self.ho_predictor = LteHandoverPredictor()

        self.attributes = ["predict_condition", "handover_occurence"]

        #no source callbacks are included
        self.register_kpi("Mobility", "HANDOVER_PREDICTION", self.__on_lte_rrc_msg, 0)



    def reset(self):
        """
        Reset the state machine
        """
        self.__mobility_state_machine.reset()
        self.__handoff_sample = HandoffSample()


    def set_handoff_prediction(self,b_predict):
        """
        Enable/disable handoff prediction

        :param b_prediction: True if prediction should be enabled, False otherwise
        :type b_prediction: boolean
        """
        self.__b_prediction = b_predict


    def __on_lte_rrc_msg(self,msg):
        """
        Handle LTE RRC messages.
        It updates the mobility state, 
        recovers the partial state transition, 
        and then the entire handoff mobility graph

        :param msg: the event (message) from the trace collector.
        """    

        #The message from LteRrcAnalyzer is decoded XML messages
        # if self.__print == False:
        #     print msg.data.decode()
        #     self.__print = True
        # if self.ho_predictor.

        for field in msg.data.iter('field'):
            if field.get('name') == "lte-rrc.rrcConnectionRelease_element" or field.get('name') == 'lte-rrc.rrcConnectionRequest_element':
                self.ho_predictor.setCurrentTime(msg.timestamp)
                self.ho_predictor.recRrcRelease()

            if field.get('name')=="lte-rrc.mobilityControlInfo_element":

                #A handoff command: create a new HandoffState
                target_cell = None
                target_cell_id = None
                target_bw = None
                for val in field.iter('field'):
                    #Currently we focus on freq-level handoff
                    if val.get('name')=='lte-rrc.dl_Bandwidth':
                        target_bw = val.get('show')
                    if val.get('name')=='lte-rrc.dl_CarrierFreq':
                        target_cell = val.get('show')
                    if val.get('name')=='lte-rrc.targetPhysCellId':
                        target_cell_id = int(val.get('show'))
                    
                if not target_cell:
                    #In T-Mobile, some logs does not carry dl_CarrierFreq.
                    #These are intra-frequency handoff
                    target_cell = self.get_analyzer("LteRrcAnalyzer").get_cur_cell().freq

                if target_cell:
                    #FIXME: consider 4G->3G handover (e.g., SRVCC, CSFB)
                    meas_state_tmp = copy.deepcopy(self.__handoff_sample.cur_state)

                    handoff_state = HandoffState("LTE",target_cell)
                    self.__handoff_sample.add_state_transition(handoff_state)
                    #Trigger merging function
                    self.__mobility_state_machine.update_state_machine(self.__handoff_sample)
                    #Reset handoff sample
                    
                    self.__handoff_sample = HandoffSample()
                    self.__handoff_sample.cur_state = copy.deepcopy(meas_state_tmp)

                    bcast_dict = {}
                    bcast_dict['Timestamp'] = str(string2timestamp(msg.timestamp))
                    bcast_dict['event'] = str(handoff_state.dump())
                    # self.broadcast_info('HANDOVER_EVENT',bcast_dict)
                    # self.log_info(str(string2timestamp(msg.timestamp))+" Handoff to " + handoff_state.dump())
                    # self.log_info(target_bw)

                    # Broadcast to apps
                    # cur_cell_freq   = self.get_analyzer("LteRrcAnalyzer").ql_get_cur_freq()
                    # cur_cell_id     = self.get_analyzer("LteRrcAnalyzer").ql_get_cur_cellid()
                    cur_cell_freq   = self.get_analyzer("LteRrcAnalyzer").get_cur_cell().freq
                    cur_cell_id     = self.get_analyzer("LteRrcAnalyzer").get_cur_cell().id
                    # print cur_cell_freq
                    # print self.get_analyzer("LteRrcAnalyzer").get_cur_cell().freq
                    # self.ho_predictor.recMeasControl({'Current Cell ID':cur_cell_id, 'Current Freq':cur_cell_freq, 'Target Cell Id':target_cell_id, 'Target Radio':handoff_state.rat, 'Target Freq':int(handoff_state.freq), 'Target bandwidth':target_bw})
                    self.ho_predictor.recMeasControl()

                    return


            if field.get('name')=="lte-rrc.measurementReport_element":
                #A measurement report: parse it, push it into Handoff sample
                meas_id = None
                rss = None
                
                neighborCells = []
                maxRss = -140
                curNeighborCell = None

                for val in field.iter('field'):
                    if val.get('name')=='lte-rrc.measId':
                        meas_id = val.get('show')
                    if val.get('name')=='lte-rrc.rsrpResult':
                        rss_tmp = int(val.get('show'))-140
                        if curNeighborCell == None:
                            rss = rss_tmp
                        else:
                            if neighborCells == [] or rss_tmp > maxRss:
                                neighborCells = [curNeighborCell]
                                maxRss = rss_tmp
                            elif neighborCells != [] and rss_tmp == maxRss:
                                neighborCells.append(curNeighborCell)

                            # neighCellDict[neighCell] = rss_tmp
                    if val.get('name')=='lte-rrc.physCellId':
                        curNeighborCell = int(val.get('show'))

                if meas_id and self.__handoff_sample.cur_state:
                    

                    meas_report =  self.__handoff_sample.cur_state.get_meas_report_obj(meas_id)
                    self.__handoff_sample.add_meas_report(meas_report)

                    bandwidth = None
                    # self.log_info(meas_id+" "+str(self.__handoff_sample.cur_state.measid_list)+" "+str(meas_report))
                    # cur_cell_freq = self.get_analyzer("LteRrcAnalyzer").ql_get_cur_freq()
                    cur_cell_freq   = self.get_analyzer("LteRrcAnalyzer").get_cur_cell().freq

                    bandwidth = None
                    meas_obj_freq = None

                    if meas_report[0]: #meas obj is known
                        try:
                            bandwidth = meas_report[0].measBandwidth
                            meas_obj_freq = meas_report[0].freq
                        except:
                            meas_obj_freq = None
                            # print 'Error'
                        # print "{},{},{}".format(msg.timestamp,meas_id,meas_obj_freq)
                        # self.log_info("Measurement obj bandwidth:"+" "+bandwidth)
                    if meas_report[1]: #report config is known
                        # self.log_info(str(string2timestamp(msg.timestamp))+" Measurement report "+str(meas_report[1].event_list[0].type)+" "+str(rss))
                        # Broadcast to apps
                        # print neighborCells

                        old_flag = self.ho_predictor.pred_flag

                        self.ho_predictor.recMeasReport({'event':str(meas_report[1].event_list[0].type), 'neighbor cells':neighborCells, 'Current freq':cur_cell_freq, 'freq':meas_obj_freq, 'bandwidth':bandwidth})

                        if not old_flag and self.ho_predictor.pred_flag:
                            bcast_dict = {}
                            bcast_dict['Timestamp'] = str(string2timestamp(msg.timestamp))
                            bcast_dict['event'] = str(meas_report[1].event_list[0].type)
                            self.store_kpi("KPI_Mobility_HANDOVER_PREDICTION", "1", msg.timestamp)
                            # self.broadcast_info('HANDOVER_PREDICTION', bcast_dict)
                            # self.log_error(str(string2timestamp(msg.timestamp))+" Handover will occur" )
                        # bcast_dict = {}
                        # bcast_dict['Timestamp']=str(msg.timestamp)
                        # bcast_dict['event'] = str(meas_report[1].event_list[0].type)
                        # bcast_dict['rss'] = str(rss)
                        # self.broadcast_info('MEAS_REPORT',bcast_dict)


            # if field.get('name')=="lte-rrc.measResultsCDMA2000_element":

            #     rss = None
            #     for val in field.iter('field'):
            #         if val.get('name')=='lte-rrc.pilotStrength':
            #             rss = val.get('show')
            #     #CDMA2000 measurement report
            #     #NOTE: Different from normal meas report, this one does not have measid/reportid
            #     tmp = LteReportConfig("CDMA2000",0)
            #     tmp.add_event("CDMA2000",0)
            #     meas_report = (LteMeasObjectCDMA2000(None,0),tmp)   #fake an empty report
                
            #     self.__handoff_sample.add_meas_report(meas_report)
            #     self.log_info(str(string2timestamp(msg.timestamp))+" Measurement report cdma2000 "+str(rss))

                #TODO: broadcast to apps


            if field.get('name')=="lte-rrc.measConfig_element":

                #A Measurement control reconfiguration
                meas_state = None
                if self.__handoff_sample.cur_state:
                    #Meas control may take stateful addition/deletion, 
                    #So need current copy whenever available
                    meas_state = copy.deepcopy(self.__handoff_sample.cur_state)
                else:
                    meas_state = MeasState()

                for val in field.iter('field'):
                    if val.get('name')=='lte-rrc.MeasObjectToAddMod_element':
                        #Add measurement object
                        meas_obj = self.__get_meas_obj(val)
                        if meas_obj:
                            meas_state.measobj[meas_obj.obj_id] = meas_obj

                    if val.get('name')=='lte-rrc.measObjectToRemoveList':
                        #Remove measurement object
                        for item in val.iter('field'):
                            if item.get('name')=='lte-rrc.MeasObjectId' \
                            and item.get('show') in meas_state.measobj:
                                del meas_state.measobj[item.get('show')]

                    if val.get('name')=='lte-rrc.ReportConfigToAddMod_element':
                        #Add/modify a report config
                        report_config = self.__get_report_config(val)
                        if report_config:
                            meas_state.report_list[report_config.report_id]=report_config
            
                    if val.get('name')=='lte-rrc.reportConfigToRemoveList':
                        #Remove a report config
                        for item in val.iter('field'):
                            if item.get('name')=='lte-rrc.ReportConfigId' \
                            and item.get('show') in meas_state.report_list:
                                del meas_state.report_list[item.get('show')]

                    if val.get('name')=='lte-rrc.MeasIdToAddMod_element':
                        #Add a measurement ID
                        meas_id = -1
                        meas_obj_id = -1
                        report_id = -1
                        for item in val.iter('field'):
                            if item.get('name')=='lte-rrc.measId':
                                meas_id = item.get('show')
                            if item.get('name')=='lte-rrc.measObjectId':
                                meas_obj_id = item.get('show')
                            if item.get('name')=='lte-rrc.reportConfigId':
                                report_id = item.get('show')
                        meas_state.measid_list[meas_id]=(meas_obj_id,report_id)
                        # self.log_info('Add:'+ meas_id+','+report_id)
                    
                    if val.get('name')=='lte-rrc.measIdToRemoveList':
                        #Remove a measurement ID
                        # self.log_info(str(meas_state.measid_list))
                        for item in val.iter('field'):
                            # if item.get('name')=='lte-rrc.MeasId':
                            #     self.log_info("In remove list:"+item.get('show'))
                            if item.get('name')=='lte-rrc.MeasId' and item.get('show') in meas_state.measid_list:
                                del meas_state.measid_list[item.get('show')]
                                # self.log_info("Remove:"+item.get('show'))
            
                #Generate a new state to the handoff sample
                self.__handoff_sample.add_state_transition(meas_state)

                # self.__mobility_state_machine.update_state_machine(self.__handoff_sample)
                # #Reset handoff sample
                # self.__handoff_sample = HandoffSample()
                # self.log_info(str(string2timestamp(msg.timestamp))+" Measurement control")
                # self.log_info("Meas State: \n"+meas_state.dump())

                # Broadcast to apps
                # bcast_dict = {}
                # bcast_dict['Timestamp']=str(msg.timestamp)
                # bcast_dict['Control info'] = meas_state.dump()
                # self.broadcast_info('MEAS_CTRL',bcast_dict)


    def __get_meas_obj(self,msg):
        """
        Parse MeasObjectToAddMod_element, return a measurement object

        :param msg: the XML msg with MeasObjectToAddMod_element
        :returns: a measurement objects to be added
        """
        measobj_id = -1
        for field in msg.iter('field'):
            if field.get('name') == "lte-rrc.measObjectId":
                measobj_id = field.get('show')

            if field.get('name') == "lte-rrc.measObjectEUTRA_element": 

                #A LTE meas obj
                field_val = {}

                field_val['lte-rrc.carrierFreq'] = None
                field_val['lte-rrc.offsetFreq'] = 0

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')
                if field_val['lte-rrc.carrierFreq']:
                    freq = int(field_val['lte-rrc.carrierFreq'])
                    offsetFreq = int(field_val['lte-rrc.offsetFreq'])
                    if field_val['lte-rrc.allowedMeasBandwidth']:
                        measBandwidth = field_val['lte-rrc.allowedMeasBandwidth']
                    # print "obj:{},{}".format(msg.timestamp,measobj_id,freq)
                    return LteMeasObjectEutra(measobj_id,freq,offsetFreq,measBandwidth)

            # if field.get('name') == "lte-rrc.measObjectUTRA_element":
            #     field_val = {}

            #     field_val['lte-rrc.carrierFreq'] = None
            #     field_val['lte-rrc.offsetFreq'] = 0

            #     for val in field.iter('field'):
            #         field_val[val.get('name')] = val.get('show')
                
            #     if field_val['lte-rrc.carrierFreq']:
            #         freq = int(field_val['lte-rrc.carrierFreq'])
            #         offsetFreq = int(field_val['lte-rrc.offsetFreq'])
            #         return LteMeasObjectUtra(measobj_id,freq,offsetFreq)

            # if field.get('name') == "lte-rrc.measObjectGERAN_element":
            #     field_val = {}

            #     field_val['lte-rrc.bandIndicator'] = None
            #     field_val['lte-rrc.offsetFreq'] = 0

            #     for val in field.iter('field'):
            #         field_val[val.get('name')] = val.get('show')

            #     if field_val['lte-rrc.bandIndicator']:
            #         freq = int(field_val['lte-rrc.bandIndicator'])
            #         offsetFreq = int(field_val['lte-rrc.offsetFreq'])
            #         return LteMeasObjectGERAN(measobj_id,freq,offsetFreq)

            # if field.get('name') == 'lte-rrc.measObjectCDMA2000_element':
            #     field_val = {}

            #     field_val['lte-rrc.bandClass'] = None

            #     for val in field.iter('field'):
            #         field_val[val.get('name')] = val.get('show')

            #     if field_val['lte-rrc.bandClass']:
            #         freq = int(field_val['lte-rrc.bandClass'])
            #         return LteMeasObjectCDMA2000(measobj_id,freq)
        
        return None #How can this happen?
    
    def __get_report_config(self,msg):
        """
        Parse ReportConfigToAddMod_element, return a report config

        :param msg: the XML msg with ReportConfigToAddMod_element
        :returns: a measurement objects to be added
        """
        report_id = -1
        hyst = 0
        for val in msg.iter('field'):
            if val.get('name') == "lte-rrc.reportConfigId":
                report_id = val.get('show')
            if val.get('name') == 'lte-rrc.hysteresis':
                hyst = int(val.get('show'))

        report_config = LteReportConfig(report_id,hyst/2)

        for val in msg.iter('field'):
            if val.get('name') == 'lte-rrc.eventA1_element':
                for item in val.iter('field'):
                    if item.get('name') == 'lte-rrc.threshold_RSRP':
                        report_config.add_event('a1',int(item.get('show'))-140)
                        break
                    if item.get('name') == 'lte-rrc.threshold_RSRQ':
                        report_config.add_event('a1',(int(item.get('show'))-40)/2)
                        break

            if val.get('name') == 'lte-rrc.eventA2_element':
                for item in val.iter('field'):
                    if item.get('name') == 'lte-rrc.threshold_RSRP':
                        report_config.add_event('a2',int(item.get('show'))-140)
                        break
                    if item.get('name') == 'lte-rrc.threshold_RSRQ':
                        report_config.add_event('a2',(int(item.get('show'))-40)/2)
                        break

            if val.get('name') == 'lte-rrc.eventA3_element':
                for item in val.iter('field'):
                    if item.get('name') == 'lte-rrc.a3_Offset':
                        report_config.add_event('a3',int(item.get('show'))/2)
                        break

            if val.get('name') == 'lte-rrc.eventA4_element':
                for item in val.iter('field'):
                    if item.get('name') == 'lte-rrc.threshold_RSRP':
                        report_config.add_event('a4',int(item.get('show'))-140)
                        break
                    if item.get('name') == 'lte-rrc.threshold_RSRQ':
                        report_config.add_event('a4',(int(item.get('show'))-40)/2)
                        break

            if val.get('name') == 'lte-rrc.eventA5_element':
                threshold1 = None
                threshold2 = None
                for item in val.iter('field'):
                    if item.get('name') == 'lte-rrc.a5_Threshold1':
                        for item2 in item.iter('field'):
                            if item2.get('name') == 'lte-rrc.threshold_RSRP':
                                threshold1 = int(item2.get('show'))-140
                                break
                            if item2.get('name') == 'lte-rrc.threshold_RSRQ':
                                threshold1 = (int(item2.get('show'))-40)/2
                                break
                    if item.get('name') == 'lte-rrc.a5_Threshold2':
                        for item2 in item.iter('field'):
                            if item2.get('name') == 'lte-rrc.threshold_RSRP':
                                threshold2 = int(item2.get('show'))-140
                                break
                            if item2.get('name') == 'lte-rrc.threshold_RSRQ':
                                threshold2 = (int(item2.get('show'))-40)/2
                                break
                report_config.add_event('a5',threshold1,threshold2)

            if val.get('name') == 'lte-rrc.eventB1_element':
                for item in val.iter('field'):
                    if item.get('name') == 'lte-rrc.threshold_RSRP':
                        report_config.add_event('b1',int(item.get('show'))-140)
                        break
                    if item.get('name') == 'lte-rrc.threshold_RSRQ':
                        report_config.add_event('b1',(int(item.get('show'))-40)/2)
                        break
                    if item.get('name') == 'lte-rrc.threshold_RSCP':
                        report_config.add_event('b1',int(item.get('show'))-115)
                        break
            
            if val.get('name') == 'lte-rrc.eventB2_element':

                threshold1 = None
                threshold2 = None
                for item in val.iter('field'):
                    if item.get('name') == 'lte-rrc.b2_Threshold1':
                        for item2 in item.iter('field'):
                            if item2.get('name') == 'lte-rrc.threshold_RSRP':
                                threshold1 = int(item2.get('show'))-140
                                break
                            if item2.get('name') == 'lte-rrc.threshold_RSRQ':
                                threshold1 = (int(item2.get('show'))-40)/2
                                break
                    if item.get('name') == 'lte-rrc.b2_Threshold2':
                        for item2 in item.iter('field'):
                            if item2.get('name') == 'lte-rrc.threshold_RSRP':
                                threshold2 = int(item2.get('show'))-140
                                break
                            if item2.get('name') == 'lte-rrc.threshold_RSRQ':
                                threshold2 = (int(item2.get('show'))-40)/2
                                break
                            if item2.get('name') == 'lte-rrc.utra_RSCP':
                                threshold2 = int(item2.get('show'))-115
                                break
                report_config.add_event('b2',threshold1,threshold2)

        if report_config.event_list:
            return report_config
        else:
            #periodical report. No impact on handoff
            return None


# Handoff rule inference modules
############################################
class HandoffState:
    """
    A state abstraction to represent the handoff target
    This is used for handoff policy inference.

    In current implement, we choose frequency-level handoff target granualrity 
    (rather than cell level). This is based on the observation that cells of the 
    same frequency are homogeneous. Operators in reality tend to not differentiate them
    """
    def __init__(self,rat, freq):
        self.rat = rat #Radio access technology (3G or 4G)
        self.freq = freq #Frequency band

    def equals(self,handoff_state):

        if handoff_state.__class__.__name__!="HandoffState":
            return False
        return handoff_state.freq==self.freq \
        and handoff.rat==self.rat 

    def dump(self):
        return "("+str(self.rat)+","+str(self.freq)+")\n"


class MeasState:
    """
    A Measurement state for the handoff policy inference
    """
    def __init__(self):
        #TODO: initialize some containers
        #FIXME: change the key of measobj: freq to obj_id
        self.measobj = {} #obj_id->measobject
        self.report_list = {} #report_id->reportConfig
        self.measid_list = {} #meas_id->(obj_id,report_id)

    def get_measobj(self,meas_id):
        """
        Given the measurement ID, returns the corresponding measurement object

        :param meas_id: measurement ID
        :type meas_id: integer
        :returns: Measurement object in it, or None if the id does not exist
        """
        # meas_obj = None
        # for i in self.measobj:
        #     if self.measobj[i].obj_id == self.measid_list[item][0]:
        #         meas_obj = self.measobj[i]
        # return meas_obj
        if meas_id not in self.measid_list \
        or self.measid_list[meas_id][0] not in self.measobj:
            # print "get_measobj: meas_id="+str(meas_id)+" meas_obj="+str(self.measid_list[meas_id])
            # print "debug: "+str(self.measobj.keys())
            return None 
        else:
            return self.measobj[self.measid_list[meas_id][0]]

    def get_reportconfig(self,meas_id):
        """
        Given the measurement ID, returns the corresponding report configuration object

        :param meas_id: measurement ID
        :type meas_id: integer
        :returns: ReportConfig in it, or None if the id does not exist
        """
        if meas_id not in self.measid_list \
        or self.measid_list[meas_id][1] not in self.report_list:
            return None
        else:
            return self.report_list[self.measid_list[meas_id][1]]

    def get_meas_report_obj(self,meas_id):
        """
        return the measurement report obj
        :param meas_id: measurement ID
        :type meas_id: integer
        :returns: (measobj,report_config) pair
        """
        measobj = self.get_measobj(meas_id)
        report_config = self.get_reportconfig(meas_id)

        return (measobj,report_config)

    def equals(self,meas_state):
        """
        Compare two states to see if they are equivalent

        :param meas_state: another measurement state
        :type meas_state: MeasState
        :returns: True if two states are equivalent, False otherwise
        """

        if meas_state.__class__.__name__ != "MeasState":
            return False

        #Algorithm for comparison:
        #Compare all objects in measid_list
        #For each one, check its freq, and the event/threshold configurations
        if len(self.measid_list) != len(meas_state.measid_list):
            return False
        for meas_id in self.measid_list:

            #Get its measobj and reportConfig
            meas_obj = self.get_measobj(meas_id)
            report_obj = self.get_reportconfig(meas_id)
            if not meas_obj or not report_obj:
                #Should not happen unless bug exists
                return False     

            #Find if this measurement object also exists int meas_state
            meas_id_exist = False
            for meas_id2 in meas_state.measid_list:
                meas_obj2 = meas_state.get_measobj(meas_id2)
                report_obj2 = meas_state.get_reportconfig(meas_id2)
                if meas_obj.equals(meas_obj2) and report_obj.equals(report_obj2):
                    meas_id_exist = True
                    break
            if not meas_id_exist:
                return False
        return True

    def dump(self):
        """
        Report the cell's active-state configurations

        :returns: a string that encodes the cell's active-state configurations
        :rtype: string
        """
        res = ""
        for item in self.measobj:
            res += self.measobj[item].dump()
        for item in self.report_list:
            res += self.report_list[item].dump()
        for item in self.measid_list:
            res += "MeasObj "+str(item)+' '+str(self.measid_list[item])+'\n'
        return res


class MeasReportSeq:
    """
    An abstraction for measurement report sequence
    """
    def __init__(self):
        self.meas_report_queue=[]

    def add_meas_report(self,meas_report):
        """
        Append a measurement report.
        Currently we abstract the concrete measured signal strength

        :param meas_report: a (MeasObject,ReportConfig) pair for that report
        :type meas_report:(MeasObject,ReportConfig)
        :returns: True if successfully appended, False otherwise
        """
        if meas_report.__class__.__name__!="tuple":
            return False
        if meas_report[0].__class__.__name__!="LteMeasObjectEutra" \
        and meas_report[0].__class__.__name__!="LteMeasObjectUtra" \
        and meas_report[0].__class__.__name__!="LteMeasObjectGERAN" \
        and meas_report[0].__class__.__name__!="LteMeasObjectCDMA2000" \
        and meas_report[1].__class__.__name__!="LteReportConfig":
            return False
        if meas_report[0] and meas_report[1]:
            self.meas_report_queue.append(meas_report)
        return True

    def merge_seq(self,meas_report_seq):
        """
        Merge two measurement report sequence with longest common substring (LCS) algorithm
        This is the core function of mobility policy inference
        
        :param meas_report_seq: measurement report sequence
        :type meas_report_seq: MeasReportSeq
        :returns: True if succeeded, False otherwise
        """
        if meas_report_seq.__class__.__name__!="MeasReportSeq":
            return False

        #TODO: this function should be moved to MobilityStateMachine, 
        #because it needs to resolve global conflicts
        #TODO: replace the following code with LCS algorithm
        #As first step, we simply replace the existing sequence
        self.meas_report_queue = meas_report_seq.meas_report_queue

    def equals(self,meas_report_seq):
        """
        Compare if two measurement sequences are equivalent
        
        :param meas_report_seq: measurement report sequence
        :type meas_report_seq: MeasReportSeq
        :returns: True if equivalent, False otherwise
        """
        if meas_report_seq.__class__.__name__!="MeasReportSeq":
            return False

        


class HandoffSample:
    """
    A handoff sample based on observation
    """
    def __init__(self):
        self.cur_state = None
        #(From_State,To_State,tx_cond)
        #For the first element, its tx_cond is meaningless
        self.tx_list=[] 
        self.tx_cond=MeasReportSeq()

    def add_meas_report(self,meas_report):
        """
        Add a measurement report_event

        :param meas_report: a new measurement report
        :type meas_report: (meas_obj,report_config)
        """

        #If current state is None, ignore the input (i.e., drop this sample)
        if self.cur_state:
           self.tx_cond.add_meas_report(meas_report) 


    def add_state_transition(self,new_state):
        """
        Append a new state and its transition condition.

        :param new_state: a MeasState or a HandoffState
        :type new_state: MeasState or HandoffState
        :returns: True if succeeds, or False otherwise
        """
        if new_state.__class__.__name__!="MeasState" \
        and new_state.__class__.__name__!="HandoffState":
            return False

        if new_state.equals(self.cur_state):
            #If they are same states, no transition
            return False   

        self.tx_list.append((self.cur_state,new_state,self.tx_cond))
        self.cur_state=new_state

        #Reset measurement sequence
        self.tx_cond=MeasReportSeq()
        return True


class MobilityStateMachine:
    """
    A mobility policy inference model based on state machine

    The state machine is in following from
    state_machine={MeasState:{MeasState:MeasReportSeq,MeasState:MeasReportSeq,...},
                   MeasState:{HandoffState:MeasReportSeq,MeasState:MeasReportSeq,...}
                   ...}
    """
    def __init__(self):
        self.state_machine={} 

    def reset(self):
        """
        Reset the state machine (test purpose)
        """
        self.state_machine={}

    def load_state_machine(self,state_machine):
        """
        Load a state machine from a historical profile.
        WARNING: this method would also reset the current handoff state

        :param state_machine: the stored state machine
        :type state_machine: MobilityStateMachine
        :returns: True if succeeds, False otherwise
        """
        if state_machine.__class__.__name__!="MobilityStateMachine":
            return False
        self.state_machine=state_machine
        self.cur_state=None
        return True

    def update_state_machine(self,handoff_sample):
        """
        Update the state machine based on the new handoff sample

        :param handoff_sample: a new handoff sample
        :type handoff_sample: HandoffSample
        :returns: True if succeeds, or False otherwise
        """
        if handoff_sample.__class__.__name__!="HandoffSample":
            return False

        for item in handoff_sample.tx_list:
            self.__merge_transition(item)

        # self.dump()

    def predict_handoff(self, handoff_sample):
        """
        Based on current state and measseq, predict potential handoff

        :param handoff_sample: current handoff sample that includes current state and meas sequence
        :type handoff_sample: HandoffSample
        :returns: a handoffstate that indicates the prediction, or None if unpredictable
        """

        cur_state = handoff_state.cur_state
        cur_meas_seq = handoff_state.tx_cond

        equal_state = None

        #Note that states may not be identical but equivalent
        for state in self.state_machine:
            if state.equals(cur_state):
                equal_state = state
                break
        if not equal_state:
            return None #unknown state, unpredictable



    def __merge_transition(self,transition):
        """
        Merge a new state and its transition condition.
        This is the core function of the handoff rule inference

        :param transition: a tuple of (From_State,To_State,tx_cond)
        :type new_state: (MeasState,MeasState/HandoffState,MeasReportSeq)
        :returns: True if the addition succeeds, False otherwise.
        """
        from_state = transition[0]
        to_state = transition[1]
        tx_cond = transition[2]

        if from_state.__class__.__name__!="MeasState" \
        and to_state.__class__.__name__!="MeasState" \
        and to_state.__class__.__name__!="HandoffState" \
        and tx_cond.__class__.__name__!="MeasReportSeq":
            #Invalid input
            return False

        if not from_state:
            #First configuration: add to_state only
            if to_state not in self.state_machine:
                self.state_machine[to_state]={}
            return True
        else:
            if from_state not in self.state_machine:
                #Invalid input
                return False
            if to_state not in self.state_machine[from_state]:
                #Transition to a new state: this is the first sample
                self.state_machine[from_state][to_state]= tx_cond
            else:
                #the state has been observed before: merge the sequence
                self.state_machine[from_state][to_state].merge_seq(tx_cond)

            if to_state not in self.state_machine:
                #new state observed
                self.state_machine[to_state]={}
            return True 


    # def dump(self):
    #     print "State machine"
    #     for item in self.state_machine:
            
    #         for item2 in self.state_machine[item]:
    #             print item.__class__.__name__+"->" \
    #                 +item2.__class__.__name__+": " \
    #                 +str(self.state_machine[item][item2].meas_report_queue)
    #             print item.dump()
    #             print item2.dump()

    def dump(self):
        print("Handoff State Machine")
        for item in self.state_machine:
            for item2 in self.state_machine[item]:
                meas_report=""
                for report in self.state_machine[item][item2].meas_report_queue:
                    meas_report=meas_report+"("+str(report[0].freq)+","+str(report[1].event_list[0].type)+") "
                print(item.__class__.__name__+"->" \
                    +item2.__class__.__name__+": " \
                    +meas_report)
                print("From State:\n",item.dump())
                print("To State:\n",item2.dump())
############################################


# Helper modules
############################################
class LteMeasObjectEutra:
    """
    LTE Measurement object configuration
    """

    def __init__(self,measobj_id,freq,offset_freq, measureBandwidth):
        self.obj_id = measobj_id
        self.freq = freq # carrier frequency
        self.offset_freq = offset_freq # frequency-specific measurement offset
        self.cell_list = {} # cellID->cellIndividualOffset

        self.measBandwidth = measureBandwidth
        #TODO: add cell blacklist


    def equals(self,meas_obj):
        """
        Compare if this meas_obj is equal to another one

        :param meas_obj: a measurement object
        :type meas_obj: LteMeasObjectEutra
        :returns: True if they are equivalent, False otherwise
        """
        return meas_obj.__class__.__name__ == "LteMeasObjectEutra" \
        and self.freq == meas_obj.freq \
        and self.offset_freq == meas_obj.offset_freq \
        and self.cell_list == meas_obj.cell_list

    def add_cell(self,cell_id,cell_offset):
        """
        Add a cell individual offset

        :param cell_id: the cell identifier
        :type cell_id: int
        :param cell_offset: the cell individual offset
        :type cell_offset: int
        """
        self.cell_list[cell_id]=cell_offset

    def dump(self):
        """
        Report the cell's LTE measurement configurations

        :returns: a string that encodes the cell's LTE measurement configurations
        :rtype: string
        """
        # res = self.__class__.__name__+' '+str(self.obj_id)+' '\
        # +str(self.freq)+' '+ str(self.offset_freq)+'\n'
        res = (self.__class__.__name__
            + ' ' + str(self.obj_id)
            + ' ' + str(self.freq)
            + ' ' + str(self.offset_freq)
            +'\n')
        for item in self.cell_list:
            res += str(item) + ' ' + str(self.cell_list[item]) + '\n'
        return res


class LteReportConfig:
    """
    LTE measurement report configuration
    """
    def __init__(self,report_id=None,hyst=None):
        self.report_id = report_id
        self.hyst = hyst
        self.event_list = []

    def equals(self,report_config):
        """
        Compare the equivalence of two ReportConfig

        :param report_config: report configuration
        :types report_config: LteReportConfig
        :returns: True if they are equivalent, False otherwise
        """
        if report_config.__class__.__name__ != "LteReportConfig" \
        or self.hyst != report_config.hyst:
            return False
        for item in self.event_list:
            item_exist = False
            for item2 in report_config.event_list:
                if item.equals(item2):
                    item_exist = True
                    break
            if not item_exist:
                return False
        return True
    
    def add_event(self,event_type,threshold1,threshold2=None):
        """
        Add a measurement event

        :param event_type: a measurement type (r.f. 5.5.4, TS36.331)
        :type event_type: string
        :param threshold1: threshold 1
        :type threshold1: int
        :param threshold2: threshold 2
        :type threshold2: int
        """
        self.event_list.append(LteRportEvent(event_type,threshold1,threshold2))

    def dump(self):
        """
        Report the cell's measurement report configurations

        :returns: a string that encodes the cell's measurement report configurations
        :rtype: string
        """
        res = (self.__class__.__name__
            + ' ' + str(self.report_id)
            + ' ' + str(self.hyst) + '\n')
        for item in self.event_list:
            res += (str(item.type) 
                + ' ' + str(item.threshold1) 
                + ' ' + str(item.threshold2) + '\n')
        return res


class LteRportEvent:
    """
    Abstraction for LTE report event
    """
    def __init__(self,event_type,threshold1,threshold2=None):
        self.type = event_type
        self.threshold1 = threshold1
        self.threshold2 = threshold2    

    def equals(self,report_event):
        """
        Compare two report event

        :param report_event: a LTE report event configuration
        :type report_event: LteReportEvent
        :returns: True if they are equivalent, False otherwise
        """
        return report_event.__class__.__name__ == "LteRportEvent" \
        and self.type == report_event.type \
        and self.threshold1 ==  report_event.threshold1 \
        and self.threshold2 ==  report_event.threshold2
############################################
