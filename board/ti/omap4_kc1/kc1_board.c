#include <common.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>

#include <kc1_board.h>


/*****************************************
 * Routine: mbid_init
 * Description: Initializes reading of MBID values via GPIO
 *****************************************/
void mbid_init(void)
{
  // Make MBID GPIOs inputs.  Pads should already be configured by x-loader
  unsigned val;
  val = __raw_readl(OMAP44XX_GPIO6_BASE + OMAP_GPIO_OE) | ((1 << MBID0_SHIFT) | (1 << MBID1_SHIFT) | (1 << MBID2_SHIFT) | (1 << MBID3_SHIFT));
  __raw_writel(val, OMAP44XX_GPIO6_BASE + OMAP_GPIO_OE);
}

/*****************************************
 * Routine: get_mbid
 * Description: returns mbid from GPIOs and implied by silicon type and rev
 *****************************************/
unsigned get_mbid(void)
{
  unsigned val;
  unsigned mbid;

  val = __raw_readl(OMAP44XX_GPIO6_BASE + OMAP_GPIO_DATAIN);
  mbid = ((val & (1 << MBID0_SHIFT)) >> MBID0_SHIFT) |
         ((val & (1 << MBID1_SHIFT)) >> (MBID1_SHIFT - 1)) |
         ((val & (1 << MBID2_SHIFT)) >> (MBID2_SHIFT - 2)) |
         ((val & (1 << MBID3_SHIFT)) >> (MBID3_SHIFT - 3));

  return mbid;
}

/*****************************************
 * Routine: get_panelid
 * Description: returns panelid from GPIOs
 *****************************************/
unsigned get_panelid(void)
{
  unsigned val;
  unsigned panelid;

  val = __raw_readl(OMAP44XX_GPIO6_BASE + OMAP_GPIO_DATAIN);
  panelid = ((val & (1 << PANELID0_SHIFT)) >> PANELID0_SHIFT) |
            ((val & (1 << PANELID1_SHIFT)) >> (PANELID1_SHIFT - 1));

  return panelid;
}

/*****************************************
 * Routine: get_touchid
 * Description: returns touchid from GPIOs
 *****************************************/
unsigned get_touchid(void)
{
  unsigned val;
  unsigned touchid;

  val = __raw_readl(OMAP44XX_GPIO2_BASE + OMAP_GPIO_DATAIN);
  touchid = ((val & (1 << TOUCHID0_SHIFT)) >> TOUCHID0_SHIFT) |
            ((val & (1 << TOUCHID1_SHIFT)) >> (TOUCHID1_SHIFT - 1));

  return touchid;
}

