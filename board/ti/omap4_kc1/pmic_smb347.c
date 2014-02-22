/*
 * (C) Copyright 2011
 * Eric
 *
 */
#include <common.h>
#include <asm/io.h>
#include <twl6030.h>

#include <kc1_board.h>

#include "kc1_panel.h"
#include "kc1_twl6030.h"
#include "pmic_smb347.h"
#include "fg_bq27541.h"

#define TEMPERATURE_COLD_LIMIT     -19
#define TEMPERATURE_HOT_LIMIT      59
#define LOW_BATTERY_CAPACITY_LIMIT 5
#define LOW_BATTERY_VOLTAGE_LIMIT  3450
#define LOW_LCD_VOLTAGE_LIMIT      3300
#define LOW_BAT_SCREEN_TICKS       25
#define NO_VBUS                    -2

//#define TEST_LOW_BATT 1

#define mdelay(n) ({ unsigned long msec = (n); while (msec--) udelay(1000); })

static int kc1_chargerdetect_setting[] = {
    R0_FCC_2500mA |
      R0_PCC_150mA |
      R0_TC_250mA,
    R1_DC_ICL_1800mA |
      R1_USBHC_ICL_1800mA,
    R2_SUS_CTRL_BY_REGISTER |
      R2_BAT_TO_SYS_NORMAL |
      R2_VFLT_PLUS_200mV |
      R2_AICL_ENABLE |
      R2_AIC_TH_4200mV |
      R2_USB_IN_FIRST |
      R2_BAT_OV_END_CHARGE,
    R3_PRE_CHG_VOLTAGE_THRESHOLD_3_0 |
      R3_FLOAT_VOLTAGE_4_2_0,
    R4_AUTOMATIC_RECHARGE_ENABLE |
      R4_CURRENT_TERMINATION_ENABLE |
      R4_BMD_VIA_THERM_IO |
      R4_AUTO_RECHARGE_100mV |
      R4_APSD_ENABLE |
      R4_NC_APSD_ENABLE |
      R4_SECONDARY_INPUT_NOT_ACCEPTED_IN_OVLO,
    R5_STAT_ACTIVE_LOW |
      R5_STAT_CHARGEING_STATE |
      R5_STAT_ENABLE |
      R5_NC_INPUT_HC_SETTING |
      R5_CC_TIMEOUT_764MIN |
      R5_PC_TIMEOUT_48MIN,
    R6_LED_BLINK_DISABLE |
      R6_EN_PIN_ACTIVE_LOW |
      R6_USB_HC_CONTROL_BY_PIN |
      R6_USB_HC_DUAL_STATE |
      R6_CHARGER_ERROR_NO_IRQ |
      R6_APSD_DONE_IRQ |
      R6_DCIN_INPUT_PRE_BIAS_ENABLE,
    0x80 |
      R7_MIN_SYS_3_4_5_V |
      R7_THERM_MONITOR_VDDCAP |
      R7_THERM_MONITOR_ENABLE |
      R7_SOFT_COLD_CC_FV_COMPENSATION |
      R7_SOFT_HOT_CC_FV_COMPENSATION,
    R8_INOK_OPERATION |
      R8_USB_2 |
      R8_VFLT_MINUS_240mV |
      R8_HARD_TEMP_CHARGE_SUSPEND |
      R8_PC_TO_FC_THRESHOLD_ENABLE |
      R8_INOK_ACTIVE_LOW,
    R9_RID_DISABLE_OTG_I2C_CONTROL |
      R9_OTG_PIN_ACTIVE_HIGH |
      R9_LOW_BAT_VOLTAGE_3_4_6_V,
    RA_CCC_700mA |
      RA_DTRT_130C |
      RA_OTG_CURRENT_LIMIT_500mA |
      RA_OTG_BAT_UVLO_THRES_2_7_V,
    0x61,
    RC_AICL_COMPLETE_TRIGGER_IRQ,
    RD_INOK_TRIGGER_IRQ |
      RD_LOW_BATTERY_TRIGGER_IRQ,
};

