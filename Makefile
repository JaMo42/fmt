CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11

ifdef $(RELEASE)
	CFLAGS += -march=native -mtune=native -O3
else
	CFLAGS += -g
endif

test: test.c fmt.h
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: vg
vg: test
	valgrind --leak-check=full --track-origins=yes ./test

.PHONY: clean
clean:
	rm -f vgcore.*
