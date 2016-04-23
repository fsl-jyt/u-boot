/*
 * Copyright 2015 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef FSL_MMDC_H
#define FSL_MMDC_H

/* MMDC Registers */
struct mmdc_p_regs {
	u32 mdctl;
	u32 mdpdc;
	u32 mdotc;
	u32 mdcfg0;
	u32 mdcfg1;
	u32 mdcfg2;
	u32 mdmisc;
	u32 mdscr;
	u32 mdref;
	u32 res1[2];
	u32 mdrwd;
	u32 mdor;
	u32 mdmrr;
	u32 mdcfg3lp;
	u32 mdmr4;
	u32 mdasp;
	u32 res3[239];
	u32 maarcr;
	u32 mapsr;
	u32 res4[254];
	u32 mpzqhwctrl;
	u32 res5[2];
	u32 mpwldectrl0;
	u32 mpwldectrl1;
	u32 res6;
	u32 mpodtctrl;
	u32 mprddqby0dl;
	u32 mprddqby1dl;
	u32 mprddqby2dl;
	u32 mprddqby3dl;
	u32 res7[4];
	u32 mpdgctrl0;
	u32 mpdgctrl1;
	u32 res8;
	u32 mprddlctl;
	u32 res9;
	u32 mpwrdlctl;
	u32 res10[25];
	u32 mpmur0;
};

#endif /* FSL_MMDC_H */
