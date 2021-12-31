#include <jnix/sched.h>
#include <asm/current.h>
#include <jnix/sched/task.h>
#include <asm/processor.h>

struct task_struct *current_task = &init_task;
unsigned long cpu_current_top_of_stack = TOP_OF_INIT_STACK;