#ifndef FMT_H
#define FMT_H
#include <stddef.h>
#if __STDC_VERSION__ <= 201710L
#  include <stdbool.h>
#endif
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <threads.h>
#include <uchar.h>
#include <math.h>
#include <time.h>

// `char8_t` is only available since C23 but we also want to support C11 so
// since we need our typedef anyways we never use `char8_t` since we wouldn't
// want to use that name for the C11 version.
typedef uint_least8_t fmt_char8_t;

// TODO: remove
#include "icecream.h"

#if __STDC_VERSION__ > 201710L
#  define FMT__NORETURN [[noreturn]]
#else
#  define FMT__NORETURN _Noreturn
#endif

#ifndef FMT_DEFAULT_TIME_FORMAT
#  define FMT_DEFAULT_TIME_FORMAT "%a %b %d %H:%M:%S %Y"
#endif

#ifndef FMT_LOWER_INF
#  define FMT_LOWER_INF "inf"
#endif

#ifndef FMT_UPPER_INF
#  define FMT_UPPER_INF "INF"
#endif

#ifndef FMT_LOWER_NAN
#  define FMT_LOWER_NAN "NaN"
#endif

#ifndef FMT_UPPER_NAN
#  define FMT_UPPER_NAN "NAN"
#endif

////////////////////////////////////////////////////////////////////////////////
// Recursive macros
////////////////////////////////////////////////////////////////////////////////

// TODO: I copied this, why does it use 2 indirections?
#define FMT__CAT(a, b) FMT__CAT_1(a, b)
#define FMT__CAT_1(a, b) FMT__CAT_2(a, b)
#define FMT__CAT_2(a, b) a##b

#define FMT__FOR_EACH_1(what, x) what(x)
// no clue why the __VA_OPT__ is needed but without it FMT__ARGS breaks with
// only 1 arguments.  It seems like it's calling FMT__FOR_EACH_2 even though
// there is only 1.  I don't know why and I couldn't fix it in another way.
#define FMT__FOR_EACH_2(what, x, ...) what(x) __VA_OPT__(, FMT__FOR_EACH_1(what, __VA_ARGS__))
#define FMT__FOR_EACH_3(what, x, ...) what(x), FMT__FOR_EACH_2(what, __VA_ARGS__)
#define FMT__FOR_EACH_4(what, x, ...) what(x), FMT__FOR_EACH_3(what, __VA_ARGS__)
#define FMT__FOR_EACH_5(what, x, ...) what(x), FMT__FOR_EACH_4(what, __VA_ARGS__)
#define FMT__FOR_EACH_6(what, x, ...) what(x), FMT__FOR_EACH_5(what, __VA_ARGS__)
#define FMT__FOR_EACH_7(what, x, ...) what(x), FMT__FOR_EACH_6(what, __VA_ARGS__)
#define FMT__FOR_EACH_8(what, x, ...) what(x), FMT__FOR_EACH_7(what, __VA_ARGS__)
#define FMT__FOR_EACH_9(what, x, ...) what(x), FMT__FOR_EACH_8(what, __VA_ARGS__)
#define FMT__FOR_EACH_10(what, x, ...) what(x), FMT__FOR_EACH_9(what, __VA_ARGS__)
#define FMT__FOR_EACH_11(what, x, ...) what(x), FMT__FOR_EACH_10(what, __VA_ARGS__)
#define FMT__FOR_EACH_12(what, x, ...) what(x), FMT__FOR_EACH_11(what, __VA_ARGS__)
#define FMT__FOR_EACH_13(what, x, ...) what(x), FMT__FOR_EACH_12(what, __VA_ARGS__)
#define FMT__FOR_EACH_14(what, x, ...) what(x), FMT__FOR_EACH_13(what, __VA_ARGS__)
#define FMT__FOR_EACH_15(what, x, ...) what(x), FMT__FOR_EACH_14(what, __VA_ARGS__)
#define FMT__FOR_EACH_16(what, x, ...) what(x), FMT__FOR_EACH_15(what, __VA_ARGS__)
#define FMT__FOR_EACH_17(what, x, ...) what(x), FMT__FOR_EACH_16(what, __VA_ARGS__)
#define FMT__FOR_EACH_18(what, x, ...) what(x), FMT__FOR_EACH_17(what, __VA_ARGS__)
#define FMT__FOR_EACH_19(what, x, ...) what(x), FMT__FOR_EACH_18(what, __VA_ARGS__)
#define FMT__FOR_EACH_20(what, x, ...) what(x), FMT__FOR_EACH_19(what, __VA_ARGS__)
#define FMT__FOR_EACH_21(what, x, ...) what(x), FMT__FOR_EACH_20(what, __VA_ARGS__)
#define FMT__FOR_EACH_22(what, x, ...) what(x), FMT__FOR_EACH_21(what, __VA_ARGS__)
#define FMT__FOR_EACH_23(what, x, ...) what(x), FMT__FOR_EACH_22(what, __VA_ARGS__)
#define FMT__FOR_EACH_24(what, x, ...) what(x), FMT__FOR_EACH_23(what, __VA_ARGS__)
#define FMT__FOR_EACH_25(what, x, ...) what(x), FMT__FOR_EACH_24(what, __VA_ARGS__)
#define FMT__FOR_EACH_26(what, x, ...) what(x), FMT__FOR_EACH_25(what, __VA_ARGS__)
#define FMT__FOR_EACH_27(what, x, ...) what(x), FMT__FOR_EACH_26(what, __VA_ARGS__)
#define FMT__FOR_EACH_28(what, x, ...) what(x), FMT__FOR_EACH_27(what, __VA_ARGS__)
#define FMT__FOR_EACH_29(what, x, ...) what(x), FMT__FOR_EACH_28(what, __VA_ARGS__)
#define FMT__FOR_EACH_30(what, x, ...) what(x), FMT__FOR_EACH_29(what, __VA_ARGS__)
#define FMT__FOR_EACH_31(what, x, ...) what(x), FMT__FOR_EACH_30(what, __VA_ARGS__)
#define FMT__FOR_EACH_32(what, x, ...) what(x), FMT__FOR_EACH_31(what, __VA_ARGS__)
#define FMT__FOR_EACH_33(what, x, ...) what(x), FMT__FOR_EACH_32(what, __VA_ARGS__)
#define FMT__FOR_EACH_34(what, x, ...) what(x), FMT__FOR_EACH_33(what, __VA_ARGS__)
#define FMT__FOR_EACH_35(what, x, ...) what(x), FMT__FOR_EACH_34(what, __VA_ARGS__)
#define FMT__FOR_EACH_36(what, x, ...) what(x), FMT__FOR_EACH_35(what, __VA_ARGS__)
#define FMT__FOR_EACH_37(what, x, ...) what(x), FMT__FOR_EACH_36(what, __VA_ARGS__)
#define FMT__FOR_EACH_38(what, x, ...) what(x), FMT__FOR_EACH_37(what, __VA_ARGS__)
#define FMT__FOR_EACH_39(what, x, ...) what(x), FMT__FOR_EACH_38(what, __VA_ARGS__)
#define FMT__FOR_EACH_40(what, x, ...) what(x), FMT__FOR_EACH_39(what, __VA_ARGS__)
#define FMT__FOR_EACH_41(what, x, ...) what(x), FMT__FOR_EACH_40(what, __VA_ARGS__)
#define FMT__FOR_EACH_42(what, x, ...) what(x), FMT__FOR_EACH_41(what, __VA_ARGS__)
#define FMT__FOR_EACH_43(what, x, ...) what(x), FMT__FOR_EACH_42(what, __VA_ARGS__)
#define FMT__FOR_EACH_44(what, x, ...) what(x), FMT__FOR_EACH_43(what, __VA_ARGS__)
#define FMT__FOR_EACH_45(what, x, ...) what(x), FMT__FOR_EACH_44(what, __VA_ARGS__)
#define FMT__FOR_EACH_46(what, x, ...) what(x), FMT__FOR_EACH_45(what, __VA_ARGS__)
#define FMT__FOR_EACH_47(what, x, ...) what(x), FMT__FOR_EACH_46(what, __VA_ARGS__)
#define FMT__FOR_EACH_48(what, x, ...) what(x), FMT__FOR_EACH_47(what, __VA_ARGS__)
#define FMT__FOR_EACH_49(what, x, ...) what(x), FMT__FOR_EACH_48(what, __VA_ARGS__)
#define FMT__FOR_EACH_50(what, x, ...) what(x), FMT__FOR_EACH_49(what, __VA_ARGS__)
#define FMT__FOR_EACH_51(what, x, ...) what(x), FMT__FOR_EACH_50(what, __VA_ARGS__)
#define FMT__FOR_EACH_52(what, x, ...) what(x), FMT__FOR_EACH_51(what, __VA_ARGS__)
#define FMT__FOR_EACH_53(what, x, ...) what(x), FMT__FOR_EACH_52(what, __VA_ARGS__)
#define FMT__FOR_EACH_54(what, x, ...) what(x), FMT__FOR_EACH_53(what, __VA_ARGS__)
#define FMT__FOR_EACH_55(what, x, ...) what(x), FMT__FOR_EACH_54(what, __VA_ARGS__)
#define FMT__FOR_EACH_56(what, x, ...) what(x), FMT__FOR_EACH_55(what, __VA_ARGS__)
#define FMT__FOR_EACH_57(what, x, ...) what(x), FMT__FOR_EACH_56(what, __VA_ARGS__)
#define FMT__FOR_EACH_58(what, x, ...) what(x), FMT__FOR_EACH_57(what, __VA_ARGS__)
#define FMT__FOR_EACH_59(what, x, ...) what(x), FMT__FOR_EACH_58(what, __VA_ARGS__)
#define FMT__FOR_EACH_60(what, x, ...) what(x), FMT__FOR_EACH_59(what, __VA_ARGS__)
#define FMT__FOR_EACH_61(what, x, ...) what(x), FMT__FOR_EACH_60(what, __VA_ARGS__)
#define FMT__FOR_EACH_62(what, x, ...) what(x), FMT__FOR_EACH_61(what, __VA_ARGS__)
#define FMT__FOR_EACH_63(what, x, ...) what(x), FMT__FOR_EACH_62(what, __VA_ARGS__)
#define FMT__FOR_EACH_64(what, x, ...) what(x), FMT__FOR_EACH_63(what, __VA_ARGS__)

