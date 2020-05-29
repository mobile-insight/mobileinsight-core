#!/usr/bin/python

import sys

from mobile_insight.monitor import OfflineReplayer
from mobile_insight.analyzer import LteDlRetxAnalyzer

if __name__ == "__main__":
	src = OfflineReplayer()
	src.set_input_path(sys.argv[1])

	lteAnalyzer = LteDlRetxAnalyzer()
	lteAnalyzer.set_source(src)

	src.run()

	mac_delay = 0.0
	mac_delay_sample = 0
	
	rlc_delay = 0.0
	rlc_delay_sample = 0

	for _, bearer in lteAnalyzer.bearer_entity.items():
		for item in bearer.mac_retx:
			mac_delay += item['mac_retx']
		mac_delay_sample += len(bearer.mac_retx)

		for item in bearer.rlc_retx:
			rlc_delay += item['rlc_retx']
		rlc_delay_sample += len(bearer.rlc_retx)

	print("Average MAC retx delay is: ", float(mac_delay) / mac_delay_sample)
	print("Average RLC retx delay is:", float(rlc_delay) / rlc_delay_sample)