#include <asm/page.h>
#include <jnix/mm_types.h>

struct mm_struct init_task_mm = {
	.pgd = swapper_pg_dir
};

void setup_initial_init_mm(void *start_code, void *end_code, void *end_data, void *brk)
{
	init_task_mm.start_code = (unsigned long)start_code;
	init_task_mm.end_code = (unsigned long)end_code;
	init_task_mm.end_code = (unsigned long)end_data;
	init_task_mm.brk = (unsigned long)brk;
}