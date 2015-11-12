#!/usr/bin/python
# Filename: android_dev_diag_monitor.py
"""
android_dev_diag_monitor.py

Author: Jiayao Li
"""

__all__ = ["AndroidDevDiagMonitor"]


import os
import re
import subprocess
import sys
import timeit

from monitor import Monitor, Event
from dm_collector import dm_collector_c, DMLogPacket, FormatError


class AndroidDevDiagMonitor(Monitor):
    """
    An QMDL monitor for Android devics. Require root access to run.
    """

    #: a list containing the currently supported message types.
    SUPPORTED_TYPES = set(dm_collector_c.log_packet_types)

    DIAG_CFG_DIR = "/sdcard/diag_logs"

    def __init__(self, prefs={}):
        """
        Configure this class with user preferences.
        This method should be called before any actual decoding.

        :param prefs: configurations for message decoder. Empty by default.
        :type prefs: dictionary
        """
        Monitor.__init__(self)
        self._executable_path = prefs.get("diag_revealer_path", "/system/bin/diag_revealer")
        self._type_names = []
        DMLogPacket.init(prefs)     # Initialize Wireshark dissector

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

            # TODO(likayo): need to protect aganist user input
            cmd = "su -c %s %s" % (self._executable_path, os.path.join(self.DIAG_CFG_DIR, "Diag.cfg"))
            proc = subprocess.Popen(cmd,
                                    bufsize=-1,
                                    shell=True,
                                    executable=ANDROID_SHELL,
                                    stdout=subprocess.PIPE,
                                    )

            # Read log packets from diag_revealer
            BLOCK_SIZE = 64
            while True:
                s = proc.stdout.read(BLOCK_SIZE)
                if s:
                    print "Received %d bytes" % len(s)
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


        except (KeyboardInterrupt, RuntimeError), e:
            print "\n\n%s Detected: Disabling all logs" % type(e).__name__
            sys.exit(e)
        except Exception, e:
            sys.exit(e)
