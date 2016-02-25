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
    A log replayer for offline analysis.
    """

    SUPPORTED_TYPES = set(dm_collector_c.log_packet_types)

    def __init__(self, prefs={}):
        Monitor.__init__(self)
        DMLogPacket.init(prefs)

        self._type_names=[]

    def enable_log(self, type_name):
        """
        Enable the messages to be monitored. Refer to cls.SUPPORTED_TYPES for supported types.

        If this method is never called, the config file existing on the SD card will be used.
        
        :param type_name: the message type(s) to be monitored
        :type type_name: string or list

        :except ValueError: unsupported message type encountered
        """
        cls = self.__class__
        if isinstance(type_name, str):
            type_name = [type_name]
        for n in type_name:
            if n not in cls.SUPPORTED_TYPES:
                raise ValueError("Unsupported log message type: %s" % n)
            elif n not in self._type_names:
                self._type_names.append(n)

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

        # fd = open('./Diag.cfg','wb')
        # dm_collector_c.generate_diag_cfg(fd, self._type_names)
        # fd.close()

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

                        if d["type_id"] in self._type_names:
                            event = Event(  timeit.default_timer(),
                                            d["type_id"],
                                            packet)
                            self.send(event)
                    except FormatError, e:
                        # skip this packet
                        print "FormatError: ", e
            
            self._input_file.close()

        except Exception, e:
            import traceback
            sys.exit(str(traceback.format_exc()))
            # sys.exit(e)
