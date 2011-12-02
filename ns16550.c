/*
 * COM1 NS16550 support
 * originally from linux source (arch/powerpc/boot/ns16550.c)
 * modified to use CONFIG_SYS_ISA_MEM and new defines
 */

#include <config.h>
#include <serial.h>
#include <watchdog.h>
#include <linux/types.h>
#include <asm/io.h>

/*
 * Note that the following macro magic uses the fact that the compiler
 * will not allocate storage for arrays of size 0
 */

#if !defined(CONFIG_SYS_NS16550_REG_SIZE) || (CONFIG_SYS_NS16550_REG_SIZE == 0)
#error "Please define NS16550 registers size."
#elif (CONFIG_SYS_NS16550_REG_SIZE > 0)
#define UART_REG(x)						   \
	unsigned char prepad_##x[CONFIG_SYS_NS16550_REG_SIZE - 1]; \
	unsigned char x;
#elif (CONFIG_SYS_NS16550_REG_SIZE < 0)
#define UART_REG(x)							\
	unsigned char x;						\
	unsigned char postpad_##x[-CONFIG_SYS_NS16550_REG_SIZE - 1];
#endif

struct NS16550 {
	UART_REG(rbr);		/* 0 */
	UART_REG(ier);		/* 1 */
	UART_REG(fcr);		/* 2 */
	UART_REG(lcr);		/* 3 */
	UART_REG(mcr);		/* 4 */
	UART_REG(lsr);		/* 5 */
	UART_REG(msr);		/* 6 */
	UART_REG(spr);		/* 7 */
	UART_REG(mdr1);		/* 8 */
	UART_REG(reg9);		/* 9 */
	UART_REG(regA);		/* A */
	UART_REG(regB);		/* B */
	UART_REG(regC);		/* C */
	UART_REG(regD);		/* D */
	UART_REG(regE);		/* E */
	UART_REG(uasr);		/* F */
	UART_REG(scr);		/* 10*/
	UART_REG(ssr);		/* 11*/
	UART_REG(reg12);	/* 12*/
	UART_REG(osc_12m_sel);	/* 13*/
};

#define thr rbr
#define iir fcr
#define dll rbr
#define dlm ier

/*
 * These are the definitions for the FIFO Control Register
 */
#define UART_FCR_FIFO_EN 	0x01 /* Fifo enable */
#define UART_FCR_CLEAR_RCVR	0x02 /* Clear the RCVR FIFO */
#define UART_FCR_CLEAR_XMIT	0x04 /* Clear the XMIT FIFO */
#define UART_FCR_DMA_SELECT	0x08 /* For DMA applications */
#define UART_FCR_TRIGGER_MASK	0xC0 /* Mask for the FIFO trigger range */
#define UART_FCR_TRIGGER_1	0x00 /* Mask for trigger set at 1 */
#define UART_FCR_TRIGGER_4	0x40 /* Mask for trigger set at 4 */
#define UART_FCR_TRIGGER_8	0x80 /* Mask for trigger set at 8 */
#define UART_FCR_TRIGGER_14	0xC0 /* Mask for trigger set at 14 */

#define UART_FCR_RXSR		0x02 /* Receiver soft reset */
#define UART_FCR_TXSR		0x04 /* Transmitter soft reset */

/*
 * These are the definitions for the Modem Control Register
 */
#define UART_MCR_DTR	0x01		/* DTR   */
#define UART_MCR_RTS	0x02		/* RTS   */
#define UART_MCR_OUT1	0x04		/* Out 1 */
#define UART_MCR_OUT2	0x08		/* Out 2 */
#define UART_MCR_LOOP	0x10		/* Enable loopback test mode */

#define UART_MCR_DMA_EN	0x04
#define UART_MCR_TX_DFR	0x08

/*
 * These are the definitions for the Line Control Register
 *
 * Note: if the word length is 5 bits (UART_LCR_WLEN5), then setting
 * UART_LCR_STOP will select 1.5 stop bits, not 2 stop bits.
 */
#define UART_LCR_WLS_MSK 0x03		/* character length select mask */
#define UART_LCR_WLS_5	0x00		/* 5 bit character length */
#define UART_LCR_WLS_6	0x01		/* 6 bit character length */
#define UART_LCR_WLS_7	0x02		/* 7 bit character length */
#define UART_LCR_WLS_8	0x03		/* 8 bit character length */
#define UART_LCR_STB	0x04		/* Number of stop Bits, off = 1, on = 1.5 or 2) */
#define UART_LCR_PEN	0x08		/* Parity eneble */
#define UART_LCR_EPS	0x10		/* Even Parity Select */
#define UART_LCR_STKP	0x20		/* Stick Parity */
#define UART_LCR_SBRK	0x40		/* Set Break */
#define UART_LCR_BKSE	0x80		/* Bank select enable */
#define UART_LCR_DLAB	0x80		/* Divisor latch access bit */

/*
 * These are the definitions for the Line Status Register
 */