#define FMT__FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, N, ...) N

#define FMT__FOR_EACH_RSEQ_N 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FMT__FOR_EACH_NARG_(...) FMT__FOR_EACH_ARG_N(__VA_ARGS__)
#define FMT__FOR_EACH_NARG(...) FMT__FOR_EACH_NARG_(__VA_ARGS__, FMT__FOR_EACH_RSEQ_N)

#define FMT__FOR_EACH_(N, what, x, ...) \
  FMT__CAT(FMT__FOR_EACH_, N)(what, x __VA_OPT__(,) __VA_ARGS__)
#define FMT__FOR_EACH(what, x, ...) \
  FMT__FOR_EACH_(FMT__FOR_EACH_NARG(x, __VA_ARGS__), what, x, __VA_ARGS__)

/// Returns the number of arguments given.
#define FMT__VA_ARG_COUNT(...) (0 __VA_OPT__(+ FMT__FOR_EACH_NARG(__VA_ARGS__)))

////////////////////////////////////////////////////////////////////////////////
// Types IDs
////////////////////////////////////////////////////////////////////////////////

typedef enum {
    fmt__TYPE_CHAR,
    fmt__TYPE_SIGNED_CHAR,
    fmt__TYPE_SHORT,
    fmt__TYPE_INT,
    fmt__TYPE_LONG,
    fmt__TYPE_LONG_LONG,
    fmt__TYPE_UNSIGNED_CHAR,
    fmt__TYPE_UNSIGNED_SHORT,
    fmt__TYPE_UNSIGNED,
    fmt__TYPE_UNSIGNED_LONG,
    fmt__TYPE_UNSIGNED_LONG_LONG,
    fmt__TYPE_FLOAT,
    fmt__TYPE_DOUBLE,
    fmt__TYPE_BOOL,
    fmt__TYPE_STRING,
    fmt__TYPE_STRING_16,
    fmt__TYPE_STRING_32,
    fmt__TYPE_POINTER,
    fmt__TYPE_UNKNOWN,
} fmt_Type_Id;

#ifdef _MSC_VER
#define fmt__TYPE_WSTRING fmt__TYPE_STRING_16
#else
#define fmt__TYPE_WSTRING fmt__TYPE_STRING_32
#endif

#define FMT__TYPE_ID(x) \
    _Generic((x), \
        char: fmt__TYPE_CHAR, \
        signed char: fmt__TYPE_SIGNED_CHAR, \
        short: fmt__TYPE_SHORT, \
        int: fmt__TYPE_INT, \
        long: fmt__TYPE_LONG, \
        long long: fmt__TYPE_LONG_LONG, \
        unsigned char: fmt__TYPE_UNSIGNED_CHAR, \
        unsigned short: fmt__TYPE_UNSIGNED_SHORT, \
        unsigned: fmt__TYPE_UNSIGNED, \
        unsigned long: fmt__TYPE_UNSIGNED_LONG, \
        unsigned long long: fmt__TYPE_UNSIGNED_LONG_LONG, \
        float: fmt__TYPE_FLOAT, \
        double: fmt__TYPE_DOUBLE, \
        bool: fmt__TYPE_BOOL, \
        char *: fmt__TYPE_STRING, \
        const char *: fmt__TYPE_STRING, \
        const char16_t *: fmt__TYPE_STRING_16, \
        char16_t *: fmt__TYPE_STRING_16, \
        const char32_t *: fmt__TYPE_STRING_32, \
        char32_t *: fmt__TYPE_STRING_32, \
        wchar_t *: fmt__TYPE_WSTRING, \
        const wchar_t *: fmt__TYPE_WSTRING, \
        void *: fmt__TYPE_POINTER, \
        const void *: fmt__TYPE_POINTER, \
        default: fmt__TYPE_UNKNOWN \
    )

#define FMT__TYPE_ID_AND_VALUE(x) FMT__TYPE_ID(x), (x)

/// Turns a list of parameters into a list of their type IDs and the parameter:
/// `param1, param2, param3` -> `type1, param1, type2, param2, type3, param3`
/// WARNING: does not handle 0 parameters and must be wrapped in __VA_OPT__ by
/// by the calling macro.
#define FMT__ARGS(...) FMT__FOR_EACH(FMT__TYPE_ID_AND_VALUE, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// Writer interface
////////////////////////////////////////////////////////////////////////////////

typedef struct fmt_Writer {
    /// Writes a single byte
    int (*write_byte)(struct fmt_Writer *self, char byte);
    /// Writes `n` bytes of `data`
    int (*write_data)(struct fmt_Writer *self, const char *data, size_t n);
    /// Writes `str` until a null-byte is encountered
    int (*write_str)(struct fmt_Writer *self, const char *str);
} fmt_Writer;

typedef struct {
    fmt_Writer base;
    FILE *stream;
} fmt_Stream_Writer;

typedef struct {
    fmt_Writer base;
    char *string;
    char *at;
    char *end;
} fmt_String_Writer;

typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} fmt_String;

typedef struct {
    fmt_Writer base;
    fmt_String string;
} fmt_Allocating_String_Writer;

typedef struct {
    fmt_Writer base;
    int bytes_written;
    int display_width;
} fmt_Metric_writer;

int fmt__write_stream_byte(fmt_Writer *p_self, char byte);
int fmt__write_stream_data(fmt_Writer *p_self, const char *data, size_t n);
int fmt__write_stream_str (fmt_Writer *p_self, const char *str);

int fmt__write_string_byte(fmt_Writer *p_self, char byte);
int fmt__write_string_data(fmt_Writer *p_self, const char *data, size_t n);
int fmt__write_string_str (fmt_Writer *p_self, const char *str);

int fmt__write_alloc_byte(fmt_Writer *p_self, char byte);
int fmt__write_alloc_data(fmt_Writer *p_self, const char *data, size_t n);
int fmt__write_alloc_str (fmt_Writer *p_self, const char *str);

int fmt__write_metric_byte(fmt_Writer *p_self, char byte);
int fmt__write_metric_data(fmt_Writer *p_self, const char *data, size_t n);
int fmt__write_metric_str (fmt_Writer *p_self, const char *str);

#define FMT_NEW_STREAM_WRITER(_stream)            \
    ((fmt_Writer *)&(fmt_Stream_Writer){          \
        .base = (fmt_Writer) {                    \
            .write_byte = fmt__write_stream_byte, \
            .write_data = fmt__write_stream_data, \
            .write_str = fmt__write_stream_str,   \
        },                                        \
        .stream = (_stream),                      \
    })

#define FMT_NEW_STRING_WRITER(_string, _n) \
    ((fmt_Writer *)&(fmt_String_Writer) { \
        .base = (fmt_Writer) { \
            .write_byte = fmt__write_string_byte, \
            .write_data = fmt__write_string_data, \
            .write_str = fmt__write_string_str, \
        }, \
        .string = (_string), \
        .at = (_string), \
        .end = (_string) + (_n), \
    })

////////////////////////////////////////////////////////////////////////////////
// Core functions
////////////////////////////////////////////////////////////////////////////////

extern int fmt_implementation(fmt_Writer *writer, const char *format, int arg_count, va_list ap);
extern int fmt__with_writer(fmt_Writer *writer, const char *format, int arg_count, ...);
/// Implementation for the stdout and stderr printers which handles locking and
/// adding a newline for the `-ln` variants.  It takes a stream because stdout
/// and stderr are not const so we can't easily use static variables for them.
extern int fmt__default_printer(FILE *stream, const char *format, bool newline, int arg_count, ...);
FMT__NORETURN extern void fmt__panic(const char *file, int line, const char *format, int arg_count, ...);

