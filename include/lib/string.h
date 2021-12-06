#ifndef STRING_H
#define STRING_H

#include <jnix/types.h>

void *memcpy(void *dst, const void *src, size_t len);
void *memset(void *dst, int c, size_t len);
int memcmp(const void *s1, const void *s2, size_t len);

size_t strcmp(const char *str1, const char *str2);
int strncmp(const char *cs, const char *ct, size_t count);
size_t strlen(const char *s);
char *strstr(const char *s1, const char *s2);
extern char *strchr(const char *s, int c);
size_t strnlen(const char *s, size_t maxlen);
extern unsigned int atou(const char *s);

void *memmove(void *dest, const void *src, size_t n);
#endif