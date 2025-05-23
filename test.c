#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "./smallunit.h"

#define FMT_IMPLEMENTATION
//#define FMT_BIN_GROUP_NIBBLES
#define FMT_DEFAULT_FLOAT_PRECISION -1
//#define FMT_FAST_DISPLAY_WIDTH
#include "fmt.h"

#define STRINGIFY_2(x) #x
#define STRINGIFY(x) STRINGIFY_2(x)

enum { STRING_BUF_SIZE = 256 };

static char* get_string_buf() {
    static char buf[STRING_BUF_SIZE];
    return buf;
}

bool compact_flag = false;
bool stop_on_failure_flag = false;

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
    char *const buf = get_string_buf();
    memset(buf, 0, STRING_BUF_SIZE);
    va_list ap;
    va_start(ap, arg_count);
    fmt_String_Writer writer = fmt_sw_new(buf, STRING_BUF_SIZE);
    const int written = fmt_va_write((fmt_Writer *)&writer, fmt, arg_count, ap);
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
            __VA_OPT__(, FMT_ARGS(__VA_ARGS__)) \
        )) {                                     \
            su_fail();                           \
        }                                        \
    } while(0)

static bool expect_time_impl(
    int source_line, const char *expected, const char *fmt, const struct tm *datetime
) {
    char *const buf = get_string_buf();
    memset(buf, 0, STRING_BUF_SIZE);
    fmt_String_Writer writer = fmt_sw_new(buf, STRING_BUF_SIZE);
    const int written = fmt_write_time((fmt_Writer *)&writer, fmt, datetime);
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
        .size = (size_t)(after - before),
        .capacity = 0,
    };
}

/// Get the text for the `number`th check from the variadic arguments
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
        format_specifier, out, type, 1, arg_count, FMT__VA_LIST_REF(ap)
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
            __VA_OPT__(, FMT_ARGS(__VA_ARGS__))       \
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
// Panic
////////////////////////////////////////////////////////////////////////////////

static bool expect_panic_impl(
    int source_line, const char *message, const char *format, int arg_count, ...
) {
    char *const buf = get_string_buf();
    int pipe_ends[2];
    pipe(pipe_ends);
    const int rx = pipe_ends[0];
    const int tx = pipe_ends[1];
    const int pid = fork();
    switch (pid) {
    case -1:
        fmt_panic("fork failed");

    case 0:
        dup2(tx, STDERR_FILENO);
        close(rx);
        va_list ap;
        va_start(ap, arg_count);
        fmt_va_sprint(buf, STRING_BUF_SIZE, format, arg_count, ap);
        va_end(ap);
        close(tx);
        exit(EXIT_SUCCESS);
    }
    close(tx);
    int stat = -1;
    waitpid(pid, &stat, 0);
    if (stat == 0) {
        fmt_eprintln(
            "  Line {}:\n    expected call to panic but it didn't", source_line
        );
        return false;
    }
    const ssize_t n = read(rx, buf, STRING_BUF_SIZE);
    buf[n - 1] = '\0';
    close(rx);
    // I have no idea why but inserts a newline between the fmt__panic_loc
    // and fmt_va_write calls here.  This does not happen when printing to
    // the terminal.
    for (int i = 0; i < n; ++i) {
        if (buf[i] == '\n') {
            memmove(buf + i, buf + i + 1, n - i);
        }
    }
    // skip "file:line: "
    char *start = strchr(strchr(buf, ':') + 1, ':') + 2;
    if (memcmp(start, message, strlen(message))) {
        fmt_eprintln(
            "  Line {}:\n    panic message mismatch:\n      expected: \"{}\"\n      got:      \"{}\"",
            source_line, message, start
        );
        return false;
    }
    return true;
}

/// Checks if a call panics and it's panic message after the source location
/// starts with the given message.
#define expect_panic(_message, _format, ...)    \
    do {                                        \
        if (!expect_panic_impl(                 \
            __LINE__,                           \
            _message,                           \
            _format,                            \
            FMT_VA_ARG_COUNT(__VA_ARGS__)       \
            __VA_OPT__(, FMT_ARGS(__VA_ARGS__)) \
        )) {                                    \
            su_fail();                          \
        }                                       \
    } while(0)

