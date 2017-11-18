#!/usr/bin/python
# Filename: rlcexp_retx_interval.py
import os
import sys

from mobile_insight.monitor import OfflineReplayer
# from lte_rlc_retx_interval import LteRlcRetxIntervalAnalyzer
from mobile_insight.analyzer.lte_rlc_reorder_analyzer import LteRlcReorderAnalyzer

__all__ = ["LteRlcReorderAnalyzer"]

if __name__ == "__main__":
	src = OfflineReplayer()
	src.set_input_path(sys.argv[1])

	lteAnalyzer = LteRlcReorderAnalyzer()
	lteAnalyzer.set_source(src)

	src.run()

	fout = open(sys.argv[2], 'w')
	for value in lteAnalyzer.rlc_reorder_list:
		fout.write("{}, {}, {}, {}, {}, {}\n".format(value[0], value[1], value[2], value[3], str(value[4]), value[5]))

	fout.write("{},{}\n".format(lteAnalyzer.rlc_cnt, lteAnalyzer.rlc_retx))
	fout.close()