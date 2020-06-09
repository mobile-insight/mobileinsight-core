#!/usr/bin/python
# Filename: rrc_analyzer.py
"""
A RRC analyzer that integrates LTE and WCDMA RRC

Author: Yuanjie Li
"""
import xml.etree.ElementTree as ET
from .analyzer import *
from .wcdma_rrc_analyzer import WcdmaRrcAnalyzer
from .lte_rrc_analyzer import LteRrcAnalyzer

__all__ = ["RrcAnalyzer"]


class RrcAnalyzer(Analyzer):
    """
    A protocol ananlyzer for 3G/4G Radio Resource Control (RRC).
    It depends on WcdmaRrcAnalyzer and LteRrcAnalyzer, and integrates configurations
    from both ananlyzers.
    """

    def __init__(self):
        Analyzer.__init__(self)

        # include analyzers
        self.include_analyzer("WcdmaRrcAnalyzer", [self.__on_event])
        self.include_analyzer("LteRrcAnalyzer", [self.__on_event])

        self.__cur_RAT = None  # current RAT

        # init packet filters
        self.add_source_callback(self.__rrc_filter)

    def __rrc_filter(self, msg):
        """
        Callback to process RRC messages.

        :param msg: the WCDMA/LTE RRC message from trace source.
        :type msg: Event
        """

        if msg.type_id.find("LTE") != -1:  # LTE RRC msg received, so it's LTE
            self.__cur_RAT = "LTE"
        # WCDMA RRC msg received, so it's WCDMA
        elif msg.type_id.find("WCDMA") != -1:
            self.__cur_RAT = "WCDMA"

    def __on_event(self, event):
        """
        Triggered by WcdmaRrcAnalyzer and/or LteRrcAnalyzer.
        Push the event to analyzers that depend on RrcAnalyzer

        :param event: the event raised by WcdmaRrcAnalyzer and/or LteRrcAnalyzer.
        :type event: Event
        """
        e = Event(event.timestamp, "RrcAnalyzer", event.data)
        self.send(e)

    def get_cell_list(self):
        """
        Get a complete list of cell IDs.

        :returns: a list of cells the device has associated with
        """
        # lte_cell_list=self.__lte_rrc_analyzer.get_cell_list()
        lte_cell_list = self.get_analyzer("LteRrcAnalyzer").get_cell_list()
        # wcdma_cell_list=self.__wcdma_rrc_analyzer.get_cell_list()
        wcdma_cell_list = self.get_analyzer("WcdmaRrcAnalyzer").get_cell_list()
        return lte_cell_list + wcdma_cell_list

    def get_cell_config(self, cell):
        """
        Return a cell's active/idle-state configuration.

        :param cell:  a cell identifier
        :type cell: a (cell_id,freq) pair
        :returns: this cell's active/idle-state configurations
        :rtype: LteRrcConfig or WcdmaRrcConfig
        """
        # res=self.__lte_rrc_analyzer.get_cell_config(cell)
        res = self.get_analyzer("LteRrcAnalyzer").get_cell_config(cell)
        if res:
            return res
        else:
            # return self.__wcdma_rrc_analyzer.get_cell_config(cell)
            return self.get_analyzer("WcdmaRrcAnalyzer").get_cell_config(cell)

    def get_cur_cell(self):
        """
        Get current cell's status

        :returns: current cell's status
        :rtype: LteRrcStatus   or WcdmaRrcStatus
        """
        if self.__cur_RAT == "LTE":
            # return self.__lte_rrc_analyzer.get_cur_cell()
            return self.get_analyzer("LteRrcAnalyzer").get_cur_cell()
        elif self.__cur_RAT == "WCDMA":
            # return self.__wcdma_rrc_analyzer.get_cur_cell()
            return self.get_analyzer("WcdmaRrcAnalyzer").get_cur_cell()
        else:
            return None

    def get_cur_cell_config(self):
        """
        Get current cell's configuration

        :returns: current cell's status
        :rtype: LteRrcConfig or WcdmaRrcConfig
        """
        if self.__cur_RAT == "LTE":
            # return self.__lte_rrc_analyzer.get_cur_cell_config()
            return self.get_analyzer("LteRrcAnalyzer").get_cur_cell_config()
        elif self.__cur_RAT == "WCDMA":
            # return self.__wcdma_rrc_analyzer.get_cur_cell_config()
            return self.get_analyzer("WcdmaRrcAnalyzer").get_cur_cell_config()
        else:
            return None

    def get_cell_on_freq(self, freq):
        """
        Given a frequency band, get all cells under this freq in the cell_list.

        :param freq: a frequency band
        :type freq: int
        """
        cell_list = self.get_cell_list()
        res = []
        for cell in cell_list:
            if cell[1] == freq:
                res.append(cell)
        return res

    def get_cell_neighbor(self, cell):
        """
        Given a cell, return its neighbor cells

        :param cell: a cell identifier
        :type cell: a (cell_id,freq) pair
        """
        cell_config = self.get_cell_config(cell)
        cell_freq = cell_config.status.freq
        inter_freq_dict = cell_config.sib.inter_freq_config
        neighbor_cells = []

        # add intra-freq neighbors
        neighbor_cells += self.get_cell_on_freq(cell[1])
        neighbor_cells.remove(cell)  # remove the cell itself

        # add inter-freq/RAT neighbors
        for freq in inter_freq_dict:
            neighbor_cells += self.get_cell_on_freq(freq)

        # WCDMA: any cells under WCDMA can be its neighbors
        if cell_config.status.rat == "UTRA":
            cell_list = self.get_cell_list()
            for item in cell_list:
                if self.get_cell_config(item).status.rat == "UTRA" \
                        and item not in neighbor_cells:
                    neighbor_cells.append(item)

        if cell in neighbor_cells:
            neighbor_cells.remove(cell)  # remove the current cell itself

        return neighbor_cells
