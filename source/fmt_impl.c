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
#include "fmt/fmt.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <locale.h>
#include <time.h>

const char *fmt_default_type = "s";

// Alignments
typedef enum
{
  FMT_RIGHT = '>',
  FMT_LEFT = '<',
  FMT_CENTER = '^',
  // For numbers, place the padding after the sign (default when padding with
  // zeros)
  FMT_AFTER_SIGN = '='
} FmtAlignment;

// Signing modes
typedef enum
{
  FMT_NEGATIVE = '-',
  FMT_ALWAYS = '+',
  FMT_SPACE = ' '
} FmtSign;

// Length modifiers
typedef enum
{
  FMT_CHAR,
  FMT_SHORT,
  FMT_NONE,
  FMT_LONG,
  FMT_LONG_LONG,
  FMT_LONG_DOUBLE,
  FMT_SIZE,
  FMT_PTRDIFF,
  FMT_INTMAX
} FmtLengthModifier;

// Structure representing a base
typedef struct
{
  const char *digits;  /**< Digits the base uses (used for upper/lowercase hex) */
  const char *prefix;  /**< The prefix the base uses (@c NULL for none) */
  uint8_t base;        /**< The base */
  uint8_t prefix_len;  /**< The length of the prefix */
  uint8_t group_at;    /**< The thousands separator gets placed every @c group_at digits */
} FmtBase;

// Format specifier information
typedef struct
{
  char fill;
  FmtAlignment align;
  FmtSign sign;
  bool alternate_form;
  bool pad_zeros;
  int width;
  char grouping;
  int precision;
  bool has_precision;
} FmtFormatSpecifier;

// Union containing all the possible argument types.
typedef union
{
  int64_t int_;
  uint64_t uint;
  double float_;
  bool bool_;
  char char_;
  char *string;
  void *pointer;
  int *out;
  struct tm *time;
} FmtArg;

/**
 * Resets a format specifier to its default values.
 */
static void
fmt_reset_format_specifier(FmtFormatSpecifier *fs)
{
  fs->fill = ' ';
  fs->align = FMT_RIGHT;
  fs->sign = FMT_NEGATIVE;
  fs->alternate_form = false;
  fs->pad_zeros = false;
  fs->width = 0;
  fs->grouping = 0;
  fs->precision = 6;
  fs->has_precision = false;
}

#include "fmt_impl_formatters.h"

#define FMT_IS_PARAM (fmt[0] == '{' && fmt[1] == '}')
/**
 * Parses a format specifier
 * @param fmt format string starting after the ':'
 * @param fs format specifier recieving the information.
 * @param args arguments passed to the function, used to fill in parameterized
 * options.
 */
static const char *
fmt_parse_format_specifier(const char *fmt, FmtFormatSpecifier *fs, va_list args)
{
  const char *p;
  for (p = fmt; *p; ++p)
    {
      if (*p == '}')
        // Accoding to Python:
        // Brackets can not be escaped inside a format specifier.
        // Instead they should be given as parameters.
        return p;
      // Alignment without fill character
      else if (strchr("<>^=", *p))
        {
          fs->align = (FmtAlignment)*p;
        }
      // Alignment with fill charater
      else if (strchr("<>^=", p[1]))
        {
          fs->fill = p[0];
          fs->align = (FmtAlignment)p[1];
          ++p;
        }
      // Alignment with parameterized fill character
      else if (strchr("<>^=", p[2]) && p[0] == '{' && p[1] == '}')
        {
          fs->fill = (char)va_arg(args, int);
          fs->align = (FmtAlignment)p[2];
          p += 2;
        }
      // Sign
      else if (strchr("-+ ", *p))
        fs->sign = (FmtSign)*p;
      // Alternate form
      else if (*p == '#')
        fs->alternate_form = true;
      // Zero padding
      else if (*p == '0' && !fs->pad_zeros)
        fs->pad_zeros = true;
      // Width
      else if (isdigit(*p))
        {
          while (isdigit(*p))
            {
              fs->width *= 10;
              fs->width += *p - '0';
              ++p;
            }
          --p;
        }
      else if (p[0] == '{' && p[1] == '}')
        {
          fs->width = va_arg(args, int);
          ++p;
        }
      // Grouping
      else if (*p == '_' || *p == ',')
        fs->grouping = *p;
      // Precision
      else if (*p == '.')
        {
          ++p;
          if (isdigit(*p))
            {
              fs->precision = 0;
              while (isdigit(*p))
                {
                  fs->precision *= 10;
                  fs->precision += *p - '0';
                  ++p;
                }
              --p;
            }
          else if (p[0] == '{' && p[1] == '}')
            {
              fs->precision = va_arg(args, int);
              ++p;
            }
          fs->has_precision = true;
        }
    }

  return p;
}
#undef FMT_IS_PARAM

