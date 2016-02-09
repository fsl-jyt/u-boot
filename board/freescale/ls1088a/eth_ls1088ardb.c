/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 *
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <netdev.h>
#include <malloc.h>
#include <fsl_mdio.h>
#include <miiphy.h>
#include <phy.h>
#include <fm_eth.h>
#include <asm/io.h>
#include <exports.h>
#include <asm/arch/fsl_serdes.h>
#include <fsl-mc/ldpaa_wriop.h>

DECLARE_GLOBAL_DATA_PTR;

#define MC_BOOT_ENV_VAR "mcinitcmd"
int board_eth_init(bd_t *bis)
{
#if defined(CONFIG_FSL_MC_ENET)
	char *mc_boot_env_var;
	struct ccsr_gur *gur = (void *)(CONFIG_SYS_FSL_GUTS_ADDR);
	u32 srds_s1, cfg;

	cfg = in_le32(&gur->rcwsr[FSL_CHASSIS3_SRDS1_REGSR - 1]) &
				FSL_CHASSIS3_SRDS1_PRTCL_MASK;
	cfg >>= FSL_CHASSIS3_SRDS1_PRTCL_SHIFT;

	srds_s1 = serdes_get_number(FSL_SRDS_1, cfg);

	switch (srds_s1) {
	case 0x1D:
		/* TDB: Update as per RDB board */
		wriop_set_phy_address(WRIOP1_DPMAC1, 1);
		wriop_set_phy_address(WRIOP1_DPMAC2, 2);
		wriop_set_phy_address(WRIOP1_DPMAC3, 3);
		wriop_set_phy_address(WRIOP1_DPMAC4, 4);
		wriop_set_phy_address(WRIOP1_DPMAC5, 5);
		wriop_set_phy_address(WRIOP1_DPMAC6, 6);
		wriop_set_phy_address(WRIOP1_DPMAC7, 7);
		wriop_set_phy_address(WRIOP1_DPMAC8, 8);
		wriop_set_phy_address(WRIOP1_DPMAC9, 9);
		wriop_set_phy_address(WRIOP1_DPMAC10, 10);

		break;
	default:
		printf("SerDes1 protocol 0x%x is not supported on LS1088ARDB\n",
		       srds_s1);
		break;
	}

	mc_boot_env_var = getenv(MC_BOOT_ENV_VAR);
	if (mc_boot_env_var)
		run_command_list(mc_boot_env_var, -1, 0);
	cpu_eth_init(bis);
#endif /* CONFIG_FMAN_ENET */

	return pci_eth_init(bis);
}
