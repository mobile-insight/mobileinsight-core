#!/usr/bin/python
# Filename: nr_dci_analyzer.py
"""
A NR PDCP DL Data PDU analyzer.
Author: 
"""

from mobile_insight.analyzer.analyzer import *
import json

import matplotlib.pyplot as plt
import numpy as np
import math

import os, re

__all__ = ["NrPdcpDlDataPduAnalyzer"]

class NrPdcpDlDataPduAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.__times = []
        self.__cycles = 0
        self.__prev_frame_num = -1

        self.__rlcPaths = []
        self.__rlcPathTimes = []

        self.__routeStatuses = []
        self.__routeStatusTimes = []

        self.__startCounts = []
        self.__startCountTimes = []

        self.__endCounts = []
        self.__endCountTimes = []

        self.__rlcEndSns = []
        self.__rlcEndSnTimes = []

        self.__numIpPackets = []
        self.__numIpPacketTimes = []

        self.__numIpBytes = []
        self.__numIpByteTimes = []

        self.add_source_callback(self.__msg_callback)
    
    def reset_analyzer(self):
        self.__times = []
        self.__cycles = 0
        self.__prev_frame_num = -1

        self.__rlcPaths = []
        self.__rlcPathTimes = []

        self.__routeStatuses = []
        self.__routeStatusTimes = []

        self.__startCounts = []
        self.__startCountTimes = []

        self.__endCounts = []
        self.__endCountTimes = []

        self.__rlcEndSns = []
        self.__rlcEndSnTimes = []

        self.__numIpPackets = []
        self.__numIpPacketTimes = []

        self.__numIpBytes = []
        self.__numIpByteTimes = []

        # delete any figures
        #for f in os.listdir(dir):
        #    if re.search("*.png", f):
        #        os.remove(f)

    def set_source(self, source):
        """
        Set the trace source. 
        :param source: the trace source (collector).
        """
        Analyzer.set_source(self, source)

        # Phy-layer logs
        source.enable_log("NR_PDCP_DL_Data_Pdu")


    def __msg_callback(self, msg):
        log_item = msg.data.decode()
        log_item_dict = dict(log_item)

        #if the type is not JSON serializable (like datatime), convert it to str
        #json_log = json.dumps(log_item_dict, indent=4, default=str) 
        #print(json_log)

        if log_item_dict["type_id"] == "NR_PDCP_DL_Data_Pdu":
            self.__nr_pdcp_dl_data_pdu_analysis(log_item_dict["Versions"]["Version 5"])
    
    def __nr_pdcp_dl_data_pdu_analysis(self, version):
        num_slots = 10
        num_frames = 1024
        time_per_frame = 10 #10ms
        time_per_slots = time_per_frame / num_slots

        numRb = version["Number of RB"]
        for i in range(numRb):
            pdcpState = version["PDCP State"][i]
            rbConfigIndex = pdcpState["RB Cfg Index"]
            rxDeliv = pdcpState["RX Deliv"]
            rxNext = pdcpState["RX Next"]
            nextCount = pdcpState["Next Count"]

        numMeta = version["Number of Meta"]
        for i in range(numMeta):
            metaLog = version["Meta Log Buffer"][i]

            systemTime = metaLog["System Time"]
            slot_num = systemTime["Slot"]
            frame_num = systemTime["Frame"]
            if frame_num < self.__prev_frame_num:
                self.__cycles += 1
            self.__prev_frame_num = frame_num
            time = (frame_num + self.__cycles * num_frames) * time_per_frame + slot_num * time_per_slots

            keyIndex = metaLog["Key Index"]
            rlcPath = metaLog["RLC Path"]
            routeStatus = metaLog["Route Status"]
            ipPacketHeader0 = metaLog["IP Packet Header[0]"]
            ipPacketHeader1 = metaLog["IP Packet Header[1]"]
            startCount = metaLog["Start Count"]
            endCount = metaLog["End Count"]
            rlcEndSn = metaLog["RLC end SN"]
            numIpPackets = metaLog["Number IP Pkts"]
            numIpBytes = metaLog["Number IP bytes"]

            # check if time already exists - if it does, modify that current entry, otherwise add a new time entry
            if time in self.__times:
                duplicateTimeIndex = self.__times.index(time)
                self.__endCounts[duplicateTimeIndex] = endCount
                self.__rlcEndSns[duplicateTimeIndex] = rlcEndSn
                self.__numIpPackets[duplicateTimeIndex] = self.__numIpPackets[duplicateTimeIndex] + numIpPackets
                self.__numIpBytes[duplicateTimeIndex] = self.__numIpBytes[duplicateTimeIndex] + numIpBytes
            else:
                self.__times.append(time)

                self.__rlcPaths.append(rlcPath)
                self.__rlcPathTimes.append(time)

                self.__routeStatuses.append(routeStatus)
                self.__routeStatusTimes.append(time)

                self.__startCounts.append(startCount)
                self.__startCountTimes.append(time)

                self.__endCounts.append(endCount)
                self.__endCountTimes.append(time)

                self.__rlcEndSns.append(rlcEndSn)
                self.__rlcEndSnTimes.append(time)

                self.__numIpPackets.append(numIpPackets)
                self.__numIpPacketTimes.append(time)

                self.__numIpBytes.append(numIpBytes)
                self.__numIpByteTimes.append(time)
    
    def draw_num_IP_packets(self, figure_size=(100,4), start_time = 0, end_time = math.inf):
        if end_time == math.inf:
            end_time = int(self.__times[-1])

        plt.figure(figsize=figure_size)
        plt.xlabel("Number of IP Packets")
        plt.ylabel("Frequency")
        plt.title("IP Packet Assignment: {} ms - {} ms".format(start_time, end_time if end_time!= math.inf else "inf"))

        s_idx, e_idx = np.searchsorted(self.__times, [start_time, end_time])
        numIpPacketsX = np.array(self.__numIpPacketTimes[s_idx : e_idx])
        numIpPacketsY = np.array(self.__numIpPackets[s_idx : e_idx])

        #plt.scatter(numIpPacketsX, numIpPacketsY)
        plt.hist(numIpPacketsY)
        #plt.plot(throughput_x, throughput_y, '-o')
        plt.savefig("numberIpPackets_{}_ms_{}_ms.png".format(start_time, end_time), dpi=200)
        #plt.show()
    
    def draw_num_IP_packets_assignment(self,figure_size=(100,4), start_time = 0, end_time = math.inf):
        if end_time == math.inf:
            end_time = int(self.__times[-1])
        
        plt.figure(figsize=figure_size)
        plt.xlabel('Time in ms')
        plt.ylabel("Number of IP Packets")

        plt.title("Number of IP Packets Assignment: {} ms - {} ms".format(start_time, end_time if end_time!= math.inf else "inf"))

        startIndexNumIpPacket, endIndexNumIpPacket = np.searchsorted(self.__times, [start_time,end_time])
        numIpPacketX = np.array(self.__numIpPacketTimes[startIndexNumIpPacket : endIndexNumIpPacket])
        numIpPacketY = np.array(self.__numIpPackets[startIndexNumIpPacket : endIndexNumIpPacket])

        #print(numIpPacketX)
        #print(numIpPacketY)
        
        plt.scatter(numIpPacketX, numIpPacketY, s=2)
        #plt.plot(numIpPacketX, numIpPacketY)

        #plt.legend()

        plt.savefig("IP_packets_assignment_{}_ms_{}_ms.png".format(start_time, end_time if end_time!= math.inf else "inf"), dpi=200)
        #plt.show()
    
    def draw_num_IP_bytes_assignment(self,figure_size=(100,4), start_time = 0, end_time = math.inf):
        if end_time == math.inf:
            end_time = int(self.__times[-1])
        
        plt.figure(figsize=figure_size)
        plt.xlabel('Time in ms')
        plt.ylabel("Number of IP Bytes")
        plt.yticks(np.arange(0, 50000, 1000))
        plt.ylim(0, 10000)

        plt.title("Number of IP Bytes Assignment: {} ms - {} ms".format(start_time, end_time if end_time!= math.inf else "inf"))

        startIndexNumIpBytes, endIndexNumIpBytes = np.searchsorted(self.__times, [start_time,end_time])
        numIpBytesX = np.array(self.__numIpByteTimes[startIndexNumIpBytes : endIndexNumIpBytes])
        numIpPacketY = np.array(self.__numIpBytes[startIndexNumIpBytes : endIndexNumIpBytes])

        plt.scatter(numIpBytesX, numIpPacketY, s=2)
        #plt.plot(numIpBytesX, numIpPacketY)

        #plt.legend()

        plt.savefig("IP_bytes_assignment_{}_ms_{}_ms.png".format(start_time, end_time if end_time!= math.inf else "inf"), dpi=200)
        #plt.show()

    def draw_rlc_path(self, figure_size=(100,4), start_time = 0, end_time = math.inf):
        if end_time == math.inf:
            end_time = int(self.__times[-1])

        fig, ax = plt.subplots()

        fig.patch.set_visible(False)
        ax.axis('off')
        ax.axis('tight')

        s_idx, e_idx = np.searchsorted(self.__times, [start_time, end_time])
        rlcPathX = np.array(self.__rlcPathTimes[s_idx : e_idx])
        rlcPathY = np.array(self.__rlcPaths[s_idx : e_idx])

        cols = ["RLC Path Value", "Frequency Percentage"]

        uniques, counts = np.unique(rlcPathY, return_counts = True)
        percentages = dict(zip(uniques, counts * 100 / len(rlcPathY)))

        cell_text = []
        probabilities = []
        for rlcPathVal, valProbability in percentages.items():
            cell_text.append([f'"{rlcPathVal}"', valProbability])
            probabilities.append(valProbability)
        for probability in probabilities:
            if 100 == probability:
                cell_text.append(["Other", 0.0])
        
        rlcPathTable = plt.table(cellText = cell_text, colLabels = cols, loc = 'center')
        rlcPathTable.scale(1, 1.5)
        plt.savefig("rlcPaths_{}_ms_{}_ms.png".format(start_time, end_time), dpi=200)
    
    def draw_route_status(self, figure_size=(100,4), start_time = 0, end_time = math.inf):
        if end_time == math.inf:
            end_time = int(self.__times[-1])

        fig, ax = plt.subplots()

        fig.patch.set_visible(False)
        ax.axis('off')
        ax.axis('tight')

        s_idx, e_idx = np.searchsorted(self.__times, [start_time, end_time])
        routeStatusX = np.array(self.__routeStatusTimes[s_idx : e_idx])
        routeStatusY = np.array(self.__routeStatuses[s_idx : e_idx])

        cols = ["Route Status Value", "Frequency Percentage"]

        uniques, counts = np.unique(routeStatusY, return_counts = True)
        percentages = dict(zip(uniques, counts * 100 / len(routeStatusY)))

        cell_text = []
        probabilities = []
        for routeStatusVal, valProbability in percentages.items():
            cell_text.append([f'"{routeStatusVal}"', valProbability])
            probabilities.append(valProbability)
        for probability in probabilities:
            if 100 == probability:
                cell_text.append(["Other", 0.0])
        
        routeStatusTable = plt.table(cellText = cell_text, colLabels = cols, loc = 'center')
        routeStatusTable.scale(1, 1.5)
        plt.savefig("routeStatuses_{}_ms_{}_ms.png".format(start_time, end_time), dpi=200)

        '''
        if end_time == math.inf:
            end_time = int(self.__times[-1])

        plt.figure(figsize=figure_size)
        plt.xlabel("Route Status Value")
        plt.ylabel("Frequency")
        plt.title("Route Statuses: {} ms - {} ms".format(start_time, end_time if end_time!= math.inf else "inf"))

        s_idx, e_idx = np.searchsorted(self.__times, [start_time, end_time])
        routeStatusX = np.array(self.__routeStatusTimes[s_idx : e_idx])
        routeStatusY = np.array(self.__routeStatuses[s_idx : e_idx])

        plt.hist(routeStatusY)
        plt.savefig("routeStatuses_{}_ms_{}_ms.png".format(start_time, end_time), dpi=200)
        '''

    def draw_end_count_assignment(self,figure_size=(100,4), start_time = 0, end_time = math.inf):
        if end_time == math.inf:
            end_time = int(self.__times[-1])
        
        plt.figure(figsize=figure_size)
        plt.xlabel('Time in ms')
        plt.ylabel("End Count Sequence Number")

        plt.title("End Count Sequence Number Assignment: {} ms - {} ms".format(start_time, end_time if end_time!= math.inf else "inf"))

        startIndexEndCount, endIndexEndCount = np.searchsorted(self.__times, [start_time,end_time])
        endCountX = np.array(self.__endCountTimes[startIndexEndCount : endIndexEndCount])
        endCountY = np.array(self.__endCounts[startIndexEndCount : endIndexEndCount])

        plt.scatter(endCountX, endCountY, s=5)
        
        #plt.plot(endCountX, endCountY)

        #plt.legend()

        plt.savefig("end_count_assignment_{}_ms_{}_ms.png".format(start_time, end_time if end_time!= math.inf else "inf"), dpi=200)
        #plt.show()

    def draw_rlc_end_sn_assignment(self,figure_size=(100,4), start_time = 0, end_time = math.inf):
        if end_time == math.inf:
            end_time = int(self.__times[-1])
        
        plt.figure(figsize=figure_size)
        plt.xlabel('Time in ms')
        plt.ylabel("RLC End Sequence Number")

        plt.title("RLC End Sequence Number Assignment: {} ms - {} ms".format(start_time, end_time if end_time!= math.inf else "inf"))

        startIndexRlcEndSn, endIndexRlcEndSn = np.searchsorted(self.__times, [start_time,end_time])
        rlcEndSnX = np.array(self.__rlcEndSnTimes[startIndexRlcEndSn : endIndexRlcEndSn])
        rlcEndSnY = np.array(self.__rlcEndSns[startIndexRlcEndSn : endIndexRlcEndSn])

        plt.scatter(rlcEndSnX, rlcEndSnY, s=5)
        
        #plt.plot(rlcEndSnX, rlcEndSnY)

        #plt.legend()

        plt.savefig("rlc_ed_sn_assignment_{}_ms_{}_ms.png".format(start_time, end_time if end_time!= math.inf else "inf"), dpi=200)
        #plt.show()