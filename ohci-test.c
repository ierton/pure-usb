#include <common.h>
#include <asm/byteorder.h>
#include <ohci.h>

//#define INFO(fmt,args...) printf("%s:%d " fmt "\n", __FUNCTION__, __LINE__,  ##args)
#define INFO(fmt,args...)


#define CHECK(x, cmd) do { if(!(x)) { INFO("failed condition %s", #x) ; { cmd ; } ; } } while(0)

#define USB_BASE CONFIG_FUSB20_BASE

#define EHCI_HCCAPBASE (USB_BASE + 0x00000000)

#define EHCI_HCSPARAMS (USB_BASE + 0x00000004)

#define EHCI_HCCPARAMS (USB_BASE + 0x00000008)

#define EHCI_USBCMD (USB_BASE + 0x00000010)
#define EHCI_USBCMD_RS	(1<<0)
#define EHCI_USBCMD_HCRESET	(1<<1)
#define EHCI_USBCMD_FRSZ1	(1<<2)
#define EHCI_USBCMD_FRSZ2	(1<<3)
#define EHCI_USBCMD_PSHEN	(1<<4)
#define EHCI_USBCMD_ASYNCSHED	(1<<5)
#define EHCI_USBCMD_IAAD	(1<<6)
#define EHCI_USBCMD_LHRST	(1<<7)
#define EHCI_USBCMD_ACNT1	(1<<8)
#define EHCI_USBCMD_ACNT2	(1<<9)
#define EHCI_USBCMD_AEN	(1<<11)

#define EHCI_USBSTS (USB_BASE + 0x00000014)
#define EHCI_USBSTS_USBINT	(1<<0)
#define EHCI_USBSTS_USBEINT	(1<<1)
#define EHCI_USBSTS_PCHANGE	(1<<2)
#define EHCI_USBSTS_FLR	(1<<3)
#define EHCI_USBSTS_HSERR	(1<<4)
#define EHCI_USBSTS_IAADV	(1<<5)
#define EHCI_USBSTS_HSHALT	(1<<12)
#define EHCI_USBSTS_RECL	(1<<13)
#define EHCI_USBSTS_PSS	(1<<14)
#define EHCI_USBSTS_ASS	(1<<15)

#define EHCI_USBINTR (USB_BASE + 0x00000018)

#define EHCI_FRINDEX (USB_BASE + 0x0000001C)

#define EHCI_CTRLDSSEGMENT (USB_BASE + 0x00000020)

#define EHCI_PERIODICLISTBASE (USB_BASE + 0x00000024)

#define EHCI_ASYNCLISTADDR (USB_BASE + 0x00000028)

#define EHCI_CONFIGFLAG (USB_BASE + 0x00000050)

#define EHCI_PORTSC_1 (USB_BASE + 0x00000054)
#define EHCI_PORTSC_1_CCON	(1<<0)
#define EHCI_PORTSC_1_CSCH	(1<<1)
#define EHCI_PORTSC_1_EN	(1<<2)
#define EHCI_PORTSC_1_ENCH	(1<<3)
#define EHCI_PORTSC_1_OC	(1<<4)
#define EHCI_PORTSC_1_OCCH	(1<<5)
#define EHCI_PORTSC_1_RESUME	(1<<6)
#define EHCI_PORTSC_1_SUSP	(1<<7)
#define EHCI_PORTSC_1_PRST	(1<<8)
#define EHCI_PORTSC_1_LST1	(1<<10)
#define EHCI_PORTSC_1_LST2	(1<<11)
#define EHCI_PORTSC_1_PP	(1<<12)
#define EHCI_PORTSC_1_POWN	(1<<13)
#define EHCI_PORTSC_1_PICTR1	(1<<14)
#define EHCI_PORTSC_1_PICTR2	(1<<15)

