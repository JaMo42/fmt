#include <smallunit.h>
#include <icecream.h>

#define FMT_IMPLEMENTATION
//#define FMT_BIN_GROUP_NIBBLES
#define FMT_DEFAULT_FLOAT_PRECISION -1
#include "fmt.h"

#define STRINGIFY_2(x) #x
#define STRINGIFY(x) STRINGIFY_2(x)

////////////////////////////////////////////////////////////////////////////////
// Expect formatted string
////////////////////////////////////////////////////////////////////////////////

static bool expect_check(
    int source_line, const char *expected, const char *got, int written
) {
    if (strcmp(expected, got) != 0) {
        //size_t diff;
        //for (diff = 0; expected[diff] == got[diff] && got[diff]; ++diff) {}
        //const char *part2 = got + diff;
        //fmt_eprintln(
        //    "  Line {}:\n    mismatch:\n      expected: \"{}\"\n      got:      \"{:.{}}\x1b[31m{}\x1b[0m\"",
        //    source_line, expected, got, diff, part2
        //);
        fmt_eprintln(
            "  Line {}:\n    mismatch:\n      expected: \"{}\"\n      got:      \"{}\"",
            source_line, expected, got
        );
        return false;
    }
    const int expect_written = strlen(expected);
    if (written != expect_written) {
        fmt_eprintln(
            "  Line {}:\n    wrong number of bytes written reported:\n"
            "      expected: {}\n      got: {}\n      string: \"{}\"",
            source_line, expect_written, written, got
        );
        return false;
    }
    return true;
}

static bool expect_impl(
    int source_line, const char *expected, const char *fmt, int arg_count, ...
) {
    static char buf[256];
    memset(buf, 0, sizeof(buf));
    va_list ap;
    va_start(ap, arg_count);
#ifdef __cplusplus
    fmt_String_Writer writer = (fmt_String_Writer) {
        .base = fmt_STRING_WRITER_FUNCTIONS,
        .string = buf,
        .at = buf,
        .end = buf + sizeof(buf),
    };
    const int written = fmt_implementation((fmt_Writer *)&writer, fmt, arg_count, ap);
#else
    const int written = fmt_implementation(
        FMT_NEW_STRING_WRITER(buf, sizeof(buf)), fmt, arg_count, ap
    );
#endif
    va_end(ap);
    return expect_check(source_line, expected, buf, written);
}

#define expect(_expected, _fmt, ...)             \
    do {                                         \
        if (!expect_impl(                        \
            __LINE__,                            \
            _expected,                           \
            _fmt,                                \
            FMT_VA_ARG_COUNT(__VA_ARGS__)        \
            __VA_OPT__(, FMT__ARGS(__VA_ARGS__)) \
        )) {                                     \
            su_fail();                           \
        }                                        \
    } while(0)

static bool expect_time_impl(
    int source_line, const char *expected, const char *fmt, const struct tm *datetime
) {
    static char buf[256];
    memset(buf, 0, sizeof(buf));
#ifdef __cplusplus
    fmt_String_Writer writer = (fmt_String_Writer) {
        .base = fmt_STRING_WRITER_FUNCTIONS,
        .string = buf,
        .at = buf,
        .end = buf + sizeof(buf),
    };
    const int written = fmt_write_time((fmt_Writer *)&writer, fmt, datetime);
#else
    const int written = fmt_write_time(
        FMT_NEW_STRING_WRITER(buf, sizeof(buf)), fmt, datetime
    );
#endif
    return expect_check(source_line, expected, buf, written);
}

#define expect_time(_expected, _fmt, _tm)                        \
    do {                                                         \
        if (!expect_time_impl(__LINE__, _expected, _fmt, _tm)) { \
            su_fail();                                           \
        }                                                        \
    } while (0)

////////////////////////////////////////////////////////////////////////////////
// Expect format specifier
////////////////////////////////////////////////////////////////////////////////

