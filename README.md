# fmt

Printf-family functions with python-style format strings.

## Format String Syntax

Format strings contain replacement fields surrounded by curly braces `{}`.
Anything that is not a replacement field is written to the output unchanged.
If you need literal curly braces, they can be escaped like this: `{{` and `}}`.

### Replacement fields

`{[length_modifier][u][n][type][:format_specifier]}` (positional)

- `length_modifier` See below.

- `u` Unsigned flag, integer argument is unsigned.

- `n` Locale flag, get thousands separator and decimal point from current locale.

- `type` Type specifier:

  - `d` Decimal integer

  - `b` Binary integer

  - `o` Octal integer, using `0o` as prefix

  - `O` Octal integer, using `0` as prefix

  - `x` Hexadecimal integer, lowercase

  - `X` Hexadecimal integer, uppercase

  - `f` Floating point, lowercase

  - `F` Floating point, uppercase

  - `e` Scientific notation, lowercase

  - `E` Scientific notation, uppercase

  - `%` Floating point, multiplies number by 100 and adds a `%`

  - `c` Character

  - `s` String of character

  - `B` Boolean printed as `true` or `false` string

  - `p` Pointer address

  - `t` Time structure

  - `D` Binary data in Base64

  - `n` Nothing is printed, the number of character written so far is stored in the argument.

Expected types:

Rows: Type specifier, Columns: Length modifier

| | (none) | hh | h | l | ll | j | z | t | L |
|---|---|---|---|---|---|---|---|---|---|
| d b o O x X | `int` | `signed char` | `short` | `long` | `long long` | `intmax_t` | `intmax_t` | `ptrdiff_t` |
| ud ub uo uO ux uX| `unsigned` | `unsigned char` | `unsigned short` | `unsigned long` | `unsigned long long` | `uintmax_t` | `size_t` | `size_t` |
| f e % | | | | | | | | | `long double` |
| c | `char` |
| s | `char *` |
| B | `_Bool` |
| p | `void *` |
| n | `int *` |
| t | `struct tm *` |
| D | `void *` |

Do note that the type string is positional, this is important as `n` and `t` are both types and flags and must be last element of the type string in order to be considered a type.

The type may also be omitted, in this case:

- if the unsigned flag is given it defaults to `d`

- otherwise it defaults to the `fmt_default_type` string

### Format specifier

`[[fill]align]][sign][#][0][width][grouping][.precision]` (non-positional)

- `fill` The character to use for padding. May also be given as a parameter (type `char`).

- `align` The alignment, must one of

  - `<` Align to the left

  - `>` Align to the right (default)

  - `^` Align in the center

  - `=` Align to the right, place the sign/base prefix before the padding

- `sign` The signing mode, must be one of

  - `-` Only sign negative numbers (default)

  - `+` Always sign

  - `' '` Place a leading space in front of positive numbers

- `#` Use the "alternate form"

  - For integers printed in binary, octal or, hexadecimal, this prefixes the output with `0b`, `0`/`0o`, or `0x`, respectively.

  - For floats, the decimal point is printed even when the precision is set to `0`.

  - For booleans, print the first character in uppercase

  - For Base64, print a newline every 76 characters

- `0` Pad zeros, for integers and floats sets align to `=` and fill to `0`

- `width` The width of the field. May also be given as a parameter (type `int`).

- `grouping`, must be either `,` or `_`.
Enables grouping using either the given character or, if the locale flag is given, the one from the current locale.
For integers using binary, octal, or hexadecimal representation the separator is placed every 4 digits.

- `.precision`, for

  - `f` Specifies the length of the fraction.

  - `s` Specifies the maximum length of the string.

  - `D` Specifies the number of bytes (must be provided).

  May also be given as a parameter (type `int`).

If options are given as parameter, they are replaced by `{}` in the format specifier.
Option parameters follow the value in the function arguments.

It is not possible to directly use `{` or `}` as fill character, they have to be given as parameter.

### Time format specifier

If formatting a time structure, the format specifier is replaced by a `strftime` format string.
This string may also be given as a parameter (type `char *`).