#define UART_LSR_DR	0x01		/* Data ready */
#define UART_LSR_OE	0x02		/* Overrun */
#define UART_LSR_PE	0x04		/* Parity error */
#define UART_LSR_FE	0x08		/* Framing error */
#define UART_LSR_BI	0x10		/* Break */
#define UART_LSR_THRE	0x20		/* Xmit holding register empty */
#define UART_LSR_TEMT	0x40		/* Xmitter empty */
#define UART_LSR_ERR	0x80		/* Error */

#define UART_MSR_DCD	0x80		/* Data Carrier Detect */
#define UART_MSR_RI	0x40		/* Ring Indicator */
#define UART_MSR_DSR	0x20		/* Data Set Ready */
#define UART_MSR_CTS	0x10		/* Clear to Send */
#define UART_MSR_DDCD	0x08		/* Delta DCD */
#define UART_MSR_TERI	0x04		/* Trailing edge ring indicator */
#define UART_MSR_DDSR	0x02		/* Delta DSR */
#define UART_MSR_DCTS	0x01		/* Delta CTS */

/*
 * These are the definitions for the Interrupt Identification Register
 */
#define UART_IIR_NO_INT	0x01	/* No interrupts pending */
#define UART_IIR_ID	0x06	/* Mask for the interrupt ID */

#define UART_IIR_MSI	0x00	/* Modem status interrupt */
#define UART_IIR_THRI	0x02	/* Transmitter holding register empty */
#define UART_IIR_RDI	0x04	/* Receiver data interrupt */
#define UART_IIR_RLSI	0x06	/* Receiver line status interrupt */

/*
 * These are the definitions for the Interrupt Enable Register
 */
#define UART_IER_MSI	0x08	/* Enable Modem status interrupt */
#define UART_IER_RLSI	0x04	/* Enable receiver line status interrupt */
#define UART_IER_THRI	0x02	/* Enable Transmitter holding register int. */
#define UART_IER_RDI	0x01	/* Enable receiver data interrupt */


#ifdef CONFIG_OMAP1510
#define OSC_12M_SEL	0x01	/* selects 6.5 * current clk div */
#endif

/* useful defaults for LCR */
#define UART_LCR_8N1	0x03

#define UART_LCRVAL UART_LCR_8N1		/* 8 data, 1 stop, no parity */
#define UART_MCRVAL (UART_MCR_DTR | \
		     UART_MCR_RTS)		/* RTS/DTR */
#define UART_FCRVAL (UART_FCR_FIFO_EN |	\
		     UART_FCR_RXSR |	\
		     UART_FCR_TXSR)		/* Clear & enable FIFOs */

#ifdef CONFIG_SYS_NS16550_PORT_MAPPED
#define serial_out(x,y)	outb(x,(ulong)y)
#define serial_in(y)	inb((ulong)y)
#else
#define serial_out(x,y) writeb(x,y)
#define serial_in(y) 	readb(y)
#endif

static struct NS16550 *g_ns16550;

void serial_init(void)
{
	g_ns16550 = (struct NS16550 *)CONFIG_SYS_NS16550_COM1;

	const int MODE_X_DIV = 16;

	/* Compute divisor value. Normally, we should simply return:
	 *   CONFIG_SYS_NS16550_CLK) / MODE_X_DIV / gd->baudrate
	 * but we need to round that value by adding 0.5.
	 * Rounding is especially important at high baud rates.
	 */
	const int baud_divisor = 
		(CONFIG_SYS_NS16550_CLK + (CONFIG_BAUDRATE * (MODE_X_DIV / 2))) /
		(MODE_X_DIV * CONFIG_BAUDRATE);

	serial_out(0x00, &g_ns16550->ier);
	serial_out(UART_LCR_BKSE | UART_LCRVAL, (ulong)&g_ns16550->lcr);
	serial_out(0, &g_ns16550->dll);
	serial_out(0, &g_ns16550->dlm);
	serial_out(UART_LCRVAL, &g_ns16550->lcr);
	serial_out(UART_MCRVAL, &g_ns16550->mcr);
	serial_out(UART_FCRVAL, &g_ns16550->fcr);
	serial_out(UART_LCR_BKSE | UART_LCRVAL, &g_ns16550->lcr);
	serial_out(baud_divisor & 0xff, &g_ns16550->dll);
	serial_out((baud_divisor >> 8) & 0xff, &g_ns16550->dlm);
	serial_out(UART_LCRVAL, &g_ns16550->lcr);
}

void serial_putc (char c)
{
	while ((serial_in(&g_ns16550->lsr) & UART_LSR_THRE) == 0);
	serial_out(c, &g_ns16550->thr);

	/*
	 * Call watchdog_reset() upon newline. This is done here in putc
	 * since the environment code uses a single puts() to print the complete
	 * environment upon "printenv". So we can't put this watchdog call
	 * in puts().
	 */
	if (c == '\n')
		WATCHDOG_RESET();
}

char serial_getc (void)
{
	while ((serial_in(&g_ns16550->lsr) & UART_LSR_DR) == 0) {
		WATCHDOG_RESET();
	}
	return serial_in(&g_ns16550->rbr);
}

int serial_tstc (void)
{
	return ((serial_in(&g_ns16550->lsr) & UART_LSR_DR) != 0);
}