static fmt_String expect_spec_get_check_impl(int number, const char *checks) {
    const char *before, *after;
    if (number) {
        int i;
        // note: postfix incement, will point after the comma
        for (before = checks, i = 0; i < number; i += *before++ == ',') {}
    } else {
        // points after the opening parenthesis
        before = checks + 1;
    }
    after = strchr(before, ',');
    if (!after) {
        after = before + strlen(before) - 1;
    }
    // comma_after either points at the comma after or the closing parenthesis now
    while (*before == ' ') {
        ++before;
    }
    while (*after == ' ') {
        --after;
    }
    return (fmt_String) {
        .data = (char *)before,
        .capacity = 0,
        .size = (size_t)(after - before),
    };
}

/// Get the text for the `number`th check from the varidadic arguments
#define expect_spec_get_check(number, ...) \
    expect_spec_get_check_impl((number), STRINGIFY((__VA_ARGS__)))

static const char * my_parse_specifier(
    fmt_Format_Specifier *out,
    fmt_Type_Id type,
    const char *format_specifier,
    int *arg_count,
    ...
) {
    va_list ap;
    va_start(ap, arg_count);
    const char *const end = fmt__parse_specifier(
        format_specifier, out, type, 1, arg_count, ap
    );
    va_end(ap);
    return end;
}

#define expect_spec(_type, _format, ...)               \
    do {                                               \
        fmt_Format_Specifier spec;                     \
        fmt__format_specifier_default(&spec);          \
        int arg_count = FMT_VA_ARG_COUNT(__VA_ARGS__); \
        const char *end = my_parse_specifier(          \
            &spec,                                     \
            _type,                                     \
            _format,                                   \
            &arg_count                                 \
            __VA_OPT__(, FMT__ARGS(__VA_ARGS__))       \
        );                                             \
        su_assert_eq(*end, '\0');                      \
        su_assert_eq(arg_count, 0);                    \
        expect_spec_2

#define expect_spec_2(...)                                            \
        bool checks[] = { __VA_ARGS__ };                              \
        const int count = sizeof(checks) / sizeof(*checks);           \
        for (int i = 0; i < count; ++i) {                             \
            if (!checks[i]) {                                         \
                fmt_eprintln(                                         \
                    "format specifier check failed near line {}: {}", \
                    __LINE__,                                         \
                    expect_spec_get_check(i, __VA_ARGS__)             \
                );                                                    \
                su_fail();                                            \
            }                                                         \
        }                                                             \
    } while (0)

////////////////////////////////////////////////////////////////////////////////
// Expect time format specifier
////////////////////////////////////////////////////////////////////////////////

#define expect_time_spec(_format)                     \
    do {                                              \
        fmt_Format_Specifier spec;                    \
        fmt__time_format_specifier_default(&spec, 0); \
        const char *end = fmt__parse_time_specifier(  \
            _format,                                  \
            &spec,                                    \
            1                                         \
        );                                            \
        su_assert_eq(*end, '\0');                     \
        expect_time_spec_2

#define expect_time_spec_2(...)                                            \
        bool checks[] = { __VA_ARGS__ };                                   \
        const int count = sizeof(checks) / sizeof(*checks);                \
        for (int i = 0; i < count; ++i) {                                  \
            if (!checks[i]) {                                              \
                fmt_eprintln(                                              \
                    "time format specifier check failed near line {}: {}", \
                    __LINE__,                                              \
                    expect_spec_get_check(i, __VA_ARGS__)                  \
                );                                                         \
                su_fail();                                                 \
            }                                                              \
        }                                                                  \
    } while (0)

////////////////////////////////////////////////////////////////////////////////
// Unicode test cases
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    const fmt_char8_t *data;
    char32_t codepoint;
    int size;
} Unicode_Test_Case;

const Unicode_Test_Case unicode_test_cases[] = {
    {(const fmt_char8_t *)"a", U'a', 1},
    {(const fmt_char8_t *)"Ä", U'Ä', 2},
    {(const fmt_char8_t *)"가", U'가', 3},
    {(const fmt_char8_t *)"💚", U'💚', 4},
};
const int unicode_test_case_count = sizeof(unicode_test_cases) / sizeof(Unicode_Test_Case);

