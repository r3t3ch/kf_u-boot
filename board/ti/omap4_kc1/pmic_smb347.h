#ifndef __PMIC_SMB347__H__
#define __PMIC_SMB347__H__

/* Bit operators */
#define IS_BIT_SET(reg, x)                  (((reg)&(0x1<<(x)))!=0)
#define IS_BIT_CLEAR(reg, x)                (((reg)&(0x1<<(x)))==0)
#define CLEAR_BIT(reg, x)                   ((reg)&=(~(0x1<<(x))))
#define SET_BIT(reg, x)                     ((reg)|=(0x1<<(x)))

/* I2C chip addresses */
#define SMB347_I2C_ADDRESS                  0x6

/*  REGISTERS */
#define SUMMIT_SMB347_R0_CHARGE_CURRENT	    0x0
#define SUMMIT_SMB347_R1_INPUT_CURR_LIMIT   0x1
#define SUMMIT_SMB347_R2_FUNCTIONS          0x2
#define SUMMIT_SMB347_R3_FLOAT_VOLTAGE      0x3
#define SUMMIT_SMB347_R4_CHARGE_CONTROL     0x4
#define SUMMIT_SMB347_R5_STAT_TIMERS        0x5
#define SUMMIT_SMB347_R6_ENABLE_CONTROL     0x6
#define SUMMIT_SMB347_R7_THERMAL_CONTROL    0x7
#define SUMMIT_SMB347_R8_SYSOK_USB30        0x8
#define SUMMIT_SMB347_R9_OTHER_CONTROL_A    0x9
#define SUMMIT_SMB347_RA_OTG_THERM_CONTROL  0xA
#define SUMMIT_SMB347_RB_CELL_TEMP          0xB
#define SUMMIT_SMB347_RC_FAULT_INTERRUPT    0xC
#define SUMMIT_SMB347_RD_INTERRUPT_STAT     0xD
#define SUMMIT_SMB347_RE_SLAVE_ADDR         0xE

/*  Command register*/
#define SUMMIT_SMB347_RA_COMMAND            0x30
#define SUMMIT_SMB347_RB_COMMAND            0x31
#define SUMMIT_SMB347_RC_COMMAND            0x33

#define SUMMIT_SMB347_RA_INTSTAT            0x35
#define SUMMIT_SMB347_RB_INTSTAT            0x36
#define SUMMIT_SMB347_RC_INTSTAT            0x37
#define SUMMIT_SMB347_RD_INTSTAT            0x38
#define SUMMIT_SMB347_RE_INTSTAT            0x39
#define SUMMIT_SMB347_RF_INTSTAT            0x3A

#define SUMMIT_SMB347_RA_STATUS             0x3B
#define SUMMIT_SMB347_RB_STATUS             0x3C
#define SUMMIT_SMB347_RC_STATUS             0x3D
#define SUMMIT_SMB347_RD_STATUS             0x3E
#define SUMMIT_SMB347_RE_STATUS             0x3F

/* R0[7:5]Fast Charge Current */
#define R0_FCC_700mA                        (0)
#define R0_FCC_900mA                        (1 << 5)
#define R0_FCC_1200mA                       (2 << 5)
#define R0_FCC_1500mA                       (3 << 5)
#define R0_FCC_1800mA                       (4 << 5)
#define R0_FCC_2000mA                       (5 << 5)
#define R0_FCC_2200mA                       (6 << 5)
#define R0_FCC_2500mA                       (7 << 5)
#define R0_FCC_CLEAN                        (7 << 5)
/* R0[4:3]Pre-charge Current */
#define R0_PCC_100mA                        (0)
#define R0_PCC_150mA                        (1 << 3)
#define R0_PCC_200mA                        (2 << 3)
#define R0_PCC_250mA                        (3 << 3)
#define R0_PCC_CLEAN                        (3 << 3)
/* R0[2:0]Termination Current */
#define R0_TC_37mA                          (0)
#define R0_TC_50mA                          (1)
#define R0_TC_100mA                         (2)
#define R0_TC_150mA                         (3)
#define R0_TC_200mA                         (4)
#define R0_TC_250mA                         (5)
#define R0_TC_500mA                         (6)
#define R0_TC_600mA                         (7)
#define R0_TC_CLEAN                         (7)
#define R0_CONFIG_NO_CHANGE                 (-1)

