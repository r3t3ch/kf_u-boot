/*
 * cmd_idme.c
 *
 * Copyright 2011 Amazon Technologies, Inc. All Rights Reserved.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <common.h>
#include <command.h>
#include <mmc.h>
#include <idme.h>

#include <asm/io.h>

#ifdef CONFIG_ENABLE_IDME
typedef struct nvram_t {
	const char *name;
	unsigned int offset;
	unsigned int size;
} nvram_t;

#define IDME_MAGIC_NUMBER "beefdeed"
#define MAGIC_NUMBER_OFFSET 0
#define IDME_BASE_ADDR 8        /* magic number and size takes 8 bytes */

#define MAX_IDME_VALUE 2560

#define BOOT_PARTITION 1
#define USER_PARTITION 0

#define IDME_NUM_OF_EMMC_BLOCKS 10
#define IDME_VERSION_STR "1.2"

static const char *nvram_sig = "adbdefg12345";
static const struct nvram_t nvram_sig_info = {
        .name = "writesig",
        .offset = 0x1f0,
        .size = 12,
};

static const struct nvram_t nvram_sig_info_bitmap = {
        .name = "writesig_bitmap",
        .offset = 0x1fc,
        .size = 4,
};

#define GET_OFFSET(a) (a##_OFFSET)
#define GET_SIZE(a) (a##_SIZE)
#define CALC_OFFSET(a,b) ((a##_OFFSET(b)) + (a##_SIZE))

#define VER_OFFSET 0
#define VER_SIZE 4

#define BOARDID_OFFSET (VER_OFFSET + VER_SIZE)
#define BOARDID_SIZE 16

#define SERIAL_OFFSET (BOARDID_OFFSET + BOARDID_SIZE)
#define SERIAL_SIZE 16

#define MAC_OFFSET (SERIAL_OFFSET + SERIAL_SIZE)
#define MAC_SIZE 16

#define SEC_OFFSET (MAC_OFFSET+MAC_SIZE)
#define SEC_SIZE 32

#define BT_OFFSET (SEC_OFFSET+SEC_SIZE)
#define BT_SIZE 16

#define GYRO_CAL_OFFSET (BT_OFFSET + BT_SIZE)
#define GYRO_CAL_SIZE 36

#define PID_OFFSET 0x200
#define PID_SIZE 32

#define PID2_OFFSET (PID_OFFSET + PID_SIZE)
#define PID2_SIZE 32

#define CAM_CAL_OFFSET (PID2_OFFSET + PID2_SIZE)
#define CAM_CAL_SIZE 2560

#define MFG_DATA_OFFSET (CAM_CAL_OFFSET + CAM_CAL_SIZE)
#define MFG_DATA_SIZE 512

#define BM_OFFSET 0x1000
#define BM_SIZE 16

#define PM_OFFSET (BM_OFFSET + BM_SIZE)
#define PM_SIZE 16

/* base address of idme location on user partition */
#define CONFIG_USERDATA_IDME_ADDR 0x80000
#define CONFIG_IDME_SIZE 0x1400  /* total size of  5120 bytes - 10 NAND native blocks*/


static int idme_write(const unsigned char *pbuf,
                      const int is_boot);


static const struct nvram_t nvram_info[] = {
	{
		.name = "version",
		.offset = GET_OFFSET(VER),
		.size = GET_SIZE(VER),
	},
	{
		.name = "boardid",
		.offset =GET_OFFSET(BOARDID),
		.size = GET_SIZE(BOARDID),
	},

	{
		.name = "serial",
		.offset = GET_OFFSET(SERIAL),
		.size = GET_SIZE(SERIAL),
	},
	{
		.name = "mac",
		.offset = GET_OFFSET(MAC),
		.size = GET_SIZE(MAC),
	},
	{
		.name = "sec",
		.offset = GET_OFFSET(SEC),
		.size = GET_SIZE(SEC),
	},
	{
		.name = "bt",
		.offset = GET_OFFSET(BT),
		.size = GET_SIZE(BT),
	},
	{
		.name = "pid",
		.offset = GET_OFFSET(PID),
		.size = GET_SIZE(PID),
	},
	{
		.name = "pid2",
		.offset = GET_OFFSET(PID2),
		.size = GET_SIZE(PID2),
	},
	{
		.name = "bootmode",
		.offset = GET_OFFSET(BM),
		.size = GET_SIZE(BM),
	},
	{
		.name = "postmode",
		.offset = GET_OFFSET(PM),
		.size = GET_SIZE(PM),
	},
	{
		.name = "gyrocal",
		.offset = GET_OFFSET(GYRO_CAL),
		.size = GET_SIZE(GYRO_CAL),
	},
	{
		.name = "camcal",
		.offset = GET_OFFSET(CAM_CAL),
		.size = GET_SIZE(CAM_CAL),
	},
};


#define CONFIG_NUM_NV_VARS (sizeof(nvram_info)/sizeof(nvram_info[0]))

static unsigned char gblock_buff[CONFIG_MMC_BLOCK_SIZE];

static unsigned char gidme_buff[CONFIG_IDME_SIZE];
static int gidme_buff_valid = 0;

