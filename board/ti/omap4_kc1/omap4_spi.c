#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/io.h>
#include <asm/bitops.h>
#include <asm/arch/sys_proto.h>

#include <kc1_board.h>
#include "omap4_spi.h"

/*
 * clock parameter 0=48MHZ 1=24MHZ 2=12MHZ 3=6MHZ 4=3MHz
 * below 4 is not recommended to use. Because CS force high may not work,
 * if following function force CS low immediately.
 * for example, if the clock parameter is 7, I call
 * HW_WRITE_WORD function, then call HW_READ_WORD function, the
 * HW_READ_WORD function will fail, because between write and read
 * no CS pull hign then pull down
 */

#define CLOCK_PARAM	1

#define SPI_REG_WRITE(base, shift, reg)  w |= (base << shift) | reg; spi_otter_reg_write(w); w &= 0x0000;

void spi_otter_reg_write(unsigned int value);

/* the SPI register structure for omap */
static volatile MCSPI_REGS *gpMCSPIRegs;


/*
 * wait_txs_rxs
 *
 * Description
 *     wait for transmit / receive available
 * Parameters:
 *     int flag (IN): TXS for transmit, RXS for receive
 * Return
 *     0 if successful
 */
static int wait_txs_rxs(int flag)
{
    gpMCSPIRegs = (MCSPI_REGS *)MCSPI_SPI4_IO_BASE;
    unsigned long u, i;

    for (i = 0; i < 500; i++) {
        if (flag == TXS)
            u = CSP_BITFEXT(gpMCSPIRegs->CHxSTAT, SPI_CHxSTAT_TXS);
        else
            u = CSP_BITFEXT(gpMCSPIRegs->CHxSTAT, SPI_CHxSTAT_RXS);
        if (u)
            return 0;
    }

    if (flag == TXS)
        printf("Wait TXS time-out\n");
    else
        printf("Wait RXS time-out\n");

    return i;
}


/*
 * SPI_SetMode
 *
 * Description:
 *     Set SPI mode (POL, POA)
 * Parameters:
 *     SPIMODE mode (IN): SPI_MODE0, SPI_MODE1, SPI_MODE2 and SPI_MODE3
 * Return
 *     (None)
 */
static void SPI_SetMode(SPIMODE mode)
{
    gpMCSPIRegs = (MCSPI_REGS *)MCSPI_SPI4_IO_BASE;

    switch (mode) {

        case SPI_MODE0:
            CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_POL, 0);
            CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_PHA, 0);
            break;

        case SPI_MODE1:
            CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_POL, 0);
            CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_PHA, 1);
            break;

        case SPI_MODE2:
            CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_POL, 1);
            CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_PHA, 0);
            break;

        case SPI_MODE3:
            CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_POL, 1);
            CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_PHA, 1);
            break;

        default:
            break;

    }
}


/*
 * SPI_SetMode
 *
 * Description:
 *     Get SPI mode.
 * Parameters:
 *     None
 * Return
 *     SPIMODE
 *
 */
static SPIMODE SPI_GetMode(void)
{
    gpMCSPIRegs = (MCSPI_REGS *)MCSPI_SPI4_IO_BASE;
    unsigned long uPOL;
    unsigned long uPHA;

    uPOL = CSP_BITFEXT(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_POL);
    uPHA = CSP_BITFEXT(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_PHA);

    if (uPOL == 0 && uPHA == 0)
        return SPI_MODE0;
    else if (uPOL == 0 && uPHA == 1)
        return SPI_MODE1;
    else if (uPOL == 1 && uPHA == 0)
        return SPI_MODE2;
    else if (uPOL == 1 && uPHA == 1)
        return SPI_MODE3;
    else {
        printf("Error to get mode\n");
        return SPI_MODE0;
    }
}


/*
 * SPI_SetClockDivider
 *
 * Description
 *     Set SPI clock devider
 * Parameters
 *     unsigned long uDevider (IN)
 * Return
 *     None
 *
 */
static void SPI_SetClockDivider(unsigned long uDivider)
{
    gpMCSPIRegs = (MCSPI_REGS *)MCSPI_SPI4_IO_BASE;
    if (uDivider > 0x0C)
        return;
    CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_CLKD, uDivider);
}


/*
 * SPI_GetClockDivider
 *
 * Description:
 *     Set SPI clock divider
 * Parameters:
 *     None
 * Return
 *     SPI clock divider
 */
static unsigned long SPI_GetClockDivider(void)
{
    gpMCSPIRegs = (MCSPI_REGS *)MCSPI_SPI4_IO_BASE;
    return CSP_BITFEXT(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_CLKD);
}


/*
 * spi_init_spi4
 *
 * Description
 *     OMAP SPI4 initialization
 * Parameters
 *     None
 * Return
 *     None
 *
 */
