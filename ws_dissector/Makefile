WS_LIB_PATH=/usr/local/lib
WS_SRC_PATH=$(HOME)/wireshark-2.0.1-androidcc
WS_ANDROID_LIB_PATH=$(HOME)/android/prefix/lib
GLIB_SRC_PATH=$(HOME)/android-glib/glib
GLIB_ANDROID_LIB_PATH=$(HOME)/android-glib/lib
LIBPCAP_PATH=$(HOME)/projects/platform_external_libpcap

# WS_SRC_PATH=/home/mobileinsight-dev/wireshark-2.0.2
# WS_ANDROID_LIB_PATH=/home/mobileinsight-dev/wireshark-2.0.2/lib
# GLIB_SRC_PATH=$(HOME)/Code/glib
# GLIB_ANDROID_LIB_PATH=$(HOME)/Code/glib/libs/armeabi
# LIBPCAP_PATH=$(HOME)/Code/platform_external_libpcap

CROSS_COMPILE=arm-linux-androideabi
ANDROID_PREFIX=$(HOME)/androidcc
CROSS_PATH=${ANDROID_PREFIX}/bin/${CROSS_COMPILE}
CXX=${CROSS_PATH}-g++
STRIP=${CROSS_PATH}-strip

ANDROID_CC_FLAGS=-I"$(WS_SRC_PATH)" \
				-I"$(LIBPCAP_PATH)" \
				-I"$(GLIB_SRC_PATH)" \
				-I"$(GLIB_SRC_PATH)/glib" \
				-I"$(GLIB_SRC_PATH)/gmodule" \
				-I"$(GLIB_SRC_PATH)/android" \
				-L"$(WS_ANDROID_LIB_PATH)" -lwireshark \
				-L"$(GLIB_ANDROID_LIB_PATH)" -lglib-2.0


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
