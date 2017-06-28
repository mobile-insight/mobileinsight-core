#!/bin/bash
# Uninstallation script for mobileinsight-core
# It cleans up the installed libraries
# Author  : Zengwen Yuan
# Date    : 2017-06-26
# Version : 1.0

# Wireshark version to install
WS_VER=2.0.*
MOBILEINSIGHT_PATH=$(pwd)

# Clean up compiled codes
cd ${MOBILEINSIGHT_PATH}
sudo rm -rf MobileInsight.egg-info build dist ws_dissector/ws_dissector wireshark-${WS_VER}
sudo rm -rf ~/.cache/Python-Eggs/MobileInsight*
sudo rm -rf ~/.python-eggs/MobileInsight*

# Clean up installed binaries
sudo rm /usr/local/bin/mi-gui
sudo rm /usr/local/bin/ws_dissector
sudo rm -rf /usr/local/share/mobileinsight/

# Clean up old MobileInsight-2.x installed libraries
sudo rm /usr/lib/libwireshark.so*
sudo rm /usr/lib/libwiretap.so*
sudo rm /usr/lib/libwsutil.so*
sudo rm /usr/lib/libwireshark.6.dylib
sudo rm /usr/lib/libwireshark.dylib
sudo rm /usr/lib/libwiretap.5.dylib
sudo rm /usr/lib/libwiretap.dylib
sudo rm /usr/lib/libwsutil.6.dylib
sudo rm /usr/lib/libwsutil.dylib

# Clean up old MobileInsight-2.x installed binary
sudo rm -rf $((echo "import sys" ; echo "print sys.exec_prefix+'/mobile_insight'") | python)