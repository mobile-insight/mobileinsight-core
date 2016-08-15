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
    0:    1.0911,
    1:    1.8289,
    2:    2.2541,
    3:    2.5779,
    4:    3.1557,
    5:    4.8534,
    6:    5.7557,
    7:    6.8142,
    8:    7.3716,
    9:    7.5516,
    10: 10.29, 
    11: 36.089,
    12: 41.667,
    13: 38.477,
    14: 31.359,
    15: 23.774,
}

class LtePhyAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)

        self.init_timestamp=None


        # Record per-second downlink bandwidth
        self.lte_dl_bw = 0 # Downlink bandwidth (from PDSCH)
        self.lte_ul_bw = 0 # Uplink bandwidth (from PUSCH DCI grants)
        self.lte_ul_grant_utilized = 0 # Uplink grant utilization (in bits)
        self.prev_timestamp_dl = None # Track timestamp to calculate avg DL bandwidth
        self.prev_timestamp_ul = None # Track timestamp to calculate avg DL bandwidth
        self.avg_window = 1.0 # Average link BW time window (in seconds)

        # Record last observed CQI (for DL bandwidth prediction)
        self.cur_cqi0 = 0
        self.cur_cqi1 = 0
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
        source.enable_log("LTE_MAC_UL_Tx_Statistics") # includes PUSCH grant usage info (~10 msg/s)

    def callback_pdsch(self,msg):
        """
        Dump PDSCH bandwidth and modulation

        :param msg: raw LTE_PHY_PDSCH_Packet packet
        """
        log_item = msg.data.decode()

        if not self.init_timestamp:
            self.init_timestamp = log_item['timestamp']
            
        if not self.prev_timestamp_dl:
            self.prev_timestamp_dl = log_item['timestamp']

        # Log runtime PDSCH information
        # self.log_info(str((log_item['timestamp']-self.init_timestamp).total_seconds())+" "
        # + str(log_item["MCS 0"])+" "
        # + str(log_item["MCS 1"])+" "
        # + str(log_item["TBS 0"])+" "
        # + str(log_item["TBS 1"])+" "
        # + str(log_item["PDSCH RNTI Type"])) 

        self.log_debug(str(log_item['timestamp']) + " "
        + "MCS0=" + str(log_item["MCS 0"]) + " "
        + "MCS1=" + str(log_item["MCS 1"]) + " "
        + "TBS0=" + str(log_item["TBS 0"]) + "bits "
        + "TBS1=" + str(log_item["TBS 1"]) + "bits "
        + "C-RNTI=" + str(log_item["PDSCH RNTI Type"])) 

        # Broadcast bandwidth to other apps
        if log_item["PDSCH RNTI Type"] == "C-RNTI":

            self.cur_tbs = (log_item["TBS 0"]+log_item["TBS 1"])
            self.lte_dl_bw += (log_item["TBS 0"]+log_item["TBS 1"])

            if (log_item['timestamp']-self.prev_timestamp_dl).total_seconds() >= self.avg_window:
                bcast_dict = {}
                bandwidth = self.lte_dl_bw/((log_item['timestamp']-self.prev_timestamp_dl).total_seconds()*1000000.0)
                pred_bandwidth = self.predict_bw()
                bcast_dict['Bandwidth (Mbps)'] = str(round(bandwidth,2))

                # """
                # TEST PURPOSE
                # """
                # if pred_bandwidth:
                #     bcast_dict['Bandwidth (Mbps)'] = str(round(pred_bandwidth,2)) #TEST
                # else:
                #     bcast_dict['Bandwidth (Mbps)'] = str(round(bandwidth,2))
                # """
                # END OF TEST PURPOSE
                # """

                if pred_bandwidth:
                    bcast_dict['Predicted Bandwidth (Mbps)'] = str(round(pred_bandwidth,2))
                else:
                    # Use current PDSCH bandwidth as estimation
                    bcast_dict['Predicted Bandwidth (Mbps)'] = str(round(bandwidth,2))

                bcast_dict['Modulation 0'] = str(log_item["MCS 0"])
                bcast_dict['Modulation 1'] = str(log_item["MCS 1"])

                # Log/notify average bandwidth
                self.log_info(str(log_item['timestamp']) + ' LTE_DL_Bandwidth=' + bcast_dict['Bandwidth (Mbps)'] + "Mbps")
                self.broadcast_info('LTE_DL_BW',bcast_dict)

                # Reset bandwidth statistics
                self.prev_timestamp_dl = log_item['timestamp']
                self.lte_dl_bw = 0



    def callback_pusch(self,msg):
        """
        Callback for LTE_PHY_PUSCH_CSF.
        Currently it updates CQI.

        :param msg: raw LTE_PHY_PUSCH_CSF packet
        """

        log_item = msg.data.decode()
        self.cur_cqi0 = log_item['WideBand CQI CW0']
        self.cur_cqi1 = log_item['WideBand CQI CW1']



    def callback_pusch_grant(self,msg):

        log_item = msg.data.decode()

        if not self.init_timestamp:
            self.init_timestamp = log_item['timestamp']

        if not self.prev_timestamp_ul:
           self.prev_timestamp_ul = log_item['timestamp'] 

        # Calculate PUSCH uplink utilization
        grant_received = 0
        grant_utilized = 0
        grant_utilization = 0

        for i in range(0,log_item['Num SubPkt']):
            grant_received += log_item['Subpackets'][i]['Sample']['Grant received']
            grant_utilized += log_item['Subpackets'][i]['Sample']['Grant utilized']

        if grant_received != 0:
            grant_utilization = round(100.0*grant_utilized/grant_received, 2)

        self.log_debug(str(log_item['timestamp'])+" PUSCH UL grant: received="+str(grant_received)+" bytes"
                     +" used="+str(grant_utilized)+" bytes"
                     +" utilization="+str(grant_utilization)+"%")

        self.lte_ul_grant_utilized += grant_utilized*8
        self.lte_ul_bw += grant_received*8

        if (log_item['timestamp']-self.prev_timestamp_ul).total_seconds() >= self.avg_window:

            bcast_dict = {}
            bandwidth = self.lte_ul_bw/((log_item['timestamp']-self.prev_timestamp_ul).total_seconds()*1000000.0)
            grant_utilization = self.lte_ul_grant_utilized/((log_item['timestamp']-self.prev_timestamp_ul).total_seconds()*1000000.0)
            bcast_dict['Bandwidth (Mbps)'] = str(round(bandwidth,2))
            bcast_dict['Utilized (Mbps)'] = str(round(grant_utilization,2))
            if self.lte_ul_bw:
                bcast_dict['Utilization (%)'] = str(round(self.lte_ul_grant_utilized*100.0/self.lte_ul_bw,2));
            else:
                bcast_dict['Utilization (%)'] = '0'


            # self.log_info(str(log_item['timestamp']) + ' LTE_UL_Bandwidth=' + bcast_dict['Bandwidth (Mbps)'] + "Kbps "
            #              + "UL_utilized="+bcast_dict['Utilized (Mbps)']+"Kbps "
            #              + "Utilization="+bcast_dict['Utilization (%)']+"%")

            self.log_info(str(log_item['timestamp']) + ' UL ' + bcast_dict['Bandwidth (Mbps)'] + " "
                         + bcast_dict['Utilized (Mbps)']+" "
                         + bcast_dict['Utilization (%)']+"")


            self.broadcast_info('LTE_UL_BW',bcast_dict)
            # Reset bandwidth statistics
            self.prev_timestamp_ul = log_item['timestamp']
            self.lte_ul_bw = 0
            self.lte_ul_grant_utilized = 0



    def predict_bw(self):
        """
        Predict bandwidth based on CQI
        Currently it implements a naive solution based on pre-trained CQI->BW table

        """
        if self.cur_cqi0 in cqi_to_bw:
            return cqi_to_bw[self.cur_cqi0]
        else:
            return None



    def __msg_callback(self,msg):

        if msg.type_id == "LTE_PHY_PDSCH_Packet":
            self.callback_pdsch(msg)
        elif msg.type_id == "LTE_PHY_PUSCH_CSF":
            self.callback_pusch(msg)
        elif msg.type_id ==  "LTE_MAC_UL_Tx_Statistics":
            self.callback_pusch_grant(msg)
        	
