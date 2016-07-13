#!/usr/bin/python
# Filename: lte_mac_analyzer.py
"""
A 4G MAC-layer analyzer to get Modulation and coding Scheme (MCS)

Author: Yuanjie Li
"""


from mobile_insight.analyzer.analyzer import *
import datetime

__all__=["LteMacAnalyzer"]

class LteMacAnalyzer(Analyzer):

    def __init__(self):
        Analyzer.__init__(self)

        self.add_source_callback(self.__msg_callback)


    def set_source(self,source):
        """
        Set the trace source. Enable the cellular signaling messages

        :param source: the trace source (collector).
        """
        Analyzer.set_source(self,source)

        #Phy-layer logs
        source.enable_log("LTE_MAC_UL_Tx_Statistics")

    def __msg_callback(self,msg):

        if msg.type_id=="LTE_MAC_UL_Tx_Statistics":
            log_item = msg.data.decode()

            grant_received = 0
            grant_utilized = 0
            grant_utilization = 0

            for i in range(0,log_item['Num SubPkt']):
                grant_received += log_item['Subpackets'][i]['Sample']['Grant received']
                grant_utilized += log_item['Subpackets'][i]['Sample']['Grant utilized']

            if grant_received != 0:
                grant_utilization = round(100.0*grant_utilized/grant_received, 2)

            timestamp = datetime.datetime.fromtimestamp(float(msg.timestamp)).strftime('%H:%M:%S.%f')
            self.log_info(timestamp+" MAC UL grant: received="+str(grant_received)+" bytes"
                         +" used="+str(grant_utilized)+" bytes"
                         +" utilization="+str(grant_utilization)+"%")

            # self.log_info(str(grant_received)
            #              +" "+str(grant_utilized)
            #              +" "+str(grant_utilization))
        	
