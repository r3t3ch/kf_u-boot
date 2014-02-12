/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 * Syed Mohammed Khasim <khasim@ti.com>
 *
 * Referred to Linux Kernel DSS driver files for OMAP4 by
 * Tomi Valkeinen from drivers/video/omap2/dss/
 * editted for OMAP4 by Michael Scott
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation's version 2 and any
 * later version the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef DSS_H
#define DSS_H

/* DSS Base Registers */
#define OMAP4_DSS_BASE		0x48040000
#define OMAP4_DISPC_BASE	0x48041000
#define OMAP4_RFBI_BASE		0x48042000
#define OMAP4_VENC_BASE		0x48043000
#define OMAP4_DSI1_BASE		0x48044000
#define OMAP4_DSI2_BASE		0x48045000

/* DSS Registers */
struct dss_regs {
	u32 revision;				/* 0x00 */
	u8 res1[16];				/* 0x04 */
	u32 sysstatus;				/* 0x14 */
	u8 res2[40];				/* 0x18 */
	u32 control;				/* 0x40 */
	u8 res3[24];				/* 0x44 */
	u32 status;				/* 0x5C */
};

/* DISPC Registers */
struct dispc_regs {
	u32 revision;				/* 0x000 */
	u8  res1[12];				/* 0x004 */
	u32 sysconfig;				/* 0x010 */
	u32 sysstatus;				/* 0x014 */
	u32 irqstatus;				/* 0x018 */
	u32 irqenable;				/* 0x01C */
	u8  res2[32];				/* 0x020 */
	u32 control1;				/* 0x040 */
	u32 config1;				/* 0x044 */
	u32 reserve_2;				/* 0x048 */
	u32 default_color0;			/* 0x04C */
	u32 default_color1;			/* 0x050 */
	u32 trans_color0;			/* 0x054 */
	u32 trans_color1;			/* 0x058 */
	u32 line_status;			/* 0x05C */
	u32 line_number;			/* 0x060 */
	u32 timing_h1;				/* 0x064 */
	u32 timing_v1;				/* 0x068 */
	u32 pol_freq1;				/* 0x06C */
	u32 divisor1;				/* 0x070 */
	u32 global_alpha;			/* 0x074 */
	u32 size_tv;				/* 0x078 */
	u32 size_lcd1;				/* 0x07C */
	u32 gfx_ba0;				/* 0x080 */
	u32 gfx_ba1;				/* 0x084 */
	u32 gfx_position;			/* 0x088 */
	u32 gfx_size;				/* 0x08C */
	u8  unused[16];				/* 0x090 */
	u32 gfx_attributes;			/* 0x0A0 */
	u32 gfx_fifo_threshold;			/* 0x0A4 */
	u32 gfx_fifo_size_status;		/* 0x0A8 */
	u32 gfx_row_inc;			/* 0x0AC */
	u32 gfx_pixel_inc;			/* 0x0B0 */
	u32 gfx_window_skip;			/* 0x0B4 */
	u32 gfx_table_ba;			/* 0x0B8 */
	u8  vid1_settings[136];			/* 0x0BC */
	u8  vid2_settings[144];			/* 0x144 */
	u32 data1_cycle1;			/* 0x1D4 */
	u32 data1_cycle2;			/* 0x1D8 */
	u32 data1_cycle3;			/* 0x1DC */
	u8  vid1_fir_coef[32];			/* 0x1E0 */
	u8  vid2_fir_coef[32];			/* 0x200 */
	u32 cpr1_coef_red;			/* 0x220 */
	u32 cpr1_coef_green;			/* 0x224 */
	u32 cpr1_coef_blue;			/* 0x228 */
	u32 gfx_preload;			/* 0x22C */
	u32 vid1_preload;			/* 0x230 */
	u32 vid2_preload;			/* 0x234 */
	u32 control2;				/* 0x238 */
	u8  unused_2[196];			/* 0x23C */
	u8  vid3_settings[172];			/* 0x300 */
	u32 default_color2;			/* 0x3AC */
	u32 trans_color2;			/* 0x3B0 */
	u32 cpr2_coef_red;			/* 0x3B4 */
	u32 cpr2_coef_green;			/* 0x3B8 */
	u32 cpr2_coef_blue;			/* 0x3BC */
	u32 data2_cycle1;			/* 0x3C0 */
	u32 data2_cycle2;			/* 0x3C4 */
	u32 data2_cycle3;			/* 0x3C8 */
	u32 size_lcd2;				/* 0x3CC */
	u8  unused_3[48];			/* 0x3D0 */
	u32 timing_h2;				/* 0x400 */
	u32 timing_v2;				/* 0x404 */
	u32 pol_freq2;				/* 0x408 */
	u32 divisor2;				/* 0x40C */
	u8  unused_4[240];			/* 0x410 */
	u8  wb_settings[172];			/* 0x500 */
	u8  unused_5[84];			/* 0x5AC */
	u32 vid1_ba0_uv;			/* 0x600 */
	u32 vid1_ba1_uv;			/* 0x604 */
	u32 vid2_ba0_uv;			/* 0x608 */
	u32 vid2_ba1_uv;			/* 0x60C */
	u32 vid3_ba0_uv;			/* 0x610 */
	u32 vid3_ba1_uv;			/* 0x614 */
	u32 wb_ba0_uv;				/* 0x618 */
	u32 wb_ba1_uv;				/* 0x61C */
	u32 config2;				/* 0x620 */
};

