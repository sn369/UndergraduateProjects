#include "window.h"
#include "graphics.h"
#include "sheet.h"

static void make_wtitle8(unsigned char* buf, int xsize, const char* title, char act) {
  static const char closebtn[14][16] = {
    "OOOOOOOOOOOOOOO@",
    "OQQQQQQQQQQQQQ$@",
    "OQQQQQQQQQQQQQ$@",
    "OQQQ@@QQQQ@@QQ$@",
    "OQQQQ@@QQ@@QQQ$@",
    "OQQQQQ@@@@QQQQ$@",
    "OQQQQQQ@@QQQQQ$@",
    "OQQQQQ@@@@QQQQ$@",
    "OQQQQ@@QQ@@QQQ$@",
    "OQQQ@@QQQQ@@QQ$@",
    "OQQQQQQQQQQQQQ$@",
    "OQQQQQQQQQQQQQ$@",
    "O$$$$$$$$$$$$$$@",
    "@@@@@@@@@@@@@@@@",
  };
  static const unsigned char table[][2] = {
    { '@', COL8_BLACK },
    { '$', COL8_DARK_GRAY },
    { 'Q', COL8_GRAY },
    { 'O', COL8_WHITE },
  };
  unsigned char tc, tbc;
  if (act) {
    tc = COL8_WHITE;
    tbc = COL8_DARK_BLUE;
  } else {
    tc = COL8_GRAY;
    tbc = COL8_DARK_GRAY;
  }
  boxfill8(buf, xsize, tbc, 3, 3, xsize - 3, 21);
  putfonts8_asc(buf, xsize, 24, 25, 4, COL8_BLACK, title);
  putfonts8_asc(buf, xsize, 24, 24, 4, tc, title);
  convert_image8(buf, xsize, xsize - 21, 5, 16, 14, &closebtn[0][0], &table[0][0]);
}

void make_window8(struct SHEET* sheet, const char* title, char act) {
  draw_shaded_box(sheet->buf, sheet->bxsize, 0, 0, sheet->bxsize, sheet->bysize, COL8_GRAY, COL8_BLACK, -1);
  draw_shaded_box(sheet->buf, sheet->bxsize, 1, 1, sheet->bxsize - 1, sheet->bysize - 1, COL8_WHITE, COL8_DARK_GRAY, COL8_GRAY);
  make_wtitle8(sheet->buf, sheet->bxsize, title, act);
}

void change_wtitle8(SHTCTL* shtctl, SHEET* sheet, char act) {
  int tc_new, tbc_new, tc_old, tbc_old;
  if (act) {
    tc_new = COL8_WHITE;
    tbc_new = COL8_DARK_BLUE;
    tc_old = COL8_GRAY;
    tbc_old = COL8_DARK_GRAY;
  } else {
    tc_new = COL8_GRAY;
    tbc_new = COL8_DARK_GRAY;
    tc_old = COL8_WHITE;
    tbc_old = COL8_DARK_BLUE;
  }

  unsigned char* buf = sheet->buf;
  int xsize = sheet->bxsize;
  for (int y = 3; y < 20; ++y) {
    for (int x = 3; x < xsize - 3; ++x) {
      unsigned char c = buf[y * xsize + x];
      if (c == tc_old && x <= xsize - 22) {
        c = tc_new;
      } else if (c == tbc_old) {
        c = tbc_new;
      }
      buf[y * xsize + x] = c;
    }
  }
  sheet_refresh(shtctl, sheet, 3, 3, xsize, 21);
}

void make_textbox8(SHEET* sheet, int x0, int y0, int sx, int sy, int c) {
  int x1 = x0 + sx, y1 = y0 + sy;
  draw_shaded_box(sheet->buf, sheet->bxsize, x0 - 2, y0 - 2, x1 + 2, y1 + 2, COL8_GRAY, COL8_WHITE, -1);
  draw_shaded_box(sheet->buf, sheet->bxsize, x0 - 1, y0 - 1, x1 + 1, y1 + 1, COL8_DARK_GRAY, COL8_GRAY, c);
}

void putfonts8_asc_sht(SHTCTL* shtctl, SHEET* sheet, int x, int y, int c, int b, const char* s, int l) {
  boxfill8(sheet->buf, sheet->bxsize, b, x, y, x + l * 8, y + 16);
  putfonts8_asc(sheet->buf, sheet->bxsize, sheet->bysize, x, y, c, s);
  sheet_refresh(shtctl, sheet, x, y, x + l * 8, y + 16);
}
