# -*- coding: utf-8 -*-
import os
import sys
from distutils.core import setup
import py2exe

# This is an temporary workaround to deal with importing errors using py2exe.
if "py2exe" in sys.argv:
    sys.path.append("./automator")
    sys.path.append("./win_dep")

PY2EXE_OPTIONS = {
    "bundle_files": 1,      # 1: bundle everything
}

setup(
    # setting for console program
    console = [
        {
            "script": "automator/automator.py",
            # "icon_resources": [(1, "resources/icon.ico")],
        },
    ],
    options = { 'py2exe' : PY2EXE_OPTIONS },
    data_files = [("command_files", ["command_files/cmd_dict.txt",
                                     "command_files/example_cmds.txt",])]
)
