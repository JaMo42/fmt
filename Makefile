CC = gcc
FEATURES = -DFMT_SUPPORT_LOCALE -DFMT_SUPPORT_TIME
OPTFLAGS = -O3 -march=native -mtune=native
CFLAGS = $(FEATURES) -Wall -Wextra -Iinclude
LDFLAGS = -lm

LIB = libfmt

all: test

build/fmt.o: source/fmt.c include/fmt/fmt.h
	$(CC) $(CFLAGS) -fPIC -o $@ -c $<

build/fmt_impl.o: source/fmt_impl.c source/fmt_impl_formatters.h
	$(CC) $(CFLAGS) -fPIC -o $@ -c $<

build/test.o: test.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(LIB).so: build/fmt.o build/fmt_impl.o
	$(CC) $(LDFLAGS) -shared -o $@ $<

$(LIB).a: build/fmt.o build/fmt_impl.o
	ar rcs $(LDFLAGS) -o $@ $<

test: build/fmt_impl.o build/fmt.o build/test.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f test build/*.o *.vgcore

.PHONY: clean

