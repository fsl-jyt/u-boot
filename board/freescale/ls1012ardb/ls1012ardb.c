/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <i2c.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/fsl_serdes.h>
#include <asm/arch/ppa.h>
#include <asm/arch/soc.h>
#include <hwconfig.h>
#include <ahci.h>
#include <mmc.h>
#include <scsi.h>
#include <fsl_csu.h>
#include <fsl_esdhc.h>
#include <environment.h>
#include <fsl_mmdc.h>
#include <netdev.h>

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	u8 in1;

	puts("Board: LS1012ARDB ");

	/* Initialize i2c early for Serial flash bank information */
	i2c_set_bus_num(0);

	if (i2c_read(I2C_MUX_IO1_ADDR, 1, 1, &in1, 1) < 0) {
		printf("Error reading i2c boot information!\n");
		return 0; /* Don't want to hang() on this error */
	}

	puts("Version");
	if ((in1 & (~__SW_REV_MASK)) == __SW_REV_A)
		puts(": RevA");
	else if ((in1 & (~__SW_REV_MASK)) == __SW_REV_B)
		puts(": RevB");
	else
		puts(": unknown");

	printf(", boot from QSPI");
	if ((in1 & (~__SW_BOOT_MASK)) == __SW_BOOT_EMU)
		puts(": emu\n");
	else if ((in1 & (~__SW_BOOT_MASK)) == __SW_BOOT_BANK1)
		puts(": bank1\n");
	else if ((in1 & (~__SW_BOOT_MASK)) == __SW_BOOT_BANK2)
		puts(": bank2\n");
	else
		puts("unknown\n");

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

int board_eth_init(bd_t *bis)
{
	return pci_eth_init(bis);
}

int board_early_init_f(void)
{
	fsl_lsch2_early_init_f();

	return 0;
}

int board_init(void)
{
	struct ccsr_cci400 *cci = (struct ccsr_cci400 *)CONFIG_SYS_CCI400_ADDR;
	/*
	 * Set CCI-400 control override register to enable barrier
	 * transaction
	 */
	out_le32(&cci->ctrl_ord, CCI400_CTRLORD_EN_BARRIER);

#ifdef CONFIG_ENV_IS_NOWHERE
	gd->env_addr = (ulong)&default_environment[0];
#endif

#ifdef CONFIG_LAYERSCAPE_NS_ACCESS
	enable_layerscape_ns_access();
#endif

	return 0;
}

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
