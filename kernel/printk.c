#include <jnix/console.h>
#include <jnix/types.h>
#include <jnix/stdarg.h>
#include <lib/vsprintf.h>
#include <jnix/printk.h>
#include <jnix/init.h>
#include <jnix/spinlock.h>

static struct console *exclusive_console;
struct console *console_drivers;

static void call_console_drivers(const char *text, size_t len);

spinlock_t console_lock;

static void call_console_drivers(const char *text, size_t len)
{
    struct console *con;

    if (!console_drivers)
		return;
	
	spin_lock_irq(&console_lock);
	for_each_console(con) {
		if (exclusive_console && con != exclusive_console)
			continue;
		if (!(con->flags & CON_ENABLED))
			continue;
		if (!con->write)
			continue;
		else {
			con->write(con, text, len);
		}
	}
	spin_unlock_irq(&console_lock);
}

int printk(const char *fmt, ...)
{
	va_list args;
	int r;
	char buf[512];

	va_start(args, fmt);
	// r = vprintk(fmt, args);
	r = vsprintf(buf, fmt, args);
	va_end(args);
	
	call_console_drivers(buf, r);

	return r;
}

void __init console_init(void)
{
	int ret;
	initcall_t call;
	initcall_entry_t *ce;

	/* Setup the default TTY line discipline. */
	// n_tty_init();

	/*
	 * set up the console device so that later boot sequences can
	 * inform about problems etc..
	 */
	ce = __con_initcall_start;
	// trace_initcall_level("console");
	while (ce < __con_initcall_end) {
		call = initcall_from_entry(ce);
		// trace_initcall_start(call);
		ret = call();
		// trace_initcall_finish(call, ret);
		ce++;
	}
}

void register_console(struct console *newcon)
{
	struct console *bcon = NULL;
	for_each_console(bcon) {
		if (bcon==newcon) {
			return;
		}
	}

	if (console_drivers==NULL) {
		newcon->next = console_drivers;
		console_drivers = newcon;
	} else {
		newcon->next = console_drivers->next;
		console_drivers->next = newcon;
	}

	if (newcon->flags & CON_PRINTBUFFER) {
		exclusive_console = newcon;
	}
	
}

int unregister_console(struct console *console)
{
	struct console *con;
	int res;

	if (console_drivers == console)
	{
		console_drivers=console->next;
		res = 0;
	}
	else
	{
		for_each_console(con) {
			if (con->next == console) {
				con->next = console->next;
				res = 0;
				break;
			}
		}
	}

	if (console->exit)
		res = console->exit(console);
		
	return res;
}