static void spi_init_spi4(void)
{
    /* set SPI4 base address */
    gpMCSPIRegs = (MCSPI_REGS *)MCSPI_SPI4_IO_BASE;

    /* Overwrite pin muxing for mcspi4 */
    sr32((void *)CONTROL_PADCONF_MCSPI4_CLK, 0, 32, 0x01100110);
    sr32((void *)CONTROL_PADCONF_MCSPI4_SOMI, 0, 32, 0x01100110);

    udelay(5000);
}


void spi_command(void)
{
    unsigned int w = 0;
    unsigned int vendor0 = 1, vendor1 = 1;
    unsigned int mbid = get_mbid();

    //gpi_175
    vendor1 = (__raw_readl(0x4805D138) & 0x8000) >> 15;

    //gpi_176
    vendor0 = (__raw_readl(0x4805D138) & 0x10000) >> 16;
    //printf("LCD vendor1 = %d, vendor0 = %d\n", vendor1, vendor0);

    //Before DVT
    if (mbid < 4) {
        if ((vendor1 == 0) && (vendor0 == 0)) {
            printf("Panel of LG\n");
            SPI_REG_WRITE(0x00, 10, 0x21)
            SPI_REG_WRITE(0x00, 10, 0xa5)
            SPI_REG_WRITE(0x01, 10, 0x30)
            SPI_REG_WRITE(0x02, 10, 0x40)
            SPI_REG_WRITE(0x0e, 10, 0x5f)
            SPI_REG_WRITE(0x0f, 10, 0xa4)
            SPI_REG_WRITE(0x0d, 10, 0x00)
            SPI_REG_WRITE(0x02, 10, 0x43)
            SPI_REG_WRITE(0x0a, 10, 0x28)
            SPI_REG_WRITE(0x10, 10, 0x41)
            SPI_REG_WRITE(0x00, 10, 0xad)
        }
        else if ((vendor1 == 1) && (vendor0 == 0)) {
            printf("Panel of HYDIS\n");
            SPI_REG_WRITE(0x00, 10, 0x29)
            SPI_REG_WRITE(0x00, 10, 0x25)
            SPI_REG_WRITE(0x01, 10, 0x30)
            SPI_REG_WRITE(0x02, 10, 0x40)
            SPI_REG_WRITE(0x0e, 10, 0x5f)
            SPI_REG_WRITE(0x0f, 10, 0xa4)
            SPI_REG_WRITE(0x0d, 10, 0x01)
            SPI_REG_WRITE(0x00, 10, 0x2d)
        }
        else if ((vendor1 == 0) && (vendor0 == 1)) {
            printf("Panel of CMO\n");
            SPI_REG_WRITE(0x00, 10, 0xad)
            SPI_REG_WRITE(0x01, 10, 0x10)
            SPI_REG_WRITE(0x02, 10, 0x40)
            SPI_REG_WRITE(0x03, 10, 0x04)
        }
        else {
            printf("undefined panel, use default LGD settings\n");
            SPI_REG_WRITE(0x00, 10, 0x21)
            SPI_REG_WRITE(0x00, 10, 0xa5)
            SPI_REG_WRITE(0x01, 10, 0x30)
            SPI_REG_WRITE(0x02, 10, 0x40)
            SPI_REG_WRITE(0x0e, 10, 0x5f)
            SPI_REG_WRITE(0x0f, 10, 0xa4)
            SPI_REG_WRITE(0x0d, 10, 0x00)
            SPI_REG_WRITE(0x02, 10, 0x43)
            SPI_REG_WRITE(0x0a, 10, 0x28)
            SPI_REG_WRITE(0x10, 10, 0x41)
            SPI_REG_WRITE(0x00, 10, 0xad)
        }
    }
    //After DVT
    else {
        if ((vendor1 == 0) && (vendor0 == 0)) {
            printf("Panel of LG\n");
            SPI_REG_WRITE(0x00, 10, 0x21)
            SPI_REG_WRITE(0x00, 10, 0xa5)
            SPI_REG_WRITE(0x01, 10, 0x30)
            SPI_REG_WRITE(0x02, 10, 0x40)
            SPI_REG_WRITE(0x0e, 10, 0x5f)
            SPI_REG_WRITE(0x0f, 10, 0xa4)
            SPI_REG_WRITE(0x0d, 10, 0x00)
            SPI_REG_WRITE(0x02, 10, 0x43)
            SPI_REG_WRITE(0x0a, 10, 0x28)
            SPI_REG_WRITE(0x10, 10, 0x41)
            SPI_REG_WRITE(0x00, 10, 0xad)
        } else if ((vendor1 == 0) && (vendor0 == 1)) {
            printf("Panel of HYDIS\n");
            SPI_REG_WRITE(0x00, 10, 0x29)
            SPI_REG_WRITE(0x00, 10, 0x25)
            SPI_REG_WRITE(0x01, 10, 0x30)
            SPI_REG_WRITE(0x02, 10, 0x40)
            SPI_REG_WRITE(0x0e, 10, 0x5f)
            SPI_REG_WRITE(0x0f, 10, 0xa4)
            SPI_REG_WRITE(0x0d, 10, 0x01)
            SPI_REG_WRITE(0x00, 10, 0x2d)
        } else {
            printf("undefined panel, use default LGD settings\n");
            SPI_REG_WRITE(0x00, 10, 0x21)
            SPI_REG_WRITE(0x00, 10, 0xa5)
            SPI_REG_WRITE(0x01, 10, 0x30)
            SPI_REG_WRITE(0x02, 10, 0x40)
            SPI_REG_WRITE(0x0e, 10, 0x5f)
            SPI_REG_WRITE(0x0f, 10, 0xa4)
            SPI_REG_WRITE(0x0d, 10, 0x00)
            SPI_REG_WRITE(0x02, 10, 0x43)
            SPI_REG_WRITE(0x0a, 10, 0x28)
            SPI_REG_WRITE(0x10, 10, 0x41)
            SPI_REG_WRITE(0x00, 10, 0xad)
        }
    }
}


