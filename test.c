#include "fmt/fmt.h"
#include "smallunit.h"
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>

SU_SOURCE;

#define FMT_TEST_BUFSIZE 1024*2

#define streq(a, b) !strcmp(a, b)
#define check(s)\
  if (!streq(buf, s)) printf("    buf = \"%s\"\n", buf);\
  su_assert(streq(buf, s)); \
  memset(buf, 0, FMT_TEST_BUFSIZE)
#define print(...) fmt_sprint(buf, __VA_ARGS__)

const char *lorem_ipsum =\
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
  "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis "
  "nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. "
  "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore "
  "eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, "
  "sunt in culpa qui officia deserunt mollit anim id est laborum.";

const char *lorem_ipsum_b64 =\
  "TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQsIGNvbnNlY3RldHVyIGFkaXBpc2NpbmcgZWxpdCwg"
  "c2VkIGRvIGVpdXNtb2QgdGVtcG9yIGluY2lkaWR1bnQgdXQgbGFib3JlIGV0IGRvbG9yZSBtYWdu"
  "YSBhbGlxdWEuIFV0IGVuaW0gYWQgbWluaW0gdmVuaWFtLCBxdWlzIG5vc3RydWQgZXhlcmNpdGF0"
  "aW9uIHVsbGFtY28gbGFib3JpcyBuaXNpIHV0IGFsaXF1aXAgZXggZWEgY29tbW9kbyBjb25zZXF1"
  "YXQuIER1aXMgYXV0ZSBpcnVyZSBkb2xvciBpbiByZXByZWhlbmRlcml0IGluIHZvbHVwdGF0ZSB2"
  "ZWxpdCBlc3NlIGNpbGx1bSBkb2xvcmUgZXUgZnVnaWF0IG51bGxhIHBhcmlhdHVyLiBFeGNlcHRl"
  "dXIgc2ludCBvY2NhZWNhdCBjdXBpZGF0YXQgbm9uIHByb2lkZW50LCBzdW50IGluIGN1bHBhIHF1"
  "aSBvZmZpY2lhIGRlc2VydW50IG1vbGxpdCBhbmltIGlkIGVzdCBsYWJvcnVtLg==";

const char *lorem_ipsum_b64_nl =\
  "TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQsIGNvbnNlY3RldHVyIGFkaXBpc2NpbmcgZWxpdCwg\n"
  "c2VkIGRvIGVpdXNtb2QgdGVtcG9yIGluY2lkaWR1bnQgdXQgbGFib3JlIGV0IGRvbG9yZSBtYWdu\n"
  "YSBhbGlxdWEuIFV0IGVuaW0gYWQgbWluaW0gdmVuaWFtLCBxdWlzIG5vc3RydWQgZXhlcmNpdGF0\n"
  "aW9uIHVsbGFtY28gbGFib3JpcyBuaXNpIHV0IGFsaXF1aXAgZXggZWEgY29tbW9kbyBjb25zZXF1\n"
  "YXQuIER1aXMgYXV0ZSBpcnVyZSBkb2xvciBpbiByZXByZWhlbmRlcml0IGluIHZvbHVwdGF0ZSB2\n"
  "ZWxpdCBlc3NlIGNpbGx1bSBkb2xvcmUgZXUgZnVnaWF0IG51bGxhIHBhcmlhdHVyLiBFeGNlcHRl\n"
  "dXIgc2ludCBvY2NhZWNhdCBjdXBpZGF0YXQgbm9uIHByb2lkZW50LCBzdW50IGluIGN1bHBhIHF1\n"
  "aSBvZmZpY2lhIGRlc2VydW50IG1vbGxpdCBhbmltIGlkIGVzdCBsYWJvcnVtLg==";

