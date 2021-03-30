CFLAGS ?= -g -pedantic -pedantic -Wall -Werror -Wextra \
		  -Wstrict-prototypes -Wold-style-definition -Iinclude -std=c99 \
		  -D_GNU_SOURCE -O0
CC ?= gcc

BIN = clipfs
ALL_DEP_FILES = $(patsubst %.c,%.d,$(wildcard *.c))
OBJ = $(patsubst %.c,%.o,$(wildcard *.c))

CFLAGS += $(shell pkg-config x11 fuse3 --cflags)
LDFLAGS += $(shell pkg-config x11 fuse3 --libs)

.PHONY: all clean

all: $(BIN)

-include $(ALL_DEP_FILES)

%.o: %.c
	$(CC) -MMD -c $(CFLAGS) $< -o $@

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -rvf $(BIN) $(OBJ) $(ALL_DEP_FILES)
