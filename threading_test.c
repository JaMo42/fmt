#define FMT_LOCKED_DEFAULT_PRINTERS
#define FMT_IMPLEMENTATION
#include "fmt.h"
#include <threads.h>
#include <time.h>

static const struct timespec t_sleep = {
    .tv_sec = 0,
    .tv_nsec = 1000,
};

int worker(void *arg) {
    int number = (int)(uintptr_t)arg;
    for (int i = 0; i < 10; ++i) {
        fmt_println("{}: The quick brown fox jumps over the lazy dog", number);
        thrd_yield();
        thrd_sleep(&t_sleep, NULL);
        thrd_yield();
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
    for (int i = 0; i < COUNT; ++i) {
        thrd_join(threads[i], NULL);
    }
}
