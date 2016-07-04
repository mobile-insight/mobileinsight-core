#!/usr/bin/python
# Filename: android_dev_diag_monitor.py
"""
android_dev_diag_monitor.py

Author: Jiayao Li, Yuanjie Li
"""

__all__ = ["AndroidDevDiagMonitor"]


import binascii
import errno
import os
import stat
import re
import subprocess
import threading
import struct
import stat
import sys
import timeit
import time


from monitor import Monitor, Event
from dm_collector import dm_collector_c, DMLogPacket, FormatError

is_android=False
try:
    from jnius import autoclass #For Android
    try:
        service_context = autoclass('org.renpy.android.PythonService').mService
    except Exception, e:
    	service_context = cast("android.app.Activity",
                            autoclass("org.renpy.android.PythonActivity").mActivity)

    is_android=True
except Exception, e:
    #not used, but bugs may exist on laptop
    is_android=False

ANDROID_SHELL = "/system/bin/sh"



def get_cache_dir():
    if is_android:
        return str(service_context.getCacheDir().getAbsolutePath())
    else:
        return ""


class ChronicleProcessor(object):
    TYPE_LOG = 1
    TYPE_START_LOG_FILE = 2
    TYPE_END_LOG_FILE = 3
    READ_TYPE = 0
    READ_MSG_LEN = 1
    READ_TS = 2
    READ_PAYLOAD = 3
    READ_FILENAME = 4

    def __init__(self):
        self._init_state()

    def _init_state(self):
        cls = self.__class__
        self.state = cls.READ_TYPE
        self.msg_type = None
        self.bytes = ["", "", "", "", ""]
        self.to_read = [2, 2, 8, None, None] # short, short, double, varstring, varstring

    def process(self, b):
        cls = self.__class__
        ret_msg_type = self.msg_type
        ret_ts = None
        ret_payload = None
        ret_filename = None

        while len(b) > 0:
            if len(b) <= self.to_read[self.state]:
                self.bytes[self.state] += b
                self.to_read[self.state] -= len(b)
                b = ""
            else:
                idx = self.to_read[self.state]
                self.bytes[self.state] += b[0:idx]
                self.to_read[self.state] = 0
                b = b[idx:]
            # Process input data
            if self.state == cls.READ_TYPE:
                if self.to_read[self.state] == 0:   # current field is complete
                    self.msg_type = struct.unpack("<h", self.bytes[self.state])[0]
                    ret_msg_type = self.msg_type
                    self.state = cls.READ_MSG_LEN
            elif self.state == cls.READ_MSG_LEN:
                if self.to_read[self.state] == 0:   # current field is complete
                    msg_len = struct.unpack("<h", self.bytes[self.state])[0]
                    if self.msg_type == cls.TYPE_LOG:
                        self.to_read[cls.READ_PAYLOAD] = msg_len - 8
                        self.state = cls.READ_TS
                    elif self.msg_type in {cls.TYPE_START_LOG_FILE, cls.TYPE_END_LOG_FILE}:
                        self.to_read[cls.READ_FILENAME] = msg_len
                        self.state = cls.READ_FILENAME
                    else:
                        raise RuntimeError("Unknown msg type %s" % str(self.msg_type))
            elif self.state == cls.READ_TS:
                if self.to_read[self.state] == 0:   # current field is complete
                    ret_ts = struct.unpack("<d", self.bytes[self.state])[0]
                    self.state = cls.READ_PAYLOAD
            elif self.state == cls.READ_PAYLOAD:
                if len(self.bytes[self.state]) > 0: # don't need to wait for complete field
                    ret_payload = self.bytes[self.state]
                    self.bytes[self.state] = ""
                if self.to_read[self.state] == 0:   # current field is complete
                    self._init_state()
                    break
            else:   # READ_FILENAME
                if self.to_read[self.state] == 0:   # current field is complete
                    ret_filename = self.bytes[self.state]
                    self._init_state()
                    break
        remain = b
        return ret_msg_type, ret_ts, ret_payload, ret_filename, remain


