/* arch/arm/mach-msm/board-deluxe_ub1-keypad.c
 * Copyright (C) 2010 HTC Corporation.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio_event.h>
#include <linux/gpio.h>
#include <linux/keyreset.h>
#include <asm/mach-types.h>
#include <mach/board_htc.h>
#include <mach/gpio.h>
#include <linux/mfd/pm8xxx/pm8921.h>
#include "board-deluxe_ub1.h"

#undef MODULE_PARAM_PREFIX
#define MODULE_PARAM_PREFIX "board_deluxe_ub1."


static struct gpio_event_direct_entry deluxe_ub1_keypad_map[] = {
	{
		.gpio = PWR_KEY_MSMz,
		.code = KEY_POWER,
	},
	{
		.gpio = VOL_DOWNz,
		.code = KEY_VOLUMEDOWN,
	},
	{
		.gpio = VOL_UPz,
		.code = KEY_VOLUMEUP,
	},
};

static uint32_t matirx_inputs_gpio_table[] = {
	GPIO_CFG(PWR_KEY_MSMz, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
		 GPIO_CFG_2MA),
	GPIO_CFG(VOL_DOWNz, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
		 GPIO_CFG_2MA),
	GPIO_CFG(VOL_UPz, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
		 GPIO_CFG_2MA),
};

static void deluxe_ub1_direct_inputs_gpio(void)
{
	gpio_tlmm_config(matirx_inputs_gpio_table[0], GPIO_CFG_ENABLE);
	gpio_tlmm_config(matirx_inputs_gpio_table[1], GPIO_CFG_ENABLE);
	gpio_tlmm_config(matirx_inputs_gpio_table[2], GPIO_CFG_ENABLE);

	return;
}

uint32_t hw_clr_gpio_table[] = {
	GPIO_CFG(RESET_EN_CLRz, 0, GPIO_CFG_INPUT,
		GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
	GPIO_CFG(RESET_EN_CLRz, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
};

static void deluxe_ub1_clear_hw_reset(void)
{
	printk(KERN_INFO "[KEY] %s ++++++\n", __func__);
	gpio_tlmm_config(hw_clr_gpio_table[1], GPIO_CFG_ENABLE);
	gpio_set_value(RESET_EN_CLRz, 0);
	msleep(100);
	gpio_tlmm_config(hw_clr_gpio_table[0], GPIO_CFG_ENABLE);
	printk(KERN_INFO "[KEY] %s ------\n", __func__);
}

static struct gpio_event_input_info deluxe_ub1_keypad_power_info = {
	.info.func = gpio_event_input_func,
	.flags = GPIOEDF_PRINT_KEYS,
	.type = EV_KEY,
#if BITS_PER_LONG != 64 && !defined(CONFIG_KTIME_SCALAR)
	.debounce_time.tv.nsec = 20 * NSEC_PER_MSEC,
# else
	.debounce_time.tv64 = 20 * NSEC_PER_MSEC,
# endif
	.keymap = deluxe_ub1_keypad_map,
	.keymap_size = ARRAY_SIZE(deluxe_ub1_keypad_map),
	.setup_input_gpio = deluxe_ub1_direct_inputs_gpio,
	.clear_hw_reset = deluxe_ub1_clear_hw_reset,
};

static struct gpio_event_info *deluxe_ub1_keypad_info[] = {
	&deluxe_ub1_keypad_power_info.info,
};

static struct gpio_event_platform_data deluxe_ub1_keypad_data = {
	.name = "keypad_8960",
	.info = deluxe_ub1_keypad_info,
	.info_count = ARRAY_SIZE(deluxe_ub1_keypad_info),
};

static struct platform_device deluxe_ub1_keypad_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev		= {
		.platform_data	= &deluxe_ub1_keypad_data,
	},
};

static struct keyreset_platform_data deluxe_ub1_reset_keys_pdata = {
	
	.keys_down = {
		KEY_POWER,
		KEY_VOLUMEDOWN,
		KEY_VOLUMEUP,
		0
	},
};

static struct platform_device deluxe_ub1_reset_keys_device = {
	.name = KEYRESET_NAME,
	.dev.platform_data = &deluxe_ub1_reset_keys_pdata,
};

int __init deluxe_ub1_init_keypad(void)
{
	if (platform_device_register(&deluxe_ub1_reset_keys_device))
		printk(KERN_WARNING "%s: register reset key fail\n", __func__);

	return platform_device_register(&deluxe_ub1_keypad_device);
}