/* VENC Registers */
struct venc_regs {
	u32 rev_id;				/* 0x00 */
	u32 status;				/* 0x04 */
	u32 f_control;				/* 0x08 */
	u32 reserve_1;				/* 0x0C */
	u32 vidout_ctrl;			/* 0x10 */
	u32 sync_ctrl;				/* 0x14 */
	u32 reserve_2;				/* 0x18 */
	u32 llen;				/* 0x1C */
	u32 flens;				/* 0x20 */
	u32 hfltr_ctrl;				/* 0x24 */
	u32 cc_carr_wss_carr;			/* 0x28 */
	u32 c_phase;				/* 0x2C */
	u32 gain_u;				/* 0x30 */
	u32 gain_v;				/* 0x34 */
	u32 gain_y;				/* 0x38 */
	u32 black_level;			/* 0x3C */
	u32 blank_level;			/* 0x40 */
	u32 x_color;				/* 0x44 */
	u32 m_control;				/* 0x48 */
	u32 bstamp_wss_data;			/* 0x4C */
	u32 s_carr;				/* 0x50 */
	u32 line21;				/* 0x54 */
	u32 ln_sel;				/* 0x58 */
	u32 l21__wc_ctl;			/* 0x5C */
	u32 htrigger_vtrigger;			/* 0x60 */
	u32 savid__eavid;			/* 0x64 */
	u32 flen__fal;				/* 0x68 */
	u32 lal__phase_reset;			/* 0x6C */
	u32 hs_int_start_stop_x;		/* 0x70 */
	u32 hs_ext_start_stop_x;		/* 0x74 */
	u32 vs_int_start_x;			/* 0x78 */
	u32 vs_int_stop_x__vs_int_start_y;	/* 0x7C */
	u32 vs_int_stop_y__vs_ext_start_x;	/* 0x80 */
	u32 vs_ext_stop_x__vs_ext_start_y;	/* 0x84 */
	u32 vs_ext_stop_y;			/* 0x88 */
	u32 reserve_3;				/* 0x8C */
	u32 avid_start_stop_x;			/* 0x90 */
	u32 avid_start_stop_y;			/* 0x94 */
	u32 reserve_4;				/* 0x98 */
	u32 reserve_5;				/* 0x9C */
	u32 fid_int_start_x__fid_int_start_y;	/* 0xA0 */
	u32 fid_int_offset_y__fid_ext_start_x;	/* 0xA4 */
	u32 fid_ext_start_y__fid_ext_offset_y;	/* 0xA8 */
	u32 reserve_6;				/* 0xAC */
	u32 tvdetgp_int_start_stop_x;		/* 0xB0 */
	u32 tvdetgp_int_start_stop_y;		/* 0xB4 */
	u32 gen_ctrl;				/* 0xB8 */
	u32 reserve_7;				/* 0xBC */
	u32 reserve_8;				/* 0xC0 */
	u32 output_control;			/* 0xC4 */
	u32 dac_b__dac_c;			/* 0xC8 */
	u32 height_width;			/* 0xCC */
};

