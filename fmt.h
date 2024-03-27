/* https://github.com/JaMo42/fmt */
#ifndef FMT_H
#define FMT_H

#ifdef _MSC_VER
#  define FMT_NO_LANGINFO
#endif

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
#ifndef _WIN32
#  include <threads.h>
#  define FMT__THREAD_LOCAL thread_local
#else
#  ifdef FMT_LOCKED_DEFAULT_PRINTERS
#    undef FMT_LOCKED_DEFAULT_PRINTERS
#    error "FMT_LOCKED_DEFAULT_PRINTERS not supported because <threads.h> is not implemented on this platform"
#  endif
#  if __STDC_VERSION__ <= 201710L
#    define FMT__THREAD_LOCAL _Thread_local
#  else
#    define FMT__THREAD_LOCAL thread_local
#  endif
#endif
#include <uchar.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#ifndef FMT_NO_LANGINFO
#  include <langinfo.h>
#endif
#include <wctype.h>

// `char8_t` is only available since C23 but we also want to support C11 so
// since we need our typedef anyways we never use `char8_t` since we wouldn't
// want to use that name for the C11 version.
typedef uint_least8_t fmt_char8_t;

#if defined(__cplusplus) || __STDC_VERSION__ > 201710L
#  define FMT__NORETURN [[noreturn]]
#  define FMT__STATIC_ASSERT static_assert
#else
#  define FMT__NORETURN _Noreturn
#  define FMT__STATIC_ASSERT _Static_assert
#endif

#ifndef FMT_DEFAULT_TIME_FORMAT
#  define FMT_DEFAULT_TIME_FORMAT "{a} {b} {d:0} {H}:{M}:{S} {Y}"
#endif

// Detecting empty integer macro: https://stackoverflow.com/a/48540034
#if defined(FMT_DEFAULT_FLOAT_PRECISION) \
    && ((0 - FMT_DEFAULT_FLOAT_PRECISION - 1) == 1 \
    && (FMT_DEFAULT_FLOAT_PRECISION + 0) != -2)
#  error "FMT_DEFAULT_FLOAT_PRECISION is empty, reverting to default"
#  undef FMT_DEFAULT_FLOAT_PRECISION
#endif
#ifndef FMT_DEFAULT_FLOAT_PRECISION
#  define FMT_DEFAULT_FLOAT_PRECISION 3
#endif

#if defined(FMT_BUFFERED_WRITER_CAPACITY) \
    && ((0 - FMT_BUFFERED_WRITER_CAPACITY - 1) == 1 \
    && (FMT_BUFFERED_WRITER_CAPACITY + 0) != -2)
#  error "FMT_BUFFERED_WRITER_CAPACITY is empty, reverting to default"
#  undef FMT_BUFFERED_WRITER_CAPACITY
#endif
#ifndef FMT_BUFFERED_WRITER_CAPACITY
#  define FMT_BUFFERED_WRITER_CAPACITY 32
#endif

#define FMT_TIME_DELIM '%'

// Apparently C++ doesn't have this.
#if defined(__cplusplus) && !defined(restrict)
#  define FMT__MY_RESTRICT
#  define restrict
#endif

// _WIN32 may not be the correct check here, only tested with clang 16.0.5 on
// Windows.  On that compiler we can't just copy the `va_list`s as that would
// keep giving us the first argument over and over.  On linux however we can't
// take the address of a `va_list`.
#ifdef _WIN32
typedef va_list *fmt__va_list_ref;
#define FMT__VA_LIST_REF(ap) &ap
#define FMT__VA_LIST_DEREF(ap) *ap
#else
typedef va_list fmt__va_list_ref;
#define FMT__VA_LIST_REF(ap) ap
#define FMT__VA_LIST_DEREF(ap) ap
#endif

////////////////////////////////////////////////////////////////////////////////
// Recursive macros
////////////////////////////////////////////////////////////////////////////////

#define FMT__CAT(a, b) FMT__CAT_1(a, b)
#define FMT__CAT_1(a, b) a##b

#define FMT__FOR_EACH_1(what, x) what(x)
// no clue why the __VA_OPT__ is needed but without it FMT_ARGS breaks with
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
#define FMT_VA_ARG_COUNT(...) (0 __VA_OPT__(+ FMT__FOR_EACH_NARG(__VA_ARGS__)))

////////////////////////////////////////////////////////////////////////////////
// Types IDs
////////////////////////////////////////////////////////////////////////////////

typedef enum {
    fmt__TYPE_UNKNOWN,
    fmt__TYPE_CHAR,
    fmt__TYPE_WCHAR,
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
    fmt__TYPE_TIME,
    fmt__TYPE_FMT_STRING,
    fmt__TYPE_FMT_STRING_TAKE,
    fmt__TYPE_ID_COUNT,
} fmt_Type_Id;

static const char *fmt_Type_Names[fmt__TYPE_ID_COUNT] = {
    "(unknown)",
    "char",
    "wchar_t",
    "signed char",
    "short",
    "int",
    "long",
    "long long",
    "unsigned char",
    "unsigned short",
    "unsigned",
    "unsigned long",
    "unsigned long long",
    "float",
    "double",
    "bool",
    "char *", // shared with `char8_t *`
    "char16_t *",
    "char32_t *",
    "void *",
    "struct tm *",
    "fmt_String",
    "fmt_String",
};

#ifdef __cplusplus

// Need to define fmt_String here so we can use it to overload the type id function.

struct fmt_String_Take {
    char *data;
    size_t size;
};

struct fmt_String {
    union {
        fmt_String_Take take;
        struct {
            char *data;
            size_t size;
            size_t capacity;
        };
    };
};

#define FMT_TYPE_ID(_T, _id) \
    static constexpr fmt_Type_Id FMT__TYPE_ID([[maybe_unused]] _T _) { return _id; }
FMT_TYPE_ID(char, fmt__TYPE_CHAR);
FMT_TYPE_ID(char16_t, fmt__TYPE_UNSIGNED_SHORT;)
FMT_TYPE_ID(char32_t, fmt__TYPE_UNSIGNED;)
FMT_TYPE_ID(wchar_t, fmt__TYPE_WCHAR);
FMT_TYPE_ID(signed char, fmt__TYPE_SIGNED_CHAR);
FMT_TYPE_ID(short, fmt__TYPE_SHORT);
FMT_TYPE_ID(int, fmt__TYPE_INT);
FMT_TYPE_ID(long, fmt__TYPE_LONG);
FMT_TYPE_ID(long long, fmt__TYPE_LONG_LONG);
FMT_TYPE_ID(unsigned char, fmt__TYPE_UNSIGNED_CHAR);
FMT_TYPE_ID(unsigned short, fmt__TYPE_UNSIGNED_SHORT);
FMT_TYPE_ID(unsigned, fmt__TYPE_UNSIGNED);
FMT_TYPE_ID(unsigned long, fmt__TYPE_UNSIGNED_LONG);
FMT_TYPE_ID(unsigned long long, fmt__TYPE_UNSIGNED_LONG_LONG);
FMT_TYPE_ID(float, fmt__TYPE_FLOAT);
FMT_TYPE_ID(double, fmt__TYPE_DOUBLE);
FMT_TYPE_ID(bool, fmt__TYPE_BOOL);
FMT_TYPE_ID(char *, fmt__TYPE_STRING);
FMT_TYPE_ID(const char *, fmt__TYPE_STRING);
FMT_TYPE_ID(fmt_char8_t *, fmt__TYPE_STRING);
FMT_TYPE_ID(const fmt_char8_t *, fmt__TYPE_STRING);
FMT_TYPE_ID(const char16_t *, fmt__TYPE_STRING_16);
FMT_TYPE_ID(char16_t *, fmt__TYPE_STRING_16);
FMT_TYPE_ID(const char32_t *, fmt__TYPE_STRING_32);
FMT_TYPE_ID(char32_t *, fmt__TYPE_STRING_32);
#ifdef _WIN32
FMT_TYPE_ID(wchar_t *, fmt__TYPE_STRING_16);
FMT_TYPE_ID(const wchar_t *, fmt__TYPE_STRING_16);
#else
FMT_TYPE_ID(wchar_t *, fmt__TYPE_STRING_32);
FMT_TYPE_ID(const wchar_t *, fmt__TYPE_STRING_32);
#endif
FMT_TYPE_ID(void *, fmt__TYPE_POINTER);
FMT_TYPE_ID(const void *, fmt__TYPE_POINTER);
FMT_TYPE_ID(tm *, fmt__TYPE_TIME);
FMT_TYPE_ID(const tm *, fmt__TYPE_TIME);
FMT_TYPE_ID(fmt_String, fmt__TYPE_FMT_STRING);
FMT_TYPE_ID(fmt_String_Take, fmt__TYPE_FMT_STRING_TAKE);
template<class Else>
FMT_TYPE_ID(Else, fmt__TYPE_UNKNOWN);
#undef FMT_TYPE_ID

#else // __cplusplus

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
        fmt_char8_t *: fmt__TYPE_STRING, \
        const fmt_char8_t *: fmt__TYPE_STRING, \
        const char16_t *: fmt__TYPE_STRING_16, \
        char16_t *: fmt__TYPE_STRING_16, \
        const char32_t *: fmt__TYPE_STRING_32, \
        char32_t *: fmt__TYPE_STRING_32, \
        /* These match wchar_t strings on linux, on windows they are already
           matched by char16_t. */ \
        int *: fmt__TYPE_STRING_32, \
        const int*: fmt__TYPE_STRING_32, \
        void *: fmt__TYPE_POINTER, \
        const void *: fmt__TYPE_POINTER, \
        struct tm *: fmt__TYPE_TIME, \
        const struct tm *: fmt__TYPE_TIME, \
        fmt_String: fmt__TYPE_FMT_STRING, \
        struct fmt_String_Take: fmt__TYPE_FMT_STRING_TAKE, \
        default: fmt__TYPE_UNKNOWN \
    )

#endif // __cplusplus

#define FMT__TYPE_ID_AND_VALUE(x) FMT__TYPE_ID(x), (x)

/// Turns a list of parameters into a list of their type IDs and the parameter:
/// `param1, param2, param3` -> `type1, param1, type2, param2, type3, param3`
/// WARNING: does not handle 0 parameters and must be wrapped in __VA_OPT__ by
/// by the calling macro.
#define FMT_ARGS(...) FMT__FOR_EACH(FMT__TYPE_ID_AND_VALUE, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// Writer interface
////////////////////////////////////////////////////////////////////////////////

/// "vtable" for writers
typedef struct fmt_Writer {
    /// Writes a single byte
    int (*const write_byte)(
        struct fmt_Writer *self, char byte
    );
    /// Writes `n` bytes of `data`
    int (*const write_data)(
        struct fmt_Writer *restrict self, const char *restrict data, size_t n
    );
    /// Writes `str` until a null-byte is encountered
    int (*const write_str)(
        struct fmt_Writer *restrict self, const char *restrict str
    );
} fmt_Writer;

typedef struct {
    const fmt_Writer base;
    FILE *stream;
} fmt_Stream_Writer;

typedef struct {
    const fmt_Writer base;
    const char *const string;
    char *at;
    const char *const end;
} fmt_String_Writer;

#ifndef __cplusplus
typedef struct {
    union {
        /// Proxy type what when passed to a fmt function will cause the `data`
        /// member to be free'd after printing it.
        struct fmt_String_Take {
            char *data;
            size_t size;
        } take;

        struct {
            char *data;
            size_t size;
            size_t capacity;
        };
    };
} fmt_String;
#endif

FMT__STATIC_ASSERT(
    sizeof(fmt_String) == sizeof(char *) + 2 * sizeof(size_t),
    "fmt_String is too large"
);

typedef struct {
    const fmt_Writer base;
    fmt_String string;
} fmt_Allocating_String_Writer;

/// Writer that discards the data written but keeps track of the number of
/// bytes, number of codepoints, and display width of the data.
typedef struct {
    const fmt_Writer base;
    size_t bytes;
    size_t characters;
    size_t width;
} fmt_Metric_Writer;

/// Wraps another writer and only writes up to a limited number of codepoints.
typedef struct {
    const fmt_Writer base;
    fmt_Writer *inner;
    int characters_left;
} fmt_Limited_Writer;

/// Buffers the data written to it and only writes it to the inner writer when
/// the buffer is full or when flushed.  You must always call `fmt_bw_flush`
/// after using this writer to ensure all data is written.  Note that the
/// buffer is rather small and stored in-line inside the struct.
typedef struct {
    const fmt_Writer base;
    union {
        fmt_Writer *inner;
        // For the case of being used for writing to a stream, so creating an
        // extra stream writer and keeping it in scope for the lifetime of the
        // buffered writer can be avoided.  The `is_stream` flag would likely
        // be padding bytes anyways so there is no space overhead.
        FILE *stream;
    };
    char buffer[FMT_BUFFERED_WRITER_CAPACITY];
    uint8_t used;
    bool is_stream;
} fmt_Buffered_Writer;

/// Default implementation for the writers `write_str` function which just
/// calls `write_data` with the string and its length using `strlen`.
int fmt__write_any_str(fmt_Writer *restrict writer, const char *restrict str);

int fmt__write_stream_byte(fmt_Writer *p_self, char byte);
int fmt__write_stream_data(
    fmt_Writer *restrict p_self, const char *restrict data, size_t n
);

int fmt__write_string_byte(fmt_Writer *p_self, char byte);
int fmt__write_string_data(
    fmt_Writer *restrict p_self, const char *restrict data, size_t n
);

int fmt__write_alloc_byte(fmt_Writer *p_self, char byte);
int fmt__write_alloc_data(
    fmt_Writer *restrict p_self, const char *restrict data, size_t n
);

int fmt__write_metric_byte(fmt_Writer *p_self, char byte);
int fmt__write_metric_data(
    fmt_Writer *restrict p_self, const char *restrict data, size_t n
);

int fmt__write_limited_byte(fmt_Writer *p_self, char byte);
int fmt__write_limited_data(
    fmt_Writer *restrict p_self, const char *restrict data, size_t n
);

int fmt__write_buffered_byte(fmt_Writer *p_self, char byte);
int fmt__write_buffered_data(
    fmt_Writer *restrict p_self, const char *restrict data, size_t n
);

static const fmt_Writer fmt_STREAM_WRITER_FUNCTIONS = {
    .write_byte = fmt__write_stream_byte,
    .write_data = fmt__write_stream_data,
    .write_str = fmt__write_any_str,
};

static const fmt_Writer fmt_STRING_WRITER_FUNCTIONS = {
    .write_byte = fmt__write_string_byte,
    .write_data = fmt__write_string_data,
    .write_str = fmt__write_any_str,
};

static const fmt_Writer fmt_ALLOC_WRITER_FUNCTIONS = {
    .write_byte = fmt__write_alloc_byte,
    .write_data = fmt__write_alloc_data,
    .write_str = fmt__write_any_str,
};

static const fmt_Writer fmt_METRIC_WRITER_FUNCTIONS = {
    .write_byte = fmt__write_metric_byte,
    .write_data = fmt__write_metric_data,
    .write_str = fmt__write_any_str,
};

static const fmt_Writer fmt_LIMITED_WRITER_FUNCTIONS = {
    .write_byte = fmt__write_limited_byte,
    .write_data = fmt__write_limited_data,
    .write_str = fmt__write_any_str,

};

static const fmt_Writer fmt_BUFFERED_WRITER_FUNCTIONS = {
    .write_byte = fmt__write_buffered_byte,
    .write_data = fmt__write_buffered_data,
    .write_str = fmt__write_any_str,
};

