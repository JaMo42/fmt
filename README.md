# fmt

single-header-only C formatting library with Python/Rust-style format strings and automatic type detection.

## Example

```c
#define FMT_IMPLEMENTATION
#include <fmt.h>

int main(void) {
    const double pi = 3.1415926;
    const int length = 10;
    fmt_println(
        "The area of a {} with {} {} is {}",
        "square", L"side length", length, length*length
    );
    fmt_println(
        "The area of a {} with {} {} is {}",
        u"circle", U"radius", length, pi * (length*length)
    );
    fmt_println("with {c} = {:.7}", u'\u03C0', pi);
}
```

## Requirements

### General

- C11 or newer

### Linux

- `_DEFAULT_SOURCE` to be defined

- compile with `-lm` (I don't think I can get rid of this)

### Windows

Windows builds only work with clang (gcc not tested), as Microsoft has moved on to C++ and `msvc` is lacking a lot of the newer standard C features.

Unlike on linux no directives need to be defined or libraries linked.

---

The library can also be compiled using a C++ compiler which may be useful when mixing both languages without mixing compilers.

## Configuration

All of these only need to be defined in the same file as `FMT_IMPLEMENTATION`.

If a macro has a values its default is given in [ ], otherwise it is not defined by default.

- `FMT_IMPLEMENTATION`

    Must be defined in exactly one translation unit which will receive the
    implementation of the library.

- `FMT_LOCKED_DEFAULT_PRINTERS`

    Use an internal mutex for all calls to `fmt_print`, `fmt_println`, `fmt_eprint`, `fmt_eprintln`, and `fmt_panic`.
    `fmt_init_threading()` must be called before any of these functions are used.
    Note that you can still use `fmt_init_threading()` if this is not defined in which case it will just do nothing.

- `FMT_DEFAULT_FLOAT_PRECISION` [`3`]

    Number of digits to appear after the radix character for floating point conversions, if not specified by the format string.

- `FMT_BIN_GROUP_NIBBLES`

    Insert the grouping character for binary numbers every 4 digits, instead of every 8.

- `FMT_NO_LANGINFO`

    Disable locale support, always disabled on systems that don't provide the `<langinfo.h>` header.

- `FMT_DEFAULT_TIME_FORMAT` [`"{a} {b} {d:0} {H}:{M}:{S} {Y}"`]

    Default time format.

- `FMT_TIME_DELIM` [`'%'`]

    Delimiter for embedded time format strings and indicator for parameterized time format strings.

## Format string format

In the format string *replacement fields* are enclosed in curly braces (`{}`).
Anything not in curly braces is literal text that's copied directly from the format to the output.
If you need to include a literal opening curly bracket character in the template string,
then you can escape it by doubling it: `{{`.
Unlike other languages that use this kind of format closing curly braces do not need to be escaped.

The overall syntax of *replacement fields* is:

1. `{[type][:format_specifier]}`
1. `{%{}[:format_specifier]}`
1. `{%{time_format}%[:format_specifier]}`

(1) is the usual format string as seen in other languages, (2) and (3) are used to embed time formats.

The overall syntax of *format specifiers* is:

    :[[fill]align][sign][#][0][width][group][.precision]

The `fill`, `width`, and `precision` fields can be parameterized if by using `{}`,
in this case their values must be passed after the value being formatted and in the order they appear in in the format specifier.

There are few differences compared to usual implementations to note here:

- The *conversion specifier* `type` comes before the format specifier, but is still just a conversion specifier and is not used to determine the argument type like in printf.

- The thousands separator `group` can be any character and can also be parameterized, this has means:
    - if you want to use a digit for some reason you need to specify the width first.
    - it will only use the thousands separators from the current locale if it's set to `,` or `.`.

- `{` and `}` for the `fill` and `group` fields do not need to be escaped

- The field width takes display width into account (i.e. `가` is 2 width, `a` is 1 wide)

- The precision field for strings specifies the number of Unicode codepoints.

### Embedded time formats

Note: the `%` used in the specification can be changed using the `FMT_TIME_DELIM` macro should you need a percent in the time format (see [Configuration](#configuration)).

In the format (2), the format string for the time is parameterized and must be
the next argument after the `tm` value.
This needs the `%` as a separator between the two opening curly braces as they
would otherwise be considred an escaped curly bracket.

In the format(3), the format string is embedded between the two `%` delimiters.

Examples:

```c
struct tm *datetime = ...;
fmt_println("{%{}:16}", datetime, "{H}:{M}");
fmt_println("{%{H}:{M}%:16}", datetime);
fmt_println("{:16}", datetime); // use FMT_DEFAULT_TIME_FORMAT
```

## Time

The overall syntax of *replacement fields* in the time format is:

    {[field][:format_specifier]}

The overall syntax of *format specifiers* in the time format is:

    :[[fill]align][width][.precision]

The format specifier components are the same as in the normal format strings.

The `field` can be any of `aAbBcCdeFHIjMpPrRsSwxXyYzZ`,
which have the same result as the respective `strftime` fields.

`fmt_translate_strftime(const char *strftime, char *translated, int size)` can be used to convert a strftime format string into a fmt format string.
`size` is the size of `translated`.

Example: `{ %H:%M }` would turn into `{{ {H}:{M} }`.

## Synopsis

### Types

```c
typedef struct {
    int (*write_byte)(fmt_Writer *self, char byte);
    int (*write_data)(const char *data, size_t n);
    int (*write_str)(const char *str);
} fmt_Writer;

typedef struct {
    /// Proxy type what when passed to a fmt function will cause the `data`
    /// member to be free'd after printing it.
    /* unspecified */ take;
    char *data;       // must be released using free()
    size_t capacity;  // number of bytes allocated in data
    size_t size;      // size of the string, excluding null terminator
} fmt_String;
```

### Common functions

These are all macros which expand to a single function call expression.

```c
int fmt_write(fmt_Writer *writer, const char *format, ...);
int fmt_print(const char *format, ...);
int fmt_println(const char *format, ...);
int fmt_eprint(const char *format, ...);
int fmt_eprintln(const char *format, ...);
int fmt_sprint(char *str, size_t size, const char *format, ...);
int fmt_fprint(FILE *stream, const char *format, ...);
fmt_String fmt_format(const char *format, ...);
[[noreturn]] void fmt_panic(const char *format, ...);
```

### Misc functions

```c
void fmt_init_threading();
void fmt_translate_strftime(const char *strftime, char *translated, int size);
```

### Time functions

```c
int fmt_write_time(fmt_Writer *writer, const char *format, const struct tm *datetime);
int fmt_format_time_to(char *str, size_t size, const char *format, const struct tm *datetime);
fmt_String fmt_format_time(const char *format, const struct tm *datetime);
```

### Implementation details

These may be useful for creating your own wrappers.
See `test.c` for a few examples of creating such wrappers.

#### Macros

```c
/// Returns true if X is a value that can be printed
bool fmt_can_print(x);

/// Returns the number of variadic arguments
int FMT_VA_ARG_COUNT(...);

/// Transforms arguments to pairs of type ID and argument
... FMT_ARGS(...);
```

#### Functions

Both the `va_list` and `...` parameters for these must be pairs of type IDs and arguments, obtained from `FMT_ARGS`.

Function mostly follow this pattern: `fmt_va_X` implements `fmt__X`, which implements the `fmt_X` macro.

```c
int fmt_va_write(fmt_Writer *writer, const char *format, int arg_count, va_list ap);
int fmt__write(fmt_Writer *writer, const char *format, int arg_count, ...);

/// Implementation of fmt_print, fmt_println, fmt_eprint, and fmt_eprintln.
int fmt__std_print(FILE *stream, const char *format, bool newline, int arg_count, ...);

int fmt_va_sprint(char *str, size_t size, const char *format, int arg_count, va_list ap);
int fmt__sprint(char *str, size_t size, const char *format, int arg_count, ...);

int fmt_va_fprint(FILE *stream, const char *format, int arg_count, va_list ap);
int fmt__fprint(FILE *stream, const char *format, int arg_count, ...);

[[noreturn]] void fmt__panic(const char *file, int line, const char *format, int arg_count, ...);

fmt_String fmt_va_format(const char *format, int arg_count, va_list ap);
fmt_String fmt__format(const char *format, int arg_count, ...);
```

## Supported types

Type | Default conversion specifier | Valid conversion specifiers
-|-|-
`[const] char *` <br> `[const] char8_t *` <br> `[const] char16_t *` <br> `[const] char32_t *` <br> (see note 1) | s | p P s
`char` (see note 2 and 3) | c | b c d i o x X $
`signed char` <br> `short` <br> `int` <br> `long` <br> `long long` <br> `unsigned char` <br> `unsigned short` <br> `unsigned` <br> `unsigned long` <br> `unsigned long long`  | d | b c d i o x X $
`float` <br> `double` | f | f F e E g G % $
`[const] void *` | p | p P
`[const] struct tm *` | | Cannot have a presentation type due to the different replacement specifier syntax
Anything else | | p P (see below)

Any other type is considered unknown and will cause a runtime if they are used without a presentation type.
They can however be used with the `p` or `P` specifier in which case they are assumed to be pointers,
since we can't realistically have a type ID for every pointer type.

The `long double` type is not supported.

### Notes

1. These will also include `[const] wchar_t *` and due to compiler implementation details may also match various types of integer pointers.
1. If you pass a character literal like `'a'` you need to cast it to a `char` as character literals are integers.
1. `char8_t`, `char16_t`, `char32_t`, and `wchar_t` are supported but are not distinct types so they will be interpreted as integers by default.  Use the `c` conversion specifier to interpret any integer as a Unicode codepoint.

## More examples

### Writers

Writers are any structure containing a `fmt_Writer` as their first member.

```c
typedef struct {
    fmt_Writer base;
    int fd;
} My_Writer;

My_Writer writer = { ... };
fmt_write(writer, "{}", 123);
```

### `fmt_String.take`

```c
fmt_String message = fmt_format("message");
fmt_println("{}", message);
fmt_println("{}", message.take); // string data is free'd here

// More useful example: applying format specifier to
// formatted string without storing it in between.
fmt_println("{:^32}", fmt_format("{} {}", some_value, other_value).take);
```

### Thread exclusion

In this example all 10 messages are printed on their own lines without being
mixed up.

```c
#include <threads.h>
#define FMT_LOCKED_DEFAULT_PRINTERS
#define FMT_IMPLEMENTATION
#include "fmt.h"

int async_function(void *unused) {
    (void)unused;
    // If the string does not contain any replacement specifiers or escaped
    // opening curly braces it would be written at once and the interruption
    // could only happen between the string and the newline.
    fmt_println(
        "{} {} {} {} {} {} {} {} {} {}",
        "I", "am", "on", "my", "own", "line",
        "with", "nothing", "interrupting", "me"
    );
    return 0;
}

int main(void) {
    fmt_init_threading();
    for (int i = 0; i < 10; ++i) {
        thrd_t thread;
        thrd_create(&thread, async_function, NULL);
    }
}
```

### Currency values

These ignore the `FMT_DEFAULT_FLOAT_PRECISION` value and always default to 2 decimal digits.

```c
// The LC_MONETARY category controls the currency symbol, but using LC_ALL also
// sets the decimal dot and thousands separator from LC_NUMERIC.
setlocale(LC_ALL, "en_US.utf8");
fmt_println("{$:.}", -1000); // "-$1,000.00"
setlocale(LC_ALL, "de_DE.utf8");
fmt_println("{$:.}", -1000); // "-1.000,00€"
setlocale(LC_ALL, "ko_KR.utf8");
fmt_println("{$:.}", -1000); // "-₩1,000.00"
```
