#!/usr/bin/python
# Filename: state_machine.py
"""
State machine abstraction

Abstract protocol FSMs, and operationnal policies (algorithms)

Author: Yuanjie Li
"""

__all__=["StateMachnieProfile","StateMachnie"]

class StateMachnieProfile(object):
    '''
    A graphical representation of state machnie profile
    '''    

    def __init__(self):
        #TODO: a declarative approach for state machine
        pass


    def merge_state(self,partial_state):
        '''
        Merge partial state transitions from existing ones. 
        This function is designed for operational analyzers (e.g., learning the handoff rules online)

        :param partial_state: partial state transition
        :type partial_state: dictionary
        '''
        pass
        


class StateMachnie(object):
    
    def __init__(self,profile,analyzer):
        '''
        Initialize a state machine with a pre-define profile

        :param profile: a state machnie profile
        :type profile: StateMachnieProfile
        :param analyzer: the analyzer that includes this state machnie (used for callback)
        :type analyzer: Analyzer
        '''
        pass

    def __trigger_transition(self):
        '''
        Trigger state transition from analyzer events

        '''
        pass

    def get_current_state(self):
        '''
        Get current state

        :returns: current state
        '''
        pass

    def get_transition_condition(self,src,dest):
        '''
        Get transition condition from source to destination

        :param src: source state
        :type src: string
        :param dest: destination state
        :type dest: string
        :returns: an ordered list of callbacks that represent the state transition
        ''' 
        pass

