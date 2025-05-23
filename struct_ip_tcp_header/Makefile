CC = x86_64-w64-mingw32-gcc
CFLAGS = -O2 -Wall
LDFLAGS = -lws2_32

TARGET = main.exe
SRC = main.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(TARGET)
	@echo Running...
	@$(TARGET)

clean:
	-del /Q $(TARGET) 2>nul || exit 0
