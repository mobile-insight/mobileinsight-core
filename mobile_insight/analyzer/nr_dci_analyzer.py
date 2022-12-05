#!/usr/bin/python
# Filename: nr_dci_analyzer.py
"""
A NR 5G DCI analyzer.
Author: 
"""

from mobile_insight.analyzer.analyzer import *
import json
import matplotlib.pyplot as plt
import numpy as np

__all__ = ["NrDciAnalyzer"]

class NrDciAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self._records = []
        self._ULs = []
        self._DLs = []
        self._MCSs_UL = []
        self._MCSs_DL = []
        self._RBs = []
        self._both_DL_and_UL =[]
        self._throughput_UL = []
        self._TRA_DL = []
        self._throughput_DL = []
        
        self.__cycles = 0
        self.__prev_frame_num = -1

        self.add_source_callback(self.__msg_callback)

    def reset_analyzer(self):
        self._records = []
        self._ULs = []
        self._DLs = []
        self._MCSs_UL = []
        self._MCSs_DL = []
        self._RBs = []
        self._both_DL_and_UL =[]
        self._throughput_UL = []
        self._TRA_DL = []
        self._throughput_DL = []
        
        self.__cycles = 0
        self.__prev_frame_num = -1

    def set_source(self, source):
        """
        Set the trace source. 
        :param source: the trace source (collector).
        """
        Analyzer.set_source(self, source)

        # Phy-layer logs
        source.enable_log("NR_DCI_Message")
    
    def __msg_callback(self, msg):
        log_item = msg.data.decode()
        log_item_dict = dict(log_item)
        
        if log_item_dict['type_id'] == "NR_DCI_Message":
            self.__5g_dci_analysis(log_item_dict['Records'])

    def __5g_dci_analysis(self, records):
        """
        extract the time slot, DL/UL pattern to figure out the assignment pattern
        extract MCS RB of ULs and compute the approximate throughput
        """
        num_slots = 10
        num_frames = 1024
        time_per_frame = 10 #10ms
        time_per_slots = time_per_frame / num_slots

        mcs_to_Qm = [2,2,2,2,2,2,2,2,2,2,4,4,4,4,4,4,4,6,6,6,6,6,6,6,6,6,6,6,6,2,4,6]
        mcs_to_r = [120,157,193,251,308,379,449,526,602,679,340,378,434,490,553,
                    616,658,438,466,517,567,616,666,719,772,822,873,910,948,0,0,0] # last 3 ???
        TRA_to_l = [12,10,9,7,5,4,4,7,2,2,2,13,4,7,4]

        for record in records:
            slot_num = record['System Time']['Slot']
            frame_num = record['System Time']['Frame']
            if frame_num < self.__prev_frame_num:
                self.__cycles += 1
            self.__prev_frame_num = frame_num
            time = (frame_num + self.__cycles * num_frames) * time_per_frame + slot_num * time_per_slots

            if len(record['DCI Info']) == 2 and record['DCI Info'][0]['DCI Format'][:2]== 'DL' and record['DCI Info'][1]['DCI Format'][:2]== 'UL':
                self._both_DL_and_UL.append(time)
                
            for dci in record['DCI Info']:
                if dci['DCI Format'][:2] == 'UL':
                    self._ULs.append(time)
                    self._MCSs_UL.append(dci['DCI Params']['UL']['MCS'])
                    self._RBs.append(dci['DCI Params']['UL']['RB Assignment'])
                    self._throughput_UL.append(int(dci['DCI Params']['UL']['RB Assignment'])*mcs_to_Qm[int(dci['DCI Params']['UL']['MCS'])]*mcs_to_r[int(dci['DCI Params']['UL']['MCS'])])
                    
                if dci['DCI Format'][:2] == 'DL':
                    self._DLs.append(time)
                    self._MCSs_DL.append(dci['DCI Params']['DL']['TB 1 MCS'])
                    self._TRA_DL.append(dci['DCI Params']['DL']['Time Resource Assignment'])
                    self._throughput_DL.append(TRA_to_l[int(dci['DCI Params']['DL']['Time Resource Assignment'])]*mcs_to_Qm[int(dci['DCI Params']['DL']['TB 1 MCS'])]*mcs_to_r[dci['DCI Params']['DL']['TB 1 MCS']])


    def __reject_outliers(self,throughput_x, throughput_y, m=2):
        """
        filter out the outliers in throughput
        """
        mean = np.mean(throughput_y)
        std = np.std(throughput_y)
        res_x = []
        res_y = []
        for idx, throughput in enumerate(throughput_y):
            if abs(throughput - mean) < m * std:
                res_y.append(throughput)
                res_x.append(throughput_x[idx])
        return res_x, res_y

    def draw_throughput_ul(self, figure_size=(100,4), outlier_filter_m = 3):

        plt.figure(figsize=figure_size)
        
        plt.xlabel('Time in ms')
        plt.ylabel('Throughput')
        throughput_x = np.array(self._ULs)
        throughput_y = np.array(self._throughput_UL)
        # normalized_y = (throughput_y-np.min(throughput_y))/(np.max(throughput_y)-np.min(throughput_y))
        throughput_x, throughput_y = self.__reject_outliers(throughput_x, throughput_y, outlier_filter_m)

        plt.scatter(throughput_x, throughput_y, label='Throughput UL')
        plt.legend()
        plt.savefig('throughput_ul.png', dpi=200)
        #plt.show()

    def draw_throughput_dl(self, figure_size=(100,4), outlier_filter_m = 3):

        plt.figure(figsize=figure_size)
        
        plt.xlabel('Time in ms')
        plt.ylabel('Throughput')
        throughput_x = np.array(self._DLs)
        throughput_y = np.array(self._throughput_DL)
        # normalized_y = (throughput_y-np.min(throughput_y))/(np.max(throughput_y)-np.min(throughput_y))
        throughput_x, throughput_y = self.__reject_outliers(throughput_x, throughput_y, outlier_filter_m)

        plt.scatter(throughput_x, throughput_y, label='Throughput DL')
        plt.legend()
        plt.savefig('throughput_dl.png', dpi=200)
        #plt.show()

    def draw_assignment_pattern(self,figure_size=(100,4)):
        plt.figure(figsize=figure_size)
        plt.yticks([])
        plt.ylim([0.5, 3.5])
        #plt.xlim([7500, 29000])
        plt.xlabel('Time in ms')

        both_DL_and_ULx = np.array(self._both_DL_and_UL)
        both_DL_and_ULy = np.array([3]*len(self._both_DL_and_UL))
        plt.scatter(both_DL_and_ULx, both_DL_and_ULy, label='self._both_DL_and_UL')

        ULx = np.array(self._ULs)
        ULy = np.array([2]*len(self._ULs))
        plt.scatter(ULx, ULy, label='UL')

        DLx = np.array(self._DLs)
        DLy = np.array([1]*len(self._DLs))
        plt.scatter(DLx, DLy, label='DL')

        plt.legend()
        plt.savefig('assignment_pattern.png', dpi=200)
        #plt.show()