/**
 * Parses a type string.
 */
static inline const char *
fmt_parse_type(const char *p, FmtLengthModifier *lm, bool *unsigned_flag, char *type, bool *locale_flag)
{
  // Length modifier
  if (*p == 'h')
    {
      *lm = FMT_SHORT;
      ++p;
      if (*p == 'h')
        {
          *lm = FMT_CHAR;
          ++p;
        }
    }
  else if (*p == 'l')
    {
      *lm = FMT_LONG;
      ++p;
      if (*p == 'l')
        {
          *lm = FMT_LONG_LONG;
          ++p;
        }
    }
  else if (*p == 'z')
    {
      *lm = FMT_SIZE;
      ++p;
    }
  else if (*p == 'k')
    {
      *lm = FMT_PTRDIFF;
      ++p;
    }
  else if (*p == 'j')
    {
      *lm = FMT_INTMAX;
      ++p;
    }
  // Unsigned flag
  if (*p == 'u')
    {
      *type = 'd';
      *unsigned_flag = true;
      ++p;
    }
#ifdef FMT_SUPPORT_LOCALE
  if (*p == 'n')
    {
      *type = 'd';
      *locale_flag = true;
      ++p;
    }
#endif
  // Type
  if (memchr("scdbBoOxXfFeE%ptn", *p, 17))
    {
      *type = *p;
      ++p;
    }
  else if (*p == '}' || *p == ':')
    {
      // End of the type string.
      // Check if we mistook a flag for the type
      if (*unsigned_flag)
        *type = 'd';
      else if (*lm == FMT_PTRDIFF)
        {
          *lm = FMT_NONE;
          *type = 't';
        }
#ifdef FMT_SUPPORT_LOCALE
      else if (*locale_flag)
        {
          *locale_flag = false;
          *type = 'n';
        }
#endif
    }
  else
    {
      // There was a character that, that is not a known type specifier.
      *type = 0;
    }
  return p;
}

#define FMT_GET_INT(n, lm, T, s, i, p) \
  switch (lm) \
    { \
    case FMT_CHAR: n = (long long T)va_arg(args, int T); break; \
    case FMT_SHORT: n = (long long T)va_arg(args, int T); break; \
    default: \
    case FMT_NONE: n = (long long T)va_arg(args, int T); break; \
    case FMT_LONG: n = (long long T)va_arg(args, long T); break; \
    case FMT_LONG_LONG: n = (long long T)va_arg(args, long long T); break; \
    case FMT_SIZE: n = (long long T)va_arg(args, s); break; \
    case FMT_INTMAX: n = (long long T)va_arg(args, i); break;\
    case FMT_PTRDIFF: n = (long long T)va_arg(args, p); break; \
    }
// Get a length modified signed integer
#define FMT_GET_SIGNED(n, lm) FMT_GET_INT(n, lm, signed, intmax_t, intmax_t, ptrdiff_t)
// Get a length modified unsigned interger
#define FMT_GET_UNSIGNED(n, lm) FMT_GET_INT(n, lm, unsigned, size_t, uintmax_t, size_t)
// Get a length modified float
#define FMT_GET_FLOAT(n, lm) \
  switch (lm) \
    { \
    default: \
    case FMT_NONE: \
    case FMT_LONG: n = va_arg(args, double); break; \
    case FMT_LONG_DOUBLE: n = (double)va_arg(args, long double); break; \
    }