static int kc1_chargerdetect_setting_pvt[] = {
    R0_FCC_2500mA |
      R0_PCC_150mA |
      R0_TC_200mA,
    R1_DC_ICL_1800mA |
      R1_USBHC_ICL_1800mA,
    R2_SUS_CTRL_BY_REGISTER |
      R2_BAT_TO_SYS_NORMAL |
      R2_VFLT_PLUS_200mV |
      R2_AICL_ENABLE |
      R2_AIC_TH_4200mV |
      R2_USB_IN_FIRST |
      R2_BAT_OV_END_CHARGE,
    R3_PRE_CHG_VOLTAGE_THRESHOLD_3_0 |
      R3_FLOAT_VOLTAGE_4_2_0,
    R4_AUTOMATIC_RECHARGE_ENABLE |
      R4_CURRENT_TERMINATION_ENABLE |
      R4_BMD_VIA_THERM_IO |
      R4_AUTO_RECHARGE_100mV |
      R4_APSD_ENABLE |
      R4_NC_APSD_ENABLE |
      R4_SECONDARY_INPUT_NOT_ACCEPTED_IN_OVLO,
    R5_STAT_ACTIVE_LOW |
      R5_STAT_CHARGEING_STATE |
      R5_STAT_ENABLE |
      R5_NC_INPUT_HC_SETTING |
      R5_CC_TIMEOUT_764MIN |
      R5_PC_TIMEOUT_48MIN,
    R6_LED_BLINK_DISABLE |
      R6_EN_PIN_ACTIVE_LOW |
      R6_USB_HC_CONTROL_BY_PIN |
      R6_USB_HC_DUAL_STATE |
      R6_CHARGER_ERROR_NO_IRQ |
      R6_APSD_DONE_IRQ |
      R6_DCIN_INPUT_PRE_BIAS_ENABLE,
    0x80 |
      R7_MIN_SYS_3_4_5_V |
      R7_THERM_MONITOR_VDDCAP |
      R7_THERM_MONITOR_ENABLE |
      R7_SOFT_COLD_CC_FV_COMPENSATION |
      R7_SOFT_HOT_FV_COMPENSATION,
    R8_INOK_OPERATION |
      R8_USB_2 |
      R8_VFLT_MINUS_240mV |
      R8_HARD_TEMP_CHARGE_SUSPEND |
      R8_PC_TO_FC_THRESHOLD_ENABLE |
      R8_INOK_ACTIVE_LOW,
    R9_RID_DISABLE_OTG_I2C_CONTROL |
      R9_OTG_PIN_ACTIVE_HIGH |
      R9_LOW_BAT_VOLTAGE_3_4_6_V,
    RA_CCC_700mA |
      RA_DTRT_130C |
      RA_OTG_CURRENT_LIMIT_500mA |
      RA_OTG_BAT_UVLO_THRES_2_7_V,
    0xf5,
    RC_AICL_COMPLETE_TRIGGER_IRQ,
    RD_INOK_TRIGGER_IRQ |
      RD_LOW_BATTERY_TRIGGER_IRQ,
};

