#include <config.h>
#include <common.h>
#include <asm/omap_common.h>
#include <asm/arch/sys_proto.h>

#include "kc1_debug.h"

#ifdef DEBUG
#define SPACER "                             "
#define RMEM(x, y) printf("#%2d [0x%8x]=0x%8x\n", y, x, __raw_readl(x));
#define RMEMA(x) printf("[0x%8x]=0x%8x\n", x, __raw_readl(x));
#define RMEMB(y, x) printf("%-27s=0x%8x [0x%8x]\n", y,  __raw_readl(x), x);
#else
#define RMEM(x, y) {};
#define RMEMA(x) {};
#define RMEMB(y, x) {};
#endif

extern struct prcm_regs const **prcm;

void dump_gpio(void)
{
#ifdef DEBUG
RMEMA(0x4A100000);
RMEMA(0x4A100004);
RMEMA(0x4A100008);
RMEMA(0x4A10000C);
RMEMA(0x4A100010);
RMEMA(0x4A100014);
RMEMA(0x4A100018);
RMEMA(0x4A10001C);
RMEMA(0x4A100020);
RMEMA(0x4A100024);
RMEMA(0x4A100028);
RMEMA(0x4A10002C);
RMEMA(0x4A100030);
RMEMA(0x4A100034);
RMEMA(0x4A100038);
RMEMA(0x4A10003C);
RMEMA(0x4A100040);
RMEMA(0x4A100044);
RMEMA(0x4A100048);
RMEMA(0x4A10004C);
RMEMA(0x4A100050);
RMEMA(0x4A100054);
RMEMA(0x4A100058);
RMEMA(0x4A10005C);
RMEMA(0x4A100060);
RMEMA(0x4A100064);
RMEMA(0x4A100068);
RMEMA(0x4A10006C);
RMEMA(0x4A100070);
RMEMA(0x4A100074);
RMEMA(0x4A100078);
RMEMA(0x4A10007C);
RMEMA(0x4A100080);
RMEMA(0x4A100084);
RMEMA(0x4A100088);
RMEMA(0x4A10008C);
RMEMA(0x4A100090);
RMEMA(0x4A100094);
RMEMA(0x4A100098);
RMEMA(0x4A10009C);
RMEMA(0x4A1000A0);
RMEMA(0x4A1000A4);
RMEMA(0x4A1000A8);
RMEMA(0x4A1000AC);
RMEMA(0x4A1000B0);
RMEMA(0x4A1000B4);
RMEMA(0x4A1000B8);
RMEMA(0x4A1000BC);
RMEMA(0x4A1000C0);
RMEMA(0x4A1000C4);
RMEMA(0x4A1000C8);
RMEMA(0x4A1000CC);
RMEMA(0x4A1000D0);
RMEMA(0x4A1000D4);
RMEMA(0x4A1000D8);
RMEMA(0x4A1000DC);
RMEMA(0x4A1000E0);
RMEMA(0x4A1000E4);
RMEMA(0x4A1000E8);
RMEMA(0x4A1000EC);
RMEMA(0x4A1000F0);
RMEMA(0x4A1000F4);
RMEMA(0x4A1000F8);
RMEMA(0x4A1000FC);
RMEMA(0x4A100100);
RMEMA(0x4A100104);
RMEMA(0x4A100108);
RMEMA(0x4A10010C);
RMEMA(0x4A100110);
RMEMA(0x4A100114);
RMEMA(0x4A100118);
RMEMA(0x4A10011C);
RMEMA(0x4A100120);
RMEMA(0x4A100124);
RMEMA(0x4A100128);
RMEMA(0x4A10012C);
RMEMA(0x4A100130);
RMEMA(0x4A100134);
RMEMA(0x4A100138);
RMEMA(0x4A10013C);
RMEMA(0x4A100140);
RMEMA(0x4A100144);
RMEMA(0x4A100148);
RMEMA(0x4A10014C);
RMEMA(0x4A100150);
RMEMA(0x4A100154);
RMEMA(0x4A100158);
RMEMA(0x4A10015C);
RMEMA(0x4A100160);
RMEMA(0x4A100164);
RMEMA(0x4A100168);
RMEMA(0x4A10016C);
RMEMA(0x4A100170);
RMEMA(0x4A100174);
RMEMA(0x4A100178);
RMEMA(0x4A10017C);
RMEMA(0x4A100180);
RMEMA(0x4A100184);
RMEMA(0x4A100188);
RMEMA(0x4A10018C);
RMEMA(0x4A100190);
RMEMA(0x4A100194);
RMEMA(0x4A100198);
RMEMA(0x4A10019C);
RMEMA(0x4A1001A0);
RMEMA(0x4A1001A4);
RMEMA(0x4A1001A8);
RMEMA(0x4A1001AC);
RMEMA(0x4A1001B0);
RMEMA(0x4A1001B4);
RMEMA(0x4A1001B8);
RMEMA(0x4A1001BC);
RMEMA(0x4A1001C0);
RMEMA(0x4A1001C4);
RMEMA(0x4A1001C8);
RMEMA(0x4A1001CC);
RMEMA(0x4A1001D0);
RMEMA(0x4A1001D4);
RMEMA(0x4A1001D8);
RMEMA(0x4A1001DC);
RMEMA(0x4A1001E0);
RMEMA(0x4A1001E4);
RMEMA(0x4A1001E8);
RMEMA(0x4A1001EC);
RMEMA(0x4A1001F0);

RMEMA(0x48055134);
RMEMA(0x48057134);
RMEMA(0x48059134);
RMEMA(0x4805B134);
RMEMA(0x4805D134);

RMEMA(0x48055138);
RMEMA(0x48057138);
RMEMA(0x48059138);
RMEMA(0x4805B138);
RMEMA(0x4805D138);

RMEMA(0x4805513C);
RMEMA(0x4805713C);
RMEMA(0x4805913C);
RMEMA(0x4805B13C);
RMEMA(0x4805D13C);
#endif
}

