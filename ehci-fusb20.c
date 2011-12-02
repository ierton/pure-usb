/*
 * (C) Copyright 2011 Sergey Mironov.
 * Starter USB file for Fujitsu USB2.0 controller found in UEMD board
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#include "ehci-core.h"
#include "ehci.h"

int ehci_specific_init(void)
{
	/* Global capability regs pointer */
	hccr = (struct ehci_hccr *)((uint32_t)CONFIG_FUSB20_BASE);
	/* Global operational regs pointer */
	hcor = (struct ehci_hcor *)((uint32_t)CONFIG_FUSB20_BASE + 0x10);

	//ehci_writel(0x1004C000, 0x00608000);
	//ehci_writel(0x1004C004, 0x03004018);
	//ehci_writel(0x1004C008, 0x00200014);

	return 0;
}

void ehci_specific_stop(void)
{
}

