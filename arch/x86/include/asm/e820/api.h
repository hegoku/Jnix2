#ifndef _ASM_E820_API_H
#define _ASM_E820_API_H

extern unsigned long e820__end_of_ram_pfn(void);
extern unsigned long e820__end_of_low_ram_pfn(void);

extern void e820__memblock_setup(void);

#endif /* _ASM_E820_API_H */