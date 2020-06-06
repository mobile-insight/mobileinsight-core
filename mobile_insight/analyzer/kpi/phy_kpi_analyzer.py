#!/usr/bin/python
# Filename: phy_kpi_analyzer.py
"""
phy_kpi_analyzer.py
Physical-layer KPI analyzer

Author: Yuanjie Li
"""

__all__ = ["PhyKpiAnalyzer"]

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
from .kpi_analyzer import KpiAnalyzer

class PhyKpiAnalyzer(KpiAnalyzer):
    """
    An KPI analyzer to monitor and manage uplink latency breakdown
    """

    def __init__(self):
        KpiAnalyzer.__init__(self)
        self.include_analyzer('LtePhyAnalyzer', [self.__kpi_callback])

        self.register_kpi("Wireless", "MODULATION_SCHEME", self.__kpi_callback, None)
        self.register_kpi("Wireless", "PUSCH_CQI", self.__kpi_callback, None)

    def __kpi_callback(self,msg):
        if msg.type_id == "LtePhyAnalyzer":
            if msg.data == "MODULATION_SCHEME":
                self.__update_modulation_scheme()
            elif msg.data == "PUSCH_CQI":
                self.__update_pusch_cqi()




    def __update_modulation_scheme(self):
        kpi={}
        kpi['QPSK'] = self.get_analyzer('LtePhyAnalyzer').mcs_qpsk_count
        kpi['16QAM'] = self.get_analyzer('LtePhyAnalyzer').mcs_16qam_count
        kpi['64QAM'] = self.get_analyzer('LtePhyAnalyzer').mcs_64qam_count
        self.upload_kpi("KPI.Wireless.MODULATION_SCHEME", kpi)

        # self.log_info(str(kpi))
        
    def __update_pusch_cqi(self):
        kpi={}
        kpi['CQI-CW0'] = self.get_analyzer('LtePhyAnalyzer').cur_cqi0
        kpi['CQI-CW1'] = self.get_analyzer('LtePhyAnalyzer').cur_cqi1
        self.upload_kpi("KPI.Wireless.PUSCH_CQI", kpi)

