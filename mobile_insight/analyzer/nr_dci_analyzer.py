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
import math

__all__ = ["NrDciAnalyzer"]

class NrDciAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self._records = []
        self._time_ULs = []
        self._time_DLs = []
        # self._MCSs_UL = []
        # self._MCSs_DL = []
        self._RBs_UL = []
        self._both_DL_and_UL =[]
        self._throughput_UL = []
        # self._TRA_DL = []
        self._symbol_length_DL = []
        self._throughput_DL = []
        self.__cycles = 0
        self.__prev_frame_num = -1

        self.add_source_callback(self.__msg_callback)

    def reset_analyzer(self):
        self._records = []
        self._time_ULs = []
        self._time_DLs = []
        # self._MCSs_UL = []
        # self._MCSs_DL = []
        self._RBs_UL = []
        self._both_DL_and_UL =[]
        self._throughput_UL = []
        #self._TRA_DL = []
        self._symbol_length_DL = []
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
                    self._time_ULs.append(time)
                    # self._MCSs_UL.append(dci['DCI Params']['UL']['MCS'])
                    self._RBs_UL.append(dci['DCI Params']['UL']['RB Assignment'])
                    self._throughput_UL.append(int(dci['DCI Params']['UL']['RB Assignment'])*mcs_to_Qm[int(dci['DCI Params']['UL']['MCS'])]*mcs_to_r[int(dci['DCI Params']['UL']['MCS'])])
                    
                if dci['DCI Format'][:2] == 'DL':
                    self._time_DLs.append(time)
                    # self._MCSs_DL.append(dci['DCI Params']['DL']['TB 1 MCS'])
                    #self._TRA_DL.append(dci['DCI Params']['DL']['Time Resource Assignment'])
                    self._symbol_length_DL.append(TRA_to_l[int(dci['DCI Params']['DL']['Time Resource Assignment'])])
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

    def draw_throughput_ul(self, figure_size=(100,4), outlier_filter_m = 3, start_time = 0, end_time = math.inf, fillzero = False):
        plt.figure(figsize=figure_size)
        plt.xlabel('Time in ms')
        plt.ylabel("Approximate Throughput")
        plt.title("throughput UL: {} ms - {} ms".format(start_time, end_time if end_time!= math.inf else "inf"))

        throughput_x, throughput_y = self.__reject_outliers(self._time_ULs, self._throughput_UL, outlier_filter_m)

        if end_time == math.inf:
            end_time = int(throughput_x[-1])
        if fillzero:
            x = range(start_time, end_time)
            y = [throughput_y[throughput_x.index(t)] if t in throughput_x else 0 for t in x]
            plt.plot(x, y)
        else:
            s_idx, e_idx = np.searchsorted(throughput_x, [start_time,end_time])
            throughput_x, throughput_y = np.array(throughput_x[s_idx : e_idx]), np.array(throughput_y[s_idx : e_idx])
            plt.scatter(throughput_x, throughput_y, label='Throughput UL')
            #plt.plot(throughput_x, throughput_y, '-o')
            plt.legend()
        plt.savefig("throughput_UL_{}_ms_{}_ms.png".format(start_time, end_time), dpi=200)
        #plt.show()

    def draw_throughput_dl(self, figure_size=(100,4), outlier_filter_m = 3, start_time = 0, end_time = math.inf, fillzero = False):
        plt.figure(figsize=figure_size)
        plt.xlabel('Time in ms')
        plt.ylabel("Approximate Throughput")
        plt.title("throughput DL: {} ms - {} ms".format(start_time, end_time if end_time!= math.inf else "inf"))

        throughput_x, throughput_y = self.__reject_outliers(self._time_DLs, self._throughput_DL, outlier_filter_m)

        if end_time == math.inf:
            end_time = int(throughput_x[-1])
        if fillzero:
            x = range(start_time, end_time)
            y = [throughput_y[throughput_x.index(t)] if t in throughput_x else 0 for t in x]
            plt.plot(x, y)
        else: 
            s_idx, e_idx = np.searchsorted(throughput_x, [start_time,end_time])
            throughput_x, throughput_y = np.array(throughput_x[s_idx : e_idx]), np.array(throughput_y[s_idx : e_idx])
            plt.scatter(throughput_x, throughput_y, label='Throughput DL')
            #plt.plot(throughput_x, throughput_y, '-o')
            plt.legend()
        plt.savefig("throughput_DL_{}_ms_{}_ms.png".format(start_time, end_time), dpi=200)
        #plt.show()

    def draw_assignment_pattern(self,figure_size=(100,4), start_time = 0, end_time = math.inf):
        plt.figure(figsize=figure_size)
        plt.yticks([])
        plt.ylim([0.5, 3.5])
        #plt.xlim([7500, 29000])
        plt.xlabel('Time in ms')

        plt.title("Assignment Pattern: {} ms - {} ms".format(start_time, end_time if end_time!= math.inf else "inf"))

        s_idx_ul_dl, e_idx_ul_dl = np.searchsorted(self._both_DL_and_UL, [start_time,end_time])
        both_DL_and_ULx = np.array(self._both_DL_and_UL[s_idx_ul_dl: e_idx_ul_dl])
        both_DL_and_ULy = np.array([3]*len(both_DL_and_ULx))
        plt.scatter(both_DL_and_ULx, both_DL_and_ULy, label='self._both_DL_and_UL')

        s_idx_ul, e_idx_ul = np.searchsorted(self._time_ULs, [start_time,end_time])
        ULx = np.array(self._time_ULs[s_idx_ul:e_idx_ul])
        ULy = np.array([2]*len(ULx))
        plt.scatter(ULx, ULy, label='UL')

        s_idx_dl, e_idx_dl = np.searchsorted(self._time_DLs, [start_time,end_time])
        DLx = np.array(self._time_DLs[s_idx_dl:e_idx_dl])
        DLy = np.array([1]*len(DLx))
        plt.scatter(DLx, DLy, label='DL')

        plt.legend()
        plt.savefig("Assignment Pattern_{}_ms_{}_ms.png".format(start_time, end_time if end_time!= math.inf else "inf"), dpi=200)
        #plt.show()

    
    def draw_aggregated_frame(self):
        plt.figure()
        plt.xlabel('slot')
        plt.ylabel("Throughput")
        plt.title("resource assignment within frame - UL")

        x = range(10)
        y_ul = [0]*10
        for i in range(len(self._throughput_UL)):
            y_ul[int(self._time_ULs[i] % 10)] += self._throughput_UL[i]
        plt.plot(x, y_ul)
        plt.savefig("Frame_Pattern_UL.png", dpi=200)

        plt.figure()
        plt.xlabel('slot')
        plt.ylabel("Throughput")
        plt.title("resource assignment within frame - DL")

        x = range(10)
        y_dl = [0]*10
        for i in range(len(self._throughput_DL)):
            y_dl[int(self._time_DLs[i] % 10)] += self._throughput_DL[i]
        plt.plot(x, y_dl)
        plt.savefig("Frame_Pattern_DL.png", dpi=200)