////////////////////////////////////////////////////////////////////////////////
// Tests
////////////////////////////////////////////////////////////////////////////////

static int format_specifier_parsing_tests() {
    void *su__skip = &&skip;
    int su__status = SU_PASS;
    expect_spec(fmt__TYPE_INT, "{x}")(
        spec.type == 'x'
    );
    expect_spec(fmt__TYPE_INT, "{:{}^+#{}..{}}", 'a', 123, 456)(
        spec.fill == 'a',
        spec.align == fmt_ALIGN_CENTER,
        spec.sign == fmt_SIGN_ALWAYS,
        spec.alternate_form == true,
        spec.group == '.',
        spec.precision == 456
    );
    expect_spec(fmt__TYPE_INT, "{:{}<0}", 'a')(
        spec.fill == 'a', // zero padding only affects the fill character in the
        // printing functions, not at parsing
        spec.align == fmt_ALIGN_RIGHT, // but it does change the alignment
    );
skip:
    return su__status;
}

static int time_format_specifier_parsing_tests() {
    void *su__skip = &&skip;
    int su__status = SU_PASS;
    expect_time_spec("{H}")(
        spec.type == 'H'
    );
    expect_time_spec("{M: <.2}")(
        spec.type == 'M',
        spec.fill == ' ',
        spec.align == fmt_ALIGN_LEFT,
        spec.precision == 2,
    );
skip:
    return su__status;
}

su_module_d(internal_functions, "internal functions", {
    su_test("integer width", {
        su_assert_eq(fmt__unsigned_width(1000, 10), 4);
        su_assert_eq(fmt__unsigned_width(0x1000, 16), 4);
        su_assert_eq(fmt__unsigned_width(0b1000, 2), 4);
        su_assert_eq(fmt__unsigned_width(01000, 8), 4);
    })

    su_test("display width", {
        su_assert_eq(fmt__utf8_width_and_length("Hello", -1, -1).first, 5);
        su_assert_eq(fmt__utf8_width_and_length("Hello\n", -1, -1).first, 5);
        su_assert_eq(fmt__utf8_width_and_length("안녕", -1, -1).first, 4);
        su_assert_eq(fmt__utf8_width_and_length("안녕", -1, 1).first, 2);
        su_assert_eq(fmt__utf8_width_and_length("Hello", -1, -1).second, 5);
        su_assert_eq(fmt__utf8_width_and_length("안녕", -1, -1).second, 2);
        // TODO: zero-width characters
        su_assert_eq(fmt__utf16_width_and_length(u"Hello", -1, -1).first, 5);
        su_assert_eq(fmt__utf16_width_and_length(u"안녕", -1, -1).first, 4);
        su_assert_eq(fmt__utf16_width_and_length(u"안녕", -1, 1).first, 2);
        su_assert_eq(fmt__utf16_width_and_length(u"\U00012345", -1, -1).first, 1);
        su_assert_eq(fmt__utf16_width_and_length(u"\U00012345", -1, -1).second, 1);
        su_assert_eq(fmt__utf32_width(U"Hello", -1), 5);
        su_assert_eq(fmt__utf32_width(U"안녕", -1), 4);
        su_assert_eq(fmt__utf32_width(U"안녕", 1), 2);
    })

    su_test("float base and exponent", {
        double f;
        double base;
        double control_base;
        int exp;
        int control_exp;
        for (int i = 0; i <= 100; ++i) {
            f = pow(2.0, i);
            control_exp = (int)log10(f);
            control_base = f / pow(10.0, control_exp);
            fmt__get_base_and_exponent(f, &base, &exp);
            su_assert_eq(exp, control_exp);
            su_assert_eq(base, control_base);
        }
    })

    su_test("utf8 encode and decode", {
        // ENCODE
        char data[4];
        for (
            const Unicode_Test_Case
                *c = unicode_test_cases,
                *end = unicode_test_cases + unicode_test_case_count;
            c != end;
            ++c
        ) {
            su_assert_eq(fmt__utf8_encode(c->codepoint, data), c->size);
            su_assert(memcmp(data, c->data, c->size) == 0);
        }
        // DECODE
        char32_t codepoint;
        for (
            const Unicode_Test_Case
                *c = unicode_test_cases,
                *end = unicode_test_cases + unicode_test_case_count;
            c != end;
            ++c
        ) {
            su_assert_eq(fmt__utf8_decode(c->data, &codepoint), c->size);
            su_assert_eq(codepoint, c->codepoint);
        }
    })

    su_test("float part widths", {
        int expected = 0;
        su_assert_eq(fmt__float_integer_width(0.0), 1);
        for (int i = 1; i <= 1000000; i *= 10) {
            ++expected;
            su_assert_eq(fmt__float_integer_width((double)i), expected);
        }
        expected = 0;
        for (int i = 1; i <= 1000000; i *= 10) {
            if (i == 1) {
                su_assert_eq(fmt__float_fraction_width(1.0 / i), 1);
            } else {
                su_assert_eq(fmt__float_fraction_width(1.0 / i), expected);
            }
            ++expected;
        }
    })

    su_test("format specifier parsing", {
        // Something about the 2-part macro with the variadic arguments
        // irritates the smallunit macros so this needs to be in a separate
        // function.
        su__status = format_specifier_parsing_tests();
    })

    su_test("time format specifier parsing", {
        // Something about the 2-part macro with the variadic arguments
        // irritates the smallunit macros so this needs to be in a separate
        // function.
        su__status = time_format_specifier_parsing_tests();
    })
})

