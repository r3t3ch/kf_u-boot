#ifndef __KC1_BOARD_H
#define __KC1_BOARD_H

#define MBID0_GPIO                  174
#define MBID1_GPIO                  173
#define MBID2_GPIO                  178
#define MBID3_GPIO                  177
#define PANELID0_GPIO               176
#define PANELID1_GPIO               175
#define TOUCHID0_GPIO               50
#define TOUCHID1_GPIO               51

#define GPIO6_START                 160
#define GPIO2_START                 32

#define MBID0_SHIFT                 (MBID0_GPIO - GPIO6_START)
#define MBID1_SHIFT                 (MBID1_GPIO - GPIO6_START)
#define MBID2_SHIFT                 (MBID2_GPIO - GPIO6_START)
#define MBID3_SHIFT                 (MBID3_GPIO - GPIO6_START)
#define PANELID0_SHIFT              (PANELID0_GPIO - GPIO6_START)
#define PANELID1_SHIFT              (PANELID1_GPIO - GPIO6_START)
#define TOUCHID0_SHIFT              (TOUCHID0_GPIO - GPIO2_START)
#define TOUCHID1_SHIFT              (TOUCHID1_GPIO - GPIO2_START)

void mbid_init(void);
unsigned get_mbid(void);
unsigned get_panelid(void);
unsigned get_touchid(void);

#endif /* __KC1_BOARD_H */
