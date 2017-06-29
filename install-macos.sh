#!/bin/bash
# Installation script for mobileinsight-core on macOS
# It installs package under /usr/local folder
# Author  : Zengwen Yuan
# Date    : 2017-06-24
# Version : 2.0

echo "Upgrading MobileInsight..."
./uninstall.sh

# set -e
# set -u

# Wireshark version to install
ws_ver=2.0.13

# Use local library path
LD_LIBRARY_PATH=/usr/local/lib
MOBILEINSIGHT_PATH=$(pwd)
WIRESHARK_SRC_PATH=${MOBILEINSIGHT_PATH}/wireshark-${ws_ver}

# Install up-to-date Homebrew and use default prefixes
echo "Checking if you have Homebrew installed..."
which -s brew
if [[ $? != 0 ]] ; then
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
    echo "Exiting now. MobileInsight installation failed with status 2."
    exit 2
else
    echo "All clear, updating Homebrew..."
    brew update
fi

# Check if Wireshark is installed
wireshark=$(brew ls --versions wireshark)
if [[ $? != 0 ]]; then
    echo "You don't have a Wireshark installed by Homebrew, installing..."
    # Wireshark is not installed, install dependencies
    brew install pkg-config cmake
    brew install glib gnutls libgcrypt dbus
    brew install geoip c-ares
    # Install Wireshark stable version 2.0.x using our own formulae
    brew install ./wireshark.rb
elif [[ $wireshark == *${ws_ver}* ]]; then
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

echo "Download Wireshark source to compile ws_dissector"
if [ ! -d "${WIRESHARK_SRC_PATH}" ]; then
    echo "You do not have source codes for Wireshark version ${ws_ver}, downloading..."
    wget https://www.wireshark.org/download/src/all-versions/wireshark-${ws_ver}.tar.bz2
    tar xvf wireshark-${ws_ver}.tar.bz2
    rm wireshark-${ws_ver}.tar.bz2
fi

echo "Configuring Wireshark source for ws_dissector compilation..."
cd ${WIRESHARK_SRC_PATH}
./configure --disable-wireshark

echo "Compiling Wireshark dissector for MobileInsight..."
cd ${MOBILEINSIGHT_PATH}/ws_dissector
if [ -e "ws_dissector" ]; then
    rm ws_dissector
fi
g++ ws_dissector.cpp packet-aww.cpp -o ws_dissector `pkg-config --libs --cflags glib-2.0` \
    -I"${WIRESHARK_SRC_PATH}" -L"${LD_LIBRARY_PATH}" -lwireshark -lwsutil -lwiretap
strip ws_dissector

echo "Installing Wireshark dissector to /usr/local/bin"
if cp ws_dissector /usr/local/bin/ > /dev/null; then
    chmod 755 /usr/local/bin/ws_dissector
else
    sudo cp ws_dissector /usr/local/bin/
    sudo chmod 755 /usr/local/bin/ws_dissector
fi

echo "Installing dependencies for MobileInsight GUI..."
brew ls --versions python
if [[ $? != 0 ]] ; then
    echo "Installing Python via Homebrew..."
    brew install python
else
    echo "Python is installed on your mac by Homebrew"
fi

echo "Installing wxPython..."
brew install wxpython

which -s pip
if [[ $? != 0 ]] ; then
    echo "It appears that pip is not installed on your computer, installing..."
    brew install python
fi
if python -m pip install pyserial matplotlib > /dev/null; then
    echo "pyserial and matplotlib are successfully installed!"
else
    echo "Installing pyserial and matplotlib using sudo, your password may be required..."
    sudo python -m pip install pyserial matplotlib
fi

echo "Installing mobileinsight-core..."
cd ${MOBILEINSIGHT_PATH}
if python setup.py install > /dev/null; then
    echo "Congratulations! mobileinsight-core is successfully installed!"
else
    echo "Installing mobileinsight-core using sudo, your password may be required..."
    sudo python setup.py install
fi

echo "Installing GUI for MobileInsight..."
cd ${MOBILEINSIGHT_PATH}
if mkdir -p /usr/local/share/mobileinsight/ > /dev/null; then
    cp -r gui/* /usr/local/share/mobileinsight/
    ln -s /usr/local/share/mobileinsight/mi-gui /usr/local/bin/mi-gui
else
    sudo mkdir -p /usr/local/share/mobileinsight/
    sudo cp -r gui/* /usr/local/share/mobileinsight/
    sudo ln -s /usr/local/share/mobileinsight/mi-gui /usr/local/bin/mi-gui
fi

echo -e "Testing the MobileInsight offline analysis example.\n"
cd ${MOBILEINSIGHT_PATH}/examples
python offline-analysis-example.py
if [[ $? != 0 ]] ; then
    echo -e "Successfully ran the offline analysis example!\n"
else
    echo -e "Failed to run offline analysis example!\n"
fi

echo "Testing MobileInsight GUI (you need to be in a graphic session)..."
mi-gui
if [[ $? != 0 ]] ; then
    echo -e "Successfully ran MobileInsight GUI!\n"
    echo -e "The installation of mobileinsight-core is finished!"
fi
