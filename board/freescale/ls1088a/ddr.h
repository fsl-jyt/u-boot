/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __DDR_H__
#define __DDR_H__
struct board_specific_parameters {
	u32 n_ranks;
	u32 datarate_mhz_high;
	u32 rank_gb;
	u32 clk_adjust;
	u32 wrlvl_start;
	u32 wrlvl_ctl_2;
	u32 wrlvl_ctl_3;
};

/*
 * These tables contain all valid speeds we want to override with board
 * specific parameters. datarate_mhz_high values need to be in ascending order
 * for each n_ranks group.
 */

static const struct board_specific_parameters udimm0[] = {
	/*
	 * memory controller 0
	 *   num|  hi| rank|  clk| wrlvl |   wrlvl   |  wrlvl
	 * ranks| mhz| GB  |adjst| start |   ctl2    |  ctl3
	 */
#if defined(CONFIG_TARGET_LS1088AQDS)
	{2,  1666, 0, 4,   15,  0x0b0a0808, 0x08080807,},
	{2,  1900, 0, 4,    6,  0x08080A0C, 0x0D0E0F0A,},
	{2,  2200, 0, 4,   10,  0x0B0C0D0C, 0x0E0F110E,},
	{1,  1666, 0, 4,   15,  0x0b0a0808, 0x08080807,},
	{1,  1900, 0, 4,    9,  0x0A0B0C0B, 0x0D0E0F0D,},
	{1,  2200, 0, 4,   10,  0x0B0C0D0C, 0x0E0F110E,},
	{}
#elif defined(CONFIG_TARGET_LS1088ARDB)
	{2,  1666, 0, 4,     12, 0x0b0a0907, 0x0707070a,},
	{2,  1900, 0, 4,     12, 0x0b0a0907, 0x0707070a,},
	{2,  2200, 0, 4,     12, 0x0b0a0907, 0x0707070a,},
	{1,  1666, 0, 4,     12, 0x0b0a0907, 0x0707070a,},
	{1,  1900, 0, 4,     12, 0x0b0a0907, 0x0707070a,},
	{1,  2200, 0, 4,     12, 0x0b0a0907, 0x0707070a,},
	{}
#else
	{2,  2140, 0, 4,     4, 0x0, 0x0},
	{1,  2140, 0, 4,     4, 0x0, 0x0},
	{}
#endif
};

static const struct board_specific_parameters *udimms[] = {
	udimm0,
};
#endif
