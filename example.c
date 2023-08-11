// This file contains the example from the readme to ensure it actually works
#define FMT_IMPLEMENTATION
#include "fmt.h"

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
