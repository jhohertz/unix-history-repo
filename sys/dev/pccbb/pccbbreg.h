/*
 * Copyright (c) 2000,2001 Jonathan Chen.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification, immediately at the beginning of the file.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

/*
 * Register definitions for PCI to Cardbus Bridge chips
 */


/* PCI header registers */
#define	PCCBBR_SOCKBASE				0x10	/* len=4 */

#define	PCCBBR_MEMBASE0				0x1c	/* len=4 */
#define	PCCBBR_MEMLIMIT0			0x20	/* len=4 */
#define	PCCBBR_MEMBASE1				0x24	/* len=4 */
#define	PCCBBR_MEMLIMIT1			0x28	/* len=4 */
#define	PCCBBR_IOBASE0				0x2c	/* len=4 */
#define	PCCBBR_IOLIMIT0				0x30	/* len=4 */
#define	PCCBBR_IOBASE1				0x34	/* len=4 */
#define	PCCBBR_IOLIMIT1				0x38	/* len=4 */
#define	PCCBB_MEMALIGN				4096
#define	PCCBB_IOALIGN				4

#define	PCCBBR_INTRLINE				0x3c	/* len=1 */
#define	PCCBBR_INTRPIN				0x3d	/* len=1 */
#define	PCCBBR_BRIDGECTRL			0x3e	/* len=2 */
# define	PCCBBM_BRIDGECTRL_MASTER_ABORT		0x0020
# define	PCCBBM_BRIDGECTRL_RESET			0x0040
# define	PCCBBM_BRIDGECTRL_INTR_IREQ_EN		0x0080
# define	PCCBBM_BRIDGECTRL_PREFETCH_0		0x0100
# define	PCCBBM_BRIDGECTRL_PREFETCH_1		0x0200
# define	PCCBBM_BRIDGECTRL_WRITE_POST_EN		0x0400
  /* additional bit for RF5C46[567] */
# define	PCCBBM_BRIDGECTRL_RL_3E0_EN		0x0800
# define	PCCBBM_BRIDGECTRL_RL_3E2_EN		0x1000

#define	PCCBBR_LEGACY				0x44	/* len=4 */

#define	PCCBBR_CBCTRL				0x91	/* len=1 */
  /* bits for TI 113X */
# define	PCCBBM_CBCTRL_113X_RI_EN		0x80
# define	PCCBBM_CBCTRL_113X_ZV_EN		0x40
# define	PCCBBM_CBCTRL_113X_PCI_IRQ_EN		0x20
# define	PCCBBM_CBCTRL_113X_PCI_INTR		0x10
# define	PCCBBM_CBCTRL_113X_PCI_CSC		0x08
# define	PCCBBM_CBCTRL_113X_PCI_CSC_D		0x04
# define	PCCBBM_CBCTRL_113X_SPEAKER_EN		0x02
# define	PCCBBM_CBCTRL_113X_INTR_DET		0x01
  /* bits for TI 12XX */
# define	PCCBBM_CBCTRL_12XX_RI_EN		0x80
# define	PCCBBM_CBCTRL_12XX_ZV_EN		0x40
# define	PCCBBM_CBCTRL_12XX_AUD2MUX		0x04
# define	PCCBBM_CBCTRL_12XX_SPEAKER_EN		0x02
# define	PCCBBM_CBCTRL_12XX_INTR_DET		0x01
#define	PCCBBR_DEVCTRL				0x92	/* len=1 */
# define	PCCBBM_DEVCTRL_INT_SERIAL		0x04
# define	PCCBBM_DEVCTRL_INT_PCI			0x02

#define	PCCBBR_TOPIC_SOCKETCTRL			0x90
# define	PCCBBM_TOPIC_SOCKETCTRL_SCR_IRQSEL	0x00000001 /* PCI intr */

#define	PCCBBR_TOPIC_SLOTCTRL			0xa0
# define	PCCBBM_TOPIC_SLOTCTRL_SLOTON		0x00000080
# define	PCCBBM_TOPIC_SLOTCTRL_SLOTEN		0x00000040
# define	PCCBBM_TOPIC_SLOTCTRL_ID_LOCK		0x00000020
# define	PCCBBM_TOPIC_SLOTCTRL_ID_WP		0x00000010
# define	PCCBBM_TOPIC_SLOTCTRL_PORT_MASK		0x0000000c
# define	PCCBBM_TOPIC_SLOTCTRL_PORT_SHIFT	2
# define	PCCBBM_TOPIC_SLOTCTRL_OSF_MASK		0x00000003
# define	PCCBBM_TOPIC_SLOTCTRL_OSF_SHIFT		0
# define	PCCBBM_TOPIC_SLOTCTRL_INTB		0x00002000
# define	PCCBBM_TOPIC_SLOTCTRL_INTA		0x00001000
# define	PCCBBM_TOPIC_SLOTCTRL_INT_MASK		0x00003000
# define	PCCBBM_TOPIC_SLOTCTRL_CLOCK_MASK	0x00000c00
# define	PCCBBM_TOPIC_SLOTCTRL_CLOCK_2		0x00000800 /* PCI Clk/2 */
# define	PCCBBM_TOPIC_SLOTCTRL_CLOCK_1		0x00000400 /* PCI Clk */
# define	PCCBBM_TOPIC_SLOTCTRL_CLOCK_0		0x00000000 /* no clock */
# define	PCCBBM_TOPIC_SLOTCTRL_CARDBUS		0x80000000
# define	PCCBBM_TOPIC_SLOTCTRL_VS1		0x04000000
# define	PCCBBM_TOPIC_SLOTCTRL_VS2		0x02000000
# define	PCCBBM_TOPIC_SLOTCTRL_SWDETECT		0x01000000

