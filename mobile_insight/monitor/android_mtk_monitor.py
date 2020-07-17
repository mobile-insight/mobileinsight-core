# Filename: android_mtk_monitor.py
"""
An MediaTek-version monitor for Android platform.

Author: Zhehan Li, Yuanjie Li
"""

__all__ = ["AndroidMtkMonitor"]

import os
import re
import subprocess
import sys
import timeit
import datetime

from .monitor import Monitor, Event
from .dm_collector import dm_collector_c, DMLogPacket, FormatError
from . import mtk_log_parser

is_android = False
try:
    from jnius import autoclass  # For Android
    try:
        service_context = autoclass('org.kivy.android.PythonService').mService
        if not service_context:
            service_context = cast("android.app.Activity", autoclass(
                "org.kivy.android.PythonActivity").mActivity)
    except Exception as e:
        service_context = cast("android.app.Activity", autoclass(
            "org.kivy.android.PythonActivity").mActivity)

    is_android = True
except Exception as e:
    # not used, but bugs may exist on laptop
    is_android = False

ANDROID_SHELL = "/system/bin/sh"
msg_type = ["UMTS_NAS_OTA_Packet", "LTE_NAS_ESM_OTA_Incoming_Packet", "WCDMA_RRC_OTA_Packet", "LTE_RRC_OTA_Packet"]
msg_short = ["N3", "N4", "R3", "R4"]
msg_enabled = [0 for x in range(0,len(msg_type))]

type_num = len(msg_type)
dest_file = "catcher_filter_1_lwg_n.bin"

def get_cache_dir():
    if is_android:
        return str(service_context.getCacheDir().getAbsolutePath())
    else:
        return ""

def get_files_dir():
    if is_android:
        return str(service_context.getFilesDir().getAbsolutePath() + '/app')
    else:
        return ""


