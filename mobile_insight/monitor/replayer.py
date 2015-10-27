#!/usr/bin/python
# Filename: replayer.py
"""
A replayer of messages saved by MsgSerializer.

Author: Jiayao Li
"""

__all__ = ["Replayer"]

import pickle
import sys

from monitor import Monitor


class Replayer(Monitor):
    """
    A replayer of messages saved by MsgSerializer. It is implemented using Python pickle format.
    """

    def __init__(self):
        Monitor.__init__(self)

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
                event = pickle.load(self._input_file)
                self.send(event)

        except EOFError, e:
            # Pickle file EOF
            pass
        except Exception, e:
            sys.exit(e)
