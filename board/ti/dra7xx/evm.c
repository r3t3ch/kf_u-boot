/*
 * (C) Copyright 2013
 * Texas Instruments Incorporated, <www.ti.com>
 *
 * Lokesh Vutla <lokeshvutla@ti.com>
 *
 * Based on previous work by:
 * Aneesh V       <aneesh@ti.com>
 * Steve Sakoman  <steve@sakoman.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
#include <palmas.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mmc_host_def.h>

#include "mux_data.h"

#ifdef CONFIG_USB_EHCI
#include <usb.h>
#include <asm/arch/ehci.h>
#include <asm/ehci-omap.h>
#endif

#ifdef CONFIG_DRIVER_TI_CPSW
#include <cpsw.h>
#endif

#ifdef CONFIG_BOOTIPU1
#include <usb/fastboot.h>
#include <mmc.h>
#include <malloc.h>

#endif


#define CTRL_CORE_MPU_IRQ_159_REG			0x4a002b76
#define CTRL_CORE_MPU_IRQ_152_REG			0x4A002b68
#define CTRL_CORE_MPU_IRQ_158_REG			0x4a002b74
#define CTRL_CORE_MPU_IRQ_155_REG			0x4a002b6e
#define CTRL_CORE_MPU_IRQ_154_REG			0x4a002b6c
#define CTRL_CORE_MPU_IRQ_156_REG			0x4a002b70
#define CTRL_CORE_MPU_IRQ_157_REG			0x4a002b72
#define CTRL_CORE_MPU_IRQ_136_REG			0x4a002b48
#define CTRL_CORE_MPU_IRQ_141_REG			0x4a002b52
#define CTRL_CORE_MPU_IRQ_142_REG			0x4a002b54
#define CTRL_CORE_MPU_IRQ_143_REG			0x4a002b56
#define CTRL_CORE_MPU_IRQ_144_REG			0x4a002b58
#define CTRL_CORE_MPU_IRQ_145_REG			0x4a002b5a
#define CTRL_CORE_MPU_IRQ_146_REG			0x4a002b5c
#define CTRL_CORE_MPU_IRQ_147_REG			0x4a002b5e
#define CTRL_CORE_MPU_IRQ_124_REG			0x4a002b34
#define CTRL_CORE_MPU_IRQ_50_REG			0x4a002aa0
#define CTRL_CORE_MPU_IRQ_51_REG			0x4a002aa2
#define CTRL_CORE_MPU_IRQ_52_REG			0x4a002aa4
#define CTRL_CORE_MPU_IRQ_53_REG			0x4a002aa6
#define CTRL_CORE_MPU_IRQ_81_REG			0X4a002ade
#define CTRL_CORE_MPU_IRQ_82_REG			0X4a002ae0

#define CTRL_CORE_DMA_SYSTEM_DREQ_79_REG		0x4a002c16
#define CTRL_CORE_DMA_SYSTEM_DREQ_78_REG		0x4a002c14
#define CTRL_CORE_DMA_SYSTEM_DREQ_63_REG		0x4a002bf6
#define CTRL_CORE_DMA_SYSTEM_DREQ_62_REG		0x4a002bf4
#define CTRL_CORE_DMA_SYSTEM_DREQ_69_REG		0x4a002c02
#define CTRL_CORE_DMA_SYSTEM_DREQ_70_REG		0x4a002c04

DECLARE_GLOBAL_DATA_PTR;

const struct omap_sysinfo sysinfo = {
	"Board: DRA7xx\n"
};

#ifdef CONFIG_BOOTIPU1
#define L4_CFG_TARG                  0x4A000000
#define L4_WKUP_TARG                 0x4AE00000
#define IPU2_TARGET_TARG             0x55000000
#define IPU1_TARGET_TARG             0x58800000
#define CTRL_MODULE_CORE             (L4_CFG_TARG + 0x2000)
#define CM_CORE_AON                  (L4_CFG_TARG + 0x5000)
#define CM_CORE                      (L4_CFG_TARG + 0x8000)
#define PRM                          (L4_WKUP_TARG + 0x6000)
#define MPU_CM_CORE_AON		     (CM_CORE_AON + 0x300)
#define IPU_CM_CORE_AON              (CM_CORE_AON + 0x500)
#define RTC_CM_CORE_AON		     (CM_CORE_AON + 0x740)
#define VPE_CM_CORE_AON		     (CM_CORE_AON + 0x760)
#define COREAON_CM_CORE		     (CM_CORE + 0x600)
#define CORE_CM_CORE                 (CM_CORE + 0x700)
#define CAM_CM_CORE		     (CM_CORE + 0x1000)
#define DSS_CM_CORE		     (CM_CORE + 0x1100)
#define L3INIT_CM_CORE		     (CM_CORE + 0x1300)
#define L4PER_CM_CORE		     (CM_CORE + 0x1700)
#define CKGEN_PRM		     (PRM + 0x100)
#define IPU_PRM                      (PRM + 0x500)
#define CORE_PRM                     (PRM + 0x700)
#define WKUPAON_CM		     (PRM + 0x1800)

#define CM_L3MAIN1_CLKSTCTRL	     (CORE_CM_CORE + 0x000)
#define CM_IPU2_CLKSTCTRL	     (CORE_CM_CORE + 0x200)
#define CM_DMA_CLKSTCTRL	     (CORE_CM_CORE + 0x300)
#define CM_EMIF_CLKSTCTRL	     (CORE_CM_CORE + 0x400)
#define CM_L4CFG_CLKSTCTRL	     (CORE_CM_CORE + 0x600)

#define CM_DSS_CLKSTCTRL	     (DSS_CM_CORE + 0x00)
#define CM_CAM_CLKSTCTRL	     (CAM_CM_CORE + 0x00)
#define CM_COREAON_CLKSTCTRL	     (COREAON_CM_CORE + 0x00)
#define CM_L3INIT_CLKSTCTRL	     (L3INIT_CM_CORE + 0x00)
#define CM_GMAC_CLKSTCTRL	     (L3INIT_CM_CORE + 0xC0)
#define CM_L4PER_CLKSTCTRL	     (L4PER_CM_CORE + 0x000)
#define CM_L4PER2_CLKSTCTRL	     (L4PER_CM_CORE + 0x1FC)
#define CM_L4PER3_CLKSTCTRL	     (L4PER_CM_CORE + 0x210)
#define CM_MPU_CLKSTCTRL	     (MPU_CM_CORE_AON + 0x00)
#define CM_RTC_CLKSTCTRL	     (RTC_CM_CORE_AON + 0x00)
#define CM_VPE_CLKSTCTRL	     (VPE_CM_CORE_AON + 0x00)
#define CM_WKUPAON_CLKSTCTRL	     (WKUPAON_CM + 0x00)

#define RM_IPU1_RSTCTRL              (IPU_PRM + 0x10)
#define RM_IPU1_RSTST                (IPU_PRM + 0x14)
#define CM_IPU1_CLKSTCTRL            (IPU_CM_CORE_AON + 0x0)
#define CM_IPU1_IPU1_CLKCTRL         (IPU_CM_CORE_AON + 0x20)
#define CM_IPU2_IPU2_CLKCTRL	     (CORE_CM_CORE + 0x220)
#define CM_IPU_CLKSTCTRL	     (IPU_CM_CORE_AON + 0x40)
#define CM_IPU_MCASP1_CLKCTRL	     (IPU_CM_CORE_AON + 0x50)
#define CM_IPU_TIMER5_CLKCTRL	     (IPU_CM_CORE_AON + 0x58)
#define CM_IPU_TIMER6_CLKCTRL	     (IPU_CM_CORE_AON + 0x60)
#define CM_IPU_TIMER7_CLKCTRL	     (IPU_CM_CORE_AON + 0x68)
#define CM_IPU_TIMER8_CLKCTRL	     (IPU_CM_CORE_AON + 0x70)
#define CM_L4PER2_L4_PER2_CLKCTRL    (L4PER_CM_CORE + 0x0C)
#define CM_L4PER3_L4_PER3_CLKCTRL    (L4PER_CM_CORE + 0x14)
#define CM_L4PER_I2C1_CLKCTRL	     (L4PER_CM_CORE + 0xA0)
#define CM_L4PER_I2C2_CLKCTRL	     (L4PER_CM_CORE + 0xA8)
#define CM_L4PER_I2C3_CLKCTRL	     (L4PER_CM_CORE + 0xB0)
#define CM_L4PER_I2C4_CLKCTRL	     (L4PER_CM_CORE + 0xB8)
#define CM_L4PER_L4_PER1_CLKCTRL     (L4PER_CM_CORE + 0xC0)
#define CM_CAM_VIP1_CLKCTRL	     (CAM_CM_CORE + 0x20)
#define CM_CAM_VIP2_CLKCTRL	     (CAM_CM_CORE + 0x28)
#define CM_CAM_VIP3_CLKCTRL	     (CAM_CM_CORE + 0x30)
#define CM_DMA_DMA_SYSTEM_CLKCTRL    (CORE_CM_CORE + 0x320)
#define CM_L3INSTR_L3_MAIN_2_CLKCTRL (CORE_CM_CORE + 0x728)
#define CM_DSS_DSS_CLKCTRL	     (DSS_CM_CORE + 0x20)
#define CM_VPE_VPE_CLKCTRL	     (VPE_CM_CORE_AON + 0x04)

#define CTRL_CORE_CONTROL_IO_2	     (CTRL_MODULE_CORE + 0x558)

#define IPU1_BASE_ADDR               (IPU1_TARGET_TARG + 0x20000)
#define IPU1_MMU_CFG                 (IPU1_TARGET_TARG + 0x80000)

#define CTRL_CORE_CORTEX_M4_MMUADDRTRANSLTR	0x4A002358
#define CTRL_CORE_CORTEX_M4_MMUADDRLOGICTR	0x4A00235C

#define IPUMMU_MAP_LARGE_PAGE(pageNum, logicalAddress, translatedAddress, flags) \
			__raw_writel(logicalAddress, IPU1_MMU_CFG+0x800 + pageNum*0x4); \
			__raw_writel(translatedAddress, IPU1_MMU_CFG+0x820 + pageNum*0x4); \
			__raw_writel(flags, IPU1_MMU_CFG+0x840 + pageNum*0x4);

#define IPUMMU_MAP_MEDIUM_PAGE(pageNum, logicalAddress, translatedAddress, flags) \
			__raw_writel(logicalAddress, IPU1_MMU_CFG+0x860 + pageNum*0x4); \
			__raw_writel(translatedAddress, IPU1_MMU_CFG+0x8A0 + pageNum*0x4); \
			__raw_writel(flags, IPU1_MMU_CFG+0x8E0 + pageNum*0x4);

#define IPUMMU_MAP_SMALL_PAGE(pageNum, logicalAddress, translatedAddress, flags) \
			__raw_writel(logicalAddress, IPU1_MMU_CFG+0x920 + pageNum*0x4); \
			__raw_writel(translatedAddress, IPU1_MMU_CFG+0x9A0 + pageNum*0x4); \
			__raw_writel(flags, IPU1_MMU_CFG+0xA20 + pageNum*0x4);


void reset_ipu(void)
{
	/* printf("bringing IPU1-M4 out of reset\n"); */

	/* bring the IPU1 out of reset */
	__raw_writel(0x0, RM_IPU1_RSTCTRL);

	/* printf("checking if IPU1 is ready or not\n"); */
	/* check module is functional or not */
	/* printf("!! value = 0x%x !!\n",__raw_readl(RM_IPU1_RSTST)); */
	while(((__raw_readl(RM_IPU1_RSTST)&0x7)!=0x7));
	__raw_writel(0x7, RM_IPU1_RSTST);
}

