#!/usr/bin/python
# Filename: lte_phy_analyzer.py
"""
A 4G PHY analyzer to get Modulation and coding Scheme (MCS)

Author: Yuanjie Li
"""


from mobile_insight.analyzer.analyzer import *

__all__=["LtePhyAnalyzer"]

"""
 A CQI->PDSCH_TBS (Mbps) mapping table.
 It is learned based on results in ownCloud/PhyAnalysis.
 For each CQI, we map it to 95th of the TBS0+TBS1 (in Mbps)
"""
cqi_to_bw={
    0: 0,
    1: 7.224,
    2: 7.224,
    3: 6.968,
    4: 10.296,
    5: 13.536,
    6: 21.36,
    7: 30.576,
    8: 34.216,
    9: 32.856,
    10: 46.672,
    11: 46.416,
    12: 46.888,
    13: 55.056,
    14: 57.336,
    15: 70.32,
}

class LtePhyAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

        self.init_timestamp=None


        # Record per-second bandwidth
        self.lte_bw = 0
        self.prev_timestamp = None
        self.avg_window = 1.0 # Average link BW time window (1.0 is no average by default)

        # Record last observed CQI (for prediction)
        self.cur_cqi0 = None
        self.cur_cqi1 = None
        self.cur_tbs = None

    def set_source(self,source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)

        #Phy-layer logs
        source.enable_log("LTE_PHY_PDSCH_Packet")
        source.enable_log("LTE_PHY_PUSCH_CSF")

    def callback_pdsch(self,msg):
        """
        Dump PDSCH bandwidth and modulation

        :param msg: raw LTE_PHY_PDSCH_Packet packet
        """
        log_item = msg.data.decode()

        if not self.init_timestamp:
            self.init_timestamp = log_item['timestamp']
            self.prev_timestamp = log_item['timestamp']

        # Log runtime PDSCH information
        # self.log_info(str((log_item['timestamp']-self.init_timestamp).total_seconds())+" "
        # + str(log_item["MCS 0"])+" "
        # + str(log_item["MCS 1"])+" "
        # + str(log_item["TBS 0"])+" "
        # + str(log_item["TBS 1"])+" "
        # + str(log_item["PDSCH RNTI Type"])) 

        # self.log_info(str((log_item['timestamp']-self.init_timestamp).total_seconds())+"s "
        # + "MCS0=" + str(log_item["MCS 0"])+" "
        # + "MCS1=" + str(log_item["MCS 1"])+" "
        # + "TBS0=" + str(log_item["TBS 0"])+" "
        # + "TBS1=" + str(log_item["TBS 1"])+" "
        # + "C-RNTI=" + str(log_item["PDSCH RNTI Type"])) 

        # Broadcast bandwidth to other apps
        if log_item["PDSCH RNTI Type"] == "C-RNTI":

            self.cur_tbs = (log_item["TBS 0"]+log_item["TBS 1"])
            self.lte_bw += (log_item["TBS 0"]+log_item["TBS 1"])

            if (log_item['timestamp']-self.prev_timestamp).total_seconds() >= self.avg_window:
                bcast_dict = {}
                bandwidth = self.lte_bw/((log_item['timestamp']-self.prev_timestamp).total_seconds()*1000000.0)
                pred_bandwidth = self.predict_bw()
                bcast_dict['Bandwidth (Mbps)'] = str(round(bandwidth,2))
                if pred_bandwidth:
                    bcast_dict['Predicted Bandwidth (Mbps)'] = str(round(pred_bandwidth,2))
                bcast_dict['Modulation 0'] = str(log_item["MCS 0"])
                bcast_dict['Modulation 1'] = str(log_item["MCS 1"])
                self.broadcast_info('LTE_BW',bcast_dict)
                # Reset bandwidth statistics
                self.prev_timestamp = log_item['timestamp']
                self.lte_bw = 0


    def callback_pusch(self,msg):
        """
        Callback for LTE_PHY_PUSCH_CSF.
        Currently it updates CQI.

        :param msg: raw LTE_PHY_PUSCH_CSF packet
        """

        log_item = msg.data.decode()
        self.cur_cqi0 = log_item['WideBand CQI CW0']
        self.cur_cqi1 = log_item['WideBand CQI CW1']



    def predict_bw(self):
        """
        Predict bandwidth based on CQI
        Currently it implements a naive solution based on pre-trained CQI->BW table

        """
        if self.cur_cqi0 in cqi_to_bw:
            self.log_info("Predicted BW = "+str(cqi_to_bw[self.cur_cqi0])+"Mbps")
            return cqi_to_bw[self.cur_cqi0]
        else:
            return None



    def __msg_callback(self,msg):

        if msg.type_id == "LTE_PHY_PDSCH_Packet":
            self.callback_pdsch(msg)
        elif msg.type_id == "LTE_PHY_PUSCH_CSF":
            self.callback_pusch(msg)
        	
