CC = gcc
# CFLAGS = -Wall -Wextra
CFLAGS = 
LDFLAGS = -lws2_32

TCP_SRC = tcp_echo.c
UDP_SRC = udp_echo.c

TCP_BIN = tcp_echo.exe
UDP_BIN = udp_echo.exe

all: run

tcp: $(TCP_SRC)
	$(CC) $(CFLAGS) $(TCP_SRC) -o $(TCP_BIN) $(LDFLAGS)

udp: $(UDP_SRC)
	$(CC) $(CFLAGS) $(UDP_SRC) -o $(UDP_BIN) $(LDFLAGS)

run: tcp
	./$(TCP_BIN)

clean:
	del /Q *.exe *.o 2>NUL || rm -f *.exe *.o

.PHONY: all tcp udp run clean
