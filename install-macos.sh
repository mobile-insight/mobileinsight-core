#!/bin/bash
# Installation script for mobileinsight-core on macOS
# It installs package under /usr/local folder
# Author  : Zengwen Yuan
# Date    : 2017-06-24
# Version : 2.0

# Wireshark version to install
WS_VER=2.0.13

# Use local library path
LD_LIBRARY_PATH=/usr/local/lib
MOBILEINSIGHT_PATH=$(pwd)
WIRESHARK_SRC_PATH=${MOBILEINSIGHT_PATH}/wireshark-${WS_VER}

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
elif [[ $wireshark == *${WS_VER}* ]]; then
    echo "Congrats! You have a Wireshark version ${WS_VER} installed, continuing..."
else
    echo "You have a Wireshark other than current version installed."
    echo "Installing Wireshark version ${WS_VER}..."
    brew install ./wireshark.rb
    brew link --overwrite wireshark
fi

echo "Installing dependencies for compiling Wireshark libraries"
brew install wget gettext libffi

# Download Wireshark source files to compile ws_dissector
if [ ! -d "${WIRESHARK_SRC_PATH}" ]; then
    echo "You do not have source codes for Wireshark version ${WS_VER}, downloading..."
    wget https://www.wireshark.org/download/src/all-versions/wireshark-${WS_VER}.tar.bz2
    tar xvf wireshark-${WS_VER}.tar.bz2
    rm wireshark-${WS_VER}.tar.bz2
fi

echo "Configuring Wireshark for compilation..."
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

echo "Installing dependencies for MobileInsight GUI..."
which -s pip
if [[ $? != 0 ]] ; then
    echo "It appears that pip is not installed on your computer, installing..."
    brew install python
    brew install wxpython
    pip install pyserial matplotlib
else
    brew install wxpython
    echo "wxPython is successfully installed!"
    if pip install pyserial matplotlib > /dev/null; then
        echo "pyserial and matplotlib are successfully installed!"
    else
        echo "Installing pyserial and matplotlib using sudo, your password may be required..."
        sudo pip install pyserial matplotlib
    fi
fi

wireshark=$(brew ls --versions wireshark)
if python setup.py install > /dev/null; then
    echo "Congratulations! mobileinsight-core is successfully installed!"
else
    echo "Installing mobileinsight-core using sudo, your password may be required..."
    sudo python setup.py install
fi

echo "Installing mobileinsight-core..."
cd ${MOBILEINSIGHT_PATH}
if python setup.py install > /dev/null; then
    echo "Congratulations! mobileinsight-core is successfully installed!"
else
    echo "Installing mobileinsight-core using sudo, your password may be required..."
    sudo python setup.py install
fi

# Run example
echo $"\n\nTesting the MobileInsight offline analysis example."
cd ${MOBILEINSIGHT_PATH}/examples
python offline-analysis-example.py
echo $"\n\nSuccessfully ran the offline analysis example!"
