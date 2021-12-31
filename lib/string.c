#include <lib/string.h>
#include <jnix/types.h>

char *strchr(const char *str, int c)
{
	for (; *str != (char)c; ++str)
		if (*str == '\0')
			return NULL;
	return (char *)str;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	char *tmp = dest;

	while (n) {
		if ((*tmp = *src) != 0)
			src++;
		tmp++;
		n--;
	}
	return dest;
}

size_t strlcpy(char *dest, const char *src, size_t n)
{
	size_t ret = strlen(src);
	if (n) {
		size_t len = (ret>=n) ? n-1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}
	return ret;
}

size_t strcspn(const char *str1, const char *str2)
{
	const char *p;
	const char *r;
	size_t count = 0;

	for (p = str1; *p != '\0'; ++p) {
		for (r = str2; *r != '\0'; ++r) {
			if (*p == *r)
				return count;
		}
		++count;
	}
	return count;
}

size_t strcmp(const char * str1, const char *str2)
{
	if ((str1 == 0) || (str2 == 0)) { /* for robustness */
		return (str1 - str2);
	}

	const char * p1 = str1;
	const char * p2 = str2;

	for (; *p1 && *p2; p1++,p2++) {
		if (*p1 != *p2) {
			break;
		}
	}

	return (*p1 - *p2);
}

int strncmp(const char *cs, const char *ct, size_t count)
{
	unsigned char c1, c2;

	while (count) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
		count--;
	}
	return 0;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	char *tmp = dest;
	const char *s = src;

	while (n--)
		*tmp++ = *s++;
	return dest;
}

int memcmp(const void *str1, const void *str2, size_t n)
{
	const unsigned char *su1, *su2;
	int res = 0;

	for (su1 = str1, su2 = str2; 0 < n; ++su1, ++su2, n--)
		if ((res = *su1 - *su2) != 0)
			break;
	return res;
}

size_t strnlen(const char *s, size_t maxlen)
{
	const char *es = s;
	while (*es && maxlen) {
		es++;
		maxlen--;
	}

	return (es - s);
}

size_t strlen(const char *s)
{
	size_t len = 0;
	const char *ptr = s;
	while(*ptr!='\n') {
		len++;
		ptr++;
	}
	return len;
}

void *memset(void *str, int c, size_t n)
{
	char *xs = str;

	while (n--)
		*xs++ = c;
	return str;
}

void* memmove(void* dest, const void* src, size_t n)
{
    char*     d  = (char*) dest;
    const char*  s = (const char*) src;
  
    if (s>d)
    {
         // start at beginning of s
         while (n--)
            *d++ = *s++;
    }
    else if (s<d)
    {
        // start at end of s
        d = d+n-1;
        s = s+n-1;
  
        while (n--)
           *d-- = *s--;
    }
    return dest;
}