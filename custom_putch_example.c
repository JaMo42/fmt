#include "fmt/fmt.h"
#include <stdio.h>  // putchar
#include <stdlib.h>  // malloc/free
#include <limits.h>  // INT_MAX

// Suppose we want to print to a linked list for some reason.
typedef struct List
{
  char ch;
  struct List *next;
} List;

#if 1
// Using a global variable.
// not threadsafe.

static List *current_list;

void list_putch(char **bufptr, char ch)
{
  (void)bufptr;
  current_list->ch = ch;
  current_list->next = malloc(sizeof(List));
  current_list = current_list->next;
  current_list->next = NULL;
}

int list_print(List *l, const char *fmt, ...)
{
  current_list = l;
  va_list args;
  va_start(args, fmt);
  int len = fmt_format_impl(list_putch, NULL, INT_MAX, fmt, args);
  va_end(args);
  return len;
}
#else
// Using type punning
// less clean, but threadsafe

typedef union
{
  List **l;
  char *s;
} ListConv;

void list_putch(char **nodeptr, char ch)
{
  ListConv conv = { .s = *nodeptr };
  (*conv.l)->ch = ch;
  (*conv.l)->next = malloc(sizeof(List));
  *conv.l = (*conv.l)->next;
  (*conv.l)->next = NULL;
}

int list_print(List *l, const char *fmt, ...)
{
  ListConv conv = { .l = &l };
  va_list args;
  va_start(args, fmt);
  int len = fmt_format_impl(list_putch, conv.s, INT_MAX, fmt, args);
  va_end(args);
  return len;
}

#endif

void free_list(List *p)
{
  if (p->next)
    free_list(p->next);
  free(p);
}

int main()
{
  List *l = malloc(sizeof(List));

  list_print(l, "{} {c} {f:.3}\n", "pi", '=', 3.1415926);

  // Print and clean up list
  for (List *p = l; p->next; p = p->next)
    putchar(p->ch);
  free_list(l);
}