////////////////////////////////////////////////////////////////////////////////
// User-facing wrapper macros
////////////////////////////////////////////////////////////////////////////////

/// Returns `true` if the given variable can be printed.
#define fmt_can_print(x) (FMT__TYPE_ID(x) != fmt__TYPE_UNKNOWN)

#define fmt_with_writer(_writer, _format, ...) \
    fmt__with_writer(                          \
        _writer,                               \
        _format,                               \
        FMT__VA_ARG_COUNT(__VA_ARGS__)         \
        __VA_OPT__(, FMT__ARGS(__VA_ARGS__))   \
    )

#define fmt_print(_format, ...)              \
    fmt__default_printer(                    \
        stdout,                              \
        _format,                             \
        false,                               \
        FMT__VA_ARG_COUNT(__VA_ARGS__)       \
        __VA_OPT__(, FMT__ARGS(__VA_ARGS__)) \
    )

#define fmt_println(_format, ...)            \
    fmt__default_printer(                    \
        stdout,                              \
        _format,                             \
        true,                                \
        FMT__VA_ARG_COUNT(__VA_ARGS__)       \
        __VA_OPT__(, FMT__ARGS(__VA_ARGS__)) \
    )

#define fmt_eprint(_format, ...)             \
    fmt__default_printer(                    \
        stderr,                              \
        _format,                             \
        false,                               \
        FMT__VA_ARG_COUNT(__VA_ARGS__)       \
        __VA_OPT__(, FMT__ARGS(__VA_ARGS__)) \
    )

#define fmt_eprintln(_format, ...)           \
    fmt__default_printer(                    \
        stderr,                              \
        _format,                             \
        true,                                \
        FMT__VA_ARG_COUNT(__VA_ARGS__)       \
        __VA_OPT__(, FMT__ARGS(__VA_ARGS__)) \
    )

#define fmt_sprint(_string, _n, _format, ...)   \
    fmt__with_writer(                           \
        FMT_NEW_STRING_WRITER((_string), (_n)), \
        _format,                                \
        false,                                  \
        FMT__VA_ARG_COUNT(__VA_ARGS__)          \
        __VA_OPT__(, FMT__ARGS(__VA_ARGS__))    \
    )

#define fmt_panic(_format, ...)              \
    fmt__panic(                              \
        __FILE__,                            \
        __LINE__,                            \
         _format,                            \
        FMT__VA_ARG_COUNT(__VA_ARGS__)       \
        __VA_OPT__(, FMT__ARGS(__VA_ARGS__)) \
    )

#endif /* FMT_H */

#ifdef FMT_IMPLEMENTATION

////////////////////////////////////////////////////////////////////////////////
// Type ID functions
////////////////////////////////////////////////////////////////////////////////

static size_t fmt__va_get_unsigned_integer(va_list ap) {
    fmt_Type_Id type = va_arg(ap, fmt_Type_Id);
    switch (type) {
    case fmt__TYPE_SIGNED_CHAR:
    case fmt__TYPE_SHORT:
    case fmt__TYPE_INT:
    // these are promoted to int
    case fmt__TYPE_UNSIGNED_CHAR:
    case fmt__TYPE_UNSIGNED_SHORT: {
        int n = va_arg(ap, int);
        if (n < 0) goto negative;
        return n;
    }
    case fmt__TYPE_LONG: {
        long n = va_arg(ap, long);
        if (n < 0) goto negative;
        return n;
    }
    case fmt__TYPE_LONG_LONG: {
        long long n = va_arg(ap, long long);
        if (n < 0) goto negative;
        return n;
    }
    case fmt__TYPE_UNSIGNED_LONG: {
        unsigned long n = va_arg(ap, unsigned long);
        return n;
    }
    case fmt__TYPE_UNSIGNED_LONG_LONG: {
        unsigned long long n = va_arg(ap, unsigned long long);
        return n;
    }
    default:
        fmt_panic("expected integer type");
    }
negative:
    fmt_panic("expected unsigned value");
}

static char32_t fmt__va_get_character(va_list ap) {
    fmt_Type_Id type = va_arg(ap, fmt_Type_Id);
    switch (type) {
    case fmt__TYPE_CHAR:
        // char gets promoted to int
        return va_arg(ap, int);
    case fmt__TYPE_INT:
        // character literals have type `int`, fmt__TYPE_CHAR is only available
        // for `char` variables.
        return va_arg(ap, int);
    case fmt__TYPE_UNSIGNED_SHORT:
        // promoted to int
        return va_arg(ap, int);
    case fmt__TYPE_UNSIGNED:
        return va_arg(ap, char32_t);
    default:
        fmt_panic("expected character type");
    }
}

static const char *fmt__valid_display_types(fmt_Type_Id type) {
    switch (type) {
        case fmt__TYPE_CHAR:
            // `t` is pretty useless here since it can only represent 127
            // seconds since the epoch but we want it to be equivalent to the
            // integer types.
            return "cdxXbo";
        case fmt__TYPE_SIGNED_CHAR:
        case fmt__TYPE_SHORT:
        case fmt__TYPE_INT:
        case fmt__TYPE_LONG:
        case fmt__TYPE_LONG_LONG:
        case fmt__TYPE_UNSIGNED_CHAR:
        case fmt__TYPE_UNSIGNED_SHORT:
        case fmt__TYPE_UNSIGNED:
        case fmt__TYPE_UNSIGNED_LONG:
        case fmt__TYPE_UNSIGNED_LONG_LONG:
            return "cdxXbo";
        case fmt__TYPE_FLOAT:
        case fmt__TYPE_DOUBLE:
            return "fFeEgG%";
        case fmt__TYPE_STRING:
        case fmt__TYPE_STRING_16:
        case fmt__TYPE_STRING_32:
            return "spP";
        case fmt__TYPE_POINTER:
        case fmt__TYPE_UNKNOWN:
            return "pP";
        default:
            return "";
    }
}

////////////////////////////////////////////////////////////////////////////////
// Writers
////////////////////////////////////////////////////////////////////////////////

int fmt__write_stream_byte(fmt_Writer *p_self, char byte) {
    fmt_Stream_Writer *self = (fmt_Stream_Writer*)p_self;
    fputc(byte, self->stream);
    return 1;
}

int fmt__write_stream_data(fmt_Writer *p_self, const char *data, size_t n) {
    fmt_Stream_Writer *self = (fmt_Stream_Writer*)p_self;
    return (int)fwrite(data, 1, n, self->stream);
}

int fmt__write_stream_str(fmt_Writer *p_self, const char *str) {
    fmt_Stream_Writer *self = (fmt_Stream_Writer*)p_self;
    // fputs doesn't give use the number of bytes written.
    const size_t len = strlen(str);
    return (int)fwrite(str, 1, len, self->stream);
}

static void fmt__string_writer_check(fmt_String_Writer *self, int space) {
    if (self->at + space >= self->end) {
        fmt_panic("string writer overflow\n  current content: \"{:.{}}\"", self->string, self->at - self->string);
    }
}

int fmt__write_string_byte(fmt_Writer *p_self, char byte) {
    fmt_String_Writer *self = (fmt_String_Writer *)p_self;
    fmt__string_writer_check(self, 1);
    *self->at++ = byte;
    return 1;
}

int fmt__write_string_data(fmt_Writer *p_self, const char *data, size_t n) {
    fmt_String_Writer *self = (fmt_String_Writer *)p_self;
    fmt__string_writer_check(self, n);
    memcpy(self->at, data, n);
    self->at += n;
    return n;
}

int fmt__write_string_str (fmt_Writer *p_self, const char *str) {
    return fmt__write_string_data(p_self, str, strlen(str));
}

////////////////////////////////////////////////////////////////////////////////
// Unicode utilities
////////////////////////////////////////////////////////////////////////////////

enum {
    /// The Unicode replacement character
    FMT_REPLACEMENT_CHARACTER = 0xfffd,
    /// The highest valid Unicode codepoint
    FMT_MAX_CODEPOINT = 0x10FFFF,
};

typedef struct {
    int first;
    int second;
} fmt_Int_Pair;

/// Returns the number of bytes a UTF-8 codepoint takes based on the leading byte.
static int fmt__utf8_codepoint_length(fmt_char8_t leading_byte) {
    const int LOOKUP[] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1,
        2, 2,
        3,
        4
    };
    return LOOKUP[leading_byte >> 4];
}

/// Returns `true` if the given value is a valid Unicode codepoint.
static bool fmt__is_valid_codepoint(char32_t codepoint) {
    const bool is_surrogate = codepoint >= 0xd800 && codepoint <= 0xdfff;
    return !is_surrogate && codepoint <= FMT_MAX_CODEPOINT;
}

