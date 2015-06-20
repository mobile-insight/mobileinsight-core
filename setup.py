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
        sys.path.append("./mobile_insight")
        sys.path.append("./win_dep")

PY2EXE_OPTIONS = {
    "bundle_files": 1,      # 1: bundle everything
}

dm_endec_c_module = Extension('mobile_insight.dm_endec.dm_endec_c',
                                sources = [ "dm_endec_c/dm_endec_c.cpp",
                                            "dm_endec_c/log_packet.cpp",
                                            "dm_endec_c/log_config.cpp",],
                                extra_compile_args=["-std=c++11"])

dm_collector_c_module = Extension('mobile_insight.trace_collector.dm_collector.dm_collector_c',
                                sources = [ "dm_endec_c/dm_collector_c.cpp",
                                            "dm_endec_c/hdlc.cpp",],
                                extra_compile_args=["-std=c++11"])

"dm_endec_c/crc.cpp",
setup(
    # setting for console program
    console = [
        {
            "script": "mobile_insight/automator.py",
            # "icon_resources": [(1, "resources/icon.ico")],
        },
    ],
    packages = ['mobile_insight',
                'mobile_insight.analyzer',
                'mobile_insight.trace_collector',
                'mobile_insight.dm_endec'],
    options = { 'py2exe' : PY2EXE_OPTIONS },
    ext_modules = [dm_endec_c_module, dm_collector_c_module],
)
