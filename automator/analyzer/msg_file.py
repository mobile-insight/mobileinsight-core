#! /usr/bin/env python
"""
msg_file.py

Dump the message to a file

Author: Yuanjie Li
"""

from analyzer import *

import xml.etree.ElementTree as ET
import io

class MsgFile(Analyzer):

	def dump_message(self,msg):
		#FIXME: incorrect output!!!
		self.file.write(str(msg))



	def __init__(self,filename):
		Analyzer.__init__(self)

		self.file=open(filename,'wb')
		self.add_source_callback(self.dump_message)

	def __del__(self):
		Analyzer.__del__(self)
		self.file.close()