/// Returns the display width of a character.
static int fmt__display_width(char32_t ucs) {
    // TODO: full implementaion and only use this reduced one if
    // `FMT_FAST_DISPLAY_WIDTH` is defined.
    // https://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
    /* test for 8-bit control characters */
    if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0))
        return 0;
    return 1 +
    (ucs >= 0x1100 &&
     (ucs <= 0x115f ||                    /* Hangul Jamo init. consonants */
      ucs == 0x2329 || ucs == 0x232a ||
      (ucs >= 0x2e80 && ucs <= 0xa4cf &&
       ucs != 0x303f) ||                  /* CJK ... Yi */
      (ucs >= 0xac00 && ucs <= 0xd7a3) || /* Hangul Syllables */
      (ucs >= 0xf900 && ucs <= 0xfaff) || /* CJK Compatibility Ideographs */
      (ucs >= 0xfe10 && ucs <= 0xfe19) || /* Vertical forms */
      (ucs >= 0xfe30 && ucs <= 0xfe6f) || /* CJK Compatibility Forms */
      (ucs >= 0xff00 && ucs <= 0xff60) || /* Fullwidth Forms */
      (ucs >= 0xffe0 && ucs <= 0xffe6) ||
      (ucs >= 0x20000 && ucs <= 0x2fffd) ||
      (ucs >= 0x30000 && ucs <= 0x3fffd)));
}

/// Decodes 1 codepoint from valid UTF-8 data.  Returns the number of bytes the
/// codepoint uses.
static int fmt__utf8_decode(const fmt_char8_t *data, char32_t *codepoint) {
        if (data[0] < 0x80) {
            *codepoint = data[0];
            return 1;
        } else if (data[0] < 0xe0) {
            *codepoint = ((char32_t)(data[0] & 0x1f) << 6) | (data[1] & 0x3f);
            return 2;
        } else if (data[0] < 0xf0) {
            *codepoint = (((char32_t)(data[0] & 0x0f) << 12)
                         | ((char32_t)(data[1] & 0x3f) << 6)
                         | (data[2] & 0x3f));
            return 3;
        } else {
            *codepoint = (((char32_t)(data[0] & 0x07) << 18)
                         | ((char32_t)(data[1] & 0x3f) << 12)
                         | ((char32_t)(data[2] & 0x3f) << 6)
                         | (data[3] & 0x3f));
            return 4;
        }
}

/// Returns the display width and number of codepoints in a UTF-8 encoded string.
/// If `size` is negative it is determined using `strlen`.
/// If `max_chars_for_width` is non-negative only that many characters are used
/// for the width calculation.
static fmt_Int_Pair fmt__utf8_width_and_length(const char *str, int size, int max_chars_for_width) {
    int width = 0;
    int length = 0;
    if (size < 0) {
        size = strlen(str);
    }
    if (max_chars_for_width < 0) {
        max_chars_for_width = size;
    }
    const fmt_char8_t *p = (const fmt_char8_t *)str;
    const fmt_char8_t *end = p + size;
    char32_t codepoint;
    while (p != end) {
        p += fmt__utf8_decode(p, &codepoint);
        if (max_chars_for_width-- > 0) {
            width += fmt__display_width(codepoint);
        }
        ++length;
    }
    return (fmt_Int_Pair) { width, length };
}

static size_t fmt__utf16_strlen(const char16_t *str) {
    size_t length = 0;
    while (*str++) {
        ++length;
    }
    return length;
}

/// Returns the display width and number of codepoints in a UTF-16 encoded string.
/// If `size` is negative it is determined using `strlen`.
/// If `max_chars_for_width` is non-negative only that many characters are used
/// for the width calculation.
static fmt_Int_Pair fmt__utf16_width_and_length(const char16_t *str, int size, int max_chars_for_width) {
    int width = 0;
    int length = 0;
    if (size < 0) {
        size = fmt__utf16_strlen(str);
    }
    if (max_chars_for_width < 0) {
        max_chars_for_width = size;
    }
    char32_t c;
    while (size --> 0) {
        c = *str++;
        if (c >= 0xD800 && c <= 0xDBFF) {
            c = (c & 0x3FF) << 10;
            c |= (char32_t)*str++ & 0x3FF;
            c += 0x10000;
            --size;
        }
        if (max_chars_for_width-- > 0) {
            width += fmt__display_width(c);
        }
        ++length;
    }
    return (fmt_Int_Pair) { width, length };
}

static size_t fmt__utf32_strlen(const char32_t *str) {
    size_t length = 0;
    while (*str++) {
        ++length;
    }
    return length;
}

static int fmt__utf32_width(const char32_t *str, int size) {
    int width = 0;
    if (size < 0) {
        size = fmt__utf32_strlen(str);
    }
    while (size --> 0) {
        width += fmt__display_width(*str++);
    }
    return width;
}

static int fmt__utf8_encode(char32_t codepoint, char *buf) {
    static const char REPLACEMENT_CHARACTER_UTF8[] = "\xEF\xBF\xBD";
    if (!fmt__is_valid_codepoint(codepoint)) {
        *buf++ = REPLACEMENT_CHARACTER_UTF8[0];
        *buf++ = REPLACEMENT_CHARACTER_UTF8[1];
        *buf++ = REPLACEMENT_CHARACTER_UTF8[2];
        return 3;
    }
    if (codepoint < (1 << 7)) {
        *buf = codepoint;
        return 1;
    }
    if (codepoint < (1 << 11)) {
        *buf++ = 0xC0 | (codepoint >> 6);
        *buf++ = 0x80 | (codepoint & 0x3F);
        return 2;
    }
    if (codepoint < (1 << 16)) {
        *buf++ = 0xE0 | (codepoint >> 12);
        *buf++ = 0x80 | ((codepoint >> 6) & 0x3F);
        *buf++ = 0x80 | (codepoint & 0x3F);
        return 3;
    }
    {
        *buf++ = 0xF0 | (codepoint >> 18);
        *buf++ = 0x80 | ((codepoint >> 12) & 0x3F);
        *buf++ = 0x80 | ((codepoint >> 6) & 0x3F);
        *buf++ = 0x80 | (codepoint & 0x3F);
        return 4;
    }
}

/// `len` is the number of codepoints.
static int fmt__write_utf8(fmt_Writer *writer, const char *str, int len) {
    const char *end = str;
    while (len --> 0) {
        end += fmt__utf8_codepoint_length(*end);
    }
    return writer->write_data(writer, str, end - str);
}

static int fmt__write_codepoint(fmt_Writer *writer, char32_t codepoint) {
    char buf[4];
    const int len = fmt__utf8_encode(codepoint, buf);
    return writer->write_data(writer, buf, len);
}

/// `len` is the number of codepoints.
static int fmt__write_utf16(fmt_Writer *writer, const char16_t *str, int len) {
    char32_t c;
    int written = 0;
    while (len --> 0) {
        c = *str++;
        if (c >= 0xD800 && c <= 0xDBFF) {
            c = (c & 0x3FF) << 10;
            c |= (char32_t)*str++ & 0x3FF;
            c += 0x10000;
            --len;
        }
        written += fmt__write_codepoint(writer, c);
    }
    return written;
}

static int fmt__write_utf32(fmt_Writer *writer, const char32_t *str, int len) {
    int written = 0;
    while (len --> 0) {
        written += fmt__write_codepoint(writer, *str++);
    }
    return written;
}

static int fmt__utf8_chars_len(const char *str, int chars) {
    int length = 0;
    int cp_len;
    while (*str && chars--) {
        cp_len = fmt__utf8_codepoint_length(*str);
        length += cp_len;
        str += cp_len;
    }
    return length;
}

static int fmt__utf16_chars_len(const char16_t *str, int chars) {
    int length = 0;
    while (*str++ && chars--) {
        if (*str >= 0xD800 && *str <= 0xDBFF) {
            ++str;
        }
        ++length;
        ++str;
    }
    return length;
}

static int fmt__utf32_chars_len(const char32_t *str, int chars) {
    int length = 0;
    while (*str++ && chars--) {
        ++length;
    }
    return length;
}

////////////////////////////////////////////////////////////////////////////////
// Format specifiers
////////////////////////////////////////////////////////////////////////////////

typedef enum {
    fmt_ALIGN_LEFT = 1,
    fmt_ALIGN_RIGHT,
    fmt_ALIGN_CENTER,
    fmt_ALIGN_AFTER_SIGN,
} fmt_Alignment;

typedef enum {
    fmt_SIGN_NEGATIVE = 1,
    fmt_SIGN_SPACE,
    fmt_SIGN_ALWAYS,
} fmt_Sign;

// TODO: reorder to minimize padding.
typedef struct {
    char type;
    char fill;
    fmt_Alignment align;
    fmt_Sign sign;
    bool alternate_form;
    bool zero_pad;
    int width;
    char group;
    int precision;
} fmt_Format_Specifier;