su_module_d(basic_printing, "basic printing", {
    su_test("characters", {
        expect("a", "{}", (char)'a');
        expect("a", "{c}", u'a');
        expect("a", "{c}", U'a');
        expect("a", "{c}", L'a');
        expect("가", "{c}", u'가');
        expect("a", "{c}", 97);
    })

    su_test("strings", {
        expect("hello", "{}", "hello");
        expect("hello", "{}", u"hello");
        expect("hello", "{}", U"hello");
        expect("hello", "{}", L"hello");
        expect("안녕", "{}", U"안녕");
    })

    su_test("booleans", {
        bool t = true;
        bool f = false;
        expect("true", "{}", t);
        expect("false", "{}", f);
    })

    su_test("integers", {
        expect("0", "{}", 0);
        expect("0", "{b}", 0);
        expect("0", "{x}", 0);
        expect("0", "{X}", 0);
        expect("0", "{o}", 0);
        expect("123", "{}", 123);
        expect("-123", "{}", -123);
        expect("123abc", "{x}", 0x123abc);
        expect("-123abc", "{x}", -0x123abc);
        expect("123ABC", "{X}", 0x123abc);
        expect("-123ABC", "{X}", -0x123abc);
        expect("11011", "{b}", 0b11011);
        expect("-11011", "{b}", -0b11011);
        expect("644", "{o}", 0644);
        expect("-644", "{o}", -0644);
        expect("18446744073709551615", "{}", 18446744073709551615ULL);
        expect("1111111111111111111111111111111111111111111111111111111111111111", "{b}", 18446744073709551615ULL);
        expect("1777777777777777777777", "{o}", 18446744073709551615ULL);
        expect("ffffffffffffffff", "{x}", 18446744073709551615ULL);
        expect("FFFFFFFFFFFFFFFF", "{X}", 18446744073709551615ULL);
    })

    su_test("floats", {
        expect("3.1410000000000000142108547152020037174224853515625", "{}", 3.141);
        expect("0.0123456789", "{:.10}", 0.0123456789);
        expect(FMT_LOWER_INF, "{}", INFINITY);
        expect(FMT_UPPER_INF, "{F}", INFINITY);
        expect(FMT_LOWER_NAN, "{}", NAN);
        expect(FMT_UPPER_NAN, "{F}", NAN);
        expect("-" FMT_LOWER_INF, "{}", -INFINITY);
        expect("-" FMT_UPPER_INF, "{F}", -INFINITY);
        expect("-" FMT_LOWER_NAN, "{}", -NAN);
        expect("-" FMT_UPPER_NAN, "{F}", -NAN);
        expect(FMT_LOWER_INF, "{}", HUGE_VAL);
        expect("1.0e00", "{e}", 1.0);
        expect("1.0e03", "{e}", 1000.0);
        expect("1.0e-02", "{e}", 0.01);
        // Currently gives this: 7.378697629483820463747179019264876842498779296875e19
        //expect("7.3786976294838206464e19", "{e}", 0x1p66);
        expect("12.000%", "{%:.3}", 0.12);
        expect("12.30000%", "{%:.5}", 0.123);
        expect("50%", "{%:.0}", 0.5);
    })

    su_test("pointers", {
        const void *p = (void *)0x123;
        const char *s = (char *)0x123;
        expect("123", "{}", p);
        expect("123", "{p}", s);
        expect("abc", "{}", (void*)0xabc);
        expect("ABC", "{P:}", (void*)0xabc);
        expect("ABC", "{P:}", (char*)0xabc);
    })

    su_test("fmt_String", {
        char data[] = "Hello World";
        fmt_String string = ((fmt_String) {
            .data = data,
            .capacity = 0,
            .size = sizeof(data) - 1,
        });
        expect("Hello World", "{}", string);
        string.size = 5;
        expect("Hello World", "{} World", string);
    })
})

