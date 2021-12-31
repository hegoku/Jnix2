#include <jnix/fs.h>
#include <lib/string.h>
#include <jnix/printk.h>

static struct file_system_type *file_systems;

static struct file_system_type **find_filesystem(const char *name, unsigned len)
{
	struct file_system_type **p;
	for (p = &file_systems; *p; p = &(*p)->next)
		if (strncmp((*p)->name, name, len) == 0 &&
		    !(*p)->name[len])
			break;
	return p;
}

int register_filesystem(struct file_system_type *fs)
{
    struct file_system_type ** p;

	p = find_filesystem(fs->name, strlen(fs->name));
	if (*p)
		return -1;
	else
		*p = fs;
	return 0;
}

int unregister_filesystem(struct file_system_type * fs)
{
	struct file_system_type ** tmp;

	tmp = &file_systems;
	while (*tmp) {
		if (fs == *tmp) {
			*tmp = fs->next;
			fs->next = NULL;
			return 0;
		}
		tmp = &(*tmp)->next;
	}

	return -1;
}