// Note: these macros don't work in C++ because you can't take the address of
// an rvalue.

/// Creates a `fmt_Stream_Writer` on the stack and returns it's address as a
/// `fmt_Writer *`.
#define FMT_NEW_STREAM_WRITER(_stream)       \
    ((fmt_Writer *)&(fmt_Stream_Writer){     \
        .base = fmt_STREAM_WRITER_FUNCTIONS, \
        .stream = (_stream),                 \
    })

/// Creates a `fmt_String_Writer` on the stack and returns it's address as a
/// `fmt_Writer *`.
#define FMT_NEW_STRING_WRITER(_string, _n)   \
    ((fmt_Writer *)&(fmt_String_Writer) {    \
        .base = fmt_STRING_WRITER_FUNCTIONS, \
        .string = (_string),                 \
        .at = (_string),                     \
        .end = (_string) + (_n),             \
    })

fmt_Buffered_Writer fmt_bw_new(fmt_Writer *inner);

fmt_Buffered_Writer fmt_bw_new_stream(FILE *stream);

void fmt_bw_flush(fmt_Buffered_Writer *bw);

////////////////////////////////////////////////////////////////////////////////
// Core functions
////////////////////////////////////////////////////////////////////////////////

extern void fmt_init_threading();

/// The core function of this library that all other formatting functions/macros
/// eventually call.
///
/// `ap` should contain pairs of type IDs and arguments.
/// `arg_count` is the number of those pairs.
extern int fmt_va_write(
    fmt_Writer *restrict writer,
    const char *restrict format,
    int arg_count,
    va_list ap
);

/// Implementation for the `fmt_write` macro.
///
/// Examples:
/// ```c
/// FILE *log_file = fopen("log.txt", "w");
/// fmt_Writer *log_writer = FMT_NEW_STREAM_WRITER(log_file);
/// #define log(_format, ...)
///     fmt__write(
///         log_writer,
///         "{}: " _format,
///         1 + FMT_VA_ARG_COUNT(__VA_ARGS__),
///         current_time()
///         __VA_OPT__(, FMT_ARGS(__VA_ARGS__)
///     )
///
/// log("something {}", "happened");
/// ```
/// (note that this macro would need backslashes for multiple lines).
extern int fmt__write(
    fmt_Writer *restrict writer, const char *restrict format, int arg_count, ...
);

/// Implementation for the stdout and stderr printers which handles locking and
/// adding a newline for the `-ln` variants.  It takes a stream because stdout
/// and stderr are not const so we can't easily use static variables for them.
extern int fmt__std_print(
    FILE *restrict stream,
    const char *restrict format,
    bool newline,
    int arg_count,
    ...
);

/// Implementation for the `fmt_panic` macro.
FMT__NORETURN extern void fmt__panic(
    const char *restrict file,
    int line,
    const char *restrict format,
    int arg_count,
    ...
);

/// Like fmt__format but takes a `va_list`, see `fmt_va_write`.
extern fmt_String fmt_va_format(const char *format, int arg_count, va_list ap);

/// Implementation for the `fmt_format` macro.
extern fmt_String fmt__format(const char *format, int arg_count, ...);

/// Like fmt__sprint but takes a `va_list`, see `fmt_va_write`.
extern int fmt_va_sprint(
    char *restrict string,
    size_t size,
    const char *restrict format,
    int arg_count,
    va_list ap
);

/// Implementation for the `fmt_sprint` macro, this only exists for C++ builds
/// which prevent usage of the `FMT_NEW_STRING_WRITER` macro.
extern int fmt__sprint(
    char *restrict string,
    size_t size,
    const char *restrict format,
    int arg_count,
    ...
);

/// Like fmt__fprint but takes a `va_list`, see `fmt_va_write`.
extern int fmt_va_fprint(
    FILE *restrict stream, const char *restrict format, int arg_count, va_list ap
);

/// Implementation for the `fmt_fprint` macro.
extern int fmt__fprint(
    FILE *restrict stream, const char *restrict format, int arg_count, ...
);

/// Formats the broken-down time `datetime` per the specified format string.
extern int fmt_write_time(
    fmt_Writer *restrict writer,
    const char *restrict format,
    const struct tm *restrict datetime
);

/// Like `fmt_write_time` with a string writer, but unlike `fmt_write_time` this
/// will also add a null terminator.
extern int fmt_format_time_to(
    char *restrict buf,
    size_t size,
    const char *restrict format,
    const struct tm *restrict datetime
);

/// Like `fmt_write_time` but writes to an allocated string.
extern fmt_String fmt_format_time(
    const char *restrict format, const struct tm *restrict datetime
);

/// Translates a strftime format string into the fmt time format.
///
/// The format specifiers introduced by glibc are not supported.
///
/// Modifiers ('E' and 'O') are not supported (fields can only be 1 character).
extern void fmt_translate_strftime(
    const char *restrict strftime, char *restrict translated, int size
);

////////////////////////////////////////////////////////////////////////////////
// User-facing wrapper macros
////////////////////////////////////////////////////////////////////////////////

/// Returns `true` if the given variable can be printed.
#define fmt_can_print(x) (FMT__TYPE_ID(x) != fmt__TYPE_UNKNOWN)

/// Writes formatted data into a buffer.
///
/// This macro accepts a ‘writer’, a format string, and a list of arguments.
/// Arguments will be formatted according to the specified format string and the
/// result will be passed to the writer.
///
/// If you already have a `va_list` with the type IDs and arguments use
/// `fmt_va_write` instead.
///
/// Examples:
/// ```c
/// FILE *outf = fopen("output.txt", "w");
/// fmt_Writer *writer = FMT_NEW_STREAM_WRITER(outf);
/// fmt_write(writer, "test");
/// fmt_write(writer, "formatted {}", "arguments");
/// // File content: "testformatted arguments"
/// ```
#define fmt_write(_writer, _format, ...)     \
    fmt__write(                              \
        _writer,                             \
        _format,                             \
        FMT_VA_ARG_COUNT(__VA_ARGS__)        \
        __VA_OPT__(, FMT_ARGS(__VA_ARGS__))  \
    )

/// Prints to the standard output.
///
/// Equivalent to the fmt_println macro except that a newline is not printed at the
/// end of the message.
///
/// Note that stdout is frequently line-buffered by default so it may be
/// necessary to use `fflush(stdout)` to ensure the output is emitted
/// immediately.
///
/// If `FMT_LOCKED_DEFAULT_PRINTERS` is defined this will lock the internal
/// mutex on each call.
///
/// Use `fmt_print` only for the primary output of your program. Use
/// `fmt_eprint` instead to print error and progress messages.
///
/// Examples:
/// ```c
/// fmt_print("this ");
/// fmt_print("will ");
/// fmt_print("be ");
/// fmt_print("on ");
/// fmt_print("the ");
/// fmt_print("same ");
/// fmt_print("line ");
/// fflush(stdout);
///
/// fmt_print("this string has a newline, why not choose fmt_println instead?\n");
/// fflush(stdout);
/// ```
#define fmt_print(_format, ...)              \
    fmt__std_print(                          \
        stdout,                              \
        _format,                             \
        false,                               \
        FMT_VA_ARG_COUNT(__VA_ARGS__)        \
        __VA_OPT__(, FMT_ARGS(__VA_ARGS__))  \
    )

/// Prints to the standard output, with a newline.
///
/// On all platforms, the newline is the LINE FEED character (`\n`/`U+000A`)
/// alone (no additional CARRIAGE RETURN (`\r`/`U+000D`)).
///
/// If `FMT_LOCKED_DEFAULT_PRINTERS` is defined this will lock the internal
/// mutex on each call.
///
/// Examples:
/// ```c
/// fmt_println("hello there!");
/// fmt_println("format {} arguments", "some");
/// const char *variable = "some";
/// fmt_println("format {} arguments", variable);
/// ```
#define fmt_println(_format, ...)            \
    fmt__std_print(                          \
        stdout,                              \
        _format,                             \
        true,                                \
        FMT_VA_ARG_COUNT(__VA_ARGS__)        \
        __VA_OPT__(, FMT_ARGS(__VA_ARGS__))  \
    )

/// Prints to the standard error.
///
/// Equivalent to the `fmt_print` macro, except that output goes to `stderr`
/// instead of `stdout`. See `fmt_print` for example usage.
///
/// Use `fmt_eprint` only for error and progress messages. Use `fmt_print`
/// instead for the primary output of your program.
#define fmt_eprint(_format, ...)             \
    fmt__std_print(                          \
        stderr,                              \
        _format,                             \
        false,                               \
        FMT_VA_ARG_COUNT(__VA_ARGS__)        \
        __VA_OPT__(, FMT_ARGS(__VA_ARGS__))  \
    )

/// Prints to the standard error, with a newline.
///
/// Equivalent to the `fmt_println` macro, except that output goes to `stderr`
/// instead of `stdout`. See `fmt_println` for example usage.
///
/// Use `fmt_eprintln` only for error and progress messages. Use `fmt_println`
/// instead for the primary output of your program.
#define fmt_eprintln(_format, ...)           \
    fmt__std_print(                          \
        stderr,                              \
        _format,                             \
        true,                                \
        FMT_VA_ARG_COUNT(__VA_ARGS__)        \
        __VA_OPT__(, FMT_ARGS(__VA_ARGS__))  \
    )

/// Accumulates formatted data in an allocated string.
///
/// String structure:
/// ```c
/// typedef struct {
///     char *data;
///     size_t capacity;
///     size_t size;
/// } fmt_String;
/// ```
///
/// The `data` field must be released using `free`.
/// The `capacity` field contains the amount of usable allocated data, actual
/// allocated size is 1 more byte which is not considered as part of the
/// capacity so it can always contain the null terminator without needing to
/// reallocate just to add it.
/// The `size` field contains the length of the formatted text (without the null
/// terminator).
#define fmt_format(_format, ...)            \
    fmt__format(                            \
        _format,                            \
        FMT_VA_ARG_COUNT(__VA_ARGS__)       \
        __VA_OPT__(, FMT_ARGS(__VA_ARGS__)) \
    )

/// Writes formatted data into an existing buffer.
/// Panics if more than `n - 1` characters are required, as null terminator is
/// always added after the formatted data.
///
/// Examples:
/// ```c
/// char buffer[16];
/// fmt_sprint(buffer, 16, "Hello {}", "World");
/// assert(memcmp(buffer, "Hello World", 12) == 0);
/// ```
#define fmt_sprint(_string, _n, _format, ...) \
    fmt__sprint(                              \
        (_string),                            \
        (_n),                                 \
        _format,                              \
        FMT_VA_ARG_COUNT(__VA_ARGS__)         \
        __VA_OPT__(, FMT_ARGS(__VA_ARGS__))   \
    )

/// Convenience function for writing to a file.
///
/// Unlike the default functions for writing to stdout and stderr this is not
/// affected by `FMT_LOCKED_DEFAULT_PRINTERS`.
///
/// Equivalent to:
/// ```c
/// fmt_Writer *writer = FMT_NEW_STREAM_WRITER(stream);
/// fmt_write(writer, format, ...);
/// ```
///
/// Examples:
/// ```c
/// FILE *log_file = fopen("log.txt", "w");
/// fmt_fprint(log_file, "Hello {}\n", "World");
/// ```
#define fmt_fprint(_stream, _format, ...)    \
    fmt__fprint(                             \
        _stream,                             \
        _format,                             \
        FMT_VA_ARG_COUNT(__VA_ARGS__)        \
        __VA_OPT__(, FMT_ARGS(__VA_ARGS__))  \
    )

/// Aborts the program with an error message.
///
/// The error message will look like this: `file:line: message[\n]`.
/// The newline is only added if the message does not already end with a newline.
///
/// The message is formatted as with all the other macros.
///
/// Examples:
/// ```c
/// fmt_panic("this is a terrible mistake!");
/// fmt_panic("this is a {} {}", "fancy", "message");
/// ```
#define fmt_panic(_format, ...)              \
    fmt__panic(                              \
        __FILE__,                            \
        __LINE__,                            \
         _format,                            \
        FMT_VA_ARG_COUNT(__VA_ARGS__)        \
        __VA_OPT__(, FMT_ARGS(__VA_ARGS__))  \
    )

/// Indicates unimplemented code by panicking with a message of “not implemented”.
///
/// Wraps `fmt_panic`.
///
/// Examples:
/// ```c
/// fmt_todo();
/// fmt_todo("string");
/// fmt_todo("format {}", 123);
/// ```
#define fmt_todo(...) \
    fmt_panic("not yet implemented" __VA_OPT__(": ") __VA_ARGS__)

/// Indicates unfinished code.
///
/// Wraps `fmt_panic`.
///
/// Examples:
/// ```c
/// fmt_unimplemented();
/// fmt_unimplemented("string");
/// fmt_unimplemented("format {}", 123);
/// ```
#define fmt_unimplemented(...) \
    fmt_panic("not implemented" __VA_OPT__(": ") __VA_ARGS__)

/// Indicates unreachable code.
///
/// Wraps `fmt_panic`, but unlike the other macros this expands to a do-while
/// statement and not a function call expression.
///
/// If `NDEBUG` is defined this will never print anything and just hints to the
/// compiler that this code in unreachable.
///
/// Examples:
/// ```c
/// fmt_unreachable();
/// fmt_unreachable("string");
/// fmt_unreachable("format {}", 123);
/// ```
#ifdef NDEBUG
// we only support gcc and clang which both provide this builtin.
#  define fmt_unreachable(...) __builtin_unreachable()
#else
#  define fmt_unreachable(...)                                              \
    do {                                                                    \
        fmt_panic("entered unreachable code" __VA_OPT__(": ") __VA_ARGS__); \
        __builtin_unreachable();                                            \
    } while (0)
#endif

#endif /* FMT_H */





#ifdef FMT_IMPLEMENTATION
#undef FMT_IMPLEMENTATION

static int fmt__min(int a, int b) {
    return a < b ? a : b;
}

static int fmt__max(int a, int b) {
    return a > b ? a : b;
}

////////////////////////////////////////////////////////////////////////////////
// Type ID functions
////////////////////////////////////////////////////////////////////////////////

static size_t fmt__va_get_unsigned_integer(fmt__va_list_ref ap) {
    fmt_Type_Id type = (fmt_Type_Id)va_arg(FMT__VA_LIST_DEREF(ap), int);
    switch (type) {
    case fmt__TYPE_SIGNED_CHAR:
    case fmt__TYPE_SHORT:
    case fmt__TYPE_INT:
    // these are promoted to int
    case fmt__TYPE_UNSIGNED_CHAR:
    case fmt__TYPE_UNSIGNED_SHORT: {
        int n = va_arg(FMT__VA_LIST_DEREF(ap), int);
        if (n < 0) goto negative;
        return n;
    }
    case fmt__TYPE_LONG: {
        long n = va_arg(FMT__VA_LIST_DEREF(ap), long);
        if (n < 0) goto negative;
        return n;
    }
    case fmt__TYPE_LONG_LONG: {
        long long n = va_arg(FMT__VA_LIST_DEREF(ap), long long);
        if (n < 0) goto negative;
        return n;
    }
    case fmt__TYPE_UNSIGNED_LONG: {
        unsigned long n = va_arg(FMT__VA_LIST_DEREF(ap), unsigned long);
        return n;
    }
    case fmt__TYPE_UNSIGNED_LONG_LONG: {
        unsigned long long n = va_arg(FMT__VA_LIST_DEREF(ap), unsigned long long);
        return n;
    }
    default:
        fmt_panic("expected integer type");
    }
negative:
    fmt_panic("expected unsigned value");
}