/* DSI1/2 Registers */
struct dsi_regs {
	u32 revision;				/* 0x000 */
	u8  res1[12];				/* 0x004 */
	u32 sysconfig;				/* 0x010 */
	u32 sysstatus;				/* 0x014 */
	u32 irqstatus;				/* 0x018 */
	u32 irqenable;				/* 0x01C */
	u8  res2[32];				/* 0x020 */
	u32 ctrl;				/* 0x040 */
	u32 gnq;				/* 0x044 */
	u32 complexio_cfg1;			/* 0x048 */
	u32 complexio_irqstatus;		/* 0x04C */
	u32 complexio_irqenable;		/* 0x050 */
	u32 clk_ctrl;				/* 0x054 */
	u32 timing1;				/* 0x058 */
	u32 timing2;				/* 0x05C */
	u32 vm_timing1;				/* 0x060 */
	u32 vm_timing2;				/* 0x064 */
	u32 vm_timing3;				/* 0x068 */
	u32 clk_timing;				/* 0x06C */
	u32 tx_fifo_vc_size;			/* 0x070 */
	u32 rx_fifo_vc_size;			/* 0x074 */
	u32 complexio_cfg2;			/* 0x078 */
	u32 rx_fifo_vc_fullness;		/* 0x07C */
	u32 vm_timing4;				/* 0x080 */
	u32 tx_fifo_vc_emptyness;		/* 0x084 */
	u32 vm_timing5;				/* 0x088 */
	u32 vm_timing6;				/* 0x08C */
	u32 vm_timing7;				/* 0x090 */
	u32 stopclk_timing;			/* 0x094 */
	u32 ctrl2;				/* 0x098 */
	u32 vm_timing8;				/* 0x09C */
	u32 te_hsync_width_0;			/* 0x0A0 */
	u32 te_vsync_width_0;			/* 0x0A4 */
	u32 te_hsync_number_0;			/* 0x0A8 */
	u32 te_hsync_width_1;			/* 0x0AC */
	u32 te_vsync_width_1;			/* 0x0B0 */
	u32 te_hsync_number_1;			/* 0x0B4 */
	u32 vc_ctrl_0;				/* 0x100 */
	u32 vc_te_0;				/* 0x104 */
	u32 vc_long_packet_header_0;		/* 0x108 */
	u32 vc_long_packet_payload_0;		/* 0x10C */
	u32 vc_short_packet_header_0;		/* 0x110 */
	u32 vc_short_packet_payload_0;		/* 0x114 */
	u32 vc_irqstatus_0;			/* 0x118 */
	u32 vc_irqenable_0;			/* 0x11C */
	u32 vc_ctrl_1;				/* 0x120 */
	u32 vc_te_1;				/* 0x124 */
	u32 vc_long_packet_header_1;		/* 0x128 */
	u32 vc_long_packet_payload_1;		/* 0x12C */
	u32 vc_short_packet_header_1;		/* 0x130 */
	u32 vc_short_packet_payload_1;		/* 0x134 */
	u32 vc_irqstatus_1;			/* 0x138 */
	u32 vc_irqenable_1;			/* 0x13C */
	u32 vc_ctrl_2;				/* 0x140 */
	u32 vc_te_2;				/* 0x144 */
	u32 vc_long_packet_header_2;		/* 0x148 */
	u32 vc_long_packet_payload_2;		/* 0x14C */
	u32 vc_short_packet_header_2;		/* 0x150 */
	u32 vc_short_packet_payload_2;		/* 0x154 */
	u32 vc_irqstatus_2;			/* 0x158 */
	u32 vc_irqenable_2;			/* 0x15C */
	u32 vc_ctrl_3;				/* 0x160 */
	u32 vc_te_3;				/* 0x164 */
	u32 vc_long_packet_header_3;		/* 0x168 */
	u32 vc_long_packet_payload_3;		/* 0x16C */
	u32 vc_short_packet_header_3;		/* 0x170 */
	u32 vc_short_packet_payload_3;		/* 0x174 */
	u32 vc_irqstatus_3;			/* 0x178 */
	u32 vc_irqenable_3;			/* 0x17C */
};

enum omap_display_type {
	OMAP_DISPLAY_TYPE_NONE		= 0,
	OMAP_DISPLAY_TYPE_DPI		= 1 << 0,
	OMAP_DISPLAY_TYPE_DBI		= 1 << 1,
	OMAP_DISPLAY_TYPE_SDI		= 1 << 2,
	OMAP_DISPLAY_TYPE_DSI		= 1 << 3,
	OMAP_DISPLAY_TYPE_VENC		= 1 << 4,
	OMAP_DISPLAY_TYPE_HDMI		= 1 << 5,
};

