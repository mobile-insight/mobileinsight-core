#WS_SRC_PATH=$(HOME)/wireshark-1.12.3
#WS_LIB_PATH=$(HOME)/wireshark-local-1.12.3/lib
#WS_ANDROID_LIB_PATH=$(HOME)/wireshark-local-1.12.3-armeabi/lib
WS_SRC_PATH=$(HOME)/Documents/wireshark-1.12.4
WS_LIB_PATH=/usr/local/lib/

GLIB_SRC_PATH=$(HOME)/Code/glib
GLIB_ANDROID_LIB_PATH=$(HOME)/Code/glib/libs/armeabi
LIBPCAP_PATH=$(HOME)/Code/platform_external_libpcap

ANDROID_CC_FLAGS=-I"$(WS_SRC_PATH)" -L"$(WS_ANDROID_LIB_PATH)" -lwireshark \
				-I"$(LIBPCAP_PATH)" \
				-I"$(GLIB_SRC_PATH)" -I"$(GLIB_SRC_PATH)/glib" -I"$(GLIB_SRC_PATH)/gmodule" -I"$(GLIB_SRC_PATH)/android" -L"$(GLIB_ANDROID_LIB_PATH)" -lglib-2.0

all: ws_dissector

.PHONY: android

android: android_ws_dissector android_pie_ws_dissector

ws_dissector: ws_dissector.cpp packet-aww.cpp
	g++ $^ -o $@ `pkg-config --libs --cflags glib-2.0` -I"$(WS_SRC_PATH)" \
	-L"$(WS_LIB_PATH)" -lwireshark -lwsutil -lwiretap

android_ws_dissector: ws_dissector.cpp packet-aww.cpp
	arm-linux-androideabi-g++ -v $^ -o $@ $(ANDROID_CC_FLAGS)

android_pie_ws_dissector: ws_dissector.cpp packet-aww.cpp
	arm-linux-androideabi-g++ -v $^ -o $@ $(ANDROID_CC_FLAGS) -pie

clean:
	rm ws_dissector