static char *simple_itoa(unsigned int i)
{
	/* 21 digits plus null terminator, good for 64-bit or smaller ints */
	static char local[22];
	char *p = &local[21];
	*p-- = '\0';
	do {
		*p-- = '0' + i % 10;
		i /= 10;
	} while (i > 0);
	return p + 1;
}

unsigned int simple_atoi(const char *s, const char *e)
{
        unsigned int result = 0, value = 0;
        /* first remove all leading 0s */
        while(*s == '0' && s < e){
                s++;
        }

        while(s < e){
                value = *s - '0';
                result = result * 10 + value;
                s++;
        }
        return result;
}

int idme_get_magic_size( char *buf)
{
        int ret = 0;

	if (mmc_sw_part(CONFIG_MMC_BOOTFLASH, CONFIG_IDME_PARTITION_NUM) != 1) {
		printf("ERROR: couldn't switch to boot partition\n");
		return -1;
	}

	/*
	 * ey: mmc_read is actually a block read, which takes block address and returns a block of data
	 */
	ret = mmc_read(CONFIG_MMC_BOOTFLASH, 0, gblock_buff, CONFIG_MMC_BLOCK_SIZE);
	if (ret != 1) {
		printf("%s error! Couldn't read vars from partition\n", __FUNCTION__);
		/* need to switch back to user partition even on error */
	}

	/*  ey: switching out of boot partition by clearing PARTITION_ACCESS bits */
	if (mmc_sw_part(CONFIG_MMC_BOOTFLASH, 0) != 1) {
		printf("ERROR: couldn't switch back to user partition\n");
		return -1;
	}

	// if the mmc_read failed, return
	if (ret != 1) {
		return ret;
	}

        // copy the magic word and size
        memcpy(buf, gblock_buff, IDME_BASE_ADDR);

        return 0;
}

/* read entire idme out */
static int idme_read(unsigned char *pbuf, int is_boot)
{
        int ret=0;
        int len = 0;
        int block_addr = 0;

        block_addr = CONFIG_USERDATA_IDME_ADDR / CONFIG_MMC_BLOCK_SIZE;
        if (is_boot){

                /* switch to boot partition */
                if (mmc_sw_part(CONFIG_MMC_BOOTFLASH, CONFIG_IDME_PARTITION_NUM) != 1) {
                        printf("ERROR: couldn't switch to boot partition\n");
                        return -1;
                }
                block_addr = 0;
        }

        while (len < CONFIG_IDME_SIZE){
	/*
	 * ey: mmc_read is actually a block read, which takes block address and returns a block of data
	 */
                ret = mmc_read(CONFIG_MMC_BOOTFLASH,
                               block_addr,
                               pbuf+len,
                               CONFIG_MMC_BLOCK_SIZE);
                if (ret != 1) {
                        printf("%s error! Couldn't read vars from partition\n", __FUNCTION__);
                        /* need to switch back to user partition even on error */
                        break;
                }
                len += CONFIG_MMC_BLOCK_SIZE;
                block_addr++;
        }

        if (is_boot){
                /*  ey: switching out of boot partition by clearing PARTITION_ACCESS bits */
                if (mmc_sw_part(CONFIG_MMC_BOOTFLASH, 0) != 1) {
                        printf("ERROR: couldn't switch back to user partition\n");
                        return -1;
                }
        }

	// if the mmc_read failed, return
	if (ret != 1) {
		return -1;
	}
        return 0;
}

static const struct nvram_t *idme_find_var(const char *name)
{
	int i, len;

	for (i = 0; i < CONFIG_NUM_NV_VARS; i++) {

		len = strlen(nvram_info[i].name);
		// Fix pid and pid2 string match problem
		if (strlen(name) == len && strncmp(name, nvram_info[i].name, len) == 0) {
			return &nvram_info[i];
		}
	}

	return NULL;
}

/* get variable from cached memory */
static int idme_get_var_ex(
 const unsigned char *pbuf,
 const char *name,
 const char *buf,
 const int buflen)
{
        const struct nvram_t *nv;
        int offset = 0, size= 0;
        nv = idme_find_var(name);

        if (!nv) {
                printf("Error! Couldn't find NV variable %s\n", name);
                return -1;
        }

        offset = nv->offset;
        size = nv->size;

        if (size + 1 > buflen){
                printf("Error, buffer is too small for %s, return needed size\n", name);
                return (size+1);
        }

        memcpy(buf, pbuf+offset, size);

        return 0;
}

static int idme_print_vars_ex(void)
{
        int i,j;
        char value[MAX_IDME_VALUE];
        unsigned char buffer[CONFIG_IDME_SIZE];
        unsigned char *pidme_data = &buffer[IDME_BASE_ADDR];

        memset(buffer, 0, CONFIG_IDME_SIZE);
        if (idme_read(buffer,BOOT_PARTITION) != 0){
                printf("Error, failed to read the idme\n");
                return -1;
        }

       /* checking the magic nubmer */
        if (strncmp(buffer, IDME_MAGIC_NUMBER, strlen(IDME_MAGIC_NUMBER))){
                printf("Warning, no idme data in boot area.\n");
                return -1;
        }

        for (i = 0; i < CONFIG_NUM_NV_VARS; i++){
                if (idme_get_var_ex(pidme_data,
                                    nvram_info[i].name,
                                    value,
                                    sizeof(value)) == 0){
                        value[nvram_info[i].size] = 0;
			if (nvram_info[i].size < CFG_CBSIZE) {
			  printf("%s: %s\n", nvram_info[i].name, value);
			}
                }else{
                        return 0;
                }
        }
        return 0;
}

