# Makefile for ws_dissector PC and Android version
# Author  : Zengwen Yuan, Yuanjie Li, Jiayao Li, Yunqi Guo
# Date    : 2016-11-22
# Update  : 2020-11-20 [For wireshark 3.4.0 or later]

# Note: Please compile wireshark and glib before building ws_dissector.
# The following settings work with the cross-compiling environment provided
# by https://github.com/luckiday/wireshark-for-android. If you are using a
# customized environment, please update the <include path>(-I), <library path>(l).

LIB_PREFIX=/usr/local/lib
HOME=/home/vagrant/
WIRESHARK_SRC=/home/vagrant/wireshark-3.4.0
GLIB_SRC=/home/vagrant/glib-2.54.3
LIBPCAP_SRC=/home/vagrant/libpcap-1.9.1
ANDROID_LIB_PREFIX=/home/vagrant/lib
ANDROID_LIB_A=/home/vagrant/androidcc/lib
ANDROID_LIB_B=/usr/local/lib
CXX=/home/vagrant/android-ndk-toolchain/bin/arm-linux-androideabi-g++

STRIP=${CROSS_PATH}-strip
ANDROID_CC_FLAGS=-I"$(WIRESHARK_SRC)" \
                                -I"$(LIBPCAP_SRC)" \
                                -I"$(GLIB_SRC)" \
                                -I"$(GLIB_SRC)/glib" \
                                -I"$(GLIB_SRC)/gmodule" \
                                -I"$(GLIB_SRC)/object" \
                                -L"$(ANDROID_LIB_B)" -lwireshark -lwiretap -lwsutil \
                                -L"$(ANDROID_LIB_A)" -lglib-2.0


all: ws_dissector

.PHONY: android

android: android_ws_dissector android_pie_ws_dissector

ws_dissector: ws_dissector.cpp packet-aww.cpp
	g++ $^ -o $@ `pkg-config --libs --cflags glib-2.0` -I"$(WS_SRC_PATH)" \
	-L"$(WS_LIB_PATH)" -lwireshark -lwsutil -lwiretap

android_ws_dissector: ws_dissector.cpp packet-aww.cpp
	$(CXX) -v $^ -o $@ $(ANDROID_CC_FLAGS)

android_pie_ws_dissector: ws_dissector.cpp packet-aww.cpp
	$(CXX) -v $^ -o $@ $(ANDROID_CC_FLAGS) -pie

strip:
	$(STRIP) android_ws_dissector android_pie_ws_dissector
	strip ws_dissector

clean:
	rm ws_dissector android_ws_dissector android_pie_ws_dissector
