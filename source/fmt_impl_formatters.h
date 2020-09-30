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
#ifndef FMT_IMPL_FORMATTERS_H
#define FMT_IMPL_FORMATTERS_H

// Bases (digits, prefix, base, prefix_len, group_at)
#define FMT_DECIMAL   (FmtBase){"0123456789", NULL, 10, 0, 3}
#define FMT_BIN       (FmtBase){"01", "0b", 2, 2, 4}
#define FMT_HEX_LOWER (FmtBase){"0123456789abcdef", "0x", 16, 2, 4}
#define FMT_HEX_UPPER (FmtBase){"0123456789ABCDEF", "0X", 16, 2, 4}
#define FMT_OCT_FULL  (FmtBase){"01234567", "0o", 8, 2, 4}
#define FMT_OCT_ZERO  (FmtBase){"01234567", "0", 8, 1, 4}

#define FMT_GET_PADDING(l, r, total) \
  { \
    if (fs->align == FMT_RIGHT || fs->align == FMT_AFTER_SIGN) \
      l = (total); \
    else if (fs->align == FMT_LEFT) \
      r = (total); \
    else \
      { \
        l = (int)__builtin_floorf((float)(total) / 2.0f); \
        r = (int)__builtin_ceilf((float)(total) / 2.0f); \
      } \
  }

#define FMT_PAD(n, c) \
  for (int fmt_pad__i = 0; fmt_pad__i < n; ++fmt_pad__i) \
    putch(buffer, c)

// Print characters without formatting
static inline int
fmt_print_chars(FmtPutch putch, char **buffer, const char *chars, int len)
{
  for (int i = 0; i < len; ++i)
    putch(buffer, chars[i]);
  return len;
}

// Get the length of a number in a given base.
static inline int
fmt_intlen(uint64_t n, int base)
{
  int digits = 0;
  do
    {
      n /= base;
      ++digits;
    }
  while (n);
  return digits;
}

// Print digits of a number
static inline int
fmt_print_digits(FmtPutch putch, char **buffer, uint64_t number, int len,
                 int base, const char *digits)
{
  uint64_t nn = (uint64_t)pow(base, len);
  while (nn / base)
    {
      nn /= base;
      putch(buffer, digits[number / nn]);
      number %= nn;
    }
  return len;
}

// Print digits of a number, with thousands separator
static inline int
fmt_print_digits_grouped(FmtPutch putch, char **buffer, uint64_t number,
                         int len, int base, const char *digits, char grouping,
                         int group_at)
{
  uint64_t nn = (uint64_t)pow(base, len);
  int i = group_at - (len % group_at);
  while (nn / base)
    {
      nn /= base;
      putch(buffer, digits[number / nn]);
      number %= nn;
      if (++i % group_at == 0 && number)
        putch(buffer, grouping);
    }
  return len + (len / group_at);
}

#define FMT_FORMATTER FmtPutch putch, char **buffer, FmtFormatSpecifier *fs

/**
 * @param str string to print
 * @param len length of @c str
 */
static inline int
fmt_print_string(FMT_FORMATTER, const char *str, int len)
{
  if (!fs->has_precision)
    fs->precision = UINT_MAX >> 1;

  if (fs->precision < len)
    len = fs->precision;

  int lpad = 0;
  int rpad = 0;
  if (fs->width > len)
    FMT_GET_PADDING(lpad, rpad, fs->width - len);

  FMT_PAD(lpad, fs->fill);
  fmt_print_chars(putch, buffer, str, len);
  FMT_PAD(rpad, fs->fill);
  return lpad + len + rpad;
}

/**
 * @param number the number to print.
 * @param sign sign of the number ('\0' for none)
 * @param base base of the number
 */
