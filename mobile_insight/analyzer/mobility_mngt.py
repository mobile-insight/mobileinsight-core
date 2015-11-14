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

        #Track current freq and RAT. If any one changes, load the new state machine
        self.__cur_RAT = None
        self.__cur_freq = None

        self.__handoff_sample = HandoffSample()
        self.__mobility_state_machine = MobilityStateMachine()



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
    def __init__(self,rat, to_freq):
        self.rat = rat #Radio access technology (3G or 4G)
        self.freq = freq #Frequency band

    def equals(self,handoff_state):
        return handoff_state.freq==self.freq \
        and handoff.rat==self.rat 


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
        and meas_report[1].__class__.__name__!="LteReportConfig":
            return False
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


class HandoffSample:
    """
    A handoff sample based on observation
    """
    def __init__(self):
        self.cur_state = None
        #(From_State,To_State,tx_cond)
        #For the first element, its tx_cond is meaningless
        self.tx_list=[] 

    def reset(self):
        """
        Reset the handoff sample
        """
        self.cur_state = None
        del self.tx_list[:]

    def add_state_transition(self,new_state,tx_cond):
        """
        Append a new state and its transition condition.

        :param new_state: a MeasState or a HandoffState
        :type new_state: MeasState or HandoffState
        :param tx_cond: state transition condition, represented as meas report sequence
        :type tx_cond: MeasReportSeq
        :returns: True if succeeds, or False otherwise
        """
        if new_state.__class__.__name__!="MeasState" \
        and new_state.__class__.__name__!="HandoffState" \
        and tx_cond.__class__.__name__!="MeasReportSeq":
            return False

        self.tx_list.append((self.cur_state,new_state,tx_cond))
        self.cur_state=new_state
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

        for item in handoff_sample:
            self.__merge_transition(item)


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

        if new_state.__class__.__name__!="MeasState" \
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

############################################


# Helper modules
############################################
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
############################################