/*
 * spi_init
 *
 * Description
 *     Initialize OMAP SPI interface
 * Parameters
 *     None
 * Return
 *     None
 */
void spi_init(void)
{
    unsigned long u, n;
    gpMCSPIRegs = (MCSPI_REGS *)MCSPI_SPI4_IO_BASE;

    /* initialize the multipad and interface clock */
    spi_init_spi4();

    /* soft reset */
    CSP_BITFINS(gpMCSPIRegs->SYSCONFIG, SPI_SYSCONFIG_SOFTRESET, 1);
    for (n = 0; n < 100; n++) {
        u = CSP_BITFEXT(gpMCSPIRegs->SYSSTATUS, SPI_SYSSTATUS_RESETDONE);
        if (u)
            break;
    }

    /* disable the channel */
    CSP_BITFINS(gpMCSPIRegs->CHxCTRL, SPI_CHxCTRL_EN, 0);

    /* disable all interrupts */
    gpMCSPIRegs->IRQENABLE = 0x0;

    /* clear all interrupt status bits */
    gpMCSPIRegs->IRQSTATUS = 0xFFFFFFFF;

    /* set DPE0 to 1 */
    CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_DPE0, 1);

    /* set DPE1 to 0 */
    CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_DPE1, 0);

    /* set IS to 0 */
    CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_IS, 0);

    /* CS hold low at active state */
    CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_EPOL, 1);

    /* set clock divider
    * Note: the clock divider must big than 4. otherwise the
    * CS force high will not work, if following function force
    * CS low immediately. for example, if the clock devider is 7, I call
    * HW_WRITE_WORD function, then call HW_READ_WORD function, the
    * HW_READ_WORD function will fail, because between write and read
    * no CS pull hign then pull down.
    */

    SPI_SetClockDivider(CLOCK_PARAM);

    /* set mode 0 */
    SPI_SetMode(SPI_MODE0);

    /*
    * set module to master
    * drive CS to high
    */
    CSP_BITFINS(gpMCSPIRegs->MODULECTRL, SPI_MODULCTRL_MS, 0);
    CSP_BITFINS(gpMCSPIRegs->MODULECTRL, SPI_MODULCTRL_SINGLE, 1);
    CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_FORCE, 0);
}


/*
 * spi_otter_reg_write
 *
 * Description
 *     Write value to ks8851 register
 *
 * Parameters
 *     unsigned short value (IN)
 *     value to write
 * Return
 *     None
 */
void spi_otter_reg_write(unsigned int value)
{
    gpMCSPIRegs = (MCSPI_REGS *)MCSPI_SPI4_IO_BASE;

    //debug("*** %s::reg_write, value=0x%4x\n", __func__, value);
    /*
    * use 32 bit length to do transmit
    * set transmit mode only
    * drive CS to low
    */
    CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_WL, 0xF);
    CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_TRM, 2);
    CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_FORCE, 1);

    /* enable transmit */
    CSP_BITFINS(gpMCSPIRegs->CHxCTRL, SPI_CHxCTRL_EN, 1);
    if (wait_txs_rxs(TXS) != 0)
        return;

    /*
    * Because the data clock out from register high byte
    * so we need to change the order here
    */
    gpMCSPIRegs->TXx = value;
    if (wait_txs_rxs(TXS) != 0)
        return;

    /*
    * disable channel
    * driver CS to high
    */
    CSP_BITFINS(gpMCSPIRegs->CHxCTRL, SPI_CHxCTRL_EN, 0);
    CSP_BITFINS(gpMCSPIRegs->CHxCONF, SPI_CHxCONF_FORCE, 0);
}