/* R1[7:4] Max. DCIN Input current limit */
#define R1_DC_ICL_300mA                     (0)
#define R1_DC_ICL_500mA                     (1 << 4)
#define R1_DC_ICL_700mA                     (2 << 4)
#define R1_DC_ICL_900mA                     (3 << 4)
#define R1_DC_ICL_1200mA                    (4 << 4)
#define R1_DC_ICL_1500mA                    (5 << 4)
#define R1_DC_ICL_1800mA                    (6 << 4)
#define R1_DC_ICL_2000mA                    (7 << 4)
#define R1_DC_ICL_2200mA                    (8 << 4)
#define R1_DC_ICL_2500mA                    (9 << 4)
#define R1_DC_ICL_CLEAN                     (15 <<4)
/* R1[3:0] Max. USBIN HC Input Current Limit */
#define R1_USBHC_ICL_500mA                  (1)
#define R1_USBHC_ICL_700mA                  (2)
#define R1_USBHC_ICL_900mA                  (3)
#define R1_USBHC_ICL_1200mA                 (4)
#define R1_USBHC_ICL_1500mA                 (5)
#define R1_USBHC_ICL_1800mA                 (6)
#define R1_USBHC_ICL_2000mA                 (7)
#define R1_USBHC_ICL_2200mA                 (8)
#define R1_USBHC_ICL_2500mA                 (9)
#define R1_USBHC_ICL_CLEAN                  (15)

/* R2[7]Input to system FET(suspend) on/off control, SUSP pin or register */
#define R2_SUS_CTRL_BY_SUS_PIN              (0)
#define R2_SUS_CTRL_BY_REGISTER             (1 << 7)
/* R2[6]Battery to system power control ,Normal or FET is turn off */
#define R2_BAT_TO_SYS_NORMAL                0
#define R2_FET_TURN_OFF                     (1 << 6)
/* R2[5]Maximum system voltage,Vflt+0.1V or Vflt+0.2V */
#define R2_VFLT_PLUS_100mV                  (0)
#define R2_VFLT_PLUS_200mV                  (1 << 5)
/* R2[4]Automatic Input Current LImit,*/
#define R2_AICL_DISABLE                     (0)
#define R2_AICL_ENABLE                      (1 << 4)
#define R2_SET_AICL_DISABLE(X)              CLEAR_BIT(X,4)
#define R2_SET_AICL_ENABLE(X)               SET_BIT(X,4)
#define R2_IS_AICL_ENABLE(X)                IS_BIT_SET(X,4)
#define R2_IS_AICL_DISABLE(X)               IS_BIT_CLEAR(X,4)
/* R2[3]Automatic Input Current Limit Threshold,4.25 or 4.5 */
#define R2_AIC_TH_4200mV                    (0)
#define R2_AIC_TH_4500mV                    (1 << 3)
#define R2_SET_AIC_TH_4200mV(X)             CLEAR_BIT(X,3)
#define R2_SET_AIC_TH_4500mV(X)             SET_BIT(X,3)
/* R2[2]Input Source Priority(When both DCIN and USBIN are valid) */
#define R2_DC_IN_FIRST                      (0)
#define R2_USB_IN_FIRST                     (1 << 2)
/* R2[1]Battery OV,*/
#define R2_BAT_OV_DOES_NOT_END_CHARGE       (0)
#define R2_BAT_OV_END_CHARGE                (1 << 1)
/* R2[0]VCHG*/
#define R2_VCHG_DISABLE                     (0)
#define R2_VCHG_ENABLE                      (1)

/* R3[7:5]Pre_charge to Fast-charge voltage threshold */
#define R3_PRE_CHG_VOLTAGE_THRESHOLD_2_4    (0)
#define R3_PRE_CHG_VOLTAGE_THRESHOLD_2_6    (1 << 6)
#define R3_PRE_CHG_VOLTAGE_THRESHOLD_2_8    (2 << 6)
#define R3_PRE_CHG_VOLTAGE_THRESHOLD_3_0    (3 << 6)
#define R3_PRE_CHG_VOLTAGE_CLEAN            (3 << 6)
/* R3[7:5]Float_charge_voltage */
#define R3_FLOAT_VOLTAGE_3_5_0              (0)
#define R3_FLOAT_VOLTAGE_3_5_2              (1)
#define R3_FLOAT_VOLTAGE_3_5_4              (2)
#define R3_FLOAT_VOLTAGE_4_0_0              (25)
#define R3_FLOAT_VOLTAGE_4_2_0              (35)
#define R3_FLOAT_VOLTAGE_CLEAN              (63)