static int idme_print_vars(void)
{
	int i;
	char value[MAX_IDME_VALUE];

        if (idme_get_magic_size(value) != 0){
                printf("Error, failed to get the idme magic word\n");
                return -1;
        }


       /* checking the magic nubmer */
        if (strncmp(value, IDME_MAGIC_NUMBER, strlen(IDME_MAGIC_NUMBER))){
                printf("Error, invalid magic number\n");
                return -1;
        }

	for (i = 0; i < CONFIG_NUM_NV_VARS; i++) {

		if (idme_get_var(nvram_info[i].name, value, sizeof(value)) == 0){
			value[nvram_info[i].size] = 0;
			if (nvram_info[i].size < CFG_CBSIZE) {
			  printf("%s: %s\n", nvram_info[i].name, value);
			}
		}else{
			/* ey: stop reading if we ever got error because last read failure will cause the following read hang */
			return 0;
		}
	}

	return 0;
}

int idme_get_var(const char *name, char *buf, int buflen)
{
	int ret;
	int block, offset, size;


        const struct nvram_t *nv;
                
	buf[0] = 0;
        nv = idme_find_var(name);
        if (!nv) {
                printf("Error! Couldn't find NV variable %s\n", name);
                return -1;
        }
        // get the sector address
        block = (nv->offset + IDME_BASE_ADDR) / CONFIG_MMC_BLOCK_SIZE; 
        // get the offset into the sector
        offset = (nv->offset + IDME_BASE_ADDR) % CONFIG_MMC_BLOCK_SIZE; 
        size = nv->size;


	if (mmc_sw_part(CONFIG_MMC_BOOTFLASH, CONFIG_IDME_PARTITION_NUM) != 1) {
		printf("ERROR: couldn't switch to boot partition\n");
		return -1;
	}

	/*
	 * ey: mmc_read is actually a block read, 
         which takes block address and returns a block of data
	 */
	ret = mmc_read(CONFIG_MMC_BOOTFLASH, block, gblock_buff, CONFIG_MMC_BLOCK_SIZE);
	if (ret != 1) {
		printf("%s error! Couldn't read vars from partition\n", __FUNCTION__);
		/* need to switch back to user partition even on error */
	}

	/*  ey: switching out of boot partition by clearing PARTITION_ACCESS bits */
	if (mmc_sw_part(CONFIG_MMC_BOOTFLASH, 0) != 1) {
		printf("ERROR: couldn't switch back to user partition\n");
		return -1;
	}

	// if the mmc_read failed, return
	if (ret != 1) {
		return ret;
	}

	// buffer is too small, return needed size
	if (size + 1 > buflen) {
		return (size + 1);
	}

 	memcpy(buf, gblock_buff + offset, size);

	buf[size] = 0;

	return 0;
}

static int idme_clean(void)
{
	unsigned char buffer[CONFIG_IDME_SIZE];
	memset(buffer, 0, CONFIG_IDME_SIZE);

	idme_write(buffer, USER_PARTITION) ;
	idme_write(buffer, BOOT_PARTITION) ;

        return 0;
}

/* generate default idme data */
static int idme_generate_default_vars(const unsigned char *pbuf)
{
        unsigned char *pidme_data = pbuf + IDME_BASE_ADDR;
        int i = 0;
        printf("generate default vars\n");

        memset(pbuf, 0, CONFIG_IDME_SIZE);

        /* generate the magic number */
        memcpy(pbuf, IDME_MAGIC_NUMBER, strlen(IDME_MAGIC_NUMBER));

        /* use default values to intialize idme data */
        memcpy(pidme_data+nvram_info[0].offset, IDME_VERSION_STR, strlen(IDME_VERSION_STR));

        memcpy(pidme_data+nvram_info[1].offset, "8010400000000010", 16); /* boardid default jem PVT */

        for (i = 2; i < CONFIG_NUM_NV_VARS; i++){
                if (strncmp(nvram_info[i].name, "bootmode", strlen("bootmode")) == 0){
                        memcpy(pidme_data+nvram_info[i].offset, "2", 1);    /* set initial bootmode to diag kernel */
                } else if(strncmp(nvram_info[i].name, "postmode", strlen("postmode")) == 0){
                        memset(pidme_data+nvram_info[i].offset, 0, nvram_info[i].size);
                }else{
                        memset(pidme_data+nvram_info[i].offset, '0', nvram_info[i].size);
                }
        }


        return 0;
}

