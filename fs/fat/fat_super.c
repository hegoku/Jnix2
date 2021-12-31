#include <jnix/fs.h>
#include <jnix/init.h>
#include <jnix/printk.h>

struct file_system_type fat_system_type = {
	.name = "fat"
};

int init_fat(void)
{
	return register_filesystem(&fat_system_type);
}

core_initcall(init_fat);