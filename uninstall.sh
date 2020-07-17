#!/bin/bash
# Uninstallation script for mobileinsight-core
# It cleans up the installed libraries
# Author  : Zengwen Yuan
# Date    : 2017-11-13
# Version : 2.0

# Wireshark version to install
WS_VER=2.0.*
MOBILEINSIGHT_PATH=$(pwd)

mi_dir=$(python -c "import sys; print(sys.exec_prefix + '/mobile_insight')")
mi_dir2=$(python3 -c "import sys; print(sys.exec_prefix + '/mobile_insight')")

delete () {
    if [[ -e $1 ]]; then
        echo "deleting $1"
        sudo rm -rf $1
    fi
}

uninstall() {
    # Clean up compiled codes
    delete ${MOBILEINSIGHT_PATH}/MobileInsight.egg-info
    delete ${MOBILEINSIGHT_PATH}/build
    delete ${MOBILEINSIGHT_PATH}/dist
    delete ${MOBILEINSIGHT_PATH}/ws_dissector/ws_dissector
    delete ${MOBILEINSIGHT_PATH}/wireshark-${WS_VER}
    delete ~/.cache/Python-Eggs/MobileInsight*
    delete ~/.python-eggs/MobileInsight*
    delete /usr/local/bin/mi-gui
    delete /usr/local/bin/ws_dissector
    delete /usr/local/share/mobileinsight/

    # Clean up old MobileInsight-2.x installed libraries
    delete /usr/lib/libwireshark.so*
    delete /usr/lib/libwiretap.so*
    delete /usr/lib/libwsutil.so*
    delete /usr/lib/libwireshark.6.dylib
    delete /usr/lib/libwireshark.dylib
    delete /usr/lib/libwiretap.5.dylib
    delete /usr/lib/libwiretap.dylib
    delete /usr/lib/libwsutil.6.dylib
    delete /usr/lib/libwsutil.dylib

    # Clean up old MobileInsight-2.x installed binary
    delete ${mi_dir}
    delete ${mi_dir2}
}

readme() {
    echo "Uninstalling MobileInsight-core v2.0 from your computer"
    echo "This script will require sudo access to delete following files:"
    echo "${MOBILEINSIGHT_PATH}/MobileInsight.egg-info"
    echo "${MOBILEINSIGHT_PATH}/build"
    echo "${MOBILEINSIGHT_PATH}/dist"
    echo "${MOBILEINSIGHT_PATH}/ws_dissector/ws_dissector"
    echo "${MOBILEINSIGHT_PATH}/wireshark-${WS_VER}"
    echo "${HOME}/.cache/Python-Eggs/MobileInsight*"
    echo "${HOME}/.python-eggs/MobileInsight*"
    echo "/usr/local/bin/mi-gui"
    echo "/usr/local/bin/ws_dissector"
    echo "/usr/local/share/mobileinsight/*"
    echo "/usr/lib/libwireshark.so*"
    echo "/usr/lib/libwsutil.so*"
    echo "/usr/lib/libwireshark.6.dylib"
    echo "/usr/lib/libwireshark.dylib"
    echo "/usr/lib/libwiretap.5.dylib"
    echo "/usr/lib/libwiretap.dylib"
    echo "/usr/lib/libwsutil.6.dylib"
    echo "/usr/lib/libwsutil.dylib"
    echo ${mi_dir}
    echo " "
}

readme
read -p "Do you wish to proceed to uninstall the listed files? (y/n) " answer
case ${answer:0:1} in
    y|Y )
        echo " "
        echo "Uninstalling old MobileInsight build files and libs..."
        uninstall
        echo "Uninstallation finished."
        echo " "
    ;;
    * )
        echo "Abort uninstallation. Exiting."
    ;;
esac