////////////////////////////////////////////////////////////////////////////////
// Unicode test cases
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    const fmt_char8_t *data;
    fmt_char32_t codepoint;
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
    expect_spec(fmt__TYPE_INT, "{:{}^+#{}..{}?}", 'a', 123, 456)(
        spec.fill == 'a',
        spec.align == fmt_ALIGN_CENTER,
        spec.sign == fmt_SIGN_ALWAYS,
        spec.alternate_form == true,
        spec.group == '.',
        spec.precision == 456,
        spec.debug == true
    );
    expect_spec(fmt__TYPE_INT, "{:.}")(
        spec.group == '.'
    );
    expect_spec(fmt__TYPE_INT, "{:.?}")(
        spec.group == '.'
    );
    expect_spec(fmt__TYPE_INT, "{:.1?}")(
        spec.precision == 1,
        spec.debug == true,
    );
    expect_spec(fmt__TYPE_INT, "{:{}<0}", 'a')(
        spec.fill == '0',
        spec.align == fmt_ALIGN_AFTER_SIGN,
    );
    fmt_Format_Specifier def;
    fmt__format_specifier_default(&def);
    expect_spec(fmt__TYPE_INT, "{:?}")(
        spec.group != '?',
        spec.debug == true
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
    su_compact = compact_flag;
    su_stop_on_failure = stop_on_failure_flag;

    su_test("argument count", {
        su_assert_eq(FMT_VA_ARG_COUNT(), 0);
        su_assert_eq(FMT_VA_ARG_COUNT(1), 1);
        su_assert_eq(FMT_VA_ARG_COUNT(1, 2), 2);
        su_assert_eq(FMT_VA_ARG_COUNT(,), 2);
    })

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
        su_assert_eq(fmt__utf16_width_and_length(u"Hello", -1, -1).first, 5);
        su_assert_eq(fmt__utf16_width_and_length(u"안녕", -1, -1).first, 4);
        su_assert_eq(fmt__utf16_width_and_length(u"안녕", -1, 1).first, 2);
        su_assert_eq(fmt__utf16_width_and_length(u"\U00012345", -1, -1).first, 1);
        su_assert_eq(fmt__utf16_width_and_length(u"\U00012345", -1, -1).second, 1);
        su_assert_eq(fmt__utf32_width(U"Hello", -1), 5);
        su_assert_eq(fmt__utf32_width(U"안녕", -1), 4);
        su_assert_eq(fmt__utf32_width(U"안녕", 1), 2);
#ifndef FMT_FAST_DISPLAY_WIDTH
        su_assert_eq(fmt__display_width(u'\u0303'), 0);
#endif
        su_assert_eq(fmt__debug_char_width('a', false), 1);
        su_assert_eq(fmt__debug_char_width('\n', false), 2);
        su_assert_eq(fmt__debug_char_width('\n', true), 2);
        su_assert_eq(fmt__debug_char_width('\'', false), 1);
        su_assert_eq(fmt__debug_char_width('\'', true), 2);
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
        fmt_char32_t codepoint;
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

    su_test("format specifier parsing", {
        // Something about the 2-part macro with the variadic arguments
        // irritates the smallunit macros so this needs to be in a separate
        // function.
        su__status = format_specifier_parsing_tests();
    })

    su_test("time format specifier parsing", {
        // as above
        su__status = time_format_specifier_parsing_tests();
    })

#ifndef __cplusplus
    su_test("strftime format string translation", {
        enum { COUNT = 3};
        const char **sources = ((const char*[COUNT]){
            "%H",
            "%%",
            "{a}",
        });
        const char **expected = ((const char*[COUNT]){
            "{H}",
            "%",
            // Closing curly braces are not currently escaped
            //"{{a}}"
            "{{a}"
        });
        char buf[6];
        for (int i = 0; i < COUNT; ++i) {
            fmt_translate_strftime(sources[i], buf, sizeof(buf));
            if (strcmp(buf, expected[i]) != 0) {
                fmt_eprintln("\"{}\" != \"{}\"", buf, expected[i]);
                su_fail();
            }
        }
    })
#endif
})