static void fmt__format_specifier_default(fmt_Format_Specifier *spec) {
    spec->type = 0;
    spec->fill = ' ';
    spec->align = fmt_ALIGN_LEFT;
    spec->sign = fmt_SIGN_NEGATIVE;
    spec->alternate_form = false;
    spec->zero_pad = false;
    spec->width = 0;
    spec->group = 0;
    spec->precision = -1;
}

static int fmt__parse_alignment(char ch) {
    if (ch == '<') {
        return (int)fmt_ALIGN_LEFT;
    } else if (ch == '>') {
        return (int)fmt_ALIGN_RIGHT;
    } else if (ch == '^') {
        return (int)fmt_ALIGN_CENTER;
    } else if (ch == '=') {
        return (int)fmt_ALIGN_AFTER_SIGN;
    } else {
        return 0;
    }
}

static int fmt__parse_sign(char ch) {
    if (ch == '+') {
        return (int)fmt_SIGN_ALWAYS;
    } else if (ch == ' ') {
        return (int)fmt_SIGN_SPACE;
    } else {
        return 0;
    }
}

/// Parses or gets a parameterized integer for the width and precision fields of
/// the format specifier.
static const char * fmt__parse_int(
    const char *format_specifier,
    const char *what,
    int *out,
    int specifier_number,
    int *arg_count,
    va_list ap
) {
    if (*format_specifier == '{') {
        ++format_specifier;
        if (*format_specifier == '}') {
            ++format_specifier;
            // FIXME: check arg_count
            *out = fmt__va_get_unsigned_integer(ap);
            --*arg_count;
        } else {
            fmt_eprintln(
                "\nMissing } for parameterized {} at format specifier {}",
                what,
                specifier_number
            );
        }
    } else if (isdigit(*format_specifier)) {
        *out = 0;
        int last;
        bool overflow = false;
        while (isdigit(*format_specifier)) {
            last = *out;
            *out *= 10;
            *out += *format_specifier - '0';
            if (*out < last && !overflow) {
                fmt_eprintln(
                    "\nOverflow in {} at format specifier {}",
                    what,
                    specifier_number
                );
                overflow = true;
            }
            ++format_specifier;
        }
        if (overflow) {
            *out = 0;
        }
    }
    return format_specifier;
}

static const char * fmt__parse_specifier(
    const char *format_specifier,
    fmt_Format_Specifier *out,
    fmt_Type_Id type,
    int specifier_number,
    int *arg_count,
    va_list ap
) {
    int parsed;
    fmt__format_specifier_default(out);
    ++format_specifier;  // skip '{'
    if (*format_specifier == '}') {
        return ++format_specifier;
    }
    if (*format_specifier != ':') {
        parsed = *format_specifier++;
        const char *valid = fmt__valid_display_types(type);
        if (strchr(valid, parsed) != NULL) {
            out->type = parsed;
        } else {
            fmt_eprintln("invalid display type '{}' in specifier {}, expected one of: {}", (char)parsed, specifier_number, valid);
        }
    }
    if (*format_specifier == '}') {
        return ++format_specifier;
    } else if (*format_specifier == ':') {
        ++format_specifier;
    } else {
        fmt_panic("expected : or } after type in format specifier {}", specifier_number);
    }
    if ((parsed = fmt__parse_alignment(*format_specifier))) {
        out->align = (fmt_Alignment)parsed;
        ++format_specifier;
    } else if ((parsed = fmt__parse_alignment(format_specifier[1]))) {
        out->align = (fmt_Alignment)parsed;
        out->fill = *format_specifier;
        format_specifier += 2;
    } else if ((parsed = fmt__parse_alignment(format_specifier[2]))
                && format_specifier[0] == '{' && format_specifier[1] == '}') {
        out->align = (fmt_Alignment)parsed;
        // FIXME: check arg_count
        out->fill = fmt__va_get_character(ap);
        --*arg_count;
        format_specifier += 3;
    }
    if ((parsed = fmt__parse_sign(*format_specifier))) {
        out->sign = parsed;
        ++format_specifier;
    }
    if (*format_specifier == '#') {
        out->alternate_form = true;
        ++format_specifier;
    }
    if (*format_specifier == '0') {
        out->zero_pad = true;
        ++format_specifier;
    }
    if (*format_specifier == '{' || isdigit(*format_specifier)) {
        format_specifier = fmt__parse_int(format_specifier, "width", &out->width, specifier_number, arg_count, ap);
    }
    // If we have a . the next charater must also be a . or the end of the
    // specifier for it to be the grouping character.  Anything else in this
    // position must be the grouping character.
    if ((*format_specifier != '.' && *format_specifier != '}')
        || format_specifier[1] == '.' || format_specifier[1] == '}') {
        out->group = *format_specifier;
        ++format_specifier;
    }
    if (*format_specifier == '.') {
        ++format_specifier;
        format_specifier = fmt__parse_int(format_specifier, "precision", &out->precision, specifier_number, arg_count, ap);
    }
    ++format_specifier; // skip '}'
    return format_specifier;
}

#undef READ_ARG

////////////////////////////////////////////////////////////////////////////////
// Auxillary functions for printing functions
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    const char *prefix;
    int prefix_len;
    int base;
    int (*write_digits)(fmt_Writer *writer, uint64_t n, int len);
    int (*write_digits_grouped)(fmt_Writer *writer, uint64_t n, int len, char32_t group_char);
} fmt_Base;

static int fmt__unsigned_width_10(unsigned long long n) {
    if (n < 10ULL) return 1;
    if (n < 100ULL) return 2;
    if (n < 1000ULL) return 3;
    if (n < 10000ULL) return 4;
    if (n < 100000ULL) return 5;
    if (n < 1000000ULL) return 6;
    if (n < 10000000ULL) return 7;
    if (n < 100000000ULL) return 8;
    if (n < 1000000000ULL) return 9;
    if (n < 10000000000ULL) return 10;
    if (n < 100000000000ULL) return 11;
    if (n < 1000000000000ULL) return 12;
    if (n < 10000000000000ULL) return 13;
    if (n < 100000000000000ULL) return 14;
    if (n < 1000000000000000ULL) return 15;
    if (n < 10000000000000000ULL) return 16;
    if (n < 100000000000000000ULL) return 17;
    if (n < 1000000000000000000ULL) return 18;
    if (n < 10000000000000000000ULL) return 19;
    return 20;
}

static int fmt__unsigned_width_16(unsigned long long n) {
    if (n < 0x10ULL) return 1;
    if (n < 0x100ULL) return 2;
    if (n < 0x1000ULL) return 3;
    if (n < 0x10000ULL) return 4;
    if (n < 0x100000ULL) return 5;
    if (n < 0x1000000ULL) return 6;
    if (n < 0x10000000ULL) return 7;
    if (n < 0x100000000ULL) return 8;
    if (n < 0x1000000000ULL) return 9;
    if (n < 0x10000000000ULL) return 10;
    if (n < 0x100000000000ULL) return 11;
    if (n < 0x1000000000000ULL) return 12;
    if (n < 0x10000000000000ULL) return 13;
    if (n < 0x100000000000000ULL) return 14;
    if (n < 0x1000000000000000ULL) return 15;
    return 16;
}

static int fmt__unsigned_width_2(unsigned long long n) {
    if (n == 0) {
        return 1;
    }
    int width = 0;
    while (n) {
        ++width;
        n >>= 1;
    }
    return width;
}

static int fmt__unsigned_width_8(unsigned long long n) {
    if (n == 0) {
        return 1;
    }
    int width = 0;
    while (n) {
        ++width;
        n /= 8;
    }
    return width;
}

static int fmt__unsigned_width(unsigned long long n, int base) {
    switch (base) {
    case 2: return fmt__unsigned_width_2(n);
    case 8: return fmt__unsigned_width_8(n);
    case 10: return fmt__unsigned_width_10(n);
    case 16: return fmt__unsigned_width_16(n);
    default:
        fmt_eprintln("fmt__unsigned_width: invalid base: {}", base);
        exit(1);
    }
}

static int fmt__min(int a, int b) {
    return a < b ? a : b;
}

static void fmt__pad(fmt_Writer *writer, int n, char32_t ch) {
    static char buf[32];
    memset(buf, ch, 32);
    while (n > 0) {
        // TODO: utf-8
        writer->write_data(writer, buf, fmt__min(n, 32));
        n -= 32;
    }
}

static fmt_Int_Pair fmt__distribute_padding(int amount, fmt_Alignment align) {
    fmt_Int_Pair result;
    if (amount <= 0) {
        result.first = 0;
        result.second = 0;
    } else if (align == fmt_ALIGN_RIGHT || align == fmt_ALIGN_AFTER_SIGN) {
        result.first = amount;
        result.second = 0;
    } else if (align == fmt_ALIGN_LEFT) {
        result.first = 0;
        result.second = amount;
    } else {
        const int half = amount / 2;
        result.first = half;
        result.second = amount - half;
    }
    return result;
}

