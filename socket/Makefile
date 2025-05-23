CC = x86_64-w64-mingw32-gcc
CFLAGS = -O2
LDFLAGS = -lws2_32

SERVER_TARGET = socket_server.exe
SERVER_SRC = socket_server.c

CLIENT_TARGET = socket_client.exe
CLIENT_SRC = socket_client.c

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET)

run-client: $(CLIENT_TARGET)
	./$(CLIENT_TARGET)

clean:
	del /Q $(SERVER_TARGET) $(CLIENT_TARGET) 2>nul || exit 0