enum omap_plane {
	OMAP_DSS_GFX	= 0,
	OMAP_DSS_VIDEO1	= 1,
	OMAP_DSS_VIDEO2	= 2,
	OMAP_DSS_VIDEO3 = 3,
	OMAP_DSS_WB	= 4,
};

enum omap_channel {
	OMAP_DSS_CHANNEL_LCD	= 0,
	OMAP_DSS_CHANNEL_DIGIT	= 1,
	OMAP_DSS_CHANNEL_LCD2	= 2,
};

enum omap_color_mode {
	OMAP_DSS_COLOR_CLUT1		= 1 << 0,  /* BITMAP 1 */
	OMAP_DSS_COLOR_CLUT2		= 1 << 1,  /* BITMAP 2 */
	OMAP_DSS_COLOR_CLUT4		= 1 << 2,  /* BITMAP 4 */
	OMAP_DSS_COLOR_CLUT8		= 1 << 3,  /* BITMAP 8 */
	OMAP_DSS_COLOR_RGB12U		= 1 << 4,  /* RGB12, 16-bit container */
	OMAP_DSS_COLOR_ARGB16		= 1 << 5,  /* ARGB16 */
	OMAP_DSS_COLOR_RGB16		= 1 << 6,  /* RGB16 */
	OMAP_DSS_COLOR_RGB24U		= 1 << 7,  /* RGB24, 32-bit container */
	OMAP_DSS_COLOR_RGB24P		= 1 << 8,  /* RGB24, 24-bit container */
	OMAP_DSS_COLOR_YUV2		= 1 << 9,  /* YUV2 4:2:2 co-sited */
	OMAP_DSS_COLOR_UYVY		= 1 << 10, /* UYVY 4:2:2 co-sited */
	OMAP_DSS_COLOR_ARGB32		= 1 << 11, /* ARGB32 */
	OMAP_DSS_COLOR_RGBA32		= 1 << 12, /* RGBA32 */
	OMAP_DSS_COLOR_RGBX32		= 1 << 13, /* RGBx32 */
	OMAP_DSS_COLOR_NV12		= 1 << 14, /* NV12 format: YUV 4:2:0 */
	OMAP_DSS_COLOR_RGBA16		= 1 << 15, /* RGBA16 - 4444 */
	OMAP_DSS_COLOR_RGBX16		= 1 << 16, /* RGBx16 - 4444 */
	OMAP_DSS_COLOR_ARGB16_1555	= 1 << 17, /* ARGB16 - 1555 */
	OMAP_DSS_COLOR_XRGB16_1555	= 1 << 18, /* xRGB16 - 1555 */
};

enum omap_dispc_gfx_format {
	OMAP_DISP_GFX_CLUT1		= 0,  /* BITMAP 1 */
	OMAP_DISP_GFX_CLUT2		= 1,  /* BITMAP 2 */
	OMAP_DISP_GFX_CLUT4		= 2,  /* BITMAP 4 */
	OMAP_DISP_GFX_CLUT8		= 3,  /* BITMAP 8 */
	OMAP_DISP_GFX_RGB12U		= 4,  /* RGB12, 16-bit container */
	OMAP_DISP_GFX_ARGB16		= 5,  /* ARGB16-4444 */
	OMAP_DISP_GFX_RGB16		= 6,  /* RGB16 565 */
	OMAP_DISP_GFX_ARGB16_1555	= 7,  /* ARGB16-1555 */
	OMAP_DISP_GFX_RGB24U		= 8,  /* RGB24, 32-bit container */
	OMAP_DISP_GFX_RGB24P		= 9,  /* RGB24, 24-bit container */
	OMAP_DISP_GFX_RGBx12_4444	= 10, /* RGBx12 */
	OMAP_DISP_GFX_RGBA12_4444	= 11, /* RGBA12 */
	OMAP_DISP_GFX_ARGB32		= 12, /* ARGB32 */
	OMAP_DISP_GFX_RGBA32		= 13, /* RGBA32 */
	OMAP_DISP_GFX_RGB24A		= 14, /* RGB24, 24-bit container aligned on MSB */
	OMAP_DISP_GFX_RGB15_1555	= 15,
};

