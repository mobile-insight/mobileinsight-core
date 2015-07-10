# -*- coding: utf-8 -*-
import os
import platform
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

if platform.system() == "Windows":
    PACKAGE_DATA = {'mobile_insight.monitor.dm_collector': ['./dm_collector_c.pyd']}
    ws_files = ['ws_dissector/ws_dissector.exe']
    # include all dlls
    for root, dirs, files in os.walk('ws_dissector'):
        ws_files.extend(['ws_dissector/' + name for name in files if name.endswith('.dll')])
    DATA_FILES = [(sys.exec_prefix+'/mobile_insight/ws_dissector',ws_files)]
else:   # Linux or OS X
    PACKAGE_DATA = {'mobile_insight.monitor.dm_collector': ['./dm_collector_c.so']}
    DATA_FILES = [(sys.exec_prefix+'/mobile_insight/ws_dissector/',['ws_dissector/ws_dissector']),
                  ('/usr/local/lib/',['libs/libwireshark.5.dylib','libs/libwiretap.4.dylib','libs/libwsutil.4.dylib'])]

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
    package_data = PACKAGE_DATA,
    data_files = DATA_FILES,
    options = { 'py2exe' : PY2EXE_OPTIONS },
)
