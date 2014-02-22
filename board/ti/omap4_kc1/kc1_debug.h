#ifndef __KC1_DEBUG_H
#define __KC1_DEBUG_H

void dump_gpio(unsigned int base);
void dump_prcm(void);
void dump_dss(void);
void dump_musb(void);
void dump_control_general_core(void);
void dump_control_general_wkup(void);
void dump_control_padconf_core(void);
void dump_control_padconf_wkup(void);
void dump_mcspi(unsigned int base);
void dump_i2c_twl6030(void);
void dump_i2c_smb347(void);

#endif /* __KC1_BOARD_H */
