#include <uapi/asm/bootparam.h>
#include <asm/sections.h>
#include <asm/desc.h>
#include <jnix/init.h>
#include <uapi/asm/e820.h>
#include <lib/string.h>
#include <jnix/printk.h>
#include <asm/setup.h>
#include <asm/e820/api.h>
#include <mm/memblock.h>
#include <asm/page.h>
#include <mm/kmalloc.h>
#include <mm/mm.h>
#include <asm/irq.h>

/*
 * max_low_pfn_mapped: highest directly mapped pfn < 4 GB
 * max_pfn_mapped:     highest directly mapped pfn > 4 GB
 *
 * The direct mapping only covers E820_TYPE_RAM regions, so the ranges and gaps are
 * represented by pfn_mapped[].
 */
unsigned long max_low_pfn_mapped;
unsigned long max_pfn_mapped;

/*
 * Range of the BSS area. The size of the BSS area is determined
 * at link time, with RESERVE_BRK() facility reserving additional
 * chunks.
 */
unsigned long _brk_start = (unsigned long)__brk_base;
unsigned long _brk_end   = (unsigned long)__brk_base;

struct boot_params boot_params;

#define PTRS_PER_PGD 1024
RESERVE_BRK(pagetables, (LOWMEM_PAGES/PTRS_PER_PGD * PAGE_SIZE));


struct gdt_page __attribute__((section(".data..page_aligned"))) gdt_page  = {.gdt = {

	[GDT_ENTRY_KERNEL_CS] = GDT_ENTRY_INIT(0xc09a, 0, 0xfffff),
	[GDT_ENTRY_KERNEL_DS] = GDT_ENTRY_INIT(0xc092, 0, 0xfffff),
	[GDT_ENTRY_DEFAULT_USER_CS] = GDT_ENTRY_INIT(0xc0fa, 0, 0xfffff),
	[GDT_ENTRY_DEFAULT_USER_DS] = GDT_ENTRY_INIT(0xc0f2, 0, 0xfffff),
	/*
	* Segments used for calling PnP BIOS have byte granularity.
	* They code segments and data segments have fixed 64k limits,
	* the transfer segment sizes are set at run time.
	*/
	/* 32-bit code */
	[GDT_ENTRY_PNPBIOS_CS32] = GDT_ENTRY_INIT(0x409a, 0, 0xffff),
	/* 16-bit code */
	[GDT_ENTRY_PNPBIOS_CS16] = GDT_ENTRY_INIT(0x009a, 0, 0xffff),
	/* 16-bit data */
	[GDT_ENTRY_PNPBIOS_DS] = GDT_ENTRY_INIT(0x0092, 0, 0xffff),
	/* 16-bit data */
	[GDT_ENTRY_PNPBIOS_TS1] = GDT_ENTRY_INIT(0x0092, 0, 0),
	/* 16-bit data */
	[GDT_ENTRY_PNPBIOS_TS2] = GDT_ENTRY_INIT(0x0092, 0, 0),
	/*
	* The APM segments have byte granularity and their bases
	* are set at run time.  All have 64k limits.
	*/
	/* 32-bit code */
	[GDT_ENTRY_APMBIOS_BASE] = GDT_ENTRY_INIT(0x409a, 0, 0xffff),
	/* 16-bit code */
	[GDT_ENTRY_APMBIOS_BASE + 1] = GDT_ENTRY_INIT(0x009a, 0, 0xffff),
	/* data */
	[GDT_ENTRY_APMBIOS_BASE + 2] = GDT_ENTRY_INIT(0x4092, 0, 0xffff),

	[GDT_ENTRY_ESPFIX_SS] = GDT_ENTRY_INIT(0xc092, 0, 0xfffff),
	[GDT_ENTRY_PERCPU] = GDT_ENTRY_INIT(0xc092, 0, 0xfffff),
}};

static void __init reserve_brk(void)
{
	if (_brk_end > _brk_start)
		memblock_reserve(__pa(_brk_start),
				 _brk_end - _brk_start);

	/* Mark brk area as locked down and no longer taking any
	   new allocations */
	_brk_start = 0;
}

static void __init reserve_brk_print(void)
{
	printk("__brk_base:%#x  _brk_start:%#x  _brk_end:%#x  __brk_limit:%#x" \
	"  size:%#x  used:%#x\n", __brk_base, _brk_start, _brk_end, __brk_limit,
		(__brk_limit-__brk_base), _brk_end-(unsigned long)__brk_base
	);
}

void * __init extend_brk(size_t size, size_t align)
{
	size_t mask = align - 1;
	void *ret;

	if (_brk_start==0) {
		printk("reserve_brk done!\n");
		return 0;
	}
	// BUG_ON(_brk_start == 0);
	if (align & mask) {
		printk("extend_brk: not align, align:%#x, mask:%#x\n", align, mask);
		return 0;
	}
	// BUG_ON(align & mask);

	_brk_end = (_brk_end + mask) & ~mask;
	if ((char *)(_brk_end + size) > __brk_limit) {
		printk("extend_brk: exceed __brk_limit: %#x,%#x\n", _brk_end + size, __brk_limit);
		return 0;
	}
	// BUG_ON((char *)(_brk_end + size) > __brk_limit);

	ret = (void *)_brk_end;
	_brk_end += size;

	memset(ret, 0, size);

	return ret;
}

void __init setup_arch(char **cmdline_p)
{
	sanitize_e820_map();
	max_pfn = e820_end_of_ram_pfn();

	reserve_brk();
	reserve_brk_print();
	printk("max_pfn_mapped:%d max_pfn:%d\n", max_pfn_mapped, max_pfn);

	e820__memblock_setup();
	memblock_reserve(0xA000, 0xF6000); //video
	memblock_reserve(0x10000, __pa(_end)-0x10000); //kernel
	memblock_print();

	init_page();

	idt_setup_traps();
}