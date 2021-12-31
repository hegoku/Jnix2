#ifndef _JNIX_TYPES_H
#define _JNIX_TYPES_H

#ifndef __ASSEMBLER__

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long int
#define u64 unsigned long long int

#define __u8 u8
#define __u16 u16
#define __u32 u32
#define __u64 u64

#define uint unsigned int

#define NULL 0

typedef unsigned int size_t;
typedef u32 phys_addr_t;

typedef int pid_t;

typedef unsigned int fmode_t;

#endif /* ASSEMBLER */
#endif