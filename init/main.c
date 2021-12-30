#include <jnix/init.h>
#include <jnix/console.h>
#include <jnix/printk.h>
#include <jnix/interrupt.h>
#include <mm/mm.h>
#include <mm/memblock.h>
#include <mm/kmalloc.h>
#include <jnix/sched/task.h>
#include <uapi/jnix/sched.h>
#include <jnix/binfmts.h>
#include <jnix/sched/init.h>
#include <jnix/kthread.h>

#define CONFIG_INIT_ENV_ARG_LIMIT 10
#define MAX_INIT_ARGS CONFIG_INIT_ENV_ARG_LIMIT
#define MAX_INIT_ENVS CONFIG_INIT_ENV_ARG_LIMIT

static __initdata int kthreadd_done = 0;

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

static const char *argv_init[MAX_INIT_ARGS+2] = { "init", NULL, };
const char *envp_init[MAX_INIT_ENVS+2] = { "HOME=/", "TERM=linux", NULL, };

static int kernel_init(void *);

void __ref rest_init(void)
{
	struct task_struct *tsk;
	int pid;

	// rcu_scheduler_starting();
	/*
	 * We need to spawn init first so that it obtains pid 1, however
	 * the init task will end up wanting to create kthreads, which, if
	 * we schedule it before we create kthreadd, will OOPS.
	 */
	pid = kernel_thread(kernel_init, NULL, CLONE_FS);
	/*
	 * Pin init on the boot CPU. Task migration is not properly working
	 * until sched_init_smp() has been run. It will set the allowed
	 * CPUs for init to the non isolated CPUs.
	 */
	// rcu_read_lock();
	// tsk = find_task_by_pid_ns(pid, &init_pid_ns);
	// tsk->flags |= PF_NO_SETAFFINITY;
	// set_cpus_allowed_ptr(tsk, cpumask_of(smp_processor_id()));
	// rcu_read_unlock();

	// numa_default_policy();
	pid = kernel_thread(kthreadd, NULL, CLONE_FS | CLONE_FILES);
	// rcu_read_lock();
	// kthreadd_task = find_task_by_pid_ns(pid, &init_pid_ns);
	// rcu_read_unlock();

	/*
	 * Enable might_sleep() and smp_processor_id() checks.
	 * They cannot be enabled earlier because with CONFIG_PREEMPTION=y
	 * kernel_thread() would trigger might_sleep() splats. With
	 * CONFIG_PREEMPT_VOLUNTARY=y the init task might have scheduled
	 * already, but it's stuck on the kthreadd_done completion.
	 */
	// system_state = SYSTEM_SCHEDULING;

	// complete(&kthreadd_done);
	kthreadd_done = 1;

	/*
	 * The boot idle thread must execute schedule()
	 * at least once to get things moving:
	 */
	// schedule_preempt_disabled();
	/* Call into cpu_idle with preempt disabled */
	// cpu_startup_entry(CPUHP_ONLINE);
}

void __init __attribute__((weak)) arch_call_rest_init(void)
{
	rest_init();
}

__attribute__((regparm(0))) void __init __attribute__((no_sanitize_address)) start_kernel(void)
{
	char *command_line;

	console_init();

	setup_arch(&command_line);

	init_mm();
	memblock_print();
	init_bucket_desc();

	sched_init();

	early_irq_init();
	init_IRQ();

	time_init();

	if (late_time_init)
		late_time_init();

	do_basic_setup();
	int *a = 100;
	*a = 0x98;

	arch_call_rest_init();
	printk("done!\n");
	for (;;)
	{
		printk("main ");
	}
}

static int run_init_process(const char *init_filename)
{
	const char *const *p;

	argv_init[0] = init_filename;
	printk("Run %s as init process\n", init_filename);
	printk("  with arguments:\n");
	for (p = argv_init; *p; p++)
		printk("    %s\n", *p);
	printk("  with environment:\n");
	for (p = envp_init; *p; p++)
		printk("    %s\n", *p);
	return kernel_execve(init_filename, argv_init, envp_init);
}

static int __ref kernel_init(void *unused)
{
	int ret;

	while(!kthreadd_done){}
	for(;;){
		printk("kernel_init ");
		schedule();
	}

	ret = run_init_process("/sbin/init");
		if (!ret)
			return 0;
		printk("Requested init %s failed (error %d).",
		      "/sbin/init", ret);
	for(;;){}
}