static int kc1_phydetect_setting[] = {
    R0_FCC_2500mA |
      R0_PCC_150mA |
      R0_TC_150mA,
    R1_DC_ICL_1800mA |
      R1_USBHC_ICL_1800mA,
    R2_SUS_CTRL_BY_REGISTER |
      R2_BAT_TO_SYS_NORMAL |
      R2_VFLT_PLUS_200mV |
      R2_AICL_ENABLE | // AICL_DISABLE
      R2_AIC_TH_4200mV |
      R2_USB_IN_FIRST |
      R2_BAT_OV_END_CHARGE,
    R3_PRE_CHG_VOLTAGE_THRESHOLD_3_0 |
      R3_FLOAT_VOLTAGE_4_2_0,
    //detect by omap
    R4_AUTOMATIC_RECHARGE_ENABLE |
      R4_CURRENT_TERMINATION_ENABLE |
      R4_BMD_VIA_THERM_IO |
      R4_AUTO_RECHARGE_100mV |
      R4_APSD_DISABLE |
      R4_NC_APSD_DISABLE |
      R4_SECONDARY_INPUT_NOT_ACCEPTED_IN_OVLO,
    R5_STAT_ACTIVE_LOW |
      R5_STAT_CHARGEING_STATE |
      R5_STAT_DISABLE |
      R5_NC_INPUT_HC_SETTING |
      R5_CC_TIMEOUT_DISABLED |
      R5_PC_TIMEOUT_DISABLED,
    R6_LED_BLINK_DISABLE |
      R6_CHARGE_EN_I2C_0 |
      R6_USB_HC_CONTROL_BY_REGISTER |
      R6_USB_HC_TRI_STATE |
      R6_CHARGER_ERROR_NO_IRQ |
      R6_APSD_DONE_IRQ |
      R6_DCIN_INPUT_PRE_BIAS_ENABLE,
    0x80 |
      R7_MIN_SYS_3_4_5_V |
      R7_THERM_MONITOR_VDDCAP |
      R7_SOFT_COLD_NO_RESPONSE |
      R7_SOFT_HOT_NO_RESPONSE,
    R8_INOK_OPERATION |
      R8_USB_2 |
      R8_VFLT_MINUS_60mV |
      R8_PC_TO_FC_THRESHOLD_ENABLE |
      R8_HARD_TEMP_CHARGE_SUSPEND |
      R8_INOK_ACTIVE_LOW,
    R9_RID_DISABLE_OTG_I2C_CONTROL |
      R9_OTG_PIN_ACTIVE_HIGH |
      R9_LOW_BAT_VOLTAGE_3_5_8_V,
    RA_CCC_700mA |
      RA_DTRT_130C |
      RA_OTG_CURRENT_LIMIT_500mA |
      RA_OTG_BAT_UVLO_THRES_2_7_V,
    0x61,
    RC_TEMP_OUTSIDE_COLD_HOT_HARD_LIMITS_TRIGGER_IRQ |
      RC_TEMP_OUTSIDE_COLD_HOT_SOFT_LIMITS_TRIGGER_IRQ |
      RC_USB_OVER_VOLTAGE_TRIGGER_IRQ |
      RC_USB_UNDER_VOLTAGE_TRIGGER_IRQ |
      RC_AICL_COMPLETE_TRIGGER_IRQ,
    RD_CHARGE_TIMEOUT_TRIGGER_IRQ |
      RD_TERMINATION_OR_TAPER_CHARGING_TRIGGER_IRQ |
      RD_FAST_CHARGING_TRIGGER_IRQ |
      RD_INOK_TRIGGER_IRQ |
      RD_MISSING_BATTERY_TRIGGER_IRQ |
      RD_LOW_BATTERY_TRIGGER_IRQ,
};


static int aicl_results[]={
    300,500,700,900,1200,1500,1800,2000,2200,2500,2500,2500,2500,2500,2500,2500
};


void summit_read_status_c(void) {
    u8 value = 0;
    u8 temp = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    smb347_i2c_read_u8(SUMMIT_SMB347_RC_STATUS, &value);

    temp = value;
    if (SRC_CHARGEING_CYCLE_STATUS(value))
        printf("At least one charge cycle\n");
    else
        printf("No charge cycle\n");

    temp = value;
    if (SRC_BATTERY_VOLTAGE_LEVEL_STATUS(value))
        printf("Bat < 2.1V\n");
    else
        printf("Bat > 2.1V\n");

    temp = value;
    if (SRC_HOLD_OFF_STATUS(value))
        printf("Not in hold-off status\n");
    else
        printf("in hold-off status\n");

    //temp = value;
    temp = SRC_CHARGEING_STATUS(value);
    if (temp == SRC_NO_CHARGING_STATUS)
        printf("NO_CHARGING\n");
    else if (temp == SRC_PRE_CHARGING_STATUS)
        printf("PRE_CHARGING\n");
    else if (temp == SRC_FAST_CHARGING_STATUS)
        printf("FAST_CHARGING\n");
    else if (temp == SRC_TAPER_CHARGING_STATUS)
        printf("TAPER_CHARGING\n");

    temp = value;
    if (SRC_CHARGEING_ENABLE_DISABLE_STATUS(temp))
        printf("Charger Enabled\n");
    else
        printf("Charger Disabled\n");
}