#define EHCI_PORTSC_2 (USB_BASE + 0x00000058)
#define EHCI_PORTSC_2_CCON	(1<<0)
#define EHCI_PORTSC_2_CSCH	(1<<1)
#define EHCI_PORTSC_2_EN	(1<<2)
#define EHCI_PORTSC_2_ENCH	(1<<3)
#define EHCI_PORTSC_2_OC	(1<<4)
#define EHCI_PORTSC_2_OCCH	(1<<5)
#define EHCI_PORTSC_2_RESUME	(1<<6)
#define EHCI_PORTSC_2_SUSP	(1<<7)
#define EHCI_PORTSC_2_PRST	(1<<8)
#define EHCI_PORTSC_2_LST1	(1<<10)
#define EHCI_PORTSC_2_LST2	(1<<11)
#define EHCI_PORTSC_2_PP	(1<<12)
#define EHCI_PORTSC_2_POWN	(1<<13)
#define EHCI_PORTSC_2_PICTR1	(1<<14)
#define EHCI_PORTSC_2_PICTR2	(1<<15)

#define usb_readl_(x)		cpu_to_le32((*((volatile u32 *)(x))))
#define usb_writel_(a, b)	(*((volatile u32 *)(a)) = \
					cpu_to_le32(((volatile u32)b)))

static void ehci_writel(volatile u32 reg, volatile u32 val)
{
	//INFO("EHCI[0x%02X] <- 0x%08X", ((u32)reg & 0xFF), val);
	usb_writel_(reg, val);
}

static u32 ehci_readl(volatile u32 reg)
{
	u32 val;
	val = usb_readl_(reg);
	//INFO("EHCI[0x%02X] -> 0x%08X", ((u32)reg & 0xFF), val);
	return val;
}

void ohci_writel(volatile u32 reg, volatile u32 val)
{
	//INFO("OHCI[0x%02X] <- 0x%08X", ((u32)reg & 0xFF), val);
	usb_writel_(CONFIG_SYS_USB_OHCI_REGS_BASE + reg, val);
}

u32 ohci_readl(volatile u32 reg)
{
	u32 val;
	val = usb_readl_(CONFIG_SYS_USB_OHCI_REGS_BASE + reg);
	//INFO("OHCI[0x%02X] -> 0x%08X", ((u32)reg & 0xFF), val);
	return val;
}

void ohci_check(void)
{
	u32 ret;

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0), goto out);

	ohci_writel ( 0x00000014 , 0x80000000 ) ;

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000048 ) ;
	CHECK(ret == (0x02000902), goto out);

	ret = ohci_readl ( 0x00000034 ) ;
	CHECK(ret == (0x00002EDF), goto out);

	ohci_writel ( 0x00000004 , 0 ) ;

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0), goto out);


	ohci_writel ( 0x00000008 , 0x00000001 ) ;

	ret = ohci_readl ( 0x00000008 ) ;
	CHECK(ret == (0), goto out);

	ohci_writel ( 0x00000020 , 0 ) ;

	ohci_writel ( 0x00000028 , 0 ) ;

	ohci_writel ( 0x00000018 , 0x478F8000 ) ;

	ret = ohci_readl ( 0x00000034 ) ;
	CHECK(ret == (0x00002EDF), goto out);

	ohci_writel ( 0x00000034 , 0xA7782EDF ) ;

	ohci_writel ( 0x00000040 , 0x00002A2F ) ;

	ret = ohci_readl ( 0x00000034 ) ;
	CHECK(ret == (0xA7782EDF), goto out);

	ret = ohci_readl ( 0x00000040 ) ;
	CHECK(ret == (0x00002A2F), goto out);

	ohci_writel ( 0x00000004 , 0x00000083 ) ;

	ohci_writel ( 0x00000050 , 0x00008000 ) ;

	ohci_writel ( 0x0000000C , 0xFFFFFFFF ) ;

	ohci_writel ( 0x00000010 , 0x8000005A ) ;

	ret = ohci_readl ( 0x00000048 ) ;
	CHECK(ret == (0x02000902), goto out);

	ohci_writel ( 0x00000048 , 0x02000202 ) ;

	ohci_writel ( 0x00000050 , 0x00010000 ) ;

	ohci_writel ( 0x0000004C , 0 ) ;

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0x00000083), goto out);

	INFO("Skipping IRQ");
	return;

	INFO("MARK: ohci entering IRQ");

	do {
		ret = ohci_readl ( 0x0000000C ) ;
	}while (ret != 0x00000040);
	CHECK(ret == (0x00000040), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	ohci_writel ( 0x0000000C , 0x00000048 ) ;

	ohci_writel ( 0x00000014 , 0x00000040 ) ;

	ohci_writel ( 0x0000000C , 0x00000040 ) ;

	ohci_writel ( 0x00000010 , 0x80000000 ) ;

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0x00000083), goto out);

	INFO("MARK: ohci leaving IRQ");

	/* Kinda dump */
	ret = ohci_readl ( 0x00000000 ) ;
	CHECK(ret == (0x00000010), goto out);

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0x00000083), goto out);

	ret = ohci_readl ( 0x00000008 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000001A), goto out);

	ret = ohci_readl ( 0x0000001C ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000020 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000024 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000028 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x0000002C ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000030 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000048 ) ;
	CHECK(ret == (0x02000202), goto out);

	ret = ohci_readl ( 0x0000004C ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000050 ) ;
	CHECK(ret == (0x00008000), goto out);

	ret = ohci_readl ( 0x00000054 ) ;
	CHECK(ret == (0x00000100), goto out);

	ret = ohci_readl ( 0x00000058 ) ;
	CHECK(ret == (0x00000100), goto out);
