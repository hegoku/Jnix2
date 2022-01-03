const sys_call_ptr_t ia32_sys_call_table[] = {
	[20] = sys_getpid,
	[64] = sys_getppid,
	[224] = sys_gettid
};