su_module_d(basic_printing, "basic printing", {
    su_compact = compact_flag;
    su_stop_on_failure = stop_on_failure_flag;

    su_test("characters", {
        expect("a", "{}", (char)'a');
        expect("a", "{c}", u'a');
        expect("a", "{c}", U'a');
        expect("a", "{c}", L'a');
#ifdef __cplusplus
        expect("a", "{}", L'a');
#endif
        expect("가", "{c}", u'가');
        expect("a", "{c}", 97);
        expect("�", "{c}", -97);
        expect("�", "{c}", FMT_MAX_CODEPOINT + 1);
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
        expect("true", "{B}", 1 != 2);
    })

    su_test("integers", {
        expect("0", "{}", 0);
        expect("0", "{b}", 0);
        expect("0", "{x}", 0);
        expect("0", "{X}", 0);
        expect("0", "{o}", 0);
        expect("123", "{}", 123);
        expect("123", "{d}", 123);
        expect("123", "{i}", 123);
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
        expect("$12.00", "{$}", 12);
        expect("-$10.00", "{$}", -10);
    })

    su_test("floats", {
        expect("-2", "{}", -2.0);
        expect("-1", "{}", -1.0);
        expect("-0.5", "{}", -0.5);
        expect("-0.1", "{}", -0.1);
        expect("-0.01", "{}", -0.01);
        expect("0", "{}", -0.0);
        expect("0", "{}", 0.0);
        expect("0.01", "{}", 0.01);
        expect("0.1", "{}", 0.1);
        expect("0.5", "{}", 0.5);
        expect("1", "{}", 1.0);
        expect("2", "{}", 2.0);
        expect("3.141", "{}", 3.141);
        expect("0.0123456789", "{:.10}", 0.0123456789);
        expect("-3.141", "{:.3}", -3.141);
        expect("73786976294838210000", "{}", 0x1p66);
        expect("inf", "{}", INFINITY);
        expect("INF", "{F}", INFINITY);
        expect("nan", "{}", NAN);
        expect("NAN", "{F}", NAN);
        expect("-inf", "{}", -INFINITY);
        expect("-INF", "{F}", -INFINITY);
        expect("-nan", "{}", -NAN);
        expect("-NAN", "{F}", -NAN);
        expect("inf", "{}", HUGE_VAL);
        expect("1e0", "{e}", 1.0);
        expect("1e3", "{e}", 1000.0);
        expect("1e-2", "{e}", 0.01);
        expect("7.378697629483821e19", "{e}", 0x1p66);
        expect("12.000%", "{%:.3}", 0.12);
        expect("12.30000%", "{%:.5}", 0.123);
        expect("50%", "{%:.0}", 0.5);
        expect("3.14159", "{g}", 3.14159265359);
        expect("314.159", "{g}", 314.159265359);
        expect("31415.9", "{g}", 31415.9265359);
        expect("3.14159e6", "{g}", 3141592.65359);
        expect("3.14159e8", "{g}", 314159265.359);
        expect("$3.14", "{$}", 3.141);
        expect("-$1.00", "{$}", -1.0);
        expect("1e-10", "{}", 1e-10);
        expect("1e-7", "{}", 1e-7);
        expect("0.000001", "{}", 1e-6);
        expect("10", "{}", 1e1);
        expect("10000000000", "{}", 1e10);
        expect("100000000000000000000", "{}", 1e20);
        expect("1e21", "{}", 1e21);
        expect("1e100", "{}", 1e100);
    })

    su_test("pointers", {
        const void *p = (void *)0x123;
        const char *s = (char *)0x123;
        const int *i = (int *)0x123;
        expect("123", "{}", p);
        expect("123", "{p}", s);
        expect("123", "{p}", i);
        expect("abc", "{}", (void*)0xabc);
        expect("ABC", "{P}", (void*)0xabc);
        expect("ABC", "{P}", (char*)0xabc);
    })

    su_test("fmt_String", {
        char data[] = "Hello World";
        fmt_String string = ((fmt_String) {
            .data = data,
            .size = sizeof(data) - 1,
            .capacity = 0,
        });
        expect("Hello World", "{}", string);
        string.size = 5;
        expect("Hello World", "{} World", string);
        string = fmt_format("World");
        expect("Hello World", "Hello {}", string.take);
    })

    su_test("escaping", {
        expect("{d}", "{{d}");
        // turns out we don't need to escape in the format specifier, but we
        // probably should require it? (TODO)
        expect("{{{12{345", "{d:{>9{}", 12345);
        expect_time("{H}", "{{H}", NULL);
        expect("{H}", "{%{{H}%}", (struct tm *)NULL);
        expect("{H}", "{%{}}", (struct tm *)NULL, "{{H}");
    })
})

