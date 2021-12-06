#ifndef BOOT_STRING_H
#define BOOT_STRING_H

void *memcpy(void *dst, const void *src, size_t len) __attribute__((regparm(3)));
void *memset(void *dst, int c, size_t len)__attribute__((regparm(3)));
int memcmp(const void *s1, const void *s2, size_t len);

// #define memcpy(d,s,l) __builtin_memcpy(d,s,l)
// #define memset(d,c,l) __builtin_memset(d,c,l)
// #define memcmp	__builtin_memcmp

int strcmp(const char *str1, const char *str2);
int strncmp(const char *cs, const char *ct, size_t count);
size_t strlen(const char *s);
char *strstr(const char *s1, const char *s2);
extern char *strchr(const char *s, int c);
extern size_t strnlen(const char *s, size_t maxlen);
extern unsigned int atou(const char *s);

#endif /* BOOT_STRING_H */