#ifndef _JNIX_MOUNT_H
#define _JNIX_MOUNT_H

struct super_block;
struct vfsmount;
struct dentry;
// struct mnt_namespace;
struct fs_context;

struct vfsmount {
	struct dentry *mnt_root;	/* root of the mounted tree */
	struct super_block *mnt_sb;	/* pointer to superblock */
	int mnt_flags;
	// struct user_namespace *mnt_userns;
};

// extern struct vfsmount *vfs_kern_mount(struct file_system_type *type, int flags, const char *name, void *data);

#endif