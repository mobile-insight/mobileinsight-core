#!/usr/bin/python
# Filename: lte_rrc_analyzer.py
"""
A LTE RRC analyzer.

Author: Yuanjie Li
"""

import xml.etree.ElementTree as ET
from analyzer import *
import timeit

from profile import Profile,ProfileHierarchy

__all__=["LteRrcAnalyzer"]

#Q-offset range mapping (6.3.4, TS36.331)
q_offset_range = {
    0:-24, 1:-22, 2:-20, 3:-18, 4:-16, 5:-14,
    6:-12, 7:-10, 8:-8, 9:-6, 10:-5, 11:-4, 
    12:-3, 13:-2, 14:-1, 15:0, 16:1, 17:2, 
    18:3, 19:4, 20:5, 21:6, 22:8, 23:10, 24:12, 
    25:14, 26:16, 27:18, 28:20, 29:22, 30:24
}


class LteRrcAnalyzer(Analyzer):

    """
    A protocol ananlyzer for LTE Radio Resource Control (RRC) protocol.
    """
    def __init__(self):

        Analyzer.__init__(self)

        #init packet filters
        self.add_source_callback(self.__rrc_filter)

        #init internal states
        self.__status = LteRrcStatus()    # current cell status
        self.__history = {}    # cell history: timestamp -> LteRrcStatus()
        self.__config = {}    # (cell_id,freq) -> LteRrcConfig()
        # self.__config = Profile(LteRrcProfileHierarchy())

        self.__profile = Profile(LteRrcProfileHierarchy())

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE RRC messages.

        :param source: the trace source.
        :type source: trace collector
        """
        Analyzer.set_source(self,source)
        #enable LTE RRC log
        source.enable_log("LTE_RRC_OTA_Packet")
        source.enable_log("LTE_RRC_Serv_Cell_Info_Log_Packet")

    def __rrc_filter(self,msg):

        """
        Filter all LTE RRC packets, and call functions to process it

        :param msg: the event (message) from the trace collector.
        """

        # log_item = msg.data
        log_item = msg.data.decode()
        log_item_dict = dict(log_item)

        #Convert msg to dictionary format
        raw_msg = Event(msg.timestamp,msg.type_id,log_item_dict)
        self.__callback_serv_cell(raw_msg)

        if 'Msg' not in log_item_dict:
            return
        
        #Convert msg to xml format
        log_xml = ET.fromstring(log_item_dict['Msg'])
        xml_msg = Event(msg.timestamp,msg.type_id,log_xml)

        # Calllbacks triggering

        if msg.type_id == "LTE_RRC_OTA_Packet":    
            self.__callback_rrc_conn(xml_msg)
            self.__callback_sib_config(xml_msg)
            self.__callback_rrc_reconfig(xml_msg)
            #TODO: callback RRC

        # Raise event to other analyzers
        # FIXME: the timestamp is incoherent with that from the trace collector
        e = Event(timeit.default_timer(),self.__class__.__name__,"")
        self.send(e)

    def __callback_serv_cell(self,msg):

        """
        A callback to update current cell status

        :param msg: the RRC messages with cell status
        """
        if not self.__status.inited():
            if 'Freq' in msg.data:
                self.__status.freq = msg.data['Freq']
                self.logger.info(self.__status.dump())
            if 'Physical Cell ID' in msg.data:
                self.__status.id = msg.data['Physical Cell ID']
                self.logger.info(self.__status.dump())
        else:
            if 'Freq' in msg.data and self.__status.freq != msg.data['Freq']:
                self.__status = LteRrcStatus()
                self.__status.freq = msg.data['Freq']
                self.__history[msg.timestamp] = self.__status
                self.logger.info(self.__status.dump())
            if 'Physical Cell ID' in msg.data and self.__status.id != msg.data['Physical Cell ID']:
                self.__status = LteRrcStatus()
                self.__status.id = msg.data['Physical Cell ID']
                self.__history[msg.timestamp] = self.__status
                self.logger.info(self.__status.dump())

    def __callback_sib_config(self,msg):
        """
        A callback to extract configurations from System Information Blocks (SIBs), 
        including the radio asssement thresholds, the preference settings, etc.

        :param msg: RRC SIB messages
        """

        for field in msg.data.iter('field'):

            #TODO: use MIB, not lte-rrc.trackingAreaCode
            if field.get('name') == "lte-rrc.trackingAreaCode": #tracking area code
                self.__status.tac = field.get('show')

            #serving cell and intra-frequency reselection info
            if field.get('name') == "lte-rrc.sib3_element":
                field_val = {}

                #Default value setting
                #FIXME: set default to those in TS36.331
                field_val['lte-rrc.cellReselectionPriority'] = 0 #mandatory
                field_val['lte-rrc.threshServingLow'] = None #mandatory
                field_val['lte-rrc.s_NonIntraSearch'] = "inf"
                field_val['lte-rrc.q_Hyst'] = 0
                field_val['lte-rrc.q_RxLevMin'] = None #mandatory
                field_val['lte-rrc.p_Max'] = 23 #default value for UE category 3
                field_val['lte-rrc.s_IntraSearch'] = "inf"
                field_val['lte-rrc.t_ReselectionEUTRA'] = None

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                cur_pair = (self.__status.id,self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = LteRrcConfig()
                    self.__config[cur_pair].status=self.__status

                self.__config[cur_pair].sib.serv_config = LteRrcSibServ(
                    int(field_val['lte-rrc.cellReselectionPriority']),
                    int(field_val['lte-rrc.threshServingLow'])*2,
                    float(field_val['lte-rrc.s_NonIntraSearch'])*2,
                    int(field_val['lte-rrc.q_Hyst']))

                #Test profile
                self.__profile.update("LteRrcProfile:"+str(cur_pair)+".idle.serv_config",
                	{'priority':field_val['lte-rrc.cellReselectionPriority'],
                	 'threshserv_low':str(int(field_val['lte-rrc.threshServingLow'])*2),
                	 's_nonintrasearch':str(float(field_val['lte-rrc.s_NonIntraSearch'])*2)),
                     'q_hyst':field_val['lte-rrc.q_Hyst']})

                self.__config[cur_pair].sib.intra_freq_config = LteRrcSibIntraFreqConfig(
                    int(field_val['lte-rrc.t_ReselectionEUTRA']),
                    int(field_val['lte-rrc.q_RxLevMin'])*2,
                    int(field_val['lte-rrc.p_Max']),
                    float(field_val['lte-rrc.s_IntraSearch'])*2) 

                #Test profile
                self.__profile.update("LteRrcProfile:"+str(cur_pair)+".idle.intra_freq_config",
                	{'tReselection':field_val['lte-rrc.t_ReselectionEUTRA'],
                	 'q_RxLevMin':str(int(field_val['lte-rrc.q_RxLevMin'])*2),
                	 'p_Max':field_val['lte-rrc.p_Max'],
                	 's_IntraSearch':str(float(field_val['lte-rrc.s_IntraSearch'])*2)})

            #inter-frequency (LTE)
            if field.get('name') == "lte-rrc.interFreqCarrierFreqList":
                field_val = {}

                #FIXME: set to the default value based on TS36.331
                field_val['lte-rrc.dl_CarrierFreq'] = None #mandatory
                field_val['lte-rrc.t_ReselectionEUTRA'] = None #mandatory
                field_val['lte-rrc.q_RxLevMin'] = None #mandatory
                field_val['lte-rrc.p_Max'] = 23 #optional, r.f. 36.101
                field_val['lte-rrc.cellReselectionPriority'] = 0 #mandatory
                field_val['lte-rrc.threshX_High'] = None #mandatory
                field_val['lte-rrc.threshX_Low'] = None #mandatory
                field_val['lte-rrc.q_OffsetFreq'] = 0

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                cur_pair=(self.__status.id,self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = LteRrcConfig()
                    self.__config[cur_pair].status = self.__status

                neighbor_freq=int(field_val['lte-rrc.dl_CarrierFreq'])
                self.__config[cur_pair].sib.inter_freq_config[neighbor_freq] = LteRrcSibInterFreqConfig(
                        "LTE",
                        neighbor_freq,
                        int(field_val['lte-rrc.t_ReselectionEUTRA']),
                        int(field_val['lte-rrc.q_RxLevMin'])*2,
                        int(field_val['lte-rrc.p_Max']),
                        int(field_val['lte-rrc.cellReselectionPriority']),
                        int(field_val['lte-rrc.threshX_High'])*2,
                        int(field_val['lte-rrc.threshX_Low'])*2,
                        int(field_val['lte-rrc.q_OffsetFreq']))

                #2nd round: inter-freq cell individual offset
                for val in field.iter('field'):
                    if val.get('name') == "lte-rrc.InterFreqNeighCellInfo_element":
                        field_val2 = {}

                        field_val2['lte-rrc.physCellId'] = None #mandatory
                        field_val2['lte-rrc.q_OffsetCell'] = None #mandatory

                        for val2 in field.iter('field'):
                            field_val2[val2.get('name')] = val2.get('show')

                        cell_id = int(field_val2['lte-rrc.physCellId'])
                        offset = int(field_val2['lte-rrc.q_OffsetCell'])
                        offset_pair = (cell_id,neighbor_freq)
                        self.__config[cur_pair].sib.inter_freq_cell_config[offset_pair] = q_offset_range[int(offset)]


            #inter-RAT (UTRA)
            if field.get('name') == "lte-rrc.CarrierFreqUTRA_FDD_element":
                field_val = {}

                #Default value setting
                #FIXME: set to default based on TS25.331
                field_val['lte-rrc.carrierFreq'] = None #mandatory
                field_val['lte-rrc.q_RxLevMin'] = None #mandatory
                field_val['lte-rrc.p_MaxUTRA'] = None #mandatory
                field_val['lte-rrc.cellReselectionPriority'] = 0 #mandatory
                field_val['lte-rrc.threshX_High'] = None #mandatory
                field_val['lte-rrc.threshX_High'] = None #mandatory

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                cur_pair = (self.__status.id,self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = LteRrcConfig()
                    self.__config[cur_pair].status = self.__status

                neighbor_freq=int(field_val['lte-rrc.carrierFreq']) 
                self.__config[cur_pair].sib.inter_freq_config[neighbor_freq] = LteRrcSibInterFreqConfig(
                        "UTRA",
                        neighbor_freq,
                        None,    #For 3G, tReselection is not in this IE
                        int(field_val['lte-rrc.q_RxLevMin'])*2,
                        int(field_val['lte-rrc.p_MaxUTRA']),
                        int(field_val['lte-rrc.cellReselectionPriority']),
                        int(field_val['lte-rrc.threshX_High'])*2,
                        int(field_val['lte-rrc.threshX_Low'])*2,
                        0)    #inter-RAT has no freq-offset

            if field.get('name') == "lte-rrc.t_ReselectionUTRA":
                cur_pair = (self.__status.id,self.__status.freq)
                for config in self.__config[cur_pair].sib.inter_freq_config.itervalues():
                    if config.rat == "UTRA":
                        config.tReselection = float(field.get('show'))

            #TODO: inter-RAT (GERAN): lte-rrc.CarrierFreqsInfoGERAN_element
            if field.get('name') == "lte-rrc.CarrierFreqsInfoGERAN_element":
                field_val = {}

                #Default value setting
                #FIXME: set to default based on TS25.331
                field_val['lte-rrc.startingARFCN'] = None #mandatory
                field_val['lte-rrc.q_RxLevMin'] = None #mandatory
                field_val['lte-rrc.lte-rrc.p_MaxGERAN'] = None #mandatory
                field_val['lte-rrc.cellReselectionPriority'] = 0 #mandatory
                field_val['lte-rrc.threshX_High'] = None #mandatory
                field_val['lte-rrc.threshX_High'] = None #mandatory

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                cur_pair = (self.__status.id,self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = LteRrcConfig()
                    self.__config[cur_pair].status = self.__status

                neighbor_freq = int(field_val['lte-rrc.startingARFCN']) 
                self.__config[cur_pair].sib.inter_freq_config[neighbor_freq] = LteRrcSibInterFreqConfig(
                        "GERAN",
                        neighbor_freq,
                        None,    #For 3G, tReselection is not in this IE
                        int(field_val['lte-rrc.q_RxLevMin'])*2,
                        int(field_val['lte-rrc.p_MaxGERAN']),
                        int(field_val['lte-rrc.cellReselectionPriority']),
                        int(field_val['lte-rrc.threshX_High'])*2,
                        int(field_val['lte-rrc.threshX_Low'])*2,
                        0)    #inter-RAT has no freq-offset

            #FIXME: t_ReselectionGERAN appears BEFORE config, so this code does not work!
            if field.get('name') == "lte-rrc.t_ReselectionGERAN":
                cur_pair = (self.__status.id,self.__status.freq)
                for config in self.__config[cur_pair].sib.inter_freq_config.itervalues():
                    if config.rat == "GERAN":
                        config.tReselection = float(field.get('show'))


            #intra-frequency cell offset
            if field.get('name') == "lte-rrc.IntraFreqNeighCellInfo_element":
                field_val = {}

                field_val['lte-rrc.physCellId'] = None #mandatory
                field_val['lte-rrc.q_OffsetCell'] = None #mandatory

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                cur_pair = (self.__status.id,self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = LteRrcConfig()
                    self.__config[cur_pair].status = self.__status

                cell_id = int(field_val['lte-rrc.physCellId'])
                offset = int(field_val['lte-rrc.q_OffsetCell'])
                self.__config[cur_pair].sib.intra_freq_cell_config[cell_id] = q_offset_range[int(offset)]

            #TODO: RRC connection status update

    def __callback_rrc_reconfig(self,msg):
        
        """
        Extract configurations from RRCReconfiguration Message, 
        including the measurement profiles, the MAC/RLC/PDCP configurations, etc.

        :param msg: LTE RRC reconfiguration messages
        """

        #TODO: optimize code to handle objects/config under the same ID
        measobj_id = -1
        report_id = -1

        for field in msg.data.iter('field'):

            if field.get('name') == "lte-rrc.measObjectId":
                measobj_id = field.get('show')

            if field.get('name') == "lte-rrc.reportConfigId":
                report_id = field.get('show')



            #Add a LTE measurement object
            if field.get('name') == "lte-rrc.measObjectEUTRA_element": 
                field_val = {}

                field_val['lte-rrc.carrierFreq'] = None
                field_val['lte-rrc.offsetFreq'] = 0

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                cur_pair = (self.__status.id,self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = LteRrcConfig()
                    self.__config[cur_pair].status = self.__status

                freq = int(field_val['lte-rrc.carrierFreq'])
                offsetFreq = int(field_val['lte-rrc.offsetFreq'])
                self.__config[cur_pair].active.measobj[freq] = LteMeasObjectEutra(measobj_id,freq,offsetFreq)

                #2nd round: handle cell individual offset
                for val in field.iter('field'):
                    if val.get('name') == 'lte-rrc.CellsToAddMod_element':
                        cell_val = {}
                        for item in val.iter('field'):
                            cell_val[item.get('name')] = item.get('show')

                        if 'lte-rrc.physCellId' in cell_val:
                            cell_id = int(cell_val['lte-rrc.physCellId'])
                            cell_offset = q_offset_range[int(cell_val['lte-rrc.cellIndividualOffset'])]
                            self.__config[cur_pair].active.measobj[freq].add_cell(cell_id,cell_offset)

            #Add a UTRA (3G) measurement object:
            if field.get('name') == "lte-rrc.measObjectUTRA_element":
                field_val = {}

                field_val['lte-rrc.carrierFreq'] = None
                field_val['lte-rrc.offsetFreq'] = 0

                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                cur_pair = (self.__status.id,self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = LteRrcConfig()
                    self.__config[cur_pair].status = self.__status

                freq = int(field_val['lte-rrc.carrierFreq'])
                offsetFreq = int(field_val['lte-rrc.offsetFreq'])
                self.__config[cur_pair].active.measobj[freq] = LteMeasObjectUtra(measobj_id,freq,offsetFreq)


            #Add a LTE report configuration
            if field.get('name') == "lte-rrc.reportConfigEUTRA_element":

                cur_pair = (self.__status.id,self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = LteRrcConfig()
                    self.__config[cur_pair].status = self.__status

                hyst = 0
                for val in field.iter('field'):
                    if val.get('name') == 'lte-rrc.hysteresis':
                        hyst = int(val.get('show'))

                report_config = LteReportConfig(report_id,hyst/2)

                for val in field.iter('field'):

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
                                        threshold1 = int(item.get('show'))-140
                                        break
                                    if item2.get('name') == 'lte-rrc.threshold_RSRQ':
                                        threshold1 = (int(item.get('show'))-40)/2
                                        break
                            if item.get('name') == 'lte-rrc.a5_Threshold2':
                                for item2 in item.iter('field'):
                                    if item2.get('name') == 'lte-rrc.threshold_RSRP':
                                        threshold2 = int(item.get('show'))-140
                                        break
                                    if item2.get('name') == 'lte-rrc.threshold_RSRQ':
                                        threshold2 = (int(item.get('show'))-40)/2
                                        break
                        report_config.add_event('a5',threshold1,threshold2)

                    if val.get('name') == 'lte-rrc.eventB2_element':

                        threshold1 = None
                        threshold2 = None
                        for item in val.iter('field'):
                            if item.get('name') == 'lte-rrc.b2_Threshold1':
                                for item2 in item.iter('field'):
                                    if item2.get('name') == 'lte-rrc.threshold_RSRP':
                                        threshold1 = int(item.get('show'))-140
                                        break
                                    if item2.get('name') == 'lte-rrc.threshold_RSRQ':
                                        threshold1 = (int(item.get('show'))-40)/2
                                        break
                            if item.get('name') == 'lte-rrc.b2_Threshold2':
                                for item2 in item.iter('field'):
                                    if item2.get('name') == 'lte-rrc.threshold_RSRP':
                                        threshold2 = int(item.get('show'))-140
                                        break
                                    if item2.get('name') == 'lte-rrc.threshold_RSRQ':
                                        threshold2 = (int(item.get('show'))-40)/2
                                        break
                                    if item2.get('name') == 'lte-rrc.utra_RSCP':
                                        threshold2 = int(item.get('show'))-115
                                        break
                        report_config.add_event('b2',threshold1,threshold2)


                self.__config[cur_pair].active.report_list[report_id] = report_config


            #Add a 2G/3G report configuration
            if field.get('name') == "lte-rrc.reportConfigInterRAT_element":

                cur_pair = (self.__status.id,self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = LteRrcConfig()
                    self.__config[cur_pair].status = self.__status

                hyst=0
                for val in field.iter('field'):
                    if val.get('name') == 'lte-rrc.hysteresis':
                        hyst = int(val.get('show'))

                report_config = LteReportConfig(report_id,hyst/2)

                for val in field.iter('field'):

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
                                        threshold1 = int(item.get('show'))-140
                                        break
                                    if item2.get('name') == 'lte-rrc.threshold_RSRQ':
                                        threshold1 = (int(item.get('show'))-40)/2
                                        break
                            if item.get('name') == 'lte-rrc.b2_Threshold2':
                                for item2 in item.iter('field'):
                                    if item2.get('name') == 'lte-rrc.threshold_RSRP':
                                        threshold2 = int(item.get('show'))-140
                                        break
                                    if item2.get('name') == 'lte-rrc.threshold_RSRQ':
                                        threshold2 = (int(item.get('show'))-40)/2
                                        break
                                    if item2.get('name') == 'lte-rrc.utra_RSCP':
                                        threshold2 = int(item.get('show'))-115
                                        break
                        report_config.add_event('b2',threshold1,threshold2)


                self.__config[cur_pair].active.report_list[report_id] = report_config

            #Add a LTE measurement report config
            if field.get('name') == "lte-rrc.MeasIdToAddMod_element":
                field_val = {}
                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                cur_pair = (self.__status.id,self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = LteRrcConfig()
                    self.__config[cur_pair].status = self.__status

                meas_id = int(field_val['lte-rrc.measId'])
                obj_id = int(field_val['lte-rrc.measObjectId'])
                config_id = int(field_val['lte-rrc.reportConfigId'])
                self.__config[cur_pair].active.measid_list[meas_id] = (obj_id,config_id)

    def __callback_rrc_conn(self,msg):
        """
        Update RRC connectivity status

        :param msg: the RRC message
        """
        for field in msg.data.iter('field'):
            if field.get('name') == "lte-rrc.rrcConnectionSetupComplete_element":
                self.__status.conn = True
                self.logger.info(self.__status.dump())

            if field.get('name') == "lte-rrc.rrcConnectionRelease_element":
                self.__status.conn = False
                self.logger.info(self.__status.dump())

    def get_cell_list(self):
        """
        Get a complete list of cell IDs.

        :returns: a list of cells the device has associated with
        """
        #FIXME: currently only return *all* cells in the LteRrcConfig
        return self.__config.keys()

    def get_cell_config(self,cell):
        """
        Return a cell's active/idle-state configuration.
        
        :param cell:  a cell identifier
        :type cell: a (cell_id,freq) pair
        :returns: this cell's active/idle-state configurations
        :rtype: LteRrcConfig
        """
        if cell in self.__config:
            return self.__config[cell]
        else:
            return None

    def get_cur_cell(self):
        """
        Get current cell's status

        :returns: current cell's status
        :rtype: LteRrcStatus      
        """
        return self.__status

    def get_cur_cell_config(self):
        """
        Get current cell's configuration

        :returns: current cell's status
        :rtype: LteRrcConfig
        """
        cur_pair = (self.__status.id,self.__status.freq)
        if cur_pair in self.__config:
            return self.__config[cur_pair]
        else:
            return None

    def get_mobility_history(self):
        """
        Get the history of cells the device associates with

        :returns: the cells the device has traversed
        :rtype: a dictionary of timestamp -> LteRrcStatus
        """
        return self.__history


def LteRrcProfileHierarchy():
    '''
    Return a Lte Rrc ProfileHierarchy (configurations)

    :returns: ProfileHierarchy for LTE RRC
    '''

    profile_hierarchy = ProfileHierarchy('LteRrcProfile')
    root = profile_hierarchy.get_root()
    status = root.add('status',False) #metadata
    sib = root.add('idle',False) #Idle-state configurations
    active = root.add('active',False) #Active-state configurations

    #Status metadata
    status.add('cell_id',False)
    status.add('freq',False)
    status.add('radio_technology',False)
    status.add('tracking_area_code',False)
    status.add('bandwidth',False)
    status.add('conn_state',False)

    #Idle-state configurations
    sib_serv = sib.add('serv_config',False) #configuration as the serving cell
    #Per-frequency configurations
    intra_freq_config = sib.add('intra_freq_config',False) #Intra-frequency handoff config
    #TODO: for inter-freq/RAT, should have a mapping from freq/RAT to config
    inter_freq_config = sib.add('inter_freq_config',True) #Inter-frequency/RAT handoff config
    intra_freq_cell_config = sib.add('intra_freq_cell_config',True) #per-cell offsets for intra-freq
    inter_freq_cell_config = sib.add('inter_freq_cell_config',True) #per-cell offsets for inter-freq

    sib_serv.add('priority',False) #cell reselection priority
    sib_serv.add('threshserv_low',False) #cell reselection threshold
    sib_serv.add('s_nonintrasearch',False) #threshold for searching other frequencies
    sib_serv.add('q_hyst',False)

    #Intra-frequency handoff parameter: frequency level
    intra_freq_config.add('tReselection',False)
    intra_freq_config.add('q_RxLevMin',False)
    intra_freq_config.add('p_Max',False)
    intra_freq_config.add('s_IntraSearch',False)

    #Inter-frequency handoff parameter: frequency level
    inter_freq_config.add('rat',False)
    inter_freq_config.add('freq',False)
    inter_freq_config.add('tReselection',False)
    inter_freq_config.add('q_RxLevMin',False)
    inter_freq_config.add('p_Max',False)
    inter_freq_config.add('priority',False)
    inter_freq_config.add('threshx_high',False)
    inter_freq_config.add('threshx_low',False)
    inter_freq_config.add('q_offset_freq',False)

    #Intra/inter-frequency parameter: per-cell level
    intra_freq_cell_config.add('offset',False)
    inter_freq_cell_config.add('offset',False)

    #Active-state configuration
    meas_obj = active.add('meas_obj',True) #freq->measobject
    report_list = active.add('report_list',True) #report_id->reportConfig
    measid_list = active.add('measid_list',True) #meas_id->(obj_id,report_id)

    #measurement object
    meas_obj.add('obj_id',False) #meas object ID
    meas_obj.add('freq',False) # carrier frequency
    meas_obj.add('offset_freq',False) # frequency-specific measurement offset
    individual_offset = meas_obj.add('offset',True) # cellID->cellIndividualOffset
    individual_offset.add('offset',False) 
    #TODO: add cell blacklist

    report_list.add('id',False) #report ID
    report_list.add('hyst',False) #Hysteresis
    event = report_list.add('report_event',True) #report event: eventID->thresholds
    event.add('event_type',False)
    event.add('threshold_1',False)
    event.add('threshold_2',False)


    #measurement id
    measid_list.add('obj_id',False)
    measid_list.add('report_id',False)

    return profile_hierarchy


class LteRrcStatus:
    """
    The metadata of a cell, including its ID, frequency band, tracking area code, 
    bandwidth, connectivity status, etc.
    """
    def __init__(self):
        self.id = None #cell ID
        self.freq = None #cell frequency
        self.rat = "LTE" #radio technology
        self.tac = None #tracking area code
        self.bandwidth = None #cell bandwidth
        self.conn = False #connectivity status (for serving cell only)

    def dump(self):
        """
        Report the cell status

        :returns: a string that encodes the cell status
        :rtype: string
        """
        return (self.__class__.__name__
            + " cellID=" + str(self.id)
            + " frequency=" + str(self.freq)
            + " TAC=" + str(self.tac)
            + " connected=" + str(self.conn) + '\n')

    def inited(self):
        # return (self.id!=None and self.freq!=None)
        return (self.id and self.freq)


class LteRrcConfig:
    """ 
    Per-cell RRC configurations

    The following configurations are supported
        - Idle-state
            - Cell reselection parameters
        - Active-state
            - PHY/MAC/PDCP/RLC configuration
            - Measurement configurations
    """

    # Update in 2.0: query and storage with hierarchical name
    
    def __init__(self):
        self.status = LteRrcStatus() #the metadata of this cell
        self.status.rat = "LTE"
        self.sib = LteRrcSib()    #Idle-state: cellID->LTE_RRC_SIB_CELL
        self.active = LteRrcActive() #active-state configurations

    def dump(self):
        """
        Report the cell configurations

        :returns: a string that encodes the cell's configurations
        :rtype: string
        """

        return (self.__class__.__name__ + '\n'
            + self.status.dump()
            + self.sib.dump()
            + self.active.dump() )

    def get_cell_reselection_config(self,cell_meta):
        """
        Given a cell, return its reselection config as a serving cell

        :param cell_meta: a cell identifier
        :type cell_meta: a (cell_id,freq) pair

        :returns: cell reselection configurations
        :rtype: LteRrcReselectionConfig
        """
        # if cell_meta == None:
        if not cell_meta:
            return None
        cell = cell_meta.id
        freq = cell_meta.freq
        if freq == self.status.freq:
            #intra-frequency 
            offset = self.sib.serv_config.q_hyst
            if cell in self.sib.intra_freq_cell_config:
                offset += self.sib.intra_freq_cell_config[cell]
            # return LteRrcReselectionConfig(cell,freq,self.sib.serv_config.priority, \
            #     offset,None,None,self.sib.serv_config.threshserv_low)
            return LteRrcReselectionConfig(cell,freq,self.sib.serv_config.priority,
                offset,None,None,self.sib.serv_config.threshserv_low)
        else:
            #inter-frequency/RAT
            if freq not in self.sib.inter_freq_config:
                return None
            freq_config = self.sib.inter_freq_config[freq]
            hyst = self.sib.serv_config.q_hyst
            offset_cell = 0
            if cell in self.sib.inter_freq_cell_config:
                offset_cell = self.sib.inter_freq_cell_config[cell]
            # return LteRrcReselectionConfig(cell,freq,freq_config.priority,\
            #     freq_config.q_offset_freq+offset_cell+hyst, \
            #     freq_config.threshx_high,freq_config.threshx_low, \
            #     self.sib.serv_config.threshserv_low)
            return LteRrcReselectionConfig(cell,freq,freq_config.priority,
                freq_config.q_offset_freq+offset_cell+hyst,
                freq_config.threshx_high,freq_config.threshx_low,
                self.sib.serv_config.threshserv_low)

    def get_meas_config(self,cell_meta):

        """
        Given a cell, return its measurement config from the serving cell.
        Note: there may be more than 1 measurement configuration for the same cell.

        :param cell_meta: a cell identifier
        :type cell_meta: a (cell_id,freq) pair
        :returns: RRC measurement configurations
        :rtype: a list of LteRrcReselectionConfig
        """

        #FIXME: this is NOT a generic function
        # if cell_meta==None:
        if not cell_meta:
            return None
        cell = cell_meta.id
        freq = cell_meta.freq

        if freq not in self.active.measobj:
            return None

        obj_id = self.active.measobj[freq].obj_id
        config_id_list = []

        #Find the corresponding report conditions
        for item in self.active.measid_list.itervalues():
            if item[0]==obj_id:
                config_id_list.append(item[1])

        if not config_id_list:
            return None

        #For each configuration, we convert it to an equivalent reselection form
        res=[]
        for config_id in config_id_list:
            if config_id in self.active.report_list:
                hyst=self.active.report_list[config_id].hyst
                for item in self.active.report_list[config_id].event_list:
                    if item.type=="a1":
                        #equivalent to high-priority reselection
                        priority=self.sib.serv_config.priority+1
                        threshX_High=item.threshold1+hyst
                        # res.append(LteRrcReselectionConfig(cell,freq,priority, \
                        #     None,threshX_High,None,self.sib.serv_config.threshserv_low))
                        res.append(LteRrcReselectionConfig(cell,freq,priority,
                            None,threshX_High,None,self.sib.serv_config.threshserv_low))
                    if item.type=="a2":
                        pass
                    if item.type=="a3":
                        #equivalent to equal-priority reselection
                        priority=self.sib.serv_config.priority
                        offset=item.threshold1+hyst-self.active.measobj[freq].offset_freq
                        if cell in self.active.measobj[freq].cell_list[freq]:
                            offset-=self.active.measobj[freq].cell_list[cell]
                        # res.append(LteRrcReselectionConfig(cell,freq,priority, \
                        #     offset,None,None,self.sib.serv_config.threshserv_low))
                        res.append(LteRrcReselectionConfig(cell,freq,priority,
                            offset,None,None,self.sib.serv_config.threshserv_low))
                    if item.type=="a4":
                        #equivalent to high-priority reselection
                        priority=self.sib.serv_config.priority+1
                        threshX_High=item.threshold1+hyst-self.active.measobj[freq].offset_freq
                        if cell in self.active.measobj[freq].cell_list[freq]:
                            threshX_High-=self.active.measobj[freq].cell_list[cell]
                        # res.append(LteRrcReselectionConfig(cell,freq,priority,None, \
                        #     threshX_High,None,self.sib.serv_config.threshserv_low))
                        res.append(LteRrcReselectionConfig(cell,freq,priority,None,
                            threshX_High,None,self.sib.serv_config.threshserv_low))
                    if item.type=="a5":
                        #equivalent o low-priority reselection
                        priority=self.sib.serv_config.priority-1
                        #TODO: add thresh_serv. Currently use offset
                        threshserv_low=item.threshold1-hyst
                        threshX_Low=item.threshold2+hyst-self.active.measobj[freq].offset_freq
                        if cell in self.active.measobj[freq].cell_list[freq]:
                            threshX_Low-=self.active.measobj[freq].cell_list[cell]
                        res.append(LteRrcReselectionConfig(cell,freq,priority,None, 
                            threshX_Low,threshserv_low))

                    if item.type=="b2":
                        #equivalent o low-priority reselection
                        priority=self.sib.serv_config.priority-1
                        #TODO: add thresh_serv. Currently use offset
                        threshserv_low=item.threshold1-hyst
                        threshX_Low=item.threshold2+hyst-self.active.measobj[freq].offset_freq
                        res.append(LteRrcReselectionConfig(cell,freq,priority,None, 
                            threshX_Low,threshserv_low))
        return res


class LteRrcSib:

    """
    Per-cell Idle-state SIB configurations
    """
    def __init__(self):
        #FIXME: init based on the default value in TS36.331
        #configuration as a serving cell (LteRrcSibServ)
        self.serv_config = LteRrcSibServ(7,0,float('inf'),0) 

        #Per-frequency configurations
        #Intra-freq reselection config
        self.intra_freq_config = LteRrcSibIntraFreqConfig(None,None,None,None) 
        #Inter-freq/RAT reselection config. Freq -> LteRrcSibInterFreqConfig
        self.inter_freq_config = {}  

        #TODO: add intra_cell_config and inter_cell config, which maps individual cell offset
        self.intra_freq_cell_config = {} # cell -> offset
        self.inter_freq_cell_config = {} # cell -> offset

    def dump(self):
        """
        Report the cell SIB configurations

        :returns: a string that encodes the cell's SIB configurations
        :rtype: string
        """
        res = self.serv_config.dump() + self.intra_freq_config.dump()
        for item in self.inter_freq_config:
            res += self.inter_freq_config[item].dump()
        for item in self.intra_freq_cell_config:
            res += ("Intra-freq offset: " + str(item) + ' '
                + str(self.intra_freq_cell_config[item]) + '\n')
        for item in self.inter_freq_cell_config:
            res += ("Inter-freq offset: " + str(item) + ' '
                + str(self.inter_freq_cell_config[item]) + '\n')
        return res


class LteRrcReselectionConfig:
    """
    Per-cell cell reselection configurations
    """
    def __init__(self,cell_id,freq,priority,offset,threshX_High,threshX_Low,threshserv_low):
        self.id = cell_id
        self.freq = freq
        self.priority = priority
        self.offset = offset #adjusted offset by considering freq/cell-specific offsets
        self.threshx_high = threshX_High
        self.threshx_low = threshX_Low
        self.threshserv_low = threshserv_low


class LteRrcSibServ:
    """
    Serving cell's SIB configurations
    """
    def __init__(self,priority,thresh_serv, s_nonintrasearch,q_hyst):
        self.priority = priority #cell reselection priority
        self.threshserv_low = thresh_serv #cell reselection threshold
        self.s_nonintrasearch = s_nonintrasearch #threshold for searching other frequencies
        self.q_hyst = q_hyst

    def dump(self):
        """
        Report the serving cell SIB configurations

        :returns: a string that encodes the cell's SIB configurations
        :rtype: string
        """
        # return self.__class__.__name__ + ' ' + str(self.priority) + ' ' \
        # + str(self.threshserv_low) + ' ' + str(self.s_nonintrasearch) + ' '\
        # + str(self.q_hyst) + '\n'
        return (self.__class__.__name__
            + ' ' + str(self.priority)
            + ' ' + str(self.threshserv_low)
            + ' ' + str(self.s_nonintrasearch)
            + ' ' + str(self.q_hyst) + '\n')


class LteRrcSibIntraFreqConfig:
    """
    Intra-frequency SIB configurations
    """
    def __init__(self,tReselection,q_RxLevMin,p_Max,s_IntraSearch):
        #FIXME: individual cell offset
        self.tReselection = tReselection
        self.q_RxLevMin = q_RxLevMin
        self.p_Max=p_Max
        self.s_IntraSearch = s_IntraSearch

    def dump(self):
        """
        Report the cell SIB configurations

        :returns: a string that encodes the cell's SIB configurations
        :rtype: string
        """
        # return self.__class__.__name__ + ' ' + str(self.tReselection) + ' ' \
        # + str(self.q_RxLevMin) + ' ' + str(self.p_Max) + ' ' + str(self.s_IntraSearch) + '\n'
        return (self.__class__.__name__
            + ' ' + str(self.tReselection)
            + ' ' + str(self.q_RxLevMin)
            + ' ' + str(self.p_Max)
            + ' ' + str(self.s_IntraSearch) + '\n')


class LteRrcSibInterFreqConfig:
    """
    Inter-frequency SIB configurations
    """    
    #FIXME: the current list is incomplete
    #FIXME: individual cell offset
    def __init__(self,rat,freq,tReselection,q_RxLevMin,p_Max,priority,threshx_high,threshx_low,q_offset_freq):
        self.rat = rat
        self.freq = freq
        self.tReselection = tReselection
        self.q_RxLevMin = q_RxLevMin
        self.p_Max = p_Max
        self.priority = priority
        self.threshx_high = threshx_high
        self.threshx_low = threshx_low
        self.q_offset_freq = q_offset_freq

    def dump(self):
        """
        Report the cell SIB configurations

        :returns: a string that encodes the cell's SIB configurations
        :rtype: string
        """
        # return self.__class__.__name__ +' '+str(self.rat)+' '\
        # +str(self.freq)+' '+str(self.tReselection)+' '\
        # +str(self.q_RxLevMin)+' '+str(self.p_Max)+' '+str(self.priority)+' '\
        # +str(self.threshx_high)+' '+str(self.threshx_low)+'\n'
        return (self.__class__.__name__
            + ' ' + str(self.rat)
            + ' ' + str(self.freq)
            + ' ' + str(self.tReselection)
            + ' ' + str(self.q_RxLevMin)
            + ' ' + str(self.p_Max)
            + ' ' + str(self.priority)
            + ' ' + str(self.threshx_high)
            + ' ' + str(self.threshx_low) + '\n')


class LteRrcActive:
    """
    RRC active-state configurations (from RRCReconfiguration messsage)
    """
    def __init__(self):
        #TODO: initialize some containers
        self.measobj = {} #freq->measobject
        self.report_list = {} #report_id->reportConfig
        self.measid_list = {} #meas_id->(obj_id,report_id)

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


class LteMeasObjectEutra:
    """
    LTE Measurement object configuration
    """

    def __init__(self,measobj_id,freq,offset_freq):
        self.obj_id = measobj_id
        self.freq = freq # carrier frequency
        self.offset_freq = offset_freq # frequency-specific measurement offset
        self.cell_list = {} # cellID->cellIndividualOffset
        #TODO: add cell blacklist

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


class LteMeasObjectUtra:
    """
    3G Measurement object configuration
    """

    def __init__(self,measobj_id,freq,offset_freq):
        self.obj_id = measobj_id
        self.freq = freq # carrier frequency
        self.offset_freq = offset_freq # frequency-specific measurement offset
        #TODO: add cell list
        
    def dump(self):
        """
        Report the cell's 3G measurement configurations

        :returns: a string that encodes the cell's 3G measurement configurations
        :rtype: string
        """
        # return self.__class__.__name__+' '+str(self.obj_id)+' '\
        # +str(self.freq,self.offset_freq)+'\n'
        return (self.__class__.__name__
            + ' ' + str(self.obj_id)
            + ' ' + str(self.freq,self.offset_freq) + '\n')


class LteReportConfig:
    """
    LTE measurement report configuration
    """
    def __init__(self,report_id,hyst):
        self.report_id = report_id
        self.hyst = hyst
        self.event_list = []

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