void enable_ipu(void)
{
	u32 reg;

	/* enable CORE domain transitions */
	__raw_writel(0x2, CM_CAM_CLKSTCTRL);
	__raw_writel(0x2, CM_L3INIT_CLKSTCTRL);
	__raw_writel(0x2, CM_GMAC_CLKSTCTRL);
	__raw_writel(0x2, CM_EMIF_CLKSTCTRL);
	__raw_writel(0x2, CM_L4CFG_CLKSTCTRL);
	__raw_writel(0x2, CM_DMA_CLKSTCTRL);
	__raw_writel(0x2, CM_COREAON_CLKSTCTRL);
	__raw_writel(0x2, CM_DSS_CLKSTCTRL);

	/* enable power domain transitions (sw_wkup mode) */
	__raw_writel(0x2, CM_IPU1_CLKSTCTRL);
	__raw_writel(0x2, CM_IPU_CLKSTCTRL);
	__raw_writel(0x2, CM_VPE_CLKSTCTRL);

	/* Enable IPU module peripherals */
	reg = __raw_readl(CM_CAM_VIP1_CLKCTRL);
	__raw_writel((reg & ~0x00000003)|0x1, CM_CAM_VIP1_CLKCTRL);
	reg = __raw_readl(CM_CAM_VIP2_CLKCTRL);
	__raw_writel((reg & ~0x00000003)|0x1, CM_CAM_VIP2_CLKCTRL);
	reg = __raw_readl(CM_CAM_VIP3_CLKCTRL);
	__raw_writel((reg & ~0x00000003)|0x1, CM_CAM_VIP3_CLKCTRL);
	reg = __raw_readl(CM_L3INSTR_L3_MAIN_2_CLKCTRL);
	__raw_writel((reg & ~0x00000003)|0x1, CM_L3INSTR_L3_MAIN_2_CLKCTRL);
	reg = __raw_readl(CM_L4PER_I2C1_CLKCTRL);
	__raw_writel((reg & ~0x00000003)|0x2, CM_L4PER_I2C1_CLKCTRL);
	reg = __raw_readl(CM_L4PER_I2C2_CLKCTRL);
	__raw_writel((reg & ~0x00000003)|0x2, CM_L4PER_I2C2_CLKCTRL);
	reg = __raw_readl(CM_VPE_VPE_CLKCTRL);
	__raw_writel((reg & ~0x00000003)|0x1, CM_VPE_VPE_CLKCTRL);

	/* enable DSS */
	reg = __raw_readl(CTRL_CORE_CONTROL_IO_2);
	__raw_writel((reg | 0x1), CTRL_CORE_CONTROL_IO_2);
	reg = __raw_readl(CM_DSS_DSS_CLKCTRL);
	__raw_writel(((reg & ~0x00000003) | 0x00003F00 | 0x2), CM_DSS_DSS_CLKCTRL);

	/* checking if DSS is enabled */
	while ((__raw_readl(CM_DSS_DSS_CLKCTRL) & 0x00030000) != 0);

	/* enable IPU1 clocks / hw_auto mode */
	reg = __raw_readl(CM_IPU1_IPU1_CLKCTRL);
	__raw_writel(((reg & ~0x00000003) | 0x01000000 | 0x1), CM_IPU1_IPU1_CLKCTRL);

	/* checking if IPU1 module is enabled */
	while ((__raw_readl(CM_IPU1_IPU1_CLKCTRL) & 0x00030000) == 0x00030000);

	/* checking if clocks are enabled */
	while (((__raw_readl(CM_IPU1_CLKSTCTRL) & 0x100) >> 8) != 1);
}

