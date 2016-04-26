/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <i2c.h>
#include <fdt_support.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/fsl_serdes.h>
#include <asm/arch/fdt.h>
#include <asm/arch/soc.h>
#include <ahci.h>
#include <hwconfig.h>
#include <mmc.h>
#include <scsi.h>
#include <fm_eth.h>
#include <fsl_csu.h>
#include <fsl_esdhc.h>
#include <fsl_mmdc.h>
#include <spl.h>
#include <netdev.h>

#include "../common/qixis.h"
#include "ls1012aqds_qixis.h"

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	puts("Board: LS1012AQDS\n");

	return 0;
}

void mmdc_init(void)
{
	struct mmdc_p_regs *mmdc =
		(struct mmdc_p_regs *)CONFIG_SYS_FSL_DDR_ADDR;

	/* Set MMDC_MDSCR[CON_REQ] */
	out_be32(&mmdc->mdscr, 0x00008000);

	/* configure timing parms */
	out_be32(&mmdc->mdotc, 0x12554000);
	out_be32(&mmdc->mdcfg0, 0xbabf7954);
	out_be32(&mmdc->mdcfg1, 0xff328f64);
	out_be32(&mmdc->mdcfg2, 0x01ff00db);

	/* other parms	*/
	out_be32(&mmdc->mdmisc,    0x00000680);
	out_be32(&mmdc->mpmur0,    0x00000800);
	out_be32(&mmdc->mdrwd,     0x00002000);
	out_be32(&mmdc->mpodtctrl, 0x0000022a);

	/* out of reset delays */
	out_be32(&mmdc->mdor, 0x00bf1023);

	/* physical parms */
	out_be32(&mmdc->mdctl, 0x05180000);
	out_be32(&mmdc->mdasp, 0x0000007f);

	/* Enable MMDC */
	out_be32(&mmdc->mdctl, 0x85180000);

	/* dram init sequence: update MRs */
	out_be32(&mmdc->mdscr, 0x00088032);
	out_be32(&mmdc->mdscr, 0x00008033);
	out_be32(&mmdc->mdscr, 0x00048031);
	out_be32(&mmdc->mdscr, 0x19308030);

	/* dram init sequence: ZQCL */
	out_be32(&mmdc->mdscr,	    0x04008040);
	out_be32(&mmdc->mpzqhwctrl, 0xa1390003);

	mdelay(100);

	/* Calibrations now: wr lvl */
	out_be32(&mmdc->mdscr,   0x00848031);
	out_be32(&mmdc->mdscr,   0x00008200);
	out_be32(&mmdc->mpwlgcr, 0x00000001);

	mdelay(100);

	out_be32(&mmdc->mdscr, 0x00048031);
	out_be32(&mmdc->mdscr, 0x00008000);

	/*    manual_refresh */
	out_be32(&mmdc->mdscr, 0x00008020);

	mdelay(100);

	/* Calibrations now: Read DQS gating calibration */
	out_be32(&mmdc->mdscr,     0x04008050);
	out_be32(&mmdc->mdscr,     0x00048033);
	out_be32(&mmdc->mppdcmpr2, 0x00000001);
	out_be32(&mmdc->mprddlctl, 0x40404040);
	out_be32(&mmdc->mpdgctrl0, 0x10000000);

	mdelay(100);

	out_be32(&mmdc->mdscr, 0x00008033);

	/*   manual_refresh */
	out_be32(&mmdc->mdscr, 0x00008020);

	mdelay(100);

	/* Calibrations now: Read calibration */
	out_be32(&mmdc->mdscr,       0x04008050);
	out_be32(&mmdc->mdscr,       0x00048033);
	out_be32(&mmdc->mppdcmpr2,   0x00000001);
	out_be32(&mmdc->mprddlhwctl, 0x00000010);

	mdelay(400);

	out_be32(&mmdc->mdscr, 0x00008033);

	/* manual_refresh */
	out_be32(&mmdc->mdscr, 0x00008020);

	mdelay(100);

	/* PD, SR */
	out_be32(&mmdc->mdpdc, 0x00030035);
	out_be32(&mmdc->mapsr, 0x00001067);

	/* refresh scheme */
	out_be32(&mmdc->mdref, 0x103e8000);

	mdelay(400);

	/* disable CON_REQ */
	out_be32(&mmdc->mdscr, 0x0);

	mdelay(50);
}

int dram_init(void)
{
	mmdc_init();

	gd->ram_size = 0x40000000;

	return 0;
}

int board_early_init_f(void)
{
	fsl_lsch2_early_init_f();

	return 0;
}

#ifdef CONFIG_MISC_INIT_R
int misc_init_r(void)
{
	u8 mux_sdhc_cd = 0x80;

	i2c_set_bus_num(0);

	i2c_write(CONFIG_SYS_I2C_FPGA_ADDR, 0x5a, 1, &mux_sdhc_cd, 1);
	return 0;
}
#endif

int board_init(void)
{
	struct ccsr_cci400 *cci = (struct ccsr_cci400 *)
				   CONFIG_SYS_CCI400_ADDR;

	/* Set CCI-400 control override register to enable barrier
	 * transaction */
	out_le32(&cci->ctrl_ord,
		 CCI400_CTRLORD_EN_BARRIER);

#ifdef CONFIG_LAYERSCAPE_NS_ACCESS
	enable_layerscape_ns_access();
#endif

#ifdef CONFIG_ENV_IS_NOWHERE
	gd->env_addr = (ulong)&default_environment[0];
#endif
	return 0;
}

int board_eth_init(bd_t *bis)
{
	return pci_eth_init(bis);
}

#ifdef CONFIG_OF_BOARD_SETUP
int ft_board_setup(void *blob, bd_t *bd)
{
	u64 base[CONFIG_NR_DRAM_BANKS];
	u64 size[CONFIG_NR_DRAM_BANKS];

	/* fixup DT for the two DDR banks */
	base[0] = gd->bd->bi_dram[0].start;
	size[0] = gd->bd->bi_dram[0].size;
	base[1] = gd->bd->bi_dram[1].start;
	size[1] = gd->bd->bi_dram[1].size;

	fdt_fixup_memory_banks(blob, base, size, 2);
	ft_cpu_setup(blob, bd);

	return 0;
}
#endif
