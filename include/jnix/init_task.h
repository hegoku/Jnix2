#ifndef _JNIX__INIT_TASK_H
#define _JNIX__INIT_TASK_H

#define __init_task_data __section(".data..init_task")
#define __init_thread_info __section(".data..init_thread_info")

#define INIT_TASK_COMM "swapper"

#endif