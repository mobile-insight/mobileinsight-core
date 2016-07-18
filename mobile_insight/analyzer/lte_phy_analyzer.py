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


    def set_source(self,source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)

        #Phy-layer logs
        source.enable_log("LTE_PHY_PDSCH_Packet")

    def rnti_conversion(self,rnti):
        """
        Convert RNTI to corresponding category

        :param rnti: RNTI from the message
        :type rnti: int
        """
        if rnti == 0:
            return "unknown"
        elif rnti ==  0xFFFD:
            return "M-RNTI"
        elif rnti == 0xFFFE:
            return "P-RNTI"
        elif rnti == 0xFFFF:
            return "SI-RNTI"
        elif rnti >= 0xFFF4 and rnti <=0xFFFC:
            return "reserved"    

    def __msg_callback(self,msg):

        if msg.type_id=="LTE_PHY_PDSCH_Packet":
            log_item = msg.data.decode()

            if not self.init_timestamp:
                self.init_timestamp = log_item['timestamp']
                self.log_info("0 "
                + str(log_item["MCS 0"])+" "
                + str(log_item["MCS 1"])+" "
                + str(log_item["TBS 0"])+" "
                + str(log_item["TBS 1"])+" "
                + str(log_item["PDSCH RNTI Type"]))
            else:
                self.log_info(str((log_item['timestamp']-self.init_timestamp).total_seconds())+" "
                + str(log_item["MCS 0"])+" "
                + str(log_item["MCS 1"])+" "
                + str(log_item["TBS 0"])+" "
                + str(log_item["TBS 1"])+" "
                + str(log_item["PDSCH RNTI Type"])) 


            # if not self.init_timestamp:
            #     self.init_timestamp = log_item['timestamp']
            #     self.log_info("0s "
            #     + "MCS0=" + str(log_item["MCS 0"])+" "
            #     + "MCS1=" + str(log_item["MCS 1"])+" "
            #     + "TBS0=" + str(log_item["TBS 0"])+" "
            #     + "TBS1=" + str(log_item["TBS 1"])+" "
            #     + "C-RNTI=" + str(log_item["PDSCH RNTI Type"]))
            # else:
            #     self.log_info(str((log_item['timestamp']-self.init_timestamp).total_seconds())+"s "
            #     + "MCS0=" + str(log_item["MCS 0"])+" "
            #     + "MCS1=" + str(log_item["MCS 1"])+" "
            #     + "TBS0=" + str(log_item["TBS 0"])+" "
            #     + "TBS1=" + str(log_item["TBS 1"])+" "
            #     + "C-RNTI=" + str(log_item["PDSCH RNTI Type"])) 
        	
