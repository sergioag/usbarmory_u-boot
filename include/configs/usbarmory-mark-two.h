/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * USB armory Mk II board configuration settings
 * https://github.com/usbarmory/usbarmory
 *
 * Copyright (C) 2019, WithSecure Corporation
 * Andrej Rosano <andrej.rosano@withsecure.com>
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/imx-regs.h>
#include "mx6_common.h"

#define CONFIG_BOARD_EARLY_INIT_F

/* UART */
#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE	UART2_BASE

/* USB */
#define CONFIG_MXC_USB_PORTSC	(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_USBD_HS

/* I2C */
#define CONFIG_SYS_I2C_SPEED 100000
#define CONFIG_SYS_I2C_MXC_I2C1

/* Boot parameters */
#define USBARMORY_FIT_PATH	"/boot/usbarmory.itb"
#define USBARMORY_FIT_ADDR	"0x80800000"
#define USBARMORY_FIT_CONF	"conf-1"
#define USBARMORY_FIT_SIZE	"0x4000000"

#ifdef CONFIG_SYS_BOOT_DEV_MICROSD
#define USBARMORY_BOOT_DEV	"0"
#define BOOT_TARGET_DEVICES(func) func(MMC, mmc, 0)
#elif CONFIG_SYS_BOOT_DEV_EMMC
#define USBARMORY_BOOT_DEV	"1"
#define BOOT_TARGET_DEVICES(func) func(MMC, mmc, 1)
#endif

/* Bootargs */

#define CONFIG_USE_BOOTARGS
#define CONFIG_BOOTARGS "console=ttymxc1,115200 root=/dev/mmcblk" USBARMORY_BOOT_DEV "p1 rootwait rw"

#define CONFIG_HOSTNAME		"usbarmory"
#define CONFIG_SYS_CBSIZE	512

/* scripts */

#define BOOTENV_CHECK_SILICON_REVISION								\
	"check_silicon_revision="								\
		"echo *** Checking Silicon Revision *** ; "					\
		"setexpr USB_ANALOG_DIGPROG *0x20c8260 ; "					\
		"setexpr imx6_family $USB_ANALOG_DIGPROG '/' 0x10000 ; "			\
		"setexpr silicon_revision $USB_ANALOG_DIGPROG '&' 0xffff ; "			\
		"if itest $imx6_family == 0x64; then "						\
			"if itest $silicon_revision < 0x0002; then "				\
				"echo ; "							\
				"echo \"***********************************************\" ; "	\
				"echo \"* WARNING                                     *\" ; "	\
				"echo \"* i.MX6UltraLight with Silicon Revision < 1.2 *\" ; "	\
				"echo \"* Important security fixes are missing        *\" ; "	\
				"echo \"***********************************************\" ; "	\
				"echo ; "							\
			"fi ; "									\
		"fi ; "										\
		"if itest $imx6_family == 0x65; then "						\
			"if itest $silicon_revision < 0x0001; then "				\
				"echo ; "							\
				"echo \"********************************************\" ; "	\
				"echo \"* WARNING                                  *\" ; "	\
				"echo \"* i.MX6ULL/ULZ with Silicon Revision < 1.1 *\" ; "	\
				"echo \"* Important security fixes are missing     *\" ; "	\
				"echo \"********************************************\" ; "	\
				"echo ; "							\
			"fi ; "									\
		"fi\0"

#define BOOTENV_CHECK_OTPMK						\
	"check_otpmk="							\
		"echo *** Checking OTPMK *** ; "			\
		"env set check_otpmk_var 0 ; "				\
		"setexpr SNVS_HPSR *0x020cc014 ; "			\
		"setexpr test $SNVS_HPSR '&' 0x08000000 ; "		\
		"if itest $test != 0; then "				\
			"env set check_otpmk_var 1 ; "			\
			"echo ; "					\
			"echo \"**************************\" ; "	\
			"echo \"* WARNING: OTPMK is zero *\" ; "	\
			"echo \"**************************\" ; "	\
			"echo ; "					\
		"fi ; "							\
		"setexpr test $SNVS_HPSR '&' 0x01FF0000 ; "		\
		"if itest $test != 0; then "				\
			"env set check_otpmk_var 1 ; "			\
			"echo ; "					\
			"echo \"******************************************\" ; "	\
			"echo \"* WARNING: OTPMK_SYNDROME error detected *\" ; "	\
			"echo \"******************************************\" ; "	\
			"echo ; "					\
		"fi ; "							\
		"setexpr test $SNVS_HPSR '&' 0xF00 ; "			\
		"if itest $test != 0xD00; then "			\
			"env set check_otpmk_var 1 ; "			\
			"echo ; "					\
			"echo \"***************************************\" ; "	\
			"echo \"* WARNING: Device not in TRUSTED mode *\" ; "	\
			"echo \"***************************************\" ; "	\
			"echo ; "					\
		"fi ; "							\
		"if itest $check_otpmk_var != 0; then "			\
			"echo *** Unable to continue. Resetting in 60s *** ; "	\
			"sleep 60 ; "						\
			"reset ; "						\
		"fi\0"

