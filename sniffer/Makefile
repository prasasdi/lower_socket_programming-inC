TARGET = sniffer.exe

INCLUDE_DIR = ../include
LIB_DIR = ../lib
SRC = main.c

CC = cl
CFLAGS = /nologo /I$(INCLUDE_DIR)
LDFLAGS = /link /LIBPATH:$(LIB_DIR) wpcap.lib Packet.lib

all:
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) /out:$(TARGET)

run: all
	$(TARGET)

clean:
	del /Q *.obj *.exe