/**
 * Get the next variadic argument.
 * @param type type specifier
 * @param lm length modifier
 * @param unsigned_flag integer is unsigned
 */
static inline bool
fmt_get_arg(FmtArg *arg, char type, FmtLengthModifier lm, bool unsigned_flag, va_list args)
{
  switch (type)
    {
    case 'c':
      arg->char_ = (char)va_arg(args, int);
      break;
    case 's':
      arg->string = va_arg(args, char *);
      break;
    case 'd':
    case 'b':
    case 'o':
    case 'O':
    case 'x':
    case 'X':
      if (unsigned_flag)
        {
          FMT_GET_UNSIGNED(arg->uint, lm);
        }
      else
        {
          FMT_GET_SIGNED(arg->int_, lm);
        }
      break;
    case 'f':
    case 'F':
    case 'e':
    case 'E':
    case '%':
      FMT_GET_FLOAT(arg->float_, lm);
      break;
    case 'B':
      arg->bool_ = (bool)va_arg(args, int);
      break;
    case 'p':
      arg->pointer = va_arg(args, void*);
      break;
    case 'n':
      arg->out = va_arg(args, int*);
      break;
#ifdef FMT_SUPPORT_TIME
    case 't':
      arg->time = va_arg(args, struct tm*);
      break;
#endif
    case '0':
    default:
      return false;
    }
  return true;
}

// Prints the integer argument, depending on the unsigned flag
#define FMT_PRINT_INT(b) \
  if (unsigned_flag) \
    written += fmt_print_unsigned(putch, &buffer, &fs, arg.uint, b); \
  else \
    written += fmt_print_signed(putch, &buffer, &fs, arg.int_, b); \

#ifdef FMT_SUPPORT_LOCALE
#define FMT_DOT (locale_flag ? *(loc->decimal_point) : '.')
#else
#define FMT_DOT '.'
#endif

