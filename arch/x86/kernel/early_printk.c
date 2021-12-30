#include <jnix/console.h>
#include <jnix/init.h>
#include <asm/io.h>
#include <asm/page.h>

/* Simple VGA output */
#define VGABASE		(0xb8000+PAGE_OFFSET)

static int max_ypos = 25, max_xpos = 80;
static int current_ypos = 0, current_xpos;

static void early_vga_write(struct console *con, const char *str, unsigned n)
{
	char c;
	int  i, k, j;
	while ((c = *str++) != '\0' && n-- > 0) {
		if (current_ypos >= max_ypos) {
			/* scroll 1 line up */
			for (k = 1, j = 0; k < max_ypos; k++, j++) {
				for (i = 0; i < max_xpos; i++) {
					writew(readw((void*)(VGABASE+2*(max_xpos*k+i))),
					       (void*)(VGABASE + 2*(max_xpos*j + i)));
				}
			}
			for (i = 0; i < max_xpos; i++)
				writew(0x720, (void*)(VGABASE + 2*(max_xpos*j + i)));
			current_ypos = max_ypos-1;
		}
		if (c == '\b') {
			if (current_xpos > 0)
				current_xpos--;
		} else if (c == '\r') {
			current_xpos = 0;
		} else
		if (c == '\n') {
			current_xpos = 0;
			current_ypos++;
		} else if (c != '\r')  {
			writew(((0x7 << 8) | (unsigned short) c),
			       (void*)(VGABASE + 2*(max_xpos*current_ypos +
						current_xpos++)));
			if (current_xpos >= max_xpos) {
				current_xpos = 0;
				current_ypos++;
			}
		}
	}
}

static struct console early_vga_console = {
	.name =		"earlyvga",
	.write =	early_vga_write,
	.flags =	CON_PRINTBUFFER|CON_ENABLED,
	.index =	-1,
};

int early_console_init(void) 
{
	register_console(&early_vga_console);
}

console_initcall(early_console_init);