static inline int
fmt_print_number(FMT_FORMATTER, uint64_t number, char sign, FmtBase base)
{
  int numlen = fmt_intlen(number, base.base);
  int to_print = ((sign ? 1 : 0) + (base.prefix ? base.prefix_len : 0) + numlen
                  + (fs->grouping ? (numlen / base.group_at) : 0));

  int lpad = 0;
  int rpad = 0;
  if (to_print < fs->width)
    FMT_GET_PADDING(lpad, rpad, fs->width - to_print);

  // Only do zero-padding. when aligning to the right, this follows the
  // behaviour of printf.
  fs->pad_zeros &= fs->align == FMT_RIGHT;

  char padchar = fs->pad_zeros ? '0' : fs->fill;
  int written = 0;
  const bool pad_after = fs->pad_zeros || fs->align == FMT_AFTER_SIGN;

  if (!pad_after)
    FMT_PAD(lpad, padchar);
  if (sign)
    {
      putch(buffer, sign);
      ++written;
    }
  if (base.prefix && fs->alternate_form)
    written += fmt_print_chars(putch, buffer, base.prefix, base.prefix_len);
  if (pad_after)
    FMT_PAD(lpad, padchar);

  if (fs->grouping)
    written += fmt_print_digits_grouped(putch, buffer, number, numlen, base.base,
                                        base.digits, fs->grouping, base.group_at);
  else
    written += fmt_print_digits(putch, buffer, number, numlen, base.base, base.digits);

  FMT_PAD(rpad, padchar);
  return lpad + written + rpad;
}

/**
 * @ref fmt_print_number
 */
static inline int
fmt_print_unsigned(FMT_FORMATTER, uint64_t number, FmtBase base)
{
  char sign = 0;
  if (fs->sign == FMT_ALWAYS)
    sign = '+';
  else if (fs->sign == FMT_SPACE)
    sign = ' ';

  return fmt_print_number(putch, buffer, fs, number, sign, base);
}

/**
 * @ref fmt_print_number
 */
static inline int
fmt_print_signed(FMT_FORMATTER, int64_t number, FmtBase base)
{
  char sign = 0;
  if (number < 0)
    {
      sign = '-';
      number = 0 - number;
    }
  else if (fs->sign == FMT_ALWAYS)
    sign= '+';
  else if (fs->sign == FMT_SPACE)
    sign = ' ';

  return fmt_print_number(putch, buffer, fs, number, sign, base);
}

/**
 * Print digits of a floating point number.
 * @param number number to print
 * @param precision length of the fraction
 * @param dot symbol to use for the decimal point
 * @param grouping symbol to use as thousands separator ('\0' for no grouping)
 * @param alt print in alternate format
 */
static inline int
fmt_print_float_digits(FmtPutch putch, char **buffer, double number, int precision, char dot, char grouping, bool alt)
{
  uint64_t intpart = (uint64_t)number;
  int intpart_len = fmt_intlen(number, 10);

  int written = intpart_len;

  if (grouping)
    {
      fmt_print_digits_grouped(putch, buffer, intpart, intpart_len, 10,
                               "0123456789", grouping, 3);
      written += intpart_len / 3;
    }
  else
    fmt_print_digits(putch, buffer, intpart, intpart_len, 10, "0123456789");

  if (precision > 0 || alt)
    {
      putch(buffer, dot);
      ++written;
    }

  if (precision > 0)
    {
      double fraction = number - intpart;
      for (int i = 0; i < precision; ++i)
        fraction *= 10;
      fmt_print_digits(putch, buffer, (uint64_t)fraction, precision, 10,
                       "0123456789");
      written += precision;
    }
  return written;
}

/**
 * @param number number to print
 * @param upper print "inf" and "nan" in uppercase
 * @param dot symbol to use for the decimal dot
 * @param pos The symbol to display after the number ('\0' for none). This is
 * used for the '%' specifier.
 */
