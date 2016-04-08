#!/usr/bin/python
# Filename: online_monitor.py
"""
A universal, cross-platform MobileInsight online monitor. 

It abstracts the low-level complexity of platform-dependent monitors. 
It wraps monitors for mobile version (currently Android only) and desktop version.

Author: Yuanjie Li
"""

__all__ = ["OnlineMonitor"]



#Test the OS version
is_android=False
try:
    print "test 1"
    from jnius import autoclass #For Android
    print "test 2"
    from android_dev_diag_monitor import AndroidDevDiagMonitor
    print "test 3"

    is_android=True

    print "test 4"

    class OnlineMonitor(AndroidDevDiagMonitor):
        def __init__(self):
            AndroidDevDiagMonitor.__init__(self)
            print "As AndroidDevDiagMonitor"

        def set_serial_port(self, phy_ser_name):
            """
            NOT USED: Compatability with DMCollector

            :param phy_ser_name: the serial port name (path)
            :type phy_ser_name: string
            """
            print "WARNING: Android version does not need to configure serial port"

        def set_baudrate(self, rate):
            """
            NOT USED: Compatability with DMCollector

            :param rate: the baudrate of the port
            :type rate: int
            """
            print "WARNING: Android version does not need to configure baudrate"

except Exception, e:

    import traceback
    print "Yuanjie:" + str(traceback.format_exc())

    #not used, but bugs may exist on laptop
    from dm_collector.dm_collector import DMCollector
    is_android=False

    class OnlineMonitor(DMCollector):
        def __init__(self):
            DMCollector.__init__(self)
            print "As DMCollector"