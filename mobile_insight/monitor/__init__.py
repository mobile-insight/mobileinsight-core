# -*- coding: utf-8 -*-

__all__ = [
            "AndroidDevDiagMonitor",
            "Monitor",
            "DMCollector",  ### P4A: THIS LINE WILL BE DELETED ###
            "OfflineReplayer",
            "OnlineMonitor", 
            "AndroidMuxrawMonitor"
            # "muxraw_parser"
            ]

is_android=False
try:
    from jnius import autoclass #For Android
    is_android = True
except Exception, e:
    #not used, but bugs may exist on laptop
    is_android=False


if is_android:
    from android_dev_diag_monitor import AndroidDevDiagMonitor
from monitor import Monitor
from dm_collector import * ### P4A: THIS LINE WILL BE DELETED ###
from offline_replayer import OfflineReplayer
from online_monitor import OnlineMonitor
from android_muxraw_monitor import AndroidMuxrawMonitor
# from muxraw_parser import *