## Functions

The functions taking the `int n` parameter print at most `n` characters.

```cpp
int fmt_print(const char *fmt, ...);
int fmt_vprint(const char *fmt, va_list);
int fmt_nprint(int n, const char *fmt, ...);
int fmt_vnprint(int n, const char *fmt, va_list);
```

Print formatted text to `stdout`, return number of characters written.

```cpp
int fmt_sprint(char *str, const char *fmt, ...);
int fmt_vsprint(char *str, const char *fmt, va_list);
int fmt_snprint(char *str, int n, const char *fmt, ...);
int fmt_vsnprint(char *str, int n, const char *fmt, va_list);
```

Print formatted text in the buffer pointed to by `str`, return number or characters written.

```cpp
int fmt_fprint(FILE *stream, const char *fmt, ...);
int fmt_vfprint(FILE *stream, const char *fmt, va_list);
int fmt_fnprint(FILE *stream, int n, const char *fmt, ...);
int fmt_vfnprint(FILE *stream, int n, const char *fmt, va_list);
```

Print formatted text to the `stream`, return number of characters written.

```cpp
char * fmt_format(const char *fmt, ...);
char * fmt_vformat(const char *fmt, vs_list);
char * fmt_nformat(int n, const char *fmt, ...);
char * fmt_vnformat(int n, const char *fmt, va_list);
```

Print formatted text into a newly allocated buffer and return it.
The buffer uses exponential growth, doubling its size when growing.
`fmt_format` and `fmt_vformat` initialize the buffer to `fmt_format_initial_size` characters, `fmt_nformat` and `fmt_vnformat` to `n`.

```cpp
int fmt_formatted_length(const char *fmt, ...);
int fmt_vformatted_length(const char *fmt, va_list);
```

Do not print anything, return the required length of the formatted text.

## Global variables

```cpp
const char * fmt_default_type = "s";
```

Change this value to your desired default type.
This gets used for the empty replacement field `{}`.

```cpp
int fmt_format_initial_size = 16;
```

The initial size of the buffer allocated by the `fmt_format` functions.

### `fmt_format_impl` - The backbone of the library

All of the above functions are wrappers around the `fmt_format_impl` function:

```cpp
int fmt_format_impl(FmtPutch putch, char *buffer, int maxlen, const char *fmt, va_list args);
```

- `putch` function used to print a character.

- `buffer` The buffer to print to, can be `NULL` (i.e. when printing to a stream).

- `maxlen` maximum number of characters to write

- `fmt` format string

- `arg` arguments

In case you have some special requirements, like printing to non-contiguous memory, you can call this function directly with your own `putch` function.

The obvious limitation is that you cannot pass any additional arguments, so you'd either need to use global variables pass an argument through `buffer` using type punning.

```cpp
typedef void(*FmtPutch)(char **bufptr, char ch);
```

- `bufptr` A pointer to a pointer to the current position in buffer.
If the `buffer` argument to `fmt_format_impl` was `NULL`, `*bufptr` is `NULL`.
The pointer is not changed by `fmt_format_impl` and has to be advanced by the `putch` function.

- `ch` character to print

See `custom_putch_example.c` for an example on this.

## Building

Compile with `-DFMT_SUPPORT_TIME` to enable the `t` type specifier.

Compile with `-DFMT_SUPPORT_LOCALE` to enable the `n` locale flag.

Make options:

  - `NO_LOCALE=1` disable locale support

  - `NO_TIME=1` disable time support

  - `RELEASE=1` enable optimization

  - `PREFIX=/some/path` path to install to (defaults to `/usr/local/`)

## Requirements

`pandoc` to use `make doc`, compiling the README to a PDF.

When using the static library, you also need to link `-lm`.

## Acknowledgments

The core design with the `fmt_format_impl` and `putch` functions was inspired by [SerenityOS](https://github.com/SerenityOS/serenity)' [printf implementation](https://github.com/SerenityOS/serenity/blob/master/AK/PrintfImplementation.h).

## License

[MIT](https://choosealicense.com/licenses/mit/)

