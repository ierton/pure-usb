/*
 * (C) Copyright 2000
 * Paolo Scaffardi, AIRVENT SAM s.p.a - RIMINI(ITALY), arsenio@tin.it
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <stdarg.h>

NS16550_t g_port;

#define SERIAL(x) NS16550_##x
#define SERIAL_PORT(addr) ((volatile struct NS16550 *)(addr))

void console_init(void)
{
	const int MODE_X_DIV = 16;

	/* Compute divisor value. Normally, we should simply return:
	 *   CONFIG_SYS_NS16550_CLK) / MODE_X_DIV / gd->baudrate
	 * but we need to round that value by adding 0.5.
	 * Rounding is especially important at high baud rates.
	 */
	const int divisor = 
		(CONFIG_SYS_NS16550_CLK + (CONFIG_BAUDRATE * (MODE_X_DIV / 2))) /
		(MODE_X_DIV * CONFIG_BAUDRATE);

	g_port = SERIAL_PORT(CONFIG_SYS_NS16550_COM1);
	SERIAL(init)(g_port,divisor);
}

/*
 * This depends on tstc() always being called before getc().
 * This is guaranteed to be true because this routine is called
 * only from fgetc() which assures it.
 * No attempt is made to demultiplex multiple input sources.
 */
static int console_getc()
{
	unsigned char ret;

	/* This is never called with testcdev == NULL */
	ret = SERIAL(getc)(g_port);
	return ret;
}

static int console_tstc()
{
	int ret;

	disable_ctrlc(1);
	ret = SERIAL(tstc)(g_port);
	if (ret > 0) {
		return ret;
	}
	disable_ctrlc(0);

	return 0;
}

static void console_putc(const char c)
{
	if(c == '\n') {
		SERIAL(putc)(g_port,'\r');
	}
	SERIAL(putc)(g_port,c);
}

static void console_puts(const char *s)
{
	while (*s) {
		console_putc(*s++);
	}
}

int fgetc(int file)
{
	if (file < MAX_FILES) {
		return console_getc();
	}
	return -1;
}

int ftstc(int file)
{
	if (file < MAX_FILES)
		return console_tstc();

	return -1;
}

void fputc(int file, const char c)
{
	if (file < MAX_FILES)
		console_putc(c);
}

void fputs(int file, const char *s)
{
	if (file < MAX_FILES)
		console_puts(s);
}

int fprintf(int file, const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CONFIG_SYS_PBSIZE];

	va_start(args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf(printbuffer, fmt, args);
	va_end(args);

	/* Send to desired file */
	fputs(file, printbuffer);
	return i;
}

int getc(void)
{
	/* Send directly to the handler */
	return console_getc();
}

int tstc(void)
{
	/* Send directly to the handler */
	return console_tstc();
}

void putc(const char c)
{
	console_putc(c);
}

void puts(const char *s)
{
	console_puts(s);
}

int printf(const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CONFIG_SYS_PBSIZE];

	va_start(args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf(printbuffer, fmt, args);
	va_end(args);

	/* Print the string */
	puts(printbuffer);
	return i;
}

int vprintf(const char *fmt, va_list args)
{
	uint i;
	char printbuffer[CONFIG_SYS_PBSIZE];

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf(printbuffer, fmt, args);

	/* Print the string */
	puts(printbuffer);
	return i;
}

/* test if ctrl-c was pressed */
static int ctrlc_disabled = 0;	/* see disable_ctrl() */
static int ctrlc_was_pressed = 0;
int ctrlc(void)
{
	if(ctrlc_was_pressed)
		return 1;

	if (tstc()) {
		switch (getc()) {
		case 0x03:		/* ^C - Control C */
			ctrlc_was_pressed = 1;
			return 1;
		default:
			break;
		}
	}
	return 0;
}

/* pass 1 to disable ctrlc() checking, 0 to enable.
 * returns previous state
 */
int disable_ctrlc(int disable)
{
	int prev = ctrlc_disabled;	/* save previous state */

	ctrlc_disabled = disable;
	return prev;
}

int had_ctrlc (void)
{
	return ctrlc_was_pressed;
}

void clear_ctrlc(void)
{
	ctrlc_was_pressed = 0;
}

