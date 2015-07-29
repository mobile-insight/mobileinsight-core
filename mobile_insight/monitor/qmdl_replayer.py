#!/usr/bin/python
# Filename: qmdl_replayer.py
"""
A QMDL format replayer

Author: Jiayao Li
"""

__all__ = ["QmdlReplayer"]

import sys
import timeit

from monitor import Monitor, Event
from dm_collector import dm_collector_c, DMLogPacket, FormatError

class QmdlReplayer(Monitor):
    """
    
    """

    def __init__(self, prefs={}):
        Monitor.__init__(self)
        DMLogPacket.init(prefs)

    def set_input_path(self, path):
        """
        Set the replay trace path

        :param path: the replay file path
        :type path: string
        """
        self._input_path = path
        self._input_file = open(path, "rb")

    def run(self):
        """
        Start monitoring the mobile network. This is usually the entrance of monitoring and analysis.
        """
        try:
            while True:
                s = self._input_file.read(64)
                if not s:   # EOF encountered
                    break
                dm_collector_c.feed_binary(s)
                decoded = dm_collector_c.receive_log_packet()
                if decoded:
                    try:
                        packet = DMLogPacket(decoded)
                        d = packet.decode()
                        # print d["type_id"], d["timestamp"]
                        # xml = packet.decode_xml()
                        # print xml
                        # print ""
                        # Send event to analyzers
                        event = Event(  timeit.default_timer(),
                                        d["type_id"],
                                        packet)
                        self.send(event)
                    except FormatError, e:
                        # skip this packet
                        print "FormatError: ", e
            
            self._input_file.close()

        except Exception, e:
            sys.exit(e)
