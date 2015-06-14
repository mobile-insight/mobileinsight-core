#!/usr/bin/env python
"""
offline-analysis.py

Read an offline trace for LTE NAS analysis

Author: Yuanjie Li
"""

import binascii
import os
import optparse
import serial
import sys
import logging

from trace_collector import *
from analyzer import *

def init_opt():
    """
    Initialize and return the option parser.
    """
    opt = optparse.OptionParser(prog="offline-analysis",
                                usage="usage: %prog [options] -p TRACE_PATH ...",
                                description="Example for LTE NAS analysis")
    opt.add_option("-p", "--path",
                    metavar="STR",
                    action="store", type="string", dest="path", 
                    help="The trace path name.")
    return opt


if __name__ == "__main__":
    opt = init_opt()
    options, args = opt.parse_args(sys.argv[1:])

    if not options.path:
        opt.error("please use -p option to specify trace path name.")
        sys.exit(1)

    # Initialize trace collector
    src = PickleCollector()
    src.set_input_path(options.path)


    # Initialize the analyzer
    lte_nas_analyzer = LteNasAnalyzer()
    lte_nas_analyzer.set_log("lte-nas-analysis.log",logging.INFO)

    # Link the analyzer with the trace collector
    lte_nas_analyzer.set_source(src) 
    

    # Start the analysis
    src.run()