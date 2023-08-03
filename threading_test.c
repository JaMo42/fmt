#define FMT_LOCKED_DEFAULT_PRINTERS
#define FMT_IMPLEMENTATION
#include "fmt.h"
#include <threads.h>

int worker(void *arg) {
    int number = (int)(uintptr_t)arg;
    for (int i = 0; i < 10; ++i) {
        fmt_println("{}: The quick brown fox jumps over the lazy dog", number);
    }
    return 0;
}

int main(void) {
    fmt_init_threading();
    thrd_t threads[10];
    static const int COUNT = sizeof(threads) / sizeof(*threads);
    for (int i = 0; i < COUNT; ++i) {
        thrd_create(threads + i, worker, (void *)(uintptr_t)i);
    }
    int unused;
    for (int i = 0; i < COUNT; ++i) {
        thrd_join(threads[i], &unused);
    }
}
