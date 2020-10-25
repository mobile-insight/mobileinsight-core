#!/usr/bin/python

import os
import sys
import shutil
import traceback

import matplotlib.pyplot as plt
import numpy as np

from mobile_insight.monitor import OfflineReplayer
from mobile_insight.analyzer import UplinkLatencyAnalyzer



def uplink_latency_analysis():
    src = OfflineReplayer()
    # src.set_input_path("./logs/latency_sample.mi2log")
    src.set_input_path(sys.argv[1])
    # print (sys.argv[1])

    analyzer = UplinkLatencyAnalyzer()
    analyzer.set_source(src)

    src.run()

    return analyzer


stats = uplink_latency_analysis()
# print stats.all_packets
# print stats.cum_err_block
# print stats.cum_block

total_latency = 0
total_wait = 0
total_trans = 0
total_retx = 0

total_retx = 8 * stats.cum_err_block[0]
for latency in stats.all_packets:
  total_wait += latency['Waiting Latency']
  total_trans += latency['Tx Latency']
  total_retx += latency['Retx Latency']

total_latency = total_wait + total_trans + total_retx
n = len(stats.all_packets)

if (n > 0):
  print ("Average latency is:", float(total_latency) / n)
  print ("Average waiting latency is:", float(total_wait) / n)
  print ("Average tx latency is:", float(total_trans) / n)
  print ("Average retx latency is:", float(total_retx) / n)
else:
  print ("Certain message type(s) missing in the provided log.")

