#!/bin/bash
# Migration script for mobileInsight desktop version on Ubuntu
# It deletes old libraries installed and use standard
# installation location /usr/local/lib
# Author  : Haotian Deng
# Date    : 2016-12-28
# Version : 1.0

# Use your local library path
LD_LIBRARY_PATH=/usr/local/lib
# A copy of Wireshark sources will be put inside the MobileInsight source folder
WIRESHARK_SRC_PATH=$(pwd)/wireshark-2.0.8

# Clean up libraries installed by the old version of MobileInsight
sudo rm /usr/lib/libwireshark.so*
sudo rm /usr/lib/libwiretap.so*
sudo rm /usr/lib/libwsutil.so*

# Make sure compile environment and other dependencies are installed
sudo apt-get install pkg-config wget libglib2.0-dev bison flex libpcap-dev

# Download necessary source files to compile ws_dissector
# Make sure the package version is in accordance to the version that was
# installed previously
if [ ! -d "${WIRESHARK_SRC_PATH}" ]; then
	wget https://1.na.dl.wireshark.org/src/wireshark-2.0.8.tar.bz2
	tar -xjvf wireshark-2.0.8.tar.bz2
	rm wireshark-2.0.8.tar.bz2
fi

# Generate config.h for Wireshark 2.0.8
cd ${WIRESHARK_SRC_PATH}
./configure --disable-wireshark

echo "Compile wireshark 2.0.8 from source code, it may take a few minutes......"
make || exit 1
sudo make install

# Compile ws_dissector
cd ../ws_dissector
if [ -e "ws_dissector" ]; then
	rm ws_dissector
fi
g++ ws_dissector.cpp packet-aww.cpp -o ws_dissector `pkg-config --libs --cflags glib-2.0` \
	-I"${WIRESHARK_SRC_PATH}" -L"${LD_LIBRARY_PATH}" -lwireshark -lwsutil -lwiretap
strip ws_dissector

# Install compiled MobileInsight desktop version
cd ..
sudo python setup.py install

# Run example
echo "\\n\\n"
echo "Successfully installed the newest version of MobileInsight desktop version!"
echo "Testing the offline analysis example."
cd ./examples
python ./offline-analysis-example.py
echo "\\n\\n"
echo "Successfully ran the offline analysis example!"
