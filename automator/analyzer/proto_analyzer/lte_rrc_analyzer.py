#! /usr/bin/env python
"""
lte_rrc_analyzer.py

A LTE_RRC analyzer
	- Maintain configurations from the SIB
	- Connection setup/release statistics
	- RRCReconfiguration statistics
	- ...

Author: Yuanjie Li
"""

import xml.etree.ElementTree as ET
from ..msg_flter import MsgFilter

class LTE_RRC_Analyzer(MsgFilter):

	lte_rrc_sib_config={}	# CellID -> SIB configuration
	