void ipu_systemReset(void)
{
	/*Assert the IPU1 - CPU0,CPU1 & MMU,cache*/
	__raw_writel(0x7, RM_IPU1_RSTCTRL);

	/* Bring the IPU Unicache/MMU out of reset*/
	__raw_writel(0x7, RM_IPU1_RSTST);
	__raw_writel(0x3, RM_IPU1_RSTCTRL);

	while((__raw_readl(RM_IPU1_RSTST) & 0x4) != 0x4);
}

void setup_ipu_mmu(void)
{
	/* Large Page Translations - 512MB page mappings */
	/* L1: non cacheable, non posted L2: non cacheable, non posted */
	IPUMMU_MAP_LARGE_PAGE (0, 0x40000000, 0x40000000, 0x00000007);
	/* L1: cacheable, posted L2: non cacheable, non posted */
	IPUMMU_MAP_LARGE_PAGE (1, 0x80000000, 0x80000000, 0x000B0007);
	/* L1: non cacheable, posted L2: non cacheable, non posted */
	IPUMMU_MAP_LARGE_PAGE (2, 0xA0000000, 0xA0000000, 0x00020007);
	/*  L1: non cacheable, non posted L2: non cacheable, non posted */
	IPUMMU_MAP_LARGE_PAGE (3, 0x60000000, 0x40000000, 0x00000007);

	/*Medium Page - 256K page mappings*/
	/*  L1: non cacheable, non posted L2: non cacheable, non posted */
	IPUMMU_MAP_MEDIUM_PAGE(0,0x80300000, 0x40300000, 0x00000007);
	/* L1: non cacheable, posted L2: non cacheable, non posted */
	IPUMMU_MAP_MEDIUM_PAGE(1,0x00400000, 0x40400000, 0x00020007);

	/*Small Page - 16K page mappings */
	/* Volatile follow L1: cacheable, non posted L2: non cacheable, non posted */
	IPUMMU_MAP_SMALL_PAGE(0, 0x00000000, 0x55020000, 0x0001000B);
	/* Volatile follow L1: non cacheable, non posted L2: non cacheable, non posted */
	IPUMMU_MAP_SMALL_PAGE(1, 0x40000000, 0x55080000, 0x0000000B);
	/* L1: cacheable, non posted L2: non cacheable, non posted */
	IPUMMU_MAP_SMALL_PAGE(2, 0x00004000, 0x55024000, 0x00010007);
	/*  L1: non cacheable, non posted L2: non cacheable, non posted */
	IPUMMU_MAP_SMALL_PAGE(3, 0x00008000, 0x55028000, 0x00000007);
	/*  L1: non cacheable, non posted L2: non cacheable, non posted */
	IPUMMU_MAP_SMALL_PAGE(4, 0x20000000, 0x55020000, 0x00000007);

}