static inline int
fmt_print_float(FMT_FORMATTER, double number, bool upper, char dot, char post)
{
  if (isnan(number))
    return fmt_print_string(putch, buffer, fs, upper ? "NAN" : "nan", 3);
  if (isinf(number))
    return fmt_print_string(putch, buffer, fs, upper ? "INF" : "inf", 3);

  char sign = 0;
  if (number < 0)
    {
      sign = '-';
      number = 0 - number;
    }
  else if (fs->sign == FMT_ALWAYS)
    sign = '+';
  else if (fs->sign == FMT_SPACE)
    sign = ' ';

  int intpart_len = fmt_intlen((uint64_t)number, 10);

  int to_print = (intpart_len + 1 + fs->precision + (sign != 0) + (post != 0)
                  + (fs->grouping ? (intpart_len / 3) : 0));
  int lpad = 0;
  int rpad = 0;
  if (to_print < fs->width)
    FMT_GET_PADDING(lpad, rpad, fs->width - to_print);

  int written = 0;

  fs->pad_zeros &= fs->align == FMT_RIGHT;
  char padchar = fs->pad_zeros ? '0' : fs->fill;
  bool after_sign = fs->pad_zeros || fs->align == FMT_AFTER_SIGN;
  if (!after_sign)
    FMT_PAD(lpad, padchar);
  if (sign)
    {
      putch(buffer, sign);
      ++written;
    }
  if (after_sign)
    FMT_PAD(lpad, padchar);

  fmt_print_float_digits(putch, buffer, number, fs->precision, dot, fs->grouping, fs->alternate_form);

  FMT_PAD(rpad, fs->fill);

  return lpad + to_print + rpad;
}

/**
 * @param numer number to print
 * @param upper print "inf" and "nan" in uppercase
 * @param dot symbol to use for the decimal dot.
 */
static inline int
fmt_print_scientific(FMT_FORMATTER, double number, bool upper, char dot)
{
  if (isnan(number))
    return fmt_print_string(putch, buffer, fs, upper ? "NAN" : "nan", 3);
  if (isinf(number))
    return fmt_print_string(putch, buffer, fs, upper ? "INF" : "inf", 3);

  char sign = 0;
  if (number < 0)
    {
      sign = '-';
      number = 0 - number;
    }
  else if (fs->sign == FMT_ALWAYS)
    sign = '+';
  else if (fs->sign == FMT_SPACE)
    sign = ' ';

  double fraction = number;
  int exp = 0;
  char exp_sign = '+';

  // multiply/divide the number until it's "1.xxx", in/de-creasing the
  // exponent
  if (number < 1.0)
    {
      exp_sign = '-';
      while (fraction < 1.0)
        {
          fraction *= 10;
          ++exp;
        }
    }
  else
    {
      while (fraction >= 10.f)
        {
          fraction /= 10;
          ++exp;
        }
    }
  int exp_len = exp < 10 ? 1 : fmt_intlen(exp, 10);

  int to_print = 4 + fs->precision + exp_len + (exp_len==1);

  int lpad = 0;
  int rpad = 0;
  if (to_print < fs->width)
    FMT_GET_PADDING(lpad, rpad, fs->width - to_print);

  fs->pad_zeros &= fs->align == FMT_RIGHT;
  char padchar = fs->pad_zeros ? '0' : fs->fill;
  bool after_sign = fs->pad_zeros || fs->align == FMT_AFTER_SIGN;

  int written = 0;

  if (!after_sign)
    FMT_PAD(lpad, padchar);
  if (sign)
    {
      putch(buffer, sign);
      ++written;
    }
  if (after_sign)
    FMT_PAD(lpad, padchar);

  fmt_print_float_digits(putch, buffer, fraction, fs->precision, dot, 0, false);
  putch(buffer, upper ? 'E' : 'e');
  putch(buffer, exp_sign);
  if (exp_len == 1)
    {
      putch(buffer, '0');
      putch(buffer, '0' + exp);
    }
  else
    fmt_print_digits(putch, buffer, exp, exp_len, 10, "0123456789");

  FMT_PAD(rpad, fs->fill);

  return lpad + to_print + rpad;
}

#ifdef FMT_SUPPORT_TIME
/**
 * Print a time object.
 *
 * For simplicity and speed @c strftime prints into a static buffer,
 * this limits the maximum length of the formatted string to 127
 * (excluding the null-terminator).
 *
 * @param fmt format string for the @c strftime function.
 * @param tm the time object to print.
 */
static inline int
fmt_print_time(FMT_FORMATTER, const char *fmt, struct tm *tm)
{
  static char buf[128];
  int len = strftime(buf, 128, fmt, tm);
  return fmt_print_string(putch, buffer, fs, buf, len);
}
#endif

#endif /* !FMT_IMPL_FORMATTERS_H */
