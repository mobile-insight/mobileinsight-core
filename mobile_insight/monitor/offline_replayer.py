#!/usr/bin/python
# Filename: offline_replayer.py
"""
An offline log replayer

Author: Jiayao Li,
        Yuanjie Li
"""

__all__ = ["OfflineReplayer"]

import sys
import os
import timeit

from monitor import Monitor, Event
from dm_collector import dm_collector_c, DMLogPacket, FormatError


is_android=False
try:
    from jnius import autoclass #For Android
    is_android=True
except Exception, e:
    #not used, but bugs may exist on laptop
    is_android=False

class OfflineReplayer(Monitor):
    """
    A log replayer for offline analysis.
    """

    SUPPORTED_TYPES = set(dm_collector_c.log_packet_types)

    def __init__(self):
        Monitor.__init__(self)

        if is_android:
            # prefs={"ws_dissect_executable_path": "/system/bin/android_pie_ws_dissector",
            #        "libwireshark_path": "/system/lib"}
            libs_path = "./data"
            prefs={"ws_dissect_executable_path": os.path.join(libs_path,"android_pie_ws_dissector"),
                   "libwireshark_path": libs_path}
        else:
            prefs={}
        DMLogPacket.init(prefs)

        self._type_names=[]

    def available_log_types(self):
        """
        Return available log types

        :returns: a list of supported message types
        """
        return self.__class__.SUPPORTED_TYPES

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
                self.log_warning("Unsupported log message type: %s" % n) 
            if n not in self._type_names:
                self._type_names.append(n)
                self.log_info("Enable "+n)
        dm_collector_c.set_filtered(self._type_names)

    def enable_log_all(self):
        """
        Enable all supported logs
        """
        cls = self.__class__
        self.enable_log(cls.SUPPORTED_TYPES)

    def set_input_path(self, path):
        """
        Set the replay trace path

        :param path: the replay file path. If it is a directory, the OfflineReplayer will read all logs under this directory (logs in subdirectories are ignored)
        :type path: string
        """
        dm_collector_c.reset()
        self._input_path = path
        # self._input_file = open(path, "rb")

    def save_log_as(self,path):
        """
        Save the log as a mi2log file (for offline analysis)

        :param path: the file name to be saved
        :type path: string
        :param log_types: a filter of message types to be saved
        :type log_types: list of string
        """
        dm_collector_c.set_filtered_export(path,self._type_names)

    def run(self):
        """
        Start monitoring the mobile network. This is usually the entrance of monitoring and analysis.
        """

        # fd = open('./Diag.cfg','wb')
        # dm_collector_c.generate_diag_cfg(fd, self._type_names)
        # fd.close()

        try:

            log_list=[]
            if os.path.isfile(self._input_path):
                log_list = [self._input_path]
            elif os.path.isdir(self._input_path):
                for file in os.listdir(self._input_path):
                    if file.endswith(".mi2log") or file.endswith(".qmdl"):
                        # log_list.append(self._input_path+"/"+file)
                        log_list.append(os.path.join(self._input_path,file))
            else:
                return

            log_list.sort() #Hidden assumption: logs follow the diag_log_TIMSTAMP_XXX format


            for file in log_list:
            	self.log_info("Loading "+file)
                self._input_file = open(file, "rb")
                dm_collector_c.reset()
                while True:
                    s = self._input_file.read(64)
                    if not s:   # EOF encountered
                        break

                    dm_collector_c.feed_binary(s)
                    # decoded = dm_collector_c.receive_log_packet()
                    decoded = dm_collector_c.receive_log_packet(self._skip_decoding,
                                                                True,   # include_timestamp
                                                                )
                    if decoded:
                        try:
                            # packet = DMLogPacket(decoded)
                            packet = DMLogPacket(decoded[0])
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
