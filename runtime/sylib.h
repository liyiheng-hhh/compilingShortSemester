#ifndef SYLIB_H
#define SYLIB_H

#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Input functions */
int getint(void);
int getch(void);
float getfloat(void);
int getarray(int a[]);
int getfarray(float a[]);

/* Output functions */
void putint(int x);
void putch(int x);
void putfloat(float x);
void putarray(int n, int a[]);
void putfarray(int n, float a[]);

/* Timing functions */
void starttime(void);
void stoptime(void);

#ifdef __cplusplus
}
#endif

#endif /* SYLIB_H */
