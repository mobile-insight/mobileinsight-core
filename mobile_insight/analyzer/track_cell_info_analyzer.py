#!/usr/bin/python
# Filename: track_cell_info_analyzer.py
"""
A LTE RRC analyzer.

Author: Yuanjie Li, Zhehui Zhang
"""

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .analyzer import *
import timeit
import time

__all__=["TrackCellInfoAnalyzer"]

class TrackCellInfoAnalyzer(Analyzer):

    """
    A protocol ananlyzer for LTE Radio Resource Control (RRC) protocol.
    """
    def __init__(self):

        Analyzer.__init__(self)

        #init internal states
        self.__status = LteRrcStatus()    # current cell status
        self.add_source_callback(self.__rrc_filter)

        # From LTE MIB Packet
        self.__mib_antenna = None
        self.__mib_dl_bandwidth = None
        self.__mib_cell_id = None
        self.__mib_freq = None

    def __rrc_filter(self,msg):

        """
        Filter all LTE RRC packets, and call functions to process it

        :param msg: the event (message) from the trace collector.
        """
        if msg.type_id == "LTE_RRC_Serv_Cell_Info":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            raw_msg = Event(msg.timestamp,msg.type_id,log_item_dict)
            self.__callback_serv_cell(raw_msg)
        elif msg.type_id == "LTE_RRC_MIB_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            raw_msg = Event(msg.timestamp,msg.type_id,log_item_dict)
            self.__callback_mib_cell(raw_msg)



    def __callback_serv_cell(self,msg):

        """
        A callback to update current cell status

        :param msg: the RRC messages with cell status
        """
        status_updated = False
        if not self.__status.inited():
            status_updated = True
            self.__status.dl_freq = msg.data['Downlink frequency']
            self.__status.ul_freq = msg.data['Uplink frequency']
            self.__status.dl_bandwidth = msg.data['Downlink bandwidth']
            self.__status.ul_bandwidth = msg.data['Uplink bandwidth']
            self.__status.allowed_access = msg.data['Allowed Access']
            self.__status.id = msg.data['Cell ID']
            self.__status.gid = msg.data['Cell Identity']
            self.__status.tac = msg.data['TAC']
            self.__status.band_indicator = msg.data['Band Indicator']
            mnc_value = msg.data['MNC']
            if mnc_value == 260:
                self.__status.op = 'T-Mobile'
            if mnc_value == 120:
                self.__status.op = 'Sprint'
            if mnc_value == 410:
                self.__status.op = 'ATT'
            if mnc_value == 480:
                self.__status.op = 'Verizon'


        else:
            if self.__status.dl_freq != msg.data['Downlink frequency'] \
                    or self.__status.id != msg.data['Cell ID'] \
                    or self.__status.tac != msg.data['TAC']:   
                status_updated = True
                curr_conn = self.__status.conn
                self.__status = LteRrcStatus()
                self.__status.conn = curr_conn
                self.__status.dl_freq = msg.data['Downlink frequency']
                self.__status.ul_freq = msg.data['Uplink frequency']
                self.__status.dl_bandwidth = msg.data['Downlink bandwidth']
                self.__status.ul_bandwidth = msg.data['Uplink bandwidth']
                self.__status.allowed_access = msg.data['Allowed Access']
                self.__status.id = msg.data['Cell ID']
                self.__status.gid = msg.data['Cell Identity']
                self.__status.tac = msg.data['TAC']
                self.__status.band_indicator = msg.data['Band Indicator']
                mnc_value = msg.data['MNC']
                if mnc_value == 260:
                    self.__status.op = 'T-Mobile'
                if mnc_value == 120:
                    self.__status.op = 'Sprint'
                if mnc_value == 410:
                    self.__status.op = 'ATT'
                if mnc_value == 480:
                    self.__status.op = 'Verizon'

        if status_updated:
            self.log_debug(self.__status.dump())
            self.broadcast_info('LTE_RRC_STATUS', self.__status.dump_dict())


    def __callback_mib_cell(self, msg):
        self.__mib_antenna = msg.data['Number of Antenna']
        self.__mib_dl_bandwidth = msg.data['DL BW']
        self.__mib_cell_id = msg.data['Physical Cell ID']
        self.__mib_freq = msg.data['Freq']
        event = Event(None, 'MIB_CELL', None)
        self.send(event)


    def set_source(self,source):
        """
        Set the trace source. Enable the LTE RRC messages.

        :param source: the trace source.
        :type source: trace collector
        """
        Analyzer.set_source(self,source)
        #enable LTE RRC log
        source.enable_log("LTE_RRC_Serv_Cell_Info")
        source.enable_log("LTE_RRC_MIB_Packet")


    def get_mib_number_of_antennas(self):
        return self.__mib_antenna

    def get_mib_downlink_bandwidth(self):
        return self.__mib_dl_bandwidth


    def get_cur_cell_id(self):
        """
        Get current cell's status

        :returns: current cell's status
        :rtype: LteRrcStatus      
        """
        return self.__status.gid

    def get_cur_cell_tac(self):
        """
        Get current cell's status

        :returns: current cell's status
        :rtype: LteRrcStatus
        """
        return self.__status.tac

    def get_cur_downlink_frequency(self):
        """
        Get current cell's downlink frequency band 
        """
        return self.__status.dl_freq

    def get_cur_uplink_frequency(self):
        """
        Get current cell's uplink frequency band 
        """
        return self.__status.ul_freq

    def get_cur_downlink_bandwidth(self):
        """
        Get current cell's downlink bandwidth
        """
        return self.__status.dl_bandwidth

    def get_cur_uplink_bandwidth(self):
        """
        Get current cell's uplink bandwidth
        """
        return self.__status.ul_bandwidth

    def get_cur_allowed_access(self):
        return self.__status.allowed_access

    def get_cur_op(self):
        return self.__status.op

    def get_cur_band_indicator(self):
        return self.__status.band_indicator

