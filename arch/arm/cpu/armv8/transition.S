/*
 * (C) Copyright 2013
 * David Feng <fenghua@phytium.com.cn>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm-offsets.h>
#include <config.h>
#include <linux/linkage.h>
#include <asm/macro.h>

ENTRY(armv8_switch_to_el2)
	switch_el x0, 1f, 0f, 0f
0:	ret
1:	armv8_switch_to_el2_m x0
ENDPROC(armv8_switch_to_el2)

ENTRY(armv8_switch_to_el1)
	switch_el x0, 0f, 1f, 0f
0:	ret
1:	armv8_switch_to_el1_m x0, x1
ENDPROC(armv8_switch_to_el1)

/*
 * x0: kernel entry point
 * x1: machine nr
 * x2: fdt address
 */
ENTRY(armv8_switch_to_el2_aarch32)
	switch_el x3, 1f, 0f, 0f
0:	ret
1:
	mov	x7, x0
	mov	x8, x1
	mov	x9, x2

	/* 32bit EL2 | HCE | SMD | RES1 (Bits[5:4]) | Non-secure EL0/EL1 */
	mov	x1, 0x1b1
	msr	scr_el3, x1
	msr	cptr_el3, xzr	/* Disable coprocessor traps to EL3 */
	mov	x1, 0x33ff
	msr	cptr_el2, x1	/* Disable coprocessor traps to EL2 */

	/* Initialize Generic Timers */
	msr	cntvoff_el2, xzr

	mov	x1, #0x0830
	movk	x1, #0x30c5, lsl #16
	msr	sctlr_el2, x1

	/* Return to AArch32 Hypervisor mode */
	mov	x1, sp
	msr	sp_el2, x1
	mrs	x1, vbar_el3
	msr	vbar_el2, x1	/* Migrate VBAR */
	mov	x1, #0x1da
	msr	spsr_el3, x1
	msr	elr_el3, x7

	mov	x0, #0
	mov	x1, x8
	mov	x2, x9

	eret
ENDPROC(armv8_switch_to_el2_aarch32)

/*
 * x0: kernel entry point
 * x1: machine nr
 * x2: fdt address
 */
ENTRY(armv8_switch_to_el1_aarch32)
	switch_el x3, 0f, 1f, 0f
0:	ret
1:
	mov	x7, x0
	mov	x8, x1
	mov	x9, x2

	/* Initialize Generic Timers */
	mrs	x0, cnthctl_el2
	orr	x0, x0, #0x3		/* Enable EL1 access to timers */
	msr	cnthctl_el2, x0
	msr	cntvoff_el2, xzr

        /* Initialize MPID/MPIDR registers */
	mrs	x0, midr_el1
	mrs	x1, mpidr_el1
	msr	vpidr_el2, x0
	msr	vmpidr_el2, x1

        /* Disable coprocessor traps */
	mov	x0, #0x33ff
	msr	cptr_el2, x0		/* Disable coprocessor traps to EL2 */
        msr	hstr_el2, xzr		/* Disable coprocessor traps to EL2 */
        mov	x0, #3 << 20
        msr	cpacr_el1, x0		/* Enable FP/SIMD at EL1 */

	/* Initialize HCR_EL2 */
	mov	x0, #(0 << 31)		/* 32bit EL1 */
	orr	x0, x0, #(1 << 29)	/* Disable HVC */
	msr	hcr_el2, x0

	mov	x0, #0x0800
	movk	x0, #0x30d0, lsl #16
	msr	sctlr_el1, x0

	/* Return to AArch32 Supervisor mode */
	mov	x0, sp
	msr	sp_el1, x0		/* Migrate SP */
	mrs	x0, vbar_el2
	msr	vbar_el1, x0		/* Migrate VBAR */
	mov     x0, #0x1d3
	msr	spsr_el2, x0
	msr	elr_el2, x7

	mov	x0, #0
	mov	x1, x8
	mov	x2, x9

	eret
ENDPROC(armv8_switch_to_el1_aarch32)
