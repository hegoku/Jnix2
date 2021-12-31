#ifndef _JNIX_FS_H
#define _JNIX_FS_H

#include <jnix/list.h>
#include <jnix/types.h>

struct super_block {
	struct list_head	s_list;		/* Keep this first */
	// dev_t			s_dev;		/* search index; _not_ kdev_t */
	// unsigned char		s_blocksize_bits;
	// unsigned long		s_blocksize;
	// loff_t			s_maxbytes;	/* Max file size */
	// struct file_system_type	*s_type;
	// const struct super_operations	*s_op;
	// const struct dquot_operations	*dq_op;
	// const struct quotactl_ops	*s_qcop;
	// const struct export_operations *s_export_op;
	// unsigned long		s_flags;
	// unsigned long		s_iflags;	/* internal SB_I_* flags */
	// unsigned long		s_magic;
	// struct dentry		*s_root;
	// struct rw_semaphore	s_umount;
	// int			s_count;
	// atomic_t		s_active;
	// struct hlist_bl_head	s_roots;	/* alternate root dentries for NFS */
	// struct list_head	s_mounts;	/* list of mounts; _not_ for fs use */
	// struct block_device	*s_bdev;
	// struct backing_dev_info *s_bdi;
	// struct mtd_info		*s_mtd;
	// struct hlist_node	s_instances;

	// struct sb_writers	s_writers;

	// /*
	//  * Keep s_fs_info, s_time_gran, s_fsnotify_mask, and
	//  * s_fsnotify_marks together for cache efficiency. They are frequently
	//  * accessed and rarely modified.
	//  */
	// void			*s_fs_info;	/* Filesystem private info */

	// /* Granularity of c/m/atime in ns (cannot be worse than a second) */
	// u32			s_time_gran;
	// /* Time limits for c/m/atime in seconds */
	// // time64_t		   s_time_min;
	// // time64_t		   s_time_max;

	// char			s_id[32];	/* Informational name */
	// // uuid_t			s_uuid;		/* UUID */

	// unsigned int		s_max_links;
	// fmode_t			s_mode;

	// /*
	//  * Filesystem subtype.  If non-empty the filesystem type field
	//  * in /proc/mounts will be "type.subtype"
	//  */
	// const char *s_subtype;

	// const struct dentry_operations *s_d_op; /* default d_op for dentries */

	// /* Number of inodes with nlink == 0 but still referenced */
	// // atomic_long_t s_remove_count;

	// /*
	//  * Number of inode/mount/sb objects that are being watched, note that
	//  * inodes objects are currently double-accounted.
	//  */
	// // atomic_long_t s_fsnotify_connectors;

	// /* Being remounted read-only */
	// int s_readonly_remount;

	// /* AIO completions deferred from interrupt context */
	// struct workqueue_struct *s_dio_done_wq;
	// struct hlist_head s_pins;


	// /*
	//  * Indicates how deep in a filesystem stack this SB is
	//  */
	// int s_stack_depth;

	// /* s_inode_list_lock protects s_inodes */
	// // spinlock_t		s_inode_list_lock ____cacheline_aligned_in_smp;
	// struct list_head	s_inodes;	/* all inodes */

	// // spinlock_t		s_inode_wblist_lock;
	// struct list_head	s_inodes_wb;	/* writeback inodes */
} ;

struct file_system_type {
	const char *name;
	int fs_flags; 
#define FS_REQUIRES_DEV		1 
#define FS_BINARY_MOUNTDATA	2
#define FS_HAS_SUBTYPE		4
#define FS_USERNS_MOUNT		8	/* Can be mounted by userns root */
#define FS_DISALLOW_NOTIFY_PERM	16	/* Disable fanotify permission events */
#define FS_ALLOW_IDMAP         32      /* FS has been updated to handle vfs idmappings. */
#define FS_RENAME_DOES_D_MOVE	32768	/* FS will handle d_move() during rename() internally. */
	// int (*init_fs_context)(struct fs_context *);
	// const struct fs_parameter_spec *parameters;
	struct dentry *(*mount) (struct file_system_type *, int, const char *, void *);
	void (*kill_sb) (struct super_block *);
	// struct module *owner;
	struct file_system_type * next;
	struct hlist_head fs_supers;
};

extern int register_filesystem(struct file_system_type *);
extern int unregister_filesystem(struct file_system_type *);

#endif