/* WARNING calling this function will overwrite the rest of idme data */
static int idme_write_magic_size(void)
{
        int size = 0;
        int ret = 0;
        char *p;
        printf("Warning! programming the magic number will overwrite idme data.\n");

	// clear out old value
	memset(gblock_buff, 0, CONFIG_MMC_BLOCK_SIZE);

        memcpy(gblock_buff, IDME_MAGIC_NUMBER, strlen(IDME_MAGIC_NUMBER));


	if (mmc_sw_part(CONFIG_MMC_BOOTFLASH, CONFIG_IDME_PARTITION_NUM) != 1) {
		printf("ERROR: couldn't switch to boot partition\n");
		return -1;
	}

	ret = mmc_write(CONFIG_MMC_BOOTFLASH, gblock_buff, 0, CONFIG_MMC_BLOCK_SIZE);
	if (ret != 1) {
		printf("Error! Couldn't write NV variables. (%d)\n", ret);
		/* need to switch back to user partition even on error */
	}


        /* switch to boot partition */
	if (mmc_sw_part(CONFIG_MMC_BOOTFLASH,0) != 1 || ret != 1) {
		printf("ERROR: couldn't switch back to user partition\n");
		return -1;
	}


        return 0;

}

/* write the buffer to either boot or user area */
static int idme_write(
                      const unsigned char *pbuf,
                      const int is_boot)
{
        int len = 0;
        int ret = 0;
        int block_addr = CONFIG_USERDATA_IDME_ADDR / CONFIG_MMC_BLOCK_SIZE;
        if (is_boot){
                block_addr = 0;

                /* switch to boot partition */
                if (mmc_sw_part(CONFIG_MMC_BOOTFLASH,
                                CONFIG_IDME_PARTITION_NUM) != 1){
                        printf("Error, could not switch to boot partition\n");
                        return -1;
                }

        }

        while (len < CONFIG_IDME_SIZE){
                ret = mmc_write(CONFIG_MMC_BOOTFLASH,
                                pbuf+len,
                                block_addr,
                                CONFIG_MMC_BLOCK_SIZE);
                if (ret != 1){
                        printf("Error, mmc_write failed.\n");
                        break;
                }
                len += CONFIG_MMC_BLOCK_SIZE;
                block_addr++;
        }

        if (is_boot){
                if (mmc_sw_part(CONFIG_MMC_BOOTFLASH,0) != 1) {
                        printf("Error, failed to switch back to user partition\n");
                        return -1;
                }
        }

        if (ret != 1){
                return -1;
        }

        return 0;
}

/* update the idme from user area to boot area */
int idme_update_vars_ex(void)
{
        /* update from user idme is allowed only in engineering build */
//#if (defined (UBOOT_ENG_BUILD) || defined (UBOOT_FTM))
        int i;
        unsigned char buffer[CONFIG_IDME_SIZE];
        unsigned char *pidme_data = NULL, *psig = NULL;
        unsigned char boot_buffer[CONFIG_IDME_SIZE];
        unsigned char *pidme_boot_data = NULL;
        unsigned int *pbitmap = NULL;

        memset(buffer, 0, CONFIG_IDME_SIZE);
        memset(boot_buffer, 0, CONFIG_IDME_SIZE);

        /* load the idme data from boot area */
        if (idme_read(boot_buffer, BOOT_PARTITION) != 0) {
                printf("Error, failed to read idme from boot area.\n");
                return -1;
        }

        /* check if the boot idme is valid */        
        if (strncmp(boot_buffer, IDME_MAGIC_NUMBER, strlen(IDME_MAGIC_NUMBER))){
                printf("WARNING: Failed to find IDME in boot area. Generating default idme ...\n");
                idme_generate_default_vars(boot_buffer);
                idme_write(boot_buffer, BOOT_PARTITION);
                return 0;
        }

        /* first load the idme data from user area */
        if (idme_read(buffer, USER_PARTITION) != 0) {
                printf("Error, failed to read idme from user area.\n");
                return -1;
        }

        if (strncmp(buffer, IDME_MAGIC_NUMBER, strlen(IDME_MAGIC_NUMBER))){

                printf("no user idme data in user area\n");
                return -1;
        }
        pidme_data = &buffer[IDME_BASE_ADDR];
        pidme_boot_data = &boot_buffer[IDME_BASE_ADDR];
        pbitmap = (unsigned int *)(pidme_data+nvram_sig_info_bitmap.offset);

        psig = pidme_data+nvram_sig_info.offset;

#ifdef IDME_DEBUG
        printf("dump sig info:\n");
        for (i = 0; i < 16; i++){
                printf("%c ", psig[i]);
        }
#endif

        /* check if the update is required */
        if (memcmp(pidme_data+nvram_sig_info.offset, 
                   nvram_sig, nvram_sig_info.size)==0){
                printf("Updating idme... \n");
                printf("bitmap 0x%X\n", *pbitmap);
                for(i = 0; i < CONFIG_NUM_NV_VARS; i++){
                        if((*pbitmap & (1 << i)) != 0){
                                memcpy(pidme_boot_data + nvram_info[i].offset, pidme_data + nvram_info[i].offset, nvram_info[i].size);
                                printf("update %s\n", nvram_info[i].name);
                        } 
                }
                /* write the buffer back to boot area */
                memset(psig, 0, nvram_sig_info.size); /* zero out the sig info area */
                *pbitmap = 0;
                if (idme_write(boot_buffer, BOOT_PARTITION) != 0){
                        printf("Error, idme_write failed.\n");
                        return -1;
                }
                // Update sig and bitmap just in case boot area does not sync to user area
                if(idme_write(buffer, USER_PARTITION) != 0){
                        printf("Error, idme_write failed.\n");
                        return -1;
                }
        }
//#endif //(UBOOT_ENG_BUILD)
        return 0;
}