void summit_read_status_e(void) {
    u8 value = 0;
    u8 temp = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    smb347_i2c_read_u8(SUMMIT_SMB347_RE_STATUS, &value);

    temp = value;
    if (SRE_USB15_HC_MODE(temp) == SRE_USB1_OR_15_MODE)
        printf("USB1_MODE\n");

    temp = value;
    if (SRE_USB15_HC_MODE(temp) == SRE_USB5_OR_9_MODE)
        printf("USB5_MODE\n");
    
    temp = value;
    if (SRE_USB15_HC_MODE(temp) == SRE_HC_MODE)
        printf("HC_MODE\n");

    temp = value;
    if(SRE_AICL_STATUS(temp))
        printf("AICL Completed\n");
    else
        printf("AICL NOt Completed\n");
    temp = value;
    printf("AICL_RESULT=%d mA\n", aicl_results[SRE_AICL_RESULT(temp)]);
}


void summit_write_config(int enable) {
    u8 command = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    smb347_i2c_read_u8(SUMMIT_SMB347_RA_COMMAND, &command);

    if (enable)
        RA_COMMAND_ALLOW_WRITE_TO_CONFIG_REGISTER(command);
    else
        RA_COMMAND_NOT_ALLOW_WRITE_TO_CONFIG_REGISTER(command);

    smb347_i2c_write_u8(SUMMIT_SMB347_RA_COMMAND, command);
}


void summit_config_apsd(int enable) {
    u8 config = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    summit_write_config(1);
    smb347_i2c_read_u8(SUMMIT_SMB347_R4_CHARGE_CONTROL, &config);

    if (enable)
        R4_SET_APSD_ENABLE(config);
    else
        R4_SET_APSD_DISABLE(config);

    smb347_i2c_write_u8(SUMMIT_SMB347_R4_CHARGE_CONTROL, config);
    summit_write_config(0);
}    


void summit_config_aicl(int enable,int aicl_thres) {
    u8 config = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    summit_write_config(1);
    smb347_i2c_read_u8(SUMMIT_SMB347_R2_FUNCTIONS, &config);

    if (enable)
        R2_SET_AICL_ENABLE(config);
    else
        R2_SET_AICL_DISABLE(config);

    if (aicl_thres == 4200)
        R2_SET_AIC_TH_4200mV(config);
    else if (aicl_thres == 4500)
        R2_SET_AIC_TH_4500mV(config);

    smb347_i2c_write_u8(SUMMIT_SMB347_R2_FUNCTIONS, config);
    smb347_i2c_read_u8(SUMMIT_SMB347_R2_FUNCTIONS, &config);

    summit_write_config(0);
}


int summit_is_aicl_complete(void)
{
    u8 value, temp = 0;
    int i, vbus;

debug("[SMB347] %s:: ENTER\n", __func__);
    for (i=0; i<=20; i++) {

        smb347_i2c_read_u8(SUMMIT_SMB347_R2_FUNCTIONS, &value);
        if (R2_IS_AICL_DISABLE(value))
            return -1;

        smb347_i2c_read_u8(SUMMIT_SMB347_RE_STATUS, &value);
        temp = value;
        if (SRE_AICL_STATUS(temp)) {
            temp = value;
            printf("AICL Complete ,result=%d \n", aicl_results[SRE_AICL_RESULT(temp)]);
            return aicl_results[SRE_AICL_RESULT(value)];
        }

        vbus = twl6030_get_vbus_status();
        if (vbus == 0)
            return -2;

    }
    return -1;
}


