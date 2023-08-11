CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lm

ifeq ($(RELEASE),1)
	CFLAGS += -march=native -mtune=native -O3
else
	CFLAGS += -g
endif

ifeq ($(COVERAGE),1)
	CFLAGS += -fprofile-arcs -ftest-coverage
endif

all: test

test: test.c fmt.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: run
run: test
	@./test

.PHONY: coverage
coverage:
	$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage -o test test.c $(LDFLAGS)
	./test &>/dev/null
	gcov test.c

threading_test: threading_test.c fmt.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

example: example.c fmt.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

fmt.3: fmt.3.md
	pandoc --standalone --metadata date="`date +'%B %Y'`" --to man $< -o $@

.PHONY: vg
vg: test
	valgrind --leak-check=full --track-origins=yes ./test

.PHONY: clean
clean:
	rm -f vgcore.* *.gcov *.gcda *.gcno