/* load the idme data to global cache buffer from boot area */
int idme_load(void)
{
        if (idme_read(gidme_buff, BOOT_PARTITION) != 0){
                printf("Error, read idme failed.\n");
                return -1;
        }

        /* checking the magic nubmer */
        if (strncmp(gidme_buff, IDME_MAGIC_NUMBER, strlen(IDME_MAGIC_NUMBER))){
                printf("Warning, no idme data found on boot area.\n");
                idme_generate_default_vars(gidme_buff);
        }

        /* set the flag to indicate the buffer is valid */
        gidme_buff_valid = 1;

        return 0;
}

static char *pdkernel = "dkernel";
static char *precovery = "recovery";
#define GPIO_VOL_UP_KEY 1       /* volume up key gpio */
#define GPIO_VOL_DN_KEY 50      /* volume down key gpio */
int idme_select_bootmode_by_key(char **ptn)
{
        int gpio_vol_up = GPIO_VOL_UP_KEY;
        int gpio_vol_down = GPIO_VOL_DN_KEY;
        int val_vol_up = 1;
        int val_vol_down = 1;

        val_vol_up = get_gpio_value(gpio_vol_up);
        val_vol_down = get_gpio_value(gpio_vol_down);
        printf("[%s] vol_up:%d, vol_down:%d\n",
               __func__, val_vol_up, val_vol_down);

#ifdef ENABLE_DIAG_KEY
        if (val_vol_up && val_vol_down) {
                /* button up is pressed only */
                printf("Tablet: Enter diag mode: ....\n");
                *ptn = pdkernel;
        }
#endif

#ifdef ENABLE_RECOVERY_KEY
        if (!val_vol_up && val_vol_down) {
                /* button up is pressed only */
                printf("Tablet: Enter recovery: ....\n");
                *ptn = precovery;
        }
#endif

#ifdef ENABLE_FASTBOOT_KEY
        if (val_vol_up && !val_vol_down) {
                /* button down is pressed only */
                printf("Tablet: Enter fastboot: ....\n");
                do_fastboot(NULL, 0, 0, NULL);
        }
#endif
        return 0;
}

/*
  This function will reboot and force to usb boot
 */
void idme_reboot_usb_boot(void)
{
        __raw_writel( 0x4A326A0C , 0x4A326A00 );   // Address in Public Internal SRAM where SW Booting Configuration is
        __raw_writel( 0xCF00AA01 , 0x4A326A0C );   // Header for SW Booting Configuration
        __raw_writel( 0x0000000C , 0x4A326A10 );
        __raw_writel( 0x00450000 , 0x4A326A14 );   // USB Boot First
        __raw_writel( 0x00000000 , 0x4A326A18 );
        __raw_writel( 0x00000000 , 0x4A326A1C );
        /* now warm reset the silicon */
        __raw_writel(PRM_RSTCTRL_RESET_WARM_BIT,PRM_RSTCTRL);
}

/*
  This function determine which kernel image (diag or normal) to load
  base on the bootmode.
 */
