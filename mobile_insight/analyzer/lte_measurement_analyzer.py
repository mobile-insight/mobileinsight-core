#!/usr/bin/python
# Filename: lte_measurement_analyzer.py
"""
An analyzer for LTE radio measurements

Author: Yuanjie Li
"""

from .analyzer import *

class LteMeasurementAnalyzer(Analyzer):
    """
    An analyzer for LTE radio measurements
    """
    def __init__(self):

        Analyzer.__init__(self)

        #init packet filters
        self.add_source_callback(self.ue_event_filter)

        self.serv_cell_rsrp=[] #rsrp measurements
        self.serv_cell_rsrq=[] #rsrq measurements

    def set_source(self,source):
        """
        Set the source of the trace. 
        Enable device's LTE internal logs.

        :param source: the source trace collector
        :param type: trace collector
        """
        Analyzer.set_source(self,source)
        #enable user's internal events
        source.enable_log("LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results")
        source.enable_log("LTE_ML1_IRAT_Measurement_Request")
        source.enable_log("LTE_ML1_Serving_Cell_Measurement_Result")
        source.enable_log("LTE_ML1_Connected_Mode_Neighbor_Meas_Req_Resp")

    def ue_event_filter(self,msg):
        """
        callback to handle user events

        :param source: the source trace collector
        :param type: trace collector
        """
        #TODO: support more user events
        self.serving_cell_rsrp(msg)

    def serving_cell_rsrp(self,msg):
        if msg.type_id == "LTE_ML1_Connected_Mode_LTE_Intra_Freq_Meas_Results":
            msg_dict=dict(msg.data.decode())
            # print msg.timestamp,msg_dict['Serving Filtered RSRP(dBm)']
            rsrp_log = (self.__class__.__name__
                + ' RSRP=' + str(msg_dict['Serving Filtered RSRP(dBm)'])+'dBm'
                + ' RSRQ=' + str(msg_dict['Serving Filtered RSRQ(dB)'])+'dB'
                + ' # of neighbors=' + str(msg_dict['Number of Neighbor Cells']))

            self.logger.info(rsrp_log)
            
            self.serv_cell_rsrp.append(msg_dict['Serving Filtered RSRP(dBm)'])
            self.serv_cell_rsrq.append(msg_dict['Serving Filtered RSRP(dBm)'])

    def get_rsrp_list(self):
    	"""
    	Get serving cell's RSRP measurement

    	:returns: a list of serving cell's measurement
    	:rtype: list
    	"""
    	return self.serv_cell_rsrp

    def get_rsrq_list(self):
    	"""
    	Get serving cell's RSRQ measurement
    	
    	:returns: a list of serving cell's measurement
    	:rtype: list
    	"""
    	return self.serv_cell_rsrq
            
