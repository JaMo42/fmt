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
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "fmt/fmt.h"

typedef union
{
  char *s;
  FILE *f;
} FmtPunningDevice;

static void
fmt_stdout_putch(char **bufptr, char ch)
{
  (void)bufptr;
  putchar(ch);
}

static void
fmt_buffer_putch(char **bufptr, char ch)
{
  *(*bufptr)++ = ch;
}

/* Old implementation, not threadsafe. Kept here for some reason

static FILE *fmt_fprint_stream;

static void
fmt_stream_putch(char **bufptr, char ch)
{
  (void)bufptr;
  fputc(ch, fmt_fprint_stream);
}
*/

static void
fmt_stream_putch(char **streamptr, char ch)
{
  FmtPunningDevice pd = { .s = *streamptr };
  fputc(ch, pd.f);
}

static int fmt_format_cap;
static int fmt_format_len;
static char *fmt_format_buf;

static void
fmt_format_putch(char **bufptr, char ch)
{
  if (fmt_format_len == fmt_format_cap)
    {
      fmt_format_cap <<= 1;
      fmt_format_buf = realloc(fmt_format_buf, fmt_format_cap);
      *bufptr = fmt_format_buf + fmt_format_len;
    }
  *(*bufptr)++ = ch;
  ++fmt_format_len;
}

static void
fmt_count_putch(char **bufptr, char ch)
{
  (void)bufptr;
  (void)ch;
}



#define FMT_VWRAPPER(...)\
  va_list args; \
  va_start(args, fmt); \
  int r = fmt_format_impl(__VA_ARGS__); \
  va_end(args);

int
fmt_vprint(const char *fmt, va_list args)
{
  return fmt_format_impl(fmt_stdout_putch, NULL, INT_MAX, fmt, args);
}

int
fmt_print(const char *fmt, ...)
{
  FMT_VWRAPPER(fmt_stdout_putch, NULL, INT_MAX, fmt, args);
  return r;
}

int
fmt_vnprint(int n, const char *fmt, va_list args)
{
  return fmt_format_impl(fmt_stdout_putch, NULL, n, fmt, args);
}

int
fmt_nprint(int n, const char *fmt, ...)
{
  FMT_VWRAPPER(fmt_stdout_putch, NULL, n, fmt, args);
  return r;
}

int
fmt_vsprint(char *s, const char *fmt, va_list args)
{
  return fmt_format_impl(fmt_buffer_putch, s, INT_MAX, fmt, args);
}

int
fmt_sprint(char *s, const char *fmt, ...)
{
  FMT_VWRAPPER(fmt_buffer_putch, s, INT_MAX, fmt, args);
  return r;
}

int
fmt_vsnprint(char *s, int n, const char *fmt, va_list args)
{
  return fmt_format_impl(fmt_buffer_putch, s, n, fmt, args);
}

int
fmt_snprint(char *s, int n, const char *fmt, ...)
{
  FMT_VWRAPPER(fmt_buffer_putch, s, n, fmt, args);
  return r;
}

int
fmt_vfprint(FILE *stream, const char *fmt, va_list args)
{
  //fmt_fprint_stream = fp;
  FmtPunningDevice pd = { .f = stream };
  return fmt_format_impl(fmt_stream_putch, pd.s, INT_MAX, fmt, args);
}

int
fmt_fprint(FILE *stream, const char *fmt, ...)
{
  //fmt_fprint_stream = fp;
  FmtPunningDevice pd = { .f = stream };
  FMT_VWRAPPER(fmt_stream_putch, pd.s, INT_MAX, fmt, args);
  return r;
}

int
fmt_vfnprint(FILE *stream, int n, const char *fmt, va_list args)
{
  //fmt_fprint_stream = fp;
  FmtPunningDevice pd = { .f = stream };
  return fmt_format_impl(fmt_stream_putch, pd.s, n, fmt, args);
}

int
fmt_fnprint(FILE *stream, int n, const char *fmt, ...)
{
  //fmt_fprint_stream = fp;
  FmtPunningDevice pd = { .f = stream };
  FMT_VWRAPPER(fmt_stream_putch, pd.s, n, fmt, args);
  return r;
}

#define FMT_FORMAT_INIT(c)\
  fmt_format_buf = malloc(c); \
  fmt_format_cap = c; \
  fmt_format_len = 0

char *
fmt_vformat(const char *fmt, va_list args)
{
  FMT_FORMAT_INIT(16);
  fmt_format_impl(fmt_format_putch, fmt_format_buf, INT_MAX, fmt, args);
  return fmt_format_buf;
}

char *
fmt_format(const char *fmt, ...)
{
  FMT_FORMAT_INIT(16);
  FMT_VWRAPPER(fmt_format_putch, fmt_format_buf, INT_MAX, fmt, args);
  (void)r;
  return fmt_format_buf;
}

char *
fmt_vnformat(int n, const char *fmt, va_list args)
{
  FMT_FORMAT_INIT(n);
  fmt_format_impl(fmt_format_putch, fmt_format_buf, n, fmt, args);
  return fmt_format_buf;
}

char *
fmt_nformat(int n, const char *fmt, ...)
{
  FMT_FORMAT_INIT(n);
  FMT_VWRAPPER(fmt_format_putch, fmt_format_buf, n, fmt, args);
  (void)r;
  return fmt_format_buf;
}

int
fmt_vformatted_length(const char *fmt, va_list args)
{
  return fmt_format_impl(fmt_count_putch, NULL, INT_MAX, fmt, args);
}

int
fmt_formatted_length(const char *fmt, ...)
{
  FMT_VWRAPPER(fmt_count_putch, NULL, INT_MAX, fmt, args);
  return r;
}

