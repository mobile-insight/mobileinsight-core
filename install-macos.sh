#!/bin/bash
# Installation script for mobileinsight-core on macOS
# It installs package under /usr/local folder
# Author  : Zengwen Yuan
# Date    : 2017-11-13
# Version : 3.0

# set -e
# set -u

echo "** Installer Script for mobileinsight-core on macOS **"
echo " "
echo "  Author : Zengwen Yuan (zyuan [at] cs.ucla.edu)"
echo "  Date   : 2017-11-13"
echo "  Rev    : 3.0"
echo "  Usage  : ./install-macos.sh"
echo " "

echo "Upgrading MobileInsight..."
yes | ./uninstall.sh

# Wireshark version to install
ws_ver=2.0.13

# Use local library path
PREFIX=/usr/local
MOBILEINSIGHT_PATH=$(pwd)
WIRESHARK_SRC_PATH=${MOBILEINSIGHT_PATH}/wireshark-${ws_ver}

PYTHON=python2
PIP=pip2

# Install up-to-date Homebrew and use default prefixes
echo "Checking if you have Homebrew installed..."
if [[ $(which -s brew) ]] ; then
    echo "It appears that Homebrew is not installed on your computer, installing..."
    ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
else
    echo "Updating Homebrew"
    brew update
fi

echo "Consulting Homebrew doctor for potential issues..."
echo -e "Output from Homebrew doctor:\n"
brew doctor
if [[ $? != 0 ]] ; then
    echo "Homebrew reported that your environment has some issues! You must fix them before proceeding."
    echo "MobileInsight installation failed. Exiting with status 2."
    exit 2
else
    echo "All clear."
fi

# Check if Wireshark is installed
brew_ws_ver=$(brew ls --versions wireshark)
if [[ $? != 0 ]]; then
    echo "You don't have a Wireshark installed by Homebrew, installing..."
    # Wireshark is not installed, install dependencies
    brew install pkg-config cmake
    brew install glib gnutls libgcrypt dbus
    brew install geoip c-ares
    # Install Wireshark stable version 2.0.x using our own formulae
    brew install ./wireshark.rb
elif [[ $brew_ws_ver == *${ws_ver}* ]]; then
    echo "Congrats! You have a Wireshark version ${ws_ver} installed, continuing..."
else
    echo "You have a Wireshark other than current version installed."
    echo "Installing Wireshark version ${ws_ver}..."
    brew remove wireshark
    brew install ./wireshark.rb
    brew link --overwrite wireshark
fi

echo "Installing dependencies for compiling Wireshark libraries"
brew install wget gettext libffi

echo "Checking Wireshark sources to compile ws_dissector"
if [ ! -d "${WIRESHARK_SRC_PATH}" ]; then
    echo "You do not have source codes for Wireshark version ${ws_ver}, downloading..."
    wget https://www.wireshark.org/download/src/all-versions/wireshark-${ws_ver}.tar.bz2
    tar xf wireshark-${ws_ver}.tar.bz2
    rm wireshark-${ws_ver}.tar.bz2
fi

echo "Configuring Wireshark sources for ws_dissector compilation..."
cd ${WIRESHARK_SRC_PATH}
./configure --disable-wireshark > /dev/null 2>&1
if [[ $? != 0 ]]; then
    echo "Error when executing '${WIRESHARK_SRC_PATH}/configure --disable-wireshark'."
    echo "You need to manually fix it before continuation. Exiting with status 3"
    exit 3
fi

echo "Compiling Wireshark dissector for MobileInsight..."
cd ${MOBILEINSIGHT_PATH}/ws_dissector
if [[ -f "ws_dissector" ]]; then
    rm ws_dissector
fi
g++ ws_dissector.cpp packet-aww.cpp -o ws_dissector `pkg-config --libs --cflags glib-2.0` \
    -I"${WIRESHARK_SRC_PATH}" -L"${PREFIX}/lib" -lwireshark -lwsutil -lwiretap
strip ws_dissector

echo "Installing Wireshark dissector to ${PREFIX}/bin"
if [[ $(cp ws_dissector ${PREFIX}/bin) ]] ; then
    chmod 755 ${PREFIX}/bin/ws_dissector
else
    sudo mkdir -p ${PREFIX}/bin
    sudo cp ws_dissector ${PREFIX}/bin
    sudo chmod 755 ${PREFIX}/bin/ws_dissector
fi

echo "Installing dependencies for MobileInsight GUI..."
if [[ $(brew ls --versions ${PYTHON}) ]] ; then
    echo "Python is installed by Homebrew and ready."
else
    echo "Installing Python via Homebrew..."
    brew install python
fi

if [[ ! -f ${PREFIX}/bin/${PYTHON} ]] ; then
    echo "[WARNING] Cannot find python installed by Homebrew under /usr/local"
    echo "Your Homebrew prefix is:"
    echo $(brew config | grep -i prefix)
    echo "The MobileInsight GUI will likely not functioning if you do not use the default /usr/local"
fi

echo "Installing wxPython..."
brew install wxpython

if [[ $(which -s ${PIP}) ]] ; then
    echo "It appears that pip is not installed on your computer, installing..."
    brew install python
fi

if [[ $(${PIP} install pyserial matplotlib) ]] ; then
    echo "pyserial and matplotlib are successfully installed!"
else
    echo "Installing pyserial and matplotlib using sudo, your password may be required..."
    sudo ${PIP} install pyserial matplotlib
fi

echo "Installing mobileinsight-core..."
cd ${MOBILEINSIGHT_PATH}
if [[ $(${PYTHON} setup.py install) ]] ; then
    echo "Congratulations! mobileinsight-core is successfully installed!"
else
    echo "Installing mobileinsight-core using sudo, your password may be required..."
    sudo ${PYTHON} setup.py install
fi

echo "Installing GUI for MobileInsight..."
cd ${MOBILEINSIGHT_PATH}
if [[ $(mkdir -p ${PREFIX}/share/mobileinsight/) ]] ; then
    cp -r gui/* ${PREFIX}/share/mobileinsight/
    ln -s ${PREFIX}/share/mobileinsight/mi-gui ${PREFIX}/bin/mi-gui
else
    sudo mkdir -p ${PREFIX}/share/mobileinsight/
    sudo cp -r gui/* ${PREFIX}/share/mobileinsight/
    sudo ln -s ${PREFIX}/share/mobileinsight/mi-gui ${PREFIX}/bin/mi-gui
fi

echo "Testing the MobileInsight offline analysis example."
cd ${MOBILEINSIGHT_PATH}/examples
${PYTHON} offline-analysis-example.py
if [[ $? == 0 ]] ; then
    echo "Successfully ran the offline analysis example!"
else
    echo "Failed to run offline analysis example!"
    echo "Exiting with status 4."
    exit 4
fi

echo "Testing MobileInsight GUI (you need to be in a graphic session)..."
mi-gui
if [[ $? == 0 ]] ; then
    echo "Successfully ran MobileInsight GUI!"
    echo "The installation of mobileinsight-core is finished!"
else
    echo "There are issues running MobileInsight GUI, you need to fix them manually"
    echo "The installation of mobileinsight-core is finished!"
fi