class AndroidMtkMonitor(Monitor):

    """
    An MediaTek-version monitor for Android devics. Require root access to run.

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
        self._input_dir = "/sdcard/mtklog"
        self._type_names = []
        self._filename_sort_key = None

        self._read_latency = []

        libs_path = os.path.join(get_files_dir(), "data")
        ws_dissector_path = os.path.join(libs_path, "android_pie_ws_dissector")
        self.libs_path = libs_path
        self.ws_dissector_path = ws_dissector_path
        self._executable_path = os.path.join(self.libs_path, "diag_revealer_mtk")
        self._log_path = os.path.join(self._input_dir, "mdlog1")
        self._filter_src_path = os.path.join(self.libs_path, "MTK-filter")
        self._filter_dest_path = os.path.join(self._input_dir, "mdlog1_config")

        prefs = {
            "ws_dissect_executable_path": os.path.join(
                libs_path,
                "android_pie_ws_dissector"),
            "libwireshark_path": libs_path}
        DMLogPacket.init(prefs) 
        # res = mtk_log_parser.ws_dissector_proc_start(prefs.get("ws_dissect_executable_path", None), prefs.get("libwireshark_path", None)) 
        # self.log_debug('qianru init: ' + str(res))
        # WSDissector.init_proc(prefs.get("ws_dissect_executable_path", None),
        #                       prefs.get("libwireshark_path", None))


    def available_log_types(self):
        """
        Return available log types

        :returns: a list of supported message types
        """
        return self.__class__.SUPPORTED_TYPES  # ??? Current RRC only.

    def set_log_directory(self, directory):
        """
        Set the directory that will store MTK log files.

        :param directory: the path of dir
        :type directory: string
        """
        # /sdcard/mtklog/mdlog1/MDLog1_YYYY_MMDD_HHMMSS/MDLog1_YYYY_MMDD_HHMMSS.muxraw.tmp
        # -->  /sdcard/mtklog/mdlog1/MDLog1_YYYY_MMDD_HHMMSS/MDLog1_YYYY_MMDD_HHMMSS.muxraw
        #FIXME: inputdir called by upperlayer
        self._input_dir = directory  # ???
        self._input_dir = "/sdcard/mtklog"

    def set_log_cut_size(self, siz):
        """
        emdlogger cutting size: 200MB by default
        200MB / 1h 15min
        """
        if siz > 1.0:
            self._log_cut_size = siz

    def set_filename_sort_key(key):
        """
        Set the key used to sort MTK log file names.

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
        for tn in type_name:
            if tn in msg_type:
                msg_enabled[msg_type.index(tn)] = 1
                self.log_info("Enable collection: " + tn)
            # else: # delelte by qianru
            #     self.log_warning("Unsupported message by MediaTek: "+str(tn))
        # # dm_collector_c.set_filtered(self._type_names)  # ???

    def set_filter(self):
        """
        Copy the chosen filter to its path
        """
        src_file = ""
        for i in range(type_num):
            if msg_enabled[i] == 1:
                src_file += msg_short[i]
        if src_file == "":
            src_file = "default"
        src_file += ".bin"
        cmd = "cp " + os.path.join(self._filter_src_path, src_file) + " " + os.path.join(self._filter_dest_path, dest_file)
        self._run_shell_cmd(cmd)
        mtk_log_parser.setfilter(msg_type, msg_enabled)

    def enable_log_all(self):
        """
        Enable all supported logs
        """
        cls = self.__class__
        self.enable_log(cls.SUPPORTED_TYPES)
        msg_enabled = [1 for x in range(0,len(msg_type))]

    def save_log_as(self, path):
        """
        Save the log as a mi2log file (for offline analysis)

        :param path: the file name to be saved
        :type path: string
        """
        dm_collector_c.set_filtered_export(path, self._type_names)  # ???

    def _run_shell_cmd(self, cmd, wait=False):
        if isinstance(cmd, str):
            cmd = cmd.encode()
        p = subprocess.Popen(
            "su",
            executable=ANDROID_SHELL,
            shell=True,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE)
        res, err = p.communicate(cmd + b'\n')
        if wait:
            p.wait()
            # return p.stdout
            return res
        else:
            # return None
            return res

    def _start_collection(self):
        cmd = "%s %s " % (self._executable_path, "-start")
        self._run_shell_cmd(cmd)

    def _stop_collection(self):
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
        # mtk_log_parser.ws_dissector_proc_start(self.ws_dissector_path, self.libs_path)
        BLOCK_SIZE = 128
        f.seek(cur_pos, 0)
        while True:
            t1 = timeit.default_timer()
            # s = f.read(BLOCK_SIZE)
            s = f.read()
            if not s:   # EOF encountered
                break
            t2 = timeit.default_timer()
            self._read_latency.append((float(t2 - t1), len(s)))

            # dm_collector_c.feed_binary(s)
            # decoded = dm_collector_c.receive_log_packet(self._skip_decoding,
            #                                             True,   # include_timestamp
            #                                             )
            ######################################
            # self.log_debug('before feed_binary: '+"\\x".join("{:02x}".format(ord(c)) for c in s))
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
                        packet = DMLogPacket([
                            ("log_msg_len", len(msg), ""),
                            ('type_id', typeid, ''),
                            ('timestamp', datetime.datetime.now(), ''),
                            ("Msg", msgstr, "msg")]) 
                            # ("Msg", msgstr, "raw_msg/" + rawid)]) #TODO: optimize parsing speed
                        event = Event(  timeit.default_timer(),
                                        typeid,
                                        packet)
                        self.send(event)
                        del event, packet, decoded
                    ##############################################

                except FormatError as e:
                    self.log_error("FormatError: %s" % str(e))   # skip this packet
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
                # self.log_info("lizhehan: cur_pos: "+cur_pos/1000+" file size: "+siz/1000+"ratio: "+cur_pos/siz)
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

        self.broadcast_info('STARTED',{})

        if not self._input_dir:
            # FIXME: test if self._input_dir exists in SDcard
            raise RuntimeError(
                "Log directory not set. Please call set_log_directory() first.")
        self.set_filter()
        cmd = "rm -r " + os.path.join(self._log_path, "*")
        self._run_shell_cmd(cmd)
        old_files = set()

        try:
            self._stop_collection()
            self._start_collection()
            # self.log_info("lizhehan: Start the collection")
            monitoring_files = []
            while True:
                current_files = set(self._get_filenames(self._log_path))
                new_files = list(current_files - old_files)
                old_files = current_files

                if new_files:   # new log files detected
                    self.log_warning("New log files " + str(new_files))
                    if self._filename_sort_key is None:
                        k = AndroidMtkMonitor._default_filename_sort_key
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
            self._stop_collection()
