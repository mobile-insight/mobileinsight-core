# -*- coding: utf-8 -*-
import os
import sys
from distutils.core import setup, Extension
try:
    import py2exe
    use_py2exe = True
except ImportError:
    use_py2exe = False

if use_py2exe:
    # This is an temporary workaround to deal with importing errors using py2exe.
    if "py2exe" in sys.argv:
        sys.path.append("./automator")
        sys.path.append("./win_dep")

PY2EXE_OPTIONS = {
    "bundle_files": 1,      # 1: bundle everything
}

c_dm_endec_module = Extension('automator.dm_endec.dm_endec_c',
                                sources = ['dm_endec_c/dm_endec_c.cpp'],
                                extra_compile_args=["-std=c++11"])

setup(
    # setting for console program
    console = [
        {
            "script": "automator/automator.py",
            # "icon_resources": [(1, "resources/icon.ico")],
        },
    ],
    packages = ['automator',
                'automator.analyzer',
                'automator.trace_collector',
                'automator.dm_endec'],
    options = { 'py2exe' : PY2EXE_OPTIONS },
    ext_modules = [c_dm_endec_module],
)
