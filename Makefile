CC = gcc
CFLAGS =  -g -fsanitize=address -fno-omit-frame-pointer -O1 -Wall -Werror -Wextra -Iinclude
LDFLAGS = -fsanitize=address
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/bin/%.o, $(SRC))
HEADER = $(wildcard include/*.h)
SERVER = build/server

all: $(SERVER)

$(SERVER): build/bin/server.o build/bin/server_util.o
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^

build/bin/%.o: src/%.c $(HEADER)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf build

.PHONY: all clean