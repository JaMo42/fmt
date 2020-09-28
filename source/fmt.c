#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "fmt/fmt.h"

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

static FILE *fmt_fprint_stream;

static void
fmt_stream_putch(char **bufptr, char ch)
{
  (void)bufptr;
  fputc(ch, fmt_fprint_stream);
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
fmt_vfprint(FILE *fp, const char *fmt, va_list args)
{
  fmt_fprint_stream = fp;
  return fmt_format_impl(fmt_stream_putch, NULL, INT_MAX, fmt, args);
}

int
fmt_fprint(FILE *fp, const char *fmt, ...)
{
  fmt_fprint_stream = fp;
  FMT_VWRAPPER(fmt_stream_putch, NULL, INT_MAX, fmt, args);
  return r;
}

int
fmt_vfnprint(FILE *fp, int n, const char *fmt, va_list args)
{
  fmt_fprint_stream = fp;
  return fmt_format_impl(fmt_stream_putch, NULL, n, fmt, args);
}

int
fmt_fnprint(FILE *fp, int n, const char *fmt, ...)
{
  fmt_fprint_stream = fp;
  FMT_VWRAPPER(fmt_stream_putch, NULL, n, fmt, args);
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

