/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/fsl_serdes.h>

struct serdes_config {
	u8 protocol;
	u8 lanes[SRDS_MAX_LANES];
};

static struct serdes_config serdes1_cfg_tbl[] = {
	/* SerDes 1 */
	{0x12, {SGMII3, SGMII7, SGMII1, SGMII2 } },
	{0x15, {SGMII3, SGMII7, XFI1, XFI2 } },
	{0x16, {SGMII3, SGMII7, SGMII1, XFI2 } },
	{0x17, {SGMII3, SGMII7, SGMII1, SGMII2 } },
	{0x18, {SGMII3, SGMII7, SGMII1, SGMII2 } },
	{0x19, {SGMII3, QSGMII_B, XFI1, XFI2} },
	{0x1A, {SGMII3, QSGMII_B, SGMII1, XFI2 } },
	{0x1B, {SGMII3, QSGMII_B, SGMII1, SGMII2 } },
	{0x1C, {SGMII3, QSGMII_B, SGMII1, SGMII2 } },
	{0x1D, {QSGMII_A, QSGMII_B, XFI1, XFI2 } },
	{0x1E, {QSGMII_A, QSGMII_B, SGMII1, XFI2  } },
	{0x1F, {QSGMII_A, QSGMII_B, SGMII1, SGMII2  } },
	{0x20, {QSGMII_A, QSGMII_B, SGMII1, SGMII2} },
	{0x35, {SGMII3, QSGMII_B, SGMII1, SGMII2} },
	{0x36, {QSGMII_A, QSGMII_B, SGMII1, SGMII2 } },
		{}
};
static struct serdes_config serdes2_cfg_tbl[] = {
	/* SerDes 2 */
	{0x0C, {PCIE1, PCIE1, PCIE1, PCIE1 } },
	{0x0D, {PCIE1, PCIE2, PCIE3, SATA1 } },
	{0x0E, {PCIE1, PCIE1, PCIE2, SATA1 } },
	{0x13, {PCIE1, PCIE1, PCIE3, PCIE3 } },
	{0x14, {PCIE1, PCIE2, PCIE3, PCIE3 } },
	{}
};

static struct serdes_config *serdes_cfg_tbl[] = {
	serdes1_cfg_tbl,
	serdes2_cfg_tbl,
};

enum srds_prtcl serdes_get_prtcl(int serdes, int cfg, int lane)
{
	struct serdes_config *ptr;

	if (serdes >= ARRAY_SIZE(serdes_cfg_tbl))
		return 0;

	ptr = serdes_cfg_tbl[serdes];
	while (ptr->protocol) {
		if (ptr->protocol == cfg)
			return ptr->lanes[lane];
		ptr++;
	}

	return 0;
}

int is_serdes_prtcl_valid(int serdes, u32 prtcl)
{
	int i;
	struct serdes_config *ptr;

	if (serdes >= ARRAY_SIZE(serdes_cfg_tbl))
		return 0;

	ptr = serdes_cfg_tbl[serdes];
	while (ptr->protocol) {
		if (ptr->protocol == prtcl)
			break;
		ptr++;
	}

	if (!ptr->protocol)
		return 0;

	for (i = 0; i < SRDS_MAX_LANES; i++) {
		if (ptr->lanes[i] != NONE)
			return 1;
	}

	return 0;
}
