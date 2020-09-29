CC = gcc
FEATURES = -DFMT_SUPPORT_LOCALE -DFMT_SUPPORT_TIME
OPTFLAGS = -O3 -march=native -mtune=native
CFLAGS = $(FEATURES) -Wall -Wextra -Iinclude
LDFLAGS = -lm

ifndef PREFIX
	PREFIX = /usr/local
endif

ifdef RELEASE
	CFLAGS += $(OPTFLAGS)
endif

LIB = libfmt

all: $(LIB).a $(LIB).so test

build/fmt.o: source/fmt.c include/fmt/fmt.h
	$(CC) $(CFLAGS) -fPIC -o $@ -c $<

build/fmt_impl.o: source/fmt_impl.c source/fmt_impl_formatters.h
	$(CC) $(CFLAGS) -fPIC -o $@ -c $<

build/test.o: test.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(LIB).so: build/fmt.o build/fmt_impl.o
	$(CC) $(LDFLAGS) -shared -o $@ $^

$(LIB).a: build/fmt.o build/fmt_impl.o
	ar rcs $@ $^

test: build/test.o $(LIB).a
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f test build/*.o *.vgcore

install: $(LIB).a $(LIB).so
	@mkdir -p $(PREFIX)/include/fmt
	@mkdir -p $(PREFIX)/lib
	@cp -rv include/fmt $(PREFIX)/include
	@cp -v $(LIB).a $(PREFIX)/lib/$(LIB).a
	@cp -v $(LIB).so $(PREFIX)/lib/$(LIB).so

uninstall:
	@rm -rfv $(PREFIX)/include/fmt
	@rm -fv $(PREFIX)/lib/$(LIB).a
	@rm -fv $(PREFIX)/lib/$(LIB).so

.PHONY: clean install

