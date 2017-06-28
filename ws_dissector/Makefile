# Makefile for ws_dissector PC and Android version
# Author  : Zengwen Yuan, Yuanjie Li, Jiayao Li
# Date    : 2016-11-22
# Version : 2.0

LIB_PREFIX=/usr/local/lib
WIRESHARK_SRC=$(HOME)/dev/wireshark-2.0.8
GLIB_SRC=$(HOME)/dev/glib-2.50.22
LIBPCAP_SRC=$(HOME)/dev/libpcap-1.8.0

ANDROID_PREFIX=$(HOME)/dev/android
ANDROID_LIB_PREFIX=$(HOME)/dev/android/lib
ARCH=arm-linux-androideabi
CROSS_PATH=${ANDROID_PREFIX}/bin/${ARCH}
CXX=${CROSS_PATH}-g++
STRIP=${CROSS_PATH}-strip
ANDROID_CC_FLAGS=-I"$(WIRESHARK_SRC)" \
				-I"$(LIBPCAP_SRC)" \
				-I"$(GLIB_SRC)" \
				-I"$(GLIB_SRC)/glib" \
				-I"$(GLIB_SRC)/gmodule" \
				-I"$(GLIB_SRC)/android" \
				-L"$(ANDROID_LIB_PREFIX)" -lwireshark -lglib-2.0


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