void summit_switch_mode(int mode)
{
    u8 command = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    switch (mode) {

        case SRE_USB1_OR_15_MODE:
            RB_COMMAND_USB1(command);
            printf("USB1_MODE\n");
            break;

        case SRE_USB5_OR_9_MODE:
            RB_COMMAND_USB5(command);
            printf("USB5_MODE\n");
            break;

        case SRE_HC_MODE:
            RB_COMMAND_HC_MODE(command);
            printf("HC_MODE\n");
            break;

    }
    printf("command=%d\n", command);
    smb347_i2c_write_u8(SUMMIT_SMB347_RB_COMMAND, command);
}


void summit_charge_enable(int enable) {
    u8 command = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    smb347_i2c_read_u8(SUMMIT_SMB347_RA_COMMAND, &command);
    if (enable) {
        printf("enable\n");
        RA_COMMAND_CHARGING_ENABLE(command);
    }
    else {
        printf("disable\n");
        RA_COMMAND_CHARGING_DISABLE(command);
    }
    smb347_i2c_write_u8(SUMMIT_SMB347_RA_COMMAND, command);
}


int detect_usb()
{
    int usb2phy, value = 0;
    int i = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    usb2phy = __raw_readl(0x4a100620);
    if (usb2phy & 0x40000000) {         //ROM code disable detect charger function
        __raw_writel(0,0x4a100620);     //enable the detect charger fuction
        //printf("    Power: USB   Dedicated charger \n");
        //return 1;
    }

    //power up the phy    
    __raw_writel(~(1), 0x4A002300);
    while (1) {
        usb2phy = __raw_readl(0x4a100620);
        value   = usb2phy;
        value   &= 0xE00000;
        value   = value >> 21;
        i++;
        if (value!=0)
            break;               
        if (i >= 20000000 || i < 0)
            break;
    }

    printf("    Power: usb2phy=0x%x i=%d \n", usb2phy, i);
    usb2phy &= 0xE00000;
    usb2phy = usb2phy >> 21;
    
    if (usb2phy == 4) {
        printf("    PHY: USB   Dedicated charger \n");
        return SRE_USB1_OR_15_MODE;
    }
    if (usb2phy == 5) {
        printf("    PHY: USB   HOST charger\n");
        return SRE_USB1_OR_15_MODE;
    }
    if (usb2phy == 6) {
        printf("    PHY: USB   PC\n");
        return SRE_USB5_OR_9_MODE;
    }
    if (usb2phy == 1 || usb2phy == 3 || usb2phy == 7) {
        printf("No contact || Unknown error || interrupt\n");
        return SRE_NA_MODE;
    }
    return SRE_USB1_OR_15_MODE;
}