#define BOOTENV_UMS			\
	"start_ums="			\
		"ums 0 mmc ${mmcdev}\0"

#define BOOTENV_TFTP							\
	"start_tftp="							\
		"dhcp ${kernel_addr_r} ${serverip}:${bootfile} ; "	\
		"dhcp ${fdt_addr_r} ${serverip}:${fdtfile} ; "		\
		"bootm ${kernel_addr_r} - ${fdt_addr_r}\0"

#define BOOTENV_VERIFIED_OPEN												\
	"start_verified_open="												\
		"ext2load mmc ${mmcdev}:1 " USBARMORY_FIT_ADDR " " USBARMORY_FIT_PATH " " USBARMORY_FIT_SIZE " ; "	\
		"bootm " USBARMORY_FIT_ADDR "#" USBARMORY_FIT_CONF "\0"

#define BOOTENV_NORMAL								\
	"start_normal=run distro_bootcmd ; "					\
		"ext2load mmc ${mmcdev}:1 ${kernel_addr_r} /boot/${bootfile} ; "\
		"ext2load mmc ${mmcdev}:1 ${fdt_addr_r} /boot/${fdtfile} ; "	\
		"bootz ${kernel_addr_r} - ${fdt_addr_r}\0"

#define BOOTENV_OPTEE								\
	"start_optee="					\
		"ext2load mmc ${mmcdev}:1 ${optee_addr_r} /boot/${opteefile} ; "\
		"ext2load mmc ${mmcdev}:1 ${kernel_addr_r} /boot/${bootfile} ; "\
		"ext2load mmc ${mmcdev}:1 ${fdt_addr_r} /boot/${fdtfile} ; "	\
		"bootm ${optee_addr_r} - ${fdt_addr_r}\0"

/* Boot modes */

#ifdef CONFIG_SYS_BOOT_MODE_UMS

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND	"run start_ums"

#elif CONFIG_SYS_BOOT_MODE_VERIFIED_OPEN

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND	"run start_verified_open"

#elif CONFIG_SYS_BOOT_MODE_TFTP

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND	"run start_tftp"

#elif CONFIG_SYS_BOOT_MODE_NORMAL

#include <config_distro_bootcmd.h>
#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND	"run start_normal"

#elif CONFIG_SYS_BOOT_MODE_VERIFIED_LOCKED

#undef CONFIG_CMDLINE
#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY -2

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND "dummy"

#endif

/* Custom environment variables */

#ifndef BOOTENV
#define BOOTENV
#endif

#define CONFIG_EXTRA_ENV_SETTINGS		\
	"kernel_addr_r=0x80800000\0"		\
	"fdt_addr_r=0x82000000\0"		\
	"scriptaddr=0x80800000\0"		\
	"ramdisk_addr_r=0x83000000\0"		\
	"optee_addr_r=0x83000000\0"		\
	"bootfile=zImage\0"			\
	"opteefile=uTee\0"			\
	"fdtfile=imx6ulz-usbarmory.dtb\0"	\
	"mmcdev=" USBARMORY_BOOT_DEV "\0"	\
	"ethact=usb_ether\0"			\
	"cdc_connect_timeout=60\0"		\
	"usbnet_devaddr=1a:55:89:a2:69:52\0"	\
	"usbnet_hostaddr=1a:55:89:a2:69:51\0"	\
	BOOTENV_CHECK_SILICON_REVISION		\
	BOOTENV_CHECK_OTPMK			\
	BOOTENV_NORMAL				\
	BOOTENV_OPTEE				\
	BOOTENV_TFTP				\
	BOOTENV_UMS				\
	BOOTENV_VERIFIED_OPEN			\
	BOOTENV

/* Physical Memory Map */
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#endif	/* __CONFIG_H */