static void fmt__reverse(char *buf, int len) {
    const int mid = len / 2;
    for (int i = 0; i < mid; ++i) {
        char t = buf[i];
        buf[i] = buf[len - 1 - i];
        buf[len - 1 - i] = t;
    }
}

static int fmt__write_grouped(fmt_Writer *writer, const char *buf, int len, char32_t groupchar, int interval) {
    char grouputf8[4];
    const int grouplen = fmt__utf8_encode(groupchar, grouputf8);
    const int offset = len % interval;
    const char *const end = buf + len;
    int written = 0;
    bool skip = true;
    if (offset) {
        written += writer->write_data(writer, buf, offset);
        skip = false;
    }
    for (buf = buf + offset; buf != end; buf += interval) {
        if (skip) {
            skip = false;
        } else {
            written += writer->write_data(writer, grouputf8, grouplen);
        }
        written += writer->write_data(writer, buf, interval);
    }
    return written;
}

#define FMT_DEFINE_WRITE_DIGITS(_name, _div, _buf_size, _grouping_interval, _lookup_string) \
    static int _name(fmt_Writer *writer, uint64_t n, int len) { \
        /* Print 2 digits at a time using this lookup string.  Doing 3 at once
           was actually slightly slower on my machine.  For numbers with an
           uneven amount of digits this will just write an extra `0` into the
           buffer but since we use the previously calculated length for the
           reversing and writing that will just be discarded. */ \
        const char *digitpairs = _lookup_string; \
        /* Initialize the buffer with one zero since the while loop will never
           run if `n` is zero. */ \
        char buf[_buf_size] = {'0'}; \
        char *p = buf; \
        int idx; \
        while (n) { \
            idx = (n % _div) * 2; \
            memcpy(p, digitpairs + idx, 2); \
            p += 2; \
            n /= _div; \
        } \
        fmt__reverse(buf, len); \
        return writer->write_data(writer, buf, len); \
    } \
    static int _name##_grouped(fmt_Writer *writer, uint64_t n, int len, char32_t groupchar) { \
        const char *digitpairs = _lookup_string; \
        char buf[_buf_size] = {'0'}; \
        char *p = buf; \
        int idx; \
        while (n) { \
            idx = (n % _div) * 2; \
            memcpy(p, digitpairs + idx, 2); \
            p += 2; \
            n /= _div; \
        } \
        fmt__reverse(buf, len); \
        return fmt__write_grouped(writer, buf, len, groupchar, _grouping_interval); \
    }

#ifdef FMT_BIN_GROUP_NIBBLES
FMT_DEFINE_WRITE_DIGITS(fmt__write_digits_2, 4, 64, 4,"00100111");
#else
FMT_DEFINE_WRITE_DIGITS(fmt__write_digits_2, 4, 64, 8,"00100111");
#endif

FMT_DEFINE_WRITE_DIGITS(
    fmt__write_digits_10,
    100,
    20,
    3,
    "00102030405060708090"
    "01112131415161718191"
    "02122232425262728292"
    "03132333435363738393"
    "04142434445464748494"
    "05152535455565758595"
    "06162636465666768696"
    "07172737475767778797"
    "08182838485868788898"
    "09192939495969798999"
);

FMT_DEFINE_WRITE_DIGITS(
    fmt__write_digits_8,
    64,
    24,
    // Python uses 4, Rust doesn't even have thousands separators, printf also uses 3.
    3,
    "0010203040506070"
    "0111213141516171"
    "0212223242526272"
    "0313233343536373"
    "0414243444546474"
    "0515253545556575"
    "0616263646566676"
    "0717273747576777"
);

FMT_DEFINE_WRITE_DIGITS(
    fmt__write_digits_16_lower,
    256,
    16,
    4,
    "00102030405060708090a0b0c0d0e0f0"
    "01112131415161718191a1b1c1d1e1f1"
    "02122232425262728292a2b2c2d2e2f2"
    "03132333435363738393a3b3c3d3e3f3"
    "04142434445464748494a4b4c4d4e4f4"
    "05152535455565758595a5b5c5d5e5f5"
    "06162636465666768696a6b6c6d6e6f6"
    "07172737475767778797a7b7c7d7e7f7"
    "08182838485868788898a8b8c8d8e8f8"
    "09192939495969798999a9b9c9d9e9f9"
    "0a1a2a3a4a5a6a7a8a9aaabacadaeafa"
    "0b1b2b3b4b5b6b7b8b9babbbcbdbebfb"
    "0c1c2c3c4c5c6c7c8c9cacbcccdcecfc"
    "0d1d2d3d4d5d6d7d8d9dadbdcdddedfd"
    "0e1e2e3e4e5e6e7e8e9eaebecedeeefe"
    "0f1f2f3f4f5f6f7f8f9fafbfcfdfefff"
);

FMT_DEFINE_WRITE_DIGITS(
    fmt__write_digits_16_upper,
    256,
    16,
    4,
    "00102030405060708090A0B0C0D0E0F0"
    "01112131415161718191A1B1C1D1E1F1"
    "02122232425262728292A2B2C2D2E2F2"
    "03132333435363738393A3B3C3D3E3F3"
    "04142434445464748494A4B4C4D4E4F4"
    "05152535455565758595A5B5C5D5E5F5"
    "06162636465666768696A6B6C6D6E6F6"
    "07172737475767778797A7B7C7D7E7F7"
    "08182838485868788898A8B8C8D8E8F8"
    "09192939495969798999A9B9C9D9E9F9"
    "0A1A2A3A4A5A6A7A8A9AAABACADAEAFA"
    "0B1B2B3B4B5B6B7B8B9BABBBCBDBEBFB"
    "0C1C2C3C4C5C6C7C8C9CACBCCCDCECFC"
    "0D1D2D3D4D5D6D7D8D9DADBDCDDDEDFD"
    "0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFE"
    "0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFFF"
);

#undef FMT_DEFINE_WRITE_DIGITS

static const fmt_Base* fmt__get_base(char type) {
    #define FMT_DEF_BASE(_name, _base, _prefix, _writer_suffix) \
        static const fmt_Base _name = (fmt_Base) { \
            _prefix, \
            sizeof(_prefix)-1, \
            _base, \
            fmt__write_digits_##_writer_suffix, \
            fmt__write_digits_##_writer_suffix##_grouped \
        }
    FMT_DEF_BASE(BASE_2, 2, "0b", 2);
    FMT_DEF_BASE(BASE_8, 8, "0o", 8);
    FMT_DEF_BASE(BASE_10, 10, "", 10);
    FMT_DEF_BASE(BASE_16_LOWER, 16, "0x", 16_lower);
    FMT_DEF_BASE(BASE_16_UPPER, 16, "0X", 16_upper);
    #undef FMT_DEF_BASE
    switch (type) {
    case 0:
    case 'd':
        return &BASE_10;
    case 'x':
        return &BASE_16_LOWER;
    case 'X':
        return &BASE_16_UPPER;
    case 'b':
        return &BASE_2;
    case 'o':
        return &BASE_8;
    default:
        fmt_panic("fmt: invalid type for integer base: {}", type);
    }
}

static unsigned long long fmt__pow(unsigned long long base, unsigned long long exp) {
    // https://stackoverflow.com/a/101613
    unsigned long long result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }
    return result;
}

static void fmt__get_base_and_exponent(double f, double *base, int *exponent) {
    // The iterative version is faster (only tested on 1 machine) but is bound
    // by the value of `d` so for large values we need to fall back to the math
    // solution.
    if (f > 0x1p66) {
        *exponent = (int)log10(f);
        *base = f / pow(10.0, *exponent);
        return;
    }
    double negate = 1.0;
    if (f < 0.0) {
        negate = -1.0;
        f = -f;
    }
    int exp = 0;
    unsigned long long d = 1;
    if (f < 1.0) {
        for (; f * d < 1.0; d *= 10, --exp) {}
        f *= d;
    } else if (f >= 10.0) {
        for (; f / d >= 10.0; d *= 10, ++exp) {}
        f /= d;
    }
    *base = f * negate;
    *exponent = exp;
}

////////////////////////////////////////////////////////////////////////////////
// Printing functions
////////////////////////////////////////////////////////////////////////////////

static int fmt__print_utf8(fmt_Writer *writer, fmt_Format_Specifier *fs, const char *string, int len) {
    const fmt_Int_Pair width_and_length = fmt__utf8_width_and_length(string, len, fs->precision);

    int to_print = width_and_length.second;
    if (fs->precision >= 0 && fs->precision < to_print) {
        to_print = fs->precision;
    }

    fmt_Int_Pair pad = fmt__distribute_padding(fs->width - width_and_length.first, fs->align);
    int written = pad.first + pad.second;

    fmt__pad(writer, pad.first, fs->fill);
    written += fmt__write_utf8(writer, string, to_print);
    fmt__pad(writer, pad.second, fs->fill);

    return written;
}

