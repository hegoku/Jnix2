#include <jnix/fs.h>
#include <jnix/init.h>

struct file_system_type rootfs_fs_type = {
	.name		= "rootfs",
	// .init_fs_context = rootfs_init_fs_context,
	// .kill_sb	= kill_litter_super,
};

void __init init_rootfs(void)
{
	register_filesystem(&rootfs_fs_type);
	// if (IS_ENABLED(CONFIG_TMPFS) && !saved_root_name[0] &&
	// 	(!root_fs_names || strstr(root_fs_names, "tmpfs")))
	// 	is_tmpfs = true;
}