# -*- coding: utf-8 -*-
import os
import sys
from distutils.core import setup
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


setup(
    name = 'MobileInsight',
    version = '1.0',
    description = 'Mobile network monitoring and analysis',
    author = 'Yuanjie Li, Jiayao Li',
    author_email = 'yuanjie.li@cs.ucla.edu, likayao@ucla.edu',
    url = 'http://metro.cs.ucla.edu/mobile_insight',
    license = 'Apache License 2.0',
    packages = ['mobile_insight',
                'mobile_insight.analyzer',
                'mobile_insight.monitor',
                'mobile_insight.monitor.dm_collector',
                'mobile_insight.monitor.dm_collector.dm_endec',
                ],
    package_dir = {'mobile_insight.monitor.dm_collector':'mobile_insight/monitor/dm_collector'},
    package_data = {'mobile_insight.monitor.dm_collector':['./dm_collector_c.so']},
    data_files = [(sys.exec_prefix+'/mobile_insight/ws_dissector/',['ws_dissector/ws_dissector'])],
    options = { 'py2exe' : PY2EXE_OPTIONS },
)