class LteRrcStatus:
    """
    The metadata of a cell, including its ID, frequency band, tracking area code,
    bandwidth, connectivity status, etc.
    """
    def __init__(self):
        self.id = None #cell ID
        self.gid = None
        # self.freq = None #cell frequency
        self.dl_freq = None # Cell downlink frequency
        self.ul_freq = None # Cell uplink frequency
        self.dl_bandwidth = None # Cell downlink bandwidth
        self.ul_bandwidth = None # Cell uplink bandwidth
        self.band_indicator = None # band indicator
        self.allowed_access = None # Allowed access
        self.rat = "LTE" #radio technology
        self.tac = None #tracking area code
        self.conn = False #connectivity status (for serving cell only)
        self.op = "None"

    def inited(self):
        # return (self.id!=None and self.freq!=None)
        return (self.id and self.dl_freq)

    def dump(self):
        """
        Report the cell status

        :returns: a string that encodes the cell status
        :rtype: string
        """
        return (self.__class__.__name__
                + " cellID=" + str(self.id)
                + " GcellID=" + str(self.gid)
                + " DL_frequency=" + str(self.dl_freq)
                + " UL_frequency=" + str(self.ul_freq)
                + " DL_bandwidth=" + str(self.dl_bandwidth)
                + " UL_bandwidth=" + str(self.ul_bandwidth)
                + " Band_indicator=" + str(self.band_indicator)
                + " TAC=" + str(self.tac)
                + " connected=" + str(self.conn) + '\n')

    def dump_dict(self):
        """
        Report the cell status

        :returns: a dict that encodes the cell status
        :rtype: dict
        """
        dumped_dict = {}
        dumped_dict['cellID'] = str(self.id)
        dumped_dict['GcellID'] = str(self.gid)
        dumped_dict['DL_frequency'] = str(self.dl_freq)
        dumped_dict['UL_frequency'] = str(self.ul_freq)
        dumped_dict['DL_bandwidth'] = str(self.dl_bandwidth)
        dumped_dict['UL_bandwidth'] = str(self.ul_bandwidth)
        dumped_dict['Band Indicator'] = str(self.band_indicator)
        dumped_dict['TAC'] = str(self.tac)
        dumped_dict['connected'] = str(self.conn)
        return dumped_dict
