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
import re
import subprocess
import struct
import stat
import sys
import timeit

from monitor import Monitor, Event
from dm_collector import dm_collector_c, DMLogPacket, FormatError

ANDROID_SHELL = "/system/bin/sh"


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
    An QMDL monitor for Android devics. Require root access to run.
    """

    #: a list containing the currently supported message types.
    SUPPORTED_TYPES = set(dm_collector_c.log_packet_types)

    DIAG_CFG_DIR = "/sdcard/diag_logs"
    TMP_FIFO_FILE = "/sdcard/diag_revealer_fifo"
    BLOCK_SIZE = 128

    def __init__(self, prefs={}):
        """
        Configure this class with user preferences.
        This method should be called before any actual decoding.

        :param prefs: configurations for message decoder. Empty by default.
        :type prefs: dictionary
        """
        Monitor.__init__(self)
        self._executable_path = prefs.get("diag_revealer_executable_path", "/system/bin/diag_revealer")
        self._fifo_path = prefs.get("diag_revealer_fifo_path", self.TMP_FIFO_FILE)
        self._input_dir = None
        self._log_cut_size = 1.0 # change size to 1.0 M
        self._skip_decoding = False
        self._type_names = []
        self._last_diag_revealer_ts = None
        DMLogPacket.init(prefs)     # Initialize Wireshark dissector

        self.__check_security_policy()


    def __check_security_policy(self):
        """
        Update SELinux policy.
        For Nexus 6/6P, the SELinux policy may forbids the log collection.
        """

        self._run_shell_cmd("su -c setenforce 0")
        self._run_shell_cmd("su -c supolicy --live \"allow init diag_device chr_file {getattr write ioctl}\"")
        self._run_shell_cmd("su -c supolicy --live \"allow init init process execmem\"")
        self._run_shell_cmd("su -c supolicy --live \"allow init properties_device file execute\"")
        self._run_shell_cmd("su -c supolicy --live \"allow atfwd diag_device chr_file {read write open ioctl}\"")
        self._run_shell_cmd("su -c supolicy --live \"allow system_server diag_device chr_file {read write}\"")
        self._run_shell_cmd("su -c supolicy --live \"allow untrusted_app app_data_file file {rename}\"")

    def _run_shell_cmd(self, cmd, wait=False):
        p = subprocess.Popen(cmd, executable=ANDROID_SHELL, shell=True)
        if wait:
            p.wait()
            return p.returncode
        else:
            return None

    def set_skip_decoding(self, val):
        self._skip_decoding = val

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
                raise ValueError("Unsupported log message type: %s" % n)
            else:
                self._type_names.append(n)

    def set_block_size(self, n):
        self.BLOCK_SIZE = n

    def _mkfifo(self, fifo_path):
        try:
            os.mknod(fifo_path, 0666 | stat.S_IFIFO)
        except OSError as err:
            if err.errno == errno.EEXIST:   # if already exists, skip this step
                print "Fifo file already exists, skipping..."
            elif err.errno == errno.EPERM:  # not permitted, try shell command
                print "Not permitted to create fifo file, try to switch to root..."
                retcode = self._run_shell_cmd("su -c mknod %s p" % fifo_path, wait=True)
                if retcode != 0:
                    raise RuntimeError("mknod returns %s" % str(retcode))
            else:
                raise err

    def get_last_diag_revealer_ts(self):
        """
        Return the timestamp when the lastest msg is sent by diag_revealer
        """
        return self._last_diag_revealer_ts

    def run(self):
        """
        Start monitoring the mobile network. This is usually the entrance of monitoring and analysis.

        This function does NOT return or raise any exception.
        """

        generate_diag_cfg = True
        if not self._type_names:
            if os.path.exists(os.path.join(self.DIAG_CFG_DIR, "Diag.cfg")):
                generate_diag_cfg = False
                print "AndroidQmdlMonitor: existing Diag.cfg file will be used."
            else:
                raise RuntimeError("Log type not specified. Please call enable_log() first.")

        try:
            if generate_diag_cfg:
                self._run_shell_cmd("su -c mkdir \"%s\"" % self.DIAG_CFG_DIR)
                fd = open(os.path.join(self.DIAG_CFG_DIR, "Diag.cfg"), "wb")
                dm_collector_c.generate_diag_cfg(fd, self._type_names)
                fd.close()

            self._mkfifo(self._fifo_path)

            # TODO(likayo): need to protect aganist user input
            cmd = "su -c %s %s %s" % (self._executable_path, os.path.join(self.DIAG_CFG_DIR, "Diag.cfg"), self._fifo_path)
            if self._input_dir:
                cmd += " %s %.6f" % (self._input_dir, self._log_cut_size)
                self._run_shell_cmd("su -c mkdir \"%s\"" % self._input_dir)
                self._run_shell_cmd("su -c chmod -R 777 \"%s\"" % self._input_dir, wait=True)
            proc = subprocess.Popen(cmd,
                                    shell=True,
                                    executable=ANDROID_SHELL,
                                    )
            fifo = os.open(self._fifo_path, os.O_RDONLY | os.O_NONBLOCK)

            # Read log packets from diag_revealer
            chproc = ChronicleProcessor()
            while True:
                try:
                    s = os.read(fifo, self.BLOCK_SIZE)
                except OSError as err:
                    if err.errno == errno.EAGAIN or err.errno == errno.EWOULDBLOCK:
                        s = None
                    else:
                        raise err # something else has happened -- better reraise

                while s:   # preprocess metadata
                    ret_msg_type, ret_ts, ret_payload, ret_filename, remain = chproc.process(s)
                    if ret_msg_type == ChronicleProcessor.TYPE_LOG:
                        if ret_ts:
                            self._last_diag_revealer_ts = ret_ts
                        if ret_payload:
                            dm_collector_c.feed_binary(ret_payload)
                    elif ret_msg_type == ChronicleProcessor.TYPE_START_LOG_FILE:
                        if ret_filename:
                            print "Start of %s" % ret_filename
                    elif ret_msg_type == ChronicleProcessor.TYPE_END_LOG_FILE:
                        if ret_filename:
                            print "End of %s" % ret_filename
                            event = Event(  timeit.default_timer(),
                                            "new_diag_log",
                                            ret_filename)
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
                        # print d["type_id"], d["timestamp"]
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
            proc.terminate()
            import traceback
            sys.exit(str(traceback.format_exc()))
            # sys.exit(e)
        except Exception, e:
            import traceback
            sys.exit(str(traceback.format_exc()))
            # sys.exit(e)