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

typedef enum
{
  FMT_RIGHT = '>',
  FMT_LEFT = '<',
  FMT_CENTER = '^',
  // For numbers, place the padding after the sign (default when padding with
  // zeros)
  FMT_AFTER_SIGN = '='
} FmtAlignment;

typedef enum
{
  FMT_NEGATIVE = '-',
  FMT_ALWAYS = '+',
  FMT_SPACE = ' '
} FmtSign;

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

typedef struct
{
  const char *digits;
  const char *prefix;
  uint8_t base;
  uint8_t prefix_len;
  uint8_t group_at;
} FmtBase;

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

typedef union
{
  int64_t int_;
  uint64_t uint;
  double float_;
  char char_;
  char *string;
  void *pointer;
  int *out;
  struct tm *time;
} FmtArg;

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
static const char *
fmt_parse_format_specifier(const char *fmt, FmtFormatSpecifier *fs, va_list args)
{
  const char *p;
  for (p = fmt; *p; ++p)
    {
      if (*p == '}')
        // Accoding to Python:
        // Brackets can not be escape inside a format specifier.
        // Instead they should be given as parameters.
        return p;
      else if (strchr("<>^=", *p))
        {
          fs->align = (FmtAlignment)*p;
        }
      else if (strchr("<>^=", p[1]))
        {
          fs->fill = p[0];
          fs->align = (FmtAlignment)p[1];
          ++p;
        }
      else if (strchr("<>^=", p[2]) && p[0] == '{' && p[1] == '}')
        {
          fs->fill = (char)va_arg(args, int);
          fs->align = (FmtAlignment)p[2];
          p += 2;
        }
      else if (strchr("-+ ", *p))
        fs->sign = (FmtSign)*p;
      else if (*p == '#')
        fs->alternate_form = true;
      else if (*p == '0' && !fs->pad_zeros)
        fs->pad_zeros = true;
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
      else if (*p == '_' || *p == ',')
        fs->grouping = *p;
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
  else
    {
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
#define FMT_GET_SIGNED(n, lm) FMT_GET_INT(n, lm, signed, intmax_t, intmax_t, ptrdiff_t)
#define FMT_GET_UNSIGNED(n, lm) FMT_GET_INT(n, lm, unsigned, size_t, uintmax_t, size_t)
#define FMT_GET_FLOAT(n, lm) \
  switch (lm) \
    { \
    default: \
    case FMT_NONE: \
    case FMT_LONG: n = va_arg(args, double); break; \
    case FMT_LONG_DOUBLE: n = (double)va_arg(args, long double); break; \
    }

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
    case 'B':
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
    case 'g':
    case 'G':
      FMT_GET_FLOAT(arg->float_, lm);
      break;
    case 'p':
      arg->pointer = va_arg(args, void*);
      break;
    case 'n':
      arg->out = va_arg(args, int*);
      break;
    case 't':
      arg->time = va_arg(args, struct tm*);
      break;
    default:
      return false;
    }
  return true;
}

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

  bool parsed_default_type = false;
  char default_type;
  FmtLengthModifier default_lm;
  bool default_unsigned_flag;
  bool default_locale_flag;

  char type;
  FmtLengthModifier lm;
  bool unsigned_flag;
  bool locale_flag;

#ifdef FMT_SUPPORT_LOCALE
  struct lconv *loc = NULL;
#endif

  FmtArg arg;

#ifdef FMT_SUPPORT_TIME
  char *strftime_fmt = NULL;
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
              if (!parsed_default_type)
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

          if (!fmt_get_arg(&arg, type, lm, unsigned_flag, args))
            continue;

          if (*p == ':')
            {
#ifdef FMT_SUPPORT_TIME
              if (type == 't')
                {
                  ++p;
                  if (strftime_fmt != NULL)
                    free(strftime_fmt);
                  if (p[0] == '{' && p[1] == '}')
                    {
                      const char *s = va_arg(args, const char *);
                      // We still need to allocate this so we don't try to free
                      // unallocated memory
                      strftime_fmt = strdup(s);
                      p += 2;
                    }
                  else
                    {
                      int len = strchr(p, '}') - p;
                      strftime_fmt = strndup(p, len);
                      p += len;
                    }
                }
              else
#endif
                p = fmt_parse_format_specifier(p + 1, &fs, args);
            }
#ifdef FMT_SUPPORT_LOCALE
          if (locale_flag)
            {
              if (loc == NULL)
                loc = localeconv();
              if (fs.grouping)
                fs.grouping = *loc->thousands_sep;
            }
#endif
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
              FMT_PRINT_INT(FMT_BIN_LOWER);
              break;
            case 'B':
              FMT_PRINT_INT(FMT_BIN_UPPER);
              break;
            case 'o':
              FMT_PRINT_INT(FMT_OCT_LOWER);
              break;
            case 'O':
              FMT_PRINT_INT(FMT_OCT_UPPER);
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
      else if (*p == '}')
        {
          if (p[1] && p[1] == '}')
            {
              putch(&buffer, '}');
              ++written;
              ++p;
            }
        }
      else
        {
          putch(&buffer, *p);
          ++written;
        }
      if (written == maxlen)
        break;
    }
#ifdef FMT_SUPPORT_TIME
  if (strftime_fmt != NULL)
    free(strftime_fmt);
#endif
  return written;
}

