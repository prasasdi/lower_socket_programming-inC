INCLUDE_DIR = ../include
LIB_DIR = ../lib

CC = cl
CFLAGS = /nologo /I$(INCLUDE_DIR)
LDFLAGS = /link /LIBPATH:$(LIB_DIR) wpcap.lib Packet.lib

all: sniffer.exe sniffer_conc.exe

sniffer.exe: main.c
	$(CC) main.c $(CFLAGS) $(LDFLAGS) /out:sniffer.exe

sniffer_conc.exe: main_conc.c
	$(CC) main_conc.c $(CFLAGS) $(LDFLAGS) /out:sniffer_conc.exe

run: sniffer.exe
	.\sniffer.exe

run_conc: sniffer_conc.exe
	.\sniffer_conc.exe

clean:
	del /Q *.obj *.exe
