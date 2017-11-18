#!/usr/bin/python
# Filename: offline-l2analysis-example.py

import os
import sys

from mobile_insight.monitor import OfflineReplayer
from mobile_insight.analyzer.lte_mac_corruption_analyzer import LteMacCorruptAnalyzer
from mobile_insight.analyzer.lte_mac_loss_cause_analyzer import LteMacLossCauseAnalyzer
from mobile_insight.analyzer.lte_rlc_retx_analyzer import LteRlcRetxAnalyzer

if __name__ == "__main__":
	src = OfflineReplayer()
	src.set_input_path(sys.argv[1])

	lteAnalyzer = LteMacCorruptAnalyzer()
	lteAnalyzer.set_source(src)

	src.run()