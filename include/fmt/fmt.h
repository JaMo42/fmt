#ifndef FMT_H
#define FMT_H
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

typedef void(*FmtPutch)(char **, char);

/**
 * Replacement fields:
 *   {[length_modifier][u][n][type][:format_specifier]}
 *
 *   "length_modifier" - Same as "printf"
 *   'u' - Integer argument is unsigned
 *   'n' - For integers and floats, get thousands separator and decimal point
 *         from current locale.
 *
 * Types:
 *   's' - String
 *   'c' - Ineger, gets converted to unicode character before printing
 *   'd' - Integer, decimal
 *   'b' - Integer, binary
 *   'o' - Integer, octal
 *   'x' - Integer, lower-case hex
 *   'X' - Integer, upper-case hex
 *   'n' - Integer, locale aware
 *   'f' - Float, lower-case
 *   'F' - Float, upper-case
 *   'e' - Float, lower-case exponent notation
 *   'E' - Float, lower-case exponent notation
 *   'g' - Float, lower-case general format
 *   'G' - Float, upper-case general format
 *   '%' - Float, multiplies number by 100, displays in 'f' format and adds a
 *         '%'
 *   't' - "struct tm", format specifier is replaced by a "strftime" format
 *         string
 *
 * Format specifier:
 *   [[fill]align][sign][#][0][width][grouping][.precision]
 *
 *   "fill", "width" and "precision" may be given as parameters ("{}")
 *
 * If no type but the unsigned flag is given, the type defaults to 'd',
 * otherwise to 's'.
 */
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