/* R4[7]Automatic Recharge */
#define R4_AUTOMATIC_RECHARGE_ENABLE        (0)
#define R4_AUTOMATIC_RECHARGE_DISABLE       (1 << 7)
/* R4[6]Current Termination */
#define R4_CURRENT_TERMINATION_ENABLE       (0)
#define R4_CURRENT_TERMINATION_DISABLE      (1 << 6)
/* R4[5:4]Battery Missing Detection */
#define R4_BMD_DISABLE                      (0)
#define R4_BMD_EVERY_3S                     (1 << 4)
#define R4_BMD_ONCE                         (2 << 4)
#define R4_BMD_VIA_THERM_IO                 (3 << 4)
/* R4[3]Auto Recharge Threshold */
#define R4_AUTO_RECHARGE_50mV               (0)
#define R4_AUTO_RECHARGE_100mV              (1 << 3)
/* R4[2]Automatic Power Source Detection */
#define R4_APSD_DISABLE                     (0)
#define R4_APSD_ENABLE                      (1 << 2)
#define R4_SET_APSD_DISABLE(X)              CLEAR_BIT(X,2)
#define R4_SET_APSD_ENABLE(X)               SET_BIT(X,2)
#define R4_IS_APSD_ENABLE(X)                IS_BIT_SET(X,2)
/* R4[1]Non-conforming(NC) Charger Detection via APSD(1 second timeput) */
#define R4_NC_APSD_DISABLE                  (0)
#define R4_NC_APSD_ENABLE                   (1 << 1)
#define R4_SET_NC_APSD_DISABLE(X)           CLEAR_BIT(X,1)
#define R4_SET_NC_APSD_ENABLE(X)            SET_BIT(X,1)
#define R4_IS_NC_APSD_ENABLE(X)             IS_BIT_SET(X,1)
/* R4[0]Primary Input OVLO Select */
#define R4_SECONDARY_INPUT_NOT_ACCEPTED_IN_OVLO (0)
#define R4_SECONDARY_INPUT_ACCEPTED_IN_OVLO     (1)

/* R5[7]STAT and Timers Control */
#define R5_STAT_ACTIVE_LOW                  (0)
#define R5_STAT_ACTIVE_HIGH                 (1 << 7)
/* R5[6]STAT Output Mode */
#define R5_STAT_CHARGEING_STATE             (0)
#define R5_STAT_USB_FAIL                    (1 << 6)
/* R5[5]STAT Output Control */
#define R5_STAT_ENABLE                      (0)
#define R5_STAT_DISABLE                     (1 << 5)
/* R5[4]NC Charger Input Current Limit */
#define R5_NC_INPUT_100mA                   (0)
#define R5_NC_INPUT_HC_SETTING              (1 << 4)
/* R5[3:2]Complete Charge Timeout */
#define R5_CC_TIMEOUT_382MIN                (0)
#define R5_CC_TIMEOUT_764MIN                (1 << 2)
#define R5_CC_TIMEOUT_1527MIN               (2 << 2)
#define R5_CC_TIMEOUT_DISABLED              (3 << 2)
/* R5[1:0]Pre-Charge Timeout */
#define R5_PC_TIMEOUT_48MIN                 (0)
#define R5_PC_TIMEOUT_95MIN                 (1)
#define R5_PC_TIMEOUT_191MIN                (2)
#define R5_PC_TIMEOUT_DISABLED              (3)

/* R6[7]LED Blinking function */
#define R6_LED_BLINK_DISABLE                (0)
#define R6_LED_BLINK_ENABLE                 (1 << 7)
/* R6[6:5]Enable (EN) Pin Control */
#define R6_CHARGE_EN_I2C_0                  (0)
#define R6_CHARGE_EN_I2C_1                  (1 << 5)
#define R6_EN_PIN_ACTIVE_HIGH               (2 << 5)
#define R6_EN_PIN_ACTIVE_LOW                (3 << 5)
/* R6[4]USB/HC Control */
#define R6_USB_HC_CONTROL_BY_REGISTER       (0)
#define R6_USB_HC_CONTROL_BY_PIN            (1 << 4)
/* R6[3]USB/HC Input State */
#define R6_USB_HC_TRI_STATE                 (0)
#define R6_USB_HC_DUAL_STATE                (1 << 3)
/* R6[2]Charger Error */
#define R6_CHARGER_ERROR_NO_IRQ             (0)
#define R6_CHARGER_ERROR_IRQ                (1 << 2)
/* R6[1]APSD Done */
#define R6_APSD_DONE_NO_IRQ                 (0)
#define R6_APSD_DONE_IRQ                    (1 << 1)
/* R6[0]DCIN Input Pre-bias */
#define R6_DCIN_INPUT_PRE_BIAS_DISABLE      (0)
#define R6_DCIN_INPUT_PRE_BIAS_ENABLE       (1)

