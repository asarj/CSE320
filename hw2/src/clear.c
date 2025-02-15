#include <stdio.h>
#ifdef TERMINFO
#include <term.h>
#endif
#undef putchar

// #include "helper.h" // MADE CHANGE HERE - added import statement

int putchar();
// static int ok_to_clear; // MADE CHANGE HERE - removed unused variable

#ifdef TERMCAP
static char clear_screen[128] = 0;
static int lines;
#endif

void clearinit () // MADE CHANGE HERE - added return identifier
{
#ifdef TERMINFO
  int i;
  setupterm(getenv("TERM"),1,&i);
  ok_to_clear = (i == 1) ? 1 : 0;
  if (i != 1) {
     fprintf(stderr,"Warning: Terminal type unknown\n");
  }
  return (i == 1) ? 0 : -1;
#endif
#ifdef TERMCAP
  char tc[1024];
  char *ptr = clear_screen;

  if (tgetent(tc, getenv("TERM")) < 1) {
    ok_to_clear = 0;
    return;
  }
  tgetstr("cl", &ptr);
  lines = tgetnum("li");
  ok_to_clear = (clear_screen[0] != 0 && lines > 0);

#endif
}

void clear_the_screen () // MADE CHANGE HERE - added return identifier
{
#ifdef TERMINFO
  if (!ok_to_clear) return;
  tputs(clear_screen,lines,putchar);
  fflush(stdout);
#endif
#ifdef TERMCAP
  if (!ok_to_clear) return;
  tputs(clear_screen,lines,putchar);
  fflush(stdout);
#endif
}