static int fmt__print_utf16(fmt_Writer *writer, fmt_Format_Specifier *fs, const char16_t *string, int len) {
    const fmt_Int_Pair width_and_length = fmt__utf16_width_and_length(string, len, fs->precision);

    int to_print = width_and_length.second;
    if (fs->precision >= 0 && fs->precision < to_print) {
        to_print = fs->precision;
    }

    fmt_Int_Pair pad = fmt__distribute_padding(fs->width - width_and_length.first, fs->align);
    int written = pad.first + pad.second;

    fmt__pad(writer, pad.first, fs->fill);
    written += fmt__write_utf16(writer, string, to_print);
    fmt__pad(writer, pad.second, fs->fill);

    return written;
}

static int fmt__print_utf32(fmt_Writer *writer, fmt_Format_Specifier *fs, const char32_t *string, int len) {
    const int width = fmt__utf32_width(string, len);

    int to_print = len;
    if (fs->precision >= 0 && fs->precision < to_print) {
        to_print = fs->precision;
    }

    fmt_Int_Pair pad = fmt__distribute_padding(fs->width - width, fs->align);
    int written = pad.first + pad.second;

    fmt__pad(writer, pad.first, fs->fill);
    written += fmt__write_utf32(writer, string, to_print);
    fmt__pad(writer, pad.second, fs->fill);

    return written;
}

static int fmt__print_char(fmt_Writer *writer, fmt_Format_Specifier *fs, char32_t ch) {
    char buf[4];
    const int len = fmt__utf8_encode(ch, buf);
    fs->precision = -1;
    return fmt__print_utf8(writer, fs, buf, len);
}

static int fmt__print_int(fmt_Writer *writer, fmt_Format_Specifier *fs, unsigned long long i, char sign) {
    const fmt_Base *base = fmt__get_base(fs->type);
    const int digits_width = fmt__unsigned_width(i, base->base);
    const int width = digits_width + !!sign;

    fmt_Int_Pair pad = fmt__distribute_padding(fs->width - width, fs->align);

    // Only do zero-padding when aligning to the right, this follows the
    // behaviour of printf.
    fs->zero_pad &= fs->align == fmt_ALIGN_RIGHT;
    const char32_t padchar = fs->zero_pad ? '0' : fs->fill;
    const bool pad_after_sign_and_base = fs->zero_pad || fs->align == fmt_ALIGN_AFTER_SIGN;

    int written = pad.first + pad.second;
    if (!pad_after_sign_and_base) {
        fmt__pad(writer, pad.first, padchar);
    }
    if (sign) {
        written += writer->write_byte(writer, sign);
    }
    if (fs->alternate_form) {
        written += writer->write_data(writer, base->prefix, base->prefix_len);
    }
    if (pad_after_sign_and_base) {
        fmt__pad(writer, pad.first, padchar);
    }
    if (fs->group) {
        written += base->write_digits_grouped(writer, i, digits_width, fs->group);
    } else {
        written += base->write_digits(writer, i, digits_width);
    }
    fmt__pad(writer, pad.second, padchar);

    return written;
}

static int fmt__print_bool(fmt_Writer *writer, fmt_Format_Specifier *fs, bool b) {
#ifdef FMT_BOOL_YES_NO
    static const char *STRINGS[] = {"no", "yes"};
    static const int LEN[] = {2, 3};
#else
    static const char *STRINGS[] = {"false", "true"};
    static const int LEN[] = {5, 4};
#endif
    return fmt__print_utf8(writer, fs, STRINGS[b], LEN[b]);
}

#define FMT__FLOAT_SPECIAL_CASES()                                                   \
    do {                                                                             \
        if (isinf(f)) {                                                              \
            const char *const s = isupper(fs->type) ? FMT_UPPER_INF : FMT_LOWER_INF; \
            return fmt__print_utf8(writer, fs, s, strlen(s));                        \
        }                                                                            \
        if (isnan(f)) {                                                              \
            const char *const s = isupper(fs->type) ? FMT_UPPER_NAN : FMT_LOWER_NAN; \
            return fmt__print_utf8(writer, fs, s, strlen(s));                        \
        }                                                                            \
    } while(0)

static int fmt__print_float_decimal(fmt_Writer *writer, fmt_Format_Specifier *fs, double f) {
    FMT__FLOAT_SPECIAL_CASES();
    char sign = 0;
    int integer_width, fraction_width = 0;
    unsigned long long integer, fraction;
    bool no_fraction = false;
    if (f < 0.0) {
        sign = '-';
        f = -f;
    }
    integer = (unsigned long long)f;
    integer_width = fmt__unsigned_width_10(integer);
    // TODO: macro option for fixed number of decimal digits like printf does
    if (fs->precision > 0) {
        // FIXME: catch too large precision values
        double unused;
        double f_fraction = modf(f, &unused);
        unsigned long long d = fmt__pow(10, fs->precision);
        fraction = (unsigned long long)(f_fraction * d);
        fraction_width = fs->precision;
    } else if (fs->precision == 0) {
        no_fraction = true;
        // TODO: remove once sure we don't accidentally use it anyways
        fraction = (unsigned long long)-1;
    } else {
        double unused;
        double f_fraction = modf(f, &unused);
        // use an integer and multiply the float once each time to reduce inaccuracy,
        // compared to multiplying the float by 10.0 each iteration.
        unsigned long long d;
        // I don't want to compile with -lm every time so we need to avoid the
        // existing math functions :(
        // Not sure what this means for speed but that's not the goal of this
        // library anyways as long as it's fast enough.
        #define fmt__round(x) ((double)(unsigned long long)(x))
        for (d = 1; f_fraction*d != fmt__round(f_fraction*d); d *= 10) {
            ++fraction_width;
        }
        #undef fmt__round
        fraction = (unsigned long long)(f_fraction * d);
        if (fraction == 0) {
            fraction_width = 1;
        }
    }

    int total_width = !!sign + integer_width + !no_fraction * (1 + fraction_width);
    fmt_Int_Pair pad = fmt__distribute_padding(fs->width - total_width, fs->align);

    fs->zero_pad &= fs->align == fmt_ALIGN_RIGHT;
    const char32_t padchar = fs->zero_pad ? '0' : fs->fill;
    const bool pad_after_sign_and_base = fs->zero_pad || fs->align == fmt_ALIGN_AFTER_SIGN;

    int written = pad.first + pad.second;
    if (!pad_after_sign_and_base) {
        fmt__pad(writer, pad.first, padchar);
    }
    if (sign) {
        written += writer->write_byte(writer, sign);
    }
    if (pad_after_sign_and_base) {
        fmt__pad(writer, pad.first, padchar);
    }
    if (fs->group && false) {
        // TODO
    } else {
        written += fmt__write_digits_10(writer, integer, integer_width);
    }
    if (!no_fraction) {
        // TODO: locale?
        written += writer->write_byte(writer, '.');
        written += fmt__write_digits_10(writer, fraction, fraction_width);
    }
    fmt__pad(writer, pad.second, padchar);

    return written;
}

static int fmt__print_float_exponential(fmt_Writer *writer, fmt_Format_Specifier *fs, double f) {
    FMT__FLOAT_SPECIAL_CASES();
    int exp;
    fmt__get_base_and_exponent(f, &f, &exp);
    int written = fmt__print_float_decimal(writer, fs, f);
    written += writer->write_byte(writer, 'e');
    fs->width = 2 + (exp < 0);
    fs->zero_pad = true;
    fs->align = fmt_ALIGN_RIGHT;
    fs->type = 'd';
    if (exp < 0) {
        written += fmt__print_int(writer, fs, -exp, '-');
    } else {
        written += fmt__print_int(writer, fs, exp, 0);
    }
    return written;
}

#undef FMT__FLOAT_SPECIAL_CASES