void dump_prcm(void)
{
#ifdef DEBUG
RMEMB("CM_DUCATI_DUCATI_CLKCTRL", (*prcm)->cm_mpu_m3_mpu_m3_clkctrl);
RMEMB("CM_DUCATI_CLKSTCTRL", (*prcm)->cm_mpu_m3_clkstctrl);
RMEMB("IVAHD_IVAHD_CLKCTRL", (*prcm)->cm_ivahd_ivahd_clkctrl);
RMEMB("IVAHD_SL2_CLKCTRL", (*prcm)->cm_ivahd_sl2_clkctrl);
RMEMB("IVAHD_CLKSTCTRL", (*prcm)->cm_ivahd_clkstctrl);
RMEMB("DSP_DSP_CLKCTRL", (*prcm)->cm_dsp_dsp_clkctrl);
RMEMB("DSP_CLKSTCTRL", (*prcm)->cm_dsp_clkstctrl);
RMEMB("CM1_ABE_CLKSTCTRL", (*prcm)->cm1_abe_clkstctrl);
RMEMB("CM1_ABE_AESS_CLKCTRL", (*prcm)->cm1_abe_aess_clkctrl);

sr32((*prcm)->cm1_abe_pdm_clkctrl, 0, 32, 0x2);
sr32((*prcm)->cm1_abe_dmic_clkctrl, 0, 32, 0x2);
sr32((*prcm)->cm1_abe_mcasp_clkctrl, 0, 32, 0x2);
sr32((*prcm)->cm1_abe_mcbsp1_clkctrl, 0, 32, 0x08000002);
sr32((*prcm)->cm1_abe_mcbsp2_clkctrl, 0, 32, 0x08000002);
sr32((*prcm)->cm1_abe_mcbsp3_clkctrl, 0, 32, 0x08000002);

sr32((*prcm)->cm1_abe_slimbus_clkctrl, 0, 32, 0xf02);
sr32((*prcm)->cm1_abe_timer5_clkctrl, 0, 32, 0x2);
sr32((*prcm)->cm1_abe_timer6_clkctrl, 0, 32, 0x2);
sr32((*prcm)->cm1_abe_timer7_clkctrl, 0, 32, 0x2);
sr32((*prcm)->cm1_abe_timer8_clkctrl, 0, 32, 0x2);
sr32((*prcm)->cm1_abe_wdt3_clkctrl, 0, 32, 0x2);

RMEMB("CM_L4PER_CLKSTCTRL", (*prcm)->cm_l4per_clkstctrl);
RMEMB("CM_L4PER_DMTIMER10_CLKCTRL", (*prcm)->cm_l4per_gptimer10_clkctrl);
RMEMB("CM_L4PER_DMTIMER11_CLKCTRL", (*prcm)->cm_l4per_gptimer11_clkctrl);
RMEMB("CM_L4PER_DMTIMER2_CLKCTRL", (*prcm)->cm_l4per_gptimer2_clkctrl);
RMEMB("CM_L4PER_DMTIMER3_CLKCTRL", (*prcm)->cm_l4per_gptimer3_clkctrl);
RMEMB("CM_L4PER_DMTIMER4_CLKCTRL", (*prcm)->cm_l4per_gptimer4_clkctrl);
RMEMB("CM_L4PER_DMTIMER9_CLKCTRL", (*prcm)->cm_l4per_gptimer9_clkctrl);
RMEMB("CM_L4PER_GPIO2_CLKCTRL", (*prcm)->cm_l4per_gpio2_clkctrl);
RMEMB("CM_L4PER_GPIO3_CLKCTRL", (*prcm)->cm_l4per_gpio3_clkctrl);
RMEMB("CM_L4PER_GPIO4_CLKCTRL", (*prcm)->cm_l4per_gpio4_clkctrl);
RMEMB("CM_L4PER_GPIO5_CLKCTRL", (*prcm)->cm_l4per_gpio5_clkctrl);
RMEMB("CM_L4PER_GPIO6_CLKCTRL", (*prcm)->cm_l4per_gpio6_clkctrl);
RMEMB("CM_L4PER_HDQ1W_CLKCTRL", (*prcm)->cm_l4per_hdq1w_clkctrl);
RMEMB("CM_L4PER_I2C1_CLKCTRL", (*prcm)->cm_l4per_i2c1_clkctrl);
RMEMB("CM_L4PER_I2C2_CLKCTRL", (*prcm)->cm_l4per_i2c2_clkctrl);
RMEMB("CM_L4PER_I2C3_CLKCTRL", (*prcm)->cm_l4per_i2c3_clkctrl);
RMEMB("CM_L4PER_I2C4_CLKCTRL", (*prcm)->cm_l4per_i2c4_clkctrl);
RMEMB("CM_L4PER_MCBSP4_CLKCTRL", (*prcm)->cm_l4per_mcbsp4_clkctrl);
RMEMB("CM_L4PER_MCSPI1_CLKCTRL", (*prcm)->cm_l4per_mcspi1_clkctrl);
RMEMB("CM_L4PER_MCSPI2_CLKCTRL", (*prcm)->cm_l4per_mcspi2_clkctrl);
RMEMB("CM_L4PER_MCSPI3_CLKCTRL", (*prcm)->cm_l4per_mcspi3_clkctrl);
RMEMB("CM_L4PER_MCSPI4_CLKCTRL", (*prcm)->cm_l4per_mcspi4_clkctrl);
RMEMB("CM_L3INIT_HSMMC1_CLKCTRL", (*prcm)->cm_l3init_hsmmc1_clkctrl);
RMEMB("CM_L3INIT_HSMMC2_CLKCTRL", (*prcm)->cm_l3init_hsmmc2_clkctrl);
RMEMB("CM_L4PER_MMCSD3_CLKCTRL", (*prcm)->cm_l4per_mmcsd3_clkctrl);
RMEMB("CM_L4PER_MMCSD4_CLKCTRL", (*prcm)->cm_l4per_mmcsd4_clkctrl);
RMEMB("CM_L4PER_MMCSD5_CLKCTRL", (*prcm)->cm_l4per_mmcsd5_clkctrl);
RMEMB("CM_L4PER_UART1_CLKCTRL", (*prcm)->cm_l4per_uart1_clkctrl);
RMEMB("CM_L4PER_UART2_CLKCTRL", (*prcm)->cm_l4per_uart2_clkctrl);
RMEMB("CM_L4PER_UART3_CLKCTRL", (*prcm)->cm_l4per_uart3_clkctrl);
RMEMB("CM_L4PER_UART4_CLKCTRL", (*prcm)->cm_l4per_uart4_clkctrl);
RMEMB("CM_WKUP_GPIO1_CLKCTRL", (*prcm)->cm_wkup_gpio1_clkctrl);
RMEMB("CM_WKUP_TIMER1_CLKCTRL", (*prcm)->cm_wkup_gptimer1_clkctrl);
RMEMB("CM_WKUP_KEYBOARD_CLKCTRL", (*prcm)->cm_wkup_keyboard_clkctrl);
RMEMB("CM_SDMA_CLKSTCTRL", (*prcm)->cm_sdma_clkstctrl);
RMEMB("CM_MEMIF_CLKSTCTRL", (*prcm)->cm_memif_clkstctrl);
RMEMB("CM_MEMIF_EMIF_1_CLKCTRL", (*prcm)->cm_memif_emif_1_clkctrl);
RMEMB("CM_MEMIF_EMIF_2_CLKCTRL", (*prcm)->cm_memif_emif_2_clkctrl);
RMEMB("CM_D2D_CLKSTCTRL", (*prcm)->cm_c2c_clkstctrl);
RMEMB("CM_L3_2_GPMC_CLKCTRL", (*prcm)->cm_l3_gpmc_clkctrl);
RMEMB("CM_L3INSTR_L3_3_CLKCTRL", (*prcm)->cm_l3instr_l3_3_clkctrl);
RMEMB("CM_L3INSTR_L3_INSTR_CLKCTRL", (*prcm)->cm_l3instr_l3_instr_clkctrl);
//RMEMB("CM_L3INSTR_OCP_WP1_CLKCTRL", (*prcm)->cm_l3instr_intrconn_wp1_clkctrl);
RMEMB("CM_WKUP_WDT2_CLKCTRL", (*prcm)->cm_wkup_wdtimer2_clkctrl);
RMEMB("CM_CAM_CLKSTCTRL", (*prcm)->cm_cam_clkstctrl);
RMEMB("CM_CAM_ISS_CLKCTRL", (*prcm)->cm_cam_iss_clkctrl);
RMEMB("CM_CAM_FDIF_CLKCTRL", (*prcm)->cm_cam_fdif_clkctrl);
RMEMB("CM_DSS_CLKSTCTRL", (*prcm)->cm_dss_clkstctrl);
RMEMB("CM_DSS_DSS_CLKCTRL", (*prcm)->cm_dss_dss_clkctrl);
RMEMB("CM_SGX_CLKSTCTRL", (*prcm)->cm_sgx_clkstctrl);
RMEMB("CM_SGX_SGX_CLKCTRL", (*prcm)->cm_sgx_sgx_clkctrl);
RMEMB("CM_L3INIT_HSI_CLKCTRL", (*prcm)->cm_l3init_hsi_clkctrl);
RMEMB("CM_L3INIT_HSUSBHOST_CLKCTRL", (*prcm)->cm_l3init_hsusbhost_clkctrl);
RMEMB("CM_L3INIT_HSUSBOTG_CLKCTRL", (*prcm)->cm_l3init_hsusbotg_clkctrl);
RMEMB("CM_L3INIT_HSUSBTLL_CLKCTRL", (*prcm)->cm_l3init_hsusbtll_clkctrl);
RMEMB("CM_L3INIT_FSUSB_CLKCTRL", (*prcm)->cm_l3init_fsusb_clkctrl);
RMEMB("CM_L3INIT_USBPHY_CLKCTRL", (*prcm)->cm_l3init_usbphy_clkctrl);
#endif
}

