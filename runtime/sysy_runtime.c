#include <stdarg.h>
#include <stdio.h>

int getint(void) {
  int x = 0;
  if (scanf("%d", &x) != 1) {
    return 0;
  }
  return x;
}

int getch(void) {
  int c = getchar();
  return c == EOF ? 0 : c;
}

float getfloat(void) {
  float x = 0.0f;
  if (scanf("%f", &x) != 1) {
    return 0.0f;
  }
  return x;
}

int getarray(int a[]) {
  int n = getint();
  for (int i = 0; i < n; ++i) {
    a[i] = getint();
  }
  return n;
}

int getfarray(float a[]) {
  int n = getint();
  for (int i = 0; i < n; ++i) {
    a[i] = getfloat();
  }
  return n;
}

void putint(int x) { printf("%d", x); }

void putch(int x) { putchar(x); }

void putfloat(float x) { printf("%a", x); }

void putarray(int n, int a[]) {
  printf("%d:", n);
  for (int i = 0; i < n; ++i) {
    printf(" %d", a[i]);
  }
  putchar('\n');
}

void putfarray(int n, float a[]) {
  printf("%d:", n);
  for (int i = 0; i < n; ++i) {
    printf(" %a", a[i]);
  }
  putchar('\n');
}

void putf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}

void _sysy_starttime(int lineno) { (void)lineno; }

void _sysy_stoptime(int lineno) { (void)lineno; }