struct ipu_image_data {
	struct fastboot_ptentry *pte;
	int ipu_image_sz;
	u32 ipu_image_load_addr;
};

static struct ipu_image_data *ipu_data;

extern int valid_elf_image(unsigned long addr);
extern unsigned long load_elf_image_phdr(unsigned long addr);

u32 spl_boot_ipu(void){
	if (valid_elf_image(IPU_LOAD_ADDR)) {
		load_elf_image_phdr(IPU_LOAD_ADDR);
		reset_ipu();
		return 0;
	} else {
		printf("Not a valid elf image at 0x%x\n", IPU_LOAD_ADDR);
	}
	return 1;
}

/**
 * find_ipu_image(void) - Find the ipu image.
 *
 **/
int find_ipu_image(void)
{
	struct fastboot_ptentry *pte;
	ipu_data = (void *) malloc(sizeof(struct ipu_image_data));
	if (ipu_data == NULL) {
		printf("unable to allocate memory requested: ipu_data\n");
		return -1;
	}

	pte = fastboot_flash_find_ptn("ipu");
	if (pte) {
		ipu_data->pte = pte;
		ipu_data->ipu_image_sz = pte->length;
		ipu_data->ipu_image_load_addr = IPU_LOAD_ADDR;
		return 1;
	}
	return -1;
}