/* R7[6]Minimum System Voltage */
#define R7_MIN_SYS_3_4_5_V                  (0)
#define R7_MIN_SYS_3_6_V                    (1 << 6)
/* R7[5]THERM Monitor Selection */
#define R7_THERM_MONITOR_USBIN              (0)
#define R7_THERM_MONITOR_VDDCAP             (1 << 5)
/* R7[4]Thermistor Monitor */
#define R7_THERM_MONITOR_ENABLE             (0)
#define R7_THERM_MONITOR_DISABLE            (1 << 4)
/* R7[3:2]Soft Cold Temp Limit Behavior */
#define R7_SOFT_COLD_NO_RESPONSE            (0)
#define R7_SOFT_COLD_CC_COMPENSATION        (1 << 2)
#define R7_SOFT_COLD_FV_COMPENSATION        (2 << 2)
#define R7_SOFT_COLD_CC_FV_COMPENSATION     (3 << 2)
/* R7[1:0]Soft Hot Temp Limit Behavior */
#define R7_SOFT_HOT_NO_RESPONSE             (0)
#define R7_SOFT_HOT_CC_COMPENSATION         (1)
#define R7_SOFT_HOT_FV_COMPENSATION         (2)
#define R7_SOFT_HOT_CC_FV_COMPENSATION      (3)

/* R8[7:6]SYSOK/CHG_DET_N Output Operation */
#define R8_INOK_OPERATION                   (0)
#define R8_SYSOK_OPERATION_A                (1 << 6)
#define R8_SYSOK_OPERATION_B                (2 << 6)
#define R8_CHG_DET_N_OPERATION              (3 << 6)
/* R8[5]USB2.0/USB3.0 Input Current Limit */
#define R8_USB_2                            (0)
#define R8_USB_3                            (1 << 5)
/* R8[4:3]Float Voltage Compensation */
#define R8_VFLT_MINUS_60mV                  (0)
#define R8_VFLT_MINUS_120mV                 (1 << 3)
#define R8_VFLT_MINUS_180mV                 (2 << 3)
#define R8_VFLT_MINUS_240mV                 (3 << 3)
/* R8[2]Hard temp Limit Behavior */
#define R8_HARD_TEMP_CHARGE_SUSPEND         (0)
#define R8_HARD_TEMP_CHARGE_NO_SUSPEND      (1 << 2)
/* R8[1]Pre-charge to fast-charge Threshold */
#define R8_PC_TO_FC_THRESHOLD_ENABLE        (0)
#define R8_PC_TO_FC_THRESHOLD_DISABLE       (1 << 1)
/* R8[0]INOK Polarity */
#define R8_INOK_ACTIVE_LOW                  (0)
#define R8_INOK_ACTIVE_HIGH                 (1 << 0)

/* R9[7:6]OTG/ID Pin Control */
#define R9_RID_DISABLE_OTG_I2C_CONTROL      (0)
#define R9_RID_DISABLE_OTG_PIN_CONTROL      (1 << 6)
#define R9_RID_ENABLE_OTG_I2C_CONTROL       (2 << 6)
#define R9_RID_ENABLE_OTG_AUTO              (3 << 6)
/* R9[5]OTG Pin Polarity */
#define R9_OTG_PIN_ACTIVE_HIGH              (0)
#define R9_OTG_PIN_ACTIVE_LOW               (1 << 5)
/* R9[3:0]Low-Battery / SYSOK Voltage Threshold */
#define R9_LOW_BAT_VOLTAGE_DISABLE          (0)
#define R9_LOW_BAT_VOLTAGE_3_4_6_V          (14)
#define R9_LOW_BAT_VOLTAGE_3_5_8_V          (0xf)

