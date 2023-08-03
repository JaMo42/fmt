CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=c11
LDFLAGS = -lm

ifdef $(RELEASE)
	CFLAGS += -march=native -mtune=native -O3
else
	CFLAGS += -g
endif

all: test

test: test.c fmt.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

.PHONY: run
run: test
	@./test

threading_test: threading_test.c fmt.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

.PHONY: vg
vg: test
	valgrind --leak-check=full --track-origins=yes ./test

.PHONY: clean
clean:
	rm -f vgcore.*
