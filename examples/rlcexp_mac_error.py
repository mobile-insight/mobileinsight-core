#!/usr/bin/python
# Filename: rlcexp_mac_error.py
import os
import sys

from mobile_insight.monitor import OfflineReplayer
from mobile_insight.analyzer.lte_mac_corruption_analyzer import LteMacCorruptAnalyzer

__all__ = ["LteMacCorruptAnalyzer"]

if __name__ == "__main__":
	src = OfflineReplayer()
	src.set_input_path(sys.argv[1])

	lteAnalyzer = LteMacCorruptAnalyzer()
	lteAnalyzer.set_source(src)

	src.run()

	fout = open(sys.argv[2], 'w')
	for key in lteAnalyzer.total_blocks:
		fout.write("{},{},{},{},{}\n".format(key, lteAnalyzer.total_blocks[key], lteAnalyzer.corrupt_blocks[key], lteAnalyzer.total_new_blocks[key], lteAnalyzer.loss_blocks[key]))
	# fout.write(str(lteAnalyzer.total_blocks) + ',' + str(lteAnalyzer.corrupt_blocks) + ',' + str(lteAnalyzer.total_new_blocks) + ',' + str(lteAnalyzer.loss_blocks) + '\n')
	# for key in lteAnalyzer.bearer_entity:
	# 	for value in lteAnalyzer.bearer_entity[key].res:
	# 		fout.write(str(value) + '\n')
	fout.close()

	# for item in lteAnalyzer.obj:
	# 	print str(item)
	# print lteUlAnalyzer.flip_cnt, lteUlAnalyzer.retx_cnt

	# fout = open(sys.argv[2], 'w')

	# for record in lteUlAnalyzer.res:
	# 	fout.write(str(record) + '\n')

	# fout.close()
