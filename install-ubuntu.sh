#!/bin/bash
# Migration script for mobileInsight desktop version on Ubuntu
# It deletes old libraries installed and use standard
# installation location /usr/local/lib
# Author  : Haotian Deng
# Date    : 2017-6-24
# Version : 2.0

# Use your local library path
LD_LIBRARY_PATH=/usr/local/lib
# A copy of Wireshark sources will be put inside the MobileInsight source folder
WIRESHARK_SRC_PATH=$(pwd)/wireshark-2.0.13

MOBILEINSIGHT_PATH=$(pwd)

echo "Make sure compile environment and other dependencies are installed"
sudo apt-get -y install pkg-config wget libglib2.0-dev bison flex libpcap-dev

# Download necessary source files to compile ws_dissector
if [ ! -d "${WIRESHARK_SRC_PATH}" ]; then
    echo "Downloading source code of wireshark-2.0.13..."
	wget https://www.wireshark.org/download/src/all-versions/wireshark-2.0.13.tar.bz2
	tar -xjvf wireshark-2.0.13.tar.bz2
	rm wireshark-2.0.13.tar.bz2
fi

# read -p "We will try to config wireshark-2.0.13 and it may takes a few seconds, do you want to continue? [Yy] " -r
# if [[ ! $REPLY =~ ^[Yy]$ ]] ; then
#     echo "Not positive input"
#     echo "The installation is aborted."
#     [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1
# fi
echo "Generating config.h of wireshark-2.0.13..."
cd ${WIRESHARK_SRC_PATH}
./configure --disable-wireshark

echo "Check if proper version of wireshark dynamic library exists in system path..."

FindWiresharkLibrary=true

if readelf -d "/usr/local/lib/libwireshark.so" | grep "SONAME" | grep "libwireshark.so.7" ; then
    echo "Found libwireshark.so.7 being used"
else
    echo "Didn't find libwireshark.so.7"
    FindWiresharkLibrary=false
fi

if readelf -d "/usr/local/lib/libwiretap.so" | grep "SONAME" | grep "libwiretap.so.5" ; then
    echo "Found libwiretap.so.5 being used"
else
    echo "Didn't find libwiretap.so.5"
    FindWiresharkLibrary=false
fi

if readelf -d "/usr/local/lib/libwsutil.so" | grep "SONAME" | grep "libwsutil.so.6" ; then
    echo "Found libwsutil.so.6 being used"
else
    echo "Didn't find libwsutil.so.6"
    FindWiresharkLibrary=false
fi

if [ "$FindWiresharkLibrary" = false ] ; then
    # read -p "Could not find necessary wireshark library, we will try to compile wireshark-2.0.13 from source code and it may takes a few minutes, do you want to continue? [Yy] " -r
	# if [[ ! $REPLY =~ ^[Yy]$ ]] ; then
    #     echo "Not positive input"
    #     echo "The installation is aborted."
	#     [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1
	# fi
    # echo "Yes"
    echo "Compiling wireshark-2.0.13 from source code, it may take a few minutes..."
    make || exit 1
    echo "Installing wireshark-2.0.13"
    sudo make install
fi

echo "Compiling Wireshark dissector for mobileinsight..."
# Compile ws_dissector
cd ${MOBILEINSIGHT_PATH}/ws_dissector
if [ -e "ws_dissector" ]; then
    echo "Removing old ws_dissector"
	rm -f ws_dissector
fi
g++ ws_dissector.cpp packet-aww.cpp -o ws_dissector `pkg-config --libs --cflags glib-2.0` \
	-I"${WIRESHARK_SRC_PATH}" -L"${LD_LIBRARY_PATH}" -lwireshark -lwsutil -lwiretap
strip ws_dissector

echo "Installing dependencies for mobileinsight for GUI..."
sudo apt-get -y install python-wxgtk3.0
which pip
if [[ $? != 0 ]] ; then
    sudo apt-get -y install python-pip
fi
pip install matplotlib
pip install pyserial

echo "Installing mobileinsight-core..."
cd ${MOBILEINSIGHT_PATH}
sudo python setup.py install

# Run example
echo ""
echo "Testing the offline analysis example."
cd ${MOBILEINSIGHT_PATH}/examples
python offline-analysis-example.py
echo ""
echo "Successfully ran the offline analysis example!"
echo "The installation of mobileinsight-core is finished!"