int summit_detect_usb(void)
{   
    u8 value   = 0;
    int i      = 0;
    int vbus   = 0;
    u8 command = 0;
    int mbid   = get_mbid();

debug("[SMB347] %s:: ENTER\n", __func__);
    // Check APSD enable
    smb347_i2c_read_u8(SUMMIT_SMB347_R4_CHARGE_CONTROL, &value);
    printf("    Charger: summit_detect_usb \n");
    if ((value & (R4_APSD_ENABLE | R4_NC_APSD_ENABLE)) == 0) {
        value = value | (R4_APSD_ENABLE | R4_NC_APSD_ENABLE);
        smb347_i2c_write_u8(SUMMIT_SMB347_R4_CHARGE_CONTROL, value);
    }

    for (i = 0; i <= 20; i++) {
        smb347_i2c_read_u8(SUMMIT_SMB347_RD_INTSTAT, &value);
        smb347_i2c_read_u8(SUMMIT_SMB347_RD_STATUS, &value);

        //printf("    Power: STATUS_D=0x%x\n",value);
        if (SRD_APSD_STATUS(value) == SRD_APSD_COMPLETED) {
            printf("    Charger: APSD Completed\n");
            value = SRD_APSD_RESULTS(value);
            break;
        }
        value = -1;

        vbus  = twl6030_get_vbus_status();
        if (vbus == 0) {
            return -2;
        }
    }

    if (value == -1) {
        printf("    Charger: APSD Not running \n");
    }
    if (value == SRE_USB5_OR_9_MODE) {
        printf("    Charger: USB   Dedicated charger \n");
        return 1;
    }
    if (value == SRE_USB1_OR_15_MODE) {
        printf("    Charger: USB   HOST charger\n");
        if (mbid < 4)  return SRE_USB5_OR_9_MODE;
        else           return SRE_USB1_OR_15_MODE;
    }
    if (value == SRE_NA_MODE) {
        printf("    Charger: Other Charging Port\n");
        summit_switch_mode(SRE_HC_MODE); 
        summit_write_config(1);
        smb347_i2c_read_u8(SUMMIT_SMB347_R6_ENABLE_CONTROL, &command);
        command &= ~R6_USB_HC_CONTROL_BY_PIN;
        smb347_i2c_write_u8(SUMMIT_SMB347_R6_ENABLE_CONTROL, command);
        summit_write_config(0);
        if (mbid < 4)  return SRE_USB5_OR_9_MODE;
        else           return SRE_USB1_OR_15_MODE;
    }
    if (value == 4) {
        printf("    Charger: USB   PC\n");
        return SRE_USB5_OR_9_MODE;
    }
    if (value == 6) {
        printf("    Charger: TBD\n");
        if (mbid < 4)  return SRE_USB5_OR_9_MODE;
        else           return SRE_NA_MODE;
    }

    return value;
}


void summit_read_setting(void)
{
    int index  = 0;
    u8  value  = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    for (index=0; index <= 0x0d; index++) {
        smb347_i2c_read_u8(index, &value);
        printf("index=0x%x \n value=0x%x\n",index,value);
        udelay(5000); 
    }
}

void summit_init(int mbid)
{
    int index  = 0;
    u8 value   = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    summit_write_config(1);

    for (index = 0; index <= 0x0d; index++) {
        smb347_i2c_read_u8(index, &value);
        udelay(1000); 

        if (mbid == 0) {
            if (value != kc1_phydetect_setting[index]) {
                smb347_i2c_write_u8(index, kc1_phydetect_setting[index]);
            }
        }
        else {
            if (mbid >= 5) {// For PVT
                if (value != kc1_chargerdetect_setting_pvt[index]) {
                    if (index != 0x05 && index != 0x3)
                        smb347_i2c_write_u8(index, kc1_chargerdetect_setting_pvt[index]);
	        }                  
            }
            else {
                if (value!=kc1_chargerdetect_setting[index]) {
                    if (index != 0x05 && index != 0x3)
                        smb347_i2c_write_u8(index, kc1_chargerdetect_setting[index]);
	        }
            }
        }
    }

    summit_write_config(0);
}


