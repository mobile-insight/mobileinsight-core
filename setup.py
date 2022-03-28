# MobileInsight package installation script

import urllib.request, urllib.parse, urllib.error
import re
import os
import platform
import sys
import platform
import stat
from distutils.core import setup
from setuptools import setup, Extension

try:
    import py2exe

    use_py2exe = True
except ImportError:
    use_py2exe = False

if use_py2exe:
    # workaround to deal with importing errors using py2exe
    if "py2exe" in sys.argv:
        sys.path.append("./mobile_insight")
        sys.path.append("./win_dep")

PY2EXE_OPTIONS = {
    "bundle_files": 1,  # bundle everything
}

# =============================================================================
# Remove the "-Wstrict-prototypes" compiler option, which isn't valid for C++.
import distutils.sysconfig

cfg_vars = distutils.sysconfig.get_config_vars()
for key, value in list(cfg_vars.items()):
    if type(value) == str:
        cfg_vars[key] = value.replace("-Wstrict-prototypes", "")
# =============================================================================

dm_collector_c_module = Extension('mobile_insight.monitor.dm_collector.dm_collector_c',
                                  sources=["dm_collector_c/dm_collector_c.cpp",
                                           "dm_collector_c/export_manager.cpp",
                                           "dm_collector_c/hdlc.cpp",
                                           "dm_collector_c/log_config.cpp",
                                           "dm_collector_c/log_packet.cpp",
                                           "dm_collector_c/utils.cpp", ],
                                  define_macros=[('EXPOSE_INTERNAL_LOGS', 1), ]
                                  )


def parse_libs(url, suffix):
    pattern = '<a href=".*%s.*">.*</a>' % suffix
    response = urllib.request.urlopen(url).read()
    res = []
    for item in re.findall(pattern, response):
        lib_start = item.find('href="')
        lib_end = item.find('">')
        lib_name = item[lib_start + 6:lib_end]
        res.append(lib_name)
    return res


def download_libs(url, libs, chmod=True):
    for lib in libs:
        if not os.path.isfile("./libs/" + lib):
            urllib.request.urlretrieve(url + lib, "./libs/" + lib)
    if chmod:
        for lib in libs:
            os.chmod("./libs/" + lib, 0o755 | stat.S_IEXEC)


def download_win(url):
    if not os.path.isfile("./ws_dissector/ws_dissector.exe"):
        urllib.request.urlretrieve(url + "ws_dissector.exe", "./ws_dissector/ws_dissector.exe")
    if not os.path.isfile("./mobile_insight/monitor/dm_collector/dm_collector_c.pyd"):
        urllib.request.urlretrieve(url + "dm_collector_c.pyd",
                                   "./mobile_insight/monitor/dm_collector/dm_collector_c.pyd")


print("Building libraries...")

if platform.system() == "Darwin" or platform.system() == "Linux":
    PACKAGE_DATA = {'mobile_insight.monitor.dm_collector': ['./dm_collector_c*.so']}
    DATA_FILES = []

elif platform.system() == "Windows":
    # Download necessary libraries
    if not os.path.exists("./libs"):
        os.makedirs("./libs")
    if not os.path.exists("./ws_dissector"):
        os.makedirs("./ws_dissector")

    arch = platform.architecture()
    if arch[0] == '32bit':
        url = "http://www.mobileinsight.net/libs/win-32/libs/"
    elif arch[0] == '64bit':
        url = "http://www.mobileinsight.net/libs/win-32/libs/"
    else:
        print(("Unsupported operating system: " + str(arch)))
        sys.exit()

    libs = parse_libs(url, "dll")
    download_libs(url, libs, False)
    download_win(url)

    PACKAGE_DATA = {'mobile_insight.monitor.dm_collector': ['./dm_collector_c.pyd']}
    ws_files = ['ws_dissector/ws_dissector.exe']
    # include all dlls
    for root, dirs, files in os.walk('ws_dissector'):
        ws_files.extend(['ws_dissector\\' + name for name in files if name.endswith('.dll')])

    lib_list = ["./libs/" + x for x in os.listdir("./libs/")]
    DATA_FILES = [(sys.exec_prefix + '\\mobile_insight\\ws_dissector\\', ws_files),
                  (sys.exec_prefix + '\\mobile_insight\\ws_dissector\\', lib_list)]
else:
    print(("Unsupported operating system: " + str(platform.system())))
    sys.exit()

setup(
    name='MobileInsight',
    version='6.0.0',
    description='Mobile network monitoring and analysis',
    author='MobileInsight team',
    author_email='support@mobileinsight.net',
    url='http://www.mobileinsight.net',
    license='Apache License 2.0',
    packages=['mobile_insight',
              'mobile_insight.analyzer',
              'mobile_insight.analyzer.kpi',
              'mobile_insight.monitor',
              'mobile_insight.monitor.dm_collector',
              'mobile_insight.monitor.dm_collector.dm_endec',
              ],
    package_data=PACKAGE_DATA,
    data_files=DATA_FILES,
    options={'py2exe': PY2EXE_OPTIONS},
    ext_modules=[dm_collector_c_module],
)
