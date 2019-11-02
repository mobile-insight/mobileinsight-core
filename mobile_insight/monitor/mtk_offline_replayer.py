#!/usr/bin/python
# Filename: mtk_offline_replayer.py
"""
An offline log replayer for MediaTek

Author: Yuanjie Li,
        Qianru Li
"""

__all__ = ["MtkOfflineReplayer"]

import sys
import os
import timeit
import datetime

from .monitor import Monitor, Event
from .dm_collector import dm_collector_c, DMLogPacket, FormatError
# from monitor.dm_collector.dm_endec.ws_dissector import *
from . import mtk_log_parser


msg_type = ["UMTS_NAS_OTA_Packet", "LTE_NAS_ESM_OTA_Incoming_Packet", "WCDMA_RRC_OTA_Packet", "LTE_RRC_OTA_Packet"]
msg_enabled = [0 for x in range(0,len(msg_type))]


class MtkOfflineReplayer(Monitor):
    """
    A log replayer for offline analysis.
    """

    SUPPORTED_TYPES = set(dm_collector_c.log_packet_types)

    def __test_android(self):
        try:
            from jnius import autoclass, cast  # For Android
            # try:
            #     self.service_context = autoclass('org.kivy.android.PythonService').mService
            #     if not self.service_context:
            #         self.service_context = autoclass("org.kivy.android.PythonActivity").mActivity
            # except Exception, e:
            #     self.service_context = autoclass("org.kivy.android.PythonActivity").mActivity
            self.is_android = True
            try:
                from service import mi2app_utils
                self.service_context = autoclass(
                    'org.kivy.android.PythonService').mService
            except Exception as e:
                self.service_context = None

        except Exception as e:
            # not used, but bugs may exist on laptop
            self.is_android = False

    def __init__(self):
        Monitor.__init__(self)

        self.is_android = False
        self.service_context = None

        self.__test_android()

        # self.ws_dissector_path = "ws_dissector"
        self.ws_dissector_path = None
        self.libs_path = None

        if self.is_android:
            libs_path = self.__get_libs_path()
            ws_dissector_path = os.path.join(libs_path, "android_pie_ws_dissector")
            self.libs_path = libs_path
            self.ws_dissector_path = ws_dissector_path
            prefs = {
                "ws_dissect_executable_path": os.path.join(
                    libs_path,
                    "android_pie_ws_dissector"),
                "libwireshark_path": libs_path}
        else:
            prefs = {}

        print(prefs)

        DMLogPacket.init(prefs)

        self._type_names = []

    def __del__(self):
        if self.is_android and self.service_context:
            print("detaching...")
            from service import mi2app_utils
            mi2app_utils.detach_thread()

    # def __get_cache_dir(self):
    #     if self.is_android:
    #         return str(self.service_context.getCacheDir().getAbsolutePath())
    #     else:
    #         return ""

    def __get_libs_path(self):
        if self.is_android and self.service_context:
            return os.path.join(
                self.service_context.getFilesDir().getAbsolutePath(), "data")
        else:
            return "./data"

    def available_log_types(self):
        """
        Return available log types

        :returns: a list of supported message types
        """
        return self.__class__.SUPPORTED_TYPES  # ??? Current RRC only.

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
        for tn in type_name:
            if tn in msg_type:
                msg_enabled[msg_type.index(tn)] = 1
                self.log_info("Enable collection: " + tn)
            else:
                self.log_warning("Unsupported message by MediaTek: "+str(tn))
        # dm_collector_c.set_filtered(self._type_names)  # ???

    # def enable_log(self, type_name):
    #     """
    #     Enable the messages to be monitored. Refer to cls.SUPPORTED_TYPES for supported types.

    #     If this method is never called, the config file existing on the SD card will be used.

    #     :param type_name: the message type(s) to be monitored
    #     :type type_name: string or list

    #     :except ValueError: unsupported message type encountered
    #     """
    #     cls = self.__class__
    #     if isinstance(type_name, str):
    #         type_name = [type_name]
    #     for n in type_name:
    #         if n not in cls.SUPPORTED_TYPES:
    #             self.log_warning("Unsupported log message type: %s" % n)
    #         if n not in self._type_names:
    #             self._type_names.append(n)
    #             self.log_info("Enable " + n)
    #     dm_collector_c.set_filtered(self._type_names)

    # def enable_log_all(self):
    #     """
    #     Enable all supported logs
    #     """
    #     cls = self.__class__
    #     self.enable_log(cls.SUPPORTED_TYPES)

    def enable_log_all(self):
        """
        Enable all supported logs
        """
        cls = self.__class__
        self.enable_log(cls.SUPPORTED_TYPES)
        msg_enabled = [1 for x in range(0,len(msg_type))]

    def set_input_path(self, path):
        """
        Set the replay trace path

        :param path: the replay file path. If it is a directory, the OfflineReplayer will read all logs under this directory (logs in subdirectories are ignored)
        :type path: string
        """
        dm_collector_c.reset()
        self._input_path = path
        # self._input_file = open(path, "rb")

    # def save_log_as(self, path):
    #     """
    #     Save the log as a mi2log file (for offline analysis)

    #     :param path: the file name to be saved
    #     :type path: string
    #     :param log_types: a filter of message types to be saved
    #     :type log_types: list of string
    #     """
    #     dm_collector_c.set_filtered_export(path, self._type_names)

    def save_log_as(self, path):
        """
        Save the log as a mi2log file (for offline analysis)

        :param path: the file name to be saved
        :type path: string
        """
        dm_collector_c.set_filtered_export(path, self._type_names)  # ???

    def run(self):
        """
        Start monitoring the mobile network. This is usually the entrance of monitoring and analysis.
        """

        # fd = open('./Diag.cfg','wb')
        # dm_collector_c.generate_diag_cfg(fd, self._type_names)
        # fd.close()

        self.log_info("Running Offline replayer")

        try:

            self.broadcast_info('STARTED',{})

            log_list = []
            if os.path.isfile(self._input_path):
                log_list = [self._input_path]
            elif os.path.isdir(self._input_path):
                for file in os.listdir(self._input_path):
                    if file.endswith(".muxraw"):
                        log_list.append(os.path.join(self._input_path, file))
            else:
                self.log_debug("No files???")
                return

            log_list.sort()  # Hidden assumption: logs follow the diag_log_TIMSTAMP_XXX format

            # mtk_log_parser.ws_dissector_proc_start(self.ws_dissector_path, self.libs_path)

            for file in log_list:
                self.log_info("Loading " + file)
                self._input_file = open(file, "rb")
                dm_collector_c.reset()
                while True:
                    s = self._input_file.read() 
                    if not s:
                        break
                    decoded = mtk_log_parser.feed_binary(s)  # self for debug
            # decoded = mtk_log_parser.receive_log_packet(self._skip_decoding,
            #                                             True   # include_timestamp
            #                                             )
            ######################################

                    if decoded != []:
                        try:
                    # # packet = DMLogPacket(decoded)
                    # packet = DMLogPacket(decoded[0])
                    # d = packet.decode()
                    # # print d["type_id"], d["timestamp"]
                    # # xml = packet.decode_xml()
                    # # print xml
                    # # print ""
                    # # Send event to analyzers
                    # event = Event(  timeit.default_timer(),
                    #                 d["type_id"],
                    #                 packet)
                    # self.send(event)

                    ##############################################
                            for msg in decoded:
                                typeid, rawid, msgstr = mtk_log_parser.decode(self, msg) #self for debug

                                if typeid == "":
                                    continue
                                packet = DMLogPacket([("log_msg_len", len(msg), ""),('type_id', typeid, ''),('timestamp', datetime.datetime.now(), ''),("Msg", msgstr, "msg")]) # ("Msg", msgstr, "raw_msg/" + rawid)])
                                # print "DMLogPacket decoded[0]:",str([typeid])
                                event = Event(  timeit.default_timer(), typeid, packet)
                                self.send(event)
                    ##############################################

                        except FormatError as e:
                            print(("FormatError: ", e))  # skip this packet
                self._input_file.close()


        except Exception as e:
            import traceback
            print((str(traceback.format_exc())))
