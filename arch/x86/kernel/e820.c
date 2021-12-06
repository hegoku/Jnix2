#include <uapi/asm/e820.h>
#include <asm/setup.h>
#include <jnix/init.h>
#include <uapi/asm/bootparam.h>
#include <jnix/printk.h>
#include <asm/e820/api.h>
#include <mm/memblock.h>

struct change_member {
	/* Pointer to the original entry: */
	// struct e820_entry	*entry;
	struct boot_e820_entry *entry;
	/* Address for this change point: */
	unsigned long long	addr;
};

static struct change_member	change_point_list[2*E820_MAX_ENTRIES]	__initdata;
static struct change_member	*change_point[2*E820_MAX_ENTRIES]	__initdata;
static struct boot_e820_entry	*overlap_list[E820_MAX_ENTRIES]		__initdata;
static struct boot_e820_entry	new_entries[E820_MAX_ENTRIES]		__initdata;

int sanitize_e820_map()
{
	int nr = boot_params.e820_entries;
	int i = 0;
	for (i = 0; i < boot_params.e820_entries; i++)
	{
		if (boot_params.e820_table[i].addr + boot_params.e820_table[i].size < boot_params.e820_table[i].addr)
			return -1;
	}

	for (i = 0; i < 2 * boot_params.e820_entries; i++)
		change_point[i] = &change_point_list[i];
	
	int chg_idx = 0;
	for (i = 0; i < boot_params.e820_entries; i++)	{
		if (boot_params.e820_table[i].size != 0) {
			change_point[chg_idx]->addr	= boot_params.e820_table[i].addr;
			change_point[chg_idx++]->entry = &boot_params.e820_table[i];
			change_point[chg_idx]->addr	= boot_params.e820_table[i].addr + boot_params.e820_table[i].size;
			change_point[chg_idx++]->entry	= &boot_params.e820_table[i];
		}
	}
	int chg_nr = chg_idx;

	//sort
	struct change_member *tmp;
	for (i = 0; i < chg_nr-1; i++)
	{
		for (int j = 0; j < chg_nr-1-i; j++)
		{
			if (change_point[j]->addr!=change_point[j+1]->addr) {
				if (change_point[j]->addr > change_point[j+1]->addr) {
					tmp = change_point[j];
					change_point[j] = change_point[j+1];
					change_point[j] = tmp;
				}
				else
				{
					if (((change_point[j]->addr != change_point[j]->entry->addr) - (change_point[j+1]->addr != change_point[j+1]->entry->addr))==1) {
						tmp = change_point[j];
						change_point[j] = change_point[j+1];
						change_point[j] = tmp;
					}
				}
			}
		}
		
	}

	int current_type, last_type=0;
	u32 new_nr_entries=0, overlap_entries=0;
	int max_nr_entries = boot_params.e820_entries;
	unsigned long long last_addr=0;
	/* Loop through change-points, determining effect on the new map: */
	for (chg_idx = 0; chg_idx < chg_nr; chg_idx++) {
		/* Keep track of all overlapping entries */
		if (change_point[chg_idx]->addr == change_point[chg_idx]->entry->addr) {
			/* Add map entry to overlap list (> 1 entry implies an overlap) */
			overlap_list[overlap_entries++] = change_point[chg_idx]->entry;
		} else {
			/* Remove entry from list (order independent, so swap with last): */
			for (i = 0; i < overlap_entries; i++) {
				if (overlap_list[i] == change_point[chg_idx]->entry)
					overlap_list[i] = overlap_list[overlap_entries-1];
			}
			overlap_entries--;
		}
		/*
		 * If there are overlapping entries, decide which
		 * "type" to use (larger value takes precedence --
		 * 1=usable, 2,3,4,4+=unusable)
		 */
		current_type = 0;
		for (i = 0; i < overlap_entries; i++) {
			if (overlap_list[i]->type > current_type)
				current_type = overlap_list[i]->type;
		}

		/* Continue building up new map based on this information: */
		if (current_type != last_type) {
			if (last_type != 0)	 {
				new_entries[new_nr_entries].size = change_point[chg_idx]->addr - last_addr;
				/* Move forward only if the new size was non-zero: */
				if (new_entries[new_nr_entries].size != 0)
					/* No more space left for new entries? */
					if (++new_nr_entries >= max_nr_entries)
						break;
			}
			if (current_type != 0)	{
				new_entries[new_nr_entries].addr = change_point[chg_idx]->addr;
				new_entries[new_nr_entries].type = current_type;
				last_addr = change_point[chg_idx]->addr;
			}
			last_type = current_type;
		}
	}
	memcpy(boot_params.e820_table, new_entries, new_nr_entries*sizeof(*boot_params.e820_table));
	boot_params.e820_entries = new_nr_entries;

	__u64 usable_total = 0;
	for (int i = 0; i < boot_params.e820_entries; i++)
	{
		__u64 size = boot_params.e820_table[i].addr + boot_params.e820_table[i].size - 1;
		printk("BIOS-e820: [mem %#010x%08x - %#010x%08x] %d\n", (__u32)(boot_params.e820_table[i].addr >> 32), (__u32)boot_params.e820_table[i].addr,
			   (__u32)(size>>32), (__u32)size, boot_params.e820_table[i].type);
		if (boot_params.e820_table[i].type==1) {
			usable_total += boot_params.e820_table[i].size;
		}
	}
	printk("Total Memory: %dMB\n", (__u32)(usable_total / 1024 / 1024));

	return 0;
}

void __init e820__memblock_setup(void)
{
    int i;
    u64 end;

    for (i = 0; i < boot_params.e820_entries; i++) {
        struct boot_e820_entry *ei = &boot_params.e820_table[i];

        end = ei->addr + ei->size;
        // if (end != (resource_size_t)end)
            // continue;

        if (ei->type != E820_RAM && ei->type != E820_RESERVED_KERN)
            continue;

        memblock_add(ei->addr, ei->size);
    }

    /* throw away partial pages */
    // memblock_trim_memory(PAGE_SIZE);

    // memblock_dump_all();
}