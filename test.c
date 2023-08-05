#include <smallunit.h>
#include <icecream.h>
#define FMT_IMPLEMENTATION
//#define FMT_BIN_GROUP_NIBBLES
#define FMT_DEFAULT_FLOAT_PRECISION -1
#include "fmt.h"

static bool expect_impl(int source_line, const char *expected, const char *fmt, int arg_count, ...) {
    static char buf[256];
    memset(buf, 0, sizeof(buf));
    va_list ap;
    va_start(ap, arg_count);
    const int written = fmt_implementation(
        FMT_NEW_STRING_WRITER(buf, sizeof(buf)), fmt, arg_count, ap
    );
    va_end(ap);
    if (strcmp(expected, buf) != 0) {
        fmt_eprintln(
            "  Line {}:\n    mismatch:\n      expected: \"{}\"\n      got:      \"{}\"",
            source_line, expected, buf
        );
        return false;
    }
    const int expect_written = strlen(expected);
    if (written != expect_written) {
        fmt_eprintln(
            "  Line {}:\n    wrong number of bytes written reported:\n"
            "      expected: {}\n      got: {}\n      string: \"{}\"",
            source_line, expect_written, written, buf
        );
        return false;
    }
    return true;
}

#define expect(_expected, _fmt, ...)             \
    do {                                         \
        if (!expect_impl(                        \
            __LINE__,                            \
            _expected,                           \
            _fmt,                                \
            FMT__VA_ARG_COUNT(__VA_ARGS__)       \
            __VA_OPT__(, FMT__ARGS(__VA_ARGS__)) \
        )) {                                     \
            su_fail();                           \
        }                                        \
    } while(0)

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
        #ifdef FMT_BOOL_YES_NO
        expect("yes", "{}", t);
        expect("no", "{}", f);
        #else
        expect("true", "{}", t);
        expect("false", "{}", f);
        #endif
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
        expect("-"FMT_LOWER_INF, "{}", -INFINITY);
        expect("-"FMT_UPPER_INF, "{F}", -INFINITY);
        expect("-"FMT_LOWER_NAN, "{}", -NAN);
        expect("-"FMT_UPPER_NAN, "{F}", -NAN);
        expect(FMT_LOWER_INF, "{}", HUGE_VAL);
        expect("1.0e00", "{e}", 1.0);
        expect("1.0e03", "{e}", 1000.0);
        expect("1.0e-02", "{e}", 0.01);
        // Currently gives this: 7.378697629483820463747179019264876842498779296875e19
        //expect("7.3786976294838206464e19", "{e}", 0x1p66);
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
        expect("java", "{:.4}", "javascript");
        expect("안녕", "{:.2}", "안녕하세요");
        // Booleans are treated as strings
        // The cast to bool is needed for C11 builds
        #ifdef FMT_BOOL_YES_NO
        expect("y", "{:.1}", (bool)true);
        #else
        expect("t", "{:.1}", (bool)true);
        #endif
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
    })

    su_test("padding", {
        expect("10000", "{:0<5}", 100);
        expect("00100", "{:0>5}", 100);
        expect("0-100", "{:0>5}", -100);
        expect("-0100", "{:05}", -100);
        expect("-0100", "{:0=5}", -100);
        expect("- 100", "{:=5}", -100);
        expect("  2  ", "{:^5.0}", 2.0);
    })
})

int main() {
    su_run_module(internal_functions);
    su_run_module(basic_printing);
    su_run_module(formatting);
}