/* RA[7:6]Charge Current Compensation */
#define RA_CCC_250mA                        (0)
#define RA_CCC_700mA                        (1 << 6)
#define RA_CCC_900mA                        (2 << 6)
#define RA_CCC_1200mA                       (3 << 6)
/* RA[5:4]Digital Thermal Regulation Temperature Threshold */
#define RA_DTRT_100C                        (0)
#define RA_DTRT_110C                        (1 << 4)
#define RA_DTRT_120C                        (2 << 4)
#define RA_DTRT_130C                        (3 << 4)
/* RA[3:2]OTG Current Limit at USBIN */
#define RA_OTG_CURRENT_LIMIT_100mA          (0)
#define RA_OTG_CURRENT_LIMIT_250mA          (1 << 2)
#define RA_OTG_CURRENT_LIMIT_500mA          (2 << 2)
#define RA_OTG_CURRENT_LIMIT_750mA          (3 << 2)
/* RA[1:0]OTG Battery UVLO Threshold */
#define RA_OTG_BAT_UVLO_THRES_2_7_V         (0)
#define RA_OTG_BAT_UVLO_THRES_2_9_V         (1)
#define RA_OTG_BAT_UVLO_THRES_3_1_V         (2)
#define RA_OTG_BAT_UVLO_THRES_3_3_V         (3)

/* RC FAULT Interrupt Register, Setting what condition assert an interrupt */
#define RC_TEMP_OUTSIDE_COLD_HOT_HARD_LIMITS_TRIGGER_IRQ  (1 << 7)
#define RC_TEMP_OUTSIDE_COLD_HOT_SOFT_LIMITS_TRIGGER_IRQ  (1 << 6)
#define RC_OTG_BAT_FAIL_ULVO_TRIGGER_IRQ                  (1 << 5)
#define RC_OTG_OVER_CURRENT_LIMIT_TRIGGER_IRQ             (1 << 4)
#define RC_USB_OVER_VOLTAGE_TRIGGER_IRQ                   (1 << 3)
#define RC_USB_UNDER_VOLTAGE_TRIGGER_IRQ                  (1 << 2)
#define RC_AICL_COMPLETE_TRIGGER_IRQ                      (1 << 1)
#define RC_INTERNAL_OVER_TEMP_TRIGGER_IRQ                 (1)

/* RD STATUS Interrupt Register, Setting what condition assert an interrupt */
#define RD_CHARGE_TIMEOUT_TRIGGER_IRQ                     (1 << 7)
#define RD_OTG_INSERTED_REMOVED_TRIGGER_IRQ               (1 << 6)
#define RD_BAT_OVER_VOLTAGE_TRIGGER_IRQ                   (1 << 5)
#define RD_TERMINATION_OR_TAPER_CHARGING_TRIGGER_IRQ      (1 << 4)
#define RD_FAST_CHARGING_TRIGGER_IRQ                      (1 << 3)
#define RD_INOK_TRIGGER_IRQ                               (1 << 2)
#define RD_MISSING_BATTERY_TRIGGER_IRQ                    (1 << 1)
#define RD_LOW_BATTERY_TRIGGER_IRQ                        (1)

/* Interrupt Status Register A */
#define ISRA_HOT_TEMP_HARD_LIMIT_IRQ                      (1 << 7)
#define ISRA_HOT_TEMP_HARD_LIMIT_STATUS                   (1 << 6)
#define ISRA_COLD_TEMP_HARD_LIMIT_IRQ                     (1 << 5)
#define ISRA_COLD_TEMP_HARD_LIMIT_STATUS                  (1 << 4)
#define ISRA_HOT_TEMP_SOFT_LIMIT_IRQ                      (1 << 3)
#define ISRA_HOT_TEMP_SOFT_LIMIT_STATUS                   (1 << 2)
#define ISRA_COLD_TEMP_SOFT_LIMIT_IRQ                     (1 << 1)
#define ISRA_COLD_TEMP_SOFT_LIMIT_STATUS                  (1)

/* Interrupt Status Register B */
#define ISRB_BAT_OVER_VOLTAGE_IRQ                         (1 << 7)
#define ISRB_BAT_OVER_VOLTAGE_STATUS                      (1 << 6)
#define ISRB_MISSING_BAT_IRQ                              (1 << 5)
#define ISRB_MISSING_BAT_STATUS                           (1 << 4)
#define ISRB_LOW_BAT_VOLTAGE_IRQ                          (1 << 3)
#define ISRB_LOW_BAT_VOLTAGE_STATUS                       (1 << 2)
#define ISRB_PRE_TO_FAST_CHARGE_VOLTAGE_IRQ               (1 << 1)
#define ISRB_PRE_TO_FAST_CHARGE_VOLTAGE_STATUS            (1)

