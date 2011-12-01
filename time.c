#include <common.h>

/* Gives 0.5..1 second on current CPU and settings */
#define DELAY_SEC (3000000)

#define DELAY_MSEC (3000)

#define DELAY_USEC (3)

void mdelay(unsigned long msec)
{
	do {
		volatile int i;
		for(i=0; i<DELAY_MSEC; i++);
	} while(msec-- > 0);
}

void udelay(unsigned long usec)
{
	do {
		volatile int i;
		for(i=0; i<DELAY_USEC; i++);
	} while(usec-- > 0);
}

