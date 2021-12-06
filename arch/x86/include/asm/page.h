#ifndef _ASM_X86_PAGE_H
#define _ASM_X86_PAGE_H

#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL<<PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))
#define PTE_PFN_MASK 0xFFC00000

#define PGDIR_SHIFT 20

#define CONFIG_PAGE_OFFSET 0xC0000000
#define __PAGE_OFFSET_BASE	CONFIG_PAGE_OFFSET
#define __PAGE_OFFSET		__PAGE_OFFSET_BASE
#define PAGE_OFFSET __PAGE_OFFSET

#define __START_KERNEL_map	__PAGE_OFFSET

#define THREAD_SIZE_ORDER	1
#define THREAD_SIZE		(PAGE_SIZE << THREAD_SIZE_ORDER)


#define IRQ_STACK_SIZE		THREAD_SIZE

#define N_EXCEPTION_STACKS	1

/*
 * User space process size: 3GB (default).
 */
#define IA32_PAGE_OFFSET	__PAGE_OFFSET
#define TASK_SIZE		__PAGE_OFFSET
#define TASK_SIZE_LOW		TASK_SIZE
#define TASK_SIZE_MAX		TASK_SIZE
#define DEFAULT_MAP_WINDOW	TASK_SIZE
#define STACK_TOP		TASK_SIZE
#define STACK_TOP_MAX		STACK_TOP


#define PTE_IDENT_ATTR	 0x003		/* PRESENT+RW */
#define PDE_IDENT_ATTR	 0x067		/* PRESENT+RW+DIRTY+ACCESSED */
#define PGD_IDENT_ATTR	 0x001		/* PRESENT (no other attributes) */

#define LOWMEM_PAGES (((((2ULL)<<31) - __PAGE_OFFSET) >> PAGE_SHIFT))

#ifndef __ASSEMBLER__

#ifdef __pa
#undef __pa
#endif
#define __pa(x)  ((unsigned long)(x) - PAGE_OFFSET)

#define __pa_symbol(x) \
	__phys_addr_symbol(__phys_reloc_hide((unsigned long)(x)))

#ifndef __va
#define __va(x)			((void *)((unsigned long)(x)+PAGE_OFFSET))
#endif

typedef unsigned long	pteval_t;
typedef unsigned long	pmdval_t;
typedef unsigned long	pudval_t;
typedef unsigned long	p4dval_t;
typedef unsigned long	pgdval_t;
typedef unsigned long	pgprotval_t;

typedef struct { pgdval_t pgd; } pgd_t;
typedef struct {
	pteval_t pte;
} pte_t;


extern pgd_t swapper_pg_dir[1024];


extern unsigned long max_low_pfn_mapped;
extern unsigned long max_pfn_mapped;
#endif /* __ASSEMBLER__ */

#endif /*_ASM_X86_PAGE_H*/