# -*- coding: utf-8 -*-
"""
pickle_collector.py

A pickle message replayer

Author: Jiayao Li
"""

__all__ = ["PickleCollector"]

import pickle

from trace_collector import TraceCollector


class PickleCollector(TraceCollector):

    def __init__(self):
        TraceCollector.__init__(self)

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
