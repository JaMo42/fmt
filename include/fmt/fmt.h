/*
 * Copyright (c) 2020 Jakob Mohrbacher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef FMT_H
#define FMT_H
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

typedef void(*FmtPutch)(char **, char);

// The type to use when no type is given and it is no implied (defaults to "s").
extern const char * fmt_default_type;

// The initial size of the buffer allocated by the fmt_format functions.
extern int fmt_format_initial_size;

int
fmt_format_impl(FmtPutch putch, char *buffer, int maxlen, const char *fmt, va_list args);

// Stdout print

int
fmt_vprint(const char *fmt, va_list args);

int
fmt_print(const char *fmt, ...);

int
fmt_vnprint(int n, const char *fmt, va_list args);

int
fmt_nprint(int n, const char *fmt, ...);

// String print

int
fmt_vsprint(char *s, const char *fmt, va_list args);

int
fmt_sprint(char *s, const char *fmt, ...);

int
fmt_vsnprint(char *s, int n, const char *fmt, va_list args);

int
fmt_snprint(char *s, int n, const char *fmt, ...);

// File print

int
fmt_vfprint(FILE *fp, const char *fmt, va_list arg);

int
fmt_fprint(FILE *fp, const char *fmt, ...);

int
fmt_vfnprint(FILE *fp, int n, const char *fmt, va_list arg);

int
fmt_fnprint(FILE *fp, int n, const char *fmt, ...);

// Format

char *
fmt_vformat(const char *fmt, va_list args);

char *
fmt_format(const char *fmt, ...);

char *
fmt_vnformat(int n, const char *fmt, va_list args);

char *
fmt_nformat(int n, const char *fmt, ...);

// Formatted length

int
fmt_vformatted_length(const char *fmt, va_list args);

int
fmt_formatted_length(const char *fmt, ...);

#endif /* !FMT_H */