/**
 * load_ipu_image(atag_load_addr) - Load the ipu image if found.
 *
 * If an ipu image is found within a partition then it is loaded into
 * the ipu image load address.
 *
 * The load address depends on the partition it was found in:
 * Default is #define IPU_LOAD_ADDR for ipu image partition
 * where the ipu image is just the compiled binary or undefined.
 * Otherwise the address is read from the associated header.
 *
 * Returns the load addres in memory of the ipu image.
 **/
u32 load_ipu_image(void)
{
	int ret = 0;
	int sector;
	int num_sectors;
	int sector_sz = 0;
	u32 ipu_load_addr = 0;
	struct mmc* mmc = NULL;
	int status;

	ret = find_ipu_image();
	if (ret < 0) {
		printf("%s: IPU image loading not supported\n", __func__);
		goto out;
	}
	mmc = find_mmc_device(1);
	if (mmc == NULL) {
		goto out;
	}

	status = mmc_init(mmc);
	if (status != 0) {
		printf("mmc init failed\n");
		goto out;
	}

	sector_sz = mmc->block_dev.blksz;
	sector = ipu_data->pte->start;

	num_sectors = (ipu_data->ipu_image_sz/sector_sz);
	if (num_sectors <= (ipu_data->ipu_image_sz / sector_sz))
		num_sectors = (ipu_data->ipu_image_sz / sector_sz);


	status = mmc->block_dev.block_read(1, sector, num_sectors,
			(void *)ipu_data->ipu_image_load_addr);
	if (status < 0) {
		printf("mmc read failed\n");
		goto out;
	}

	printf("ipu_image @ %08x (%d)\n",
		ipu_data->ipu_image_load_addr,
		ipu_data->ipu_image_sz);

out:
	if (ipu_data && ipu_data->ipu_image_load_addr) {
		ipu_load_addr = ipu_data->ipu_image_load_addr;
	}

	free(ipu_data);
	return ipu_load_addr;

}

#endif

/*
 * Adjust I/O delays on the Tx control and data lines of each MAC port. This
 * is a workaround in order to work properly with the DP83865 PHYs on the EVM.
 * In 3COM RGMII mode this PHY applies it's own internal clock delay, so we
 * essentially need to counteract the DRA7xx internal delay, and we do this
 * by delaying the control and data lines. If not using this PHY, you probably
 * don't need to do this stuff!
 */
static void dra7xx_adj_io_delay(const struct io_delay *io_dly)
{
	int i = 0;
	u32 reg_val;
	u32 delta;
	u32 coarse;
	u32 fine;

	writel(CFG_IO_DELAY_UNLOCK_KEY, CFG_IO_DELAY_LOCK);

	while(io_dly[i].addr) {
		writel(CFG_IO_DELAY_ACCESS_PATTERN & ~CFG_IO_DELAY_LOCK_MASK,
		       io_dly[i].addr);
		delta = io_dly[i].dly;
		reg_val = readl(io_dly[i].addr) & 0x3ff;
		coarse = ((reg_val >> 5) & 0x1F) + ((delta >> 5) & 0x1F);
		coarse = (coarse > 0x1F) ? (0x1F) : (coarse);
		fine = (reg_val & 0x1F) + (delta & 0x1F);
		fine = (fine > 0x1F) ? (0x1F) : (fine);
		reg_val = CFG_IO_DELAY_ACCESS_PATTERN |
				CFG_IO_DELAY_LOCK_MASK |
				((coarse << 5) | (fine));
		writel(reg_val, io_dly[i].addr);
		i++;
	}

	writel(CFG_IO_DELAY_LOCK_KEY, CFG_IO_DELAY_LOCK);
}

