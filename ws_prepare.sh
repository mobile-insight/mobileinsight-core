#!/bin/bash
# Migration script for mobileInsight desktop version on macOS
# It deletes old libraries installed and use standard
# installation location /usr/local/lib
# Author  : Zengwen Yuan
# Date    : 2016-11-22
# Version : 1.0

# Use your local library path
LD_LIBRARY_PATH=/usr/local/lib
# A copy of Wireshark sources will be put inside the MobileInsight source folder
WIRESHARK_SRC_PATH=$(pwd)/wireshark-2.0.8

# Clean up libraries installed by the old version of MobileInsight
sudo rm /usr/lib/libglib-2.0.dylib
sudo rm /usr/lib/libgobject-2.0.dylib
sudo rm /usr/lib/libgio-2.0.dylib
sudo rm /usr/lib/libgmodule-2.0.dylib
sudo rm /usr/lib/libgthread-2.0.dylib
sudo rm /usr/lib/libwireshark.6.dylib
sudo rm /usr/lib/libwireshark.dylib
sudo rm /usr/lib/libwiretap.5.dylib
sudo rm /usr/lib/libwiretap.dylib
sudo rm /usr/lib/libwsutil.6.dylib
sudo rm /usr/lib/libwsutil.dylib

# Install up-to-date Homebrew and use default prefixes
# You can check related environment variables using `brew config`
# HOMEBREW_PREFIX: /usr/local
# HOMEBREW_REPOSITORY: /usr/local/Homebrew
# HOMEBREW_CELLAR: /usr/local/Cellar
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

# First let Homebrew install Wireshark 2.2.2 once, and solve dependencies
brew install wireshark
# Make sure compile environment and other dependencies are installed
brew install pkg-config cmake wget glib gettext libffi
# Remove Wireshark 2.2.2 and install the old stable version 2.0.8 using our own formulae
brew remove wireshark
brew install ./wireshark.rb
brew link --overwrite wireshark

# Download necessary source files to compile ws_dissector
# Make sure the package version is in accordance to the version that was installed by Homebrew
if [ ! -d "${WIRESHARK_SRC_PATH}" ]; then
	wget https://1.na.dl.wireshark.org/src/wireshark-2.0.8.tar.bz2
	tar -xjvf wireshark-2.0.8.tar.bz2
	rm wireshark-2.0.8.tar.bz2
fi

# Generate config.h for Wireshark 2.0.8
cd ${WIRESHARK_SRC_PATH}
./configure --disable-wireshark

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
rm -r ./libs

# Run example
echo "\\n\\n"
echo "Successfully installed the newest version of MobileInsight desktop version!"
echo "Testing the offline analysis example."
cd ./examples
python ./offline-analysis-example.py
echo "\\n\\n"
echo "Successfully ran the offline analysis example!"
