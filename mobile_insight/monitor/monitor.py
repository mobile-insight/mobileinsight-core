#!/usr/bin/python
# Filename: monitor.py
"""
An abstraction for mobile network monitor.
All specific trace collectors (DM, RIL, pcap, Replayer, etc.) derive from it.

Author: Yuanjie Li
"""

from ..element import Element, Event


class Monitor(Element):
    """
    An abstraction for mobile network monitors
    """

    def __init__(self):
        # No source for Monitor
        Element.__init__(self)

        self._skip_decoding = False

        self._save_log_path = None
        self._save_file = None

    def available_log_types(self):
        """
        Return available log types
        """
        return None

    def save_log_as(self, path):
        """
        Save the log as a mi2log file (for offline analysis)

        :param path: the file name to be saved
        :type path: string
        :param log_types: a filter of message types to be saved
        :type log_types: list of string
        """
        pass

    def set_skip_decoding(self, decoding):
        """
        Configure whether deferred message decoding is enabled

        :param decoding: if True, only the message header would be decoded, otherwise the entire packet would be decoded
        :type decoding: Boolean
        """
        self._skip_decoding = decoding

    # Add an analyzer that needs the message
    def register(self, analyzer):
        """
        Register an analyzer driven by this monitor

        :param analyzer: the analyzer to be added
        :type analyzer: Analyzer
        """
        if analyzer not in self.to_list:
            self.to_list.append(analyzer)

    def deregister(self, analyzer):
        """
        Deregister an analyzer driven by this monitor

        :param analyzer: the analyzer to be removed
        :type analyzer: Analyzer
        """
        if analyzer in self.to_list:
            self.to_list.remove(analyzer)

    def enable_log(self, type_name):
        """
        Enable the messages to be monitored.

        :param type_name: the message type(s) to be monitored
        :type type_name: string or list
        """
        pass

    def enable_log_all(self):
        """
        Enable all supported logs
        """
        pass

    def set_sampling_rate(self, sampling_rate):
        """
        Set a target sampling rate for cross-layer sampling

        :param sampling_rate: the target sampling rate
        :type sampling_rate: an integer between 0 and 100
        """
        pass

    def run(self):
        """
        Start monitoring the mobile network. This is usually the entrance of monitoring and analysis.

        This method should be overloaded in every subclass.
        """
        # send() is used to pass message to analyzers
        # EVERY derived collector SHOULD overload this function to generate msg (event)
        # Currently only QXDM is used, and each time only ONE collector can be used
        # TODO: we may extend this module to support multiple sources later
        pass