static char32_t fmt__va_get_character(fmt__va_list_ref ap) {
    fmt_Type_Id type = (fmt_Type_Id)va_arg(FMT__VA_LIST_DEREF(ap), int);
    switch (type) {
    case fmt__TYPE_CHAR:
        // char gets promoted to int
        return va_arg(FMT__VA_LIST_DEREF(ap), int);
    case fmt__TYPE_INT:
        // character literals have type `int`, fmt__TYPE_CHAR is only available
        // for `char` variables.
        return va_arg(FMT__VA_LIST_DEREF(ap), int);
    case fmt__TYPE_UNSIGNED_SHORT:
        // promoted to int
        return va_arg(FMT__VA_LIST_DEREF(ap), int);
    case fmt__TYPE_UNSIGNED:
        return va_arg(FMT__VA_LIST_DEREF(ap), unsigned);
    default:
        fmt_panic("expected character type");
    }
}

static const char * fmt__va_get_utf8_string(fmt__va_list_ref ap) {
    fmt_Type_Id type = (fmt_Type_Id)va_arg(FMT__VA_LIST_DEREF(ap), int);
    switch (type) {
    case fmt__TYPE_STRING:
        return va_arg(FMT__VA_LIST_DEREF(ap), const char *);
    default:
        fmt_panic("expected string");
    }
}

