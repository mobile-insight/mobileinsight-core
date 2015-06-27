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



dm_collector_c_module = Extension('mobile_insight.monitor.dm_collector.dm_collector_c',
                                sources = [ "dm_collector_c/dm_collector_c.cpp",
                                            "dm_collector_c/hdlc.cpp",
                                            "dm_collector_c/log_config.cpp",
                                            "dm_collector_c/log_packet.cpp",],
                                extra_compile_args=["-std=c++11"])

setup(
    # setting for console program
    # console = [
    #     {
    #         "script": "mobile_insight/automator.py",
    #         # "icon_resources": [(1, "resources/icon.ico")],
    #     },
    # ],
    name = 'MobileInsight',
    version = '1.0',
    description = 'Mobile network monitoring and analysis',
    author = 'Yuanjie Li, Jiayao Li',
    url = 'http://metro.cs.ucla.edu/mobile_insight',
    packages = ['mobile_insight',
                'mobile_insight.analyzer',
                'mobile_insight.monitor',
                'mobile_insight.monitor.dm_collector',
                'mobile_insight.monitor.dm_collector.dm_endec',
                ],
    options = { 'py2exe' : PY2EXE_OPTIONS },
    ext_modules = [dm_collector_c_module],
)
