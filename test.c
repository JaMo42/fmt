#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "fmt/fmt.h"
#define SEP puts("========");

int main()
{
  //char *loc_name= setlocale(LC_NUMERIC, NULL);
  //printf("loc_name = \"%s\"\n", loc_name);
  locale_t us_loc = newlocale(LC_NUMERIC_MASK, "en_US.UTF-8", (locale_t)0);
  locale_t de_loc = newlocale(LC_NUMERIC_MASK, "de_DE.UTF-8", (locale_t)0);

  time_t _t = time(NULL);
  struct tm *t = localtime(&_t);

  fmt_print("{} {s}\n", "one", "two");
  puts("one two");
  SEP;
  // Alignment
  fmt_print("|{:>10}|\n", "test");
  puts("|      test|");
  SEP;
  fmt_print("|{:<10}|\n", "test");
  puts("|test      |");
  SEP;
  fmt_print("|{:_<10}|\n", "test");
  puts("|test______|");
  SEP;
  fmt_print("|{:^10}|\n", "test");
  puts("|   test   |");
  SEP;
  fmt_print("|{:^6}|\n", "zip");
  puts("| zip  |");
  SEP;
  // Truncation
  fmt_print("|{:.5}|\n", "xylophone");
  puts("|xylop|");
  SEP;
  fmt_print("|{:10.5}|\n", "xylophone");
  puts("|     xylop|");
  SEP;
  // Numbers
  fmt_print("|{f}|\n", 3.141592653589793);
  puts("|3.141592|");
  SEP;
  fmt_print("|{d:4}|\n", 42);
  puts("|  42|");
  SEP;
  fmt_print("|{f:06.2}|\n", 3.141592653589793);
  puts("|003.14|");
  SEP;
  fmt_print("|{d:04}|\n", 42);
  puts("|0042|");
  SEP;
  fmt_print("|{d:+}|\n", 42);
  puts("|+42|");
  SEP;
  fmt_print("|{d: }|\n", -23);  // error
  puts("|-23|");
  SEP;
  fmt_print("|{d: }|\n", 42);
  puts("| 42|");
  SEP;
  fmt_print("|{d:=5}|\n", -23);  // error
  puts("|-  23|");
  SEP;
  fmt_print("|{d:=+5}|\n", 23);
  puts("|+  23|");
  SEP;
  // Datetime
#ifdef FMT_SUPPORT_TIME
  fmt_print("|{t:%Y-%m-%d %H:%M}|\n", t);
  puts("|YYYY-MM-DD hh:mm|");
  SEP;
#endif
  // Parameterized formats
  fmt_print("|{:^{}}|\n", "test", 10);
  puts("|   test   |");
  SEP;
  fmt_print("|{:.{}} = {f:.{}}|\n", "Gibberish", 3, 2.7182, 3);
  puts("|Gib = 2.718|");
  SEP;
  fmt_print("|{f:{}.{}}|\n", 2.7182, 5, 2);
  puts("| 2.72|");
#ifdef FMT_SUPPORT_TIME
  SEP;
  fmt_print("|{t:{}}|\n", t, "%Y-%m-%d %H:%M");
  puts("|YYYY-MM-DD hh:mm|");
#endif
#ifdef FMT_SUPPORT_LOCALE
  // Locale
  SEP;
  uselocale(us_loc);
  fmt_print("|{nf:,}|\n", 12345.6789);
  puts("|12,345.678900|");
  SEP;
  uselocale(de_loc);
  fmt_print("|{nf:,}|\n", 12345.6789);
  puts("|12.345,678900|");
#endif

  freelocale(us_loc);
  freelocale(de_loc);

  // Format
  SEP;
  char *s = fmt_format("|{} {c} \"{f:{}^,+{}.{}}\"|\n", "msg", '=', '*', 20, 3, 3.1415926);
  fmt_print(s);
  puts("|msg = \"*******+3.141*******\"|");
  free(s);

  // Formatted length
  SEP;
  fmt_print("{d}\n", fmt_formatted_length("{f:+,.{}}", 3, 3.1415926));
  puts("6");
}

