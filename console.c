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
#include <serial.h>

#define SERIAL(x) serial_##x


/* test if ctrl-c was pressed */
static int ctrlc_disabled;
static int ctrlc_was_pressed;

void console_init(void)
{
	ctrlc_disabled = 0;
	ctrlc_was_pressed = 0;
	SERIAL(init)();
}

/*
 * This depends on tstc() always being called before getc().
 * This is guaranteed to be true because this routine is called
 * only from fgetc() which assures it.
 * No attempt is made to demultiplex multiple input sources.
 */
static int console_getc(void)
{
	/* This is never called with testcdev == NULL */
	return SERIAL(getc)();
}

static int console_tstc(void)
{
	int ret;

	disable_ctrlc(1);
	ret = SERIAL(tstc)();
	if (ret > 0) {
		return ret;
	}
	disable_ctrlc(0);

	return 0;
}

static void console_putc(const char c)
{
	if(c == '\n') {
		SERIAL(putc)('\r');
	}
	SERIAL(putc)(c);
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