/*
It is a 0.2 sec loop.
work_index	second	function
0		0	voltage,capacity
1		0.2	power_button
2		0.4	vbus
3		0.6	power_button
4		0.8	temperature
5		1	power_button
6		1.2	vbus
7		1.4	power_button
8		1.6	current
9		1.8	power_button
*/
static int low_bat_charge(void)
{
    int result       = 0;
    int voltage      = 0;
    int current      = 0;
    int temperature  = 0;
    int vbus         = 0;
    int sec          = 0;
    int capacity     = 0;
    int show_low_bat = 0;
    int show_low_bat_ptw = 0;
    int power_button = 0;
    int work_index   = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    while (result == 0) {
        work_index = sec % 10;

        //printf("sec=%d work_index=%d\n",sec,work_index);

        switch (work_index) {

            case 0:
#ifndef TEST_LOW_BATT
                voltage  = get_bat_voltage();
#else
                voltage  = LOW_LCD_VOLTAGE_LIMIT + 1;
#endif
                capacity = get_bat_capacity();
                if ((voltage > LOW_BATTERY_VOLTAGE_LIMIT) && (capacity > LOW_BATTERY_CAPACITY_LIMIT)){
                    result = 1;
                    break;
                }
                if ((voltage > LOW_LCD_VOLTAGE_LIMIT) && (show_low_bat == 0)) {
                   show_low_bat = 1;
                   set_logoindex(1);
                   initialize_lcd();
                }
                printf("Battery voltage=%d capacity=%d \n",voltage,capacity);
                break;

            case 4:
                temperature = get_bat_temperature();
                //printf("Battery temperature=%d\n",temperature);
                if( (temperature>TEMPERATURE_HOT_LIMIT) || (temperature<TEMPERATURE_COLD_LIMIT)){
                     printf("shutdown due to temperature protect %d\n",temperature);
                     twl6030_shutdown();
                }
                break;

            case 8:
                current = get_bat_current();
                printf("Battery current=%d\n",current);
        	/*
                 * Shutdown if there is drain current, and we've displayed the LCD
                 * and shut it down after 10 seconds
                 */
                if ((current < 0) &&
                        ((show_low_bat < 1) || (show_low_bat > 30))) {
                    printf("shutdown due to there is discharge %d mA\n",current);
                    twl6030_shutdown();
                }
                break;

            case 2:
            case 6:
                vbus = twl6030_get_vbus_status();
                //printf("vbus=%d\n",vbus);
                if (vbus == 0) {
                    printf("shutdown due to there is no VBUS\n");
                    twl6030_shutdown();
                }
                break;

            default:
                power_button = twl6030_get_power_button_status();
                if ((power_button == 0 ) &&
                        (voltage > LOW_LCD_VOLTAGE_LIMIT) &&
                        (show_low_bat >= LOW_BAT_SCREEN_TICKS) &&
                        (show_low_bat_ptw == 0)) {
                    show_low_bat_ptw = 1;
                    set_logoindex(1);
                    initialize_lcd();
                }
                break;
        }

	// handle initial screen off
        if ((show_low_bat >= 1) && (show_low_bat < (LOW_BAT_SCREEN_TICKS-1))) { 
            show_low_bat += 1;
        }
        else if ((show_low_bat >= 1) && (show_low_bat < LOW_BAT_SCREEN_TICKS)) {
            show_low_bat += 1;
            turn_off_lcd();
        }

	// handle screen off from power button
        if ((power_button == 1) && (show_low_bat_ptw >= 1) && (show_low_bat_ptw < LOW_BAT_SCREEN_TICKS)) {
            show_low_bat_ptw += 1;
        } else if ((power_button == 1) && (show_low_bat_ptw >= LOW_BAT_SCREEN_TICKS)) {
            show_low_bat_ptw = 0;
            turn_off_lcd();
        }

        //software time protect
        if (sec > 9000) { //30 min
            result=0;
            printf("shutdown due to the charge time out\n");
            twl6030_shutdown();
            break;
        }

        //delay 0.2s
        sec++;
        mdelay(200);
    }

    //check vbus again
    vbus = twl6030_get_vbus_status();
    if ((vbus == 0)) {
        run_command("setgreenled ff", 0);
        run_command("setamberled 0", 0);
    }
    return result;
}


