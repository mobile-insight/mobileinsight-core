# -*- coding: utf-8 -*-
"""
replayer.py

A pickle message replayer

Author: Jiayao Li
"""

__all__ = ["Replayer"]

import pickle
import sys

from monitor import Monitor


class Replayer(Monitor):

    def __init__(self):
        Monitor.__init__(self)

    def set_input_path(self, path):
        self._input_path = path
        self._input_file = open(path, "rb")

    def run(self):
        try:
            while True:
                event = pickle.load(self._input_file)
                self.send(event)

        except EOFError, e:
            print "Pickle file EOF."
        except Exception, e:
            sys.exit(e)
