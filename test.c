#include <smallunit.h>
#define FMT_IMPLEMENTATION
#include "fmt.h"
#include "icecream.h"

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
            "  Line {}:\n    mismatch:\n      expected: \"{}\"\n      got: \"{}\"",
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

su_module_d(internal_functions, "internal functions", {
    su_test("integer width", {
        su_assert_eq(fmt__unsigned_width(1000, 10), 4);
        su_assert_eq(fmt__signed_width(-1000, 10), 5);
        su_assert_eq(fmt__unsigned_width(0x1000, 16), 4);
        su_assert_eq(fmt__signed_width(-0x1000, 16), 5);
        su_assert_eq(fmt__unsigned_width(0b1000, 2), 4);
        su_assert_eq(fmt__signed_width(-0b1000, 2), 5);
        su_assert_eq(fmt__unsigned_width(01000, 8), 4);
        su_assert_eq(fmt__signed_width(-01000, 8), 5);
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

    su_test("integers", {
        expect("123", "{}", 123);
        expect("-123", "{}", -123);
        expect("123abc", "{x}", 0x123abc);
        expect("-123abc", "{x}", -0x123abc);
        expect("123ABC", "{X}", 0x123ABC);
        expect("-123ABC", "{X}", -0x123ABC);
        expect("11011", "{b}", 0b11011);
        expect("-11011", "{b}", -0b11011);
        expect("644", "{o}", 0644);
        expect("-644", "{o}", -0644);
    })
})

su_module(formatting, {
    su_test("alignment", {
        expect("Hello    ", "{:9}", "Hello");
        expect("Hello    ", "{:<9}", "Hello");
        expect("    Hello", "{:>9}", "Hello");
        expect("  Hello  ", "{:^9}", "Hello");
        expect("  123  ", "{:^7}", 123);
        expect("-   123", "{:=7}", -123);
    })

    su_test("precision", {
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
})

int main() {
    su_run_module(internal_functions);
    su_run_module(basic_printing);
    su_run_module(formatting);
}