/* Few Register Offsets */
#define TFTSTN_SHIFT			3
/* LCD display type */
#define PASSIVE_DISPLAY			0
#define ACTIVE_DISPLAY			1

#define TFTDATALINES_SHIFT		8
/* TFTDATALINES */
#define LCD_INTERFACE_12_BIT		0
#define LCD_INTERFACE_16_BIT		1
#define LCD_INTERFACE_18_BIT		2
#define LCD_INTERFACE_24_BIT		3


#define GFX_ENABLE			1
#define GFX_FORMAT_SHIFT		1
#define LOADMODE_SHIFT			1

/* DSS_CTRL */
#define LCD2_TV_SEL			(1 << 17) // 0=LCD2, 1=TV

/* DISPC_SYSCONFIG */
#define DSS_SOFTRESET			(1 << 1)

/* DSS_SYSSTATUS / DISPC_SYSSTATUS */
#define DSS_RESETDONE			1

/* Enabling Display controller (DISPC_CONTROL1) */
#define LCD_ENABLE			1
#define TV_ENABLE			(1 << 1)
#define GO_LCD				(1 << 5)
#define GO_TV				(1 << 6)
#define GP_OUT0				(1 << 15)
#define GP_OUT1				(1 << 16)

/* Configure VENC DSS Params */
#define VENC_CLK_ENABLE			(1 << 3)
#define DAC_DEMEN			(1 << 4)
#define DAC_POWERDN			(1 << 5)
#define VENC_OUT_SEL			(1 << 6)
#define DIG_LPP_SHIFT			16

/* Polarity */
#define DSS_IVS				(1 << 12)
#define DSS_IHS				(1 << 13)
#define DSS_IPC				(1 << 14)
#define DSS_IEO				(1 << 15)

/* Panel Configuration */
struct panel_config {
	u32 display_type;
	u8  channel;
	u32 panel_udelay;
	u32 pixel_clock;
	u8  lck_div;
	u8  pck_div;
	u16 x_res;
	u16 y_res;
	u16 hfp;
	u16 hsw;
	u16 hbp;
	u16 vfp;
	u16 vsw;
	u16 vbp;
	u32 pol_freq;

	u32 panel_type;
	u32 data_lines;
	u32 load_mode;

	u32 panel_color;
	u32 trans_color;
};

/* Panel Configuration */
struct graphics_config {
	u32 dsi_control;
	u32 gfx_format;
	u16 x_pos;
	u16 y_pos;
	u16 fifo_threshold_low;
	u16 fifo_threshold_high;
	u8  row_inc;
	u8  pixel_inc;
	u8  window_skip;
	u32 video_data_format;
};

#define DSS_HBP(bp)    (((bp) - 1) << 20)
#define DSS_HFP(fp)    (((fp) - 1) << 8)
#define DSS_HSW(sw)    ((sw) - 1)
#define DSS_VBP(bp)    ((bp) << 20)
#define DSS_VFP(fp)    ((fp) << 8)
#define DSS_VSW(sw)    ((sw) - 1)

#define PANEL_TIMING_H(bp, fp, sw)   (DSS_HBP(bp) | DSS_HFP(fp) | DSS_HSW(sw))
#define PANEL_TIMING_V(bp, fp, sw)   (DSS_VBP(bp) | DSS_VFP(fp) | DSS_VSW(sw))
#define PANEL_LCD_SIZE(xres, yres)   ((yres - 1) << 16 | (xres - 1))
#define PANEL_DISPC_DIVISOR(pcd,lcd) ((pcd << 0) | (lcd << 16))
#define DISPC_GFX_FIFO_THRESHOLD(low,high)	((low << 0) | (high << 16))

/* Generic DSS Functions */
void omap4_dss_register_configs(const struct panel_config *new_panel_cfg, const struct graphics_config *new_gfx_cfg);
void omap4_dss_apply_panel_config(void);
void omap4_dss_apply_graphics_config(void);
int  omap4_dss_apply_disp_config(int delay_count, int timeout);
void omap4_dss_reset_display_controller(unsigned int delay_count, unsigned int timeout);
void omap4_dss_enable(void);
void omap4_dss_fill_black_data(void);
void omap4_dss_video_init(int flag);
void *omap4_dss_get_frame_buffer(void);

#endif /* DSS_H */
