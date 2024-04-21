CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=gnu11
LDFLAGS = -lm

CXX ?= g++
CXXFLAGS ?= -Wall -Wextra -D_DEFAULT_SOURCE -std=c++11

ifeq ($(RELEASE),1)
	CFLAGS += -march=native -mtune=native -O3
else
	CFLAGS += -g
endif

ifeq ($(COVERAGE),1)
	CFLAGS += -fprofile-arcs -ftest-coverage
endif

.PHONY: default
default: test

.PHONY: all
all: test threading_test example cpp_build

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
	$(CC) $(CFLAGS) -pthread -o $@ $< $(LDFLAGS)

example: example.c fmt.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

cpp_build: test.c fmt.h
	cp test.c test.cpp
	-$(CXX) $(CXXFLAGS) -o $@ test.cpp $(LDFLAGS)
	rm test.cpp

.PHONY: vg
vg: test
	valgrind --leak-check=full --track-origins=yes ./test

.PHONY: clean
clean:
	rm -f vgcore.* *.gcov *.gcda *.gcno