/* Interrupt Status Register C */
#define ISRC_INTERNAL_TEMP_LIMIT_IRQ                      (1 << 7)
#define ISRC_INTERNAL_TEMP_LIMIT_STATUS                   (1 << 6)
#define ISRC_RE_CHARGE_BAT_THRESHOLD_IRQ                  (1 << 5)
#define ISRC_RE_CHARGE_BAT_THRESHOLD_STATUS               (1 << 4)
#define ISRC_TAPER_CHARGER_MODE_IRQ                       (1 << 3)
#define ISRC_TAPER_CHARGER_MODE_STATUS                    (1 << 2)
#define ISRC_TERMINATION_CC_IRQ                           (1 << 1)
#define ISRC_TERMINATION_CC_STATUS                        (1)

/*Interrupt Status Register D*/
#define ISRD_APSD_COMPLETED_IRQ                           (1 << 7)
#define ISRD_APSD_COMPLETED_STATUS                        (1 << 6)
#define ISRD_AICL_COMPLETED_IRQ                           (1 << 5)
#define ISRD_AICL_COMPLETED_STATUS                        (1 << 4)
#define ISRD_COMPLETE_CHARGE_TIMEOUT_IRQ                  (1 << 3)
#define ISRD_COMPLETE_CHARGE_TIMEOUT_STATUS               (1 << 2)
#define ISRD_PC_TIMEOUT_IRQ                               (1 << 1)
#define ISRD_PC_TIMEOUT_STATUS                            (1)

/* Interrupt Status Register E */
#define ISRE_DCIN_OVER_VOLTAGE_IRQ                        (1 << 7)
#define ISRE_DCIN_OVER_VOLTAGE_STATUS                     (1 << 6)
#define ISRE_DCIN_UNDER_VOLTAGE_IRQ                       (1 << 5)
#define ISRE_DCIN_UNDER_VOLTAGE_STATUS                    (1 << 4)
#define ISRE_USBIN_OVER_VOLTAGE_IRQ                       (1 << 3)
#define ISRE_USBIN_OVER_VOLTAGE_STATUS                    (1 << 2)
#define ISRE_USBIN_UNDER_VOLTAGE_IRQ                      (1 << 1)
#define ISRE_USBIN_UNDER_VOLTAGE_STATUS                   (1)

/* Interrupt Status Register F */
#define ISRF_OTG_OVER_CURRENT_IRQ                         (1 << 7)
#define ISRF_OTG_OVER_CURRENT_STATUS                      (1 << 6)
#define ISRF_OTG_BAT_UNDER_VOLTAGE_IRQ                    (1 << 5)
#define ISRF_OTG_BAT_UNDER_VOLTAGE_STATUS                 (1 << 4)
#define ISRF_OTG_DETECTION_IRQ                            (1 << 3)
#define ISRF_OTG_DETECTION_STATUS                         (1 << 2)
#define ISRF_POWER_OK_IRQ                                 (1 << 1)
#define ISRF_POWER_OK_STATUS                              (1)


/* COMMAND A */
#define RA_COMMAND_NOT_ALLOW_WRITE_TO_CONFIG_REGISTER(X)  CLEAR_BIT(X,7)
#define RA_COMMAND_ALLOW_WRITE_TO_CONFIG_REGISTER(X)      SET_BIT(X,7)
#define RA_COMMAND_FORCE_PRE_CHARGE_CURRENT_SETTING(X)    CLEAR_BIT(X,6)
#define RA_COMMAND_ALLOW_FAST_CHARGE_CURRENT_SETTING(X)   SET_BIT(X,6)
#define RA_COMMAND_OTG_DISABLE(X)                         CLEAR_BIT(X,4)
#define RA_COMMAND_OTG_ENABLE(X)                          SET_BIT(X,4)
#define RA_COMMAND_BAT_TO_SYS_NORMAL(X)                   CLEAR_BIT(X,3)
#define RA_COMMAND_TURN_OFF_BAT_TO_SYS(X)                 SET_BIT(X,3)
#define RA_COMMAND_SUSPEND_MODE_DISABLE(X)                CLEAR_BIT(X,2)
#define RA_COMMAND_SUSPEND_MODE_ENABLE(X)                 SET_BIT(X,2)
#define RA_COMMAND_CHARGING_DISABLE(X)                    CLEAR_BIT(X,1)
#define RA_COMMAND_CHARGING_ENABLE(X)                     SET_BIT(X,1)
#define RA_COMMAND_STAT_OUTPUT_ENABLE(X)                  CLEAR_BIT(X,0)
#define RA_COMMAND_STAT_OUTPUT_DISABLE(X)                 SET_BIT(X,0)        