su_module(fmt_tests,{
  locale_t us_loc = newlocale(LC_NUMERIC_MASK, "en_US.UTF-8", (locale_t)0);
  locale_t de_loc = newlocale(LC_NUMERIC_MASK, "de_DE.UTF-8", (locale_t)0);
  static char buf[FMT_TEST_BUFSIZE];

  su_test(alignment, {
    // Right
    print("{:>10}", "test");
    check("      test");
    // Right
    print("{:<10}", "test");
    check("test      ");
    // Center
    print("{:^10}", "test");
    check("   test   ");
    print("{:^7}", "test");
    check(" test  ");
    // Fill character
    print("{:_<10}", "test");
    check("test______");
    print("{:{}>10}", "test", '*');
    check("******test");
  })

  su_test(character, {
    print("{c}{c}{c}", 'a', 'b', 'c');
    check("abc");
  })

  su_test(string, {
    // Implied type, simple
    print("{} {s}", "one", "two");
    check("one two");
    // Truncated
    print("{:.5}", "xylophone");
    check("xylop");
    print("{:10.5}", "xylophone");
    check("     xylop");
  })

  su_test(integer_decimal, {
    print("{d} {d}", 23, -23);
    check("23 -23");
    // Pad zeros
    print("{d:04}", 23);
    check("0023");
    // Signing
    print("{d:-} {d:-}", 23, -23);
    check("23 -23");
    print("{d:+} {d:+}", 23, -23);
    check("+23 -23");
    print("{d: } {d: }", 23, -23);
    check(" 23 -23");
    // Signing with zero padding
    print("{d:+06}", 23);
    check("+00023");
  })

  su_test(integer_bases, {
    int d = 123456789;
    // Hex
    print("{x} {X}", d, d);
    check("75bcd15 75BCD15");
    print("{x:#} {X:#}", d, d);
    check("0x75bcd15 0X75BCD15");
    // Binary
    print("{b}", d);
    check("111010110111100110100010101");
    print("{b:#}", d);
    check("0b111010110111100110100010101");
    // Octal
    print("{o} {O}", d, d);
    check("726746425 726746425");
    print("{o:#} {O:#}", d, d);
    check("0o726746425 0726746425");
    // Zeropad with prefix
    print("{x:#012} {X:#012}", d, d);
    check("0x00075bcd15 0X00075BCD15");
    // Sign with prefix
    print("{x:+#012}", d);
    check("+0x0075bcd15");
  })

  su_test(float, {
    float pi = 3.1415926f;
    print("{f} {F}", 1.234f, 1.234f);
    check("1.233999 1.233999");
    print("{f} {F}", 1.2f, 1.2f);
    check("1.200000 1.200000");
    // Precision
    print("{f:.3}", pi);
    check("3.141");
    print("{f:.0}", pi);
    check("3");
    print("{f:#.0}", pi);
    check("3.");
    // "NaN" and "Inf"
    print("{f} {F}", NAN, NAN);
    check("nan NAN");
    print("{f} {F}", INFINITY, INFINITY);
    check("inf INF");
  })

  su_test(float_exp, {
    float f = 123456.789;
    print("{e} {E}", f, f);
    check("1.234567e+05 1.234567E+05");
    print("{e:.0}", f);
    check("1e+05");
    print("{e:#.0}", f);
    check("1.e+05");
  })

  su_test(float_percent, {
    float p = 0.456789;
    print("{%}", p);
    check("45.678898%");
    print("{%:.3}", p);
    check("45.678%");
    print("{%:.0}", p);
    check("45%");
    print("{%:#.0}", p);
    check("45.%");
  })

  su_test(boolean, {
    print("{B} {B}", true, false);
    check("true false");
    print("{B:#} {B:#}", true, false);
    check("True False");
  })

  su_test(pointer, {
    void *p = (void*)0x123456ULL;
    print("{p}", p);
    check("0x123456");
  })

  su_test(time, {
    time_t _t = time(NULL);
    struct tm *t = localtime(&_t);
    char buf2[128];
    strftime(buf2, 127, "%Y-%m-%d %H:%M", t);
    print("{t:%Y-%m-%d %H:%M}", t);
    check(buf2);
    print("{t:{}}", t, "%Y-%m-%d %H:%M");
    check(buf2);
  })

  su_test(base64, {
    print("{D:.{}}", lorem_ipsum, strlen(lorem_ipsum));
    check(lorem_ipsum_b64);
    print("{D:#.{}}", lorem_ipsum, strlen(lorem_ipsum));
    check(lorem_ipsum_b64_nl);
  })

  su_test(written, {
    int n;
    print("123456{n}", &n);
    su_assert_eq(n, 6);
  })

  su_test(parameterized_format, {
    print("{:^{}}", "test", 10);
    check("   test   ");
    print("{:.{}} = {f:.{}}", "Gibberish", 3, 2.7182, 3);
    check("Gib = 2.718");
    print("{f:{}.{}}", 2.7182, 6, 2);
    check("  2.71");
  })

  su_test(grouping, {
    // Integer
    print("{d:,} {d:_}", 123456789, 123456789);
    check("123,456,789 123_456_789");
    // Integer bases
    print("{x:_}", 0x12345678);
    check("1234_5678");
    print("{o:_}", 342391);
    check("123_4567");
    print("{b:_}", 165);
    check("1010_0101");
    // Float
    print("{f:,} {f:_}", 1234.5678, 1234.5678);
    check("1,234.567800 1_234.567800");
  })

  su_test(unsigned_flag, {
    print("{d} {ud}", 23, 23);
    check("23 23");
    print("{d} {ud}", -23, -23);
    check("-23 4294967273");
  })

  su_test(locale, {
    // Integer
    uselocale(us_loc);
    print("{nd} {nd:,} {nd:_}", 12345678, 12345678, 12345678);
    check("12345678 12,345,678 12,345,678");
    uselocale(de_loc);
    print("{nd} {nd:,} {nd:_}", 12345678, 12345678, 12345678);
    check("12345678 12.345.678 12.345.678");
    // Float
    uselocale(us_loc);
    print("{nf} {nf:,} {nf:_}", 1234.5678, 1234.5678, 1234.5678);
    check("1234.567800 1,234.567800 1,234.567800");
    uselocale(de_loc);
    print("{nf} {nf:,} {nf:_}", 1234.5678, 1234.5678, 1234.5678);
    check("1234,567800 1.234,567800 1.234,567800");
  })

  su_test(implied_types, {
    // No need to test {} here, just locale/unsigned flags
    print("{u} {un}", 23, 23);
    check("23 23");
  })

  su_test(default_type, {
    print("{}", "test");
    check("test");
    fmt_default_type = "f";
    print("{}", 3.141);
    check("3.141000");
    fmt_default_type = "s";
  })

  su_test(empty_default_type, {
    fmt_default_type = "";
    print ("{} {d} {d}", 1, 2, 3);
    check (" 1 2");
    fmt_default_type = "s";
  })

  su_test(invalid_type, {
    print("{y}{d}", 1, 2);
    check("1");
    print("{y:{}<{}.{}}{d} {d}", 1, '_', 10, 3, 2);
    check("1 95");
  })

  su_test(misc, {
    print("} }}");
    check(" }");
  })

  su_test(format, {
    char *s = fmt_format("{} {c} \"{f:{}^,+{}.{}}\"", "msg", '=', 3.1415926, '*', 20, 3);
    su_assert(streq(s, "msg = \"*******+3.141*******\""));
    free (s);
  })

  su_test(formatted_length, {
    su_assert_eq(fmt_formatted_length("{f:+,.{}}", 3.1415926, 3), 6);
  })

  freelocale(us_loc);
  freelocale(de_loc);
})

int main()
{
  su_run_module(fmt_tests);
}

