# Usage: python kpi=manager-test.py [dirname]
# Example1: python kpi-manager-test.py logs/volte_sample.qmdl 
# (For testing KPI DEDICATED_BEARER_SR_QCI1)
# Example2: python kpi-manager-test.py logs/mobility_sample.qmdl 
# (For testing KPI RRC, SR, TAU, HO)
# Example3: python kpi-manager-test.py logs/attach_sample.qmdl 
# (For testing KPI ATTACH)
# Example4: python kpi-manager-test.py logs/data_sample.qmdl 
# (For testing KPI DL_TPUT)
# import os
import sys

from mobile_insight.monitor import OfflineReplayer
# from mobile_insight.analyzer import LteMacAnalyzer, LtePdcpGapAnalyzer
from mobile_insight.analyzer.kpi import KPIManager, KpiAnalyzer
import cProfile


def kpi_manager_example(path):

    src = OfflineReplayer()
    src.set_input_path(path)

    kpi_manager = KPIManager()
    # print "All supported KPIs:", str(kpi_manager.list_kpis())

    kpi_manager.enable_kpi("KPI.Accessibility.DEDICATED_BEARER_SR_QCI1_REQ")
    kpi_manager.enable_kpi("KPI.Accessibility.DEDICATED_BEARER_SR_QCI1_SUC")
    kpi_manager.enable_kpi("KPI.Accessibility.RRC_SUC", periodicity='1h')
    kpi_manager.enable_kpi("KPI.Accessibility.RRC_REQ", periodicity='1h')
    kpi_manager.enable_kpi("KPI.Accessibility.SR_REQ", periodicity='1h')
    kpi_manager.enable_kpi("KPI.Accessibility.SR_SUC", periodicity='1h')
    kpi_manager.enable_kpi("KPI.Accessibility.ATTACH_REQ")
    kpi_manager.enable_kpi("KPI.Accessibility.ATTACH_SUC")

    # Test Mobility KPIs
    kpi_manager.enable_kpi("KPI.Mobility.HO_TOTAL", periodicity='1h')
    kpi_manager.enable_kpi("KPI.Mobility.HO_FAILURE", periodicity='1h')
    kpi_manager.enable_kpi("KPI.Mobility.TAU_SUC", periodicity='1h')
    kpi_manager.enable_kpi("KPI.Mobility.TAU_REQ", periodicity='1h')

    # Test Retainability KPIs
    kpi_manager.enable_kpi("KPI.Retainability.RRC_AB_REL") 

    # Test Integrity KPIs
    kpi_manager.enable_kpi("KPI.Integrity.DL_TPUT") 

    # Test data plane KPIs
    # Test experimental KPIs - data plane
    # kpi_manager.enable_kpi("KPI.Wireless.SYMBOL_ERROR_RATE")

    # Test experimental KPIs - handover
    # kpi_manager.enable_kpi("KPI.Mobility.HANDOVER_PREDICTION")
    # kpi_manager.enable_kpi("KPI.Mobility.HANDOVER_LATENCY")

    kpi_manager.set_source(src)

    src.run()


if __name__ == '__main__':
    kpi_manager_example(sys.argv[1])