void check_low_bat(void)
{
    int power_source = 0;
    int input_limit  = 0;
    int voltage      = 0;
    int capacity     = 0;
    int temperature  = 0;
    int ms           = 0;
    int mbid         = 0;
    u8  value        = 0;

debug("[SMB347] %s:: ENTER\n", __func__);
    //turn off the phy
    __raw_writel(0x100, 0x4A0093E0);	  //disable ocp2scp_usb_phy_ick
    __raw_writel(0x30000, 0x4A0093E0);	  //disable ocp2scp_usb_phy_phy_48m
    __raw_writel(0x0, 0x4A008640);	  //disable usb_phy_cm_clk32k
    __raw_writel(1, 0x4A002300);          //power down the usb phy

    mbid = get_mbid();
    if (mbid != 0) {
        __raw_writel(0x40000000,0x4a100620);     //disable the detect charger fuction
    }

    temperature = get_bat_temperature();
#ifndef TEST_LOW_BATT
    voltage  = get_bat_voltage();
#else
    voltage  = LOW_LCD_VOLTAGE_LIMIT + 1;
#endif
    capacity = get_bat_capacity();

    printf("Battery capacity =%d voltage=%d temperature=%d\n",capacity,voltage,temperature);

    //Temperature protect 
    if( (temperature>TEMPERATURE_HOT_LIMIT) || (temperature<TEMPERATURE_COLD_LIMIT)) {
        printf("shutdown due to temperature protect %d\n",temperature);
        twl6030_shutdown();
    }

    if( (voltage <= LOW_BATTERY_VOLTAGE_LIMIT) || (capacity <= LOW_BATTERY_CAPACITY_LIMIT)) {

        if (twl6030_get_vbus_status()) {
            run_command("setgreenled 0", 0);
            run_command("setamberled 10", 0);

            //enable the VUSB 3.3 V
            twl6030_disable_vusb();
            twl6030_init_vusb();
            summit_write_config(1);

            //enable charge time out
            smb347_i2c_read_u8(SUMMIT_SMB347_R5_STAT_TIMERS, &value);

            //enable Pre-charge Timeout 48 min
            CLEAR_BIT(value,1);CLEAR_BIT(value,0);

            //enable complete charge timeout 1527 min
            CLEAR_BIT(value,2);SET_BIT(value,3);
            smb347_i2c_write_u8(SUMMIT_SMB347_R5_STAT_TIMERS, value);
            summit_write_config(0);

            //Detect the power source
            if (mbid == 0){	//EVT 1.0 DETECT_BY_PHY
                printf("DETECT_BY_PHY\n");
                power_source=detect_usb();
            } else {		//DETECT_BY_CHARGER
                printf("DETECT_BY_CHARGER\n");

                //check is this first boot?
                smb347_i2c_read_u8(SUMMIT_SMB347_R5_STAT_TIMERS, &value);
                if (IS_BIT_CLEAR(value,5)) {	//FIRST BOOT
                    printf("FIRST BOOT\n");
                    //disable stat
                    summit_write_config(1);
                    SET_BIT(value,5);
                    smb347_i2c_write_u8(SUMMIT_SMB347_R5_STAT_TIMERS, value);
                    summit_write_config(0);
                    //redo the apsd
                    summit_config_apsd(0);
                    summit_config_apsd(1);
                }

                power_source = summit_detect_usb();
                if (power_source == NO_VBUS)
                        goto LOW_BAT_TURN_OFF;
                if (power_source == SRE_USB5_OR_9_MODE) {
                    if (mbid == 0) {
                        //Need to fixed,this is bug of summit
                        summit_switch_mode(SRE_USB5_OR_9_MODE);
                        summit_init(mbid);
                        summit_charge_enable(1);
                    }
                    /* Enter low battery charge loop */
                    low_bat_charge();
                    if (mbid == 0)
                        summit_charge_enable(0);
                }
                else if (power_source == SRE_NA_MODE || power_source == SRE_USB1_OR_15_MODE) {
                    if (mbid == 0) {
                        summit_switch_mode(SRE_HC_MODE);
                        summit_init(mbid);
                        summit_charge_enable(1);
                    }
                    if (mbid >= 4) {  //For DVT
                        input_limit = summit_is_aicl_complete();
                        if (input_limit == NO_VBUS )
                            goto LOW_BAT_TURN_OFF;
                        printf("Power:AICL=%d mA\n",input_limit);
                        /* Enter low battery charge loop */
//                      if(input_limit>0 && input_limit<=900)
                        low_bat_charge();
                    }
                }
            }
        }
        else {
LOW_BAT_TURN_OFF:
            printf("shutdown due to there is weak battery \n");
            if (voltage > LOW_LCD_VOLTAGE_LIMIT) {
                set_logoindex(1);
                initialize_lcd();
                for (ms=0; ms<1000; ms++)
                    udelay(2000);//2ms
                turn_off_lcd();
            }
            twl6030_shutdown();
        }
    }
}

