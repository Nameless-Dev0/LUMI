CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -fsanitize=address -g
LDFLAGS = -fsanitize=address

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/bin/%.o, $(SRC))
HEADER = $(wildcard include/*.h)

SERVER = build/server
CLIENT = build/client

all: $(SERVER) $(CLIENT)

$(SERVER): build/bin/server.o build/bin/server_util.o
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^

$(CLIENT): build/bin/client.o
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^

build/bin/%.o: src/%.c $(HEADER)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf build

.PHONY: all clean