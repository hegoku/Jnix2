#include <jnix/init.h>
#include <jnix/console.h>
#include <jnix/printk.h>
#include <jnix/interrupt.h>
#include <mm/mm.h>
#include <mm/memblock.h>
#include <mm/kmalloc.h>

extern void init_IRQ(void);

extern void time_init(void);
/* Default late time init is NULL. archs can override this later. */
void (*__initdata late_time_init)(void);

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

extern initcall_entry_t __initcall_start[];
extern initcall_entry_t __initcall0_start[];
extern initcall_entry_t __initcall1_start[];
extern initcall_entry_t __initcall2_start[];
extern initcall_entry_t __initcall3_start[];
extern initcall_entry_t __initcall4_start[];
extern initcall_entry_t __initcall5_start[];
extern initcall_entry_t __initcall6_start[];
extern initcall_entry_t __initcall7_start[];
extern initcall_entry_t __initcall_end[];

static initcall_t *initcall_levels[] __initdata = {
	__initcall0_start,
	__initcall1_start,
	__initcall2_start,
	__initcall3_start,
	__initcall4_start,
	__initcall5_start,
	__initcall6_start,
	__initcall7_start,
	__initcall_end,
};

/* Keep these in sync with initcalls in include/linux/init.h */
static const char *initcall_level_names[] __initdata = {
	"pure",
	"core",
	"postcore",
	"arch",
	"subsys",
	"fs",
	"device",
	"late",
};

int __init do_one_initcall(initcall_t fn)
{
	return fn();
}

static void __init do_initcall_level(int level, char *command_line)
{
	initcall_entry_t *fn;

	// parse_args(initcall_level_names[level],
	// 	   command_line, __start___param,
	// 	   __stop___param - __start___param,
	// 	   level, level,
	// 	   NULL, ignore_unknown_bootoption);

	// trace_initcall_level(initcall_level_names[level]);
	for (fn = initcall_levels[level]; fn < initcall_levels[level+1]; fn++)
		do_one_initcall(initcall_from_entry(fn));
}

static void __init do_initcalls(void)
{
	int level;

	for (level = 0; level < ARRAY_SIZE(initcall_levels) - 1; level++)
		do_initcall_level(level, NULL);
}

static void __init do_basic_setup(void)
{
	// cpuset_init_smp();
	// driver_init();
	// init_irq_proc();
	// do_ctors();
	// usermodehelper_enable();
	do_initcalls();
}

__attribute__((regparm(0))) void __init __attribute__((no_sanitize_address)) start_kernel(void)
{
	char *command_line;

	console_init();

	setup_arch(&command_line);

	init_mm();
	memblock_print();
	init_bucket_desc();

	early_irq_init();
	init_IRQ();

	time_init();

	if (late_time_init)
		late_time_init();

	do_basic_setup();
	int *a = 100;
	*a = 0x98;
	printk("done!\n");
	for (;;)
	{
	}
}