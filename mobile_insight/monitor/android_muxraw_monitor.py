# Filename: android_muxraw_monitor.py
"""
An MUXRAW monitor for Android platform.

Author: Jiayao Li, Yuanjie Li, Zhehan Li
"""

__all__ = ["AndroidMuxrawMonitor"]

import os
import re
import subprocess
import sys
import timeit

from monitor import Monitor, Event
from dm_collector import dm_collector_c, DMLogPacket, FormatError
import muxraw_parser

is_android = False
try:
    from jnius import autoclass  # For Android
    try:
        service_context = autoclass('org.renpy.android.PythonService').mService
        if not service_context:
            service_context = cast("android.app.Activity", autoclass(
                "org.renpy.android.PythonActivity").mActivity)
    except Exception as e:
        service_context = cast("android.app.Activity", autoclass(
            "org.renpy.android.PythonActivity").mActivity)

    is_android = True
except Exception as e:
    # not used, but bugs may exist on laptop
    is_android = False

ANDROID_SHELL = "/system/bin/sh"


def get_cache_dir():
    if is_android:
        return str(service_context.getCacheDir().getAbsolutePath())
    else:
        return ""


def get_files_dir():
    if is_android:
        return str(service_context.getFilesDir().getAbsolutePath())
    else:
        return ""


