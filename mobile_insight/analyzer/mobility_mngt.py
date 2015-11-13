#!/usr/bin/python
# Filename: mobility_mngt.py
"""
A Mobility management analyzer.

Author: Yuanjie Li
"""

import xml.etree.ElementTree as ET
from analyzer import *
from wcdma_rrc_analyzer import WcdmaRrcAnalyzer
from lte_rrc_analyzer import LteRrcAnalyzer
from lte_nas_analyzer import LteNasAnalyzer
#TODO: import UmtsNasAnalyzer

__all__=["MobilityMngt"]

class MobilityMngt(Analyzer):
    """
    A function analyzer that models mobility management. 
    It offers two functions

        (1) Mobility event notification
        
        (2) A complete mobility history and configuration query

        (3) A handoff rule inference module
    """    

    def __init__(self):

        Analyzer.__init__(self)

        #include analyzers
        self.include_analyzer("WcdmaRrcAnalyzer",[self.__on_wcdma_rrc_msg])
        self.include_analyzer("LteRrcAnalyzer",[self.__on_lte_rrc_msg])
        # self.include_analyzer("LteNasAnalyzer",[self.__on_lte_nas_msg])

        #no source callbacks are included


    def __on_lte_rrc_msg(self,msg):
        """
        Handle LTE RRC messages.
        It updates the mobility state, 
        recovers the partial state transition, 
        and then the entire handoff mobility graph

        :param msg: the event (message) from the trace collector.
        """    

        pass

    def __on_wcdma_rrc_msg(self,msg):
        """
        Handle WCDMA RRC messages.
        It updates the mobility state, 
        recovers the partial state transition, 
        and then the entire handoff mobility graph

        :param msg: the event (message) from the trace collector.
        """    

        pass


    def __on_lte_nas_msg(self,msg):
        """
        Handle LTE NAS messages (traking area update and attach/detach)
        It updates the mobility state, 

        :param msg: the event (message) from the trace collector.
        """    

        pass



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
        if not self.measobj.has_key(self.measid_list[meas_id][0]):
            return None
        else:
            return self.measobj[meas_id][0]

    def get_reportconfig(self,meas_id):
        """
        Given the measurement ID, returns the corresponding report configuration object

        :param meas_id: measurement ID
        :type meas_id: integer
        :returns: ReportConfig in it, or None if the id does not exist
        """
        if not self.report_list.has_key(self.measid_list[meas_id][1]):
            return None
        else:
            return self.measid_list[meas_id][1]


    def equals(self,meas_state):
        """
        Compare two states to see if they are equivalent

        :param meas_state: another measurement state
        :type meas_state: MeasState
        :returns: True if two states are equivalent, False otherwise
        """

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
            for meas_id2 in meas_state:
                meas_obj2 = meas_state.get_measobj(meas_id2)
                report_obj2 = meas_state.get_reportconfig(meas_id2)
                if meas_obj.equals(meas_obj2) and report_obj.equals(report_obj2):
                    meas_id_exist = True
                    break
            if not meas_id_exist:
                return False
        return True




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


class LteMeasObjectUtra:
    """
    3G Measurement object configuration
    """

    def __init__(self,measobj_id,freq,offset_freq):
        self.obj_id = measobj_id
        self.freq = freq # carrier frequency
        self.offset_freq = offset_freq # frequency-specific measurement offset
        #TODO: add cell list

    def equals(self,meas_obj):
        """
        Compare if this meas_obj is equal to another one

        :param meas_obj: a measurement object
        :type meas_obj: LteMeasObjectUtra
        :returns: True if they are equivalent, False otherwise
        """
        return meas_obj.__class__.__name__ == "LteMeasObjectUtra" \
        and self.freq == meas_obj.freq \
        and self.offset_freq == meas_obj.offset_freq \
        and self.cell_list == meas_obj.cell_list


class LteReportConfig:
    """
    LTE measurement report configuration
    """
    def __init__(self,report_id,hyst):
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