class AndroidDevDiagMonitor(Monitor):
    """
    An Device Diag monitor for Android devices. Require root access to run.
    """

    #: a list containing the currently supported message types.
    SUPPORTED_TYPES = set(dm_collector_c.log_packet_types)

    # DIAG_CFG_DIR = "/sdcard/diag_logs"
    DIAG_CFG_DIR = get_cache_dir()
    # TMP_FIFO_FILE = "/sdcard/diag_revealer_fifo"
    TMP_FIFO_FILE = os.path.join(get_cache_dir(), "diag_revealer_fifo") 
    # Yuanjie: the smaller the lower latency, but maybe higher CPU
    # DO NOT CHANGE IT! This value has been optimized
    BLOCK_SIZE = 8

    def __init__(self):
        """
        Configure this class with user preferences.
        This method should be called before any actual decoding.

        :param prefs: configurations for message decoder. Empty by default.
        :type prefs: dictionary
        """
        Monitor.__init__(self)
        self._executable_path = "/system/bin/diag_revealer"
        self._fifo_path = self.TMP_FIFO_FILE
        self._input_dir = None
        self._log_cut_size = 0.5 # change size to 1.0 M
        # self._skip_decoding = False
        self._type_names = []
        self._last_diag_revealer_ts = None
        prefs={"ws_dissect_executable_path": "/system/bin/android_pie_ws_dissector",
            "libwireshark_path": "/system/lib"}
        DMLogPacket.init(prefs)     # Initialize Wireshark dissector

    def available_log_types(self):
        """
        Return available log types

        :returns: a list of supported message types
        """
        return self.__class__.SUPPORTED_TYPES

    def _run_shell_cmd(self, cmd, wait = False):
        p = subprocess.Popen("su", executable=ANDROID_SHELL, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        res,err = p.communicate(cmd+'\n')
        # p.stdin.write(cmd+'\n')
        if wait:
            p.wait()
            # return p.returncode
            return res
        else:
            # return None
            return res

    def set_log_directory(self, directory):
        """
        Set the directory that will store output log files.

        :param directory: the path of dir
        :type directory: string
        """
        self._input_dir = directory

    def set_log_cut_size(self, siz):
        if siz > 0.0:
            self._log_cut_size = siz

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
                self.log_info("WARNING: Unsupported log message type: %s" % n) 
            elif n not in self._type_names:
                self._type_names.append(n)
                self.log_info("Enable collection: "+n)
        dm_collector_c.set_filtered(self._type_names)

    def enable_log_all(self):
        """
        Enable all supported logs
        """
        cls = self.__class__
        self.enable_log(cls.SUPPORTED_TYPES)

    def save_log_as(self,path):
        """
        Save the log as a mi2log file (for offline analysis)

        :param path: the file name to be saved
        :type path: string
        :param log_types: a filter of message types to be saved
        :type log_types: list of string
        """
        dm_collector_c.set_filtered_export(path,self._type_names)

    def set_block_size(self, n):
        self.BLOCK_SIZE = n

    def _mkfifo(self, fifo_path):
        try:
            os.mknod(fifo_path, 0666 | stat.S_IFIFO)
        except OSError as err:
            if err.errno == errno.EEXIST:   # if already exists, skip this step
                pass
                # print "Fifo file already exists, skipping..."
            elif err.errno == errno.EPERM:  # not permitted, try shell command
                # print "Not permitted to create fifo file, try to switch to root..."
                retcode = self._run_shell_cmd("mknod %s p" % fifo_path, wait=True)
                if retcode != 0:
                    raise RuntimeError("mknod returns %s" % str(retcode))
            else:
                raise err


    def get_last_diag_revealer_ts(self):
        """
        Return the timestamp when the lastest msg is sent by diag_revealer
        """
        return self._last_diag_revealer_ts


    def _start_diag_revealer(self):
        """
        Initialize diag_revealer with correct parameters
        """
        # TODO(likayo): need to protect aganist user input
        cmd = "%s %s %s" % (self._executable_path, os.path.join(self.DIAG_CFG_DIR, "Diag.cfg"), self._fifo_path)
        if not os.path.exists(self._input_dir):
            cmd += " %s %.6f" % (self._input_dir, self._log_cut_size)
            self._run_shell_cmd("mkdir \"%s\"" % self._input_dir)
            self._run_shell_cmd("chmod -R 777 \"%s\"" % self._input_dir, wait=True)
            # os.mkdir(self._input_dir)
            # os.chmod(self._input_dir,777)
        proc = subprocess.Popen("su", executable=ANDROID_SHELL, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        proc.stdin.write(cmd+'\n')

    def _protect_diag_revealer(self):
        """
        A daemon to monitor the liveness of diag_revealer, 
        and restart if diag_revealer crashes
        """
        cmd = "ps | grep diag_revealer\n"
        while True:
            time.sleep(5)
            res = self._run_shell_cmd(cmd)

            # proc = subprocess.Popen(cmd, executable=ANDROID_SHELL, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
            # if not proc.stdout.read():
            if not res:
                # diag_revealer is not alive
                self.log_warning("diag_revealer is terminated. Restart diag_revealer ...")
                self._start_diag_revealer()

    def _stop_collection(self):
        ANDROID_SHELL = "/system/bin/sh"
        self.collecting = False

        # Find diag_mdlog process
        diag_procs = []
        pids = [pid for pid in os.listdir("/proc") if pid.isdigit()]
        for pid in pids:
            try:
                cmdline = open(os.path.join("/proc", pid, "cmdline"), "rb").read()
                if cmdline.startswith("/system/bin/diag_revealer"):
                    diag_procs.append(int(pid))
            except IOError:     # proc has been terminated
                continue

        if len(diag_procs) > 0:
            cmd2 = "kill " + " ".join([str(pid) for pid in diag_procs])
            self._run_shell_cmd(cmd2)



    def run(self):
        """
        Start monitoring the mobile network. This is usually the entrance of monitoring and analysis.

        This function does NOT return or raise any exception.
        """

        #Stop running loggers
        # self._stop_collection()

        generate_diag_cfg = True
        if not self._type_names:
            raise RuntimeError("Log type not specified. Please specify the log types with enable_log().")
            # if os.path.exists(os.path.join(self.DIAG_CFG_DIR, "Diag.cfg")):
            #     generate_diag_cfg = False
            #     # print "AndroidDevDiagMonitor: existing Diag.cfg file will be used."
            # else:
            #     raise RuntimeError("Log type not specified. Please call enable_log() first.")

        try:
            if generate_diag_cfg:
                if not os.path.exists(self.DIAG_CFG_DIR):
                    os.makedirs(self.DIAG_CFG_DIR)

                # fd = open(os.path.join(self.DIAG_CFG_DIR, "Diag.cfg"), "w+b")
                # Overwrite Diag.cfg, not append it
                fd = open(os.path.join(self.DIAG_CFG_DIR, "Diag.cfg"), "wb")
                dm_collector_c.generate_diag_cfg(fd, self._type_names)
                fd.close()

            self._mkfifo(self._fifo_path)

            # Launch diag_revealer, and protection daemon
            self._start_diag_revealer()
            self.diag_revealer_daemon = threading.Thread(target=self._protect_diag_revealer)
            self.diag_revealer_daemon.start()

            # fifo = os.open(self._fifo_path, os.O_RDONLY | os.O_NONBLOCK)
            fifo = os.open(self._fifo_path, os.O_RDONLY)    #Blocking mode: save CPU

            # Read log packets from diag_revealer
            chproc = ChronicleProcessor()
            while True:
                try:
                    # self.log_info("Before os.read(fifo, self.BLOCK_SIZE)")
                    s = os.read(fifo, self.BLOCK_SIZE)
                    # self.log_info("After os.read(fifo, self.BLOCK_SIZE)")
                except OSError as err:
                    if err.errno == errno.EAGAIN or err.errno == errno.EWOULDBLOCK:
                        # self.log_info("err.errno="+str(err.errno))
                        s = None
                    else:
                        raise err # something else has happened -- better reraise

                while s:   # preprocess metadata
                    # self.log_info("Before chproc.process(s)")
                    ret_msg_type, ret_ts, ret_payload, ret_filename, remain = chproc.process(s)
                    # self.log_info("After chproc.process(s)")
                    if ret_msg_type == ChronicleProcessor.TYPE_LOG:
                        if ret_ts:
                            self._last_diag_revealer_ts = ret_ts
                        if ret_payload:
                            dm_collector_c.feed_binary(ret_payload)
                    elif ret_msg_type == ChronicleProcessor.TYPE_START_LOG_FILE:
                        if ret_filename:
                            pass
                            # print "Start of %s" % ret_filename
                    elif ret_msg_type == ChronicleProcessor.TYPE_END_LOG_FILE:
                        if ret_filename:
                            # res_dict = {'filename':ret_filename}
                            # msg = ("new_diag_log",res_dict,"dict")
                            msg = ('filename', ret_filename, "")
                            # print "End of %s" % ret_filename
                            event = Event(  timeit.default_timer(),
                                            "new_diag_log",
                                            DMLogPacket([msg]))
                                            # ret_filename)
                            self.send(event)
                    elif ret_msg_type is not None:
                        raise RuntimeError("Unknown ret msg type: %s" % str(ret_msg_type))
                    s = remain

                result = dm_collector_c.receive_log_packet(self._skip_decoding,
                                                            True,   # include_timestamp
                                                            )
                if result:     # result = (decoded, posix_timestamp)
                    try:
                        packet = DMLogPacket(result[0])
                        d = packet.decode()
                        # print d["type_id"], d["timestamp"], result[1]
                        # xml = packet.decode_xml()
                        # print xml
                        # print ""
                        # Send event to analyzers
                        event = Event(  result[1],
                                        d["type_id"],
                                        packet)
                        self.send(event)
                    except FormatError, e:
                        # skip this packet
                        print "FormatError: ", e


        except (KeyboardInterrupt, RuntimeError), e:
            os.close(fifo)
            # proc.terminate()
            self._stop_collection()
            packet = DMLogPacket([])
            event = Event(  timeit.default_timer(),
                            "sys_shutdown",
                            packet)
            self.send(event)
            import traceback
            sys.exit(str(traceback.format_exc()))
            # sys.exit(e)
        except Exception, e:
            os.close(fifo)
            # proc.terminate()
            self._stop_collection()
            packet = DMLogPacket([])
            event = Event(  timeit.default_timer(),
                            "sys_shutdown",
                            packet)
            self.send(event)
            import traceback
            sys.exit(str(traceback.format_exc()))
            # sys.exit(e)
