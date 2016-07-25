#!/usr/bin/python
# Filename: lte_phy_analyzer.py
"""
A 4G PHY analyzer to get Modulation and coding Scheme (MCS)

Author: Yuanjie Li
"""


from mobile_insight.analyzer.analyzer import *

__all__=["LtePhyAnalyzer"]

class LtePhyAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

        self.init_timestamp=None


        # Record per-second bandwidth
        self.lte_bw = 0
        self.prev_timestamp = None
        self.avg_window = 1.0 # Average link BW time window (1.0 is no average by default)


    def set_source(self,source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)

        #Phy-layer logs
        source.enable_log("LTE_PHY_PDSCH_Packet")

    def __msg_callback(self,msg):

        if msg.type_id=="LTE_PHY_PDSCH_Packet":
            log_item = msg.data.decode()

            if not self.init_timestamp:
                self.init_timestamp = log_item['timestamp']
                self.prev_timestamp = log_item['timestamp']
    
            # Log runtime PDSCH information
            self.log_info(str((log_item['timestamp']-self.init_timestamp).total_seconds())+" "
            + str(log_item["MCS 0"])+" "
            + str(log_item["MCS 1"])+" "
            + str(log_item["TBS 0"])+" "
            + str(log_item["TBS 1"])+" "
            + str(log_item["PDSCH RNTI Type"])) 

            # self.log_info(str((log_item['timestamp']-self.init_timestamp).total_seconds())+"s "
            # + "MCS0=" + str(log_item["MCS 0"])+" "
            # + "MCS1=" + str(log_item["MCS 1"])+" "
            # + "TBS0=" + str(log_item["TBS 0"])+" "
            # + "TBS1=" + str(log_item["TBS 1"])+" "
            # + "C-RNTI=" + str(log_item["PDSCH RNTI Type"])) 

            # Broadcast bandwidth to other apps
            if log_item["PDSCH RNTI Type"] == "C-RNTI":
                # bcast_dict={}
                # bcast_dict['Bandwidth (Mbps)'] = str((log_item["TBS 0"]+log_item["TBS 1"])/1000.0)
                # bcast_dict['Modulation 0'] = str(log_item["MCS 0"])
                # bcast_dict['Modulation 1'] = str(log_item["MCS 1"])

                # self.broadcast_info('LTE_BW',bcast_dict)


                self.lte_bw += (log_item["TBS 0"]+log_item["TBS 1"])/1000000.0
                if (log_item['timestamp']-self.prev_timestamp).total_seconds() >= self.avg_window:
                    bcast_dict = {}
                    # bcast_dict['Bandwidth (Mbps)'] = str(round(self.lte_bw/(log_item['timestamp']-self.prev_timestamp).total_seconds(),2))
                    bcast_dict['Bandwidth (Mbps)'] = str(round(self.lte_bw,2))
                    self.broadcast_info('LTE_BW',bcast_dict)
                    # Reset bandwidth statistics
                    self.prev_timestamp = log_item['timestamp']
                    self.lte_bw = 0
        	
