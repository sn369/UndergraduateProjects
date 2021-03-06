#include "math.h"
#include "stdarg.h"
#include "stdio.h"  // FALSE, TRUE
#include "string.h"

static const char hextableLower[] = "0123456789abcdef";
static const char hextableUpper[] = "0123456789ABCDEF";

static char* uint2num(char *s, unsigned int x, int base, const char* table,
                      char padding, int keta) {
  *(--s) = '\0';
  do {
    *(--s) = table[x % base];
    x /= base;
    --keta;
  } while (x > 0);
  for (; keta > 0; --keta)
    *(--s) = padding;
  return s;
}

static char* int2num(char *s, int x, int base, const char* table,
                     char padding, int keta) {
  int negative = FALSE;
  if (x < 0) {
    x = -x;
    negative = TRUE;
    --keta;
  }
  char* p = uint2num(s, x, base, table, padding, keta);
  if (negative)
    *(--p) = '-';
  return p;
}

char* putDouble(char* dst, double x, int keta, char padding) {
  if (isinf(x)) {
    if (x < 0)
      *dst++ = '-';
    strcpy(dst, "inf");
    return dst + 3;
  }
  if (isnan(x)) {
    strcpy(dst, "nan");
    return dst + 3;
  }

  char buf[sizeof(int) * 3 + 3];

  if (x < 0) {
    *dst++ = '-';
    x = -x;
  }
  int i = (int)x;
  char* q = int2num(&buf[sizeof(buf)], i, 10, hextableLower, padding, keta);
  while (*q != '\0')
    *dst++ = *q++;
  *dst++ = '.';
  double y = (x - i) * 1000000;
  int j = (int)(y + 0.5);
  q = int2num(&buf[sizeof(buf)], j, 10, hextableLower, '0', 6);
  while (*q != '\0')
    *dst++ = *q++;
  return dst;
}

int vsprintf(char *str, const char *fmt, va_list ap) {
  char* dst = str;
  while (*fmt != '\0') {
    if (*fmt != '%') {
      *dst++ = *fmt++;
      continue;
    }

    int keta = 0;
    int sign = TRUE;
    char padding = ' ';
    char buf[sizeof(int) * 3 + 3];
    char* q;
  again:
    switch (*(++fmt)) {
    default:
      *dst++ = *fmt++;
      continue;
    case '\0':
      continue;
    case '0': padding = '0'; goto again;
    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
      keta = *fmt - '0';
      goto again;
    case '.': goto again;  // TODO: Handle fraction specifier.
    case 'u': sign = FALSE; goto again;
    case 'c':
      {
        int c = va_arg(ap, int);
        *dst++ = c;
        ++fmt;
        continue;
      }
      break;
    case 'd':
      if (sign)
        q = int2num(&buf[sizeof(buf)], va_arg(ap, int), 10, hextableLower, padding, keta);
      else
        q = uint2num(&buf[sizeof(buf)], va_arg(ap, unsigned int), 10, hextableLower, padding, keta);
      break;
    case 'x': q = uint2num(&buf[sizeof(buf)], va_arg(ap, int), 16, hextableLower, padding, keta); break;
    case 'X': q = uint2num(&buf[sizeof(buf)], va_arg(ap, int), 16, hextableUpper, padding, keta); break;
    case 'p':
      q = uint2num(&buf[sizeof(buf)], va_arg(ap, int), 16, hextableLower, '0', sizeof(void*) * 2);
      *(--q) = 'x';
      *(--q) = '0';
      break;
    case 's':
      q = va_arg(ap, char*);
      break;
    case 'f':
      {
        double x = va_arg(ap, double);
        dst = putDouble(dst, x, keta, padding);
        ++fmt;
        continue;
      }
      break;
    }

    while (*q != '\0')
      *dst++ = *q++;
    ++fmt;
  }
  *dst = '\0';
  return dst - str;
}
