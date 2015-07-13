# -*- coding: utf-8 -*-
import urllib
import os
import platform
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
    
    arch=platform.architecture()
    if arch[0]!='64bit':
        print "Unsupported operating system: "+str(arch)
        sys.exit()

    prefix="http://metro.cs.ucla.edu/mobile_insight/libs/osx/"
    if not os.path.isfile("./libs/libwireshark.5.dylib"):
        urllib.urlretrieve (prefix+"libwireshark.5.dylib", "./libs/libwireshark.5.dylib")
    if not os.path.isfile("./libs/libwiretap.4.dylib"):
        urllib.urlretrieve (prefix+"libwiretap.4.dylib", "./libs/libwiretap.4.dylib")
    if not os.path.isfile("./libs/libwsutil.4.dylib"):
        urllib.urlretrieve (prefix+"libwsutil.4.dylib", "./libs/libwsutil.4.dylib")
    if not os.path.isfile("./ws_dissector/ws_dissector"):
        urllib.urlretrieve (prefix+"ws_dissector", "./ws_dissector/ws_dissector")
    if not os.path.isfile("./mobile_insight/monitor/dm_collector/dm_collector_c.so"):
        urllib.urlretrieve (prefix+"dm_collector_c.so", "./mobile_insight/monitor/dm_collector/dm_collector_c.so")

    os.chmod("./libs/libwireshark.5.dylib",0o777 | stat.S_IEXEC)
    os.chmod("./libs/libwiretap.4.dylib",0o777 | stat.S_IEXEC)
    os.chmod("./libs/libwsutil.4.dylib",0o777 | stat.S_IEXEC)
    os.chmod("./ws_dissector/ws_dissector",0o777 | stat.S_IEXEC)
    os.chmod("./mobile_insight/monitor/dm_collector/dm_collector_c.so",0o777 | stat.S_IEXEC)

    PACKAGE_DATA = {'mobile_insight.monitor.dm_collector': ['./dm_collector_c.so']}
    lib_list = ["./libs/"+x for x in os.listdir("./libs/")]
    DATA_FILES = [(sys.exec_prefix+'/mobile_insight/ws_dissector/',['ws_dissector/ws_dissector']),
                  ('/usr/local/lib/',lib_list)]
elif platform.system()=="Linux":

    arch=platform.architecture()
    if arch[0]=='32bit':
        prefix="http://metro.cs.ucla.edu/mobile_insight/libs/linux-32/"
    elif arch[0]=='64bit':
        prefix="http://metro.cs.ucla.edu/mobile_insight/libs/linux-64/"
    else:
        print "Unsupported operating system: "+str(arch)
        sys.exit()

    if not os.path.isfile("./libs/libwireshark.so.5"):
        urllib.urlretrieve (prefix+"libwireshark.so.5", "./libs/libwireshark.so.5")
    if not os.path.isfile("./libs/libwiretap.so.4"):
        urllib.urlretrieve (prefix+"libwiretap.so.4", "./libs/libwiretap.so.4")
    if not os.path.isfile("./libs/libwsutil.so.4"):
        urllib.urlretrieve (prefix+"libwsutil.so.4", "./libs/libwsutil.so.4")
    if not os.path.isfile("./ws_dissector/ws_dissector"):
        urllib.urlretrieve (prefix+"ws_dissector", "./ws_dissector/ws_dissector")
    if not os.path.isfile("./mobile_insight/monitor/dm_collector/dm_collector_c.so"):
        urllib.urlretrieve (prefix+"dm_collector_c.so", "./mobile_insight/monitor/dm_collector/dm_collector_c.so")
    
    os.chmod("./libs/libwireshark.so.5",0o777 | stat.S_IEXEC)
    os.chmod("./libs/libwiretap.so.4",0o777 | stat.S_IEXEC)
    os.chmod("./libs/libwsutil.so.4",0o777 | stat.S_IEXEC)
    os.chmod("./ws_dissector/ws_dissector",0o777 | stat.S_IEXEC)
    os.chmod("./mobile_insight/monitor/dm_collector/dm_collector_c.so",0o777 | stat.S_IEXEC)

    PACKAGE_DATA = {'mobile_insight.monitor.dm_collector': ['./dm_collector_c.so']}
    lib_list = ["./libs/"+x for x in os.listdir("./libs/")]
    DATA_FILES = [(sys.exec_prefix+'/mobile_insight/ws_dissector/',['ws_dissector/ws_dissector']),
                  ('/usr/local/lib/',lib_list)]
