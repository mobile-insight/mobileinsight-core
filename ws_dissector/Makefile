WS_SRC_PATH=$(HOME)/wireshark-1.12.3
WS_LIB_PATH=$(HOME)/wireshark-local-1.12.3/lib

all: ws_dissector

ws_dissector: ws_dissector.cpp packet-aww.cpp
	g++ $^ -o $@ `pkg-config --libs --cflags glib-2.0` -I"$(WS_SRC_PATH)" \
	-L"$(WS_LIB_PATH)" -lwireshark -lwsutil -lwiretap

clean:
	rm ws_dissector

# g++ $< -o $@ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I. -I../include/ -lpthread -L/home/santosh/proj -lpcap -L/home/santosh/proj -lwireshark
	
