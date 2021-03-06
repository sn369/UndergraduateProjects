#ifndef __CONSOLE_H__
#define __CONSOLE_H__

struct SHTCTL;
struct SHEET;
struct TASK;

typedef struct CONSOLE {
  struct SHTCTL* shtctl;
  struct SHEET* sheet;
  int cur_x, cur_y, cur_c;
  int cmdp, cmdlen;
  struct TIMER* timer;
  unsigned char fontColor, bgColor;
} CONSOLE;

struct SHEET* open_console(struct SHTCTL* shtctl);
void close_constask(struct TASK* task);
void close_console(struct SHTCTL* shtctl, struct SHEET* sht);
struct TASK* open_constask(struct SHTCTL* shtctl, struct SHEET* sheet);

void cons_putchar(CONSOLE* cons, int chr, char move, char neg);
void cons_putstr0(CONSOLE* cons, const char* s);
void cons_putstr1(CONSOLE* cons, const char* s, int l);
void cons_printf(CONSOLE* cons, const char* format, ...);

#endif

