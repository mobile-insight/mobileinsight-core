#!/usr/bin/python
# Filename: nr_rrc_analyzer.py
"""
A LTE RRC analyzer.
Author: Qianru Li
"""

import xml.etree.ElementTree as ET
from mobile_insight.analyzer.analyzer import *
from mobile_insight.analyzer.protocol_analyzer import *
import timeit
import time

__all__ = ["NrRrcAnalyzer"]

class NrRrcAnalyzer(ProtocolAnalyzer):
    """
    A protocol ananlyzer for NR Radio Resource Control (RRC) protocol.
    """

    def __init__(self):
        print("Init NR RRC Analyzer")
        ProtocolAnalyzer.__init__(self)

        # init packet filters
        self.add_source_callback(self.__rrc_filter)

        # init internal states
        self.__status = NrRrcStatus()  # current cell status
        self.__history = {}  # cell history: timestamp -> LteRrcStatus()
        self.__config = {}  # (cell_id,freq) -> LteRrcConfig()

    def __rrc_filter(self, msg):

        """
        Filter all NR RRC packets, and call functions to process it

        :param msg: the event (message) from the trace collector.
        """
        log_item = msg.data.decode()
        log_item_dict = dict(log_item)

        # Calllbacks triggering
        if msg.type_id == "5G_NR_RRC_OTA_Packet":

            if 'Msg' not in log_item_dict:
                return

            # self.__update_conn(log_item_dict)
            if log_item_dict['PDU Number'] != 1 and log_item_dict['PDU Number'] != 26:
                self.__update_conn(int(log_item_dict['Freq']), int(log_item_dict['Physical Cell ID']), log_item_dict['timestamp'])

            # Convert msg to xml format
            log_xml = ET.XML(log_item_dict['Msg'])
            xml_msg = Event(log_item_dict['timestamp'], msg.type_id, log_xml)

            tic = time.process_time()
            self.__callback_rrc_conn(xml_msg)
            toc = time.process_time()

            # tic = time.process_time()
            # self.__callback_sib_config(xml_msg)
            # toc = time.process_time()

            tic = time.process_time()
            self.__callback_rrc_reconfig(xml_msg)
            toc = time.process_time()

            # Raise event to other analyzers
            self.send(xml_msg)  # deliver LTE RRC signaling messages (decoded)

    # def __update_conn(self, log_item_dict):
    def __update_conn(self, freq, cid, timestamp):
        """
        Update current cell status based on freq/cid in RRC OTA header

        :param msg: the RRC OTA messages
        """

        status_updated = False
        if not self.__status.inited():
            status_updated = True
            self.__status.freq = freq
            self.__status.id = cid
            # self.__status.tac = msg.data['TAC']

        else:
            if self.__status.freq != freq or self.__status.id != cid:
                status_updated = True
                self.__status = NrRrcStatus()
                self.__status.conn = True
                self.__status.freq = freq
                self.__status.id = cid
                # self.__status.tac = msg.data['TAC']
                self.__history[timestamp] = self.__status

        # if status_updated:
        #     self.log_info('NR_RRC_STATUS: ' + self.__status.dump())

    def __callback_sib_config(self, msg):
        """
        A callback to extract configurations from System Information Blocks (SIBs),
        including the radio asssement thresholds, the preference settings, etc.

        :param msg: RRC SIB messages
        """
        pass

    def __callback_rrc_reconfig(self, msg):

        """
        Extract configurations from RRCReconfiguration Message,
        including the measurement profiles, etc.

        :param msg: NR RRC reconfiguration messages
        """

        measobj_id = -1
        report_id = -1

        for field in msg.data.iter('field'):

            if field.get('name') == "nr-rrc.measObjectId":
                measobj_id = int(field.get('show'))

            if field.get('name') == "nr-rrc.reportConfigId":
                report_id = int(field.get('show'))

            # Add a NR measurement object
            if field.get('name') == "nr-rrc.measObjectNR_element":
                ssbFreq = 0
                for val in field.iter('field'):
                    if val.get('name') == 'nr-rrc.ssbFrequency':
                        ssbFreq = val.get('show')

                cur_pair = (self.__status.id, self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = NrRrcConfig()
                    self.__config[cur_pair].status = self.__status

                freq = int(ssbFreq)
                self.__config[cur_pair].active.measobj[measobj_id] = NrMeasObject(measobj_id, freq) # measobj_id, freq

                # TODO: check if there exist frequency/cell offset and how to get them

            # TODO: Add an LTE (4G) measurement object

            # Add a NR report configuration
            if field.get('name') == "nr-rrc.reportConfigNR_element":
                cur_pair = (self.__status.id, self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = NrRrcConfig()
                    self.__config[cur_pair].status = self.__status

                hyst = 0
                for val in field.iter('field'):
                    if val.get('name') == 'nr-rrc.hysteresis': 
                        hyst = int(val.get('show'))

                report_config = NrReportConfig(report_id, hyst / 2)

                for val in field.iter('field'):

                    if val.get('name') == 'nr-rrc.eventA1_element': # TODO: No real-trace to verify yet
                        threshold = None
                        quantity = None
                        for item in val.iter('field'):
                            if item.get('name') == 'nr-rrc.a1_Threshold':
                                for sub_item in item.iter('field'):
                                    if sub_item.get('name') == 'nr-rrc.rsrp':
                                        threshold = int(sub_item.get('show')) - 156
                                        quantity = 'rsrp'
                                    elif sub_item.get('name') == 'nr-rrc.rsrq':
                                        threshold = int(sub_item.get('show')) / 2.0 - 43
                                        quantity = 'rsrq'
                                # quantity = item.get('showname').split()[1]
                                if threshold is not None:
                                    report_config.add_event('a1', quantity, threshold)
                                break

                    if val.get('name') == 'nr-rrc.eventA2_element':
                        threshold = None
                        quantity = None
                        for item in val.iter('field'):
                            if item.get('name') == 'nr-rrc.a2_Threshold':
                                # showname="a2-Threshold: rsrp (0)"
                                for sub_item in item.iter('field'):
                                    if sub_item.get('name') == 'nr-rrc.rsrp':
                                        threshold = int(sub_item.get('show')) - 156
                                        quantity = 'rsrp'
                                    elif sub_item.get('name') == 'nr-rrc.rsrq':
                                        threshold = int(sub_item.get('show')) / 2.0 - 43
                                        quantity = 'rsrq'
                                # quantity = item.get('showname').split()[1]
                                if threshold is not None:
                                    report_config.add_event('a2', quantity, threshold)
                                break                 

                    if val.get('name') == 'nr-rrc.eventA3_element':
                        offset = None
                        quantity = None
                        for item in val.iter('field'):
                            if item.get('name') == 'nr-rrc.a3_Offset':
                                for sub_item in item.iter('field'):
                                    if sub_item.get('name') == 'nr-rrc.rsrp':
                                        offset = int(sub_item.get('show'))
                                        quantity = 'rsrp'
                                    elif sub_item.get('name') == 'nr-rrc.rsrq':
                                        offset = int(sub_item.get('show'))
                                        quantity = 'rsrq'
                                if threshold is not None:
                                    report_config.add_event('a3', quantity, offset)
                                break

                    if val.get('name') == 'nr-rrc.eventA4_element': # TODO: No real-trace to verify yet
                        threshold = None
                        quantity = None
                        for item in val.iter('field'):
                            if item.get('name') == 'nr-rrc.a4_Threshold':
                                for sub_item in item.iter('field'):
                                    if sub_item.get('name') == 'nr-rrc.rsrp':
                                        threshold = int(sub_item.get('show')) - 156
                                        quantity = 'rsrp'
                                    elif sub_item.get('name') == 'nr-rrc.rsrq':
                                        threshold = int(sub_item.get('show')) / 2.0 - 43
                                        quantity = 'rsrq'
                                if threshold is not None:
                                    report_config.add_event('a4', quantity, threshold)
                                break

                    if val.get('name') == 'nr-rrc.eventA5_element': # TODO: No real-trace to verify yet
                        threshold1 = None
                        threshold2 = None
                        quantity = None
                        for item in val.iter('field'):
                            if item.get('name') == 'nr-rrc.a5_Threshold1':
                                for sub_item in item.iter('field'):
                                    if sub_item.get('name') == 'nr-rrc.rsrp':
                                        threshold1 = int(sub_item.get('show')) - 156
                                        quantity = 'rsrp'
                                    elif sub_item.get('name') == 'nr-rrc.rsrq':
                                        threshold1 = int(sub_item.get('show')) / 2.0 - 43
                                        quantity = 'rsrq'
                                continue
                            if item.get('name') == 'nr-rrc.a5_Threshold2':
                                for sub_item in item.iter('field'):
                                    if sub_item.get('name') == 'nr-rrc.rsrp':
                                        threshold2 = int(sub_item.get('show')) - 156
                                    elif sub_item.get('name') == 'nr-rrc.rsrq':
                                        threshold2 = int(sub_item.get('show')) / 2.0 - 43
                                break
                        if threshold1 is not None:
                            report_config.add_event('a5', quantity, threshold1, threshold2)

                    if val.get('name') == 'nr-rrc.periodical_element':
                        report_config.add_event('periodic')

                    # TODO: add eventA6

                self.__config[cur_pair].active.report_list[report_id] = report_config

            # TODO: Add a LTE report configuration

            # Add a NR measurement report config
            if field.get('name') == "nr-rrc.MeasIdToAddMod_element":
                field_val = {}
                for val in field.iter('field'):
                    field_val[val.get('name')] = val.get('show')

                cur_pair = (self.__status.id, self.__status.freq)
                if cur_pair not in self.__config:
                    self.__config[cur_pair] = NrRrcConfig()
                    self.__config[cur_pair].status = self.__status

                meas_id = int(field_val['nr-rrc.measId'])
                obj_id = int(field_val['nr-rrc.measObjectId'])
                config_id = int(field_val['nr-rrc.reportConfigId'])
                self.__config[cur_pair].active.measid_list[meas_id] = (obj_id, config_id)
                # print(meas_id, obj_id, config_id)
                meas_obj, report_config = self.__config[cur_pair].get_meas_config(meas_id)
                # if meas_obj and report_config:
                #     self.log_info('NR_RRC_RECONFIG: ' + str(meas_id) + '\n' + 'meas_obj: ' + str(obj_id) + ' ' + self.__config[cur_pair].active.measobj[obj_id].dump() + '\nconfig_id: ' + str(config_id) + ' ' + self.__config[cur_pair].active.report_list[config_id].dump())
                # else:
                #     self.log_info('NR_RRC_RECONFIG: ' + str(meas_id) + '\n' + 'meas_obj: ' + str(obj_id) + '\nconfig_id: ' + str(config_id))

            if field.get("name") == "nr-rrc.measResults_element":
                # field_val = {}
                # field_val["nr-rrc.measId"] = None
                # field_val["nr-rrc.rsrp"] = None
                # field_val["nr-rrc.rsrq"] = None
                # field_val["nr-rrc.sinr"] = None
                measid = None
                serv_meas = {}
                neighborCells = []
                for val in field.iter("field"):
                    if val.get("name") == "nr-rrc.measId":
                        measid = int(val.get("show"))
                        # TODO: get configurations
                    if val.get("name") == "nr-rrc.measResultServingCell_element":
                        for sub_item in val.iter("field"):
                            if sub_item.get("name") == "nr-rrc.cellResults_element":
                                for attr in sub_item.iter("field"):
                                    if attr.get("name") == "nr-rrc.rsrp":
                                        serv_meas["nr-rrc.rsrp"] = int(attr.get("show")) - 156
                                    if attr.get("name") == "nr-rrc.rsrq":
                                        serv_meas["nr-rrc.rsrq"] = int(attr.get("show")) / 2.0 - 43
                                    if attr.get("name") == "nr-rrc.sinr":
                                        serv_meas["nr-rrc.sinr"] = int(attr.get("show")) / 2.0 - 23
                    if val.get("name") == "nr-rrc.measResultNeighCells":
                        for element in val.iter("field"):
                            if element.get("name") == "nr-rrc.MeasResultNR_element":
                                meas_element = {}
                                for sub_item in element.iter("field"):
                                    if sub_item.get("name") == "nr-rrc.physCellId":
                                        meas_element["nr-rrc.physCellId"] = int(sub_item.get("show"))
                                    if sub_item.get("name") == "nr-rrc.cellResults_element":
                                        for attr in sub_item.iter("field"):
                                            if attr.get("name") == "nr-rrc.rsrp":
                                                meas_element["nr-rrc.rsrp"] = int(attr.get("show")) - 156
                                            if attr.get("name") == "nr-rrc.rsrq":
                                                meas_element["nr-rrc.rsrq"] = int(attr.get("show")) / 2.0 - 43
                                            if attr.get("name") == "nr-rrc.sinr":
                                                meas_element["nr-rrc.sinr"] = int(attr.get("show")) / 2.0 - 23
                                neighborCells.append(meas_element)

                if measid is None:
                    return

                meas_obj = None
                report_config = None
                cur_pair = (self.__status.id, self.__status.freq)
                if cur_pair in self.__config:
                    meas_obj, report_config = self.__config[cur_pair].get_meas_config(measid)
                    if report_config and report_config.event_list[0].type == 'periodic':
                        continue
                        
                meas_obj_dump = "None" if meas_obj is None else meas_obj.dump()
                report_cfg_dump = "None" if report_config is None else report_config.dump()
                self.log_info("NR_RRC_REPORT " + str(msg.timestamp) + " " + 
                    "meas_object: " + meas_obj_dump + " " +
                    "report_config: " + report_cfg_dump + ' ' +
                    "serving_cell: " + str(serv_meas) + ' ' + 
                    "neighbor_cells: " + str(neighborCells))

            if field.get("name") == "nr-rrc.spCellConfigCommon_element":
                cid = None
                freq = None
                for val in field.iter("field"):
                    if val.get("name") == "nr-rrc.physCellId":
                        cid = int(val.get("show"))
                    if val.get("name") == "nr-rrc.absoluteFrequencySSB":
                        freq = int(val.get("show"))
                # self.log_info("UPDATE_CELL " + str(msg.timestamp) + " " +
                #     "source cell=" + str(("NR",self.__status.id, self.__status.freq)) + " " +
                #     "target cell=" + str(("NR",cid,freq)))
                self.log_info("UPDATE_NR_CELL " + str(msg.timestamp) + " " + str((freq,cid)))
                self.__update_conn(freq,cid,msg.timestamp)


    def __callback_rrc_conn(self, msg):
        """
        Update RRC connectivity status

        :param msg: the RRC message
        """
        for field in msg.data.iter('field'):
            if field.get('name') == "nr-rrc.rrcSetupComplete_element":
                self.__status.conn = True
                self.log_info(self.__status.dump())

            if field.get('name') == "nr-rrc.rrcRelease_element":
                self.__status.conn = False
                self.log_info(self.__status.dump())

    def set_source(self, source):
        """
        Set the trace source. Enable the NR RRC messages.

        :param source: the trace source.
        :type source: trace collector
        """
        Analyzer.set_source(self, source)
        # enable NR RRC log
        source.enable_log("5G_NR_RRC_OTA_Packet")

    def get_cell_list(self):
        """
        Get a complete list of cell IDs.

        :returns: a list of cells the device has associated with
        """
        # FIXME: currently only return *all* cells in the LteRrcConfig
        return list(self.__config.keys())

    def get_cell_config(self, cell):
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

    def get_cur_cellid(self):
        """
        Get current cell's ID

        :return: current cell's ID
        """

        return self.__status.id if self.__status else None

    def get_cur_freq(self):
        """
        Get current cell's EARFCN
        """

        return self.__status.freq if self.__status else None

    def get_cur_cell_status(self):
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
        :rtype: NrRrcConfig
        """
        cur_pair = (self.__status.id, self.__status.freq)
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


class NrRrcStatus:
    """
    The metadata of a cell, including its ID, frequency band, tracking area code,
    bandwidth, connectivity status, etc.
    """

    def __init__(self):
        self.id = None  # cell ID
        self.freq = None  # cell frequency
        self.rat = "NR"  # radio technology
        # self.tac = None  # TODO: tracking area code
        self.bandwidth = None  # cell bandwidth
        self.conn = False  # connectivity status (for serving cell only)

    def dump(self):
        """
        Report the cell status

        :returns: a string that encodes the cell status
        :rtype: string
        """
        return (self.__class__.__name__
                + " cellID=" + str(self.id)
                + " frequency=" + str(self.freq)
                + " bandwidth=" + str(self.bandwidth)
                + " connected=" + str(self.conn))

    def inited(self):
        # return (self.id!=None and self.freq!=None)
        return (self.id is not None and self.freq is not None)


class NrRrcConfig:
    """
    Per-cell RRC configurations

    The following configurations are supported
        - Active-state
            - PHY/MAC/PDCP/RLC configuration
            - Measurement configurations
        - (TODO) Idle-state
            - Cell reselection parameters
    """

    def __init__(self):
        self.status = NrRrcStatus()  # the metadata of this cell
        self.status.rat = "NR"
        self.active = NrRrcActive()  # active-state configurations

    def dump(self):
        """
        Report the cell configurations

        :returns: a string that encodes the cell's configurations
        :rtype: string
        """

        return (self.__class__.__name__ + '\n'
                + self.status.dump()
                + self.active.dump())

    # def get_cell_reselection_config(self, cell_meta):
    #     """
    #     Given a cell, return its reselection config as a serving cell

    #     :param cell_meta: a cell identifier
    #     :type cell_meta: a (cell_id,freq) pair

    #     :returns: cell reselection configurations
    #     :rtype: LteRrcReselectionConfig
    #     """
    #     pass

    def get_meas_config(self, meas_id):

        """
        Given a meas_id, return the meas_obj and report_config.

        :param meas_id
        :type meas_id: an integer
        :returns: meas_obj and report_config
        :rtype: a pair of (NrMeasObject, NrReportConfig)
        """
        if meas_id in self.active.measid_list:
            obj_id, report_id = self.active.measid_list[meas_id]
            if obj_id in self.active.measobj and report_id in self.active.report_list:
                return (self.active.measobj[obj_id], self.active.report_list[report_id])
        return (None,None)
        # pass


class NrRrcActive:
    """
    RRC active-state configurations (from RRCReconfiguration messsage)
    """

    def __init__(self):
        # TODO: initialize some containers
        self.measobj = {}  # meas_id->measobject
        self.report_list = {}  # report_id->reportConfig
        self.measid_list = {}  # meas_id->(obj_id,report_id)

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
            res += "MeasObj " + str(item) + ' ' + str(self.measid_list[item]) + '\n'
        return res


class NrMeasObject:
    """
    NR Measurement object configuration
    """

    # def __init__(self, measobj_id, freq, offset_freq):
    def __init__(self, measobj_id, freq, rat='NR'):
        self.obj_id = measobj_id
        self.freq = freq  # carrier frequency
        self.rat = rat

    def dump(self):
        """
        Report the cell's NR measurement objects

        :returns: a string that encodes the cell's NR measurement objects
        :rtype: string
        """
        res = (self.__class__.__name__
               + ' object_id=' + str(self.obj_id)
               + ' freq=' + str(self.freq)
               + ' RAT=' + str(self.rat))
        return res


class NrReportConfig:
    """
    NR measurement report configuration
    """

    def __init__(self, report_id, hyst):
        self.report_id = report_id
        self.hyst = hyst
        self.event_list = []

    def add_event(self, event_type, quantity=None, threshold1=None, threshold2=None):
        """
        Add a measurement event

        :param event_type: a measurement type
        :type event_type: string
        :param threshold1: threshold 1
        :type threshold1: int
        :param threshold2: threshold 2
        :type threshold2: int
        """
        self.event_list.append(NrRportEvent(event_type, quantity, threshold1, threshold2))

    def dump(self):
        """
        Report the cell's measurement report configurations

        :returns: a string that encodes the cell's measurement report configurations
        :rtype: string
        """
        res = (self.__class__.__name__
               + ' report_id=' + str(self.report_id)
               + ' hyst=' + str(self.hyst))
        for item in self.event_list:
            res += (' ' + str(item.type)
                    + ' ' + str(item.quantity)
                    + ' ' + str(item.threshold1)
                    + ' ' + str(item.threshold2))
        return res


class NrRportEvent:
    """
    Abstraction for NR report event
    """

    def __init__(self, event_type, quantity, threshold1, threshold2=None):
        self.type = event_type
        self.quantity = quantity
        self.threshold1 = threshold1
        self.threshold2 = threshold2