elif platform.system() == "Windows":
    arch=platform.architecture()
    if arch[0]=='32bit':
        prefix="http://metro.cs.ucla.edu/mobile_insight/libs/win-32/"
        if not os.path.isfile("./libs/comerr32.dll"):
            urllib.urlretrieve (prefix+"comerr32.dll", "./libs/comerr32.dll")
        if not os.path.isfile("./libs/k5sprt32.dll"):
            urllib.urlretrieve (prefix+"k5sprt32.dll", "./libs/k5sprt32.dll")
        if not os.path.isfile("./libs/krb5_32.dll"):
            urllib.urlretrieve (prefix+"krb5_32.dll", "./libs/krb5_32.dll")
        if not os.path.isfile("./libs/libcares-2.dll"):
            urllib.urlretrieve (prefix+"libcares-2.dll", "./libs/libcares-2.dll")
        if not os.path.isfile("./libs/libffi-6.dll"):
            urllib.urlretrieve (prefix+"libffi-6.dll", "./libs/libffi-6.dll")
        if not os.path.isfile("./libs/libgcc_s_sjlj-1.dll"):
            urllib.urlretrieve (prefix+"libgcc_s_sjlj-1.dll", "./libs/libgcc_s_sjlj-1.dll")
        if not os.path.isfile("./libs/libgcrypt-20.dll"):
            urllib.urlretrieve (prefix+"libgcrypt-20.dll", "./libs/libgcrypt-20.dll")
        if not os.path.isfile("./libs/libGeoIP-1.dll"):
            urllib.urlretrieve (prefix+"libGeoIP-1.dll", "./libs/libGeoIP-1.dll")

        if not os.path.isfile("./libs/libglib-2.0-0.dll"):
            urllib.urlretrieve (prefix+"libglib-2.0-0.dll", "./libs/libglib-2.0-0.dll")
        if not os.path.isfile("./libs/libgmodule-2.0-0.dll"):
            urllib.urlretrieve (prefix+"libgmodule-2.0-0.dll", "./libs/libgmodule-2.0-0.dll")
        if not os.path.isfile("./libs/libgmp-10.dll"):
            urllib.urlretrieve (prefix+"libgmp-10.dll", "./libs/libgmp-10.dll")
        if not os.path.isfile("./libs/libgnutls-28.dll"):
            urllib.urlretrieve (prefix+"libgnutls-28.dll", "./libs/libgnutls-28.dll")
        if not os.path.isfile("./libs/libgpg-error-0.dll"):
            urllib.urlretrieve (prefix+"libgpg-error-0.dll", "./libs/libgpg-error-0.dll")
        if not os.path.isfile("./libs/libhogweed-2-4.dll"):
            urllib.urlretrieve (prefix+"libhogweed-2-4.dll", "./libs/libhogweed-2-4.dll")
        if not os.path.isfile("./libs/libintl-8.dll"):
            urllib.urlretrieve (prefix+"libintl-8.dll", "./libs/libintl-8.dll")
        if not os.path.isfile("./libs/libnettle-4-6.dll"):
            urllib.urlretrieve (prefix+"libnettle-4-6.dll", "./libs/libnettle-4-6.dll")

        if not os.path.isfile("./libs/libp11-kit-0.dll"):
            urllib.urlretrieve (prefix+"libp11-kit-0.dll", "./libs/libp11-kit-0.dll")
        if not os.path.isfile("./libs/libsmi-2.dll"):
            urllib.urlretrieve (prefix+"libsmi-2.dll", "./libs/libsmi-2.dll")
        if not os.path.isfile("./libs/libtasn1-6.dll"):
            urllib.urlretrieve (prefix+"libtasn1-6.dll", "./libs/libtasn1-6.dll")
        if not os.path.isfile("./libs/libwireshark.dll"):
            urllib.urlretrieve (prefix+"libwireshark.dll", "./libs/libwireshark.dll")
        if not os.path.isfile("./libs/libwsutil.dll"):
            urllib.urlretrieve (prefix+"libwsutil.dll", "./libs/libwsutil.dll")
        if not os.path.isfile("./libs/lua52.dll"):
            urllib.urlretrieve (prefix+"lua52.dll", "./libs/lua52.dll")
        if not os.path.isfile("./libs/wiretap-1.12.0.dll"):
            urllib.urlretrieve (prefix+"wiretap-1.12.0.dll", "./libs/wiretap-1.12.0.dll")
        if not os.path.isfile("./libs/zlib1.dll"):
            urllib.urlretrieve (prefix+"zlib1.dll", "./libs/zlib1.dll")

        if not os.path.isfile("./ws_dissector/ws_dissector.exe"):
            urllib.urlretrieve (prefix+"ws_dissector.exe", "./ws_dissector/ws_dissector.exe")
        if not os.path.isfile("./mobile_insight/monitor/dm_collector/dm_collector_c.pyd"):
            urllib.urlretrieve (prefix+"dm_collector_c.pyd", "./mobile_insight/monitor/dm_collector/dm_collector_c.pyd")

    elif arch[0]=='64bit':
        prefix="http://metro.cs.ucla.edu/mobile_insight/libs/win-64/"

        if not os.path.isfile("./libs/libcares-2.dll"):
            urllib.urlretrieve (prefix+"libcares-2.dll", "./libs/libcares-2.dll")
        if not os.path.isfile("./libs/libffi-6.dll"):
            urllib.urlretrieve (prefix+"libffi-6.dll", "./libs/libffi-6.dll")
        if not os.path.isfile("./libs/libgcc_s_seh-1.dll"):
            urllib.urlretrieve (prefix+"libgcc_s_seh-1.dll", "./libs/libgcc_s_seh-1.dll")
        if not os.path.isfile("./libs/libgcrypt-20.dll"):
            urllib.urlretrieve (prefix+"libgcrypt-20.dll", "./libs/libgcrypt-20.dll")
        if not os.path.isfile("./libs/libGeoIP-1.dll"):
            urllib.urlretrieve (prefix+"libGeoIP-1.dll", "./libs/libGeoIP-1.dll")

        if not os.path.isfile("./libs/libglib-2.0-0.dll"):
            urllib.urlretrieve (prefix+"libglib-2.0-0.dll", "./libs/libglib-2.0-0.dll")
        if not os.path.isfile("./libs/libgmodule-2.0-0.dll"):
            urllib.urlretrieve (prefix+"libgmodule-2.0-0.dll", "./libs/libgmodule-2.0-0.dll")
        if not os.path.isfile("./libs/libgmp-10.dll"):
            urllib.urlretrieve (prefix+"libgmp-10.dll", "./libs/libgmp-10.dll")
        if not os.path.isfile("./libs/libgnutls-28.dll"):
            urllib.urlretrieve (prefix+"libgnutls-28.dll", "./libs/libgnutls-28.dll")
        if not os.path.isfile("./libs/libgpg-error6-0.dll"):
            urllib.urlretrieve (prefix+"libgpg-error6-0.dll", "./libs/libgpg-error6-0.dll")
        if not os.path.isfile("./libs/libhogweed-2-4.dll"):
            urllib.urlretrieve (prefix+"libhogweed-2-4.dll", "./libs/libhogweed-2-4.dll")

        if not os.path.isfile("./libs/libintl-8.dll"):
            urllib.urlretrieve (prefix+"libintl-8.dll", "./libs/libintl-8.dll")
        if not os.path.isfile("./libs/libnettle-4-6.dll"):
            urllib.urlretrieve (prefix+"libnettle-4-6.dll", "./libs/libnettle-4-6.dll")
        if not os.path.isfile("./libs/libp11-kit-0.dll"):
            urllib.urlretrieve (prefix+"libp11-kit-0.dll", "./libs/libp11-kit-0.dll")
        if not os.path.isfile("./libs/libsmi-2.dll"):
            urllib.urlretrieve (prefix+"libsmi-2.dll", "./libs/libsmi-2.dll")
        if not os.path.isfile("./libs/libtasn1-6.dll"):
            urllib.urlretrieve (prefix+"libtasn1-6.dll", "./libs/libtasn1-6.dll")
        if not os.path.isfile("./libs/libwireshark.dll"):
            urllib.urlretrieve (prefix+"libwireshark.dll", "./libs/libwireshark.dll")

        if not os.path.isfile("./libs/libwsutil.dll"):
            urllib.urlretrieve (prefix+"libwsutil.dll", "./libs/libwsutil.dll")
        if not os.path.isfile("./libs/lua52.dll"):
            urllib.urlretrieve (prefix+"lua52.dll", "./libs/lua52.dll")
        if not os.path.isfile("./libs/wiretap-1.12.0.dll"):
            urllib.urlretrieve (prefix+"wiretap-1.12.0.dll", "./libs/wiretap-1.12.0.dll")
        if not os.path.isfile("./libs/zlib1.dll"):
            urllib.urlretrieve (prefix+"zlib1.dll", "./libs/zlib1.dll")

        if not os.path.isfile("./ws_dissector/ws_dissector.exe"):
            urllib.urlretrieve (prefix+"ws_dissector.exe", "./ws_dissector/ws_dissector.exe")
        if not os.path.isfile("./mobile_insight/monitor/dm_collector/dm_collector_c.pyd"):
            urllib.urlretrieve (prefix+"dm_collector_c.pyd", "./mobile_insight/monitor/dm_collector/dm_collector_c.pyd")

    else:
        print "Unsupported operating system: "+str(arch)
        sys.exit()

    PACKAGE_DATA = {'mobile_insight.monitor.dm_collector': ['./dm_collector_c.pyd']}
    ws_files = ['ws_dissector/ws_dissector.exe']
    # include all dlls
    for root, dirs, files in os.walk('ws_dissector'):
        ws_files.extend(['ws_dissector/' + name for name in files if name.endswith('.dll')])

    lib_list = ["./libs/"+x for x in os.listdir("./libs/")]
    DATA_FILES = [(sys.exec_prefix+'/mobile_insight/ws_dissector',ws_files),
                  (sys.exec_prefix,lib_list)]
else:
    print "Unsupported operating system: "+str(arch)
    sys.exit()

setup(
    name = 'MobileInsight',
    version = '1.0',
    description = 'Mobile network monitoring and analysis',
    author = 'Yuanjie Li, Jiayao Li',
    author_email = 'yuanjie.li@cs.ucla.edu, likayo@ucla.edu',
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
