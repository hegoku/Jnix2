#include <lib/string.h>

// static void *____memcpy(void *dest, const void *src, size_t n)
// {
// 	int d0, d1, d2;
// 	asm volatile(
// 		"rep ; movsl\n\t"
// 		"movl %4,%%ecx\n\t"
// 		"rep ; movsb\n\t"
// 		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
// 		: "0" (n >> 2), "g" (n & 3), "1" (dest), "2" (src)
// 		: "memory");

// 	return dest;
// }

size_t strlen(const char * s)
{
    /*
    * slightly slower on a 486, but with better chances of
    * register allocation
    */
    register char dummy, *tmp= (char *)s;
    __asm__ __volatile__(
        "\n1:\t"
        "movb\t(%0),%1\n\t"
        "incl\t%0\n\t"
        "testb\t%1,%1\n\t"
        "jne\t1b"
        :"=r" (tmp),"=q" (dummy)
        :"0" (s)
        : "memory" );
    return (tmp-s-1);
}