static const char *fmt__valid_display_types(fmt_Type_Id type) {
    switch (type) {
        case fmt__TYPE_CHAR:
        case fmt__TYPE_WCHAR:
            return "bcdioxX$";
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
            return "bcdioxX$";
        case fmt__TYPE_FLOAT:
        case fmt__TYPE_DOUBLE:
            return "eEfFgG%$";
        case fmt__TYPE_STRING:
        case fmt__TYPE_STRING_16:
        case fmt__TYPE_STRING_32:
            return "pPs";
        case fmt__TYPE_FMT_STRING:
            return "s";
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

int fmt__write_any_str(fmt_Writer *restrict writer, const char *restrict str) {
    return writer->write_data(writer, str, strlen(str));
}

int fmt__write_stream_byte(fmt_Writer *p_self, char byte) {
    fmt_Stream_Writer *self = (fmt_Stream_Writer*)p_self;
    fputc(byte, self->stream);
    return 1;
}

int fmt__write_stream_data(fmt_Writer *restrict p_self, const char *restrict data, size_t n) {
    fmt_Stream_Writer *self = (fmt_Stream_Writer*)p_self;
    return (int)fwrite(data, 1, n, self->stream);
}



static void fmt__string_writer_check(fmt_String_Writer *self, int space) {
    if (self->at + space > self->end) {
        const size_t capacity = self->end - self->string;
        const size_t size = self->at - self->string;
        fmt_panic(
            "string writer overflow\n   content: \
{:.{}}\
\n  capacity: {}\n      size: {}",
            self->string, size, capacity, size
        );
    }
}

int fmt__write_string_byte(fmt_Writer *p_self, char byte) {
    fmt_String_Writer *self = (fmt_String_Writer *)p_self;
    fmt__string_writer_check(self, 1);
    *self->at++ = byte;
    return 1;
}

int fmt__write_string_data(
    fmt_Writer *restrict p_self, const char *restrict data, size_t n
) {
    fmt_String_Writer *self = (fmt_String_Writer *)p_self;
    fmt__string_writer_check(self, n);
    memcpy(self->at, data, n);
    self->at += n;
    return n;
}



static void fmt__string_will_append(fmt_String *str, size_t amount) {
    const size_t min_cap = str->size + amount;
    if (min_cap > str->capacity) {
        const size_t target_cap = str->capacity * 15 / 10;
        const size_t new_cap = target_cap >= min_cap ? target_cap : min_cap;
        char *const new_buf = (char *)realloc(str->data, new_cap + 1);
        if (NULL == new_buf) {
            fmt_panic("string allocation failed");
        }
        str->data = new_buf;
    }
}

int fmt__write_alloc_byte(fmt_Writer *p_self, char byte) {
    fmt_Allocating_String_Writer *self = (fmt_Allocating_String_Writer *)p_self;
    fmt__string_will_append(&self->string, 1);
    self->string.data[self->string.size++] = byte;
    return 1;
}

int fmt__write_alloc_data(
    fmt_Writer *restrict p_self, const char *restrict data, size_t n
) {
    fmt_Allocating_String_Writer *self = (fmt_Allocating_String_Writer *)p_self;
    fmt__string_will_append(&self->string, n);
    memcpy(self->string.data + self->string.size, data, n);
    self->string.size += n;
    return n;
}



// fmt__write_metric_byte and fmt__write_metric_data are defined after the
// unicode utilities on which they rely.

int fmt__write_metric_str (
    fmt_Writer *restrict p_self, const char *restrict str
) {
    return fmt__write_metric_data(p_self, str, strlen(str));
}

int fmt__write_limited_byte(fmt_Writer *p_self, char byte) {
    fmt_Limited_Writer *self = (fmt_Limited_Writer *)p_self;
    int written = 0;
    if (self->characters_left) {
        written += self->inner->write_byte(self->inner, byte);
        --self->characters_left;
    }
    return written;
}

// fmt__write_limited_data is defined after the unicode utilities.



static inline int fmt__bw_write_inner_data(
    fmt_Buffered_Writer *bw, const char *data, size_t n
) {
    if (bw->is_stream) {
        return (int)fwrite(data, 1, n, bw->stream);
    } else {
        return bw->inner->write_data(
            bw->inner, data, n
        );
    }
}

int fmt__write_buffered_byte(fmt_Writer *p_self, char byte) {
    fmt_Buffered_Writer *self = (fmt_Buffered_Writer *)p_self;
    if (self->used == sizeof(self->buffer)) {
        fmt_bw_flush(self);
    }
    self->buffer[self->used++] = byte;
    return 1;
}

int fmt__write_buffered_data(
    fmt_Writer *restrict p_self, const char *restrict data, size_t n
) {
    fmt_Buffered_Writer *self = (fmt_Buffered_Writer *)p_self;
    if ((size_t)self->used + n > sizeof(self->buffer)) {
        fmt_bw_flush(self);
        if (n >= sizeof(self->buffer)) {
            return fmt__bw_write_inner_data(self, data, n);
        }
    }
    memcpy(self->buffer + self->used, data, n);
    self->used += n;
    return n;
}

int fmt__write_buffered_str (
    fmt_Writer *restrict p_self, const char *restrict str
) {
    return fmt__write_buffered_data(p_self, str, strlen(str));
}

fmt_Buffered_Writer fmt_bw_new(fmt_Writer *inner) {
    return (fmt_Buffered_Writer){
        .base = fmt_BUFFERED_WRITER_FUNCTIONS,
        .inner = inner,
        .buffer = {},
        .used = 0,
        .is_stream = false,
    };
}

fmt_Buffered_Writer fmt_bw_new_stream(FILE *stream) {
    return (fmt_Buffered_Writer){
        .base = fmt_BUFFERED_WRITER_FUNCTIONS,
        .stream = stream,
        .buffer = {},
        .used = 0,
        .is_stream = true,
    };
}

void fmt_bw_flush(fmt_Buffered_Writer *bw) {
    if (bw->used) {
        fmt__bw_write_inner_data(bw, bw->buffer, bw->used);
        bw->used = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
// wcwidth implementation from https://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
// Names and types have been changed to fit this library.  We use the "fmt__"
// prefix as all internal functions and append the "mk_" prefix to it (short
// for the name of the author, Markus Kuhn).
// We only use the non-CJK variant.
//
// Original copyright notice:
//    Permission to use, copy, modify, and distribute this software
//    for any purpose and without fee is hereby granted. The author
//    disclaims all warranties with regard to this software.
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    char32_t first;
    char32_t last;
} fmt__Mk_Interval;

/* auxiliary function for binary search in interval table */
static int fmt__mk_bisearch(char32_t ucs, const fmt__Mk_Interval *table, int max) {
    int min = 0;
    int mid;

    if (ucs < table[0].first || ucs > table[max].last) {
        return 0;
    }
    while (max >= min) {
        mid = (min + max) / 2;
        if (ucs > table[mid].last) {
            min = mid + 1;
        } else if (ucs < table[mid].first) {
            max = mid - 1;
        } else {
            return 1;
        }
    }
    return 0;
}

/* The following two functions define the column width of an ISO 10646
 * character as follows:
 *
 *    - The null character (U+0000) has a column width of 0.
 *
 *    - Other C0/C1 control characters and DEL will lead to a return
 *      value of -1.
 *
 *    - Non-spacing and enclosing combining characters (general
 *      category code Mn or Me in the Unicode database) have a
 *      column width of 0.
 *
 *    - SOFT HYPHEN (U+00AD) has a column width of 1.
 *
 *    - Other format characters (general category code Cf in the Unicode
 *      database) and ZERO WIDTH SPACE (U+200B) have a column width of 0.
 *
 *    - Hangul Jamo medial vowels and final consonants (U+1160-U+11FF)
 *      have a column width of 0.
 *
 *    - Spacing characters in the East Asian Wide (W) or East Asian
 *      Full-width (F) category as defined in Unicode Technical
 *      Report #11 have a column width of 2.
 *
 *    - All remaining characters (including all printable
 *      ISO 8859-1 and WGL4 characters, Unicode control characters,
 *      etc.) have a column width of 1.
 *
 * This implementation assumes that wchar_t characters are encoded
 * in ISO 10646.
 */
int fmt__mk_wcwidth(char32_t ucs) {
    /* sorted list of non-overlapping intervals of non-spacing characters */
    /* generated by "uniset +cat=Me +cat=Mn +cat=Cf -00AD +1160-11FF +200B c" */
    static const fmt__Mk_Interval combining[] = {
        { 0x0300, 0x036F }, { 0x0483, 0x0486 }, { 0x0488, 0x0489 },
        { 0x0591, 0x05BD }, { 0x05BF, 0x05BF }, { 0x05C1, 0x05C2 },
        { 0x05C4, 0x05C5 }, { 0x05C7, 0x05C7 }, { 0x0600, 0x0603 },
        { 0x0610, 0x0615 }, { 0x064B, 0x065E }, { 0x0670, 0x0670 },
        { 0x06D6, 0x06E4 }, { 0x06E7, 0x06E8 }, { 0x06EA, 0x06ED },
        { 0x070F, 0x070F }, { 0x0711, 0x0711 }, { 0x0730, 0x074A },
        { 0x07A6, 0x07B0 }, { 0x07EB, 0x07F3 }, { 0x0901, 0x0902 },
        { 0x093C, 0x093C }, { 0x0941, 0x0948 }, { 0x094D, 0x094D },
        { 0x0951, 0x0954 }, { 0x0962, 0x0963 }, { 0x0981, 0x0981 },
        { 0x09BC, 0x09BC }, { 0x09C1, 0x09C4 }, { 0x09CD, 0x09CD },
        { 0x09E2, 0x09E3 }, { 0x0A01, 0x0A02 }, { 0x0A3C, 0x0A3C },
        { 0x0A41, 0x0A42 }, { 0x0A47, 0x0A48 }, { 0x0A4B, 0x0A4D },
        { 0x0A70, 0x0A71 }, { 0x0A81, 0x0A82 }, { 0x0ABC, 0x0ABC },
        { 0x0AC1, 0x0AC5 }, { 0x0AC7, 0x0AC8 }, { 0x0ACD, 0x0ACD },
        { 0x0AE2, 0x0AE3 }, { 0x0B01, 0x0B01 }, { 0x0B3C, 0x0B3C },
        { 0x0B3F, 0x0B3F }, { 0x0B41, 0x0B43 }, { 0x0B4D, 0x0B4D },
        { 0x0B56, 0x0B56 }, { 0x0B82, 0x0B82 }, { 0x0BC0, 0x0BC0 },
        { 0x0BCD, 0x0BCD }, { 0x0C3E, 0x0C40 }, { 0x0C46, 0x0C48 },
        { 0x0C4A, 0x0C4D }, { 0x0C55, 0x0C56 }, { 0x0CBC, 0x0CBC },
        { 0x0CBF, 0x0CBF }, { 0x0CC6, 0x0CC6 }, { 0x0CCC, 0x0CCD },
        { 0x0CE2, 0x0CE3 }, { 0x0D41, 0x0D43 }, { 0x0D4D, 0x0D4D },
        { 0x0DCA, 0x0DCA }, { 0x0DD2, 0x0DD4 }, { 0x0DD6, 0x0DD6 },
        { 0x0E31, 0x0E31 }, { 0x0E34, 0x0E3A }, { 0x0E47, 0x0E4E },
        { 0x0EB1, 0x0EB1 }, { 0x0EB4, 0x0EB9 }, { 0x0EBB, 0x0EBC },
        { 0x0EC8, 0x0ECD }, { 0x0F18, 0x0F19 }, { 0x0F35, 0x0F35 },
        { 0x0F37, 0x0F37 }, { 0x0F39, 0x0F39 }, { 0x0F71, 0x0F7E },
        { 0x0F80, 0x0F84 }, { 0x0F86, 0x0F87 }, { 0x0F90, 0x0F97 },
        { 0x0F99, 0x0FBC }, { 0x0FC6, 0x0FC6 }, { 0x102D, 0x1030 },
        { 0x1032, 0x1032 }, { 0x1036, 0x1037 }, { 0x1039, 0x1039 },
        { 0x1058, 0x1059 }, { 0x1160, 0x11FF }, { 0x135F, 0x135F },
        { 0x1712, 0x1714 }, { 0x1732, 0x1734 }, { 0x1752, 0x1753 },
        { 0x1772, 0x1773 }, { 0x17B4, 0x17B5 }, { 0x17B7, 0x17BD },
        { 0x17C6, 0x17C6 }, { 0x17C9, 0x17D3 }, { 0x17DD, 0x17DD },
        { 0x180B, 0x180D }, { 0x18A9, 0x18A9 }, { 0x1920, 0x1922 },
        { 0x1927, 0x1928 }, { 0x1932, 0x1932 }, { 0x1939, 0x193B },
        { 0x1A17, 0x1A18 }, { 0x1B00, 0x1B03 }, { 0x1B34, 0x1B34 },
        { 0x1B36, 0x1B3A }, { 0x1B3C, 0x1B3C }, { 0x1B42, 0x1B42 },
        { 0x1B6B, 0x1B73 }, { 0x1DC0, 0x1DCA }, { 0x1DFE, 0x1DFF },
        { 0x200B, 0x200F }, { 0x202A, 0x202E }, { 0x2060, 0x2063 },
        { 0x206A, 0x206F }, { 0x20D0, 0x20EF }, { 0x302A, 0x302F },
        { 0x3099, 0x309A }, { 0xA806, 0xA806 }, { 0xA80B, 0xA80B },
        { 0xA825, 0xA826 }, { 0xFB1E, 0xFB1E }, { 0xFE00, 0xFE0F },
        { 0xFE20, 0xFE23 }, { 0xFEFF, 0xFEFF }, { 0xFFF9, 0xFFFB },
        { 0x10A01, 0x10A03 }, { 0x10A05, 0x10A06 }, { 0x10A0C, 0x10A0F },
        { 0x10A38, 0x10A3A }, { 0x10A3F, 0x10A3F }, { 0x1D167, 0x1D169 },
        { 0x1D173, 0x1D182 }, { 0x1D185, 0x1D18B }, { 0x1D1AA, 0x1D1AD },
        { 0x1D242, 0x1D244 }, { 0xE0001, 0xE0001 }, { 0xE0020, 0xE007F },
        { 0xE0100, 0xE01EF }
    };

    /* test for 8-bit control characters */
    if (ucs == 0) {
        return 0;
    }
    if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0)) {
        // we don't want an error value, just use 0
        //return -1;
        return 0;
    }

    /* binary search in table of non-spacing characters */
    if (fmt__mk_bisearch(ucs, combining, sizeof(combining) / sizeof(fmt__Mk_Interval) - 1)) {
        return 0;
    }

    /* if we arrive here, ucs is not a combining or C0/C1 control character */

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

/// Returns the number of bytes a codepoint takes in UTF-8 representation.
static int fmt__codepoint_utf8_length(char32_t codepoint) {
    if (!fmt__is_valid_codepoint(codepoint)) {
        return 3;
    }
    if (codepoint < (1 << 7)) {
        return 1;
    }
    if (codepoint < (1 << 11)) {
        return 2;
    }
    if (codepoint < (1 << 16)) {
        return 3;
    }
    {
        return 4;
    }
}

/// Returns the display width of an ascii character.
static int fmt__ascii_display_width(char ch_) {
    unsigned ch = ch_;
    return !(ch < 32 || (ch >= 0x7f && ch < 0xa0));
}

#ifdef FMT_FAST_DISPLAY_WIDTH
/// Returns the display width of a non-zero-width character.
static int fmt__display_width(char32_t ucs) {
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
#else
#  define fmt__display_width fmt__mk_wcwidth
#endif

/// Decodes 1 codepoint from valid UTF-8 data.  Returns the number of bytes the
/// codepoint uses.
static int fmt__utf8_decode(
    const fmt_char8_t *restrict data, char32_t *restrict codepoint
) {
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

#define FMT__ITER_UTF16(_str, _len, ...)                      \
    do {                                                      \
        char32_t codepoint;                                   \
        while (_len --> 0) {                                  \
            codepoint = *_str++;                              \
            if (codepoint >= 0xD800 && codepoint <= 0xDBFF) { \
                codepoint = (codepoint & 0x3FF) << 10;        \
                codepoint |= (char32_t)*_str++ & 0x3FF;       \
                codepoint += 0x10000;                         \
                --_len;                                       \
            }                                                 \
            { __VA_ARGS__ }                                   \
        }                                                     \
    } while (0)

/// Returns the display width and number of codepoints in a UTF-8 encoded string.
/// If `size` is negative it is determined using `strlen`.
/// If `max_chars_for_width` is non-negative only that many characters are used
/// for the width calculation.
static fmt_Int_Pair fmt__utf8_width_and_length(
    const char *str, int size, int max_chars_for_width
) {
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
static fmt_Int_Pair fmt__utf16_width_and_length(
    const char16_t *str, int size, int max_chars_for_width
) {
    int width = 0;
    int length = 0;
    if (size < 0) {
        size = fmt__utf16_strlen(str);
    }
    if (max_chars_for_width < 0) {
        max_chars_for_width = size;
    }
    FMT__ITER_UTF16(str, size, {
        if (max_chars_for_width-- > 0) {
            width += fmt__display_width(codepoint);
        }
        ++length;
    });
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

// Not sure if this should stay not that I moved to `fmt_Buffered_Writer`
// for buffering which does not require any modifications to existing code.
/// Intermediate buffer for encoding and writing UTF-8 codepoints.
typedef struct {
    char data[32];
    size_t len;
    int written;
} fmt__Intermediate_Buffer;

void fmt__ib_flush(fmt__Intermediate_Buffer *buf, fmt_Writer *writer) {
    writer->write_data(writer, buf->data, buf->len);
    buf->written += buf->len;
    buf->len = 0;
}

void fmt__ib_push(
    fmt__Intermediate_Buffer *buf, char32_t ch, fmt_Writer *writer
) {
    const int len = fmt__codepoint_utf8_length(ch);
    if (buf->len + len > sizeof(buf->data)) {
        fmt__ib_flush(buf, writer);
    }
    fmt__utf8_encode(ch, buf->data + buf->len);
    buf->len += len;
}

/// `len` is the number of codepoints.
static int fmt__write_utf8(
    fmt_Writer *restrict writer, const char *restrict str, int len
) {
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
static int fmt__write_utf16(
    fmt_Writer *restrict writer, const char16_t *restrict str, int len
) {
    fmt__Intermediate_Buffer buf = {};
    FMT__ITER_UTF16(str, len, {
        fmt__ib_push(&buf, codepoint, writer);
    });
    fmt__ib_flush(&buf, writer);
    return buf.written;
}

static int fmt__write_utf32(
    fmt_Writer *restrict writer, const char32_t *restrict str, int len
) {
    fmt__Intermediate_Buffer buf = {};
    while (len --> 0) {
        fmt__ib_push(&buf, *str++, writer);
    }
    fmt__ib_flush(&buf, writer);
    return buf.written;
}

static int fmt__utf8_chars_len(const char *str, int chars) {
    int length = 0;
    int cp_len;
    while (chars--) {
        cp_len = fmt__utf8_codepoint_length(*str);
        length += cp_len;
        str += cp_len;
    }
    return length;
}

static int fmt__utf16_chars_len(const char16_t *str, int chars) {
    int length = 0;
    while (chars--) {
        if (*str >= 0xD800 && *str <= 0xDBFF) {
            ++str;
        }
        ++length;
        ++str;
    }
    return length;
}

static int fmt__utf32_chars_len(const char32_t *str, int chars) {
    // this function is useless now but we still need for the table in
    // fmt__print_specifier.
    (void)str;
    return chars;
}

static const char * fmt__utf8_skip(const char *str, int n) {
    const fmt_char8_t *s = (const fmt_char8_t *)str;
    while(n --> 0) {
        s += fmt__utf8_codepoint_length(*s);
    }
    return (const char *)s;
}

/// Skips `index` codepoints and returns the ascii character after that.
static char32_t fmt__utf8_peek_ascii(const char *str, int index) {
    return *fmt__utf8_skip(str, index);
}

static bool fmt__starts_with(
    const char *restrict str, const char *restrict with, int len
) {
    return memcmp(str, with, len) == 0;
}

int fmt__write_metric_byte(fmt_Writer *p_self, char byte) {
    fmt_Metric_Writer *self = (fmt_Metric_Writer *)p_self;
    ++self->bytes;
    ++self->characters;
    self->width += fmt__ascii_display_width(byte);
    return 1;
}

int fmt__write_metric_data(
    fmt_Writer *restrict p_self, const char *restrict data, size_t n
) {
    fmt_Metric_Writer *self = (fmt_Metric_Writer *)p_self;
    self->bytes += n;
    const fmt_Int_Pair width_and_length = fmt__utf8_width_and_length(data, n, n);
    self->characters += width_and_length.second;
    self->width += width_and_length.first;
    return n;
}

int fmt__write_limited_data(
    fmt_Writer *restrict p_self, const char *restrict data, size_t n
) {
    fmt_Limited_Writer *self = (fmt_Limited_Writer *)p_self;
    int written = 0;
    if (self->characters_left) {
        const int length = fmt__utf8_width_and_length(data, n, n).second;
        const int write = fmt__min(length, self->characters_left);
        written += fmt__write_utf8(self->inner, data, write);
        self->characters_left -= write;
    }
    return written;
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

typedef struct {
    char32_t fill;
    fmt_Alignment align;
    fmt_Sign sign;
    char32_t group;
    int precision;
    int width;
    char type;
    bool alternate_form;
    bool debug;
} fmt_Format_Specifier;

static void fmt__format_specifier_default(fmt_Format_Specifier *spec) {
    spec->type = 0;
    spec->fill = ' ';
    spec->align = fmt_ALIGN_LEFT;
    spec->sign = fmt_SIGN_NEGATIVE;
    spec->alternate_form = false;
    spec->width = 0;
    spec->group = 0;
    spec->precision = -1;
    spec->debug = false;
}

static void fmt__time_format_specifier_default(
    fmt_Format_Specifier *spec, char field
) {
    static const char ZERO_PADDED[] = "HMSIdyYjuwmC";
    spec->fill = strchr(ZERO_PADDED, field) ? '0' : ' ';
    spec->align = fmt_ALIGN_RIGHT;
    spec->width = 0;
    spec->precision = -1;
    switch (field) {
        case 'H':
        case 'M':
        case 'S':
        case 'I':
        case 'y':
        case 'e':
        case 'm':
        case 'd':
        case 'C':
            spec->width = 2;
            break;

        case 'j':
            spec->width = 3;
            break;
    }
    // We don't care about these values but since we still use the normal writing
    // functions we still need to default them so they don't cause problems.
    spec->sign = fmt_SIGN_NEGATIVE;
    spec->alternate_form = false;
    spec->group = 0;
    spec->debug = false;
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
    const char *restrict format_specifier,
    const char *restrict what,
    int *restrict out,
    int specifier_number,
    int *restrict arg_count,
    fmt__va_list_ref ap
) {
    if (*format_specifier == '{') {
        ++format_specifier;
        if (*format_specifier == '}') {
            ++format_specifier;
            if (*arg_count == 0) {
                fmt_panic(
                    "arguments exhausted at {} in format specifier {}",
                    what,
                    specifier_number
                );
            }
            *out = fmt__va_get_unsigned_integer(ap);
            --*arg_count;
        } else {
            fmt_eprintln(
                "\nmissing } for parameterized {} at format specifier {}",
                what,
                specifier_number
            );
        }
    } else if (isdigit(*format_specifier)) {
        *out = 0;
        int last;
        while (isdigit(*format_specifier)) {
            last = *out;
            *out *= 10;
            *out += *format_specifier - '0';
            if (*out < last) {
                fmt_panic(
                    "\noverflow in {} at format specifier {}",
                    what,
                    specifier_number
                );
            }
            ++format_specifier;
        }
    }
    return format_specifier;
}

static const char * fmt__parse_specifier_after_colon(
    const char *restrict format_specifier,
    fmt_Format_Specifier *restrict out,
    int specifier_number,
    int *restrict arg_count,
    fmt__va_list_ref ap
) {
    int parsed;
    // Alignment
    if ((parsed = fmt__parse_alignment(*format_specifier))) {
        // Only alignment
        out->align = (fmt_Alignment)parsed;
        ++format_specifier;
    } else if ((parsed = fmt__parse_alignment(fmt__utf8_peek_ascii(format_specifier, 1)))) {
        // Alignment and codepoint in the string
        out->align = (fmt_Alignment)parsed;
        format_specifier += fmt__utf8_decode((const fmt_char8_t *)format_specifier, &out->fill) + 1;
    } else if ((parsed = fmt__parse_alignment(fmt__utf8_peek_ascii(format_specifier, 2)))) {
        // Alignment and parameterized fill character, or opening or closing curly brace
        if (format_specifier[0] == '{' && format_specifier[1] == '}') {
            out->align = (fmt_Alignment)parsed;
            if (*arg_count == 0) {
                fmt_panic(
                    "arguments exhausted at fill character in format specifier {}",
                    specifier_number
                );
            }
            out->fill = fmt__va_get_character(ap);
            if (!fmt__is_valid_codepoint(out->fill)) {
                fmt_panic(
                    "parameterized fill character in format specifier {} is "
                    "not a valid unicode character: {x:#}",
                    specifier_number,
                    (unsigned)out->fill
                );
            }
            --*arg_count;
            format_specifier += 3;
        } else if (format_specifier[0] == '{' && format_specifier[1] == '{') {
            out->align = (fmt_Alignment)parsed;
            out->fill = '{';
            format_specifier += 3;
        } else if (format_specifier[0] == '{' && format_specifier[1] == '{') {
            out->align = (fmt_Alignment)parsed;
            out->fill = '}';
            format_specifier += 3;
        }
    }
    // Sign
    if ((parsed = fmt__parse_sign(*format_specifier))) {
        out->sign = (fmt_Sign)parsed;
        ++format_specifier;
    }
    // Alternate form
    if (*format_specifier == '#') {
        out->alternate_form = true;
        ++format_specifier;
    }
    // Zero-padding
    if (*format_specifier == '0') {
        out->align = fmt_ALIGN_AFTER_SIGN;
        out->fill = '0';
        ++format_specifier;
    }
    // Width
    if (*format_specifier == '{' || isdigit(*format_specifier)) {
        format_specifier = fmt__parse_int(
            format_specifier, "width", &out->width, specifier_number, arg_count, ap
        );
    }
    // Grouping
    // If we have a . the next character must also be a . or the end of the
    // specifier for it to be the grouping character.  Anything else in this
    // position must be the grouping character.
    char32_t next = fmt__utf8_peek_ascii(format_specifier, 1);
    // FIXME: sane condition
    if (((*format_specifier != '.' && *format_specifier != '}' && *format_specifier != '?')
         || next == '.' || next == '}' || next == '?')
        && !(*format_specifier == '?' && next == '}')) {
        format_specifier += fmt__utf8_decode(
            (const fmt_char8_t *)format_specifier, &out->group
        );
#ifndef FMT_NO_LANGINFO
        if (out->group == '.' || out->group == ',') {
            const char *const thousep = nl_langinfo(THOUSEP);
            if (*thousep) {
                fmt__utf8_decode((const fmt_char8_t *)thousep, &out->group);
            }
        }
#endif
    }
    // Precision
    if (*format_specifier == '.') {
        ++format_specifier;
        format_specifier = fmt__parse_int(
            format_specifier, "precision", &out->precision, specifier_number, arg_count, ap
        );
    }
    // Debug
    if (*format_specifier == '?') {
        ++format_specifier;
        out->debug = true;
    }
    if (*format_specifier++ != '}') {
        fmt_panic("format specifier {} is invalid", specifier_number);
    }
    return format_specifier;
}

static const char * fmt__parse_specifier(
    const char *restrict format_specifier,
    fmt_Format_Specifier *restrict out,
    fmt_Type_Id type,
    int specifier_number,
    int *restrict arg_count,
    fmt__va_list_ref ap
) {
    int parsed;
    fmt__format_specifier_default(out);
    ++format_specifier;  // skip '{'
    if (*format_specifier == '}') {
        return ++format_specifier;
    }
    if (*format_specifier != ':') {
        // Display type
        parsed = *format_specifier++;
        const char *valid = fmt__valid_display_types(type);
        if (strchr(valid, parsed) != NULL) {
            out->type = parsed;
        } else {
            fmt_panic(
                "invalid display type '{}' for argument of type '{}' in "
                "specifier {}, expected one of: {}",
                (char)parsed,
                fmt_Type_Names[(int)type],
                specifier_number,
                valid
            );
        }
    }
    if (*format_specifier == '}') {
        return ++format_specifier;
    } else if (*format_specifier == ':') {
        ++format_specifier;
    } else {
        fmt_panic(
            "expected : or } after display type in format specifier {}",
            specifier_number
        );
    }
    return fmt__parse_specifier_after_colon(
        format_specifier, out, specifier_number, arg_count, ap
    );
}

static const char * fmt__parse_time_specifier(
    const char *restrict format_specifier,
    fmt_Format_Specifier *restrict out,
    int specifier_number
) {
    static const char *const ALL_FIELDS = "HMSaAbBdyYIjpPrRTuwcexXmCFszZ";
    int parsed;
    ++format_specifier;  // skip '{'
    if (strchr(ALL_FIELDS, *format_specifier)) {
        out->type = *format_specifier++;
    } else {
        fmt_panic(
            "invalid field '{}' in time format specifier {}",
            *format_specifier, specifier_number
        );
    }
    fmt__time_format_specifier_default(out, out->type);
    if (*format_specifier == '}') {
        return ++format_specifier;
    } else if (*format_specifier == ':') {
        ++format_specifier;
    } else {
        fmt_panic(
            "expected : or } after field in time format specifier {}",
            specifier_number
        );
    }
    // Alignment
    if ((parsed = fmt__parse_alignment(*format_specifier))) {
        // Only alignment
        out->align = (fmt_Alignment)parsed;
        ++format_specifier;
    } else if ((parsed = fmt__parse_alignment(fmt__utf8_peek_ascii(format_specifier, 1)))) {
        // Alignment and codepoint in the string
        out->align = (fmt_Alignment)parsed;
        format_specifier += fmt__utf8_decode((const fmt_char8_t *)format_specifier, &out->fill) + 1;
    } else if ((parsed = fmt__parse_alignment(fmt__utf8_peek_ascii(format_specifier, 2)))) {
        // Alignment and opening or closing curly brace
        if (format_specifier[0] == '{' && format_specifier[1] == '{') {
            out->align = (fmt_Alignment)parsed;
            out->fill = '{';
            format_specifier += 3;
        } else if (format_specifier[0] == '{' && format_specifier[1] == '{') {
            out->align = (fmt_Alignment)parsed;
            out->fill = '}';
            format_specifier += 3;
        }
    }
    // Width
    if (isdigit(*format_specifier)) {
        format_specifier = fmt__parse_int(
            format_specifier, "width", &out->width, specifier_number, NULL, NULL
        );
    }
    // Precision
    if (*format_specifier == '.') {
        ++format_specifier;
        format_specifier = fmt__parse_int(
            format_specifier, "precision", &out->precision, specifier_number, NULL, NULL
        );
    }
    if (*format_specifier++ != '}') {
        fmt_panic("time format specifier {} is invalid", specifier_number);
    }
    return format_specifier;
}

static const char * fmt__parse_embedded_time_specifier(
    const char *restrict format_specifier,
    fmt_Format_Specifier *restrict out,
    const char *restrict *restrict time_string_out,
    int *restrict time_string_length_out,
    int specifier_number,
    int *restrict arg_count,
    fmt__va_list_ref ap
) {
    fmt__format_specifier_default(out);
    ++format_specifier;  // skip '{'
    if (*format_specifier == '}') {
        *time_string_out = FMT_DEFAULT_TIME_FORMAT;
        *time_string_length_out = sizeof(FMT_DEFAULT_TIME_FORMAT) - 1;
        return ++format_specifier;
    }
    if (*format_specifier == FMT_TIME_DELIM) {
        // we need a padding character here for the {} and {...} cases since the
        // mainloop would otherwise think it's a {{ and ignore the specifier.
        ++format_specifier;
        const char *time_string;
        if (format_specifier[0] == '{' && format_specifier[1] == '}') {
            if (*arg_count == 0) {
                fmt_panic(
                    "arguments exhausted at parameterized time format in format specifier {}",
                    specifier_number
                );
            }
            time_string = fmt__va_get_utf8_string(ap);
            --*arg_count;
            *time_string_out = time_string;
            *time_string_length_out = strlen(time_string);
            format_specifier += 2;
        } else {
            time_string = format_specifier;
            format_specifier = strchr(time_string, FMT_TIME_DELIM);
            if (!format_specifier) {
                fmt_panic(
                    "undelimited time format string in format specifier {}",
                    specifier_number
                );
            }
            *time_string_out = time_string;
            *time_string_length_out = format_specifier - time_string;
            ++format_specifier; // skip closing delimiter
        }
        if (*format_specifier == ':') {
            ++format_specifier;
        }
    } else if (*format_specifier == ':') {
        *time_string_out = FMT_DEFAULT_TIME_FORMAT;
        *time_string_length_out = sizeof(FMT_DEFAULT_TIME_FORMAT) - 1;
        ++format_specifier;
    } else {
        fmt_panic(
            "expected : or } after display type in format specifier {}",
            specifier_number
        );
    }
    return fmt__parse_specifier_after_colon(
        format_specifier, out, specifier_number, arg_count, ap
    );
}

////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions for printing functions
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

static int fmt__grouping_width(int length, int base, char32_t groupchar) {
    const int groupwidth = fmt__display_width(groupchar);
    // Pretty dodgy to have these hardcoded here.
    int interval;
    switch (base) {
    case 2:
#ifdef FMT_BIN_GROUP_NIBBLES
        interval = 4;
#else
        interval = 8;
#endif
        break;

    case 8:
        interval = 4;
        break;

    case 10:
        interval = 3;
        break;

    case 16:
        interval = 4;
        break;
    }
    const int groupchars = (length - 1) / interval;
    return groupchars * groupwidth;
}

static int fmt__pad(fmt_Writer *writer, int n, char32_t ch) {
    if (n <= 0) {
        return 0;
    }
    enum { BUF_SIZE = 32 };
    static FMT__THREAD_LOCAL char buf[BUF_SIZE];
    const int amount = n;
    if (ch < 0x80) {
        memset(buf, ch, BUF_SIZE);
        while (n > 0) {
            writer->write_data(writer, buf, fmt__min(n, BUF_SIZE));
            n -= BUF_SIZE;
        }
        return amount;
    } else {
        char utf8[4];
        const int len = fmt__utf8_encode(ch, utf8);
        const int count = BUF_SIZE / len;
        const int batch = count * len;
        if (!fmt__starts_with(buf, utf8, len)) {
            char *p = buf;
            for (int i = 0; i < count; ++i) {
                memcpy(p, utf8, len);
                p += len;
            }
        }
        while (n > 0) {
            writer->write_data(writer, buf, fmt__min(n*len, batch));
            n -= count;
        }
        return amount * len;
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

static int fmt__write_grouped(
    fmt_Writer *restrict writer,
    const char *restrict buf,
    int len,
    char32_t groupchar,
    int interval
) {
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
    static int _name(fmt_Writer *writer, uint64_t n, int len) {                \
        if (n == 0) {                                                          \
            return writer->write_byte(writer, '0');                            \
        }                                                                      \
        const char *digitpairs = _lookup_string;                               \
        char buf[_buf_size];                                                   \
        char *p = buf + len - 2;                                               \
        int idx;                                                               \
        while (n) {                                                            \
            idx = (n % _div) * 2;                                              \
            memcpy(p, digitpairs + idx, 2);                                    \
            p -= 2;                                                            \
            n /= _div;                                                         \
        }                                                                      \
        return writer->write_data(writer, buf, len);                           \
    }                                                                          \
    static int _name##_grouped(fmt_Writer *writer, uint64_t n, int len, char32_t groupchar) { \
        if (n == 0) {                                                          \
            return writer->write_byte(writer, '0');                            \
        }                                                                      \
        const char *digitpairs = _lookup_string;                               \
        char buf[_buf_size];                                                   \
        char *p = buf + len - 2;                                               \
        int idx;                                                               \
        while (n) {                                                            \
            idx = (n % _div) * 2;                                              \
            memcpy(p, digitpairs + idx, 2);                                    \
            p -= 2;                                                            \
            n /= _div;                                                         \
        }                                                                      \
        return fmt__write_grouped(writer, buf, len, groupchar, _grouping_interval); \
    }

#ifdef FMT_BIN_GROUP_NIBBLES
FMT_DEFINE_WRITE_DIGITS(fmt__write_digits_2, 4, 64, 4,"00011011");
#else
FMT_DEFINE_WRITE_DIGITS(fmt__write_digits_2, 4, 64, 8,"00011011");
#endif

static const char *fmt__DECIMAL_DIGIT_PAIRS =
    "00010203040506070809"
    "10111213141516171819"
    "20212223242526272829"
    "30313233343536373839"
    "40414243444546474849"
    "50515253545556575859"
    "60616263646566676869"
    "70717273747576777879"
    "80818283848586878889"
    "90919293949596979899";

FMT_DEFINE_WRITE_DIGITS(
    fmt__write_digits_10,
    100,
    20,
    3,
    fmt__DECIMAL_DIGIT_PAIRS
);

FMT_DEFINE_WRITE_DIGITS(
    fmt__write_digits_8,
    64,
    24,
    // Python uses 4, Rust doesn't have thousands separators, printf also uses 3.
    3,
    "0001020304050607"
    "1011121314151617"
    "2021222324252627"
    "3031323334353637"
    "4041424344454647"
    "5051525354555657"
    "6061626364656667"
    "7071727374757677"
);

FMT_DEFINE_WRITE_DIGITS(
    fmt__write_digits_16_lower,
    256,
    16,
    4,
    "000102030405060708090a0b0c0d0e0f"
    "101112131415161718191a1b1c1d1e1f"
    "202122232425262728292a2b2c2d2e2f"
    "303132333435363738393a3b3c3d3e3f"
    "404142434445464748494a4b4c4d4e4f"
    "505152535455565758595a5b5c5d5e5f"
    "606162636465666768696a6b6c6d6e6f"
    "707172737475767778797a7b7c7d7e7f"
    "808182838485868788898a8b8c8d8e8f"
    "909192939495969798999a9b9c9d9e9f"
    "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"
    "b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
    "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
    "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
    "e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
    "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff"
);

FMT_DEFINE_WRITE_DIGITS(
    fmt__write_digits_16_upper,
    256,
    16,
    4,
    "000102030405060708090A0B0C0D0E0F"
    "101112131415161718191A1B1C1D1E1F"
    "202122232425262728292A2B2C2D2E2F"
    "303132333435363738393A3B3C3D3E3F"
    "404142434445464748494A4B4C4D4E4F"
    "505152535455565758595A5B5C5D5E5F"
    "606162636465666768696A6B6C6D6E6F"
    "707172737475767778797A7B7C7D7E7F"
    "808182838485868788898A8B8C8D8E8F"
    "909192939495969798999A9B9C9D9E9F"
    "A0A1A2A3A4A5A6A7A8A9AAABACADAEAF"
    "B0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
    "C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF"
    "D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
    "E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEF"
    "F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF"
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
    case 'i':
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
        fmt_panic("invalid type for integer base: {}", type);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Floating point hell
////////////////////////////////////////////////////////////////////////////////

/// Returns the display width of the integer part of a floating point number.
static int fmt__float_integer_width(double f) {
    if (f < 10.0) {
        return 1;
    }
    int width = 1;
    while (f >= 10.0) {
        ++width;
        f /= 10.0;
    }
    return width;
}

/// Returns the display width of the fraction part of a floating point number.
static int fmt__float_fraction_width(double f) {
    double unused;
    double fraction = f;
    int width = 0;
    for (;;) {
        fraction = modf(fraction, &unused);
        if (fraction == (double)(int)fraction) {
            break;
        }
        fraction *= 10.0;
        ++width;
    }
    // no fraction, we want a single zero so width 1
    if (!width) {
        width = 1;
    };
    return width;
}

/// Mostly equivalent to `fmt__write_digits_10` but also handles leading zeros,
/// and does not handle `n` being `0`.
static int fmt__write_float_integer_digits_as_integer(
    fmt_Writer *writer, unsigned long long n, int len
) {
    char buf[20];
    memset(buf, '0', len);
    char *p = buf + len - 2;
    int idx;
    while (n >= 10) {
        idx = (n % 100) * 2;
        memcpy(p, fmt__DECIMAL_DIGIT_PAIRS + idx, 2);
        p -= 2;
        n /= 100;
    }
    if (n) {
        p[1] = '0' + n;
    }
    return writer->write_data(writer, buf, len);
}

static int fmt__write_float_integer_digits(
    fmt_Writer *writer,
    double f,
    int length,
    char32_t groupchar,
    int group_interval
) {
    if (f < (double)UINT64_MAX) {
        if (group_interval) {
            return fmt__write_digits_10_grouped(writer, (uint64_t)f, length, groupchar);
        } else {
            return fmt__write_digits_10(writer, (uint64_t)f, length);
        }
    }
    double div;
    int digit;
    int group_at = length % (group_interval + 1);
    int written = length;
    while (length--) {
        div = pow(10.0, length);
        digit = (int)fmod(f / div, 10.0);
        writer->write_byte(writer, '0' + digit);
        // We don't check if group_interval is 0 here but since group_at starts
        // at 0 in that case we need to overflow it and then go all the way
        // back to 0 until it matters.
        if (--group_at == 0) {
            writer->write_byte(writer, groupchar);
            ++written;
            group_at = group_interval;
        }
    }
    return written;
}

static int fmt__write_float_fraction_digits(
    fmt_Writer *writer,
    double f,
    int length
) {
    char buf[32];
    int pairindex;
    const char *bufend = buf + sizeof(buf);
    char *p = buf;
    int written = 0;
    double unused;
    if (length % 2) {
        f *= 10.0;
        writer->write_byte(writer, '0' + (int)f % 10);
        ++written;
        f -= (int)f;
        --length;
    }
    while (length) {
        length -= 2;
        pairindex = (int)(f * 100.0) * 2;
        f = modf(f * 100.0, &unused);
        memcpy(p, fmt__DECIMAL_DIGIT_PAIRS + pairindex, 2);
        p += 2;
        if (p == bufend) {
            written += writer->write_data(writer, buf, 32);
            p = buf;
        }
    }
    if (p != buf) {
        written += writer->write_data(writer, buf, p - buf);
    }
    return written;
}

static void fmt__get_base_and_exponent(
    double f, double *restrict base, int *restrict exponent
) {
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

/// Returns 10^exp.  exp may not exceed 19.
static double fmt__pow10(int exp) {
    static const double TABLE[] = {
        1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
        1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
    };
    return TABLE[exp];
}

////////////////////////////////////////////////////////////////////////////////
// Debug format
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    bool escape_single_quote;
    bool escape_double_quote;
} fmt__DebugCharEscapeArgs;

/// Escapes that are independant of fmt__DebugCharEscapeArgs
#define FMT__DEBUG_ENUM_COMMON_ESCAPES(o) \
    o('\0', "\\0") \
    o('\t', "\\t") \
    o('\r', "\\r") \
    o('\n', "\\n") \
    o('\\', "\\\\")

static int fmt__debug_escaped_char_width(char32_t ch, fmt__DebugCharEscapeArgs args) {
    #define O(_ch, _) case _ch:
    switch (ch) {
    FMT__DEBUG_ENUM_COMMON_ESCAPES(O)
        return 2;
    case '"':
        if (args.escape_double_quote) {
            return 2;
        }
        break;
    case '\'':
        if (args.escape_single_quote) {
            return 2;
        }
        break;
    }
    #undef O
    if (iswprint(ch)) {
        return fmt__display_width(ch);
    } else if (ch < 0x10000) {
        // "\uXXXX"
        return 6;
    } else {
        // "\uXXXXXXXX"
        return 10;
    }
}

static int fmt__debug_char_width(char32_t ch) {
    return fmt__debug_escaped_char_width(ch, (fmt__DebugCharEscapeArgs) {
        .escape_single_quote = true,
        .escape_double_quote = false,
    });
}

static fmt_Int_Pair fmt__debug_utf8_width_and_length(
    const char *str, int size, int max_chars_for_width
) {
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
            width += fmt__debug_escaped_char_width(
                codepoint,
                (fmt__DebugCharEscapeArgs) {
                    .escape_single_quote = false,
                    .escape_double_quote = true,
                }
            );
        }
        ++length;
    }
    return (fmt_Int_Pair) { width, length };
}

static fmt_Int_Pair fmt__debug_utf16_width_and_length(
    const char16_t *str, int size, int max_chars_for_width
) {
    int width = 0;
    int length = 0;
    if (size < 0) {
        size = fmt__utf16_strlen(str);
    }
    if (max_chars_for_width < 0) {
        max_chars_for_width = size;
    }
    FMT__ITER_UTF16(str, size, {
        if (max_chars_for_width-- > 0) {
            width += fmt__debug_escaped_char_width(
                codepoint,
                (fmt__DebugCharEscapeArgs) {
                    .escape_single_quote = false,
                    .escape_double_quote = true,
                }
            );
        }
        ++length;
    });
    return (fmt_Int_Pair) { width, length };
}

static int fmt__debug_utf32_width(const char32_t *str, int size) {
    int width = 0;
    if (size < 0) {
        size = fmt__utf32_strlen(str);
    }
    while (size --> 0) {
        width += fmt__debug_escaped_char_width(
            *str++,
            (fmt__DebugCharEscapeArgs) {
                .escape_single_quote = false,
                .escape_double_quote = true,
            }
        );
    }
    return width;
}

static int fmt__debug_write_char(
    fmt_Writer *writer, char32_t ch, fmt__DebugCharEscapeArgs args
) {
    #define O(_ch, _esc) case _ch: return writer->write_data(writer, _esc, 2);
    switch (ch) {
    FMT__DEBUG_ENUM_COMMON_ESCAPES(O);
    case '"':
        if (args.escape_double_quote) {
            return writer->write_data(writer, "\\\"", 2);
        }
        break;
    case '\'':
        if (args.escape_single_quote) {
            return writer->write_data(writer, "\\'", 2);
        }
        break;
    }
    #undef O
    if (iswprint(ch)) {
        char buf[4];
        const int len = fmt__utf8_encode(ch, buf);
        return writer->write_data(writer, buf, len);
    } else if (ch < 0x10000) {
        return fmt_write(writer, "\\u{X:0>4}", ch);
    } else {
        return fmt_write(writer, "\\U{X:0>8}", ch);
    }
}

static int fmt__debug_write_utf8(fmt_Writer *writer, const char *str, int len) {
    #define O(_ch, _) _ch,
    static const char ESCAPE_ME[] = { FMT__DEBUG_ENUM_COMMON_ESCAPES(O) '"' };
    #undef O
    int written = 0;
    char32_t codepoint = 0;
    const char *begin = str;
    const char *end = str;
    int n = 0;
    while (len --> 0) {
        n = fmt__utf8_decode((const fmt_char8_t *)end, &codepoint);
        if ((memchr(ESCAPE_ME, codepoint, sizeof(ESCAPE_ME))
             || !iswprint(codepoint))
            && codepoint != FMT_REPLACEMENT_CHARACTER) {
            if (begin != end) {
                written += writer->write_data(writer, begin, end - begin);
            }
            written += fmt__debug_write_char(writer, codepoint, (fmt__DebugCharEscapeArgs) {
                .escape_single_quote = false,
                .escape_double_quote = true,
            });
            begin = end + n;
        }
        end += n;
    }
    if (*begin) {
        written += writer->write_data(writer, begin, end - begin);
    }
    return written;
}

static int fmt__debug_write_utf16(
    fmt_Writer *restrict writer, const char16_t *restrict str, int len
) {
    int written = 0;
    FMT__ITER_UTF16(str, len, {
        written += fmt__debug_write_char(writer, codepoint, (fmt__DebugCharEscapeArgs) {
            .escape_single_quote = false,
            .escape_double_quote = true,
        });
    });
    return written;
}

static int fmt__debug_write_utf32(
    fmt_Writer *restrict writer, const char32_t *restrict str, int len
) {
    int written = 0;
    while (len --> 0) {
        written += fmt__debug_write_char(writer, *str++, (fmt__DebugCharEscapeArgs) {
            .escape_single_quote = false,
            .escape_double_quote = true,
        });
    }
    return written;
}

#undef FMT__ITER_UTF16

////////////////////////////////////////////////////////////////////////////////
// Printing functions
////////////////////////////////////////////////////////////////////////////////

static int fmt__print_utf8(
    fmt_Writer *restrict writer,
    fmt_Format_Specifier *restrict fs,
    const char *restrict string,
    int len
) {
    const fmt_Int_Pair width_and_length = fs->debug
        ? fmt__debug_utf8_width_and_length(
            string, len, fs->width > 0 ? fs->precision : 0
        )
        : fmt__utf8_width_and_length(
            string, len, fs->width > 0 ? fs->precision : 0
        );

    int to_print = width_and_length.second;
    if (fs->precision >= 0 && fs->precision < to_print) {
        to_print = fs->precision;
    }

    const int overhead = fs->debug ? 2 : 0;
    const fmt_Int_Pair pad = fmt__distribute_padding(
        fs->width - (width_and_length.first + overhead), fs->align
    );

    int written = 0;
    written += fmt__pad(writer, pad.first, fs->fill);
    if (fs->debug) {
        written += writer->write_byte(writer, '\"');
        written += fmt__debug_write_utf8(writer, string, to_print);
        written += writer->write_byte(writer, '\"');
    } else {
        written += fmt__write_utf8(writer, string, to_print);
    }
    written += fmt__pad(writer, pad.second, fs->fill);

    return written;
}

static int fmt__print_utf16(
    fmt_Writer *restrict writer,
    fmt_Format_Specifier *restrict fs,
    const char16_t *restrict string,
    int len
) {
    const fmt_Int_Pair width_and_length = fs->debug
        ? fmt__debug_utf16_width_and_length(
            string, len, fs->width > 0 ? fs->precision : 0
        )
        : fmt__utf16_width_and_length(
            string, len, fs->width > 0 ? fs->precision : 0
        );

    int to_print = width_and_length.second;
    if (fs->precision >= 0 && fs->precision < to_print) {
        to_print = fs->precision;
    }

    const int overhead = fs->debug ? 2 : 0;
    const fmt_Int_Pair pad = fmt__distribute_padding(
        fs->width - (width_and_length.first + overhead), fs->align
    );

    int written = 0;
    written += fmt__pad(writer, pad.first, fs->fill);
    if (fs->debug) {
        written += writer->write_byte(writer, '\"');
        written += fmt__debug_write_utf16(writer, string, to_print);
        written += writer->write_byte(writer, '\"');
    } else {
        written += fmt__write_utf16(writer, string, to_print);
    }
    written += fmt__pad(writer, pad.second, fs->fill);

    return written;
}

static int fmt__print_utf32(
    fmt_Writer *restrict writer,
    fmt_Format_Specifier *restrict fs,
    const char32_t *restrict string,
    int len
) {
    const int width = fs->width > 0
        ? (fs->debug
            ? 2 + fmt__debug_utf32_width(string, len)
            : fmt__utf32_width(string, len))
        : 0;

    int to_print = len;
    if (fs->precision >= 0 && fs->precision < to_print) {
        to_print = fs->precision;
    }

    const fmt_Int_Pair pad = fmt__distribute_padding(fs->width - width, fs->align);

    int written = 0;
    written += fmt__pad(writer, pad.first, fs->fill);
    if (fs->debug) {
        written += writer->write_byte(writer, '\"');
        written += fmt__debug_write_utf32(writer, string, to_print);
        written += writer->write_byte(writer, '\"');
    } else {
        written += fmt__write_utf32(writer, string, to_print);
    }
    written += fmt__pad(writer, pad.second, fs->fill);

    return written;
}

static int fmt__print_char(
    fmt_Writer *restrict writer, fmt_Format_Specifier *restrict fs, char32_t ch
) {
    const int width = (fs->debug
                       ? 2 + fmt__debug_char_width(ch)
                       : fmt__display_width(ch));

    const fmt_Int_Pair pad = fmt__distribute_padding(
        fs->width - width, fs->align
    );

    int written = 0;
    written += fmt__pad(writer, pad.first, fs->fill);
    if (fs->debug) {
        written += writer->write_byte(writer, '\'');
        written += fmt__debug_write_char(writer, ch, (fmt__DebugCharEscapeArgs) {
            .escape_single_quote = true,
            .escape_double_quote = false,
        });
        written += writer->write_byte(writer, '\'');
    } else {
        written += fmt__write_codepoint(writer, ch);
    }
    written += fmt__pad(writer, pad.second, fs->fill);

    return written;
}

static int fmt__print_int(
    fmt_Writer *restrict writer,
    fmt_Format_Specifier *restrict fs,
    unsigned long long i,
    char sign
) {
    const fmt_Base *base = fmt__get_base(fs->type);
    const int digits_width = fmt__unsigned_width(i, base->base);
    if (!sign) {
        if (fs->sign == fmt_SIGN_ALWAYS) {
            sign = '+';
        } else if (fs->sign == fmt_SIGN_SPACE) {
            sign = ' ';
        }
    }
    const int width = (digits_width + !!sign
                       + (fs->group
                          ? fmt__grouping_width(
                                digits_width, base->base, fs->group
                            )
                          : 0));

    fmt_Int_Pair pad = fmt__distribute_padding(fs->width - width, fs->align);

    int written = 0;
    if (fs->align != fmt_ALIGN_AFTER_SIGN) {
        written += fmt__pad(writer, pad.first, fs->fill);
    }
    if (sign) {
        written += writer->write_byte(writer, sign);
    }
    if (fs->alternate_form) {
        written += writer->write_data(writer, base->prefix, base->prefix_len);
    }
    if (fs->align == fmt_ALIGN_AFTER_SIGN) {
        written += fmt__pad(writer, pad.first, fs->fill);
    }
    if (fs->group) {
        written += base->write_digits_grouped(writer, i, digits_width, fs->group);
    } else {
        written += base->write_digits(writer, i, digits_width);
    }
    written += fmt__pad(writer, pad.second, fs->fill);

    return written;
}

static int fmt__print_bool(
    fmt_Writer *restrict writer, fmt_Format_Specifier *restrict fs, bool b
) {
    static const char *STRINGS[] = {"false", "true", "no", "yes"};
    static const int LEN[] = {5, 4, 2, 3};
    const int index = ((int)fs->alternate_form << 1) | (int)b;
    return fmt__print_utf8(writer, fs, STRINGS[index], LEN[index]);
}

#define FMT__FLOAT_SPECIAL_CASES()                      \
    do {                                                \
        char str[5] = {0};                              \
        char *p = str + 1;                              \
        if (isinf(f)) {                                 \
            if (isupper(fs->type)) {                    \
                memcpy(p, "INF", 3);                    \
            } else {                                    \
                memcpy(p, "inf", 3);                    \
            }                                           \
        } else if (isnan(f)) {                          \
            if (isupper(fs->type)) {                    \
                memcpy(p, "NAN", 3);                    \
            } else {                                    \
                memcpy(p, "nan", 3);                    \
            }                                           \
        }                                               \
        if (*p) {                                       \
            int len = 3;                                \
            if (signbit(f)) {                           \
                *--p = '-';                             \
                ++len;                                  \
            }                                           \
            fs->precision = -1;                         \
            return fmt__print_utf8(writer, fs, p, len); \
        }                                               \
    } while(0)

typedef struct {
    int integer_width;
    int fraction_width;
    char sign;
    bool no_fraction;
    fmt_Int_Pair padding;
} fmt__Print_Float_State;

#define FMT__WRITE_FLOAT_INIT(_state, _fs, _flt, _width_overhead)        \
    do {                                                                 \
        _state.sign = 0;                                                 \
        if (_flt < 0.0) {                                                \
            _state.sign = '-';                                           \
            _flt = -_flt;                                                \
        } else if (_fs->sign == fmt_SIGN_ALWAYS) {                       \
            _state.sign = '+';                                           \
        } else if (_fs->sign == fmt_SIGN_SPACE) {                        \
            _state.sign = ' ';                                           \
        }                                                                \
        _state.integer_width = fmt__float_integer_width(_flt);           \
        if (_fs->precision < 0) {                                        \
            _fs->precision = FMT_DEFAULT_FLOAT_PRECISION;                \
        }                                                                \
        if (_fs->precision == 0) {                                       \
            _state.no_fraction = true;                                   \
            _state.fraction_width = 0;                                   \
        } else {                                                         \
            _state.no_fraction = false;                                  \
            if (_fs->precision < 0) {                                    \
                _state.fraction_width = fmt__float_fraction_width(_flt); \
            } else {                                                     \
                _state.fraction_width = _fs->precision;                  \
                if (_state.fraction_width < 20) {                        \
                    const double p = fmt__pow10(_state.fraction_width);  \
                    _flt = round(_flt * p) / p;                          \
                }                                                        \
            }                                                            \
        }                                                                \
        const int total_width = (!!_state.sign + _state.integer_width    \
                                 + (!_state.no_fraction                  \
                                    * (1 + _state.fraction_width))       \
                                 + (_width_overhead));                   \
        _state.padding = fmt__distribute_padding(                        \
            _fs->width - total_width, _fs->align                         \
        );                                                               \
    } while (0)

#define FMT__WRITE_FLOAT_INTEGER(_writer, _state, _fs, _flt)                    \
    do {                                                                        \
        written += fmt__write_float_integer_digits(                             \
            _writer, _flt, _state.integer_width, _fs->group, _fs->group ? 3 : 0 \
        );                                                                      \
    } while (0)

#define FMT__WRITE_FLOAT_FRACTION(_writer, _state, _fs, _flt)           \
    do {                                                                \
        double unused, fraction = modf(_flt, &unused);                  \
        if (_state.fraction_width < 20) {                               \
            const uint64_t as_int = round(                              \
                fraction * fmt__pow10(_state.fraction_width)            \
            );                                                          \
            if (as_int == 0) {                                          \
                /* fmt__write_digits_10 can't give us multiple zeros */ \
                static const char zeros[19] = {                         \
                    '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',   \
                    '0' ,'0', '0', '0', '0', '0', '0', '0', '0'         \
                };                                                      \
                written += _writer->write_data(                         \
                    _writer, zeros, _state.fraction_width               \
                );                                                      \
            } else {                                                    \
                written += fmt__write_float_integer_digits_as_integer(  \
                    _writer, as_int, _state.fraction_width              \
                );                                                      \
            }                                                           \
        } else {                                                        \
            written += fmt__write_float_fraction_digits(                \
                _writer, fraction, _state.fraction_width                \
            );                                                          \
        }                                                               \
    } while (0)

static int fmt__print_float_decimal(
    fmt_Writer *restrict writer,
    fmt_Format_Specifier *restrict fs,
    double f,
    char suffix
) {
    FMT__FLOAT_SPECIAL_CASES();
    fmt__Print_Float_State state;
    FMT__WRITE_FLOAT_INIT(state, fs, f, !!suffix);

    int written = 0;
    if (fs->align != fmt_ALIGN_AFTER_SIGN) {
        written += fmt__pad(writer, state.padding.first, fs->fill);
    }
    if (state.sign) {
        written += writer->write_byte(writer, state.sign);
    }
    if (fs->align == fmt_ALIGN_AFTER_SIGN) {
        written += fmt__pad(writer, state.padding.first, fs->fill);
    }
    FMT__WRITE_FLOAT_INTEGER(writer, state, fs, f);
    if (!state.no_fraction) {
#ifdef FMT_NO_LANGINFO
        written += writer->write_byte(writer, '.');
#else
        written += writer->write_str(writer, nl_langinfo(RADIXCHAR));
#endif
        FMT__WRITE_FLOAT_FRACTION(writer, state, fs, f);
    }
    if (suffix) {
        written += writer->write_byte(writer, suffix);
    }
    written += fmt__pad(writer, state.padding.second, fs->fill);
    return written;
}

static int fmt__print_float_exponential(
    fmt_Writer *restrict writer, fmt_Format_Specifier *restrict fs, double f
) {
    FMT__FLOAT_SPECIAL_CASES();
    fmt__Print_Float_State state;
    int exp;
    fmt__get_base_and_exponent(f, &f, &exp);
    const int exp_width = fmt__unsigned_width_10(exp < 0 ? -exp : exp);
    const int width_overhead = 2 + fmt__max(2, exp_width);
    FMT__WRITE_FLOAT_INIT(state, fs, f, width_overhead);

    int written = 0;
    if (fs->align != fmt_ALIGN_AFTER_SIGN) {
        written += fmt__pad(writer, state.padding.first, fs->fill);
    }
    if (state.sign) {
        written += writer->write_byte(writer, state.sign);
    }
    if (fs->align == fmt_ALIGN_AFTER_SIGN) {
        written += fmt__pad(writer, state.padding.first, fs->fill);
    }
    FMT__WRITE_FLOAT_INTEGER(writer, state, fs, f);
    if (!state.no_fraction) {
#ifdef FMT_NO_LANGINFO
        written += writer->write_byte(writer, '.');
#else
        written += writer->write_str(writer, nl_langinfo(RADIXCHAR));
#endif
        FMT__WRITE_FLOAT_FRACTION(writer, state, fs, f);
    }
    if (exp < 0) {
        written += writer->write_data(writer, "e-0", 2 + (exp_width == 1));
        written += fmt__write_digits_10(writer, -exp, exp_width);
    } else {
        written += writer->write_data(writer, "e+0", 2 + (exp_width == 1));
        written += fmt__write_digits_10(writer, exp, exp_width);
    }
    written += fmt__pad(writer, state.padding.second, fs->fill);
    return written;
}

static int fmt__print_float_dynamic(
    fmt_Writer *restrict writer, fmt_Format_Specifier *restrict fs, double f
) {
    FMT__FLOAT_SPECIAL_CASES();
    int exp;
    double base;
    fmt__get_base_and_exponent(f, &base, &exp);
    if (fs->precision < 0) {
        fs->precision = 6;
    } else if (fs->precision == 0) {
        fs->precision = 1;
    }
    if (exp < -4 || exp >= fs->precision) {
        fs->precision -= fmt__float_integer_width(base);
        return fmt__print_float_exponential(writer, fs, f);
    } else {
        fs->precision -= fmt__float_integer_width(f);
        return fmt__print_float_decimal(writer, fs, f, 0);
    }
}

static int fmt__print_cash_money(
    fmt_Writer *restrict writer, fmt_Format_Specifier *restrict fs, double f
) {
    FMT__FLOAT_SPECIAL_CASES();
#ifdef FMT_NO_LANGINFO
    const char *symstr = "-$";
#else
    const char *symstr = nl_langinfo(CRNCYSTR);
    if (!symstr[1]) {
        symstr = "-$";
    }
#endif

    if (fs->precision < 0) {
        fs->precision = 2;
    }
    const int currency_width = fmt__utf8_width_and_length(symstr + 1, -1, -1).first;
    fmt__Print_Float_State state;
    FMT__WRITE_FLOAT_INIT(state, fs, f, currency_width - (symstr[0] == '.'));

    int written = 0;
    if (fs->align != fmt_ALIGN_AFTER_SIGN) {
        written += fmt__pad(writer, state.padding.first, fs->fill);
    }
    // Shopify's Polaris design guidelines say: "Always place the negative
    // symbol before the currency and amount in either format".
    // (https://polaris.shopify.com/foundations/formatting-localized-currency)
    if (state.sign) {
        written += writer->write_byte(writer, state.sign);
    }
    // Not sure if this padding should be before or after the currency symbol
    if (fs->align == fmt_ALIGN_AFTER_SIGN) {
        written += fmt__pad(writer, state.padding.first, fs->fill);
    }
    if (symstr[0] == '-') {
        written += writer->write_str(writer, symstr + 1);
    }
    FMT__WRITE_FLOAT_INTEGER(writer, state, fs, f);
    if (symstr[0] == '.') {
        written += writer->write_str(writer, symstr + 1);
    } else {
#ifdef FMT_NO_LANGINFO
        written += writer->write_byte(writer, '.');
#else
        written += writer->write_str(writer, nl_langinfo(RADIXCHAR));
#endif
    }
    if (!state.no_fraction) {
        FMT__WRITE_FLOAT_FRACTION(writer, state, fs, f);
    }
    if (symstr[0] == '+') {
        written += writer->write_str(writer, symstr + 1);
    }
    written += fmt__pad(writer, state.padding.second, fs->fill);
    return written;
}

#undef FMT__WRITE_FLOAT_FRACTION
#undef FMT__WRITE_FLOAT_INTEGER
#undef FMT__WRITE_FLOAT_INIT
#undef FMT__FLOAT_SPECIAL_CASES

static int fmt__print_pointer(
    fmt_Writer *restrict writer,
    fmt_Format_Specifier *restrict fs,
    const void *restrict ptr
) {
    fs->type = fs->type == 'P' ? 'X' : 'x';
    return fmt__print_int(writer, fs, (uintptr_t)ptr, 0);
}

static int fmt__write_time_sized(
    fmt_Writer *restrict writer,
    const char *restrict format,
    size_t format_size,
    const struct tm *datetime
);

static int fmt__print_time(
    fmt_Writer *restrict writer,
    fmt_Format_Specifier *restrict fs,
    const struct tm *restrict datetime,
    const char *restrict format,
    size_t format_length
) {
    fmt_Metric_Writer metric = {
        .base = fmt_METRIC_WRITER_FUNCTIONS,
        .bytes = 0,
        .characters = 0,
        .width = 0,
    };
    fmt__write_time_sized((fmt_Writer*)&metric, format, format_length, datetime);

    int written = 0;
    fmt_Int_Pair pad;
    if (fs->precision > 0 && fs->precision < (int)metric.characters) {
        fmt_Limited_Writer inner_writer = {
            .base = fmt_LIMITED_WRITER_FUNCTIONS,
            .inner = (fmt_Writer*)&metric,
            .characters_left = fs->precision,
        };
        metric.bytes = 0;
        metric.characters = 0;
        metric.width = 0;
        // re-calculate limited metrics for padding
        fmt__write_time_sized((fmt_Writer*)&inner_writer, format, format_length, datetime);
        pad = fmt__distribute_padding(fs->width - metric.width, fs->align);
        inner_writer.inner = writer;
        inner_writer.characters_left = fs->precision;
        written += fmt__pad(writer, pad.first, fs->fill);
        written += fmt__write_time_sized(
            (fmt_Writer*)&inner_writer, format, format_length, datetime
        );
        written += fmt__pad(writer, pad.second, fs->fill);
        return written;
    } else {
        pad = fmt__distribute_padding(fs->width - metric.width, fs->align);
        written += fmt__pad(writer, pad.first, fs->fill);
        written += fmt__write_time_sized(writer, format, format_length, datetime);
        written += fmt__pad(writer, pad.second, fs->fill);
        return written;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Time
////////////////////////////////////////////////////////////////////////////////

void fmt_translate_strftime(
    const char *restrict strftime, char *restrict translated, int size
) {
    for (; *strftime; ++strftime) {
        switch (*strftime) {
        case '%':
            if (*++strftime == '%') {
                // TODO: format specifiers
                // we probably don't need to care about format specifier as this
                // is used for strings from the locale which shouldn't contain
                // GNU extensions.
                *translated++ = '%';
                --size;
            } else {
                *translated++ = '{';
                *translated++ = *strftime;
                *translated++ = '}';
                size -= 3;
            }
            break;

        case '{':
            *translated++ = '{';
            *translated++ = '{';
            size -= 2;
            break;

        // Closing curly braces are not currently escaped
        //case '}':
        //    *translated++ = '}';
        //    *translated++ = '}';
        //    size -= 2;
        //    break;

        default:
            *translated++ = *strftime;
            --size;
            break;
        }
        if (size == 0) {
            fmt_panic("fmt__translate_strftime: overflow");
        }
    }
    *translated = '\0';
}

static const char * fmt__timezone_name(const struct tm *datetime) {
#ifdef _MSC_VER
    (void)datetime;
    static FMT__THREAD_LOCAL char buf[16];
    size_t unused;
    _get_tzname(&unused, buf, sizeof(buf), 0);
    return buf;
#elif defined(__USE_MISC)
    // Using this double underscore macro seems quite sketchy but I don't know
    // how else to do it and it works for me :^)
    return datetime->tm_zone;
#else
    return datetime->__tm_zone;
#endif
}

static void fmt__get_timezone_offset(
    const struct tm *restrict datetime, char buf[5]
) {
#ifdef _MSC_VER
    (void)datetime;
    long offset;
    _get_timezone(&offset);
#elif defined(__USE_MISC)
    long offset = datetime->tm_gmtoff;
#else
    long offset = datetime->__tm_gmtoff;
#endif
    if (offset < 0) {
        offset = -offset;
        *buf++ = '-';
    } else {
        *buf++ = '+';
    }
    const unsigned full_minutes = offset / 60;
    const unsigned hours = full_minutes / 60;
    const unsigned minutes = full_minutes % 60;
    memcpy(buf, fmt__DECIMAL_DIGIT_PAIRS + hours * 2, 2);
    buf += 2;
    memcpy(buf, fmt__DECIMAL_DIGIT_PAIRS + minutes * 2, 2);
    buf[2] = '\0';
}

/// Used by `fmt__print_time_specifier`, it's a separate function because
/// otherwise we'd need to jump over the declaration of the writer as we can't
/// declare it on top and set the member as most of the are `const`.
static int fmt__write_grouped_time(
    char *restrict buf,
    size_t size,
    const char *restrict format,
    const struct tm *restrict datetime
) {
    fmt_String_Writer group_writer = {
        .base = fmt_STRING_WRITER_FUNCTIONS,
        .string = buf,
        .at = buf,
        .end = buf + size,
    };
    const int end = fmt_write_time(
        (fmt_Writer *)&group_writer, format, datetime
    );
    buf[end] = '\0';
    return end;
}

static int fmt__print_time_specifier(
    fmt_Writer *restrict writer,
    const char **restrict format_specifier,
    int specifier_number,
    const struct tm *restrict datetime
) {
#ifdef FMT_NO_LANGINFO
    static const char *LONG_DAYS[]
        = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    static const char *SHORT_DAYS[]
        = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char *LONG_MONTHS[]
        = {"January", "February", "March", "April", "May", "June",
           "July", "August", "September", "October", "November", "December"};
    static const char *SHORT_MONTHS[]
        = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
#endif

    fmt_Format_Specifier spec;
    union {
        unsigned long long v_unsigned;
        const char *v_string;
#ifndef FMT_NO_LANGINFO
        nl_item v_locale_item;
#endif
    } value;
    struct tm my_datetime;
    char large_buf[128], small_buf[32];
#ifndef FMT_NO_LANGINFO
    char *buf_ptr;
#endif

    *format_specifier = fmt__parse_time_specifier(
        *format_specifier, &spec, specifier_number
    );

    switch (spec.type) {
    case 'H': value.v_unsigned = datetime->tm_hour; goto t_unsigned;
    case 'M': value.v_unsigned = datetime->tm_min; goto t_unsigned;
    case 'S': value.v_unsigned = datetime->tm_sec; goto t_unsigned;

    case 'I':
        value.v_unsigned = datetime->tm_hour % 12;
        if (value.v_unsigned == 0) {
            value.v_unsigned = 12;
        }
        goto t_unsigned;
#ifdef FMT_NO_LANGINFO
    // strftime uses lowercase p for upper case strings and vice versa...
    case 'p': value.v_string = datetime->tm_hour < 12 ? "AM" : "PM"; goto t_string;
    case 'P': value.v_string = datetime->tm_hour < 12 ? "am" : "pm"; goto t_string;
#else
    case 'p': value.v_string = nl_langinfo(datetime->tm_hour < 12 ? AM_STR : PM_STR); goto t_string;
    case 'P': value.v_string = nl_langinfo(datetime->tm_hour < 12 ? AM_STR : PM_STR); goto t_ampm_lower;
#endif

    case 'u': value.v_unsigned = datetime->tm_wday + 1; goto t_unsigned;
    case 'w': value.v_unsigned = datetime->tm_wday; goto t_unsigned;
#ifdef FMT_NO_LANGINFO
    case 'a': value.v_string = SHORT_DAYS[datetime->tm_wday]; goto t_string;
    case 'A': value.v_string = LONG_DAYS[datetime->tm_wday]; goto t_string;
#else
    case 'a': value.v_string = nl_langinfo(ABDAY_1 + datetime->tm_wday); goto t_string;
    case 'A': value.v_string = nl_langinfo(DAY_1 + datetime->tm_wday); goto t_string;
#endif

    case 'm': value.v_unsigned = datetime->tm_mon + 1; goto t_unsigned;
#ifdef FMT_NO_LANGINFO
    case 'b': value.v_string = SHORT_MONTHS[datetime->tm_mon]; goto t_string;
    case 'B': value.v_string = LONG_MONTHS[datetime->tm_mon]; goto t_string;
#else
    case 'b': value.v_string = nl_langinfo(ABMON_1 + datetime->tm_mon); goto t_string;
    case 'B': value.v_string = nl_langinfo(MON_1 + datetime->tm_mon); goto t_string;
#endif
    case 'd': value.v_unsigned = datetime->tm_mday; goto t_unsigned;
    case 'e': value.v_unsigned = datetime->tm_mday; goto t_unsigned;

    case 'y': value.v_unsigned = (1900 + datetime->tm_year) % 100; goto t_unsigned;
    case 'Y': value.v_unsigned = 1900 + datetime->tm_year; goto t_unsigned;
    case 'C': value.v_unsigned = (1900 + datetime->tm_year) / 100; goto t_unsigned;
    case 'j': value.v_unsigned = datetime->tm_yday + 1; goto t_unsigned;

    case 's':
        // mktime normalizes the structure so we need a copy
        my_datetime = *datetime;
        value.v_unsigned = mktime(&my_datetime);
        goto t_unsigned;

    case 'Z': value.v_string = fmt__timezone_name(datetime); goto t_string;
    case 'z': goto t_timezone;

    case 'R': value.v_string = "{H}:{M}"; goto t_group;
    case 'T': value.v_string = "{H}:{M}:{S}"; goto t_group;
    case 'F': value.v_string = "{Y}-{m}-{d}"; goto t_group;

#ifdef FMT_NO_LANGINFO
    case 'c': value.v_string = "{a} {b} {e} {H}:{M}:{S} {Y}"; goto t_group;
    case 'r': value.v_string = "{I}:{M}:{S} {p}"; goto t_group;
    // Note: the POSIX locale would be '{m}/{d}/{y}'
    case 'x': value.v_string = "{d}/{m}/{y}"; goto t_group;
    case 'X': value.v_string = "{H}:{M}:{S}"; goto t_group;
#else
    case 'c': value.v_locale_item = D_T_FMT; goto t_locale_group;
    case 'r': value.v_locale_item = T_FMT_AMPM; goto t_locale_group;
    case 'x': value.v_locale_item = D_FMT; goto t_locale_group;
    case 'X': value.v_locale_item = T_FMT; goto t_locale_group;
#endif
    }

    fmt_unreachable("invalid time field after parsing");

t_unsigned:
    spec.type = 'd';
    return fmt__print_int(writer, &spec, value.v_unsigned, 0);

#ifndef FMT_NO_LANGINFO
t_ampm_lower:
    buf_ptr = small_buf;
    for (const char *p = value.v_string; *p; ++p) {
        *buf_ptr++ = tolower(*p);
    }
    *buf_ptr = '\0';
    value.v_string = small_buf;
    goto t_string;
#endif

t_timezone:
    fmt__get_timezone_offset(datetime, small_buf);
    value.v_string = small_buf;
    goto t_string;

#ifndef FMT_NO_LANGINFO
t_locale_group:
    value.v_string = nl_langinfo(value.v_locale_item);
    fmt_translate_strftime(value.v_string, small_buf, sizeof(small_buf));
    value.v_string = small_buf;
    /* fallthrough */
#endif

t_group:
    fmt__write_grouped_time(
        large_buf, sizeof(large_buf), value.v_string, datetime
    );
    value.v_string = large_buf;
    /* fallthrough */

t_string:
    spec.type = 's';
    int length;
    if (spec.precision < 0) {
        length = strlen(value.v_string);
    } else {
        length = fmt__utf8_chars_len(value.v_string, spec.precision);
    }
    return fmt__print_utf8(writer, &spec, value.v_string, length);
}

// Equivalent to `fmt_write_time` but the format has an explicit size and does
// not need to be null-terminated.
static int fmt__write_time_sized(
    fmt_Writer *restrict writer,
    const char *restrict format,
    size_t format_size,
    const struct tm *restrict datetime
) {
    int written = 0;
    const char *open_bracket = format;
    int specifier_number = 1;
    const char *last_format;
    while (open_bracket) {
        if ((open_bracket = (const char *)memchr(format, '{', format_size)) != NULL) {
            last_format = format;
            written += writer->write_data(writer, format, open_bracket - format);
            if (open_bracket[1] == '{') {
                written += writer->write_byte(writer, '{');
                format = open_bracket + 2;
            } else {
                written += fmt__print_time_specifier(
                    writer, &open_bracket, specifier_number++, datetime
                );
                format = open_bracket;
            }
            format_size -= format - last_format;
        } else if (format_size) {
            written += writer->write_data(writer, format, format_size);
        }
    }
    return written;
}

////////////////////////////////////////////////////////////////////////////////
// Core functions
////////////////////////////////////////////////////////////////////////////////

#ifdef FMT_LOCKED_DEFAULT_PRINTERS
static mtx_t fmt__print_mutex;

static void fmt__clean_mutex() {
    mtx_destroy(&fmt__print_mutex);
}

/// Initializes the mutex used by the fmt_[e]print[ln] and fmt_panic macros.
/// Also registers a exit handler to destroy it.
void fmt_init_threading() {
    mtx_init(&fmt__print_mutex, mtx_plain);
    atexit(fmt__clean_mutex);
}
#else
/// FMT_LOCKED_DEFAULT_PRINTERS not defined, does nothing.
void fmt_init_threading() {}
#endif

static int fmt__print_specifier(
    fmt_Writer *restrict writer,
    const char **restrict format_specifier,
    int *restrict arg_count,
    int specifier_number,
    fmt__va_list_ref ap
) {
    if (*arg_count == 0) {
        fmt_panic("\narguments exhausted at specifier {}", specifier_number);
    }
    --*arg_count;
    fmt_Type_Id type = (fmt_Type_Id)va_arg(FMT__VA_LIST_DEREF(ap), int);
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
        const struct tm *v_time;
        fmt_String v_fmt_string;
    } value;
    int length = 0;
    char sign = 0;
    const char *time_format;

    #define FMT_PARSE_FS()                                                \
        *format_specifier = fmt__parse_specifier(                         \
            *format_specifier, &fs, type, specifier_number, arg_count, ap \
        )

    #define FMT_TID_CASE(_tid, _v, _T, _a)                 \
        case _tid: {                                       \
            value._v = va_arg(FMT__VA_LIST_DEREF(ap), _T); \
            FMT_PARSE_FS();                                \
            goto _a;                                       \
        }

    switch (type) {
        case fmt__TYPE_STRING:
            value.v_pointer = va_arg(FMT__VA_LIST_DEREF(ap), const char *);
            sign = 0;
            // we could do this after the t_string label but I'll keep it up
            // here for consistency
            FMT_PARSE_FS();
            goto t_string;
        case fmt__TYPE_STRING_16:
            value.v_pointer = va_arg(FMT__VA_LIST_DEREF(ap), const char16_t *);
            sign = 1;
            FMT_PARSE_FS();
            goto t_string;
        case fmt__TYPE_STRING_32:
            value.v_pointer = va_arg(FMT__VA_LIST_DEREF(ap), const char32_t *);
            sign = 2;
            FMT_PARSE_FS();
            goto t_string;

        case fmt__TYPE_CHAR:
        case fmt__TYPE_WCHAR: {
            int my_value = va_arg(FMT__VA_LIST_DEREF(ap), int);
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

        // Note: cases reading different types from the variadic arguments than
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

        FMT_TID_CASE(fmt__TYPE_POINTER, v_pointer, const void *, t_pointer)

        FMT_TID_CASE(fmt__TYPE_FLOAT, v_float, double, t_float)
        FMT_TID_CASE(fmt__TYPE_DOUBLE, v_float, double, t_float)

        case fmt__TYPE_TIME:
            value.v_time = va_arg(FMT__VA_LIST_DEREF(ap), struct tm *);
            *format_specifier = fmt__parse_embedded_time_specifier(
                *format_specifier, &fs, &time_format, &length, specifier_number, arg_count, ap
            );
            goto t_time;

        FMT_TID_CASE(fmt__TYPE_FMT_STRING, v_fmt_string, fmt_String, t_fmt_string)
        FMT_TID_CASE(fmt__TYPE_FMT_STRING_TAKE, v_fmt_string, fmt_String, t_fmt_string_take)

        case fmt__TYPE_UNKNOWN:
            // Unknown is also used for all pointers we don't specify an explicit
            // type id for so we don't need to cast to a void pointer every time.
            // This is of course less safe as it could also not be a pointer but
            // that would be the users fault.
            if ((*format_specifier)[1] == 'p' || (*format_specifier)[1] == 'P') {
                value.v_pointer = va_arg(FMT__VA_LIST_DEREF(ap), const void *);
                FMT_PARSE_FS();
                goto t_pointer;
            }
            fmt_panic("unimplemented argument type at specifier {}", specifier_number);

        case fmt__TYPE_ID_COUNT:; // to silence compiler warnings
    }

    #undef FMT_PARSE_FS
    #undef FMT_TID_CASE

    fmt_unreachable("type id value is not one of the enum variants");

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
    switch (fs.type) {
    case 'c':
        if (sign) {
            value.v_unsigned = FMT_REPLACEMENT_CHARACTER;
        }
        return fmt__print_char(writer, &fs, value.v_unsigned);

    case '$':
        if (sign) {
            return fmt__print_cash_money(writer, &fs, -(double)value.v_unsigned);
        } else {
            return fmt__print_cash_money(writer, &fs, (double)value.v_unsigned);
        }

    default:
        return fmt__print_int(writer, &fs, value.v_unsigned, sign);
    }

t_pointer:
    return fmt__print_pointer(writer, &fs, value.v_pointer);

t_float:
    switch (fs.type) {
    case 0:
    case 'f':
    case 'F':
        return fmt__print_float_decimal(writer, &fs, value.v_float, 0);

    case '%':
        return fmt__print_float_decimal(writer, &fs, value.v_float * 100.0, '%');

    case 'e':
    case 'E':
        return fmt__print_float_exponential(writer, &fs, value.v_float);

    case 'g':
    case 'G':
        return fmt__print_float_dynamic(writer, &fs, value.v_float);

    case '$':
        return fmt__print_cash_money(writer, &fs, value.v_float);
    }

t_bool:
    return fmt__print_bool(writer, &fs, value.v_bool);

t_time:
    return fmt__print_time(writer, &fs, value.v_time, time_format, length);

t_fmt_string:
    return fmt__print_utf8(
        writer, &fs, value.v_fmt_string.data, value.v_fmt_string.size
    );

t_fmt_string_take:;
    length = fmt__print_utf8(
        writer, &fs, value.v_fmt_string.data, value.v_fmt_string.size
    );
    free(value.v_fmt_string.data);
    return length;
}

int fmt_va_write(
    fmt_Writer *restrict writer,
    const char *restrict format,
    int arg_count,
    va_list ap
) {
    int written = 0;
    const char *open_bracket = format;
    int specifier_number = 1;
    while (open_bracket) {
        if ((open_bracket = strchr(format, '{')) != NULL) {
            written += writer->write_data(writer, format, open_bracket - format);
            if (open_bracket[1] == '{') {
                written += writer->write_byte(writer, '{');
                format = open_bracket + 2;
            } else {
                written += fmt__print_specifier(
                    writer, &open_bracket, &arg_count, specifier_number++, FMT__VA_LIST_REF(ap)
                );
                format = open_bracket;
            }
        } else if (*format) {
            written += writer->write_str(writer, format);
        }
    }
    if (arg_count) {
        fmt_panic("{} arguments left", arg_count);
    }
    return written;
}

int fmt__write(
    fmt_Writer *restrict writer, const char *restrict format, int arg_count, ...
) {
    va_list ap;
    va_start(ap, arg_count);
    const int written = fmt_va_write(writer, format, arg_count, ap);
    va_end(ap);
    return written;
}

int fmt__std_print(
    FILE *restrict stream,
    const char *restrict format,
    bool newline,
    int arg_count,
    ...
) {
    fmt_Buffered_Writer bwriter = fmt_bw_new_stream(stream);
    fmt_Writer *writer = (fmt_Writer *)&bwriter;
    va_list ap;
    va_start(ap, arg_count);
#ifdef FMT_LOCKED_DEFAULT_PRINTERS
    mtx_lock(&fmt__print_mutex);
#endif
    const int written = fmt_va_write(writer, format, arg_count, ap) + newline;
    if (newline) {
        writer->write_byte(writer, '\n');
    }
    fmt_bw_flush(&bwriter);
#ifdef FMT_LOCKED_DEFAULT_PRINTERS
    mtx_unlock(&fmt__print_mutex);
#endif
    va_end(ap);
    return written;
}

fmt_String fmt_va_format(const char *format, int arg_count, va_list ap) {
    enum { INIT_CAP = 16 };
    fmt_Allocating_String_Writer writer = (fmt_Allocating_String_Writer) {
        .base = fmt_ALLOC_WRITER_FUNCTIONS,
        .string = (fmt_String) {
            .data = (char *)malloc(INIT_CAP + 1),
            .size = 0,
            .capacity = INIT_CAP,
        },
    };
    fmt_va_write((fmt_Writer*)&writer, format, arg_count, ap);
    // We always allocate capacity+1 so this is always within bounds.
    writer.string.data[writer.string.size] = '\0';
    // Increment capacity to be the real value for the user.
    ++writer.string.capacity;
    return writer.string;
}

fmt_String fmt__format(const char *format, int arg_count, ...) {
    va_list ap;
    va_start(ap, arg_count);
    const fmt_String string = fmt_va_format(format, arg_count, ap);
    va_end(ap);
    return string;
}

// We need a 2nd function for this so we can use variadic arguments
static void fmt__panic_loc(fmt_Writer *writer, ...) {
    va_list ap;
    va_start(ap, writer);
    fmt_va_write(writer, "{}:{}: ", 2, ap);
    va_end(ap);
}

FMT__NORETURN void fmt__panic(
    const char *restrict file,
    int line,
    const char *restrict format,
    int arg_count,
    ...
) {
#ifdef FMT_LOCKED_DEFAULT_PRINTERS
    mtx_lock(&fmt__print_mutex);
#endif
    fmt_Stream_Writer swriter = {
        .base = fmt_STREAM_WRITER_FUNCTIONS,
        .stream = stderr,
    };
    fmt_Writer *writer = (fmt_Writer *)&swriter;
    fmt__panic_loc(writer, fmt__TYPE_STRING, file, fmt__TYPE_INT, line);
    va_list ap;
    va_start(ap, arg_count);
    fmt_va_write(writer, format, arg_count, ap);
    va_end(ap);
    if (format[strlen(format) - 1] != '\n') {
        writer->write_byte(writer, '\n');
    }
#ifdef FMT_LOCKED_DEFAULT_PRINTERS
    mtx_unlock(&fmt__print_mutex);
    fmt__clean_mutex();
#endif
    abort();
}

int fmt_va_sprint(
    char *restrict string,
    size_t size,
    const char *restrict format,
    int arg_count,
    va_list ap
) {
    fmt_String_Writer writer = (fmt_String_Writer) {
        .base = fmt_STRING_WRITER_FUNCTIONS,
        .string = string,
        .at = string,
        .end = string + size - 1,
    };
    const int written = fmt_va_write((fmt_Writer *)&writer, format, arg_count, ap);
    string[written] = '\0';
    return written;
}

int fmt__sprint(
    char *restrict string,
    size_t size,
    const char *restrict format,
    int arg_count,
    ...
) {
    va_list ap;
    va_start(ap, arg_count);
    const int written = fmt_va_sprint(string, size, format, arg_count, ap);
    va_end(ap);
    return written;
}

int fmt_va_fprint(
    FILE *restrict stream,
    const char *restrict format,
    int arg_count,
    va_list ap
) {
    fmt_Buffered_Writer swriter = fmt_bw_new_stream(stream);
    fmt_Writer *writer = (fmt_Writer *)&swriter;
    const int written = fmt_va_write(writer, format, arg_count, ap);
    fmt_bw_flush(&swriter);
    return written;
}

int fmt__fprint(
    FILE *restrict stream, const char *restrict format, int arg_count, ...
) {
    va_list ap;
    va_start(ap, arg_count);
    const int written = fmt_va_fprint(stream, format, arg_count, ap);
    va_end(ap);
    return written;
}

int fmt_write_time(
    fmt_Writer *restrict writer,
    const char *restrict format,
    const struct tm *restrict datetime
) {
    int written = 0;
    const char *open_bracket = format;
    int specifier_number = 1;
    while (open_bracket) {
        if ((open_bracket = strchr(format, '{')) != NULL) {
            written += writer->write_data(writer, format, open_bracket - format);
            if (open_bracket[1] == '{') {
                written += writer->write_byte(writer, '{');
                format = open_bracket + 2;
            } else {
                written += fmt__print_time_specifier(
                    writer, &open_bracket, specifier_number++, datetime
                );
                format = open_bracket;
            }
        } else if (*format) {
            written += writer->write_str(writer, format);
        }
    }
    return written;
}

int fmt_format_time_to(
    char *restrict buf,
    size_t size,
    const char *restrict format,
    const struct tm *restrict datetime
) {
    fmt_String_Writer writer = {
        .base = fmt_STRING_WRITER_FUNCTIONS,
        .string = buf,
        .at = buf,
        .end = buf + size - 1,
    };
    const int written = fmt_write_time((fmt_Writer*)&writer, format, datetime);
    *writer.at = '\0';
    return written;
}

fmt_String fmt_format_time(
    const char *restrict format, const struct tm *restrict datetime
) {
    enum { INIT_CAP = 16 };
    fmt_Allocating_String_Writer writer = (fmt_Allocating_String_Writer) {
        .base = fmt_ALLOC_WRITER_FUNCTIONS,
        .string = (fmt_String) {
            .data = (char *)malloc(INIT_CAP + 1),
            .size = 0,
            .capacity = INIT_CAP,
        },
    };
    fmt_write_time((fmt_Writer*)&writer, format, datetime);
    writer.string.data[writer.string.size] = '\0';
    return writer.string;
}

#endif /* FMT_IMPLEMENTATION */

#ifdef FMT__MY_RESTRICT
#  undef FMT__MY_RESTRICT
#  undef restrict
#endif

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
