#!/usr/bin/python
# Filename: rlcexp_retx.py
import os
import sys

from mobile_insight.monitor import OfflineReplayer
from mobile_insight.analyzer.lte_rlc_retx_analyzer import LteRlcRetxAnalyzer

__all__ = ["LteRlcRetxAnalyzer"]

if __name__ == "__main__":
	src = OfflineReplayer()
	src.set_input_path(sys.argv[1])

	lteAnalyzer = LteRlcRetxAnalyzer()
	lteAnalyzer.set_source(src)

	src.run()

	fout = open(sys.argv[2], 'w')
	for key in lteAnalyzer.bearer_entity:
		for value in lteAnalyzer.bearer_entity[key].res:
			fout.write("{}, {}, {}, {}, {}\n".format(str(value[0]), value[1], value[2], value[3], value[4]))
			# fout.write(str(value) + '\n')
	fout.close()

	# for item in lteAnalyzer.obj:
	# 	print str(item)
	# print lteUlAnalyzer.flip_cnt, lteUlAnalyzer.retx_cnt

	# fout = open(sys.argv[2], 'w')

	# for record in lteUlAnalyzer.res:
	# 	fout.write(str(record) + '\n')

	# fout.close()