/**
 * PMIC configuration to select 32KHz clock output
 *
 * @return 0
 */
#define I2C_PMIC_ADR			0x58
#define I2C_PMIC_CLK32AUDIO_CTRL	0xD5
#define I2C_PMIC_PAD2			0xFB
int pmic_set_32Khz_clock(void)
{
	u8 data;

	/* set i2c bus number */
	if (i2c_set_bus_num(0) != 0)
		return -1;

	/* probe i2c device */
	if (i2c_probe(I2C_PMIC_ADR) != 0)
		return -1;

	/* select CLK32KGA as GPIO_5 output */
	data = 0x2b;
	i2c_write(I2C_PMIC_ADR, I2C_PMIC_PAD2, 1, &data, 1);

	/* enable CLK32K */
	data = 0x01;
	i2c_write(I2C_PMIC_ADR, I2C_PMIC_CLK32AUDIO_CTRL, 1, &data, 1);

	return 0;
}

/**
 * @brief board_init
 *
 * @return 0
 */
int board_init(void)
{
	gpmc_init();
	gd->bd->bi_boot_params = (0x80000000 + 0x100); /* boot param addr */

	/* workaround to enable PMIC 32Khz output to wlink8 module */
	pmic_set_32Khz_clock();

#ifdef CONFIG_BOOTIPU1
	/* Enable IPU clocks */
	enable_ipu();
	ipu_systemReset();
	setup_ipu_mmu();
#endif
	return 0;
}

/**
 * @brief misc_init_r - Configure EVM board specific configurations
 * such as power configurations, ethernet initialization as phase2 of
 * boot sequence
 *
 * @return 0
 */
int misc_init_r(void)
{
	return 0;
}

static void do_set_mux32(u32 base,
			 struct pad_conf_entry const *array, int size)
{
	int i;
	struct pad_conf_entry *pad = (struct pad_conf_entry *)array;

	for (i = 0; i < size; i++, pad++)
		writel(pad->val, base + pad->offset);
}

void set_muxconf_regs_essential(void)
{
	do_set_mux32((*ctrl)->control_padconf_core_base,
		     core_padconf_array_essential,
		     sizeof(core_padconf_array_essential) /
		     sizeof(struct pad_conf_entry));
}

#if !defined(CONFIG_SPL_BUILD) && defined(CONFIG_GENERIC_MMC)
int board_mmc_init(bd_t *bis)
{
	omap_mmc_init(0, 0, 0, -1, -1);
	omap_mmc_init(1, 0, 0, -1, -1);
	return 0;
}
#endif

static void set_crossbar_mpu_irq(void)
{
	/* MPU_IRQ mapping to CROSSBAR_IRQ */
	writew(217, CTRL_CORE_MPU_IRQ_159_REG); /* RTC_IRQ */
	writew(351, CTRL_CORE_MPU_IRQ_152_REG); /* VIP_IRQ */
	writew(354, CTRL_CORE_MPU_IRQ_158_REG); /* VPE_IRQ */
	writew(150, CTRL_CORE_MPU_IRQ_155_REG); /* MCASP3_IRQ_AREVT */
	writew(151, CTRL_CORE_MPU_IRQ_154_REG); /* MCASP3_IRQ_AXEVT */
	writew(156, CTRL_CORE_MPU_IRQ_156_REG); /* MCASP6_IRQ_AREVT */
	writew(157, CTRL_CORE_MPU_IRQ_157_REG); /* MCASP6_IRQ_AXEVT */
	writew(158, CTRL_CORE_MPU_IRQ_146_REG); /* MCASP7_IRQ_AREVT */
	writew(159, CTRL_CORE_MPU_IRQ_147_REG); /* MCASP7_IRQ_AXEVT */
	writew(251, CTRL_CORE_MPU_IRQ_136_REG); /* MAILBOX5 */
	writew(255, CTRL_CORE_MPU_IRQ_141_REG); /* MAILBOX6 */
	writew(396, CTRL_CORE_MPU_IRQ_142_REG); /* IPU2 MMU */
	writew(145, CTRL_CORE_MPU_IRQ_143_REG); /* DSP1 MMU1 */
	writew(146, CTRL_CORE_MPU_IRQ_144_REG); /* DSP2 MMU0 */
	writew(147, CTRL_CORE_MPU_IRQ_145_REG); /* DSP2 MMU1 */
	writew(343, CTRL_CORE_MPU_IRQ_124_REG); /* QSPI */
	writew(334, CTRL_CORE_MPU_IRQ_50_REG);  /* CPSW_RX_THRESH */
	writew(335, CTRL_CORE_MPU_IRQ_51_REG);  /* CPSW_RX */
	writew(336, CTRL_CORE_MPU_IRQ_52_REG);  /* CPSW_TX */
	writew(337, CTRL_CORE_MPU_IRQ_53_REG);  /* CPSW_MISC */
	writew(222, CTRL_CORE_MPU_IRQ_81_REG);  /* DCAN1_INT0 */
	writew(225, CTRL_CORE_MPU_IRQ_82_REG);  /* DCAN2_INT0 */
}