class AndroidMuxrawMonitor(Monitor):

    """
    An MUXRAW monitor for Android devics. Require root access to run.

    This class runs the in-phone logging program [to enable emdlogger],
    and collect traces from emdlogger output.

    Whenever a new trace appears, it generates a "new_diag_log" event, carrying
    the path of the trace in the payload. Then it sends every log message
    in the trace as an event.
    """

    # SUPPORTED_TYPES: a list containing the currently supported message types.
    SUPPORTED_TYPES = set(dm_collector_c.log_packet_types)

    # DIAG_CFG_DIR = "/sdcard/diag_logs"
    DIAG_CFG_DIR = get_cache_dir()

    def __init__(self, prefs={}):

        Monitor.__init__(self)

        self._input_dir = os.path.join(
            get_cache_dir(), "mi2log")  # ??? getCacheDir()
        self._input_dir = "/sdcard/mtklog/mdlog1"
        self._type_names = []
        self._filename_sort_key = None

        self._read_latency = []

        libs_path = os.path.join(get_files_dir(), "data")
        ws_dissector_path = os.path.join(libs_path, "android_pie_ws_dissector")
        self.libs_path = libs_path
        self.ws_dissector_path = ws_dissector_path
        self._executable_path = os.path.join(libs_path, "diag_revealer_mtk")

        prefs = {
            "ws_dissect_executable_path": os.path.join(
                libs_path,
                "android_pie_ws_dissector"),
            "libwireshark_path": libs_path}
        DMLogPacket.init(prefs)  # ???

    def available_log_types(self):
        """
        Return available log types

        :returns: a list of supported message types
        """
        return self.__class__.SUPPORTED_TYPES  # ??? Current RRC only.

    def set_log_directory(self, directory):
        """
        Set the directory that will store Muxraw files.

        :param directory: the path of dir
        :type directory: string
        """
        # /sdcard/mtklog/mdlog1/MDLog1_YYYY_MMDD_HHMMSS/MDLog1_YYYY_MMDD_HHMMSS.muxraw.tmp
        # -->  /sdcard/mtklog/mdlog1/MDLog1_YYYY_MMDD_HHMMSS/MDLog1_YYYY_MMDD_HHMMSS.muxraw
        self._input_dir = directory  # ???
        self._input_dir = "/sdcard/mtklog/mdlog1"

    def set_log_cut_size(self, siz):
        """
        emdlogger cutting size: 200MB by default
        200MB / 1h 15min
        """
        if siz > 1.0:
            self._log_cut_size = siz

    def set_filename_sort_key(key):
        """
        Set the key used to sort muxraw file names.

        If key is None, a default one will be used

        :param directory: a function that takes file name as input and return a string.
        :type key: callable
        """
        self._filename_sort_key = key

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
            elif n not in self._type_names:
                self._type_names.append(n)
                self.log_info("Enable collection: " + n)
        dm_collector_c.set_filtered(self._type_names)  # ???

    def enable_log_all(self):
        """
        Enable all supported logs
        """
        cls = self.__class__
        self.enable_log(cls.SUPPORTED_TYPES)

    def save_log_as(self, path):
        """
        Save the log as a mi2log file (for offline analysis)

        :param path: the file name to be saved
        :type path: string
        """
        dm_collector_c.set_filtered_export(path, self._type_names)  # ???

    def _run_shell_cmd(self, cmd, wait=False):
        p = subprocess.Popen(
            "su",
            executable=ANDROID_SHELL,
            shell=True,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE)
        res, err = p.communicate(cmd + '\n')
        if wait:
            p.wait()
            # return p.stdout
            return res
        else:
            # return None
            return res

    def _start_collection(self, log_directory):
        cmd = "%s %s " % (self._executable_path, "-start")
        self._run_shell_cmd(cmd)

    def _stop_collection(self, wait=False):
        cmd = "%s %s " % (self._executable_path, "-stop")
        self._run_shell_cmd(cmd)

    @staticmethod
    def _default_filename_sort_key(path):
        """
        When multiple new files occur at the same time, use this key function to order them.

        :param path: the absolute path of a file.
        :type path: str
        """
        # TODO: use timestamp to order
        s = os.path.basename(path)
        return s

    def _get_filenames(self, log_directory):
        """
        Return a set of absolute pathes of files under log_directory.

        :param log_directory: the directory where files are located in.
        :type log_directory: str
        """
        res = set()
        for root, dirs, files in os.walk(log_directory):
            for f in files:
                if f.endswith('.muxraw.tmp'):
                    res.add(os.path.join(root, f))
        return res

    def get_avg_read_latency(self, last=100):
        sum_latency = 0.0
        n = 0
        i = len(self._read_latency) - 1
        while i >= 0 and n < last:
            if self._read_latency[i][1] <= last - n:
                sum_latency += self._read_latency[i][0]
                n += self._read_latency[i][1]
            else:
                sum_latency += self._read_latency[i][0] * \
                    (float(last - n) / self._read_latency[i][1])
                n = last
            i -= 1
        return (sum_latency / n) if n > 0 else 0.0

    def _read_muxraw(self, f, cur_pos):
        """
        Return final position.
        """
        BLOCK_SIZE = 128
        f.seek(cur_pos, 0)
        while True:
            t1 = timeit.default_timer()
            s = f.read(BLOCK_SIZE)
            if not s:   # EOF encountered
                break
            t2 = timeit.default_timer()
            self._read_latency.append((float(t2 - t1), len(s)))

            # dm_collector_c.feed_binary(s)
            # decoded = dm_collector_c.receive_log_packet(self._skip_decoding,
            #                                             True,   # include_timestamp
            #                                             )
            ######################################
            decoded = muxraw_parser.feed_binary(s)  # self for debug
            # decoded = muxraw_parser.receive_log_packet(self._skip_decoding,
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

                        typeid, msgstr = muxraw_parser.decode(self, msg) #self for debug
                        if typeid == "":
                            continue
                        # self.log_info("lizhehan: Receive Message: ")
                        # self.log_info("lizhehan: typeid: " + typeid)

                        packet = DMLogPacket([("MediaTek", msgstr, "msg")])
                        event = Event(  timeit.default_timer(),
                                        typeid,
                                        packet)
                        self.send(event)
                    ##############################################

                except FormatError as e:
                    print "FormatError: ", e  # skip this packet
        return f.tell()

    def _parse_muxraws(self, monitoring_files):
        i = 0
        while i < len(monitoring_files):
            filename, cur_pos = monitoring_files[i]
            siz = os.path.getsize(filename)
            if cur_pos == 0:    # new file
                self._run_shell_cmd(
                    "chmod -R 644 \"%s\"" %
                    filename, wait=True)
            if cur_pos < siz:   # has new content to read
                f = open(filename, "rb")
                cur_pos = self._read_muxraw(f, cur_pos)
                f.close()
                monitoring_files[i][1] = cur_pos
            if i != len(monitoring_files) - \
                    1:  # new file appears so this file obsoletes
                event = Event(timeit.default_timer(), "new_diag_log", filename)
                self.send(event)
                monitoring_files.pop(i)
                i -= 1
            i += 1
        return

    def run(self):
        """
        Start monitoring cellular network from Android device.
        This is the entrance of monitoring and analysis.
        :except RuntimeError: set_log_directory() or enable_log() not called before.
        """

        if not self._input_dir:
            # FIXME: test if self._input_dir exists in SDcard
            raise RuntimeError(
                "Log directory not set. Please call set_log_directory() first.")
        old_files = set(self._get_filenames(self._input_dir))

        try:
            self._stop_collection(self._input_dir)
            self._start_collection(self._input_dir)
            monitoring_files = []
            while True:
                current_files = set(self._get_filenames(self._input_dir))
                new_files = list(current_files - old_files)
                old_files = current_files

                if new_files:   # new muxraw files detected
                    self.log_warning("New muxraw files " + str(new_files))
                    if self._filename_sort_key is None:
                        k = AndroidMuxrawMonitor._default_filename_sort_key
                    else:
                        k = self._filename_sort_key
                    new_files.sort(key=k)

                    for filename in new_files:
                        monitoring_files.append([filename, 0])
                        #                        [filename, cur_pos]

                self._parse_muxraws(monitoring_files)

        except Exception as e:
            import traceback
            sys.exit(str(traceback.format_exc()))
        finally:
            self._parse_muxraws(monitoring_files)
            # decoded = muxraw_parser.last_seek()
            # if decoded != []:
            #     try:
            #         for msg in decoded:
            #             typeid, xml = muxraw_parser.decode(self, msg) #self for debug
            #             event = Event(  timeit.default_timer(),
            #                             typeid,
            #                             xml)
            #             self.send(event)
            #     except FormatError, e:
            #         print "FormatError: ", e
            self._stop_collection(wait=True)