int idme_select_boot_image(char **ptn)
{
	DECLARE_GLOBAL_DATA_PTR;

        unsigned char *pidme_data;
        char bootmode[IDME_MAX_BOOTMODE_LEN+1];
#ifdef IDME_DEBUG
        char boardid[IDME_MAX_BOARD_ID_LEN+1];
#endif
        int i;
	unsigned long boot_count = 0;
        u32 nbid = 0;

#if defined (UBOOT_ENG_BUILD)
	unsigned char buffer[CONFIG_IDME_SIZE];
        memset(buffer, 0, CONFIG_IDME_SIZE);

        /* read the idme from boot partition */
        if (idme_read(buffer, BOOT_PARTITION) != 0){
                printf("Error, failed to read idme\n");
                return -1;
        }


        /* buffer content can be change now */
        pidme_data = &buffer[IDME_BASE_ADDR];

        /* found the magic number, then copy to user partition */
        idme_write(buffer, USER_PARTITION) ;
#endif //


        memset(bootmode, 0, IDME_MAX_BOOTMODE_LEN);
#if defined (UBOOT_ENG_BUILD)
        // get the bootmode
        for (i=0; i < CONFIG_NUM_NV_VARS; i++){
                if (strncmp(nvram_info[i].name, "bootmode", strlen("bootmode")) == 0){
                        memcpy(bootmode, (pidme_data+nvram_info[i].offset),  
                               nvram_info[i].size);
                        bootmode[nvram_info[i].size+1] = 0;
                        /* print the bootmode */
                        printf("%s: %s\n", nvram_info[i].name, bootmode);
                }else if (strncmp(nvram_info[i].name, "postmode", 
                                  strlen("postmode")) == 0){
			memcpy((char*)&boot_count,
				(pidme_data+nvram_info[i].offset),
				sizeof(unsigned long));
			printf("boot_count = %d\n", boot_count);
			boot_count++; // increase the boot count
                        /* only copy the bootcount part */
			memcpy((pidme_data+nvram_info[i].offset), 
                               &boot_count, sizeof(unsigned long));
			idme_update_var("postmode", (pidme_data+nvram_info[i].offset));
		}
        }
#else
        /* for production build, directly read the bootmode from emmc */
        idme_get_var("bootmode", bootmode, sizeof(bootmode));
#endif
        /* bootmode is usually 1 byte despite the length is 16 bytes */
        switch(bootmode[0]){
        case '1':
                printf("Select Normal boot image\n");
                break;
        case '2':
                printf("Select Diagnostic image\n");
                *ptn = pdkernel;
                break;
        case '3':
                printf("Select Recovery image\n");
                *ptn = precovery;
                break;
        case '5':
                // Next time, enter android
                show_multidownload();
                idme_update_var("bootmode", "1");
                idme_reboot_usb_boot();
                break;
        case '6':
                // Next time, enter ftm
                show_multidownload();
                idme_update_var("bootmode", "2");
                idme_reboot_usb_boot();
                break;
        case '7':
                // Select recovery but next time, enter android
                idme_update_var("bootmode", "1");
                printf("Select Recovery image\n");
                *ptn = precovery;
                break;
        default:
                printf("Unknown bootmode\n");
                break;
        }
//#if defined (UBOOT_ENG_BUILD)
        // Select boot mode by combination key
        idme_select_bootmode_by_key(ptn);
//#endif
        return 0;
}

// ey: called by setup_board_info() function in imx50_yoshi.c
int idme_check_update(void) {

	int ret = 0;
	int len = 0;

#ifdef CONFIG_IDME_UPDATE

	// read the MMC boot partition, but in the user area
	ret = mmc_read(CONFIG_MMC_BOOTFLASH,
		       CONFIG_IDME_UPDATE_ADDR,  // src address (0x3f000)
		       gblock_buff,                  // destination buffer
		       CONFIG_MMC_BLOCK_SIZE);     // size to read (512 bytes)
	if (ret != 1) {
		printf("Error! Couldn't read NV variables. (%d)\n", ret);
		return 0;
	}

	/* Check to see if we need to update idme vars */
	// ey: The magic is located at bottom of CONFIG_IDME_UPDATE_MAGIC_SIZE
	if (strncmp((char *) (gblock_buff + (CONFIG_MMC_BLOCK_SIZE - CONFIG_IDME_UPDATE_MAGIC_SIZE)),
		    CONFIG_IDME_UPDATE_MAGIC,
		    CONFIG_IDME_UPDATE_MAGIC_SIZE) == 0) {

		printf("Found updated idme variables.  Flashing...\n");

		while (len < CONFIG_MMC_USERDATA_SIZE) {

			if (mmc_sw_part(CONFIG_MMC_BOOTFLASH, CONFIG_IDME_PARTITION_NUM) != 1) {
				printf("ERROR: couldn't switch to boot partition\n");
				return -1;
			}

			// write the idme update data to USERDATA partition area
			ret = mmc_write(CONFIG_MMC_BOOTFLASH,
					gblock_buff,                               // source to write
					CONFIG_MMC_USERDATA_ADDR + len,       // destination to write
					CONFIG_MMC_BLOCK_SIZE);
			if (ret != 1) {
				printf("Error! Couldn't write NV variables. (%d)\n", ret);
				/* need to switch back to user partition even on error */
			}

			// switch back to user partition
			if (mmc_sw_part(CONFIG_MMC_BOOTFLASH, 0) != 1 || ret != 1) {
				printf("ERROR: couldn't switch back to user partition\n");
				return -1;
			}

			/* clear out the idme vars so we only do this update once */
			memset(gblock_buff, 0, CONFIG_MMC_BLOCK_SIZE);

			/* zero out the IDME_UPDATE location */
			ret = mmc_write(CONFIG_MMC_BOOTFLASH, gblock_buff, CONFIG_IDME_UPDATE_ADDR + len, CONFIG_MMC_BLOCK_SIZE);
			if (ret != 1) {
				printf("Error! Couldn't clear update variables. (%d)\n", ret);
				return -1;
			}

			len += CONFIG_CONFIG_MMC_BLOCK_SIZE;

			ret = mmc_read(CONFIG_MMC_BOOTFLASH, CONFIG_IDME_UPDATE_ADDR + len, gblock_buff, CONFIG_MMC_BLOCK_SIZE);
			if (ret != 1) {
				printf("Error! Couldn't read NV variables. (%d)\n", ret);
				return 0;
			}
		}

		printf("idme variable flash complete\n");
	}
#endif /* CONFIG_IDME_UPDATE */

	return ret;
}