static void set_crossbar_sdma_dreq(void)
{
	/* SDMA_DREQ mapping to CROSSBAR_IRQ */
	writew(132, CTRL_CORE_DMA_SYSTEM_DREQ_79_REG);  /* MCASP3_DREQ_RX */
	writew(133, CTRL_CORE_DMA_SYSTEM_DREQ_78_REG);  /* MCASP3_DREQ_TX */
	writew(138, CTRL_CORE_DMA_SYSTEM_DREQ_63_REG);  /* MCASP6_DREQ_RX */
	writew(139, CTRL_CORE_DMA_SYSTEM_DREQ_62_REG);  /* MCASP6_DREQ_TX */
	writew(140, CTRL_CORE_DMA_SYSTEM_DREQ_70_REG);  /* MCASP7_DREQ_RX */
	writew(141, CTRL_CORE_DMA_SYSTEM_DREQ_69_REG);  /* MCASP7_DREQ_TX */
}

void set_crossbar_regs(void)
{
	set_crossbar_mpu_irq();
	set_crossbar_sdma_dreq();
}

#ifdef CONFIG_DRIVER_TI_CPSW

/* Delay value to add to calibrated value */
#define RGMII0_TXCTL_DLY_VAL		((0x3 << 5) + 0x8)
#define RGMII0_TXD0_DLY_VAL		((0x3 << 5) + 0x8)
#define RGMII0_TXD1_DLY_VAL		((0x3 << 5) + 0x2)
#define RGMII0_TXD2_DLY_VAL		((0x4 << 5) + 0x0)
#define RGMII0_TXD3_DLY_VAL		((0x4 << 5) + 0x0)
#define VIN2A_D13_DLY_VAL		((0x3 << 5) + 0x8)
#define VIN2A_D17_DLY_VAL		((0x3 << 5) + 0x8)
#define VIN2A_D16_DLY_VAL		((0x3 << 5) + 0x2)
#define VIN2A_D15_DLY_VAL		((0x4 << 5) + 0x0)
#define VIN2A_D14_DLY_VAL		((0x4 << 5) + 0x0)

static void cpsw_control(int enabled)
{
	/* VTP can be added here */

	return;
}

static struct cpsw_slave_data cpsw_slaves[] = {
	{
		.slave_reg_ofs	= 0x208,
		.sliver_reg_ofs	= 0xd80,
		.phy_id		= 0,
	},
	{
		.slave_reg_ofs	= 0x308,
		.sliver_reg_ofs	= 0xdc0,
		.phy_id		= 1,
	},
};

static struct cpsw_platform_data cpsw_data = {
	.mdio_base		= CPSW_MDIO_BASE,
	.cpsw_base		= CPSW_BASE,
	.mdio_div		= 0xff,
	.channels		= 8,
	.cpdma_reg_ofs		= 0x800,
	.slaves			= 1,
	.slave_data		= cpsw_slaves,
	.ale_reg_ofs		= 0xd00,
	.ale_entries		= 1024,
	.host_port_reg_ofs	= 0x108,
	.hw_stats_reg_ofs	= 0x900,
	.bd_ram_ofs		= 0x2000,
	.mac_control		= (1 << 5),
	.control		= cpsw_control,
	.host_port_num		= 0,
	.version		= CPSW_CTRL_VERSION_2,
};