out:
	INFO("OHCI finish");
}

void ehci_check(void)
{
	u32 ret;
	INFO("Start USB\n");

	ret = ehci_readl ( EHCI_HCSPARAMS ) ;
	CHECK(ret == (0x00001212), goto out);

	ret = ehci_readl ( EHCI_HCCPARAMS ) ;
	CHECK(ret == (0x00000016), goto out);

	ret = ehci_readl ( EHCI_HCSPARAMS ) ;
	CHECK(ret == (0x00001212), goto out);

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_HSHALT | 0), goto out);

	ehci_writel ( EHCI_USBINTR , 0 ) ;

	ret = ehci_readl ( EHCI_HCCPARAMS ) ;
	CHECK(ret == (0x00000016), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_ACNT1 | EHCI_USBCMD_ACNT2 | EHCI_USBCMD_AEN | 0x00080000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_HCRESET | EHCI_USBCMD_ACNT1 | EHCI_USBCMD_ACNT2 | EHCI_USBCMD_AEN | 0x00080000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_ACNT1 | EHCI_USBCMD_ACNT2 | EHCI_USBCMD_AEN | 0x00080000), goto out);

	ret = ehci_readl ( EHCI_PORTSC_2 ) ;
	CHECK(ret == (EHCI_PORTSC_2_POWN | 0), goto out);

	ehci_writel ( EHCI_PORTSC_2 , EHCI_PORTSC_2_POWN | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_ACNT1 | EHCI_USBCMD_ACNT2 | EHCI_USBCMD_AEN | 0x00080000), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_POWN | 0), goto out);

	ehci_writel ( EHCI_PORTSC_1 , EHCI_PORTSC_1_POWN | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_ACNT1 | EHCI_USBCMD_ACNT2 | EHCI_USBCMD_AEN | 0x00080000), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_ACNT1 | EHCI_USBCMD_ACNT2 | EHCI_USBCMD_AEN | 0x00080000), goto out);

	INFO("MARK: EHCI reset");

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_ACNT1 | EHCI_USBCMD_ACNT2 | EHCI_USBCMD_AEN | 0x00080000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_HCRESET | EHCI_USBCMD_ACNT1 | EHCI_USBCMD_ACNT2 | EHCI_USBCMD_AEN | 0x00080000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_ACNT1 | EHCI_USBCMD_ACNT2 | EHCI_USBCMD_AEN | 0x00080000), goto out);

	INFO("MARK: EHCI setup");

	ehci_writel ( EHCI_PERIODICLISTBASE , 0x47989000 ) ;

	ehci_writel ( EHCI_ASYNCLISTADDR , 0x47987000 ) ;

	ret = ehci_readl ( EHCI_HCCPARAMS ) ;
	CHECK(ret == (0x00000016), goto out);

	INFO("MARK: EHCI resetting port, frame size 0b11 (256 elements)");
	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000 ) ;

	ehci_writel ( EHCI_CONFIGFLAG , 0x00000001 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_HCCAPBASE ) ;
	CHECK(ret == (0x01000010), goto out);

	INFO("MARK: EHCI skip ennabling interrupts");

	/*ehci_writel ( EHCI_USBINTR , 0x00000037 ) ;*/

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (0), goto out);


	INFO("MARK: Enabling port power");
	ehci_writel ( EHCI_PORTSC_1 , EHCI_PORTSC_1_PP | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	INFO("MARK: ehci enterring IRQ");

	do {
		ret = ehci_readl ( EHCI_USBSTS ) ;
	} while( ret != (EHCI_USBSTS_PCHANGE | EHCI_USBSTS_FLR | 0));

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_PCHANGE | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_PORTSC_2 ) ;
	CHECK(ret == (0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_CSCH | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	INFO("MARK: ehci leaving IRQ");

	goto out; /*

	ret = ehci_readl ( EHCI_PORTSC_2 ) ;
	CHECK(ret == (0), goto out);

	ehci_writel ( EHCI_PORTSC_2 , EHCI_PORTSC_2_PP | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	INFO("Ohci is broken, fix base addr");
	goto out;

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0), goto out);

	ohci_writel ( 0x00000014 , 0x80000000 ) ;

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000048 ) ;
	CHECK(ret == (0x02000902), goto out);

	ret = ohci_readl ( 0x00000034 ) ;
	CHECK(ret == (0x00002EDF), goto out);

	ohci_writel ( 0x00000004 , 0 ) ;

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_CSCH | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_CSCH | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ehci_writel ( EHCI_PORTSC_1 , EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_CSCH | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_PORTSC_2 ) ;
	CHECK(ret == (EHCI_PORTSC_2_PP | 0), goto out);

	ohci_writel ( 0x00000008 , 0x00000001 ) ;

	ret = ohci_readl ( 0x00000008 ) ;
	CHECK(ret == (0), goto out);

	ohci_writel ( 0x00000020 , 0 ) ;

	ohci_writel ( 0x00000028 , 0 ) ;

	ohci_writel ( 0x00000018 , 0x478F8000 ) ;

	ret = ohci_readl ( 0x00000034 ) ;
	CHECK(ret == (0x00002EDF), goto out);

	ohci_writel ( 0x00000034 , 0xA7782EDF ) ;

	ohci_writel ( 0x00000040 , 0x00002A2F ) ;

	ret = ohci_readl ( 0x00000034 ) ;
	CHECK(ret == (0xA7782EDF), goto out);

	ret = ohci_readl ( 0x00000040 ) ;
	CHECK(ret == (0x00002A2F), goto out);

	ohci_writel ( 0x00000004 , 0x00000083 ) ;

	ohci_writel ( 0x00000050 , 0x00008000 ) ;

	ohci_writel ( 0x0000000C , 0xFFFFFFFF ) ;

	ohci_writel ( 0x00000010 , 0x8000005A ) ;

	ret = ohci_readl ( 0x00000048 ) ;
	CHECK(ret == (0x02000902), goto out);

	ohci_writel ( 0x00000048 , 0x02000202 ) ;

	ohci_writel ( 0x00000050 , 0x00010000 ) ;

	ohci_writel ( 0x0000004C , 0 ) ;

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0x00000083), goto out);

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | 0), goto out);

	INFO("MARK: ehci rejecting IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000040), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	ohci_writel ( 0x0000000C , 0x00000048 ) ;

	ohci_writel ( 0x00000014 , 0x00000040 ) ;

	ohci_writel ( 0x0000000C , 0x00000040 ) ;

	ohci_writel ( 0x00000010 , 0x80000000 ) ;

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0x00000083), goto out);

	INFO("MARK: ohci leaving IRQ");

	ret = ohci_readl ( 0x00000000 ) ;
	CHECK(ret == (0x00000010), goto out);

	ret = ohci_readl ( 0x00000004 ) ;
	CHECK(ret == (0x00000083), goto out);

	ret = ohci_readl ( 0x00000008 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000001A), goto out);

	ret = ohci_readl ( 0x0000001C ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000020 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000024 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000028 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x0000002C ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000030 ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000048 ) ;
	CHECK(ret == (0x02000202), goto out);

	ret = ohci_readl ( 0x0000004C ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000050 ) ;
	CHECK(ret == (0x00008000), goto out);

	ret = ohci_readl ( 0x00000054 ) ;
	CHECK(ret == (0x00000100), goto out);

	ret = ohci_readl ( 0x00000058 ) ;
	CHECK(ret == (0x00000100), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ehci_writel ( EHCI_PORTSC_1 , EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	ret = ohci_readl ( 0x00000048 ) ;
	CHECK(ret == (0x02000202), goto out);

	ret = ohci_readl ( 0x0000004C ) ;
	CHECK(ret == (0), goto out);

	ret = ohci_readl ( 0x00000050 ) ;
	CHECK(ret == (0x00008000), goto out);

	ohci_writel ( 0x00000054 , 0x00000100 ) ;

	ohci_writel ( 0x00000058 , 0x00000100 ) ;

	ret = ohci_readl ( 0x00000050 ) ;
	CHECK(ret == (0x00008000), goto out);

	ohci_writel ( 0x0000000C , 0x00000040 ) ;

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000054 ) ;
	CHECK(ret == (0x00000100), goto out);

	ret = ohci_readl ( 0x00000058 ) ;
	CHECK(ret == (0x00000100), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000001A), goto out);

	ohci_writel ( 0x00000010 , 0x00000040 ) ;

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ehci_writel ( EHCI_PORTSC_1 , EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0 ) ;

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST2 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_EN | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_EN | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_EN | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | 0), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000 ) ;

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002846), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_EN | EHCI_PORTSC_1_PP | 0), goto out);

	ehci_writel ( EHCI_PORTSC_1 , EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_PP | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x000028E3), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	ret = ohci_readl ( 0x00000054 ) ;
	CHECK(ret == (0x00000100), goto out);

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ohci_readl ( 0x00000058 ) ;
	CHECK(ret == (0x00000100), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002B47), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ehci_writel ( EHCI_PORTSC_1 , EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0 ) ;

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_PRST | EHCI_PORTSC_1_LST1 | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_EN | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_EN | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_PORTSC_1 ) ;
	CHECK(ret == (EHCI_PORTSC_1_CCON | EHCI_PORTSC_1_EN | EHCI_PORTSC_1_PP | 0), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | 0), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000 ) ;

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00000065), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x000001B9), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00000419), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x0000041D), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00000421), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00000424), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x0000048F), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00000492), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00000493), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00000497), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x000005A7), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");;

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00000850), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | 0), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000 ) ;

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x000008D7), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x000009AE), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00000BBB), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | 0), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000 ) ;

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002A59), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002A67), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002A69), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002A6A), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002B46), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002BA0), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002BA4), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002BAD), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002BAF), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002BB1), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002C6B), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002C70), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002C77), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002D6C), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");;;

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E16), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E21), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E26), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E33), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E37), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E3E), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E4A), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E4C), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E6A), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E6C), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E71), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E73), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E75), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E7B), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E7D), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002E7F), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_USBINT | EHCI_USBSTS_FLR | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_USBINT | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002F42), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	ret = ehci_readl ( EHCI_FRINDEX ) ;
	CHECK(ret == (0x00002FA7), goto out);

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	ehci_writel ( EHCI_USBCMD , EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | EHCI_USBCMD_IAAD | 0x00010000), goto out);

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_IAADV | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_IAADV | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_RS | EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci leaving IRQ");

	INFO("MARK: ohci entering IRQ");

	ret = ohci_readl ( 0x0000000C ) ;
	CHECK(ret == (0x00000004), goto out);

	ret = ohci_readl ( 0x00000010 ) ;
	CHECK(ret == (0x8000005A), goto out);

	INFO("MARK: ohci rejecting IRQ p2");

	INFO("MARK: ehci enterring IRQ");

	ret = ehci_readl ( EHCI_USBSTS ) ;
	CHECK(ret == (EHCI_USBSTS_FLR | EHCI_USBSTS_HSERR | EHCI_USBSTS_HSHALT | EHCI_USBSTS_RECL | EHCI_USBSTS_ASS | 0), goto out);

	ehci_writel ( EHCI_USBSTS , EHCI_USBSTS_HSERR | 0 ) ;

	ret = ehci_readl ( EHCI_USBCMD ) ;
	CHECK(ret == (EHCI_USBCMD_FRSZ2 | EHCI_USBCMD_ASYNCSHED | 0x00010000), goto out);

	INFO("MARK: ehci fatal error");

	*/

out:
	INFO("EHCI Exiting");;
}

#if 0
int do_usbtest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 ret;
	ehci_check();
	ohci_check();

	INFO("Entering loop");
	do{
		ret = ehci_readl(EHCI_USBSTS); 
	} while(1);
	return 0;
}

U_BOOT_CMD(
	usbt,	5,	1,	do_usbtest,
	"USB UEMD testing sub-system",
	"(no args)"
);
#endif