static int fmt__print_pointer(fmt_Writer *writer, fmt_Format_Specifier *fs, const void *ptr) {
    fs->type = fs->type == 'P' ? 'X' : 'x';
    return fmt__print_int(writer, fs, (uintptr_t)ptr, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Core functions
////////////////////////////////////////////////////////////////////////////////

#ifdef FMT_LOCKED_DEFAULT_PRINTERS
static _Atomic bool fmt__mutex_initialized;
static mtx_t fmt__print_mutex;

void fmt_init_threading() {
    mtx_init(&fmt__print_mutex);
    atexit(fmt_clean_mutex);
}

void fmt_clean_mutex() {
    if (fmt__mutex_initialized) {
        mtx_destroy(&fmt__print_mutex);
    }
}
#endif

static int fmt__print_specifier(fmt_Writer *writer, const char **format_specifier, int *arg_count, int specifier_number, va_list ap) {
    if (*arg_count == 0) {
        fmt_panic("\nArguments exhausted at specifier {}", specifier_number);
    }
    --*arg_count;
    fmt_Type_Id type = va_arg(ap, fmt_Type_Id);
    fmt_Format_Specifier fs;
    union {
        const char *v_string;
        const char16_t *v_string16;
        const char32_t *v_string32;
        long long v_signed;
        unsigned long long v_unsigned;
        bool v_bool;
        double v_float;
        const void *v_pointer;
        struct tm v_time;
    } value;
    int length = 0;
    char sign = 0;

    #define FMT_PARSE_FS()                                                \
        *format_specifier = fmt__parse_specifier(                         \
            *format_specifier, &fs, type, specifier_number, arg_count, ap \
        )

    #define FMT_TID_CASE(_tid, _v, _T, _a) \
        case _tid: { \
            value._v = va_arg(ap, _T); \
            FMT_PARSE_FS(); \
            goto _a; \
        }

    switch (type) {
        case fmt__TYPE_STRING:
            value.v_pointer = va_arg(ap, const char *);
            sign = 0;
            // we could do this after the t_string label but I'll keep it up
            // here for consistency
            FMT_PARSE_FS();
            goto t_string;
        case fmt__TYPE_STRING_16:
            value.v_pointer = va_arg(ap, const char16_t *);
            sign = 1;
            FMT_PARSE_FS();
            goto t_string;
        case fmt__TYPE_STRING_32:
            value.v_pointer = va_arg(ap, const char32_t *);
            sign = 2;
            FMT_PARSE_FS();
            goto t_string;

        case fmt__TYPE_CHAR: {
            int my_value = va_arg(ap, int);
            if (my_value < 0) {
                value.v_unsigned = FMT_REPLACEMENT_CHARACTER;
            } else {
                value.v_unsigned = my_value;
            }
            FMT_PARSE_FS();
            if (fs.type == 0) {
                fs.type = 'c';
            }
            goto t_unsigned;
        }

        // Note: cases reading differnt types from the variadic arguments than
        // their type ID specifier are not errors but those values are promoted
        // to the used type when passed through `...`.

        FMT_TID_CASE(fmt__TYPE_UNSIGNED_CHAR, v_unsigned, unsigned, t_unsigned)
        FMT_TID_CASE(fmt__TYPE_UNSIGNED_SHORT, v_unsigned, unsigned, t_unsigned)
        FMT_TID_CASE(fmt__TYPE_UNSIGNED, v_unsigned, unsigned, t_unsigned)
        FMT_TID_CASE(fmt__TYPE_UNSIGNED_LONG, v_unsigned, unsigned long, t_unsigned)
        FMT_TID_CASE(fmt__TYPE_UNSIGNED_LONG_LONG, v_unsigned, unsigned long long, t_unsigned)

        FMT_TID_CASE(fmt__TYPE_SIGNED_CHAR, v_signed, int, t_signed)
        FMT_TID_CASE(fmt__TYPE_SHORT, v_signed, int, t_signed)
        FMT_TID_CASE(fmt__TYPE_INT, v_signed, int, t_signed)
        FMT_TID_CASE(fmt__TYPE_LONG, v_signed, long, t_signed)
        FMT_TID_CASE(fmt__TYPE_LONG_LONG, v_signed, long long, t_signed)

        FMT_TID_CASE(fmt__TYPE_BOOL, v_bool, int, t_bool)

        FMT_TID_CASE(fmt__TYPE_POINTER, v_pointer, const void *, t_pointer);

        FMT_TID_CASE(fmt__TYPE_FLOAT, v_float, double, t_float)
        FMT_TID_CASE(fmt__TYPE_DOUBLE, v_float, double, t_float)

        case fmt__TYPE_UNKNOWN:
            // Unknown is also used for all pointers we don't specify an explicit
            // type id for so we don't need to cast to a void pointer every time.
            // This is of course less safe as it could also not be a pointer but
            // that would be the users fault.
            if ((*format_specifier)[1] == 'p' || (*format_specifier)[1] == 'P') {
                value.v_pointer = va_arg(ap, const void *);
                FMT_PARSE_FS();
                goto t_pointer;
            }
            fmt_panic("Unimplemented argument type at specifier {}", specifier_number);
    }

    #undef FMT_PARSE_FS
    #undef FMT_TID_CASE

    fmt_panic("unreachable");

t_string:
    // All string functions have the same general interface and just differ in
    // in the type of pointer the functions take but since they are all pointers
    // we can just cast them to be void pointers and choose the functions based
    // on the kind of string determined in the switch above.
    if (fs.type == 'p' || fs.type == 'P') {
        goto t_pointer;
    } else {
        static size_t (*STRLEN_TABLE[])(const void *) = {
            (size_t (*)(const void *))strlen,
            (size_t (*)(const void *))fmt__utf16_strlen,
            (size_t (*)(const void *))fmt__utf32_strlen,
        };
        static int (*CHARS_LEN_TABLE[])(const void *, int) = {
            (int (*)(const void *, int))fmt__utf8_chars_len,
            (int (*)(const void *, int))fmt__utf16_chars_len,
            (int (*)(const void *, int))fmt__utf32_chars_len,
        };
        static int (*PRINT_TABLE[])(fmt_Writer *, fmt_Format_Specifier *, const void *, int) = {
            (int (*)(fmt_Writer *, fmt_Format_Specifier *, const void *, int))fmt__print_utf8,
            (int (*)(fmt_Writer *, fmt_Format_Specifier *, const void *, int))fmt__print_utf16,
            (int (*)(fmt_Writer *, fmt_Format_Specifier *, const void *, int))fmt__print_utf32,
        };
        if (fs.precision < 0) {
            length = STRLEN_TABLE[(int)sign](value.v_pointer);
        } else {
            length = CHARS_LEN_TABLE[(int)sign](value.v_pointer, fs.precision);
        }
        return PRINT_TABLE[(int)sign](writer, &fs, value.v_pointer, length);
    }

t_signed:
    if (value.v_signed < 0) {
        sign = '-';
        value.v_unsigned = -value.v_signed;
    }
    /* fallthrough */

t_unsigned:
    if (fs.type == 'c') {
        return fmt__print_char(writer, &fs, value.v_unsigned);
    } else {
        return fmt__print_int(writer, &fs, value.v_unsigned, sign);
    }

t_pointer:
    return fmt__print_pointer(writer, &fs, value.v_pointer);

t_float:
    if (fs.type == 'e' || fs.type == 'E') {
        return fmt__print_float_exponential(writer, &fs, value.v_float);
    } else {
        return fmt__print_float_decimal(writer, &fs, value.v_float);
    }

t_bool:
    return fmt__print_bool(writer, &fs, value.v_bool);
}

int fmt__with_writer(fmt_Writer *writer, const char *format, int arg_count, ...) {
    va_list ap;
    va_start(ap, arg_count);
    const int written = fmt_implementation(writer, format, arg_count, ap);
    va_end(ap);
    return written;
}

int fmt__default_printer(FILE *stream, const char *format, bool newline, int arg_count, ...) {
#ifdef FMT_LOCKED_DEFAULT_PRINTERS
    if (!fmt__mutex_initialized) {
        mtx_init(&fmt__print_mutex, mtx_plain);
    }
    mtx_lock(&fmt__print_mutex);
#endif
    fmt_Writer *writer = FMT_NEW_STREAM_WRITER(stream);
    va_list ap;
    va_start(ap, arg_count);
    const int written = fmt_implementation(writer, format, arg_count, ap) + newline;
    if (newline) {
        writer->write_byte(writer, '\n');
    }
    va_end(ap);
#ifdef FMT_LOCKED_DEFAULT_PRINTERS
    mtx_unlock(&fmt__print_mutex);
#endif
    return written;
}

int fmt_implementation(fmt_Writer *writer, const char *format, int arg_count, va_list ap) {
    int written = 0;
    const char *open_bracket = format;
    int specifier_number = 1;
    while (open_bracket) {
        if ((open_bracket = strchr(format, '{')) != NULL) {
            written += writer->write_data(writer, format, open_bracket - format);
            if (open_bracket[1] == '{') {
                format = open_bracket;
            } else {
                written += fmt__print_specifier(writer, &open_bracket, &arg_count, specifier_number++, ap);
                format = open_bracket;
            }
        } else if (*format) {
            written += writer->write_str(writer, format);
        }
    }
    return written;
}

// We need a 2nd function for this so we can use variadic arguments
static void fmt__panic_loc(fmt_Writer *writer, ...) {
    va_list ap;
    va_start(ap, writer);
    fmt_implementation(writer, "{}:{}: ", 2, ap);
    va_end(ap);
}

void fmt__panic(const char *file, int line, const char *format, int arg_count, ...) {
    fmt_Writer *writer = FMT_NEW_STREAM_WRITER(stderr);
    fmt__panic_loc(writer, fmt__TYPE_STRING, file, fmt__TYPE_INT, line);
    va_list ap;
    va_start(ap, arg_count);
    fmt_implementation(writer, format, arg_count, ap);
    va_end(ap);
    if (format[strlen(format) - 1] != '\n') {
        writer->write_byte(writer, '\n');
    }
    abort();
}

#endif /* FMT_IMPLEMENTATION */

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2023 Jakob Mohrbacher
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
