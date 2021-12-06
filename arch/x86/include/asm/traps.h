#ifndef _ASM_X86_TRAPS_H
#define _ASM_X86_TRAPS_H

enum {
	X86_TRAP_DE = 0,	/*  0, 除零错误 */
	X86_TRAP_DB,		/*  1, 调试 */
	X86_TRAP_NMI,		/*  2, 不可屏蔽中断 */
	X86_TRAP_BP,		/*  3, 断点 */
	X86_TRAP_OF,		/*  4, 溢出 */
	X86_TRAP_BR,		/*  5, 超出范围 */
	X86_TRAP_UD,		/*  6, 操作码无效 */
	X86_TRAP_NM,		/*  7, 设备不可用 */
	X86_TRAP_DF,		/*  8, 双精度浮点错误 */
	X86_TRAP_OLD_MF,	/*  9, 协处理器段溢出 */
	X86_TRAP_TS,		/* 10, 无效的 TSS */
	X86_TRAP_NP,		/* 11, 段不存在 */
	X86_TRAP_SS,		/* 12, 堆栈段故障 */
	X86_TRAP_GP,		/* 13, 一般保护故障 */
	X86_TRAP_PF,		/* 14, 页错误 */
	X86_TRAP_SPURIOUS,	/* 15, 伪中断 */
	X86_TRAP_MF,		/* 16, x87 浮点异常 */
	X86_TRAP_AC,		/* 17, 对齐检查 */
	X86_TRAP_MC,		/* 18, 机器检测 */
	X86_TRAP_XF,		/* 19, SIMD （单指令多数据结构浮点）异常 */
	X86_TRAP_IRET = 32,	/* 32, IRET （中断返回）异常 */
};

#endif