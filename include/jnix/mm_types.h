#ifndef _JNIX_MM_TYPES_H
#define _JNIX_MM_TYPES_H

#include <asm/page.h>

struct mm_struct {
	pgd_t * pgd;

	unsigned long start_code, end_code, start_data, end_data;
	unsigned long start_brk, brk, start_stack;
	unsigned long arg_start, arg_end, env_start, env_end;
};

extern struct mm_struct init_task_mm;

#endif