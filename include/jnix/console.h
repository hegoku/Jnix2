#ifndef JNIX_CONSOLE_H
#define JNIX_CONSOLE_H

#include <jnix/types.h>

#define CON_PRINTBUFFER	(1)
#define CON_CONSDEV	(2) /* Preferred console, /dev/console */
#define CON_ENABLED	(4)
#define CON_BOOT	(8)
#define CON_ANYTIME	(16) /* Safe to call when cpu is offline */
#define CON_BRL		(32) /* Used for a braille device */
#define CON_EXTENDED	(64) /* Use the extended output format a la /dev/kmsg */

struct console {
	char	name[16];
	void	(*write)(struct console *, const char *, unsigned);
	int	(*read)(struct console *, char *, unsigned);
	struct tty_driver *(*device)(struct console *, int *);
	void	(*unblank)(void);
	int	(*setup)(struct console *, char *);
	int	(*exit)(struct console *);
	int	(*match)(struct console *, char *name, int idx, char *options);
	short	flags;
	short	index;
	int	cflag;
	uint	ispeed;
	uint	ospeed;
	void	*data;
	struct	 console *next;
};

#define for_each_console(con) \
	for (con = console_drivers; con != NULL; con = con->next)

extern struct console *early_console;

extern void register_console(struct console *);
extern int unregister_console(struct console *);
extern struct console *console_drivers;

extern void console_init(void);
#endif