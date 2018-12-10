#ifndef __X_LIB_H__
#define __X_LIB_H_

#include <stdio.h>
#include <stdbool.h>

typedef int (*callback)(char*, char*, int, void*);

int x_dirorfile(char* s);
int x_readdir(char* dir, callback lpfn, void* arg);
char* x_lastbyte(char* begin, int len, char ch, int* retlen);
bool x_ispair(char* buffer, int len, char c);

#endif