su_module(formatting, {
    su_test("alignment", {
        expect("Hello    ", "{:9}", "Hello");
        expect("Hello    ", "{:<9}", "Hello");
        expect("    Hello", "{:>9}", "Hello");
        expect("  Hello  ", "{:^9}", "Hello");
        expect("  a  ", "{:^5}", (char)'a');
        expect("  123  ", "{:^7}", 123);
        expect("-   123", "{:=7}", -123);
        expect("  true  ", "{:^8}", (bool)true);
        expect("  2.0  ", "{:^7}", 2.0);
    })

    su_test("precision", {
        expect("3.141000", "{:.6}", 3.141);
        expect("3", "{:.0}", 3.141);
        expect("1.2e03", "{e:.1}", 1234.0);
        expect(FMT_LOWER_INF, "{:.1}", INFINITY);
        expect("java", "{:.4}", "javascript");
        expect("안녕", "{:.2}", "안녕하세요");
        // Booleans are treated as strings
        // The cast to bool is needed for C11 builds
        expect("t", "{:.1}", (bool)true);
    })

    su_test("grouping", {
        expect("100", "{:'}", 100);
        expect("1'000", "{:'}", 1000);
        expect("10'000'000", "{:'}", 10000000);
        expect("1'2345'6789", "{x:'}", 0x123456789);
        expect("AB'CDEF", "{X:'}", 0xabcdef);
        #ifdef FMT_BIN_GROUP_NIBBLES
        expect("101'0101'0101", "{b:'}", 0b010101010101);
        #else
        expect("101'01010101", "{b:'}", 0b010101010101);
        #endif
        expect("777'644", "{o:'}", 0777644);
        expect("1ä000", "{:ä}", 1000);
    })

    su_test("padding", {
        expect("10000", "{:0<5}", 100);
        expect("00100", "{:0>5}", 100);
        expect("0-100", "{:0>5}", -100);
        expect("-0100", "{:05}", -100);
        expect("-0100", "{:0=5}", -100);
        expect("- 100", "{:=5}", -100);
        expect("  2  ", "{:^5.0}", 2.0);
        expect("äääabc", "{:ä>6}", "abc");
        expect("äääabc", "{:{}>6}", "abc", u'ä');
    })

    su_test("alternate form", {
        expect("0b101", "{b:#}", 0b101);
        expect("0o123", "{o:#}", 00123);
        expect("0xabc", "{x:#}", 0xabc);
        expect("0XABC", "{X:#}", 0xabc);
        expect("0xabc", "{p:#}", (void*)0xabc);
        expect("0XABC", "{P:#}", (void*)0xabc);
        expect("yes", "{:#}", (bool)true);
        expect("no", "{:#}", (bool)false);
    })
})