int
fmt_format_impl(FmtPutch putch, char *buffer, int maxlen, const char *fmt, va_list args)
{
  int written = 0;
  const char *p;

  // Default type information.
  // This gets only parsed once, when needed, indicated by the
  // "parsed_default_type" flag.
  bool parsed_default_type = false;
  char default_type;
  FmtLengthModifier default_lm = FMT_NONE;
  bool default_unsigned_flag = false;
  bool default_locale_flag = false;

  // Type information
  char type;
  FmtLengthModifier lm;
  bool unsigned_flag;
  bool locale_flag;

  // Current locale.
#ifdef FMT_SUPPORT_LOCALE
  struct lconv *loc = NULL;
#endif

  // Current argument
  FmtArg arg;

  // format string for strftime
#ifdef FMT_SUPPORT_TIME
  char *strftime_fmt = NULL;
  bool strftime_allocated = false;
#endif
  FmtFormatSpecifier fs;

  for (p = fmt; *p; ++p)
    {
      fmt_reset_format_specifier(&fs);
      lm = FMT_NONE;
      unsigned_flag = false;
      locale_flag = false;
      arg.uint = 0ULL;

      if (*p == '{' && p[1])
        {
          ++p;
          if (*p == '{')
            {
              putch(&buffer, '{');
              ++written;
              continue;
            }

          if (*p == '}' || *p == ':')
            {
              // No type given, use default
              if (!parsed_default_type)
                // First time, parse default type string.
                fmt_parse_type(fmt_default_type, &default_lm,
                               &default_unsigned_flag, &default_type,
                               &default_locale_flag);
              type = default_type;
              unsigned_flag = default_unsigned_flag;
              locale_flag = default_locale_flag;
              lm = default_lm;
            }
          else
            p = fmt_parse_type(p, &lm, &unsigned_flag, &type, &locale_flag);

          // If the type specifier was invalid, skip to the end of the format
          // specifier.
          if (!fmt_get_arg(&arg, type, lm, unsigned_flag, args))
            {
              while (*p != '}')
                {
                  ++p;
                  if (*p == '{')
                    p += 2;
                }
            }

          if (*p == ':')
            {
#ifdef FMT_SUPPORT_TIME
              if (type == 't')
                {
                  // strftime format string
                  ++p;
                  if (strftime_allocated)
                    free(strftime_fmt);
                  if (p[0] == '{' && p[1] == '}')
                    {
                      strftime_fmt = va_arg(args, char *);
                      strftime_allocated = false;
                      p += 2;
                    }
                  else
                    {
                      int len = strchr(p, '}') - p;
                      strftime_fmt = strndup(p, len);
                      strftime_allocated = true;
                      p += len;
                    }
                }
              else
#endif
                p = fmt_parse_format_specifier(p + 1, &fs, args);
            }
#ifdef FMT_SUPPORT_LOCALE
          // Get grouping character.
          if (locale_flag)
            {
              if (loc == NULL)
                // First time, get the current locale.
                loc = localeconv();
              if (fs.grouping)
                fs.grouping = *loc->thousands_sep;
            }
#endif
          // Print the argument
          switch (type)
            {
            case 's':
              if (arg.string == NULL)
                arg.string = "(null)";
              written += fmt_print_string(putch, &buffer, &fs, arg.string,
                                          strlen(arg.string));
              break;
            case 'c':
              written += fmt_print_string(putch, &buffer, &fs, &arg.char_, 1);
              break;
            case 'd':
              FMT_PRINT_INT(FMT_DECIMAL);
              break;
            case 'b':
              FMT_PRINT_INT(FMT_BIN);
              break;
            case 'o':
              FMT_PRINT_INT(FMT_OCT_FULL);
              break;
            case 'O':
              FMT_PRINT_INT(FMT_OCT_ZERO);
              break;
            case 'x':
              FMT_PRINT_INT(FMT_HEX_LOWER);
              break;
            case 'X':
              FMT_PRINT_INT(FMT_HEX_UPPER);
              break;
            case 'f':
            case 'F':
              written += fmt_print_float(putch, &buffer, &fs, arg.float_,
                                         *p == 'F', FMT_DOT, 0);
              break;
            case 'e':
            case 'E':
              written += fmt_print_scientific(putch, &buffer, &fs, arg.float_,
                                              *p == 'E', FMT_DOT);
              break;
            case '%':
              written += fmt_print_float(putch, &buffer, &fs, arg.float_,
                                         false, FMT_DOT, '%');
              break;
            case 'B':
              if (fs.alternate_form)
                written += fmt_print_string(putch, &buffer, &fs,
                                            arg.bool_ ? "True" : "False",
                                            arg.bool_ ? 4 : 5);
              else
                written += fmt_print_string(putch, &buffer, &fs,
                                            arg.bool_ ? "true" : "false",
                                            arg.bool_ ? 4 : 5);
              break;
            case 'p':
              fs.grouping = 0;
              fs.alternate_form = true;
              written += fmt_print_number(putch, &buffer, &fs, (size_t)arg.pointer,
                                          0, FMT_HEX_LOWER);
              break;
#ifdef FMT_SUPPORT_TIME
            case 't':
              written += fmt_print_time(putch, &buffer, &fs, strftime_fmt,
                                        arg.time);
              break;
#endif
            case 'n':
              arg.out = va_arg(args, int*);
              *arg.out = written;
              break;
            }
        }
      // Print ascaped closing bracket.
      else if (*p == '}')
        {
          if (p[1] && p[1] == '}')
            {
              putch(&buffer, '}');
              ++written;
              ++p;
            }
        }
      // Print normal character
      else
        {
          putch(&buffer, *p);
          ++written;
        }
      if (written == maxlen)
        break;
    }
#ifdef FMT_SUPPORT_TIME
  if (strftime_allocated)
    free(strftime_fmt);
#endif
  return written;
}