/* COMMAND B */
#define RB_COMMAND_POWER_ON_RESET(X)                      SET_BIT(X,7)
#define RB_COMMAND_USB1(X)                                CLEAR_BIT(X,1)
#define RB_COMMAND_USB5(X)                                SET_BIT(X,1)
#define RB_COMMAND_USB_MODE(X)                            CLEAR_BIT(X,0)
#define RB_COMMAND_HC_MODE(X)                             SET_BIT(X,0)

#define ACTUAL_FLOAT_VOLTAGE(X)                           (X & 0x3F)


/* INTERRUPT_STATUS_B */
#define ISRB_BAT_OVER_V_IRQ_BIT(X)          ((X & SRB_BAT_OVER_VOLTAGE_IRQ) >> 7)
    #define ISRB_NO_BAT_OVER_V_IRQ              0
    #define ISRB_BAT_OVER_V_IRQ                 1
#define ISRB_BAT_OVER_V_STATUS_BIT(X)       ((X & SRB_BAT_OVER_VOLTAGE_STATUS) >> 6)
    #define ISRB_NO_BAT_OVER_V                  0
    #define ISRB_BAT_OVER_V                     1
#define ISRB_BAT_MISS_IRQ_BIT(X)            ((X & SRB_MISSING_BAT_IRQ) >> 5)
    #define ISRB_NO_BAT_MISS_IRQ                0
    #define ISRB_BAT_MISS_IRQ                   1
#define ISRB_BAT_MISS_STATUS_BIT(X)         ((X & SRB_MISSING_BAT_STATUS) >> 4)
    #define ISRB_NO_BAT_MISS                    0
    #define ISRB_BAT_MISS                       1
#define ISRB_LOW_BAT_IRQ_BIT(X)             ((X & SRB_LOW_BAT_VOLTAGE_IRQ) >> 3)
    #define ISRB_NO_LOW_BAT_IRQ                 0
    #define ISRB_LOW_BAT_IRQ                    1
#define ISRB_LOW_BAT_STATUS_BIT(X)          ((X & SRB_LOW_BAT_VOLTAGE_STATUS) >> 2)
    #define ISRB_NO_LOW_BAT                     0
    #define ISRB_LOW_BAT                        1
#define ISRB_PRE_TO_FAST_V_BIT(X)           ((X & SRB_PRE_TO_FAST_CHARGE_VOLTAGE_IRQ) >> 1)
    #define ISRB_NO_PRE_TO_FAST_V_IRQ           0
    #define ISRB_PRE_TO_FAST_V_IRQ              1
#define ISRB_PRE_TO_FAST_V_STATUS_BIT(X)    (X & SRB_PRE_TO_FAST_CHARGE_VOLTAGE_STATUS))
    #define ISRB_NO_PRE_TO_FAST_V               0
    #define ISRB_PRE_TO_FAST_V_                 1

/* STATUS_B */
#define SRB_USB_SUSPEND_MODE_STATUS(X)      ((X & 0x80) >> 7)
    #define SRB_USB_SUSPEND_MODE_NOT_ACTIVE     0
    #define SRB_USB_SUSPEND_MODE_ACTIVE         1
#define SRB_ACTUAL_CHARGE_CURRENT(X)        ((X & 0x40) >> 6)
    
/* STATUS_C */
#define SRC_CHARGER_ERROR_IRQ_STATUS(X)     ((X & 0x80) >> 7)
    #define SRC_CHARGER_ERROR_IRQ_NOT_ASSERT    0
    #define SRC_CHARGER_ERROR_IRQ_ASSERT        1
#define SRC_CHARGER_ERROR_STATUS(X)         ((X & 0x40) >> 6)
    #define SRC_NO_CHARGER_ERROR                0
    #define SRC_CHARGER_ERROR                   1
#define SRC_CHARGEING_CYCLE_STATUS(X)       ((X & 0x20) >> 5)
    #define SRC_NO_CHARGER_CYCLE                0
    #define SRC_HAS_CHARGE_CYCLE                1
