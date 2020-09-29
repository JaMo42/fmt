CC = gcc
FEATURES :=
OPTFLAGS = -O3 -march=native -mtune=native
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lm

PREFIX ?= /usr/local

ifeq ($(RELEASE), 1)
	CFLAGS += $(OPTFLAGS)
endif

ifneq ($(NO_TIME), 1)
	FEATURES += -DFMT_SUPPORT_TIME
endif

ifneq ($(NO_LOCALE), 1)
	FEATURES += -DFMT_SUPPORT_LOCALE
endif

LIB = libfmt

all: $(LIB).a $(LIB).so test

build/fmt.o: source/fmt.c include/fmt/fmt.h
	$(CC) $(CFLAGS) $(FEATURES) -fPIC -o $@ -c $<

build/fmt_impl.o: source/fmt_impl.c source/fmt_impl_formatters.h
	$(CC) $(CFLAGS) $(FEATURES) -fPIC -o $@ -c $<

build/test.o: test.c
	$(CC) $(CFLAGS) $(FEATURES) -o $@ -c $<

$(LIB).so: build/fmt.o build/fmt_impl.o
	$(CC) $(LDFLAGS) -shared -o $@ $^

$(LIB).a: build/fmt.o build/fmt_impl.o
	ar rcs $@ $^

test: build/test.o $(LIB).a
	$(CC) $(LDFLAGS) -o $@ $^

example: custom_putch_example.c $(LIB).a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f build/*.o *.vgcore test example $(LIB).a $(LIB).so

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

doc:
	pandoc README.md -o README.pdf

.PHONY: clean install uninstall doc