void dump_dss(void)
{
#ifdef DEBUG
debug("*** DUMP DSS REGS:\n");
RMEMA(0x48040000);
RMEMA(0x48040010);
RMEMA(0x48040014);
RMEMA(0x48040040);
RMEMA(0x4804005c);
debug("*** DUMP DSS DISP REGS:\n");
RMEMA(0x48041000);
RMEMA(0x48041010);
RMEMA(0x48041014);
RMEMA(0x48041018);
RMEMA(0x4804101c);
RMEMA(0x48041040);
RMEMA(0x48041044);
RMEMA(0x48041048);
RMEMA(0x4804104c);
RMEMA(0x48041050);
RMEMA(0x48041054);
RMEMA(0x48041058);
RMEMA(0x4804105c);
RMEMA(0x48041060);
RMEMA(0x48041064);
RMEMA(0x48041068);
RMEMA(0x4804106c);
RMEMA(0x48041070);
RMEMA(0x48041074);
RMEMA(0x48041078);
RMEMA(0x4804107c);
RMEMA(0x48041080);
RMEMA(0x48041084);
RMEMA(0x48041088);
RMEMA(0x4804108c);
RMEMA(0x480410a0);
RMEMA(0x480410a4);
RMEMA(0x480410a8);
RMEMA(0x480410ac);
RMEMA(0x480410b0);
RMEMA(0x480410b4);
RMEMA(0x480411d4);
RMEMA(0x480411d8);
RMEMA(0x480411dc);
RMEMA(0x48041220);
RMEMA(0x48041224);
RMEMA(0x48041228);
RMEMA(0x4804122c);
RMEMA(0x48041238);
RMEMA(0x480413ac);
RMEMA(0x480413b0);
RMEMA(0x480413b4);
RMEMA(0x480413b8);
RMEMA(0x480413bc);
RMEMA(0x480413c0);
RMEMA(0x480413c4);
RMEMA(0x480413c8);
RMEMA(0x480413cc);
RMEMA(0x48041400);
RMEMA(0x48041404);
RMEMA(0x48041408);
RMEMA(0x4804140c);
debug("*** DUMP DSS VENC REGS:\n");
RMEMA(0x48043000);
#endif
}

