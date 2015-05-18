#! /usr/bin/env python
"""
msg_file.py

Dump the message to a file

Author: Yuanjie Li
"""

from analyzer import *

import xml.etree.ElementTree as ET
import io
import json

class MsgFile(Analyzer):

	def dump_message(self,msg):
		# json.dump(self.file,msg)
		log_item_dict = dict(msg.data)
		for key in log_item_dict:
			self.file.write('\n\n'+key+':'+str(log_item_dict[key]))



	def __init__(self,filename):
		Analyzer.__init__(self)

		self.file=open(filename,'w')
		self.add_source_callback(self.dump_message)

	def __del__(self):
		Analyzer.__del__(self)
		self.file.close()