su_module(datetime, {
    struct tm datetime_value;
    memset(&datetime_value, 0, sizeof(struct tm));
    datetime_value.tm_sec = 1;
    datetime_value.tm_min = 2;
    datetime_value.tm_hour = 3;
    datetime_value.tm_mday = 4;
    datetime_value.tm_mon = 5;
    datetime_value.tm_year = 123;
    datetime_value.tm_wday = 6;
    datetime_value.tm_yday = 7;
    datetime_value.tm_isdst = false; // daylight savings
    const struct tm *datetime = &datetime_value;

    su_test("basic time printing", {
        expect_time("Sat Jun 4 03:02:01 2023", "{a} {b} {d:0} {H}:{M}:{S} {Y}", datetime);
        expect_time("008", "{j}", datetime);
        expect_time("23", "{y}", datetime);
        expect_time("7", "{u}", datetime);
        expect_time("6", "{w}", datetime);
        expect_time("04", "{d}", datetime);
        expect_time("03:02:01 AM", "{r}", datetime);
        expect_time("03:02", "{R}", datetime);
        expect_time("03:02:01", "{T}", datetime);
    })

    su_test("12 hour clock", {
        int old = datetime_value.tm_hour;
        datetime_value.tm_hour = 0;
        expect_time("12 AM", "{I} {p}", datetime);
        datetime_value.tm_hour = 1;
        expect_time("01 am", "{I} {P}", datetime);
        datetime_value.tm_hour = 11;
        expect_time("11 AM", "{I} {p}", datetime);
        datetime_value.tm_hour = 12;
        expect_time("12 pm", "{I} {P}", datetime);
        datetime_value.tm_hour = 13;
        expect_time("01 PM", "{I} {p}", datetime);
        datetime_value.tm_hour = 23;
        expect_time("11 pm", "{I} {P}", datetime);
        datetime_value.tm_hour = old;
    })

    su_test("embedded time strings", {
        expect("Hello Sat Jun 4 03:02:01 2023 World", "Hello {} World", datetime);
        expect("Hello 03:02:01 World", "Hello {%{}} World", datetime, "{H}:{M}:{S}");
        expect("Hello 03:02:01 World", "Hello {%{H}:{M}:{S}%} World", datetime);
    })

    su_test("formatting", {
        expect_time("    Sat", "{a:>7}", datetime);
        expect_time("....Sat", "{a:.>7}", datetime);
        expect_time("Satu", "{A:.4}", datetime);
    })

    su_test("embedded formatting", {
        expect("  03:02  ", "{%{H}:{M}%:^9}", datetime);
        expect("  Satu  ", "{%{A:.4}%:^8}", datetime);
        expect("::Sat::", "{%{a}%::^7}", datetime);
        expect("::Sat Jun 4 03:02:01 2023::", "{::^27}", datetime);
    })

    su_test("strftime compatibility", {
        setlocale(LC_TIME, "POSIX");
        // Omitted: DEGgklnOtUVW%
        //
        // The 'u' field is also omitted, the standard says: "Replaced by the
        // weekday as a decimal number [1,7], with 1 representing Monday.", but
        // strftime seems to use the range [0,6] like the 'w' field for some
        // reason.
        const char specifiers[] = "aAbBcCdeFHIjMpPrRsSwxXyYzZ";
        char strftime_format[] = "%_";
        char fmt_format[] = "{_}";
        enum { SIZE = 64};
        char strftime_result[SIZE];
        char fmt_result[SIZE];
        int written;
        for (const char *s = specifiers; *s; ++s) {
            strftime_format[1] = *s;
            fmt_format[1] = *s;
            strftime(strftime_result, SIZE, strftime_format, datetime);
            written = fmt_format_time_to(fmt_result, SIZE, fmt_format, datetime);
            if (!expect_check(__LINE__, strftime_result, fmt_result, written)) {
                fmt_eprintln("      field:    {}", fmt_format);
                su_fail();
            }
        }
    })
})

int main() {
    su_run_module(internal_functions);
    su_run_module(basic_printing);
    su_run_module(formatting);
    su_run_module(datetime);
}
