#!/usr/bin/python
# Filename: rrc_config_analyzer.py
"""
rrc_config_analyzer.py
An KEI analyzer to reveal RRC config information

Author: Zhehui Zhang
"""

__all__ = ["RrcConfigAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .kpi_analyzer import KpiAnalyzer


class RrcConfigAnalyzer(KpiAnalyzer):
    """
    An KPI analyzer to monitor and manage RRC connection success rate
    """

    def __init__(self):
        KpiAnalyzer.__init__(self)

        self.current_kpi = {'SR_CONFIG_IDX': 0.00}

        for kpi in self.current_kpi:
            self.register_kpi("Configuration",kpi,self.__rrc_config_callback)

        # add callback function
        self.add_source_callback(self.__rrc_config_callback)

    def set_source(self,source):
        """
        Set the trace source. Enable the LTE RRC messages.

        :param source: the trace source.
        :type source: trace collector
        """
        KpiAnalyzer.set_source(self,source)
        #enable LTE RRC log
        source.enable_log("LTE_RRC_OTA_Packet")

    def __rrc_config_callback(self, msg):
        # deal with RRC OTA
        if msg.type_id == "LTE_RRC_OTA_Packet":
            log_item = msg.data.decode()
            log_item_dict = dict(log_item)
            if 'Msg' in log_item_dict:
                log_xml = ET.XML(log_item_dict['Msg'])
                for field in log_xml.iter('field'):
                    if field.get('name') == 'lte-rrc.sr_ConfigIndex':
                        sr_sonfigidx = int(field.get('show'))
                        if sr_sonfigidx < 4:
                            sr_period = 5
                        elif 4 < sr_sonfigidx < 15:
                            sr_period = 10
                        elif 14 < sr_sonfigidx < 35:
                            sr_period = 20
                        elif 34 < sr_sonfigidx < 75:
                            sr_period = 40
                        elif 74 < sr_sonfigidx < 155:
                            sr_period = 80
                        elif 154 < sr_sonfigidx < 157:
                            sr_period = 2
                        elif sr_sonfigidx == 157:
                            sr_period = 1
                        else:
                            self.log_warning("Unknown sr_ConfigIndex: " + str(sr_sonfigidx))
                            continue
                        self.log_info("SR period: " + str(sr_period) + ' ms, SR ConfigIdx: ' + str(sr_sonfigidx))
                        bcast_dict = {}
                        bcast_dict['period'] = str(sr_period)
                        bcast_dict['config idx'] = str(sr_sonfigidx)
                        bcast_dict['timestamp'] = str(msg.timestamp)
                        self.broadcast_info('SR_CONFIGIDX', bcast_dict)
                        self.store_kpi('KPI_CONFIGURATION_SR_CONFIG_IDX', str(sr_period), msg.timestamp)
        return 0