su_module(formatting, {
    su_compact = compact_flag;
    su_stop_on_failure = stop_on_failure_flag;

    su_test("alignment (and width)", {
        expect("Hello    ", "{:9}", "Hello");
        expect("Hello    ", "{:<9}", "Hello");
        expect("    Hello", "{:>9}", "Hello");
        expect("  Hello  ", "{:^9}", "Hello");
        expect("  a  ", "{:^5}", (char)'a');
        expect("  123  ", "{:^7}", 123);
        expect("-   123", "{:=7}", -123);
        expect("  true  ", "{:^8}", (bool)true);
        expect("   2   ", "{:^7}", 2.0);
        expect("  2.1  ", "{:^7}", 2.1);
        expect("2e2      ", "{e:<9}", 200.0);
        expect("      2e2", "{e:>9}", 200.0);
        expect("   2e2   ", "{e:^9}", 200.0);
        expect("$1.20    ", "{$:<9}", 1.2);
        expect("    $1.20", "{$:>9}", 1.2);
        expect("  $1.20  ", "{$:^9}", 1.2);
        expect("-   $1.20", "{$:=9}", -1.2);
    })

    su_test("precision", {
        expect("3.141000", "{:.6}", 3.141);
        expect("3", "{:.0}", 3.141);
        expect("1.2e3", "{e:.1}", 1234.0);
        expect("inf", "{:.1}", INFINITY);
        expect("java", "{:.4}", "javascript");
        expect("안녕", "{:.$2}", "안녕하세요");
        // Booleans are treated as strings
        // The cast to bool is needed for C11 builds
        expect("t", "{:.1}", (bool)true);
        expect("3.14", "{g:.3}", 3.14159265359);
        expect("314", "{g:.3}", 314.159265359);
        expect("3.14e4", "{g:.3}", 31415.9265359);
        expect("3.14e6", "{g:.3}", 3141592.65359);
        expect("3.14e8", "{g:.3}", 314159265.359);
        expect("3e8", "{g:.0}", 314159265.359);
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
        expect("1", "{:'}", 1.0);
        expect("100", "{:'}", 100.0);
        expect("1'000", "{:'}", 1000.0);
        expect("10'000", "{:'}", 10000.0);
        expect("100'000", "{:'}", 100000.0);
        expect("1'000'000", "{:'}", 1000000.0);
        expect("  1'000", "{:>7'}", 1000);
        expect("  1'000", "{:>7'}", 1000.0);
    })

    su_test("field width, filling, and alignment", {
        expect("10000", "{:0<5}", 100);
        expect("00100", "{:0>5}", 100);
        expect("0-100", "{:0>5}", -100);
        expect("-0100", "{:05}", -100);
        expect("-0100", "{:0=5}", -100);
        expect("- 100", "{:=5}", -100);
        expect("  2  ", "{:^5.0}", 2.0);
        expect("-     2", "{:=7}", -2.0);
        expect("äääabc", "{:ä>6}", "abc");
        expect("äääabc", "{:{}>6}", "abc", u'ä');
        expect("  1.234", "{:>7.}", 1234);
        expect("  inf  ", "{:^7}", INFINITY);
        expect("  -inf  ", "{:^8}", -INFINITY);
#ifndef FMT_FAST_DISPLAY_WIDTH
        expect(" ã ", "{:^3}", "a\u0303");
#else
        // Make sure we get the correct wrong result
        expect("ã ", "{:^3}", "a\u0303");
#endif
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

    su_test("signing", {
        expect("+123", "{:+}", 123);
        expect(" 123", "{: }", 123);
        expect("+2", "{:+}", 2.0);
        expect(" 2", "{: }", 2.0);
        expect("+$1.20", "{$:+}", 1.2);
        expect(" $1.20", "{$: }", 1.2);
    })

    su_test("debug format", {
        expect("'a'", "{c:?}", 'a');
        expect("'\\0'", "{c:?}", '\0');
        expect("'\\t'", "{c:?}", '\t');
        expect("'\\r'", "{c:?}", '\r');
        expect("'\\n'", "{c:?}", '\n');
        expect("'\\\\'", "{c:?}", '\\');
        expect("'\\''", "{c:?}", '\'');
        expect("'\"'", "{c:?}", '"');
        expect("'\\U00110000'", "{c:?}", FMT_MAX_CODEPOINT + 1);

        // UTF-8

        expect("a\"a\"a", "a{:?}a", "a");
        expect("a\"\\0\"a", "a{:.1?}a", "\0");
        expect("a\"\\t\"a", "a{:?}a", "\t");
        expect("a\"\\r\"a", "a{:?}a", "\r");
        expect("a\"\\n\"a", "a{:?}a", "\n");
        expect("a\"\\\\\"a", "a{:?}a", "\\");
        expect("a\"'\"a", "a{:?}a", "'");
        expect("a\"\\\"\"a", "a{:?}a", "\"");

        expect("\"aaa\"", "{:?}", "aaa");
        expect("\"a\\0a\"", "{:.3?}", "a\0a");
        expect("\"a\\ta\"", "{:?}", "a\ta");
        expect("\"a\\ra\"", "{:?}", "a\ra");
        expect("\"a\\na\"", "{:?}", "a\na");
        expect("\"a\\\\a\"", "{:?}", "a\\a");
        expect("\"a'a\"", "{:?}", "a'a");
        expect("\"a\\\"a\"", "{:?}", "a\"a");

        expect("\"\\0\\0\\0\"", "{:.3?}", "\0\0\0");
        expect("\"\\t\\t\\t\"", "{:?}", "\t\t\t");
        expect("\"\\r\\r\\r\"", "{:?}", "\r\r\r");
        expect("\"\\n\\n\\n\"", "{:?}", "\n\n\n");
        expect("\"\\\\\\\\\\\\\"", "{:?}", "\\\\\\");
        expect("\"\\\"\\\"\\\"\"", "{:?}", "\"\"\"");

        // UTF-16

        expect("a\"a\"a", "a{:?}a", u"a");
        expect("a\"\\0\"a", "a{:.1?}a", u"\0");
        expect("a\"\\t\"a", "a{:?}a", u"\t");
        expect("a\"\\r\"a", "a{:?}a", u"\r");
        expect("a\"\\n\"a", "a{:?}a", u"\n");
        expect("a\"\\\\\"a", "a{:?}a", u"\\");
        expect("a\"'\"a", "a{:?}a", u"'");
        expect("a\"\\\"\"a", "a{:?}a", u"\"");

        expect("\"aaa\"", "{:?}", u"aaa");
        expect("\"a\\0a\"", "{:.3?}", u"a\0a");
        expect("\"a\\ta\"", "{:?}", u"a\ta");
        expect("\"a\\ra\"", "{:?}", u"a\ra");
        expect("\"a\\na\"", "{:?}", u"a\na");
        expect("\"a\\\\a\"", "{:?}", u"a\\a");
        expect("\"a'a\"", "{:?}", u"a'a");
        expect("\"a\\\"a\"", "{:?}", u"a\"a");

        expect("\"\\0\\0\\0\"", "{:.3?}", u"\0\0\0");
        expect("\"\\t\\t\\t\"", "{:?}", u"\t\t\t");
        expect("\"\\r\\r\\r\"", "{:?}", u"\r\r\r");
        expect("\"\\n\\n\\n\"", "{:?}", u"\n\n\n");
        expect("\"\\\\\\\\\\\\\"", "{:?}", u"\\\\\\");
        expect("\"\\\"\\\"\\\"\"", "{:?}", u"\"\"\"");

        // UTF-32

        expect("a\"a\"a", "a{:?}a", U"a");
        expect("a\"\\0\"a", "a{:.1?}a", U"\0");
        expect("a\"\\t\"a", "a{:?}a", U"\t");
        expect("a\"\\r\"a", "a{:?}a", U"\r");
        expect("a\"\\n\"a", "a{:?}a", U"\n");
        expect("a\"\\\\\"a", "a{:?}a", U"\\");
        expect("a\"'\"a", "a{:?}a", U"'");
        expect("a\"\\\"\"a", "a{:?}a", U"\"");

        expect("\"aaa\"", "{:?}", U"aaa");
        expect("\"a\\0a\"", "{:.3?}", U"a\0a");
        expect("\"a\\ta\"", "{:?}", U"a\ta");
        expect("\"a\\ra\"", "{:?}", U"a\ra");
        expect("\"a\\na\"", "{:?}", U"a\na");
        expect("\"a\\\\a\"", "{:?}", U"a\\a");
        expect("\"a'a\"", "{:?}", U"a'a");
        expect("\"a\\\"a\"", "{:?}", U"a\"a");

        expect("\"\\0\\0\\0\"", "{:.3?}", U"\0\0\0");
        expect("\"\\t\\t\\t\"", "{:?}", U"\t\t\t");
        expect("\"\\r\\r\\r\"", "{:?}", U"\r\r\r");
        expect("\"\\n\\n\\n\"", "{:?}", U"\n\n\n");
        expect("\"\\\\\\\\\\\\\"", "{:?}", U"\\\\\\");
        expect("\"\\\"\\\"\\\"\"", "{:?}", U"\"\"\"");

        // Width calculation

        expect("  '\\n'  ", "{c:^8?}", '\n');
        expect("  \"\\n\"  ", "{:^8?}", "\n");
        expect("  \"\\n\"  ", "{:^8?}", u"\n");
        expect("  \"\\n\"  ", "{:^8?}", U"\n");

        // Alternate format

        expect("\\n", "{:#?}", "\n");
        expect("\\n", "{c:#?}", '\n');
        expect("  \\n  ", "{c:^#6?}", '\n');
        expect("  \\n  ", "{:^#6?}", "\n");
        expect("  \\n  ", "{:^#6?}", u"\n");
        expect("  \\n  ", "{:^#6?}", U"\n");
        expect("\"'", "{:#?}", "\"'");
        expect("'", "{c:#?}", '\'');
        expect("\"'", "{:#?}", u"\"'");
        expect("\"'", "{:#?}", U"\"'");
        expect("  \"'  ", "{:^#6?}", "\"'");
        expect("  '  ", "{c:^#5?}", '\'');
        expect("  \"'  ", "{:^#6?}", u"\"'");
        expect("  \"'  ", "{:^#6?}", U"\"'");
    })
})

su_module(datetime, {
    su_compact = compact_flag;
    su_stop_on_failure = stop_on_failure_flag;

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
    datetime_value.tm_zone = (char *)"Timezone name";
    datetime_value.tm_gmtoff = -14400; // -4 hours
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
        expect_time("-0400", "{z}", datetime);
        expect_time("Timezone name", "{Z}", datetime);
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
        expect("  Satu  ", "{%{A}%:^8.4}", datetime);
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
        #ifdef __APPLE__
        // The %P specifier is a GNU extension
        // TODO: %z gives me a different result from strftime than on Linux?
        const char specifiers[] = "aAbBcCdeFHIjMprRsSwxXyYZ";
        #else
        const char specifiers[] = "aAbBcCdeFHIjMpPrRsSwxXyYzZ";
        #endif
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

su_module(panics, {
    su_compact = compact_flag;
    su_stop_on_failure = stop_on_failure_flag;

    su_test("argument count", {
        expect_panic("arguments exhausted at specifier 1", "{}");
        expect_panic("3 arguments left", "", 1, 2, 3);
        expect_panic("arguments exhausted at fill character", "{:{}>2}", 1);
        expect_panic("arguments exhausted at width", "{:{}}", 1);
        expect_panic("arguments exhausted at precision", "{:.{}}", 1);
        expect_panic("arguments exhausted at parameterized time format", "{%{}}", (struct tm *)NULL);
    })

    su_test("format specifier format", {
        expect_panic("overflow in width", "{:>99999999999}", 1);
        expect_panic("expected : or } after display type", "{d{}", 1);
        expect_panic("undelimited time format string", "{%{{H}}", (struct tm *)NULL);
    })

    su_test("conversion specifiers", {
        expect_panic("invalid display type", "{s}", 1);
        expect_panic("invalid display type", "{d}", "a");
        expect_panic("invalid display type", "{p}", 1.2);
        long i = 0;
        expect_panic("unimplemented argument type", "{c}", &i);
        expect_panic("invalid display type", "{g}", (char)'a');
    })

    su_test("overflow", {
        expect_panic("string writer overflow", "{:>999}", 'a');
    })

    su_test("parameter types", {
        expect_panic("expected integer type", "{:{}}", 1, "width");
        expect_panic("expected unsigned value", "{:{}}", 1, -1);
        expect_panic("expected character type", "{:{}>1}", 1, 1.0);
    })
})

su_module(writers, {
    su_test("allocating writer", {
        enum { COUNT = 1000 };
        fmt_Allocating_String_Writer writer = fmt_aw_new();
        for (int i = 0; i < COUNT; ++i) {
            fmt_write((fmt_Writer*)&writer, "a");
        }
        fmt_String str = fmt_aw_finish(writer);
        su_assert_eq(str.size, COUNT+1);
        for (int i = 0; i < COUNT; ++i) {
            su_assert_eq(str.data[i], 'a');
        }
        su_assert_eq(str.data[COUNT], '\0');
        free(str.data);
    })

    su_test("buffered writer", {
        char *const buf = get_string_buf();
        memset(buf, 0, STRING_BUF_SIZE);
        fmt_String_Writer inner = fmt_sw_new(buf, STRING_BUF_SIZE);
        fmt_Buffered_Writer writer = fmt_bw_new((fmt_Writer*)&inner);
        for (int i = 0; i < FMT_BUFFERED_WRITER_CAPACITY; ++i) {
            fmt_write((fmt_Writer*)&writer, "a");
        }
        su_assert_eq(strlen(buf), 0);
        fmt_write((fmt_Writer*)&writer, "a");
        su_assert_eq(strlen(buf), FMT_BUFFERED_WRITER_CAPACITY);
        for (int i = 0; i < FMT_BUFFERED_WRITER_CAPACITY; ++i) {
            su_assert_eq(buf[i], 'a');
        }
        fmt_bw_flush(&writer);
        su_assert_eq(strlen(buf), FMT_BUFFERED_WRITER_CAPACITY + 1);
        su_assert_eq(buf[FMT_BUFFERED_WRITER_CAPACITY], 'a');
    })
})

int main(const int argc, const char **argv) {
    setlocale(LC_ALL, "C");
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--compact") == 0 || strcmp(argv[i], "-c") == 0) {
            compact_flag = true;
        } else if (strcmp(argv[i], "--stop-on-failure") == 0 || strcmp(argv[i], "-s") == 0) {
            stop_on_failure_flag = true;
        } else if (strcmp(argv[i], "--help") == 0
                   || strcmp(argv[i], "-h") == 0
                   || strcmp(argv[i], "-?") == 0) {
            puts("Usage: ./test [-c/--compact] [-s/--stop-on-failure]");
            return 0;
        }
    }
    SUResult total = su_new_result();
    su_add_result(&total, su_run_module(internal_functions));
    su_add_result(&total, su_run_module(basic_printing));
    su_add_result(&total, su_run_module(formatting));
    su_add_result(&total, su_run_module(datetime));
    su_add_result(&total, su_run_module(writers));
    // This is slow so I leave it disabled during development and only enable it
    // after finishing something new.
    //su_add_result(&total, su_run_module(panics));
    puts("Total:");
    su_print_result(&total);
}