int board_eth_init(bd_t *bis)
{
	int ret;
	uint8_t mac_addr[6];
	uint32_t mac_hi, mac_lo;
	uint32_t ctrl_val;
	const struct io_delay io_dly[] = {
		{CFG_RGMII0_TXCTL, RGMII0_TXCTL_DLY_VAL},
		{CFG_RGMII0_TXD0, RGMII0_TXD0_DLY_VAL},
		{CFG_RGMII0_TXD1, RGMII0_TXD1_DLY_VAL},
		{CFG_RGMII0_TXD2, RGMII0_TXD2_DLY_VAL},
		{CFG_RGMII0_TXD3, RGMII0_TXD3_DLY_VAL},
		{CFG_VIN2A_D13, VIN2A_D13_DLY_VAL},
		{CFG_VIN2A_D17, VIN2A_D17_DLY_VAL},
		{CFG_VIN2A_D16, VIN2A_D16_DLY_VAL},
		{CFG_VIN2A_D15, VIN2A_D15_DLY_VAL},
		{CFG_VIN2A_D14, VIN2A_D14_DLY_VAL},
		{0}
	};

	/* Adjust IO delay for RGMII tx path */
	dra7xx_adj_io_delay(io_dly);

	/* try reading mac address from efuse */
	mac_lo = readl((*ctrl)->control_core_mac_id_0_lo);
	mac_hi = readl((*ctrl)->control_core_mac_id_0_hi);
	mac_addr[0] = mac_hi & 0xFF;
	mac_addr[1] = (mac_hi & 0xFF00) >> 8;
	mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
	mac_addr[3] = mac_lo & 0xFF;
	mac_addr[4] = (mac_lo & 0xFF00) >> 8;
	mac_addr[5] = (mac_lo & 0xFF0000) >> 16;

	if (!getenv("ethaddr")) {
		printf("<ethaddr> not set. Validating first E-fuse MAC\n");

		if (is_valid_ether_addr(mac_addr))
			eth_setenv_enetaddr("ethaddr", mac_addr);
	}
	ctrl_val = readl((*ctrl)->control_core_control_io1) & (~0x33);
	ctrl_val |= 0x22;
	writel(ctrl_val, (*ctrl)->control_core_control_io1);

	ret = cpsw_register(&cpsw_data);
	if (ret < 0)
		printf("Error %d registering CPSW switch\n", ret);

	return ret;
}
#endif

#define API_HAL_BASE_INDEX				0x0
#define API_HAL_KM_VERIFYCERTIFICATESIGNATURE_INDEX	API_HAL_BASE_INDEX + 0xE
#define CERT_U_BOOT_MAGIC				0x53544255
#define CERT_KERNEL_MAGIC				0x534C4E4B
#define CERT_RAMDSK_MAGIC				0x534B4452
#define CERT_DTB_MAGIC					0x53425444
#define CERT_RIGHTS_OTHER				0x20
#define CL_RSA_KEY_MAX_SIZE				256
#define GP_DEVICE					0x3

#ifdef CONFIG_HS_AUTH
struct ti_signed_cert {
    u8  signer_info[16];
    u32 signature_info;
    u32 key_id;
    u8  signature[CL_RSA_KEY_MAX_SIZE];
};


static u32 ti_rom_auth(void *cert, u32 len, void *sign, u32 rights)
{
	u32 params[5];

	params[0] = 4;
	params[1] = (u32)cert;
	params[2] = len;
	params[3] = (u32)sign;
	params[4] = rights;

       return hal_pub_to_sec_dispatcher(
		API_HAL_KM_VERIFYCERTIFICATESIGNATURE_INDEX, 0, 0, &params[0]);
}

u32 authenticate_image_signature(u32 start_addr, u32 size)
{
	u32 cert_size, result;
	struct ti_signed_cert *cert_ptr = NULL;
	u8 buf[5];
	u32 *tmp = (u32 *)&buf[0];

	if (get_device_type() != GP_DEVICE) {
		if ((u32)cert_ptr % 4 == 0) {
			cert_size = sizeof(struct ti_signed_cert);
			cert_ptr = (struct ti_signed_cert *)
					(start_addr + size - cert_size);
		} else {
			printf("Size 0x%x from image loaded @ 0x%x ",
					start_addr, size);
			puts("is not word aligned! Sign image correctly\n");
			goto exit_error;
		}

		/* Magic u32 to string */
		*tmp = cert_ptr->signature_info;
		buf[4]='\0';

		switch (cert_ptr->signature_info) {
		case CERT_U_BOOT_MAGIC:
		case CERT_KERNEL_MAGIC:
		case CERT_RAMDSK_MAGIC:
		case CERT_DTB_MAGIC:
			debug("Authenticating %s\n", buf);
			result = ti_rom_auth(
					(void *)start_addr, size - cert_size,
					(void *)cert_ptr, CERT_RIGHTS_OTHER);
			if (result != 0) {
				printf("%s authentication failed!\n", buf);
				goto exit_error;
			} else {
				debug("%s authentication passed\n", buf);
			}
			break;
		default:
			printf("Unknown Magic [%s], image not authenticated\n",
				buf);
			goto exit_error;
		}
	} else {
		debug("GP device does not support image authentication\n");
		result = 1;
	}
	return result;

exit_error:
#ifdef CONFIG_HS_ENFORCE_AUTH
	hang();
#else
	puts("WARNING - Not enforcing image authentication\n");
#endif
	return 1;
}
#endif