static int idme_boot_count(void)
{
        char postmode_buf[PM_SIZE+1];
        unsigned long count = 0;
        memset(postmode_buf, 0, sizeof(postmode_buf));
        // get the postmode
        if (idme_get_var("postmode", postmode_buf, sizeof(postmode_buf)) == 0){
                postmode_buf[PM_SIZE] = '\0';
                printf("postmode = %s\n", postmode_buf+sizeof(unsigned long));
                /* convert the string to integer */
                memcpy((void*)&count, postmode_buf, sizeof(unsigned long));
                printf("count = %lu\n", count);
                count++;
                memcpy(postmode_buf, (void*)&count, sizeof(unsigned long));
                idme_update_var("postmode", postmode_buf);
        }
        return 0;
}

static int idme_set_boot_count(unsigned int bcount)
{
        char postmode_buf[PM_SIZE+1];
        unsigned long count = 0;
        memset(postmode_buf, 0, sizeof(postmode_buf));
        // get the postmode
        if (idme_get_var("postmode", postmode_buf, sizeof(postmode_buf)) == 0){
                postmode_buf[PM_SIZE] = '\0';
                printf("postmode = %s\n", postmode_buf+sizeof(unsigned long));
                memcpy((void*)&count, postmode_buf, sizeof(unsigned long));
                //current boot count number
                printf("count = %lu\n", count);
                count = bcount;
                memcpy(postmode_buf, (void*)&count, sizeof(unsigned long));
                idme_update_var("postmode", postmode_buf);
        }
        return 0;
}
        

int static is_valid_board_info(const char *info, const int max_len)
{
	// assume the buf is zero terminated

    	int i;

    	for (i = 0; i < max_len && info[i] != '\0'; i++) {
		if ((info[i] < '0') &&
		    (info[i] > '9') &&
		    (info[i] < 'A') &&
		    (info[i] > 'Z'))
		    return 0;
    	}
	return 1;

}
static u8 board_id[IDME_MAX_BOARD_ID_LEN+1];
static u8 board_serial[IDME_MAX_BOARD_SERIAL_LEN+1];
u8 *idme_get_board_id(void)
{
        printf("idme_get_board_id\n");

        memset(board_id, 0, sizeof board_id);
	if (idme_get_var("boardid", board_id, sizeof(board_id)) != 0){
		printf("Error, idme_get_board_id failed\n");
		// not found, clean the buffer
		memset(board_id, '0', sizeof(board_id)-1);
		board_id[sizeof(board_id)] = '\0';
	}

	if (!is_valid_board_info(board_id, sizeof(board_id))){
		printf("Error, Invalid board id\n");
		memset(board_id, '0', sizeof(board_id)-1);
		board_id[sizeof(board_id)] = '\0';
	}
        board_id[IDME_MAX_BOARD_ID_LEN]=0;
        printf("idme board id: %s\n", board_id);
	return board_id;
}

u8 *idme_get_board_serial(void)
{
        memset(board_serial, 0, sizeof board_serial);
	if(idme_get_var("serial", board_serial, sizeof(board_serial)) !=0){
		printf("Error, idme_get_board_serial failed.\n");
		memset(board_serial, '0', sizeof(board_serial)-1);
		board_serial[sizeof(board_serial)] = '\0';
	}
	if (!is_valid_board_info(board_serial, sizeof(board_serial))){
		printf("Error, invalid board serial number\n");
		memset(board_serial, '0', sizeof(board_serial)-1);
		board_serial[sizeof(board_serial)] = '\0';
	}
        board_serial[IDME_MAX_BOARD_SERIAL_LEN]=0;
	return board_serial;
}

int idme_update_var(const char *name, const char *value)
{
	int ret = 0, sz;
	int block, offset, size;

	const struct nvram_t *nv;

	nv = idme_find_var(name);
	if (!nv) {
		printf("Error! Couldn't find NV variable %s\n", name);
		return -1;
	}

	block = (nv->offset + IDME_BASE_ADDR) / CONFIG_MMC_BLOCK_SIZE;
	offset = (nv->offset + IDME_BASE_ADDR) % CONFIG_MMC_BLOCK_SIZE;
	sz = size = nv->size;

	/* switch to boot partition */
	if (mmc_sw_part(CONFIG_MMC_BOOTFLASH, CONFIG_IDME_PARTITION_NUM) != 1) {
		printf("ERROR: couldn't switch to boot partition\n");
		return -1;
	}

	ret = mmc_read(CONFIG_MMC_BOOTFLASH, block, gblock_buff, CONFIG_MMC_BLOCK_SIZE);
	if (ret != 1) {
		printf("Error! Couldn't read NV variables. (%d)\n", ret);
		goto out;
	}

	// clear out old value
	memset(gblock_buff + offset, 0, size);

	if (value != NULL) {
		// copy in new value
                if (strncmp("postmode", name, strlen("postmode")) != 0){
                        sz = min(size, strlen(value));
                }
		memcpy(gblock_buff + offset, value, sz);
	}

	ret = mmc_write(CONFIG_MMC_BOOTFLASH, gblock_buff, block, CONFIG_MMC_BLOCK_SIZE);
	if (ret != 1) {
		printf("Error! Couldn't write NV variables. (%d)\n", ret);
		/* need to switch back to user partition even on error */
	}

 out:

	if (mmc_sw_part(CONFIG_MMC_BOOTFLASH,0) != 1 || ret != 1) {
		printf("ERROR: couldn't switch back to user partition\n");
		return -1;
	}

	return ret;
}


