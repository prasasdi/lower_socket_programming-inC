INCLUDE_DIR = ../include
LIB_DIR = ../lib

CC = cl
CFLAGS = /nologo /I$(INCLUDE_DIR)
LDFLAGS = /link /LIBPATH:$(LIB_DIR) wpcap.lib Packet.lib ws2_32.lib

all: main.exe

main.exe: main.c
	$(CC) main.c $(CFLAGS) $(LDFLAGS) /out:main.exe

run: main.exe
	main.exe

clean:
	del /Q *.obj *.exe