#define SRC_BATTERY_VOLTAGE_LEVEL_STATUS(X) ((X & 0x10) >> 4)
    #define SRC_ABOVE_2_1V                      0
    #define SRC_BELOW_2_1V                      1
#define SRC_HOLD_OFF_STATUS(X)              ((X & 0x8) >> 3)
    #define SRC_NOT_IN_HOLD_OFF                 0
    #define SRC_IN_HOLD_OFF                     1
#define SRC_CHARGEING_STATUS(X)             ((X & 0x6) >> 1)
    #define SRC_NO_CHARGING_STATUS              0
    #define SRC_PRE_CHARGING_STATUS             1
    #define SRC_FAST_CHARGING_STATUS            2
    #define SRC_TAPER_CHARGING_STATUS           3
#define SRC_CHARGEING_ENABLE_DISABLE_STATUS(X) (X & 1)
    #define SRC_CHARGEING_DISABLED              0
    #define SRC_CHARGEING_ENABLE                1

/* STATUS_D */
#define SRD_RID_STATUS(X)                   ((X & 0x80) >> 7)
    #define SRD_RID_NOT_DONE                    0
    #define SRD_RID_DONE                        1
#define SRD_ACA_STATUS(X)                   ((X & 0x70) >> 4)
    #define SRD_RID_A                           0
    #define SRD_RID_B                           1
    #define SRD_RID_C                           2
    #define SRD_RID_FLOATING                    3
    #define SRD_RID_NOT_USED                    4
#define SRD_APSD_STATUS(X)                  ((X & 0x8) >> 3)
    #define SRD_APSD_NOT_COMPLETED              0
    #define SRD_APSD_COMPLETED                  1
#define SRD_APSD_RESULTS(X)                 (X & 0x7)
    #define SRD_APSD_NOT_RUN                    0
    #define SRD_APSD_CHARGING_DOWNSTREAM_PORT   1
    #define SRD_APSD_DEDICATED_DOWNSTREAM_PORT  2
    #define SRD_APSD_OTHER_DOWNSTREAM_PORT      3
    #define SRD_APSD_STANDARD_DOWNSTREAM_PORT   4
    #define SRD_APSD_ACA_CHARGER                5
    #define SRD_APSD_TBD                        6

/* STATUS_E */
#define SRE_USBIN(X)                        ((X & 0x80) >> 7)
    #define SRE_USBIN_NOT_IN_USE                0
    #define SRE_USBIN_IN_USE                    1
#define SRE_USB15_HC_MODE(X)                ((X & 0x60) >> 5)
    #define SRE_HC_MODE                         0
    #define SRE_USB1_OR_15_MODE                 1
    #define SRE_USB5_OR_9_MODE                  2
    #define SRE_NA_MODE                         3
#define SRE_AICL_STATUS(X)                  ((X & 0x10) >> 4)
    #define SRE_AICL_NOT_COMPLETED              0
    #define SRE_AICL_COMPLETED                  1
#define SRE_AICL_RESULT(X)                  (X & 0xF)
    #define SRE_AICL_300                        0
    #define SRE_AICL_500                        1
    #define SRE_AICL_700                        2
    #define SRE_AICL_900                        3
    #define SRE_AICL_1200                       4
    #define SRE_AICL_1500                       5
    #define SRE_AICL_1800                       6
    #define SRE_AICL_2000                       7
    #define SRE_AICL_2200                       8

/* Functions to read and write from SMB347 */
static inline int smb347_i2c_write_u8(u8 reg, u8 val)
{
    if (get_mbid() >= 4)
        i2c_set_bus_num(3); // 100
    return i2c_write(SMB347_I2C_ADDRESS, reg, 1, &val, 1);
}

static inline int smb347_i2c_read_u8(u8 reg, u8 *val)
{
    if (get_mbid() >= 4)
        i2c_set_bus_num(3); // 100
    return i2c_read(SMB347_I2C_ADDRESS, reg, 1, val, 1);
}

void summit_read_status_e(void);
int  summit_check_bmd(void);
int  summit_check_charge_status(void);
int  summit_is_precharge_timeout(void);
int  summit_is_complete_chg_timeout(void);
int  summit_is_aicl_complete(void);
void summit_switch_mode(int mode);    
void summit_charge_enable(int enable);
int  detect_usb(void);
void check_low_bat(void);

#endif /* __PMIC_SMB347__H__ */