/*
  return board type string. Such as Tate, Jem or Bowser
 */
int idme_get_board_type_rev_string(char *bt)
{
        int ret = 0;
        char *pBd = idme_get_board_id();
        printf("idme_get_board_type_rev_string\n");
        if ((pBd) && (pBd[0] != '\0') && bt){
                if (!strncmp(pBd, "000", 3)){
                        strncpy(bt, "Bowser-Dummy-", strlen("Bowser-Dummy-"));
                        bt += strlen("Bowser-Dummy-");
                }else if(!strncmp(pBd, "001", 3)){
                        strncpy(bt, "Jem-PreEVT2-Dummy-", strlen("Jem-PreEVT2-Dummy-"));
                        bt += strlen("Jem-PreEVT2-Dummy-");
                }else if (!strncmp(pBd, "801", 3) ||
                         !strncmp(pBd, "802", 3)){
		  if (!strncmp(&pBd[3], "01", 2)){
		    strncpy(bt, "Jem-EVT2-Dummy-", strlen("Jem-EVT2-Dummy-"));
		    bt += strlen("Jem-EVT2-Dummy-");
		  }else if (!strncmp(&pBd[3], "02",2)){
		    strncpy(bt, "Jem-EVT3-Eng-", strlen("Jem-EVT3-Eng-"));
		    bt += strlen("Jem-EVT3-Eng-");
		  }else if (!strncmp(&pBd[3], "03",2)){
		    strncpy(bt, "Jem-DVT-Eng-", strlen("Jem-DVT-Eng-"));
		    bt += strlen("Jem-DVT-Eng-");
		  }else if (!strncmp(&pBd[3], "04",2)){
		    strncpy(bt, "Jem-PVT-Prod-", strlen("Jem-PVT-Prod-"));
		    bt += strlen("Jem-PVT-Prod-");
		  }else{
		    goto unknown_board;
		  }
                }else if (!strncmp(pBd, "002", 3)){
                        strncpy(bt, "Tate-PreEVT2.1-Dummy-", strlen("Tate-PreEVT2.1-Dummy-"));
                        bt += strlen("Tate-PreEVT2.1-Dummy-");
                }else if (!strncmp(pBd, "803", 3)){
                        strncpy(bt, "Tate-PostEVT2.1-Eng-", strlen("Tate-PostEVT2.1-Eng-"));
                        bt += strlen("Tate-PostEVT2.1-Eng-");
                }else if (!strncmp(pBd, "804", 3)){
                        strncpy(bt, "Radley-", strlen("Radley-"));
                        bt += strlen("Radley-");
                }else {
                        goto unknown_board;
                }
		/* append the revision */
                strncpy(bt, &pBd[3], 2);
                return ret;
        }

 unknown_board:
        strncpy(bt, "unknown", strlen("unknown"));
        return ret;

}

int do_idme (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if (argc == 1 || strncmp(argv[1], "?", 1) == 0) {
		return idme_print_vars_ex();
	} else if (argc <= 3) {
                char *value = NULL;
		if (argc == 3) {
                        unsigned int bcount = 0, len;
                        /* treat setting boot count separately */
                        if (strncmp(argv[1], "setcount", strlen("setcount")) == 0){
                                len = strlen(argv[2]);
                                bcount =  simple_atoi(argv[2], argv[2]+len);
                                return idme_set_boot_count(bcount);
                        } else if (strncmp(argv[2], "clean", strlen("clean")) == 0){
                                value = NULL;
                                printf("clearing '%s'\n", argv[1]);
                        } else {
                                value = argv[2];
                                printf("setting '%s' to '%s'\n", argv[1], argv[2]);
                        }
		}else if (argc == 2){
                        if (strncmp(argv[1], "version", strlen("version")) == 0){
                                printf("set the idme version\n");
                                return idme_update_var(argv[1], IDME_VERSION_STR);
                        }else if (strncmp(argv[1], "magic", strlen("magic")) == 0){
                                return idme_write_magic_size();
                        }else if (strncmp(argv[1], "clean", strlen("clean")) == 0){
                                return idme_clean();
                        }else if (strncmp(argv[1], "count", strlen("count")) == 0){
                                return idme_boot_count();
                        }else {
                                printf ("Usage:\n%s", cmdtp->usage);
                                return 1;
                        }
                }

                return idme_update_var(argv[1], value);

	}

	printf ("Usage:\n%s", cmdtp->usage);
	return 1;
}

/***************************************************/
U_BOOT_CMD(
	   idme,3,1,do_idme,
	   "idme    - Set nv ram variables\n",
	   "idme <var> <value>\n"
	   "    - set a variable value\n"
	   "idme ?"
	   "    - print out known values\n"
	   );

#endif /* CONFIG_ENABLE_IDME */
