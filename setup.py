# -*- coding: utf-8 -*-
import urllib
import os
import sys
import platform
import stat
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

#Download libraries
if not os.path.exists("./libs"):
    os.makedirs("./libs")
if not os.path.exists("./ws_dissector"):
    os.makedirs("./ws_dissector")

print "Downloading libraries..."

if platform.system()=="Darwin":
    prefix="http://metro.cs.ucla.edu/mobile_insight/libs/osx/"
    urllib.urlretrieve (prefix+"libwireshark.5.dylib", "./libs/libwireshark.5.dylib")
    urllib.urlretrieve (prefix+"libwiretap.4.dylib", "./libs/libwiretap.4.dylib")
    urllib.urlretrieve (prefix+"libwsutil.4.dylib", "./libs/libwsutil.4.dylib")
    urllib.urlretrieve (prefix+"ws_dissector", "./ws_dissector/ws_dissector")
    urllib.urlretrieve (prefix+"dm_collector_c.so", "./mobile_insight/monitor/dm_collector/dm_collector_c.so")

    os.chmod("./libs/libwireshark.5.dylib",0o777 | stat.S_IEXEC)
    os.chmod("./libs/libwiretap.4.dylib",0o777 | stat.S_IEXEC)
    os.chmod("./libs/libwsutil.4.dylib",0o777 | stat.S_IEXEC)
    os.chmod("./ws_dissector/ws_dissector",0o777 | stat.S_IEXEC)
    os.chmod("./mobile_insight/monitor/dm_collector/dm_collector_c.so",0o777 | stat.S_IEXEC)

    PACKAGE_DATA = {'mobile_insight.monitor.dm_collector': ['./dm_collector_c.so']}
    DATA_FILES = [(sys.exec_prefix+'/mobile_insight/ws_dissector/',['ws_dissector/ws_dissector']),
                  ('/usr/local/lib/',['libs/libwireshark.5.dylib','libs/libwiretap.4.dylib','libs/libwsutil.4.dylib'])]
elif platform.system()=="Linux":
    arch=platform.architecture()
    prefix=""
    if arch[0]=='32bit':
        prefix="http://metro.cs.ucla.edu/mobile_insight/libs/linux-32/"
    else:
        prefix="http://metro.cs.ucla.edu/mobile_insight/libs/linux-64/"
    urllib.urlretrieve (prefix+"libwireshark.so.5", "./libs/libwireshark.so.5")
    urllib.urlretrieve (prefix+"libwiretap.so.4", "./libs/libwiretap.so.4")
    urllib.urlretrieve (prefix+"libwsutil.so.4", "./libs/libwsutil.so.4")
    urllib.urlretrieve (prefix+"ws_dissector", "./ws_dissector/ws_dissector")
    urllib.urlretrieve (prefix+"dm_collector_c.so", "./mobile_insight/monitor/dm_collector/dm_collector_c.so")
    
    os.chmod("./libs/libwireshark.so.5",0o777 | stat.S_IEXEC)
    os.chmod("./libs/libwiretap.so.4",0o777 | stat.S_IEXEC)
    os.chmod("./libs/libwsutil.so.4",0o777 | stat.S_IEXEC)
    os.chmod("./ws_dissector/ws_dissector",0o777 | stat.S_IEXEC)
    os.chmod("./mobile_insight/monitor/dm_collector/dm_collector_c.so",0o777 | stat.S_IEXEC)

    PACKAGE_DATA = {'mobile_insight.monitor.dm_collector': ['./dm_collector_c.so']}
    DATA_FILES = [(sys.exec_prefix+'/mobile_insight/ws_dissector/',['ws_dissector/ws_dissector']),
                  ('/usr/local/lib/',['libs/libwireshark.so.5','libs/libwiretap.so.4','libs/libwsutil.so.4'])]
elif platform.system() == "Windows":
    PACKAGE_DATA = {'mobile_insight.monitor.dm_collector': ['./dm_collector_c.pyd']}
    ws_files = ['ws_dissector/ws_dissector.exe']
    # include all dlls
    for root, dirs, files in os.walk('ws_dissector'):
        ws_files.extend(['ws_dissector/' + name for name in files if name.endswith('.dll')])
    DATA_FILES = [(sys.exec_prefix+'/mobile_insight/ws_dissector',ws_files)]

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