/* Socket definitions */
#define	PCCBB_SOCKET_EVENT_CSTS		0x01	/* Card Status Change */
#define	PCCBB_SOCKET_EVENT_CD1		0x02	/* Card Detect 1 */
#define	PCCBB_SOCKET_EVENT_CD2		0x04	/* Card Detect 2 */
#define	PCCBB_SOCKET_EVENT_CD		0x06	/* Card Detect all */
#define	PCCBB_SOCKET_EVENT_POWER	0x08	/* Power Cycle */

#define	PCCBB_SOCKET_MASK_CSTS		0x01	/* Card Status Change */
#define	PCCBB_SOCKET_MASK_CD		0x06	/* Card Detect */
#define	PCCBB_SOCKET_MASK_POWER		0x08	/* Power Cycle */

#define	PCCBB_SOCKET_STAT_CARDSTS	0x00000001	/* Card Status Change */
#define	PCCBB_SOCKET_STAT_CD1		0x00000002	/* Card Detect 1 */
#define	PCCBB_SOCKET_STAT_CD2		0x00000004	/* Card Detect 2 */
#define	PCCBB_SOCKET_STAT_CD		0x00000006	/* Card Detect all */
#define	PCCBB_SOCKET_STAT_PWRCYCLE	0x00000008	/* Power Cycle */
#define	PCCBB_SOCKET_STAT_16BIT		0x00000010	/* 16-bit Card */
#define	PCCBB_SOCKET_STAT_CB		0x00000020	/* Cardbus Card */
#define	PCCBB_SOCKET_STAT_IREQ		0x00000040	/* Ready */
#define	PCCBB_SOCKET_STAT_NOTCARD	0x00000080	/* Unrecognized Card */
#define	PCCBB_SOCKET_STAT_DATALOST	0x00000100	/* Data Lost */
#define	PCCBB_SOCKET_STAT_BADVCC	0x00000200	/* Bad VccRequest */
#define	PCCBB_SOCKET_STAT_5VCARD	0x00000400	/* 5 V Card */
#define	PCCBB_SOCKET_STAT_3VCARD	0x00000800	/* 3.3 V Card */
#define	PCCBB_SOCKET_STAT_XVCARD	0x00001000	/* X.X V Card */
#define	PCCBB_SOCKET_STAT_YVCARD	0x00002000	/* Y.Y V Card */
#define	PCCBB_SOCKET_STAT_5VSOCK	0x10000000	/* 5 V Socket */
#define	PCCBB_SOCKET_STAT_3VSOCK	0x20000000	/* 3.3 V Socket */
#define	PCCBB_SOCKET_STAT_XVSOCK	0x40000000	/* X.X V Socket */
#define	PCCBB_SOCKET_STAT_YVSOCK	0x80000000	/* Y.Y V Socket */

#define	PCCBB_SOCKET_FORCE_BADVCC	0x0200	/* Bad Vcc Request */

#define	PCCBB_SOCKET_CTRL_VPPMASK	0x07
#define	PCCBB_SOCKET_CTRL_VPP_OFF	0x00
#define	PCCBB_SOCKET_CTRL_VPP_12V	0x01
#define	PCCBB_SOCKET_CTRL_VPP_5V	0x02
#define	PCCBB_SOCKET_CTRL_VPP_3V	0x03
#define	PCCBB_SOCKET_CTRL_VPP_XV	0x04
#define	PCCBB_SOCKET_CTRL_VPP_YV	0x05

#define	PCCBB_SOCKET_CTRL_VCCMASK	0x70
#define	PCCBB_SOCKET_CTRL_VCC_OFF	0x00
#define	PCCBB_SOCKET_CTRL_VCC_5V	0x20
#define	PCCBB_SOCKET_CTRL_VCC_3V	0x30
#define	PCCBB_SOCKET_CTRL_VCC_XV	0x40
#define	PCCBB_SOCKET_CTRL_VCC_YV	0x50

#define	PCCBB_SOCKET_CTRL_STOPCLK	0x80

#include <dev/pccbb/pccbbdevid.h>

#define PCCBB_SOCKET_EVENT		0x00
#define PCCBB_SOCKET_MASK		0x04
#define PCCBB_SOCKET_STATE		0x08
#define PCCBB_SOCKET_FORCE		0x0c
#define PCCBB_SOCKET_CONTROL		0x10
#define PCCBB_SOCKET_POWER		0x14
