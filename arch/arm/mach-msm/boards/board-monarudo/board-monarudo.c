/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/i2c.h>
#include <linux/mpu.h>
#include <linux/r3gd20.h>
#include <linux/akm8963.h>
#include <linux/bma250.h>
#include <linux/slimbus/slimbus.h>
#include <linux/mfd/wcd9xxx/core.h>
#include <linux/mfd/wcd9xxx/pdata.h>
#include <linux/mfd/pm8xxx/misc.h>
#include <linux/msm_ssbi.h>
#include <linux/spi/spi.h>
#include <linux/dma-mapping.h>
#include <linux/platform_data/qcom_crypto_device.h>
#include <linux/ion.h>
#include <linux/memory.h>
#include <linux/memblock.h>
#include <linux/msm_thermal.h>
#include <linux/i2c/atmel_mxt_ts.h>
#include <linux/cyttsp.h>
#include <linux/gpio_keys.h>
#include <linux/proc_fs.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/hardware/gic.h>
#include <asm/mach/mmc.h>
#include <linux/platform_data/qcom_wcnss_device.h>
#include <linux/synaptics_i2c_rmi.h>
#include <linux/htc_flashlight.h>
#include <mach/board.h>
#include <mach/restart.h>
#include <mach/msm_iomap.h>
#include <mach/ion.h>
#include <linux/usb/msm_hsusb.h>
#include <mach/htc_usb.h>
#include <linux/usb/android_composite.h>
#include <mach/socinfo.h>
#include <mach/msm_spi.h>
#include "timer.h"
#include "devices.h"
#include <mach/gpio.h>
#include <mach/gpiomux.h>
#include <mach/rpm.h>
#ifdef CONFIG_ANDROID_PMEM
#include <linux/android_pmem.h>
#endif
#include <mach/msm_memtypes.h>
#include <linux/bootmem.h>
#include <asm/setup.h>
#include <mach/dma.h>
#include <mach/msm_dsps.h>
#include <mach/msm_bus_board.h>
#include <mach/cpuidle.h>
#include <mach/mdm2.h>
#include <linux/msm_tsens.h>
#include <mach/msm_xo.h>
#include <mach/msm_rtb.h>
#include <linux/fmem.h>
#include <mach/htc_headset_mgr.h>
#include <mach/htc_headset_pmic.h>
#include <mach/htc_headset_one_wire.h>
#include <linux/mfd/pm8xxx/pm8xxx-vibrator-pwm.h>
#include <mach/htc_ramdump.h>

#ifdef CONFIG_PERFLOCK
#include <mach/perflock.h>
#endif


#ifdef CONFIG_BT
#include <mach/msm_serial_hs.h>
#include <mach/htc_bdaddress.h>
#endif

#include <mach/msm_watchdog.h>
#include "board-monarudo.h"
#include "spm.h"
#include <mach/mpm.h>
#include "rpm_resources.h"
#include "pm.h"
#include "pm-boot.h"
#include <mach/board_htc.h>
#include <mach/htc_util.h>
#include <mach/cable_detect.h>
#include "devices-msm8x60.h"
#include <linux/cm3629.h>
#include <linux/pn544.h>
#include <mach/tfa9887.h>
#include <mach/tpa6185.h>
#include <mach/rt5501.h>

#ifdef CONFIG_HTC_BATT_8960
#include "mach/htc_battery_8960.h"
#include "mach/htc_battery_cell.h"
#include "linux/mfd/pm8xxx/pm8921-charger.h"
#endif

#ifdef CONFIG_FB_MSM_HDMI_MHL
#include <mach/mhl.h>
#endif

#ifdef CONFIG_SUPPORT_USB_SPEAKER
#include <linux/pm_qos.h>
#endif

#define MSM_PMEM_ADSP_SIZE         0x7800000
#define MSM_PMEM_AUDIO_SIZE        0x4CF000
#ifdef CONFIG_FB_MSM_HDMI_AS_PRIMARY
#define MSM_PMEM_SIZE 0x8200000 
#else
#define MSM_PMEM_SIZE 0x8200000 
#endif

#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
#define HOLE_SIZE		0x20000
#ifdef CONFIG_MSM_IOMMU
#define MSM_PMEM_KERNEL_EBI1_SIZE  0x280000
#else
#define MSM_PMEM_KERNEL_EBI1_SIZE  0x6400000
#endif

#define MSM_ION_KGSL_SIZE	0x6400000
#define MSM_ION_SF_SIZE		(MSM_PMEM_SIZE + MSM_ION_KGSL_SIZE)
#define MSM_ION_MM_FW_SIZE	(0x200000 - HOLE_SIZE) 
#define MSM_ION_MM_SIZE		MSM_PMEM_ADSP_SIZE
#define MSM_ION_QSECOM_SIZE	0x600000 
#define MSM_ION_MFC_SIZE	SZ_8K
#define MSM_ION_AUDIO_SIZE	MSM_PMEM_AUDIO_SIZE
#define MSM_ION_HEAP_NUM	8

#else
#define MSM_PMEM_KERNEL_EBI1_SIZE  0x110C000
#define MSM_ION_HEAP_NUM	1
#endif

#define APQ8064_FIXED_AREA_START (0xa0000000 - (MSM_ION_MM_FW_SIZE + HOLE_SIZE))
#define MAX_FIXED_AREA_SIZE	0x10000000
#define MSM_MM_FW_SIZE		(0x200000 - HOLE_SIZE)
#define APQ8064_FW_START	APQ8064_FIXED_AREA_START

#ifdef CONFIG_FB_MSM_HDMI_MHL
static int hdmi_enable_5v(int on);
static int hdmi_core_power(int on, int show);
extern void hdmi_hpd_feature(int enable);
#endif

#define TFA9887_I2C_SLAVE_ADDR	(0x68 >> 1)
#define TPA6185_I2C_SLAVE_ADDR	(0xC6 >> 1)
#define RT5501_I2C_SLAVE_ADDR	(0xF0 >> 1)

#define PM8XXX_GPIO_INIT(_gpio, _dir, _buf, _val, _pull, _vin, _out_strength, \
			_func, _inv, _disable) \
{ \
	.gpio	= PM8921_GPIO_PM_TO_SYS(_gpio), \
	.config	= { \
		.direction	= _dir, \
		.output_buffer	= _buf, \
		.output_value	= _val, \
		.pull		= _pull, \
		.vin_sel	= _vin, \
		.out_strength	= _out_strength, \
		.function	= _func, \
		.inv_int_pol	= _inv, \
		.disable_pin	= _disable, \
	} \
}

struct pm8xxx_gpio_init {
	unsigned			gpio;
	struct pm_gpio			config;
};

struct tpa6185_platform_data tpa6185_data={
         .gpio_tpa6185_spk_en = PM8921_GPIO_PM_TO_SYS(10),

};

struct rt5501_platform_data rt5501_data={
         .gpio_rt5501_spk_en = PM8921_GPIO_PM_TO_SYS(10),

};

static struct i2c_board_info msm_i2c_gsbi1_tpa6185_info[] = {
	{
		I2C_BOARD_INFO(TPA6185_I2C_NAME, TPA6185_I2C_SLAVE_ADDR),
		.platform_data = &tpa6185_data,
	},
};


static struct i2c_board_info msm_i2c_gsbi1_rt5501_info[] = {
	{
		I2C_BOARD_INFO( RT5501_I2C_NAME, RT5501_I2C_SLAVE_ADDR),
		.platform_data = &rt5501_data,
	},
};

static struct i2c_board_info msm_i2c_gsbi1_tfa9887_info[] = {
	{
		I2C_BOARD_INFO(TFA9887_I2C_NAME, TFA9887_I2C_SLAVE_ADDR)
	},
};

#define        GPIO_EXPANDER_IRQ_BASE  (PM8821_IRQ_BASE + PM8821_NR_IRQS)
#define        GPIO_EXPANDER_GPIO_BASE (PM8821_MPP_BASE + PM8821_NR_MPPS)
#define        GPIO_EPM_EXPANDER_BASE  GPIO_EXPANDER_GPIO_BASE

enum {
       SX150X_EPM,
};

#ifdef CONFIG_KERNEL_PMEM_EBI_REGION
static unsigned pmem_kernel_ebi1_size = MSM_PMEM_KERNEL_EBI1_SIZE;
static int __init pmem_kernel_ebi1_size_setup(char *p)
{
	pmem_kernel_ebi1_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_kernel_ebi1_size", pmem_kernel_ebi1_size_setup);
#endif

#ifdef CONFIG_ANDROID_PMEM
static unsigned pmem_size = MSM_PMEM_SIZE;
static int __init pmem_size_setup(char *p)
{
	pmem_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_size", pmem_size_setup);

static unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;

static int __init pmem_adsp_size_setup(char *p)
{
	pmem_adsp_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_adsp_size", pmem_adsp_size_setup);

static unsigned pmem_audio_size = MSM_PMEM_AUDIO_SIZE;

static int __init pmem_audio_size_setup(char *p)
{
	pmem_audio_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_audio_size", pmem_audio_size_setup);
#endif

#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.allocator_type = PMEM_ALLOCATORTYPE_ALLORNOTHING,
	.cached = 1,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device monarudo_android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = {.platform_data = &android_pmem_pdata},
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
};
static struct platform_device monarudo_android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};


static struct android_pmem_platform_data monarudo_android_pmem_audio_pdata = {
	.name = "pmem_audio",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device android_pmem_audio_device = {
	.name = "android_pmem",
	.id = 4,
	.dev = { .platform_data = &android_pmem_audio_pdata },
};
#endif 
#endif 

static struct platform_device monarudo_cpu_usage_stats_device = {
	.name = "monarudo_cpu_usage_stats",
	.id = -1,
};

extern int pm_monitor_enabled;  

struct fmem_platform_data apq8064_fmem_pdata = {
};

static struct memtype_reserve apq8064_reserve_table[] __initdata = {
	[MEMTYPE_SMI] = {
	},
	[MEMTYPE_EBI0] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
	[MEMTYPE_EBI1] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
};

#if defined(CONFIG_MSM_RTB)
static struct msm_rtb_platform_data monarudo_rtb_pdata = {
		.buffer_start_addr = MSM_RTB_PHYS,
		.size = MSM_RTB_BUFFER_SIZE,
};

static int __init msm_rtb_set_buffer_size(char *p)
{
       int s;

       s = memparse(p, NULL);
       monarudo_rtb_pdata.size = ALIGN(s, SZ_4K);
       return 0;
}
early_param("msm_rtb_size", msm_rtb_set_buffer_size);


static struct platform_device monarudo_rtb_device = {
       .name           = "msm_rtb",
       .id             = -1,
       .dev            = {
               .platform_data = &monarudo_rtb_pdata,
       },
};
#endif

#ifdef CONFIG_I2C
#define MSM8064_GSBI2_QUP_I2C_BUS_ID 2
#define MSM8064_GSBI3_QUP_I2C_BUS_ID 3

#endif


static void __init size_pmem_devices(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	android_pmem_adsp_pdata.size = pmem_adsp_size;
	android_pmem_pdata.size = pmem_size;
	android_pmem_audio_pdata.size = MSM_PMEM_AUDIO_SIZE;
#endif 
#endif 
}

#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
static void __init reserve_memory_for(struct android_pmem_platform_data *p)
{
	apq8064_reserve_table[p->memory_type].size += p->size;
}
#endif 
#endif 

static void __init reserve_pmem_memory(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	reserve_memory_for(&android_pmem_adsp_pdata);
	reserve_memory_for(&android_pmem_pdata);
	reserve_memory_for(&android_pmem_audio_pdata);
#endif 
	apq8064_reserve_table[MEMTYPE_EBI1].size += pmem_kernel_ebi1_size;
#endif 
}

static int monarudo_paddr_to_memtype(unsigned int paddr)
{
	return MEMTYPE_EBI1;
}

#define FMEM_ENABLED 0

#ifdef CONFIG_ION_MSM
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct ion_cp_heap_pdata cp_mm_monarudo_ion_pdata = {
	.permission_type = IPT_TYPE_MM_CARVEOUT,
	.align = PAGE_SIZE,
	.reusable = FMEM_ENABLED,
	.mem_is_fmem = FMEM_ENABLED,
	.fixed_position = FIXED_MIDDLE,
};

static struct ion_cp_heap_pdata cp_mfc_monarudo_ion_pdata = {
	.permission_type = IPT_TYPE_MFC_SHAREDMEM,
	.align = PAGE_SIZE,
	.reusable = 0,
	.mem_is_fmem = FMEM_ENABLED,
	.fixed_position = FIXED_HIGH,
};

static struct ion_co_heap_pdata co_monarudo_ion_pdata = {
	.adjacent_mem_id = INVALID_HEAP_ID,
	.align = PAGE_SIZE,
	.mem_is_fmem = 0,
};

static struct ion_co_heap_pdata fw_co_monarudo_ion_pdata = {
	.adjacent_mem_id = ION_CP_MM_HEAP_ID,
	.align = SZ_128K,
	.mem_is_fmem = FMEM_ENABLED,
	.fixed_position = FIXED_LOW,
};
#endif

static struct ion_platform_data ion_pdata = {
	.nr = MSM_ION_HEAP_NUM,
	.heaps = {
		{
			.id	= ION_SYSTEM_HEAP_ID,
			.type	= ION_HEAP_TYPE_SYSTEM,
			.name	= ION_VMALLOC_HEAP_NAME,
		},
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
		{
			.id	= ION_CP_MM_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_MM_HEAP_NAME,
			.size	= MSM_ION_MM_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &cp_mm_monarudo_ion_pdata,
		},
		{
			.id	= ION_MM_FIRMWARE_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_MM_FIRMWARE_HEAP_NAME,
			.size	= MSM_ION_MM_FW_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &fw_co_monarudo_ion_pdata,
		},
		{
			.id	= ION_CP_MFC_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_MFC_HEAP_NAME,
			.size	= MSM_ION_MFC_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &cp_mfc_monarudo_ion_pdata,
		},
		{
			.id	= ION_SF_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_SF_HEAP_NAME,
			.size	= MSM_ION_SF_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &co_monarudo_ion_pdata,
		},
		{
			.id	= ION_IOMMU_HEAP_ID,
			.type	= ION_HEAP_TYPE_IOMMU,
			.name	= ION_IOMMU_HEAP_NAME,
		},
		{
			.id	= ION_QSECOM_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_QSECOM_HEAP_NAME,
			.size	= MSM_ION_QSECOM_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &co_monarudo_ion_pdata,
		},
		{
			.id	= ION_AUDIO_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_AUDIO_HEAP_NAME,
			.size	= MSM_ION_AUDIO_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &co_monarudo_ion_pdata,
		},
#endif
	}
};

static struct platform_device monarudo_ion_dev = {
	.name = "ion-msm",
	.id = 1,
	.dev = { .platform_data = &ion_pdata },
};
#endif

static struct platform_device apq8064_fmem_device = {
	.name = "fmem",
	.id = 1,
	.dev = { .platform_data = &apq8064_fmem_pdata },
};

static void __init reserve_mem_for_ion(enum ion_memory_types mem_type,
				      unsigned long size)
{
	apq8064_reserve_table[mem_type].size += size;
}

static void __init apq8064_reserve_fixed_area(unsigned long fixed_area_size)
{
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	int ret;

	if (fixed_area_size > MAX_FIXED_AREA_SIZE)
		panic("fixed area size is larger than %dM\n",
			MAX_FIXED_AREA_SIZE >> 20);

	reserve_info->fixed_area_size = fixed_area_size;
	reserve_info->fixed_area_start = APQ8064_FW_START;

	ret = memblock_remove(reserve_info->fixed_area_start,
		reserve_info->fixed_area_size);
	BUG_ON(ret);
#endif
}

static void __init reserve_ion_memory(void)
{
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	unsigned int i;
	unsigned int reusable_count = 0;
	unsigned int fixed_size = 0;
	unsigned int fixed_low_size, fixed_middle_size, fixed_high_size;
	unsigned long fixed_low_start, fixed_middle_start, fixed_high_start;

	apq8064_fmem_pdata.size = 0;
	apq8064_fmem_pdata.reserved_size_low = 0;
	apq8064_fmem_pdata.reserved_size_high = 0;
	apq8064_fmem_pdata.align = PAGE_SIZE;
	fixed_low_size = 0;
	fixed_middle_size = 0;
	fixed_high_size = 0;

	for (i = 0; i < ion_pdata.nr; ++i) {
		const struct ion_platform_heap *heap =
			&(ion_pdata.heaps[i]);

		if (heap->type == ION_HEAP_TYPE_CP && heap->extra_data) {
			struct ion_cp_heap_pdata *data = heap->extra_data;

			reusable_count += (data->reusable) ? 1 : 0;

			if (data->reusable && reusable_count > 1) {
				pr_err("%s: Too many heaps specified as "
					"reusable. Heap %s was not configured "
					"as reusable.\n", __func__, heap->name);
				data->reusable = 0;
			}
		}
	}

	for (i = 0; i < ion_pdata.nr; ++i) {
		const struct ion_platform_heap *heap =
			&(ion_pdata.heaps[i]);

		if (heap->extra_data) {
			int fixed_position = NOT_FIXED;
			int mem_is_fmem = 0;

			switch (heap->type) {
			case ION_HEAP_TYPE_CP:
				mem_is_fmem = ((struct ion_cp_heap_pdata *)
					heap->extra_data)->mem_is_fmem;
				fixed_position = ((struct ion_cp_heap_pdata *)
					heap->extra_data)->fixed_position;
				break;
			case ION_HEAP_TYPE_CARVEOUT:
				mem_is_fmem = ((struct ion_co_heap_pdata *)
					heap->extra_data)->mem_is_fmem;
				fixed_position = ((struct ion_co_heap_pdata *)
					heap->extra_data)->fixed_position;
				break;
			default:
				break;
			}

			if (fixed_position != NOT_FIXED)
				fixed_size += heap->size;
			else
				reserve_mem_for_ion(MEMTYPE_EBI1, heap->size);

			if (fixed_position == FIXED_LOW)
				fixed_low_size += heap->size;
			else if (fixed_position == FIXED_MIDDLE)
				fixed_middle_size += heap->size;
			else if (fixed_position == FIXED_HIGH)
				fixed_high_size += heap->size;

			if (mem_is_fmem)
				apq8064_fmem_pdata.size += heap->size;
		}
	}

	if (!fixed_size)
		return;

	if (apq8064_fmem_pdata.size) {
		apq8064_fmem_pdata.reserved_size_low = fixed_low_size +
								HOLE_SIZE;
		apq8064_fmem_pdata.reserved_size_high = fixed_high_size;
	}

	fixed_size = (fixed_size + HOLE_SIZE + SECTION_SIZE - 1)
		& SECTION_MASK;
	apq8064_reserve_fixed_area(fixed_size);

	fixed_low_start = APQ8064_FIXED_AREA_START;
	fixed_middle_start = fixed_low_start + fixed_low_size + HOLE_SIZE;
	fixed_high_start = fixed_middle_start + fixed_middle_size;

	for (i = 0; i < ion_pdata.nr; ++i) {
		struct ion_platform_heap *heap = &(ion_pdata.heaps[i]);

		if (heap->extra_data) {
			int fixed_position = NOT_FIXED;
			struct ion_cp_heap_pdata *pdata = NULL;

			switch (heap->type) {
			case ION_HEAP_TYPE_CP:
				pdata =
				(struct ion_cp_heap_pdata *)heap->extra_data;
				fixed_position = pdata->fixed_position;
				break;
			case ION_HEAP_TYPE_CARVEOUT:
				fixed_position = ((struct ion_co_heap_pdata *)
					heap->extra_data)->fixed_position;
				break;
			default:
				break;
			}

			switch (fixed_position) {
			case FIXED_LOW:
				heap->base = fixed_low_start;
				break;
			case FIXED_MIDDLE:
				heap->base = fixed_middle_start;
				pdata->secure_base = fixed_middle_start
								- HOLE_SIZE;
				pdata->secure_size = HOLE_SIZE + heap->size;
				break;
			case FIXED_HIGH:
				heap->base = fixed_high_start;
				break;
			default:
				break;
			}
		}
	}
#endif
}

static struct resource mdm_resources[] = {
	{
		.start	= MDM2AP_ERR_FATAL,
		.end	= MDM2AP_ERR_FATAL,
		.name	= "MDM2AP_ERRFATAL",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= AP2MDM_ERR_FATAL,
		.end	= AP2MDM_ERR_FATAL,
		.name	= "AP2MDM_ERRFATAL",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= MDM2AP_STATUS,
		.end	= MDM2AP_STATUS,
		.name	= "MDM2AP_STATUS",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= AP2MDM_STATUS,
		.end	= AP2MDM_STATUS,
		.name	= "AP2MDM_STATUS",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= AP2MDM_PON_RESET_N,
		.end	= AP2MDM_PON_RESET_N,
		.name	= "AP2MDM_PMIC_RESET_N",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= MDM2AP_HSIC_READY_XA_XB,
		.end	= MDM2AP_HSIC_READY_XA_XB,
		.name	= "MDM2AP_HSIC_READY",
		.flags	= IORESOURCE_IO,
	},
	{
		.start  = AP2MDM_WAKEUP_XA_XB,
		.end    = AP2MDM_WAKEUP_XA_XB,
		.name   = "AP2MDM_WAKEUP",
		.flags  = IORESOURCE_IO,
	},
	{
		.start  = APQ2MDM_IPC1,
		.end    = APQ2MDM_IPC1,
		.name   = "AP2MDM_IPC1",
		.flags  = IORESOURCE_IO,
	},
};

static struct platform_device mdm_8064_device = {
	.name		= "mdm2_modem",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(mdm_resources),
	.resource	= mdm_resources,
};


#ifdef CONFIG_BT
static struct msm_serial_hs_platform_data msm_uart_dm6_pdata = {
	.inject_rx_on_wakeup = 0,

	
	.bt_wakeup_pin = PM8921_GPIO_PM_TO_SYS(BT_WAKE_XC),
	.host_wakeup_pin = PM8921_GPIO_PM_TO_SYS(BT_HOST_WAKE_XC),
};

static struct platform_device monarudo_rfkill = {
	.name = "monarudo_rfkill",
	.id = -1,
};
#endif

static void __init reserve_mdp_memory(void)
{
	monarudo_mdp_writeback(apq8064_reserve_table);
}

static void __init monarudo_calculate_reserve_sizes(void)
{
	size_pmem_devices();
	reserve_pmem_memory();
	reserve_ion_memory();
	reserve_mdp_memory();
}


static struct reserve_info monarudo_reserve_info __initdata = {
	.memtype_reserve_table = apq8064_reserve_table,
	.calculate_reserve_sizes = monarudo_calculate_reserve_sizes,
	.reserve_fixed_area = apq8064_reserve_fixed_area,
	.paddr_to_memtype = monarudo_paddr_to_memtype,
};

static int monarudo_memory_bank_size(void)
{
	return 1<<29;
}

static void __init locate_unstable_memory(void)
{
	struct membank *mb = &meminfo.bank[meminfo.nr_banks - 1];
	unsigned long bank_size;
	unsigned long low, high;

	bank_size = monarudo_memory_bank_size();
	low = meminfo.bank[0].start;
	high = mb->start + mb->size;

	
	if (high < mb->start)
		high = -PAGE_SIZE;

	low &= ~(bank_size - 1);

	if (high - low <= bank_size)
		goto no_dmm;

#ifdef CONFIG_ENABLE_DMM
	monarudo_reserve_info.low_unstable_address = mb->start -
					MIN_MEMORY_BLOCK_SIZE + mb->size;
	monarudo_reserve_info.max_unstable_size = MIN_MEMORY_BLOCK_SIZE;

	monarudo_reserve_info.bank_size = bank_size;
	pr_info("low unstable address %lx max size %lx bank size %lx\n",
		monarudo_reserve_info.low_unstable_address,
		monarudo_reserve_info.max_unstable_size,
		monarudo_reserve_info.bank_size);
	return;
#endif
no_dmm:
	monarudo_reserve_info.low_unstable_address = high;
	monarudo_reserve_info.max_unstable_size = 0;
}

int __init parse_tag_memsize(const struct tag *tags);
static unsigned int mem_size_mb;

static void __init monarudo_reserve(void)
{
	if (mem_size_mb == 64)
		return;
	
	msm_reserve();
	if (apq8064_fmem_pdata.size) {
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
		if (reserve_info->fixed_area_size) {
			apq8064_fmem_pdata.phys =
				reserve_info->fixed_area_start + MSM_MM_FW_SIZE;
			pr_info("mm fw at %lx (fixed) size %x\n",
				reserve_info->fixed_area_start, MSM_MM_FW_SIZE);
			pr_info("fmem start %lx (fixed) size %lx\n",
				apq8064_fmem_pdata.phys,
				apq8064_fmem_pdata.size);
		}
#endif
	}
}

static void __init place_movable_zone(void)
{
#ifdef CONFIG_ENABLE_DMM
	movable_reserved_start = monarudo_reserve_info.low_unstable_address;
	movable_reserved_size = monarudo_reserve_info.max_unstable_size;
	pr_info("movable zone start %lx size %lx\n",
		movable_reserved_start, movable_reserved_size);
#endif
}

static void __init monarudo_early_reserve(void)
{
	reserve_info = &monarudo_reserve_info;
	locate_unstable_memory();
	place_movable_zone();
}

#ifdef CONFIG_HTC_BATT_8960
static int pm8921_is_wireless_charger(void)
{
	int usb_in, dc_in;

	usb_in = pm8921_is_usb_chg_plugged_in();
	dc_in = pm8921_is_dc_chg_plugged_in();
	pr_info("%s: usb_in=%d, dc_in=%d\n", __func__, usb_in, dc_in);
	if (!usb_in && dc_in)
		return 1;
	else
		return 0;
}

#ifdef CONFIG_HTC_PNPMGR
extern int pnpmgr_battery_charging_enabled(int charging_enabled);
#endif 
static int critical_alarm_voltage_mv[] = {3000, 3200, 3400};

static struct htc_battery_platform_data htc_battery_pdev_data = {
	.guage_driver = 0,
	.chg_limit_active_mask = HTC_BATT_CHG_LIMIT_BIT_TALK |
								HTC_BATT_CHG_LIMIT_BIT_NAVI,
	.critical_low_voltage_mv = 3100,
	.critical_alarm_vol_ptr = critical_alarm_voltage_mv,
	.critical_alarm_vol_cols = sizeof(critical_alarm_voltage_mv) / sizeof(int),
	.overload_vol_thr_mv = 4000,
	.overload_curr_thr_ma = 0,
	
	.icharger.name = "pm8921",
	.icharger.get_charging_source = pm8921_get_charging_source,
	.icharger.get_charging_enabled = pm8921_get_charging_enabled,
	.icharger.set_charger_enable = pm8921_charger_enable,
	.icharger.set_pwrsrc_enable = pm8921_pwrsrc_enable,
	.icharger.set_pwrsrc_and_charger_enable =
						pm8921_set_pwrsrc_and_charger_enable,
	.icharger.set_limit_charge_enable = pm8921_limit_charge_enable,
	.icharger.is_ovp = pm8921_is_charger_ovp,
	.icharger.is_batt_temp_fault_disable_chg =
						pm8921_is_batt_temp_fault_disable_chg,
	.icharger.is_under_rating = pm8921_is_pwrsrc_under_rating,
	.icharger.charger_change_notifier_register =
						cable_detect_register_notifier,
	.icharger.dump_all = pm8921_dump_all,
	.icharger.get_attr_text = pm8921_charger_get_attr_text,
	.icharger.max_input_current =pm8921_set_hsml_target_ma,
	.icharger.is_safty_timer_timeout = pm8921_is_chg_safety_timer_timeout,
	.icharger.is_battery_full_eoc_stop = pm8921_is_batt_full_eoc_stop,
	
	.igauge.name = "pm8921",
	.igauge.get_battery_voltage = pm8921_get_batt_voltage,
	.igauge.get_battery_current = pm8921_bms_get_batt_current,
	.igauge.get_battery_temperature = pm8921_get_batt_temperature,
	.igauge.get_battery_id = pm8921_get_batt_id,
	.igauge.get_battery_soc = pm8921_bms_get_batt_soc,
	.igauge.get_battery_cc = pm8921_bms_get_batt_cc,
	.igauge.is_battery_temp_fault = pm8921_is_batt_temperature_fault,
	.igauge.is_battery_full = pm8921_is_batt_full,
	.igauge.get_attr_text = pm8921_gauge_get_attr_text,
	.igauge.register_lower_voltage_alarm_notifier =
						pm8xxx_batt_lower_alarm_register_notifier,
	.igauge.enable_lower_voltage_alarm = pm8xxx_batt_lower_alarm_enable,
	.igauge.set_lower_voltage_alarm_threshold =
						pm8xxx_batt_lower_alarm_threshold_set,
	
#ifdef CONFIG_HTC_PNPMGR
	.notify_pnpmgr_charging_enabled = pnpmgr_battery_charging_enabled,
#endif 
};
static struct platform_device htc_battery_pdev = {
	.name = "htc_battery",
	.id = -1,
	.dev    = {
		.platform_data = &htc_battery_pdev_data,
	},
};

static struct pm8921_charger_batt_param chg_batt_params[] = {
	
	[0] = {
		.max_voltage = 4200,
		.cool_bat_voltage = 4200,
		.warm_bat_voltage = 4000,
	},
	
	[1] = {
		.max_voltage = 4340,
		.cool_bat_voltage = 4340,
		.warm_bat_voltage = 4000,
	},
	
	[2] = {
		.max_voltage = 4300,
		.cool_bat_voltage = 4300,
		.warm_bat_voltage = 4000,
	},
	
	[3] = {
		.max_voltage = 4350,
		.cool_bat_voltage = 4350,
		.warm_bat_voltage = 4000,
	},
};

static struct single_row_lut fcc_temp_id_1 = {
	.x		= {-20,-10, 0, 10, 20, 30, 40},
	.y		= {1950, 1980, 1990, 2010, 2010, 2020, 2020},
	.cols	= 7
};

static struct single_row_lut fcc_sf_id_1 = {
	.x	= {0},
	.y	= {100},
	.cols	= 1,
};

static struct sf_lut pc_sf_id_1 = {
	.rows		= 1,
	.cols		= 1,
	
	.row_entries		= {0},
	.percent	= {100},
	.sf		= {
			{100}
	},
};

static struct sf_lut rbatt_sf_id_1 = {
	.rows		= 19,
        .cols           = 7,
	
        .row_entries            = {-20,-10, 0, 10, 20, 30, 40},
        .percent        = {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10},
        .sf                     = {
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
                                        {183,167,155,138,113,85,55,},
        }
};

static struct pc_temp_ocv_lut  pc_temp_ocv_id_1 = {
	.rows		= 29,
	.cols		= 7,
	.temp		= {-20,-10, 0, 10, 20, 30, 40},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
	.ocv		= {
				{4290,4290,4290,4290,4290,4290,4290},
				{4249,4259,4263,4265,4266,4265,4263},
				{4181,4199,4205,4208,4210,4209,4208},
				{4123,4143,4151,4154,4156,4155,4154},
				{4060,4093,4100,4102,4105,4104,4102},
				{4002,4037,4050,4055,4058,4057,4055},
				{3956,3982,3994,4003,4008,4008,4008},
				{3916,3939,3949,3960,3967,3969,3969},
				{3882,3897,3905,3914,3925,3930,3931},
				{3854,3862,3867,3871,3876,3877,3870},
				{3832,3836,3839,3841,3843,3843,3844},
				{3813,3816,3817,3818,3821,3821,3823},
				{3798,3801,3800,3800,3802,3802,3800},
				{3784,3784,3788,3787,3788,3787,3787},
				{3779,3779,3771,3771,3771,3771,3771},
				{3754,3754,3754,3754,3754,3754,3754},
				{3732,3732,3732,3732,3732,3732,3732},
				{3694,3694,3694,3694,3694,3694,3694},
				{3677,3677,3677,3677,3677,3677,3677},
				{3670,3670,3670,3670,3670,3670,3670},
				{3662,3662,3662,3662,3662,3662,3662},
				{3655,3655,3655,3655,3655,3655,3655},
				{3648,3648,3648,3648,3648,3648,3648},
				{3640,3640,3640,3640,3640,3640,3640},
				{3559,3559,3559,3559,3559,3559,3559},
				{3478,3478,3478,3478,3478,3478,3478},
				{3397,3397,3397,3397,3397,3397,3397},
				{3316,3316,3316,3316,3316,3316,3316},
				{3235,3235,3235,3235,3235,3235,3235}
	}
};

struct pm8921_bms_battery_data  bms_battery_data_id_1 = {
	.fcc			= 2020,
	.fcc_temp_lut		= &fcc_temp_id_1,
	.fcc_sf_lut		= &fcc_sf_id_1,
	.pc_temp_ocv_lut	= &pc_temp_ocv_id_1,
	.pc_sf_lut		= &pc_sf_id_1,
	.rbatt_sf_lut		= &rbatt_sf_id_1,
	.default_rbatt_mohm	= 250,
	.delta_rbatt_mohm	= 0,
};


static struct single_row_lut fcc_temp_id_2 = {
	.x		= {-20,-10, 0, 10, 20, 30, 40},
	.y		= {1960, 2000, 2000, 2020, 2020, 2020, 2020},
	.cols	= 7
};

static struct single_row_lut fcc_sf_id_2 = {
	.x		= {0},
	.y		= {100},
	.cols	= 1
};

static struct sf_lut pc_sf_id_2 = {
	.rows		= 1,
	.cols		= 1,
        
	.row_entries	= {0},
	.percent	= {100},
	.sf			= {
				{100}
	}
};

static struct sf_lut rbatt_sf_id_2 = {
	.rows		= 19,
        .cols           = 7,
	
        .row_entries            = {-20,-10, 0, 10, 20, 30, 40},
        .percent        = {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10},
        .sf                     = {
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
                                        {224,210,195,174,100,91,85,},
        }
};

static struct pc_temp_ocv_lut  pc_temp_ocv_id_2 = {
	.rows		= 29,
	.cols		= 7,
	.temp		= {-20,-10, 0, 10, 20, 30, 40},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
	.ocv		= {
				{4250,4250,4250,4250,4250,4250,4250},
				{4200,4200,4207,4209,4209,4208,4207},
				{4142,4142,4152,4156,4156,4155,4154},
				{4094,4094,4102,4106,4107,4106,4105},
				{4038,4038,4054,4061,4062,4062,4060},
				{3981,3981,3998,4008,4013,4015,4015},
				{3939,3939,3952,3961,3970,3976,3977},
				{3904,3904,3916,3923,3930,3937,3940},
				{3870,3870,3881,3887,3893,3898,3901},
				{3842,3842,3852,3857,3861,3863,3863},
				{3818,3818,3828,3832,3835,3836,3736},
				{3807,3807,3807,3811,3814,3815,3815},
				{3789,3789,3789,3794,3797,3799,3799},
				{3780,3780,3780,3780,3783,3785,3785},
				{3769,3769,3769,3769,3771,3771,3772},
				{3756,3756,3756,3756,3755,3750,3744},
				{3738,3738,3738,3738,3736,3732,3726},
				{3710,3710,3710,3710,3711,3707,3701},
				{3673,3673,3673,3673,3678,3678,3673},
				{3661,3661,3661,3661,3669,3668,3663},
				{3650,3650,3650,3650,3659,3658,3653},
				{3639,3639,3639,3639,3649,3649,3642},
				{3628,3628,3628,3628,3639,3639,3632},
				{3616,3616,3616,3616,3629,3629,3621},
				{3521,3521,3521,3521,3540,3543,3533},
				{3426,3426,3426,3426,3452,3456,3445},
				{3332,3332,3332,3332,3363,3370,3357},
				{3237,3237,3237,3237,3274,3283,3270},
				{3200,3200,3200,3200,3200,3200,3200}
	},
};

struct pm8921_bms_battery_data  bms_battery_data_id_2 = {
	.fcc			= 2020,
	.fcc_temp_lut		= &fcc_temp_id_2,
	.fcc_sf_lut		= &fcc_sf_id_2,
	.pc_temp_ocv_lut	= &pc_temp_ocv_id_2,
	.pc_sf_lut		= &pc_sf_id_2,
	.rbatt_sf_lut		= &rbatt_sf_id_2,
	.default_rbatt_mohm	= 180,
	.delta_rbatt_mohm	= 0,
};

static struct htc_battery_cell htc_battery_cells[] = {
	[0] = {
		.model_name = "BJ83100",
		.capacity = 2020,
		.id = 1,
		.id_raw_min = 50, 
		.id_raw_max = 200,
		.type = HTC_BATTERY_CELL_TYPE_HV,
		.voltage_max = 4340,
		.voltage_min = 3200,
		.chg_param = &chg_batt_params[1],
		.gauge_param = &bms_battery_data_id_1,
	},
	[1] = {
		.model_name = "BJ83100",
		.capacity = 2020,
		.id = 2,
		.id_raw_min = 201, 
		.id_raw_max = 330,
		.type = HTC_BATTERY_CELL_TYPE_HV,
		.voltage_max = 4300,
		.voltage_min = 3200,
		.chg_param = &chg_batt_params[2],
		.gauge_param = &bms_battery_data_id_2,
	},
	[2] = {
		.model_name = "UNKNOWN",
		.capacity = 2020,
		.id = 255,
		.id_raw_min = INT_MIN,
		.id_raw_max = INT_MAX,
		.type = HTC_BATTERY_CELL_TYPE_NORMAL,
		.voltage_max = 4200,
		.voltage_min = 3200,
		.chg_param = &chg_batt_params[0],
		.gauge_param = NULL,
	},
};
#endif 

#ifdef CONFIG_FB_MSM_HDMI_MHL
static struct pm8xxx_gpio_init switch_to_usb_pmic_gpio_table[] = {
        PM8XXX_GPIO_INIT(AUDIOz_MHL_SW, PM_GPIO_DIR_OUT,
                         PM_GPIO_OUT_BUF_CMOS, 0, PM_GPIO_PULL_NO,
                         PM_GPIO_VIN_S4, PM_GPIO_STRENGTH_LOW,
                         PM_GPIO_FUNC_NORMAL, 0, 0),
        PM8XXX_GPIO_INIT(USBz_AUDIO_SW, PM_GPIO_DIR_OUT,
                         PM_GPIO_OUT_BUF_CMOS, 0, PM_GPIO_PULL_NO,
                         PM_GPIO_VIN_S4, PM_GPIO_STRENGTH_LOW,
                         PM_GPIO_FUNC_NORMAL, 0, 0),
};

static struct pm8xxx_gpio_init switch_to_mhl_pmic_gpio_table[] = {
        PM8XXX_GPIO_INIT(AUDIOz_MHL_SW, PM_GPIO_DIR_OUT,
                         PM_GPIO_OUT_BUF_CMOS, 1, PM_GPIO_PULL_NO,
                         PM_GPIO_VIN_S4, PM_GPIO_STRENGTH_LOW,
                         PM_GPIO_FUNC_NORMAL, 0, 0),
        PM8XXX_GPIO_INIT(USBz_AUDIO_SW, PM_GPIO_DIR_OUT,
                         PM_GPIO_OUT_BUF_CMOS, 1, PM_GPIO_PULL_NO,
                         PM_GPIO_VIN_S4, PM_GPIO_STRENGTH_LOW,
                         PM_GPIO_FUNC_NORMAL, 0, 0),
};

static struct pm8xxx_gpio_init switch_to_usb_headset_pmic_gpio_table[] = {
        PM8XXX_GPIO_INIT(AUDIOz_MHL_SW, PM_GPIO_DIR_OUT,
                         PM_GPIO_OUT_BUF_CMOS, 0, PM_GPIO_PULL_NO,
                         PM_GPIO_VIN_S4, PM_GPIO_STRENGTH_LOW,
                         PM_GPIO_FUNC_NORMAL, 0, 0),
        PM8XXX_GPIO_INIT(USBz_AUDIO_SW, PM_GPIO_DIR_OUT,
                         PM_GPIO_OUT_BUF_CMOS, 1, PM_GPIO_PULL_NO,
                         PM_GPIO_VIN_S4, PM_GPIO_STRENGTH_LOW,
                         PM_GPIO_FUNC_NORMAL, 0, 0),
};

static void config_gpio_table(uint32_t *table, int len)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, table[n], rc);
			break;
		}
	}
}

static void monarudo_usb_dpdn_switch(int path)
{
	static int aud_in = 0;
	switch (path) {
	case PATH_USB:
	case PATH_MHL:
	{
		int i;
		int polarity = 1; 
		int mhl = (path == PATH_MHL);
		if (aud_in == 1) {
			gpio_tlmm_config(GPIO_CFG(UART_TX, 2, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
			gpio_tlmm_config(GPIO_CFG(UART_RX, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
			aud_in = 0;
		}

		if ((mhl ^ !polarity))
			for(i=0; i<ARRAY_SIZE(switch_to_mhl_pmic_gpio_table); i++)
				pm8xxx_gpio_config(switch_to_mhl_pmic_gpio_table[i].gpio,
						&switch_to_mhl_pmic_gpio_table[i].config);
		else
			for(i=0; i<ARRAY_SIZE(switch_to_usb_pmic_gpio_table); i++)
				pm8xxx_gpio_config(switch_to_usb_pmic_gpio_table[i].gpio,
						&switch_to_usb_pmic_gpio_table[i].config);
		pr_info("[CABLE] XA %s: Set %s path\n", __func__, mhl ? "MHL" : "USB");
		break;
	}
	case PATH_USB_AUD:
	{
		int i;
		aud_in = 1;
		gpio_tlmm_config(GPIO_CFG(UART_TX, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		gpio_tlmm_config(GPIO_CFG(UART_RX, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);

		for(i=0; i<ARRAY_SIZE(switch_to_usb_headset_pmic_gpio_table); i++)
			pm8xxx_gpio_config(switch_to_usb_headset_pmic_gpio_table[i].gpio,
					&switch_to_usb_headset_pmic_gpio_table[i].config);
		pr_info("[CABLE] usb dpdn switch usb_aud\n");
		break;
	}
	}

	sii9234_change_usb_owner((path == PATH_MHL) ? 1 : 0);
}

static struct regulator *reg_8921_l12;
static struct regulator *reg_8921_s4;
static struct regulator *reg_8921_l11;

#define _GET_REGULATOR(var, name) do {				\
	var = regulator_get(NULL, name);			\
	if (IS_ERR(var)) {					\
		pr_err("'%s' regulator not found, rc=%ld\n",	\
			name, IS_ERR(var));			\
		var = NULL;					\
		return -ENODEV;					\
	}							\
} while (0)

uint32_t msm_hdmi_off_gpio[] = {
        GPIO_CFG(HDMI_DDC_CLK,  0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
        GPIO_CFG(HDMI_DDC_DATA,  0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
        GPIO_CFG(HDMI_HPLG_DET,  0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};

uint32_t msm_hdmi_on_gpio[] = {
        GPIO_CFG(HDMI_DDC_CLK,  1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_6MA),
        GPIO_CFG(HDMI_DDC_DATA,  1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_6MA),
        GPIO_CFG(HDMI_HPLG_DET,  1, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
};

static void mhl_sii9234_1v2_power(bool enable)
{
	static bool prev_on = false;

	if (enable == prev_on)
		return;

	if (enable) {
		config_gpio_table(msm_hdmi_on_gpio, ARRAY_SIZE(msm_hdmi_on_gpio));
		hdmi_hpd_feature(1);
		pr_info("%s(on): success\n", __func__);
	} else {
		config_gpio_table(msm_hdmi_off_gpio, ARRAY_SIZE(msm_hdmi_off_gpio));
		hdmi_hpd_feature(0);
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = enable;
}

static int mhl_sii9234_all_power(bool enable)
{
	static bool prev_on = false;
	int rc;
	if (enable == prev_on)
		return 0;

	if (!reg_8921_s4)
		_GET_REGULATOR(reg_8921_s4, "8921_s4");
	if (!reg_8921_l11)
		_GET_REGULATOR(reg_8921_l11, "8921_l11");
	if (!reg_8921_l12)
		_GET_REGULATOR(reg_8921_l12, "8921_l12");

	if (enable) {
		rc = regulator_set_voltage(reg_8921_s4, 1800000, 1800000);
		if (rc) {
			pr_err("%s: regulator_set_voltage reg_8921_s4 failed rc=%d\n",
				__func__, rc);
			return rc;
		}
		rc = regulator_set_voltage(reg_8921_l11, 3300000, 3300000);
		if (rc) {
			pr_err("%s: regulator_set_voltage reg_8921_l11 failed rc=%d\n",
				__func__, rc);
			return rc;
		}
		rc = regulator_set_voltage(reg_8921_l12, 1200000, 1200000);
		if (rc) {
			pr_err("%s: regulator_set_voltage reg_8921_l12 failed rc=%d\n",
				__func__, rc);
			return rc;
		}
		rc = regulator_enable(reg_8921_s4);

		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"reg_8921_s4", rc);
			return rc;
		}
		rc = regulator_enable(reg_8921_l11);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"reg_8921_l11", rc);
			return rc;
		}
		rc = regulator_enable(reg_8921_l12);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"reg_8921_l12", rc);
			return rc;
		}
		pr_info("%s(on): success\n", __func__);
	} else {
		rc = regulator_disable(reg_8921_s4);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"reg_8921_s4", rc);
		rc = regulator_disable(reg_8921_l11);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"reg_8921_l11", rc);
		rc = regulator_disable(reg_8921_l12);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				"reg_8921_l12", rc);
		pr_info("%s(off): success\n", __func__);
	}

	prev_on = enable;

	return 0;
}

#ifdef CONFIG_FB_MSM_HDMI_MHL_SII9234
static uint32_t mhl_gpio_table_xb[] = {
	GPIO_CFG(MHL_RSTz_XA, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(MHL_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
};

static uint32_t mhl_gpio_table_xc[] = {
        GPIO_CFG(MHL_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
};

static struct pm8xxx_gpio_init mhl_pmic_gpio_xc[] = {
        PM8XXX_GPIO_INIT(MHL_RSTz_XC_XD, PM_GPIO_DIR_OUT,
                         PM_GPIO_OUT_BUF_CMOS, 0, PM_GPIO_PULL_NO,
                         PM_GPIO_VIN_S4, PM_GPIO_STRENGTH_LOW,
                         PM_GPIO_FUNC_NORMAL, 0, 0),
};

static int mhl_sii9234_power(int on)
{
	int rc = 0;

	switch (on) {
	case 0:
		mhl_sii9234_1v2_power(false);
		break;
	case 1:
		mhl_sii9234_all_power(true);
		if (system_rev <= XB)
			config_gpio_table(mhl_gpio_table_xb, ARRAY_SIZE(mhl_gpio_table_xb));
		else if (system_rev >= XC) {
			config_gpio_table(mhl_gpio_table_xc, ARRAY_SIZE(mhl_gpio_table_xc));
			pm8xxx_gpio_config(mhl_pmic_gpio_xc[0].gpio,
					&mhl_pmic_gpio_xc[0].config);
		}
		break;
	default:
		pr_warning("%s(%d) got unsupport parameter!!!\n", __func__, on);
		break;
	}
	return rc;
}

static T_MHL_PLATFORM_DATA mhl_sii9234_device_data = {
	.gpio_intr = MHL_INT,
	.ci2ca = 0,
#ifdef CONFIG_FB_MSM_HDMI_MHL
	.mhl_usb_switch		= monarudo_usb_dpdn_switch,
	.mhl_1v2_power = mhl_sii9234_1v2_power,
	.enable_5v = hdmi_enable_5v,
#endif
	.power = mhl_sii9234_power,
};

static struct i2c_board_info msm_i2c_mhl_sii9234_info[] =
{
	{
		I2C_BOARD_INFO(MHL_SII9234_I2C_NAME, 0x72 >> 1),
		.platform_data = &mhl_sii9234_device_data,
		.irq = MHL_INT
	},
};
#endif
#endif

#ifdef CONFIG_USB_EHCI_MSM_HSIC
static struct msm_bus_vectors hsic_init_vectors[] = {
       {
               .src = MSM_BUS_MASTER_SPS,
               .dst = MSM_BUS_SLAVE_EBI_CH0,
               .ab = 0,
               .ib = 0,
       },
       {
               .src = MSM_BUS_MASTER_SPS,
               .dst = MSM_BUS_SLAVE_SPS,
               .ab = 0,
               .ib = 0,
       },
};

static struct msm_bus_vectors hsic_max_vectors[] = {
       {
               .src = MSM_BUS_MASTER_SPS,
               .dst = MSM_BUS_SLAVE_EBI_CH0,
               .ab = 60000000,         
               .ib = 960000000,        
       },
       {
               .src = MSM_BUS_MASTER_SPS,
               .dst = MSM_BUS_SLAVE_SPS,
               .ab = 0,
               .ib = 512000000, 
       },
};

static struct msm_bus_paths hsic_bus_scale_usecases[] = {
       {
               ARRAY_SIZE(hsic_init_vectors),
               hsic_init_vectors,
       },
       {
               ARRAY_SIZE(hsic_max_vectors),
               hsic_max_vectors,
       },
};

static struct msm_bus_scale_pdata hsic_bus_scale_pdata = {
       hsic_bus_scale_usecases,
       ARRAY_SIZE(hsic_bus_scale_usecases),
       .name = "hsic",
};

 static struct msm_hsic_host_platform_data msm_hsic_pdata = {
       .strobe                 = 88,
       .data                   = 89,
       .bus_scale_table        = &hsic_bus_scale_pdata,
 };
#else
static struct msm_hsic_host_platform_data msm_hsic_pdata;
#endif

#define PID_MAGIC_ID		0x71432909
#define SERIAL_NUM_MAGIC_ID	0x61945374
#define SERIAL_NUMBER_LENGTH	127
#define DLOAD_USB_BASE_ADD	0x2A03F0C8

struct magic_num_struct {
	uint32_t pid;
	uint32_t serial_num;
};

struct dload_struct {
	uint32_t	reserved1;
	uint32_t	reserved2;
	uint32_t	reserved3;
	uint16_t	reserved4;
	uint16_t	pid;
	char		serial_number[SERIAL_NUMBER_LENGTH];
	uint16_t	reserved5;
	struct magic_num_struct magic_struct;
};

static int usb_diag_update_pid_and_serial_num(uint32_t pid, const char *snum)
{
	struct dload_struct __iomem *dload = 0;

	dload = ioremap(DLOAD_USB_BASE_ADD, sizeof(*dload));
	if (!dload) {
		pr_err("%s: cannot remap I/O memory region: %08x\n",
					__func__, DLOAD_USB_BASE_ADD);
		return -ENXIO;
	}

	pr_debug("%s: dload:%p pid:%x serial_num:%s\n",
				__func__, dload, pid, snum);
	
	dload->magic_struct.pid = PID_MAGIC_ID;
	dload->pid = pid;

	
	dload->magic_struct.serial_num = 0;
	if (!snum) {
		memset(dload->serial_number, 0, SERIAL_NUMBER_LENGTH);
		goto out;
	}

	dload->magic_struct.serial_num = SERIAL_NUM_MAGIC_ID;
	strlcpy(dload->serial_number, snum, SERIAL_NUMBER_LENGTH);
out:
	iounmap(dload);
	return 0;
}

static int dlxwl_usb_product_id_match_array[] = {
	0x0ff8, 0x0e44, 
	0x0fa4, 0x0e9f, 
	0x0fa5, 0x0ea0, 
	0x0f29, 0x079e, 
	0x0f2a, 0x079f, 
	0x0f91, 0x0ebd, 
	0x0f64, 0x07a0, 
	0x0f63, 0x07a1, 
	-1,
};

static int dlxwl_usb_product_id_rndis[] = {
	0x073c, 
	0x0742, 
	0x073d, 
	0x0743, 
	0x0740, 
	0x0746, 
	0x0741, 
	0x0747, 
	0x0794, 
	0x0798, 
	0x0795, 
	0x0799, 
	0x0796, 
	0x079a, 
	0x0797, 
	0x079b, 
};


static int dlxwl_usb_product_id_match(int product_id, int intrsharing)
{
	int *pid_array = dlxwl_usb_product_id_match_array;
	int *rndis_array = dlxwl_usb_product_id_rndis;
	int category = 0;

	if (!pid_array)
		return product_id;

	
	if (board_mfg_mode())
		return product_id;

	while (pid_array[0] >= 0) {
		if (product_id == pid_array[0])
			return pid_array[1];
		pid_array += 2;
	}

	switch (product_id) {
		case 0x0fb4: 
			category = 0;
			break;
		case 0x0fb5: 
			category = 1;
			break;
		case 0x0f8e: 
			category = 2;
			break;
		case 0x0f8f: 
			category = 3;
			break;
		case 0x0f5f: 
			category = 4;
			break;
		case 0x0f60: 
			category = 5;
			break;
		case 0x0f38: 
			category = 6;
			break;
		case 0x0f3d: 
			category = 7;
			break;
		default:
			category = -1;
			break;
	}

	if (category != -1) {
		if (intrsharing)
			return rndis_array[category * 2];
		else
			return rndis_array[category * 2 + 1];
	}
	return product_id;
}

static struct android_usb_platform_data android_usb_pdata = {
	.vendor_id	= 0x0BB4,
	.product_id	= 0x0dff,
	.version	= 0x0100,
	.product_name		= "Android Phone",
	.manufacturer_name	= "HTC",
	.num_products = ARRAY_SIZE(usb_products),
	.products = usb_products,
	.num_functions = ARRAY_SIZE(usb_functions_all),
	.functions = usb_functions_all,
	.update_pid_and_serial_num = usb_diag_update_pid_and_serial_num,
	.usb_id_pin_gpio = USB1_HS_ID_GPIO_XA_XB,
	.usb_rmnet_interface = "HSIC:HSIC",
	.usb_diag_interface = "diag,diag_mdm",
	.fserial_init_string = "HSIC:modem,tty,tty:autobot,tty:serial,tty:autobot,tty:acm",
	.serial_number = "000000000000",
	.match = dlxwl_usb_product_id_match,
	.nluns		= 1,
	.vzw_unmount_cdrom = 1,
};

static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id	= -1,
	.dev	= {
		.platform_data = &android_usb_pdata,
	},
};

static struct msm_bus_vectors usb_init_vectors[] = {
       {
               .src = MSM_BUS_MASTER_SPS,
               .dst = MSM_BUS_SLAVE_EBI_CH0,
               .ab = 0,
               .ib = 0,
       },
};

static struct msm_bus_vectors usb_max_vectors[] = {
       {
               .src = MSM_BUS_MASTER_SPS,
               .dst = MSM_BUS_SLAVE_EBI_CH0,
               .ab = 60000000,         
               .ib = 960000000,        
       },
};

static struct msm_bus_paths usb_bus_scale_usecases[] = {
       {
               ARRAY_SIZE(usb_init_vectors),
               usb_init_vectors,
       },
       {
               ARRAY_SIZE(usb_max_vectors),
               usb_max_vectors,
       },
};

static struct msm_bus_scale_pdata usb_bus_scale_pdata = {
       usb_bus_scale_usecases,
       ARRAY_SIZE(usb_bus_scale_usecases),
       .name = "usb",
};

static int phy_init_seq[] = {
       0x5a, 0x81, 
       0x24, 0x82, 
       -1
};

#ifdef CONFIG_SUPPORT_USB_SPEAKER
struct pm_qos_request pm_qos_req_dma;
void msm_hsusb_setup_gpio(enum usb_otg_state state)
{
	switch (state) {
	case OTG_STATE_UNDEFINED:
		headset_ext_detect(USB_NO_HEADSET);
		pm_qos_update_request(&pm_qos_req_dma, PM_QOS_DEFAULT_VALUE);
		break;
	case OTG_STATE_A_HOST:
		pm_qos_update_request(&pm_qos_req_dma, 3);
		break;
	default:
		break;
	}
}
#endif

static int msm_hsusb_vbus_power(bool on);
static struct msm_otg_platform_data msm_otg_pdata = {
	.mode			= USB_OTG,
	.otg_control		= OTG_PMIC_CONTROL,
	.phy_type		= SNPS_28NM_INTEGRATED_PHY,
	.vbus_power		= msm_hsusb_vbus_power,
	.power_budget		= 500,
	.bus_scale_table        = &usb_bus_scale_pdata,
	.phy_init_seq           = phy_init_seq,
#ifdef CONFIG_SUPPORT_USB_SPEAKER
	.setup_gpio		= msm_hsusb_setup_gpio,
#endif
	.ldo_power_collapse     = POWER_COLLAPSE_LDO1V8,
};

static int64_t monarudo_get_usbid_adc(void)
{
       struct pm8xxx_adc_chan_result result;
       int err = 0, adc =0;
	err = pm8xxx_adc_mpp_config_read(PM8XXX_AMUX_MPP_8, ADC_MPP_1_AMUX6, &result);
       if (err) {
               pr_info("[CABLE] %s: get adc fail, err %d\n", __func__, err);
               return err;
       }
       adc = result.physical;
	adc /= 1000;
       pr_info("[CABLE] chan=%d, adc_code=%d, measurement=%lld, \
                       physical=%lld translate voltage %d\n", result.chan, result.adc_code,
                       result.measurement, result.physical,adc);
       return adc;
}

static uint32_t usb_ID_PIN_input_table_xa_xb[] = {
	GPIO_CFG(USB1_HS_ID_GPIO_XA_XB, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static uint32_t usb_ID_PIN_ouput_table_xa_xb[] = {
	GPIO_CFG(USB1_HS_ID_GPIO_XA_XB, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};


struct pm8xxx_gpio_init usb_id_pmic_gpio_xc[] = {
	PM8XXX_GPIO_INIT(USB1_HS_ID_GPIO_XC, PM_GPIO_DIR_IN,
			 PM_GPIO_OUT_BUF_CMOS, 0, PM_GPIO_PULL_NO,
			 PM_GPIO_VIN_S4, PM_GPIO_STRENGTH_HIGH,
			 PM_GPIO_FUNC_NORMAL, 0, 0),
};

static void monarudo_config_usb_id_gpios(bool output)
{
	int rc;
	if (system_rev == XA || system_rev == XB) {
		
		if (output) {
			gpio_tlmm_config(usb_ID_PIN_ouput_table_xa_xb[0], GPIO_CFG_ENABLE);
			gpio_set_value(USB1_HS_ID_GPIO_XA_XB, 1);
			pr_info("[CABLE] %s: %d output high\n",  __func__, USB1_HS_ID_GPIO_XA_XB);
		} else {
			gpio_tlmm_config(usb_ID_PIN_input_table_xa_xb[0], GPIO_CFG_ENABLE);
			pr_info("[CABLE] %s: %d input none pull\n",  __func__, USB1_HS_ID_GPIO_XA_XB);
		}
	} else {
		
		rc = pm8xxx_gpio_config(usb_id_pmic_gpio_xc[0].gpio, &usb_id_pmic_gpio_xc[0].config);
		if (rc)
			pr_info("[USB BOARD] %s: Config ERROR: GPIO=%u, rc=%d\n",
			__func__, usb_id_pmic_gpio_xc[0].gpio, rc);
		if (output) {
			gpio_direction_output(PM8921_GPIO_PM_TO_SYS(USB1_HS_ID_GPIO_XC),1);
			pr_info("[CABLE] %s: %d output high\n",  __func__, USB1_HS_ID_GPIO_XC);
		} else {
			gpio_direction_input(PM8921_GPIO_PM_TO_SYS(USB1_HS_ID_GPIO_XC));
			pr_info("[CABLE] %s: %d input none pull\n",  __func__, USB1_HS_ID_GPIO_XC);
		}
	}
}

static struct cable_detect_platform_data cable_detect_pdata = {
	.detect_type            = CABLE_TYPE_PMIC_ADC,
	.usb_id_pin_gpio        = USB1_HS_ID_GPIO_XA_XB,
	.get_adc_cb             = monarudo_get_usbid_adc,
	.config_usb_id_gpios    = monarudo_config_usb_id_gpios,
#ifdef CONFIG_FB_MSM_HDMI_MHL
	.mhl_1v2_power = mhl_sii9234_1v2_power,
	.usb_dpdn_switch        = monarudo_usb_dpdn_switch,
#endif
	.usb_uart_switch = monarudo_usb_uart_switch,
#ifdef CONFIG_HTC_BATT_8960
	.is_wireless_charger = pm8921_is_wireless_charger,
#endif
};

static struct platform_device cable_detect_device = {
       .name   = "cable_detect",
       .id     = -1,
       .dev    = {
               .platform_data = &cable_detect_pdata,
       },
};

void monarudo_cable_detect_register(void)
{
	int rc;
	if (system_rev == XA || system_rev == XB) {
		cable_detect_pdata.usb_id_pin_gpio = USB1_HS_ID_GPIO_XA_XB;
		cable_detect_pdata.mhl_reset_gpio = MHL_RSTz_XA;
	} else {
		rc = pm8xxx_gpio_config(usb_id_pmic_gpio_xc[0].gpio, &usb_id_pmic_gpio_xc[0].config);
		if (rc)
			pr_info("[USB BOARD] %s: Config ERROR: GPIO=%u, rc=%d\n",
			__func__, usb_id_pmic_gpio_xc[0].gpio, rc);

		cable_detect_pdata.usb_id_pin_gpio = PM8921_GPIO_PM_TO_SYS(USB1_HS_ID_GPIO_XC);
		cable_detect_pdata.mhl_reset_gpio = PM8921_GPIO_PM_TO_SYS(MHL_RSTz_XC_XD);
	}
	if (board_mfg_mode() == 4)
		cable_detect_pdata.usb_id_pin_gpio = 0;

	platform_device_register(&cable_detect_device);
}

void monarudo_pm8xxx_adc_device_register(void)
{
	pr_info("%s: Register PM8XXX ADC device. rev: %d\n",
		__func__, system_rev);
	monarudo_cable_detect_register();
}

void monarudo_add_usb_devices(void)
{
	printk(KERN_INFO "%s rev: %d\n", __func__, system_rev);

	android_usb_pdata.products[0].product_id =
			android_usb_pdata.product_id;

	
	if (get_radio_flag() & RADIO_FLAG_RESERVE_17)
		android_usb_pdata.diag_init = 1;

	
	if (board_mfg_mode() == 0) {
		android_usb_pdata.nluns = 2;
		android_usb_pdata.cdrom_lun = 0x3;
	}
	android_usb_pdata.serial_number = board_serialno();

	if (system_rev == XA || system_rev == XB) 
		android_usb_pdata.usb_id_pin_gpio = USB1_HS_ID_GPIO_XA_XB;
	else
		android_usb_pdata.usb_id_pin_gpio = PM8921_GPIO_PM_TO_SYS(USB1_HS_ID_GPIO_XC);

	platform_device_register(&apq8064_device_gadget_peripheral);
	platform_device_register(&android_usb_device);
}


struct pm8xxx_gpio_init headset_pmic_gpio_xa[] = {
	PM8XXX_GPIO_INIT(V_AUD_HSMIC_2V85_EN, PM_GPIO_DIR_OUT,
			 PM_GPIO_OUT_BUF_CMOS, 0, PM_GPIO_PULL_NO,
			 PM_GPIO_VIN_S4, PM_GPIO_STRENGTH_LOW,
			 PM_GPIO_FUNC_NORMAL, 0, 0),
};

struct pm8xxx_gpio_init headset_pmic_gpio_xc[] = {
	PM8XXX_GPIO_INIT(V_AUD_HSMIC_2V85_EN, PM_GPIO_DIR_OUT,
			 PM_GPIO_OUT_BUF_CMOS, 0, PM_GPIO_PULL_NO,
			 PM_GPIO_VIN_S4, PM_GPIO_STRENGTH_LOW,
			 PM_GPIO_FUNC_NORMAL, 0, 0),
	PM8XXX_GPIO_INIT(AUD_UART_OEz_XC, PM_GPIO_DIR_OUT,
			 PM_GPIO_OUT_BUF_CMOS, 1, PM_GPIO_PULL_NO,
			 PM_GPIO_VIN_S4, PM_GPIO_STRENGTH_LOW,
			 PM_GPIO_FUNC_NORMAL, 0, 0),
};

static uint32_t headset_cpu_gpio_xc[] = {
	GPIO_CFG(CPU_1WIRE_RX, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(CPU_1WIRE_TX, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(CPU_1WIRE_RX, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(CPU_1WIRE_TX, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static void headset_init(void)
{
	int i = 0;
	int rc = 0;

	pr_info("[HS_BOARD] (%s) Headset initiation (system_rev=%d)\n",
		__func__, system_rev);
	if (system_rev < 2) {
		for( i = 0; i < ARRAY_SIZE(headset_pmic_gpio_xa); i++) {

			rc = pm8xxx_gpio_config(headset_pmic_gpio_xa[0].gpio,
						&headset_pmic_gpio_xa[0].config);
			if (rc)
				pr_info("[HS_BOARD] %s: Config ERROR: GPIO=%u, rc=%d\n",
					__func__, headset_pmic_gpio_xa[0].gpio, rc);
		}
	} else { 
		gpio_tlmm_config(headset_cpu_gpio_xc[2], GPIO_CFG_ENABLE);
		for( i = 0; i < ARRAY_SIZE(headset_pmic_gpio_xc); i++) {

			rc = pm8xxx_gpio_config(headset_pmic_gpio_xc[i].gpio,
						&headset_pmic_gpio_xc[i].config);
			if (rc)
				pr_info("[HS_BOARD] %s: Config ERROR: GPIO=%u, rc=%d\n",
					__func__, headset_pmic_gpio_xc[i].gpio, rc);
		}
	}
}



static void headset_power(int enable)
{

	pr_info("[HS_BOARD] (%s) Set MIC bias %d\n", __func__, enable);

	if (enable)
		gpio_set_value(PM8921_GPIO_PM_TO_SYS(V_AUD_HSMIC_2V85_EN), 1);
	else
		gpio_set_value(PM8921_GPIO_PM_TO_SYS(V_AUD_HSMIC_2V85_EN), 0);
}

static void uart_tx_gpo(int mode)
{
	switch (mode) {
		case 0:
			gpio_tlmm_config(headset_cpu_gpio_xc[1], GPIO_CFG_ENABLE);
			gpio_set_value_cansleep(CPU_1WIRE_TX, 0);
			break;
		case 1:
			gpio_tlmm_config(headset_cpu_gpio_xc[1], GPIO_CFG_ENABLE);
			gpio_set_value_cansleep(CPU_1WIRE_TX, 1);
			break;
		case 2:
			gpio_tlmm_config(headset_cpu_gpio_xc[3], GPIO_CFG_ENABLE);
			break;
	}
}

static void uart_lv_shift_en(int enable)
{
	gpio_set_value_cansleep(PM8921_GPIO_PM_TO_SYS(AUD_UART_OEz_XC), enable);
}

static struct htc_headset_pmic_platform_data htc_headset_pmic_data = {
	.driver_flag		= DRIVER_HS_PMIC_ADC,
	.hpin_gpio		= PM8921_GPIO_PM_TO_SYS(EARPHONE_DETz),
	.hpin_irq		= 0,
	.key_gpio		= CPU_1WIRE_RX,
	.key_irq		= 0,
	.key_enable_gpio	= 0,
	.adc_mic		= 0,
	.adc_remote		= {0, 57, 58, 147, 148, 339},
	.adc_mpp		= PM8XXX_AMUX_MPP_11,
	.adc_amux		= ADC_MPP_1_AMUX6,
	.hs_controller		= 0,
	.hs_switch		= 0,
};

static struct platform_device htc_headset_pmic = {
	.name	= "HTC_HEADSET_PMIC",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_pmic_data,
	},
};

static struct htc_headset_1wire_platform_data htc_headset_1wire_data = {
	.tx_level_shift_en	= PM8921_GPIO_PM_TO_SYS(AUD_UART_OEz_XC),
	.uart_sw		= 0,
	.one_wire_remote	={0x7E, 0x7F, 0x7D, 0x7F, 0x7B, 0x7F},
	.remote_press		= 0,
	.onewire_tty_dev	= "/dev/ttyHSL3",
};

static struct platform_device htc_headset_one_wire = {
	.name	= "HTC_HEADSET_1WIRE",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_1wire_data,
	},
};

static struct platform_device *headset_devices[] = {
	&htc_headset_pmic,
	&htc_headset_one_wire,
	
};

static struct headset_adc_config htc_headset_mgr_config[] = {
	{
		.type = HEADSET_MIC,
		.adc_max = 1530,
		.adc_min = 1223,
	},
	{
		.type = HEADSET_BEATS,
		.adc_max = 1222,
		.adc_min = 916,
	},
	{
		.type = HEADSET_BEATS_SOLO,
		.adc_max = 915,
		.adc_min = 566,
	},
	{
		.type = HEADSET_MIC, 
		.adc_max = 565,
		.adc_min = 255,
	},
	{
		.type = HEADSET_NO_MIC,
		.adc_max = 254,
		.adc_min = 0,
	},
};

static struct htc_headset_mgr_platform_data htc_headset_mgr_data = {
	.driver_flag		= DRIVER_HS_MGR_FLOAT_DET,
	.headset_devices_num	= ARRAY_SIZE(headset_devices),
	.headset_devices	= headset_devices,
	.headset_config_num	= ARRAY_SIZE(htc_headset_mgr_config),
	.headset_config		= htc_headset_mgr_config,
	.headset_init		= headset_init,
	.headset_power		= headset_power,
	.uart_tx_gpo		= uart_tx_gpo,
	.uart_lv_shift_en	= uart_lv_shift_en,
};

static struct platform_device htc_headset_mgr = {
	.name	= "HTC_HEADSET_MGR",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_mgr_data,
	},
};

static void headset_device_register(void)
{
	pr_info("[HS_BOARD] (%s) Headset device register (system_rev=%d)\n",
		__func__, system_rev);
	if (system_rev < 2)
		htc_headset_pmic_data.key_gpio = PM8921_GPIO_PM_TO_SYS(PMIC_1WIRE_RX_XA_XB);

	platform_device_register(&htc_headset_mgr);
}

#define TABLA_INTERRUPT_BASE (NR_MSM_IRQS + NR_GPIO_IRQS + NR_PM8921_IRQS)

static struct wcd9xxx_pdata monarudo_tabla_platform_data = {
	.slimbus_slave_device = {
		.name = "tabla-slave",
		.e_addr = {0, 0, 0x10, 0, 0x17, 2},
	},
	.irq = MSM_GPIO_TO_INT(42),
	.irq_base = TABLA_INTERRUPT_BASE,
	.num_irqs = NR_WCD9XXX_IRQS,
	.reset_gpio = PM8921_GPIO_PM_TO_SYS(34),
	.micbias = {
		.ldoh_v = TABLA_LDOH_2P85_V,
		.cfilt1_mv = 1800,
		.cfilt2_mv = 1800,
		.cfilt3_mv = 1800,
		.bias1_cfilt_sel = TABLA_CFILT1_SEL,
		.bias2_cfilt_sel = TABLA_CFILT2_SEL,
		.bias3_cfilt_sel = TABLA_CFILT3_SEL,
		.bias4_cfilt_sel = TABLA_CFILT3_SEL,
	},
	.regulator = {
	{
		.name = "CDC_VDD_CP",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_CP_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_RX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_RX_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_TX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_TX_CUR_MAX,
	},
	{
		.name = "VDDIO_CDC",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_VDDIO_CDC_CUR_MAX,
	},
	{
		.name = "VDDD_CDC_D",
		.min_uV = 1225000,
		.max_uV = 1250000,
		.optimum_uA = WCD9XXX_VDDD_CDC_D_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_A_1P2V",
		.min_uV = 1225000,
		.max_uV = 1250000,
		.optimum_uA = WCD9XXX_VDDD_CDC_A_CUR_MAX,
	},
	},
};

static struct slim_device monarudo_slim_tabla = {
	.name = "tabla-slim",
	.e_addr = {0, 1, 0x10, 0, 0x17, 2},
	.dev = {
		.platform_data = &monarudo_tabla_platform_data,
	},
};

static struct wcd9xxx_pdata monarudo_tabla20_platform_data = {
	.slimbus_slave_device = {
		.name = "tabla-slave",
		.e_addr = {0, 0, 0x60, 0, 0x17, 2},
	},
	.irq = MSM_GPIO_TO_INT(42),
	.irq_base = TABLA_INTERRUPT_BASE,
	.num_irqs = NR_WCD9XXX_IRQS,
	.reset_gpio = PM8921_GPIO_PM_TO_SYS(34),
	.micbias = {
		.ldoh_v = TABLA_LDOH_2P85_V,
		.cfilt1_mv = 1800,
		.cfilt2_mv = 1800,
		.cfilt3_mv = 1800,
		.bias1_cfilt_sel = TABLA_CFILT1_SEL,
		.bias2_cfilt_sel = TABLA_CFILT2_SEL,
		.bias3_cfilt_sel = TABLA_CFILT3_SEL,
		.bias4_cfilt_sel = TABLA_CFILT3_SEL,
	},
	.amic_settings = {
		.legacy_mode = 0x7F,
		.use_pdata = 0x7F,
	},
	.regulator = {
	{
		.name = "CDC_VDD_CP",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_CP_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_RX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_RX_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_TX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_TX_CUR_MAX,
	},
	{
		.name = "VDDIO_CDC",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_VDDIO_CDC_CUR_MAX,
	},
	{
		.name = "VDDD_CDC_D",
		.min_uV = 1225000,
		.max_uV = 1250000,
		.optimum_uA = WCD9XXX_VDDD_CDC_D_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_A_1P2V",
		.min_uV = 1225000,
		.max_uV = 1250000,
		.optimum_uA = WCD9XXX_VDDD_CDC_A_CUR_MAX,
	},
	},
};

static struct slim_device monarudo_slim_tabla20 = {
	.name = "tabla2x-slim",
	.e_addr = {0, 1, 0x60, 0, 0x17, 2},
	.dev = {
		.platform_data = &monarudo_tabla20_platform_data,
	},
};


static struct synaptics_i2c_rmi_platform_data syn_ts_3k_data[] = { 
	{
		.version = 0x3332,
		.packrat_number = 1293981,
		.abs_x_min = 0,
		.abs_x_max = 1600,
		.abs_y_min = 0,
		.abs_y_max = 2710,
#ifdef CONFIG_FB_MSM_RESOLUTION_OVERRIDE
		.display_width = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_WIDTH,
		.display_height = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_HEIGHT,
#else
		.display_width = 1080,
		.display_height = 1920,
#endif
		.gpio_irq = TP_ATTz,
		.gpio_reset = TP_RSTz_XC,
		.default_config = 1,
		.report_type = SYN_AND_REPORT_TYPE_B,
		.tw_pin_mask = 0x0080,
		.psensor_detection = 1,
		.reduce_report_level = {60, 60, 50, 0, 0},
		.config = {0x33, 0x32, 0x00, 0x07, 0x00, 0x7F, 0x03, 0x1E,
			0x05, 0x09, 0x00, 0x01, 0x01, 0x00, 0x10, 0x54,
			0x06, 0x40, 0x0B, 0x02, 0x14, 0x1E, 0x05, 0x50,
			0x18, 0x28, 0x1E, 0x03, 0x01, 0x3C, 0x1B, 0x01,
			0x1A, 0x01, 0x14, 0x4E, 0x3D, 0x52, 0xC4, 0xB6,
			0xC5, 0xC7, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00,
			0x0A, 0x04, 0xB7, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x19, 0x01, 0x00, 0x0A, 0x17, 0x0D, 0x0A,
			0x00, 0x14, 0x0A, 0x40, 0x78, 0x07, 0xF6, 0xC8,
			0xC0, 0x43, 0x2A, 0x05, 0x00, 0x00, 0x00, 0x00,
			0x54, 0x40, 0xB6, 0x3C, 0x32, 0x00, 0x00, 0x00,
			0x54, 0x40, 0xB6, 0x1E, 0x05, 0x00, 0x02, 0xFA,
			0x00, 0x80, 0x03, 0x0E, 0x1F, 0x11, 0x50, 0x00,
			0x14, 0x04, 0x1B, 0x00, 0x10, 0x0A, 0x60, 0x60,
			0x60, 0x68, 0x48, 0x48, 0x28, 0x20, 0x2E, 0x2B,
			0x29, 0x28, 0x26, 0x23, 0x21, 0x1F, 0x00, 0x00,
			0x00, 0x00, 0x02, 0x08, 0x0E, 0x17, 0x00, 0x7C,
			0x15, 0x00, 0x64, 0x00, 0xC8, 0x00, 0xCD, 0x0A,
			0xCD, 0x4C, 0x1D, 0x00, 0xC0, 0x19, 0x02, 0x02,
			0x04, 0x02, 0x03, 0x03, 0x03, 0x02, 0x10, 0x10,
			0x30, 0x10, 0x20, 0x20, 0x20, 0x10, 0x39, 0x3D,
			0x60, 0x43, 0x5E, 0x64, 0x6A, 0x56, 0x00, 0x96,
			0x00, 0x10, 0x28, 0x00, 0x00, 0x01, 0x08, 0x0E,
			0x14, 0x19, 0x1C, 0x1F, 0x23, 0x04, 0x31, 0x04,
			0x40, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x51, 0x51, 0x51,
			0x51, 0x51, 0x51, 0x51, 0x51, 0xCD, 0x0D, 0x04,
			0x00, 0x11, 0x14, 0x12, 0x0F, 0x0E, 0x09, 0x0A,
			0x07, 0x02, 0x01, 0x00, 0x03, 0x08, 0x0C, 0x0D,
			0x0B, 0x15, 0x17, 0x16, 0x18, 0x19, 0x1A, 0x1B,
			0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x0F, 0x10, 0x0E,
			0x08, 0x07, 0x0C, 0x01, 0x06, 0x02, 0x05, 0x04,
			0x0A, 0xFF, 0xFF, 0xFF, 0x00, 0x10, 0x00, 0x10,
			0x00, 0x10, 0x00, 0x10, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x5A, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00,
			0x0F, 0x01, 0x4F, 0x53
		}
	},
	{
		.version = 0x3332,
		.packrat_number = 1195020,
		.abs_x_min = 0,
		.abs_x_max = 1600,
		.abs_y_min = 0,
		.abs_y_max = 2710,
#ifdef CONFIG_FB_MSM_RESOLUTION_OVERRIDE
		.display_width = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_WIDTH,
		.display_height = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_HEIGHT,
#else
		.display_width = 1080,
		.display_height = 1920,
#endif
		.gpio_irq = TP_ATTz,
		.gpio_reset = TP_RSTz_XC,
		.default_config = 1,
		.report_type = SYN_AND_REPORT_TYPE_B,
		.large_obj_check = 1,
		.tw_pin_mask = 0x0080,
		.PixelTouchThreshold_bef_unlock = 208,
		.reduce_report_level = {60, 60, 50, 0, 0},
		.config = {0x33, 0x32, 0x00, 0x05, 0x80, 0x7F, 0x03, 0x1E,
			0x05, 0x09, 0x00, 0x01, 0x01, 0x00, 0x10, 0x54,
			0x06, 0x40, 0x0B, 0x02, 0x14, 0x1E, 0x05, 0x50,
			0x18, 0x28, 0x1E, 0x03, 0x01, 0x3C, 0x1B, 0x01,
			0x1A, 0x01, 0x14, 0x4E, 0x3D, 0x52, 0xC4, 0xB6,
			0xC5, 0xC7, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00,
			0x0A, 0x04, 0xB7, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x19, 0x01, 0x00, 0x0A, 0x17, 0x0D, 0x0A,
			0x00, 0x14, 0x0A, 0x40, 0x78, 0x07, 0xF6, 0xC8,
			0xC0, 0x43, 0x2A, 0x05, 0x00, 0x00, 0x00, 0x00,
			0x54, 0x40, 0xB6, 0x3C, 0x32, 0x00, 0x00, 0x00,
			0x54, 0x40, 0xB6, 0x1E, 0x05, 0x00, 0x02, 0xFA,
			0x00, 0x80, 0x03, 0x0E, 0x1F, 0x11, 0x50, 0x00,
			0x14, 0x04, 0x1B, 0x00, 0x10, 0xFF, 0x60, 0x60,
			0x60, 0x68, 0x48, 0x48, 0x28, 0x20, 0x2E, 0x2B,
			0x29, 0x28, 0x26, 0x23, 0x21, 0x1F, 0x00, 0x00,
			0x00, 0x00, 0x02, 0x08, 0x0E, 0x17, 0x00, 0x7C,
			0x15, 0x00, 0x64, 0x00, 0xC8, 0x00, 0xCD, 0x0A,
			0xCD, 0x4C, 0x1D, 0x00, 0xC0, 0x80, 0x02, 0x02,
			0x04, 0x02, 0x03, 0x03, 0x03, 0x02, 0x10, 0x10,
			0x30, 0x10, 0x20, 0x20, 0x20, 0x10, 0x39, 0x3D,
			0x60, 0x43, 0x5E, 0x64, 0x6A, 0x56, 0x00, 0x96,
			0x00, 0x10, 0x28, 0x00, 0x00, 0x01, 0x08, 0x0E,
			0x14, 0x19, 0x1C, 0x1F, 0x23, 0x04, 0x31, 0x04,
			0x40, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x51, 0x51, 0x51,
			0x51, 0x51, 0x51, 0x51, 0x51, 0xCD, 0x0D, 0x04,
			0x00, 0x11, 0x14, 0x12, 0x0F, 0x0E, 0x09, 0x0A,
			0x07, 0x02, 0x01, 0x00, 0x03, 0x08, 0x0C, 0x0D,
			0x0B, 0x15, 0x17, 0x16, 0x18, 0x19, 0x1A, 0x1B,
			0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x0F, 0x10, 0x0E,
			0x08, 0x07, 0x0C, 0x01, 0x06, 0x02, 0x05, 0x04,
			0x0A, 0xFF, 0xFF, 0xFF, 0x00, 0x10, 0x00, 0x10,
			0x00, 0x10, 0x00, 0x10, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x5A, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00,
			0x0F, 0x01
		}
	},
	{
		.version = 0x3332,
		.packrat_number = 1116012,
		.abs_x_min = 0,
		.abs_x_max = 1600,
		.abs_y_min = 0,
		.abs_y_max = 2710,
#ifdef CONFIG_FB_MSM_RESOLUTION_OVERRIDE
		.display_width = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_WIDTH,
		.display_height = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_HEIGHT,
#else
		.display_width = 1080,
		.display_height = 1920,
#endif
		.gpio_irq = TP_ATTz,
		.gpio_reset = TP_RSTz_XC,
		.default_config = 1,
		.report_type = SYN_AND_REPORT_TYPE_B,
		.large_obj_check = 1,
		.tw_pin_mask = 0x0080,
		.reduce_report_level = {60, 60, 50, 0, 0},
		.config = {0x33, 0x32, 0x00, 0x03, 0x04, 0x7F, 0x03, 0x1E,
			0x05, 0x09, 0x00, 0x01, 0x01, 0x00, 0x10, 0x54,
			0x06, 0x40, 0x0B, 0x02, 0x14, 0x23, 0x05, 0x50,
			0x49, 0x19, 0xCD, 0x02, 0x01, 0x3C, 0x1D, 0x01,
			0x1A, 0x02, 0x14, 0x4E, 0x3D, 0x52, 0xCE, 0xD1,
			0x90, 0xD7, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
			0x0C, 0x04, 0xB7, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x19, 0x01, 0x00, 0x0A, 0x17, 0x0D, 0x0A,
			0x00, 0x14, 0x0A, 0x40, 0x7D, 0x07, 0xF6, 0xDC,
			0xC0, 0x43, 0x2A, 0x05, 0x00, 0x00, 0x00, 0x00,
			0x54, 0x40, 0xB6, 0x3C, 0x32, 0x00, 0x00, 0x00,
			0x54, 0x40, 0xB6, 0x1E, 0x05, 0x20, 0x02, 0x44,
			0x01, 0x80, 0x03, 0x0E, 0x1F, 0x10, 0x73, 0x00,
			0x19, 0x04, 0x1B, 0x00, 0x10, 0xFF, 0x60, 0x68,
			0x60, 0x68, 0x60, 0x68, 0x40, 0x48, 0x30, 0x2F,
			0x2D, 0x2B, 0x2A, 0x28, 0x27, 0x25, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x00, 0x7C,
			0x15, 0x00, 0x10, 0x27, 0xB0, 0x36, 0xCD, 0xFF,
			0xE6, 0xE0, 0x2E, 0x00, 0xC0, 0x80, 0x03, 0x02,
			0x02, 0x02, 0x02, 0x05, 0x02, 0x02, 0x20, 0x10,
			0x10, 0x10, 0x10, 0x20, 0x10, 0x10, 0x66, 0x50,
			0x53, 0x56, 0x59, 0x4A, 0x60, 0x64, 0x00, 0xC8,
			0x00, 0x10, 0x28, 0x00, 0x00, 0x00, 0x06, 0x0C,
			0x12, 0x18, 0x1F, 0x25, 0x28, 0x04, 0x31, 0x04,
			0x4D, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x51, 0x51, 0x51,
			0x51, 0x51, 0x51, 0x51, 0x51, 0xCD, 0x0D, 0x04,
			0x00, 0x11, 0x14, 0x12, 0x0F, 0x0E, 0x09, 0x0A,
			0x07, 0x02, 0x01, 0x00, 0x03, 0x08, 0x0C, 0x0D,
			0x0B, 0x15, 0x17, 0x16, 0x18, 0x19, 0x1A, 0x1B,
			0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x0F, 0x10, 0x0E,
			0x08, 0x07, 0x0C, 0x01, 0x06, 0x02, 0x05, 0x04,
			0x0A, 0xFF, 0xFF, 0xFF, 0x00, 0x10, 0x00, 0x10,
			0x00, 0x10, 0x00, 0x10, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00}
	},
	{
		.version = 0x3330,
		.packrat_number = 1100755,
		.abs_x_min = 0,
		.abs_x_max = 1620,
		.abs_y_min = 0,
		.abs_y_max = 2680,
#ifdef CONFIG_FB_MSM_RESOLUTION_OVERRIDE
		.display_width = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_WIDTH,
		.display_height = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_HEIGHT,
#else
		.display_width = 1080,
		.display_height = 1920,
#endif
		.gpio_irq = TP_ATTz,
		.gpio_reset = TP_RSTz_XC,
		.default_config = 2,
		.large_obj_check = 1,
		.tw_pin_mask = 0x0080,
		.config = {0x4D, 0x4F, 0x4F, 0x31, 0x04, 0x3F, 0x03, 0x1E,
			0x05, 0xB1, 0x08, 0x0B, 0x19, 0x19, 0x00, 0x00,
			0x54, 0x06, 0x40, 0x0B, 0x02, 0x14, 0x1E, 0x05,
			0x28, 0xF5, 0x28, 0x1E, 0x05, 0x01, 0x3C, 0x18,
			0x02, 0x1A, 0x01, 0xCD, 0x4C, 0x33, 0x53, 0xEB,
			0xD5, 0x5E, 0xDA, 0x00, 0x70, 0x00, 0x00, 0x00,
			0x00, 0x0A, 0x04, 0xC0, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x19, 0x01, 0x00, 0x0A, 0x00, 0x08,
			0xA2, 0x02, 0x32, 0x0A, 0x0A, 0x96, 0x17, 0x0D,
			0x00, 0x02, 0x0F, 0x01, 0x80, 0x03, 0x0E, 0x1F,
			0x12, 0x46, 0x00, 0x19, 0x04, 0x1B, 0x00, 0x10,
			0x28, 0x00, 0x11, 0x14, 0x12, 0x0F, 0x0E, 0x09,
			0x0A, 0x07, 0x02, 0x01, 0x00, 0x03, 0x08, 0x0C,
			0x0D, 0x0B, 0x15, 0x17, 0x16, 0x18, 0x19, 0x1A,
			0x1B, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x0F, 0x10,
			0x0E, 0x08, 0x07, 0x0C, 0x01, 0x06, 0x02, 0x05,
			0x04, 0x0A, 0xFF, 0xFF, 0xFF, 0xA0, 0xA0, 0xA0,
			0xA0, 0xA0, 0xA0, 0x80, 0x80, 0x44, 0x43, 0x41,
			0x40, 0x3E, 0x3D, 0x3B, 0x39, 0x00, 0x03, 0x06,
			0x09, 0x0C, 0x0F, 0x12, 0x17, 0x00, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
			0xFF, 0xFF, 0x00, 0xC0, 0x80, 0x00, 0x10, 0x00,
			0x10, 0x00, 0x10, 0x00, 0x10, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x6C, 0x70, 0x73, 0x76, 0x79, 0x7C, 0x7F, 0x58,
			0x00, 0xFF, 0xFF, 0x10, 0x28, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0x51, 0x51, 0x51, 0x51, 0xCD, 0x0D,
			0x04}
	},
	{
		.version = 0x3230,
		.abs_x_min = 0,
		.abs_x_max = 1620,
		.abs_y_min = 0,
		.abs_y_max = 2680,
#ifdef CONFIG_FB_MSM_RESOLUTION_OVERRIDE
		.display_width = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_WIDTH,
		.display_height = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_HEIGHT,
#else
		.display_width = 1080,
		.display_height = 1920,
#endif
		.gpio_irq = TP_ATTz,
		.gpio_reset = TP_RSTz_XC,
		.default_config = 1,
		.tw_pin_mask = 0x0080,
		.config = {0x30, 0x32, 0x30, 0x30, 0x84, 0x0F, 0x03, 0x1E,
			0x05, 0x20, 0xB1, 0x00, 0x0B, 0x19, 0x19, 0x00,
			0x00, 0x54, 0x06, 0x40, 0x0B, 0x1E, 0x05, 0x2D,
			0xF6, 0x04, 0xEA, 0x01, 0x01, 0x19, 0x01, 0x15,
			0x01, 0x14, 0x4E, 0x0A, 0x53, 0xD8, 0xC4, 0x24,
			0xCD, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x0A,
			0x04, 0xC0, 0x00, 0x02, 0x1E, 0x01, 0x80, 0x01,
			0x0D, 0x1E, 0x00, 0x35, 0x00, 0x19, 0x04, 0x1E,
			0x00, 0x10, 0x0A, 0x00, 0x11, 0x14, 0x12, 0x0F,
			0x0E, 0x09, 0x0A, 0x07, 0x02, 0x01, 0x00, 0x03,
			0x08, 0x0C, 0x0D, 0x0B, 0x15, 0x17, 0x16, 0x18,
			0x19, 0x1A, 0x1B, 0xFF, 0xFF, 0xFF, 0xFF, 0x12,
			0x0F, 0x10, 0x0E, 0x08, 0x07, 0x0C, 0x01, 0x06,
			0x02, 0x05, 0x04, 0x0A, 0xFF, 0xFF, 0xFF, 0xC0,
			0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x60,
			0x5F, 0x5D, 0x5B, 0x59, 0x57, 0x56, 0x53, 0x00,
			0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0F, 0x00,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xC0, 0x80, 0x00,
			0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
			0x02, 0x02, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x5B, 0x5E, 0x60, 0x62, 0x64, 0x66,
			0x68, 0x6B, 0x19, 0x15, 0x00, 0x1E, 0x19, 0x05,
			0x01, 0x01, 0x3D, 0x08}
	},
	{
		.version = 0x0000
	},
};

static struct synaptics_i2c_rmi_platform_data syn_ts_3k_2p5D_3030_evita_data[] = { 
	{
		.version = 0x3330,
		.packrat_number = 1100755,
		.abs_x_min = 0,
		.abs_x_max = 1100,
		.abs_y_min = 0,
		.abs_y_max = 1770,
		.display_width = 1080,
		.display_height = 1920,
		.gpio_irq = TP_ATTz,
		.gpio_reset = TP_RSTz_XA_XB,
		.default_config = 2,
		.large_obj_check = 1,
		.config = {0x30, 0x32, 0x30, 0x33, 0x00, 0x3F, 0x03, 0x1E,
			0x05, 0xB1, 0x09, 0x0B, 0x01, 0x01, 0x00, 0x00,
			0x4C, 0x04, 0x75, 0x07, 0x02, 0x14, 0x1E, 0x05,
			0x2D, 0x6C, 0x19, 0x7B, 0x07, 0x01, 0x3C, 0x1B,
			0x01, 0x1C, 0x01, 0x66, 0x4E, 0x00, 0x50, 0x10,
			0xB5, 0x3F, 0xBE, 0x00, 0x70, 0x00, 0x00, 0x00,
			0x00, 0x0A, 0x04, 0xBC, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x19, 0x01, 0x00, 0x0A, 0x30, 0x32,
			0xA2, 0x02, 0x32, 0x05, 0x0F, 0x96, 0x16, 0x0C,
			0x00, 0x02, 0x18, 0x01, 0x80, 0x03, 0x0E, 0x1F,
			0x12, 0x63, 0x00, 0x13, 0x04, 0x00, 0x00, 0x08,
			0xFF, 0x00, 0x06, 0x0C, 0x0D, 0x0B, 0x15, 0x17,
			0x16, 0x18, 0x19, 0x1A, 0x1B, 0x11, 0x14, 0x12,
			0x0F, 0x0E, 0x09, 0x0A, 0x07, 0x02, 0x01, 0x00,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x04, 0x05, 0x02,
			0x06, 0x01, 0x0C, 0x07, 0x08, 0x0E, 0x10, 0x0F,
			0x12, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xA0, 0xA0,
			0xA8, 0xA8, 0xA8, 0xA8, 0x88, 0x47, 0x46, 0x44,
			0x42, 0x40, 0x3F, 0x3D, 0x3B, 0x01, 0x04, 0x08,
			0x0C, 0x10, 0x14, 0x18, 0x1C, 0x00, 0xA0, 0x0F,
			0xCD, 0x3C, 0x00, 0xC8, 0x00, 0xB3, 0xC8, 0xCD,
			0xA0, 0x0F, 0x00, 0xC0, 0x80, 0x00, 0x10, 0x00,
			0x10, 0x00, 0x10, 0x00, 0x10, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x03, 0x04, 0x04, 0x03, 0x04, 0x08, 0x03, 0x02,
			0x20, 0x30, 0x30, 0x20, 0x30, 0x50, 0x20, 0x10,
			0x58, 0x66, 0x69, 0x60, 0x6F, 0x5F, 0x68, 0x50,
			0x00, 0xA0, 0x00, 0x10, 0x0A, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0x51, 0x51, 0x51, 0x51, 0xCD, 0x0D,
			0x04}
	},
	{
		.version = 0x3230,
		.abs_x_min = 0,
		.abs_x_max = 1000,
		.abs_y_min = 0,
		.abs_y_max = 1770,
		.display_width = 1080,
		.display_height = 1920,
		.gpio_irq = TP_ATTz,
		.gpio_reset = TP_RSTz_XA_XB,
		.default_config = 1,
		.config = {0x30, 0x32, 0x30, 0x30, 0x84, 0x0F, 0x03, 0x1E,
			0x05, 0x20, 0xB1, 0x08, 0x0B, 0x19, 0x19, 0x00,
			0x00, 0xE8, 0x03, 0x75, 0x07, 0x1E, 0x05, 0x2D,
			0x0E, 0x06, 0xD4, 0x01, 0x01, 0x48, 0xFD, 0x41,
			0xFE, 0x00, 0x50, 0x65, 0x4E, 0xFF, 0xBA, 0xBF,
			0xC0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x0A,
			0x04, 0xB2, 0x00, 0x02, 0xF1, 0x00, 0x80, 0x02,
			0x0D, 0x1E, 0x00, 0x4D, 0x00, 0x19, 0x04, 0x1E,
			0x00, 0x10, 0xFF, 0x00, 0x06, 0x0C, 0x0D, 0x0B,
			0x15, 0x17, 0x16, 0x18, 0x19, 0x1A, 0x1B, 0x11,
			0x14, 0x12, 0x0F, 0x0E, 0x09, 0x0A, 0x07, 0x02,
			0x01, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x04,
			0x05, 0x02, 0x06, 0x01, 0x0C, 0x07, 0x08, 0x0E,
			0x10, 0x0F, 0x12, 0xFF, 0xFF, 0xFF, 0xFF, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x60, 0x60, 0x60, 0x3C,
			0x3A, 0x38, 0x36, 0x34, 0x33, 0x31, 0x2F, 0x01,
			0x06, 0x0C, 0x11, 0x16, 0x1B, 0x21, 0x27, 0x00,
			0x41, 0x04, 0x80, 0x41, 0x04, 0xE1, 0x28, 0xC0,
			0x14, 0xCC, 0x81, 0x0D, 0x00, 0xC0, 0x80, 0x00,
			0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x02, 0x02, 0x02, 0x03, 0x07, 0x03,
			0x0B, 0x03, 0x20, 0x20, 0x20, 0x20, 0x50, 0x20,
			0x70, 0x20, 0x73, 0x77, 0x7B, 0x56, 0x5F, 0x5C,
			0x5B, 0x64, 0x48, 0x41, 0x00, 0x1E, 0x19, 0x05,
			0xFD, 0xFE, 0x3D, 0x08}
	},
	{
		.version = 0x0000
	},
};

static struct i2c_board_info msm_i2c_gsbi3_info[] = {
	{
		I2C_BOARD_INFO(SYNAPTICS_3200_NAME, 0x40 >> 1),
		.platform_data = &syn_ts_3k_data,
		.irq = MSM_GPIO_TO_INT(TP_ATTz)
	},
};

static ssize_t virtual_syn_keys_show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	int virtual_key[3][4] = {{180, 2010, 280, 160},
				{540, 2010, 200, 160},
				{900, 2010, 240, 160}};
#ifdef CONFIG_FB_MSM_RESOLUTION_OVERRIDE
	int i, origin_x, origin_y, scale_x, scale_y;
	origin_x = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_DST_WIDTH;
	origin_y = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_DST_HEIGHT;
	scale_x = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_WIDTH;
	scale_y = CONFIG_FB_MSM_RESOLUTION_OVERRIDE_SRC_HEIGHT;
	for (i=0;i<3;i++) {
		virtual_key[i][0] = (virtual_key[i][0] * scale_x) / origin_x;
		virtual_key[i][1] = (virtual_key[i][1] * scale_y) / origin_y;
		virtual_key[i][2] = (virtual_key[i][2] * scale_x) / origin_x;
		virtual_key[i][3] = (virtual_key[i][3] * scale_y) / origin_y;
	}
#endif

	return sprintf(buf,
		__stringify(EV_KEY) ":" __stringify(KEY_BACK)       ":%d:%d:%d:%d"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":%d:%d:%d:%d"
		":" __stringify(EV_KEY) ":" __stringify(KEY_APP_SWITCH)   ":%d:%d:%d:%d"
		"\n", virtual_key[0][0], virtual_key[0][1], virtual_key[0][2], virtual_key[0][3],
		virtual_key[1][0], virtual_key[1][1], virtual_key[1][2], virtual_key[1][3],
		virtual_key[2][0], virtual_key[2][1], virtual_key[2][2], virtual_key[2][3]);

}

static ssize_t virtual_syn_keys_evita_show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf,
		__stringify(EV_KEY) ":" __stringify(KEY_BACK)       ":112:1345:120:100"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":360:1345:120:100"
		":" __stringify(EV_KEY) ":" __stringify(KEY_APP_SWITCH)   ":595:1345:120:100"
		"\n");

}

static struct kobj_attribute syn_virtual_keys_attr = {
	.attr = {
		.name = "virtualkeys.synaptics-rmi-touchscreen",
		.mode = S_IRUGO,
        },
	.show = &virtual_syn_keys_show,
};

static struct kobj_attribute syn_virtual_keys_evita_attr = {
	.attr = {
		.name = "virtualkeys.synaptics-rmi-touchscreen",
		.mode = S_IRUGO,
	},
	.show = &virtual_syn_keys_evita_show,
};

static struct attribute *properties_attrs[] = {
	&syn_virtual_keys_attr.attr,
	NULL
};

static struct attribute *properties_evita_attrs[] = {
	&syn_virtual_keys_evita_attr.attr,
	NULL
};

static struct attribute_group properties_attr_group = {
	.attrs = properties_attrs,
};

static struct attribute_group properties_evita_attr_group = {
	.attrs = properties_evita_attrs,
};

static struct regulator *motion_sensor_vreg_8921_l17;
static struct regulator *g_sensor_vreg_8921_l17;
static struct regulator *compass_vreg_8921_l17;
static struct regulator *gyro_vreg_8921_l17;
static DEFINE_MUTEX(sensor_lock);

static int monarudo_g_sensor_power_LPM(int on)
{
	int rc = 0;

	mutex_lock(&sensor_lock);

	printk(KERN_DEBUG "[GSNR][BMA250_BOSCH] %s, on = %d, "
			  "g_sensor_vreg_8921_l17 = 0x%p\n",
			  __func__, on, g_sensor_vreg_8921_l17);

	if (!g_sensor_vreg_8921_l17)
		_GET_REGULATOR(g_sensor_vreg_8921_l17, "8921_l17_g_sensor");

	if (on) {
		rc = regulator_set_optimum_mode(g_sensor_vreg_8921_l17, 100);
		if (rc < 0) {
			pr_err("[GSNR][BMA250_BOSCH] set_optimum_mode L17 to"
				" LPM failed, rc = %d\n", rc);
			mutex_unlock(&sensor_lock);
			printk(KERN_DEBUG "[GSNR][BMA250_BOSCH]%s unlock 1\n",
					  __func__);
			return -EINVAL;
		}
		rc = regulator_enable(g_sensor_vreg_8921_l17);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"g_sensor_vreg_8921_l17", rc);
			return rc;
		}
		printk(KERN_DEBUG "[GSNR][BMA250_BOSCH] %s, Set to Low Power"
			" Mode\n", __func__);
	} else {
		rc = regulator_set_optimum_mode(g_sensor_vreg_8921_l17, 100000);
		if (rc < 0) {
			pr_err("[GSNR][BMA250_BOSCH] set_optimum_mode L17 to"
				" Normal mode failed, rc = %d\n", rc);
			mutex_unlock(&sensor_lock);
			printk(KERN_DEBUG "[GSNR][BMA250_BOSCH]%s unlock 2\n",
					  __func__);
			return -EINVAL;
		}
		rc = regulator_enable(g_sensor_vreg_8921_l17);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"g_sensor_vreg_8921_l17", rc);
			return rc;
		}
		printk(KERN_DEBUG "[GSNR][BMA250_BOSCH] %s, Set to Normal Mode\n",
			__func__);
	}
	mutex_unlock(&sensor_lock);
	printk(KERN_DEBUG "[GSNR][BMA250_BOSCH]%s: unlock 3\n", __func__);
	return 0;
}

static struct bma250_platform_data gsensor_bma250_platform_data = {
        .intr = GSENSOR_INT_XC,
        .chip_layout = 1,
	.axis_map_x = 0,
	.axis_map_y = 1,
	.axis_map_z = 2,
	.negate_x = 1,
	.negate_y = 0,
	.negate_z = 1,
	.power_LPM = monarudo_g_sensor_power_LPM,
};

static int monarudo_compass_power_LPM(int on)
{
	int rc = 0;

	mutex_lock(&sensor_lock);

	printk(KERN_DEBUG "[COMP][AKM8963] %s, on = %d, "
			  "compass_vreg_8921_l17 = 0x%p\n",
			  __func__, on, compass_vreg_8921_l17);

	if (!compass_vreg_8921_l17)
		_GET_REGULATOR(compass_vreg_8921_l17, "8921_l17_compass");

	if (on) {
		rc = regulator_set_optimum_mode(compass_vreg_8921_l17, 100);
		if (rc < 0) {
			pr_err("[COMP][AKM8963] set_optimum_mode L17 to LPM"
				" failed, rc = %d\n", rc);
			mutex_unlock(&sensor_lock);
			printk(KERN_DEBUG "[COMP][AKM8963]%s unlock 1\n",
					  __func__);
			return -EINVAL;
		}
		rc = regulator_enable(compass_vreg_8921_l17);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"compass_vreg_8921_l17", rc);
			return rc;
		}
		printk(KERN_DEBUG "[COMP][AKM8963] %s, Set to Low Power"
			" Mode\n", __func__);
	} else {
		rc = regulator_set_optimum_mode(compass_vreg_8921_l17, 100000);
		if (rc < 0) {
			pr_err("[COMP][AKM8963] set_optimum_mode L17 to"
				" Normal mode failed, rc = %d\n", rc);
			mutex_unlock(&sensor_lock);
			printk(KERN_DEBUG "[COMP][AKM8963]%s unlock 2\n",
					  __func__);
			return -EINVAL;
		}
		rc = regulator_enable(compass_vreg_8921_l17);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"compass_vreg_8921_l17", rc);
			return rc;
		}
		printk(KERN_DEBUG "[COMP][AKM8963] %s, Set to Normal Mode\n",
			__func__);
	}
	mutex_unlock(&sensor_lock);
	printk(KERN_DEBUG "[COMP][AKM8963]%s unlock 3\n", __func__);
	return 0;
}

static struct akm8963_platform_data compass_platform_data = {
	.layout = 5,
	.outbit = 1,
	.gpio_DRDY = PM8921_GPIO_PM_TO_SYS(COMPASS_AKM_INT),
	.gpio_RST = 0,
	.power_LPM = monarudo_compass_power_LPM,
};

static int monarudo_gyro_power_LPM(int on)
{
	int rc = 0;

	mutex_lock(&sensor_lock);

	printk(KERN_DEBUG "[GYRO][R3GD20] %s, on = %d, "
			  "gyro_vreg_8921_l17 = 0x%p\n",
			  __func__, on, gyro_vreg_8921_l17);

	if (!gyro_vreg_8921_l17)
		_GET_REGULATOR(gyro_vreg_8921_l17, "8921_l17_gyro");

	if (on) {
		rc = regulator_set_optimum_mode(gyro_vreg_8921_l17, 100);
		if (rc < 0) {
			pr_err("[GYRO][R3GD20] set_optimum_mode L17 to LPM"
				" failed, rc = %d\n", rc);
			mutex_unlock(&sensor_lock);
			printk(KERN_DEBUG "[GYRO][R3GD20]%s unlock 1\n",
					  __func__);
			return -EINVAL;
		}
		rc = regulator_enable(gyro_vreg_8921_l17);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"gyro_vreg_8921_l17", rc);
			return rc;
		}
		printk(KERN_DEBUG "[GYRO][R3GD20] %s, Set to Low Power"
			" Mode\n", __func__);
	} else {
		rc = regulator_set_optimum_mode(gyro_vreg_8921_l17, 100000);
		if (rc < 0) {
			pr_err("[GYRO][R3GD20] set_optimum_mode L17 to"
				" Normal mode failed, rc = %d\n", rc);
			mutex_unlock(&sensor_lock);
			printk(KERN_DEBUG "[GYRO][R3GD20]%s unlock 2\n",
					  __func__);
			return -EINVAL;
		}
		rc = regulator_enable(gyro_vreg_8921_l17);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"gyro_vreg_8921_l17", rc);
			return rc;
		}
		printk(KERN_DEBUG "[GYRO][R3GD20] %s, Set to Normal Mode\n",
			__func__);
	}
	mutex_unlock(&sensor_lock);
	printk(KERN_DEBUG "[GYRO][R3GD20]%s unlock 3\n", __func__);
	return 0;
}

static struct r3gd20_gyr_platform_data gyro_platform_data = {
	.fs_range = R3GD20_GYR_FS_2000DPS,
	.axis_map_x = 0,
	.axis_map_y = 1,
	.axis_map_z = 2,
	.negate_x = 1,
	.negate_y = 0,
	.negate_z = 1,

	.poll_interval = 50,
	.min_interval = R3GD20_MIN_POLL_PERIOD_MS, 

	
	             

	.watermark = 0,
	.fifomode = 0,
	.power_LPM = monarudo_gyro_power_LPM,
};

static struct i2c_board_info motion_sensor_gsbi_2_info[] = {
        {
                I2C_BOARD_INFO(BMA250_I2C_NAME, 0x30 >> 1),
                .platform_data = &gsensor_bma250_platform_data,
                .irq = MSM_GPIO_TO_INT(GSENSOR_INT_XC),
        },
        {
                I2C_BOARD_INFO(AKM8963_I2C_NAME, 0x1A >> 1),
                .platform_data = &compass_platform_data,
                .irq = PM8921_GPIO_IRQ(PM8921_IRQ_BASE, COMPASS_AKM_INT),
        },
        {
                I2C_BOARD_INFO(R3GD20_GYR_DEV_NAME, 0xD0 >> 1),
                .platform_data = &gyro_platform_data,
                
        },
};

static struct cm3629_platform_data cm36282_pdata = {
	.model = CAPELLA_CM36282,
	.ps_select = CM3629_PS1_ONLY,
	.intr = PM8921_GPIO_PM_TO_SYS(PROXIMITY_INT),
	.levels = { 1, 3, 33, 929, 1440, 5614, 8553, 12415, 16278, 65535},
	.golden_adc = 0x1900,
	.power = NULL,
	.cm3629_slave_address = 0xC0>>1,
	.ps1_thd_set = 0x15,
	.ps1_thd_no_cal = 0xF1,
	.ps1_thd_with_cal = 0xD,
	.ps_calibration_rule = 1,
	.ps_conf1_val = CM3629_PS_DR_1_80 | CM3629_PS_IT_1_6T |
			CM3629_PS1_PERS_3,
	.ps_conf2_val = CM3629_PS_ITB_1 | CM3629_PS_ITR_1 |
			CM3629_PS2_INT_DIS | CM3629_PS1_INT_DIS,
	.ps_conf3_val = CM3629_PS2_PROL_32,
	.dark_level = 3,
};

static struct i2c_board_info i2c_CM36282_devices[] = {
	{
		I2C_BOARD_INFO(CM3629_I2C_NAME, 0xC0 >> 1),
		.platform_data = &cm36282_pdata,
		.irq =  PM8921_GPIO_IRQ(PM8921_IRQ_BASE, PROXIMITY_INT),
	},
};

#if 0 
static uint32_t gsbi2_gpio_table[] = {
       GPIO_CFG(I2C2_DATA_SENS, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
       GPIO_CFG(I2C2_CLK_SENS, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};


static uint32_t gsbi3_gpio_table[] = {
       GPIO_CFG(MONACO_GPIO_TP_I2C_DAT, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
       GPIO_CFG(MONACO_GPIO_TP_I2C_CLK, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static void gsbi_qup_i2c_gpio_config(int adap_id, int config_type) {

	printk(KERN_INFO "%s(): adap_id = %d, config_type = %d \n", __func__, adap_id, config_type);

	if ((adap_id == MSM8064_GSBI2_QUP_I2C_BUS_ID) && (config_type == 1)) {
		gpio_tlmm_config(gsbi2_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi2_gpio_table[1], GPIO_CFG_ENABLE);
	}

	if ((adap_id == MSM8064_GSBI3_QUP_I2C_BUS_ID) && (config_type == 1)) {
		gpio_tlmm_config(gsbi3_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_tlmm_config(gsbi3_gpio_table[1], GPIO_CFG_ENABLE);
	}
}
#endif

#define MSM_WCNSS_PHYS	0x03000000
#define MSM_WCNSS_SIZE	0x280000

static struct resource resources_wcnss_wlan[] = {
	{
		.start	= RIVA_APPS_WLAN_RX_DATA_AVAIL_IRQ,
		.end	= RIVA_APPS_WLAN_RX_DATA_AVAIL_IRQ,
		.name	= "wcnss_wlanrx_irq",
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= RIVA_APPS_WLAN_DATA_XFER_DONE_IRQ,
		.end	= RIVA_APPS_WLAN_DATA_XFER_DONE_IRQ,
		.name	= "wcnss_wlantx_irq",
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= MSM_WCNSS_PHYS,
		.end	= MSM_WCNSS_PHYS + MSM_WCNSS_SIZE - 1,
		.name	= "wcnss_mmio",
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= 64,
		.end	= 68,
		.name	= "wcnss_gpios_5wire",
		.flags	= IORESOURCE_IO,
	},
};

static struct qcom_wcnss_opts qcom_wcnss_pdata = {
	.has_48mhz_xo	= 1,
};

static struct platform_device msm_device_wcnss_wlan = {
	.name		= "wcnss_wlan",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(resources_wcnss_wlan),
	.resource	= resources_wcnss_wlan,
	.dev		= {.platform_data = &qcom_wcnss_pdata},
};

#ifdef CONFIG_QSEECOM
static struct msm_bus_vectors qseecom_clks_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ib = 0,
		.ab = 0,
	},
	{
		.src = MSM_BUS_MASTER_SPDM,
		.dst = MSM_BUS_SLAVE_SPDM,
		.ib = 0,
		.ab = 0,
	},
};

static struct msm_bus_vectors qseecom_enable_dfab_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ib = (492 * 8) * 1000000UL,
		.ab = (492 * 8) *  100000UL,
	},
	{
		.src = MSM_BUS_MASTER_SPDM,
		.dst = MSM_BUS_SLAVE_SPDM,
		.ib = 0,
		.ab = 0,
	},
};

static struct msm_bus_vectors qseecom_enable_sfpb_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ib = 0,
		.ab = 0,
	},
	{
		.src = MSM_BUS_MASTER_SPDM,
		.dst = MSM_BUS_SLAVE_SPDM,
		.ib = (64 * 8) * 1000000UL,
		.ab = (64 * 8) *  100000UL,
	},
};

static struct msm_bus_paths qseecom_hw_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(qseecom_clks_init_vectors),
		qseecom_clks_init_vectors,
	},
	{
		ARRAY_SIZE(qseecom_enable_dfab_vectors),
		qseecom_enable_sfpb_vectors,
	},
	{
		ARRAY_SIZE(qseecom_enable_sfpb_vectors),
		qseecom_enable_sfpb_vectors,
	},
};

static struct msm_bus_scale_pdata qseecom_bus_pdata = {
	qseecom_hw_bus_scale_usecases,
	ARRAY_SIZE(qseecom_hw_bus_scale_usecases),
	.name = "qsee",
};

static struct platform_device qseecom_device = {
	.name		= "qseecom",
	.id		= 0,
	.dev		= {
		.platform_data = &qseecom_bus_pdata,
	},
};
#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)

#define QCE_SIZE		0x10000
#define QCE_0_BASE		0x11000000

#define QCE_HW_KEY_SUPPORT	0
#define QCE_SHA_HMAC_SUPPORT	1
#define QCE_SHARE_CE_RESOURCE	3
#define QCE_CE_SHARED		0

static struct resource qcrypto_resources[] = {
	[0] = {
		.start = QCE_0_BASE,
		.end = QCE_0_BASE + QCE_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name = "crypto_channels",
		.start = DMOV8064_CE_IN_CHAN,
		.end = DMOV8064_CE_OUT_CHAN,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.name = "crypto_crci_in",
		.start = DMOV8064_CE_IN_CRCI,
		.end = DMOV8064_CE_IN_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.name = "crypto_crci_out",
		.start = DMOV8064_CE_OUT_CRCI,
		.end = DMOV8064_CE_OUT_CRCI,
		.flags = IORESOURCE_DMA,
	},
};

static struct resource qcedev_resources[] = {
	[0] = {
		.start = QCE_0_BASE,
		.end = QCE_0_BASE + QCE_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name = "crypto_channels",
		.start = DMOV8064_CE_IN_CHAN,
		.end = DMOV8064_CE_OUT_CHAN,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.name = "crypto_crci_in",
		.start = DMOV8064_CE_IN_CRCI,
		.end = DMOV8064_CE_IN_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.name = "crypto_crci_out",
		.start = DMOV8064_CE_OUT_CRCI,
		.end = DMOV8064_CE_OUT_CRCI,
		.flags = IORESOURCE_DMA,
	},
};

#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)

static struct msm_ce_hw_support qcrypto_ce_hw_suppport = {
	.ce_shared = QCE_CE_SHARED,
	.shared_ce_resource = QCE_SHARE_CE_RESOURCE,
	.hw_key_support = QCE_HW_KEY_SUPPORT,
	.sha_hmac = QCE_SHA_HMAC_SUPPORT,
	.bus_scale_table = NULL,
};

static struct platform_device qcrypto_device = {
	.name		= "qcrypto",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(qcrypto_resources),
	.resource	= qcrypto_resources,
	.dev		= {
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &qcrypto_ce_hw_suppport,
	},
};
#endif

#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)

static struct msm_ce_hw_support qcedev_ce_hw_suppport = {
	.ce_shared = QCE_CE_SHARED,
	.shared_ce_resource = QCE_SHARE_CE_RESOURCE,
	.hw_key_support = QCE_HW_KEY_SUPPORT,
	.sha_hmac = QCE_SHA_HMAC_SUPPORT,
	.bus_scale_table = NULL,
};

static struct platform_device qcedev_device = {
	.name		= "qce",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(qcedev_resources),
	.resource	= qcedev_resources,
	.dev		= {
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &qcedev_ce_hw_suppport,
	},
};
#endif

static struct mdm_vddmin_resource mdm_vddmin_rscs = {
	.rpm_id = MSM_RPM_ID_VDDMIN_GPIO,
	.ap2mdm_vddmin_gpio = 30,
	.modes  = 0x03,
	.drive_strength = 8,
	.mdm2ap_vddmin_gpio = 80,
};

static struct mdm_platform_data mdm_platform_data = {
	.mdm_version = "3.0",
	.ramdump_delay_ms = 2000,
	.vddmin_resource = &mdm_vddmin_rscs,
	.peripheral_platform_device = &apq8064_device_hsic_host,
	.ramdump_timeout_ms = 120000,
};

static struct tsens_platform_data apq_tsens_pdata  = {
		.tsens_factor		= 1000,
		.hw_type		= APQ_8064,
		.patherm0               = -1,
		.patherm1               = -1,
		.tsens_num_sensor	= 11,
		.slope = {1176, 1176, 1154, 1176, 1111,
			1132, 1132, 1199, 1132, 1199, 1132},
};

static struct platform_device msm_tsens_device = {
	.name   = "tsens8960-tm",
	.id = -1,
};

static struct msm_thermal_data msm_thermal_pdata = {
	.sensor_id = 0,
	.poll_ms = 1000,
	.limit_temp = 51,
	.temp_hysteresis = 10,
	.limit_freq = 918000,
};

static int __init check_dq_setup(char *str)
{
	int i = 0;
	int size = 0;

	size = sizeof(chg_batt_params)/sizeof(chg_batt_params[0]);

	if (!strcmp(str, "PASS")) {
		
	} else {
		for(i=0; i < size; i++)
		{
			chg_batt_params[i].max_voltage = 4200;
			chg_batt_params[i].cool_bat_voltage = 4200;
		}
	}
	return 1;
}
__setup("androidboot.dq=", check_dq_setup);


#define MSM_SHARED_RAM_PHYS 0x80000000
static void __init monarudo_map_io(void)
{
	msm_shared_ram_phys = MSM_SHARED_RAM_PHYS;
	msm_map_apq8064_io();
	if (socinfo_init() < 0)
		pr_err("socinfo_init() failed!\n");
}

static void __init monarudo_init_irq(void)
{
	struct msm_mpm_device_data *data = NULL;

#ifdef CONFIG_MSM_MPM
	data = &apq8064_mpm_dev_data;
#endif

	msm_mpm_irq_extn_init(data);
	gic_init(0, GIC_PPI_START, MSM_QGIC_DIST_BASE,
						(void *)MSM_QGIC_CPU_BASE);
}

static struct platform_device msm8064_device_saw_regulator_core0 = {
	.name	= "saw-regulator",
	.id	= 0,
	.dev	= {
		.platform_data = &monarudo_saw_regulator_pdata_8921_s5,
	},
};

static struct platform_device msm8064_device_saw_regulator_core1 = {
	.name	= "saw-regulator",
	.id	= 1,
	.dev	= {
		.platform_data = &monarudo_saw_regulator_pdata_8921_s6,
	},
};

static struct platform_device msm8064_device_saw_regulator_core2 = {
	.name	= "saw-regulator",
	.id	= 2,
	.dev	= {
		.platform_data = &monarudo_saw_regulator_pdata_8821_s0,
	},
};

static struct platform_device msm8064_device_saw_regulator_core3 = {
	.name	= "saw-regulator",
	.id	= 3,
	.dev	= {
		.platform_data = &monarudo_saw_regulator_pdata_8821_s1,

	},
};

static struct msm_rpmrs_level msm_rpmrs_levels[] = {
	{
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT,
		MSM_RPMRS_LIMITS(ON, ACTIVE, MAX, ACTIVE),
		true,
		1, 784, 180000, 100,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE,
		MSM_RPMRS_LIMITS(ON, ACTIVE, MAX, ACTIVE),
		true,
		1300, 228, 1200000, 2152,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(ON, GDHS, MAX, ACTIVE),
		false,
		2000, 138, 1208400, 3352,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(ON, HSFS_OPEN, ACTIVE, RET_HIGH),
		false,
		6000, 119, 1850300, 9152,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, GDHS, MAX, ACTIVE),
		false,
		9200, 68, 2839200, 16552,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, MAX, ACTIVE),
		false,
		10300, 63, 3128000, 18352,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, ACTIVE, RET_HIGH),
		false,
		18000, 10, 4602600, 27152,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, RET_HIGH, RET_LOW),
		false,
		20000, 2, 5752000, 32152,
	},
};

uint32_t msm_rpm_get_swfi_latency(void)
{
	return msm_rpmrs_levels[0].latency_us;
}
EXPORT_SYMBOL(msm_rpm_get_swfi_latency);

static struct msm_pm_boot_platform_data msm_pm_boot_pdata __initdata = {
	.mode = MSM_PM_BOOT_CONFIG_TZ,
};

static struct msm_rpmrs_platform_data msm_rpmrs_data __initdata = {
	.levels = &msm_rpmrs_levels[0],
	.num_levels = ARRAY_SIZE(msm_rpmrs_levels),
	.vdd_mem_levels  = {
		[MSM_RPMRS_VDD_MEM_RET_LOW]	= 750000,
		[MSM_RPMRS_VDD_MEM_RET_HIGH]	= 750000,
		[MSM_RPMRS_VDD_MEM_ACTIVE]	= 1050000,
		[MSM_RPMRS_VDD_MEM_MAX]		= 1150000,
	},
	.vdd_dig_levels = {
		[MSM_RPMRS_VDD_DIG_RET_LOW]	= 500000,
		[MSM_RPMRS_VDD_DIG_RET_HIGH]	= 750000,
		[MSM_RPMRS_VDD_DIG_ACTIVE]	= 950000,
		[MSM_RPMRS_VDD_DIG_MAX]		= 1150000,
	},
	.vdd_mask = 0x7FFFFF,
	.rpmrs_target_id = {
		[MSM_RPMRS_ID_PXO_CLK]		= MSM_RPM_ID_PXO_CLK,
		[MSM_RPMRS_ID_L2_CACHE_CTL]	= MSM_RPM_ID_LAST,
		[MSM_RPMRS_ID_VDD_DIG_0]	= MSM_RPM_ID_PM8921_S3_0,
		[MSM_RPMRS_ID_VDD_DIG_1]	= MSM_RPM_ID_PM8921_S3_1,
		[MSM_RPMRS_ID_VDD_MEM_0]	= MSM_RPM_ID_PM8921_L24_0,
		[MSM_RPMRS_ID_VDD_MEM_1]	= MSM_RPM_ID_PM8921_L24_1,
		[MSM_RPMRS_ID_RPM_CTL]		= MSM_RPM_ID_RPM_CTL,
	},
};

#if 0 
static struct msm_cpuidle_state msm_cstates[] __initdata = {
	{0, 0, "C0", "WFI",
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT},

	{0, 1, "C1", "RETENTION",
		MSM_PM_SLEEP_MODE_RETENTION},

	{0, 2, "C2", "STANDALONE_POWER_COLLAPSE",
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE},

	{0, 3, "C3", "POWER_COLLAPSE",
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE},

	{1, 0, "C0", "WFI",
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT},

	{1, 1, "C1", "RETENTION",
		MSM_PM_SLEEP_MODE_RETENTION},

	{1, 2, "C2", "STANDALONE_POWER_COLLAPSE",
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE},

	{2, 0, "C0", "WFI",
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT},

	{2, 1, "C1", "RETENTION",
		MSM_PM_SLEEP_MODE_RETENTION},

	{2, 2, "C2", "STANDALONE_POWER_COLLAPSE",
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE},

	{3, 0, "C0", "WFI",
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT},

	{3, 1, "C1", "RETENTION",
		MSM_PM_SLEEP_MODE_RETENTION},

	{3, 2, "C2", "STANDALONE_POWER_COLLAPSE",
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE},
};

static struct msm_pm_platform_data msm_pm_data[] = {
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_RETENTION)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 0,
		.suspend_enabled = 0,
	},

	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_RETENTION)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 0,
		.suspend_enabled = 0,
	},

	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(2, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(2, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(2, MSM_PM_SLEEP_MODE_RETENTION)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 0,
		.suspend_enabled = 0,
	},

	[MSM_PM_MODE(2, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(3, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(3, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(3, MSM_PM_SLEEP_MODE_RETENTION)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 0,
		.suspend_enabled = 0,
	},

	[MSM_PM_MODE(3, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},
};
#endif

static uint8_t spm_wfi_cmd_sequence[] __initdata = {
	0x03, 0x0f,
};

static uint8_t spm_power_collapse_without_rpm[] __initdata = {
	0x00, 0x24, 0x54, 0x10,
	0x09, 0x03, 0x01,
	0x10, 0x54, 0x30, 0x0C,
	0x24, 0x30, 0x0f,
};

static uint8_t spm_power_collapse_with_rpm[] __initdata = {
	0x00, 0x24, 0x54, 0x10,
	0x09, 0x07, 0x01, 0x0B,
	0x10, 0x54, 0x30, 0x0C,
	0x24, 0x30, 0x0f,
};

static struct msm_spm_seq_entry msm_spm_seq_list[] __initdata = {
	[0] = {
		.mode = MSM_SPM_MODE_CLOCK_GATING,
		.notify_rpm = false,
		.cmd = spm_wfi_cmd_sequence,
	},
	[1] = {
		.mode = MSM_SPM_MODE_POWER_COLLAPSE,
		.notify_rpm = false,
		.cmd = spm_power_collapse_without_rpm,
	},
	[2] = {
		.mode = MSM_SPM_MODE_POWER_COLLAPSE,
		.notify_rpm = true,
		.cmd = spm_power_collapse_with_rpm,
	},
};

static uint8_t l2_spm_wfi_cmd_sequence[] __initdata = {
	0x00, 0x20, 0x03, 0x20,
	0x00, 0x0f,
};

static uint8_t l2_spm_gdhs_cmd_sequence[] __initdata = {
	0x00, 0x20, 0x34, 0x64,
	0x48, 0x07, 0x48, 0x20,
	0x50, 0x64, 0x04, 0x34,
	0x50, 0x0f,
};
static uint8_t l2_spm_power_off_cmd_sequence[] __initdata = {
	0x00, 0x10, 0x34, 0x64,
	0x48, 0x07, 0x48, 0x10,
	0x50, 0x64, 0x04, 0x34,
	0x50, 0x0F,
};

static struct msm_spm_seq_entry msm_spm_l2_seq_list[] __initdata = {
	[0] = {
		.mode = MSM_SPM_L2_MODE_RETENTION,
		.notify_rpm = false,
		.cmd = l2_spm_wfi_cmd_sequence,
	},
	[1] = {
		.mode = MSM_SPM_L2_MODE_GDHS,
		.notify_rpm = true,
		.cmd = l2_spm_gdhs_cmd_sequence,
	},
	[2] = {
		.mode = MSM_SPM_L2_MODE_POWER_COLLAPSE,
		.notify_rpm = true,
		.cmd = l2_spm_power_off_cmd_sequence,
	},
};


static struct msm_spm_platform_data msm_spm_l2_data[] __initdata = {
	[0] = {
		.reg_base_addr = MSM_SAW_L2_BASE,
		.reg_init_values[MSM_SPM_REG_SAW2_SPM_CTL] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DLY] = 0x02020205,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_0] = 0x00A000AE,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_1] = 0x00A00020,
		.modes = msm_spm_l2_seq_list,
		.num_modes = ARRAY_SIZE(msm_spm_l2_seq_list),
	},
};

static struct msm_spm_platform_data msm_spm_data[] __initdata = {
	[0] = {
		.reg_base_addr = MSM_SAW0_BASE,
		.reg_init_values[MSM_SPM_REG_SAW2_CFG] = 0x1F,
#if defined(CONFIG_MSM_AVS_HW)
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_CTL] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_HYSTERESIS] = 0x00,
#endif
		.reg_init_values[MSM_SPM_REG_SAW2_SPM_CTL] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DLY] = 0x02020205,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_0] = 0x0060009C,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_1] = 0x0000001C,
		.vctl_timeout_us = 50,
		.num_modes = ARRAY_SIZE(msm_spm_seq_list),
		.modes = msm_spm_seq_list,
	},
	[1] = {
		.reg_base_addr = MSM_SAW1_BASE,
		.reg_init_values[MSM_SPM_REG_SAW2_CFG] = 0x1F,
#if defined(CONFIG_MSM_AVS_HW)
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_CTL] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_HYSTERESIS] = 0x00,
#endif
		.reg_init_values[MSM_SPM_REG_SAW2_SPM_CTL] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DLY] = 0x02020205,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_0] = 0x0060009C,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_1] = 0x0000001C,
		.vctl_timeout_us = 50,
		.num_modes = ARRAY_SIZE(msm_spm_seq_list),
		.modes = msm_spm_seq_list,
	},
	[2] = {
		.reg_base_addr = MSM_SAW2_BASE,
		.reg_init_values[MSM_SPM_REG_SAW2_CFG] = 0x1F,
#if defined(CONFIG_MSM_AVS_HW)
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_CTL] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_HYSTERESIS] = 0x00,
#endif
		.reg_init_values[MSM_SPM_REG_SAW2_SPM_CTL] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DLY] = 0x02020205,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_0] = 0x0060009C,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_1] = 0x0000001C,
		.vctl_timeout_us = 50,
		.num_modes = ARRAY_SIZE(msm_spm_seq_list),
		.modes = msm_spm_seq_list,
	},
	[3] = {
		.reg_base_addr = MSM_SAW3_BASE,
		.reg_init_values[MSM_SPM_REG_SAW2_CFG] = 0x1F,
#if defined(CONFIG_MSM_AVS_HW)
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_CTL] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_HYSTERESIS] = 0x00,
#endif
		.reg_init_values[MSM_SPM_REG_SAW2_SPM_CTL] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DLY] = 0x02020205,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_0] = 0x0060009C,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_1] = 0x0000001C,
		.vctl_timeout_us = 50,
		.num_modes = ARRAY_SIZE(msm_spm_seq_list),
		.modes = msm_spm_seq_list,
	},
};
#ifdef CONFIG_FLASHLIGHT_TPS61310
#if 0	
#ifdef CONFIG_MSM_CAMERA_FLASH
int flashlight_control(int mode)
{
pr_info("%s, linear led, mode=%d", __func__, mode);
	return tps61310_flashlight_control(mode);
}
#endif
#endif
static void config_flashlight_gpios(void)
{
	if (system_rev <= XB) {
		static uint32_t flashlight_gpio_table[] = {
			GPIO_CFG(DRIVER_EN_XA_XB, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
		};

		gpio_tlmm_config(flashlight_gpio_table[0], GPIO_CFG_ENABLE);
	}
}

static struct TPS61310_flashlight_platform_data flashlight_data = {
	.gpio_init = config_flashlight_gpios,
	.tps61310_strb0 = DRIVER_EN_XA_XB,
	.tps61310_strb1 = PM8921_GPIO_PM_TO_SYS(TORCH_FLASHz),
	.flash_duration_ms = 600,
	.led_count = 1,
	.disable_tx_mask = 1,
};

static struct i2c_board_info i2c_tps61310_flashlight[] = {
	{
		I2C_BOARD_INFO("TPS61310_FLASHLIGHT", 0x66 >> 1),
		.platform_data = &flashlight_data,
	},
};

static struct TPS61310_flashlight_platform_data flashlight_data_XC = {
	.gpio_init = config_flashlight_gpios,
	.tps61310_strb0 = PM8921_GPIO_PM_TO_SYS(DRIVER_EN_XC),
	.tps61310_strb1 = PM8921_GPIO_PM_TO_SYS(TORCH_FLASHz),
	.flash_duration_ms = 600,
	.led_count = 1,
};

static struct i2c_board_info i2c_tps61310_flashlight_XC[] = {
	{
		I2C_BOARD_INFO("TPS61310_FLASHLIGHT", 0x66 >> 1),
		.platform_data = &flashlight_data_XC,
	},
};
#endif

static struct msm_pm_sleep_status_data msm_pm_slp_sts_data = {
	.base_addr = MSM_ACC0_BASE + 0x08,
	.cpu_offset = MSM_ACC1_BASE - MSM_ACC0_BASE,
	.mask = 1UL << 13,
};

static void __init monarudo_init_buses(void)
{
	msm_bus_rpm_set_mt_mask();
	msm_bus_8064_apps_fabric_pdata.rpm_enabled = 1;
	msm_bus_8064_sys_fabric_pdata.rpm_enabled = 1;
	msm_bus_8064_mm_fabric_pdata.rpm_enabled = 1;
	msm_bus_8064_apps_fabric.dev.platform_data =
		&msm_bus_8064_apps_fabric_pdata;
	msm_bus_8064_sys_fabric.dev.platform_data =
		&msm_bus_8064_sys_fabric_pdata;
	msm_bus_8064_mm_fabric.dev.platform_data =
		&msm_bus_8064_mm_fabric_pdata;
	msm_bus_8064_sys_fpb.dev.platform_data = &msm_bus_8064_sys_fpb_pdata;
	msm_bus_8064_cpss_fpb.dev.platform_data = &msm_bus_8064_cpss_fpb_pdata;
}

static struct platform_device monarudo_device_ext_5v_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= PM8921_MPP_PM_TO_SYS(7),
	.dev	= {
		.platform_data
			= &monarudo_gpio_regulator_pdata[GPIO_VREG_ID_EXT_5V],
	},
};

static struct platform_device monarudo_device_ext_mpp8_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= PM8921_MPP_PM_TO_SYS(8),
	.dev	= {
		.platform_data
			= &monarudo_gpio_regulator_pdata[GPIO_VREG_ID_EXT_MPP8],
	},
};

static struct platform_device monarudo_device_ext_3p3v_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= monarudo_EXT_3P3V_REG_EN_GPIO,
	.dev	= {
		.platform_data =
			&monarudo_gpio_regulator_pdata[GPIO_VREG_ID_EXT_3P3V],
	},
};

static struct platform_device monarudo_device_ext_ts_sw_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= PM8921_GPIO_PM_TO_SYS(23),
	.dev	= {
		.platform_data
			= &monarudo_gpio_regulator_pdata[GPIO_VREG_ID_EXT_TS_SW],
	},
};

static struct platform_device monarudo_device_rpm_regulator __devinitdata = {
	.name	= "rpm-regulator",
	.id	= -1,
	.dev	= {
		.platform_data = &monarudo_rpm_regulator_pdata,
	},
};

#define MSM_RAM_CONSOLE_BASE   MSM_HTC_RAM_CONSOLE_PHYS
#define MSM_RAM_CONSOLE_SIZE   MSM_HTC_RAM_CONSOLE_SIZE

static struct resource ram_console_resources[] = {
	{
		.start  = MSM_RAM_CONSOLE_BASE,
		.end    = MSM_RAM_CONSOLE_BASE + MSM_RAM_CONSOLE_SIZE - 1,
		.flags  = IORESOURCE_MEM,
	},
};

static struct platform_device ram_console_device = {
	.name           = "ram_console",
	.id             = -1,
	.num_resources  = ARRAY_SIZE(ram_console_resources),
	.resource       = ram_console_resources,
};
static struct pm8xxx_vibrator_pwm_platform_data pm8xxx_vib_pwm_pdata = {
	.initial_vibrate_ms = 0,
	.max_timeout_ms = 15000,
	.duty_us = 37,
	.PERIOD_US = 38,
	.bank = PM8XXX_ID_GPIO26,
	.ena_gpio = PM8921_GPIO_PM_TO_SYS(HAPTIC_EN),
	.vdd_gpio = PM8921_GPIO_PM_TO_SYS(HAPTIC_3V3_EN_XA_XB),
};
static struct pm8xxx_vibrator_pwm_platform_data pm8xxx_vib_pwm_pdata_XC = {
	.initial_vibrate_ms = 0,
	.max_timeout_ms = 15000,
	.duty_us = 37,
	.PERIOD_US = 38,
	.bank = PM8XXX_ID_GPIO26,
	.ena_gpio = PM8921_GPIO_PM_TO_SYS(HAPTIC_EN),
	.vdd_gpio = HAPTIC_3V3_EN_XC_XD,
};
static struct pm8xxx_vibrator_pwm_platform_data pm8xxx_vib_pwm_pdata_XD = {
	.initial_vibrate_ms = 0,
    .max_timeout_ms = 15000,
	.duty_us = 35,
	.PERIOD_US = 38,
	.bank = PM8XXX_ID_GPIO26,
	.ena_gpio = PM8921_GPIO_PM_TO_SYS(HAPTIC_EN),
	.vdd_gpio = HAPTIC_3V3_EN_XC_XD,
};

static struct platform_device vibrator_pwm_device = {
	.name = PM8XXX_VIBRATOR_PWM_DEV_NAME,
	.dev = {
		.platform_data	= &pm8xxx_vib_pwm_pdata,
	},
};

static struct platform_device vibrator_pwm_device_XC = {
	.name = PM8XXX_VIBRATOR_PWM_DEV_NAME,
	.dev = {
		.platform_data	= &pm8xxx_vib_pwm_pdata_XC,
	},
};
static struct platform_device vibrator_pwm_device_XD = {
        .name = PM8XXX_VIBRATOR_PWM_DEV_NAME,
        .dev = {
                .platform_data  = &pm8xxx_vib_pwm_pdata_XD,
        },
};


#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
static struct resource hdmi_msm_resources[] = {
	{
		.name  = "hdmi_msm_qfprom_addr",
		.start = 0x00700000,
		.end   = 0x007060FF,
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = "hdmi_msm_hdmi_addr",
		.start = 0x04A00000,
		.end   = 0x04A00FFF,
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = "hdmi_msm_irq",
		.start = HDMI_IRQ,
		.end   = HDMI_IRQ,
		.flags = IORESOURCE_IRQ,
	},
};

static int hdmi_enable_5v(int on);
static int hdmi_core_power(int on, int show);

static mhl_driving_params monarudo_driving_params[] = {
	{.format = HDMI_VFRMT_640x480p60_4_3,	.reg_a3=0xFE, .reg_a6=0x0C},
	{.format = HDMI_VFRMT_720x480p60_16_9,	.reg_a3=0xFE, .reg_a6=0x0C},
	{.format = HDMI_VFRMT_1280x720p60_16_9,	.reg_a3=0xFE, .reg_a6=0x0C},
	{.format = HDMI_VFRMT_720x576p50_16_9,	.reg_a3=0xFE, .reg_a6=0x0C},
	{.format = HDMI_VFRMT_1920x1080p24_16_9, .reg_a3=0xFE, .reg_a6=0x0C},
	{.format = HDMI_VFRMT_1920x1080p30_16_9, .reg_a3=0xFE, .reg_a6=0x0C},
};

static struct msm_hdmi_platform_data hdmi_msm_data = {
	.irq = HDMI_IRQ,
	.enable_5v = hdmi_enable_5v,
	.core_power = hdmi_core_power,
	
	.driving_params = monarudo_driving_params,
	.dirving_params_count = ARRAY_SIZE(monarudo_driving_params),
};

static struct platform_device hdmi_msm_device = {
	.name = "hdmi_msm",
	.id = 0,
	.num_resources = ARRAY_SIZE(hdmi_msm_resources),
	.resource = hdmi_msm_resources,
	.dev.platform_data = &hdmi_msm_data,
};

#define BOOST_5V	"ext_5v"
static struct regulator *reg_boost_5v = NULL;

static int hdmi_enable_5v(int on)
{
	static int prev_on = 0;
	int rc;

	if (on == prev_on)
		return 0;

	if (!reg_boost_5v)
		_GET_REGULATOR(reg_boost_5v, BOOST_5V);

	if (on) {
		rc = regulator_enable(reg_boost_5v);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				BOOST_5V, rc);
			return rc;
		}
	} else {
		rc = regulator_disable(reg_boost_5v);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				BOOST_5V, rc);
	}

	pr_info("%s(%s): success\n", __func__, on?"on":"off");

	prev_on = on;

	return 0;
}

static int msm_hsusb_vbus_power(bool on)
{
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

	if (!reg_boost_5v)
		_GET_REGULATOR(reg_boost_5v, BOOST_5V);

	if (on) {
		rc = regulator_enable(reg_boost_5v);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				BOOST_5V, rc);
			return rc;
		}
	} else {
		rc = regulator_disable(reg_boost_5v);
		if (rc)
			pr_warning("'%s' regulator disable failed, rc=%d\n",
				BOOST_5V, rc);
	}

	pr_info("%s(%s): success\n", __func__, on?"on":"off");

	prev_on = on;

	return 0;
}

#define REG_CORE_POWER	"8921_lvs7"
static int hdmi_core_power(int on, int show)
{
	static struct regulator *reg;
	static int prev_on;
	int rc;

	if (on == prev_on)
		return 0;

	if (!reg) {
		reg = regulator_get(&hdmi_msm_device.dev, REG_CORE_POWER);
		if (IS_ERR(reg)) {
			pr_err("could not get %s, rc = %ld\n",
				REG_CORE_POWER, PTR_ERR(reg));
			return -ENODEV;
		}
	}
	if (on) {
		rc = regulator_enable(reg);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				REG_CORE_POWER, rc);
			return rc;
		}

		pr_info("%s(on): success\n", __func__);
	} else {
		rc = regulator_disable(reg);
		if (rc) {
			pr_err("disable %s failed, rc=%d\n", REG_CORE_POWER, rc);
			return -ENODEV;
		}
		pr_info("%s(off): success\n", __func__);
	}
	prev_on = on;
	return rc;
}
#endif 

static struct ramdump_platform_data ramdump_data_2G = {
	.count = 1,
	.region = {
		{
			.start	= 0x90000000,
			.size	= 0x70000000,
		},
	}
};

static struct ramdump_platform_data ramdump_data_1G = {
	.count = 1,
	.region = {
		{
			.start	= 0x90000000,
			.size	= 0x30000000,
		},
	}
};

struct platform_device device_htc_ramdump = {
	.name		= "htc_ramdump",
	.id		= 0,
	.dev = {.platform_data = &ramdump_data_1G},
};

static struct platform_device *common_devices[] __initdata = {
	&msm8064_device_acpuclk,
	&ram_console_device,
	&apq8064_device_dmov,
	&apq8064_device_qup_i2c_gsbi1,
	&apq8064_device_qup_i2c_gsbi2,
	&apq8064_device_qup_i2c_gsbi3,
	&apq8064_device_qup_i2c_gsbi4,
	&apq8064_device_qup_spi_gsbi5,
	&monarudo_device_ext_5v_vreg,
	&monarudo_device_ext_mpp8_vreg,
	&monarudo_device_ext_3p3v_vreg,
	&monarudo_device_ext_ts_sw_vreg,
	&apq8064_device_ssbi_pmic1,
	&apq8064_device_ssbi_pmic2,
	&msm_device_smd_apq8064,
	&apq8064_device_otg,
	&apq8064_device_hsusb_host,
	&msm_device_wcnss_wlan,
	&apq8064_fmem_device,
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	&monarudo_android_pmem_device,
	&monarudo_android_pmem_adsp_device,
	&monarudo_android_pmem_audio_device,
#endif 
#endif 
#ifdef CONFIG_ION_MSM
	&monarudo_ion_dev,
#endif
#ifdef CONFIG_QSEECOM
	&qseecom_device,
#endif
	&msm8064_device_watchdog,
	&msm8064_device_saw_regulator_core0,
	&msm8064_device_saw_regulator_core1,
	&msm8064_device_saw_regulator_core2,
	&msm8064_device_saw_regulator_core3,
#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)
	&qcrypto_device,
#endif

#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)
	&qcedev_device,
#endif

#ifdef CONFIG_HW_RANDOM_MSM
	&apq8064_device_rng,
#endif
	&apq_pcm,
	&apq_pcm_routing,
	&apq_cpudai0,
	&apq_cpudai1,
	&apq_cpudai_hdmi_rx,
	&apq_cpudai_bt_rx,
	&apq_cpudai_bt_tx,
	&apq_cpudai_fm_rx,
	&apq_cpudai_fm_tx,
	&apq_cpu_fe,
	&apq_stub_codec,
	&apq_voice,
	&apq_voip,
	&apq_lpa_pcm,
	&apq_pcm_hostless,
	&apq_cpudai_afe_01_rx,
	&apq_cpudai_afe_01_tx,
	&apq_cpudai_afe_02_rx,
	&apq_cpudai_afe_02_tx,
	&apq_pcm_afe,
	&apq_cpudai_pri_i2s_rx,
	&apq_cpudai_pri_i2s_tx,
#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
	&hdmi_msm_device,
#endif
	&apq_cpudai_auxpcm_rx,
	&apq_cpudai_auxpcm_tx,
	&apq_cpudai_stub,
	&apq_cpudai_slimbus_1_rx,
	&apq_cpudai_slimbus_1_tx,
	&apq_cpudai_slimbus_2_tx,
	&apq_cpudai_slimbus_3_rx,
        &apq_cpudai_slim_4_rx,
	&apq_cpudai_slim_4_tx,
	&apq8064_rpm_device,
	&apq8064_rpm_log_device,
	&apq8064_rpm_stat_device,
	&msm_bus_8064_apps_fabric,
	&msm_bus_8064_sys_fabric,
	&msm_bus_8064_mm_fabric,
	&msm_bus_8064_sys_fpb,
	&msm_bus_8064_cpss_fpb,
	&apq8064_msm_device_vidc,
	&msm_8960_riva,
	&msm_8960_q6_lpass,
	&msm_pil_vidc,
	&msm_gss,
#ifdef CONFIG_MSM_RTB
	&monarudo_rtb_device,
#endif
	&msm8960_gemini_device,
#ifdef CONFIG_BT
	&msm_device_uart_dm6,
	&monarudo_rfkill,
#endif

#ifdef CONFIG_HTC_BATT_8960
	&htc_battery_pdev,
#endif
#ifdef CONFIG_MSM_ROTATOR
	&msm_rotator_device,
#endif
	&msm8960_cpu_idle_device,
	&msm8960_msm_gov_device,
	&msm_tsens_device,
	&msm_device_tz_log,
	&apq8064_iommu_domain_device,
	&monarudo_cpu_usage_stats_device,   
#ifdef CONFIG_MSM_CACHE_ERP
	&apq8064_device_cache_erp,
#endif
#ifdef CONFIG_PERFLOCK
	&msm8064_device_perf_lock,
#endif
	&apq_compr_dsp,
	&apq_multi_ch_pcm,
};

static struct platform_device *cdp_devices[] __initdata = {
	&apq8064_device_uart_gsbi1,
	&apq8064_device_uart_gsbi2,
	&apq8064_device_uart_gsbi7,
	&msm_cpudai_mi2s,
	&msm_device_sps_apq8064,
};

static struct msm_spi_platform_data monarudo_qup_spi_gsbi5_pdata = {
	.max_clock_speed = 1100000,
};

#define KS8851_IRQ_GPIO		43

#if 0	
static struct spi_board_info spi_board_info[] __initdata = {
	{
		.modalias               = "ks8851",
		.irq                    = MSM_GPIO_TO_INT(KS8851_IRQ_GPIO),
		.max_speed_hz           = 19200000,
		.bus_num                = 0,
		.chip_select            = 2,
		.mode                   = SPI_MODE_0,
	},
};
#endif

#ifdef CONFIG_MSM_CAMERA
#ifdef CONFIG_RAWCHIP
static struct spi_board_info rawchip_spi_board_info[] __initdata = {
	{
		.modalias               = "spi_rawchip",
		.max_speed_hz           = 27000000,
		.bus_num                = 0,
		.chip_select            = 0,
		.mode                   = SPI_MODE_0,
	},
};
#endif
#endif

static struct slim_boardinfo monarudo_slim_devices[] = {
	{
		.bus_num = 1,
		.slim_slave = &monarudo_slim_tabla,
	},
	{
		.bus_num = 1,
		.slim_slave = &monarudo_slim_tabla20,
	},
	
};

static struct msm_i2c_platform_data monarudo_i2c_qup_gsbi1_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
};

static struct msm_i2c_platform_data apq8064_i2c_qup_gsbi2_pdata = {
	.clk_freq = 375000,
	.src_clk_rate = 24000000,
	
};


static struct msm_i2c_platform_data monarudo_i2c_qup_gsbi3_pdata = {
	.clk_freq = 384000,
	.src_clk_rate = 24000000,
	
};

static struct msm_i2c_platform_data monarudo_i2c_qup_gsbi4_pdata = {
	.clk_freq = 384000,
	.src_clk_rate = 24000000,
};

#define GSBI_DUAL_MODE_CODE 0x60
#define MSM_GSBI1_PHYS		0x12440000
static void __init monarudo_i2c_init(void)
{
	void __iomem *gsbi_mem;

	apq8064_device_qup_i2c_gsbi1.dev.platform_data =
					&monarudo_i2c_qup_gsbi1_pdata;
	gsbi_mem = ioremap_nocache(MSM_GSBI1_PHYS, 4);
	writel_relaxed(GSBI_DUAL_MODE_CODE, gsbi_mem);
	/* Ensure protocol code is written before proceeding */
	wmb();
	iounmap(gsbi_mem);
	monarudo_i2c_qup_gsbi1_pdata.use_gsbi_shared_mode = 1;

	apq8064_device_qup_i2c_gsbi2.dev.platform_data =
					&apq8064_i2c_qup_gsbi2_pdata;
	apq8064_device_qup_i2c_gsbi3.dev.platform_data =
					&monarudo_i2c_qup_gsbi3_pdata;
	apq8064_device_qup_i2c_gsbi4.dev.platform_data =
					&monarudo_i2c_qup_gsbi4_pdata;
}

#if defined(CONFIG_KS8851) || defined(CONFIG_KS8851_MODULE)
static int ethernet_init(void)
{
	int ret;
#if 0	
	ret = gpio_request(KS8851_IRQ_GPIO, "ks8851_irq");
#else
	ret = 0;
#endif	
	if (ret) {
		pr_err("ks8851 gpio_request failed: %d\n", ret);
		goto fail;
	}

	return 0;
fail:
	return ret;
}
#else
static int ethernet_init(void)
{
	return 0;
}
#endif

#define GPIO_KEY_HOME		PM8921_GPIO_PM_TO_SYS(27)
#define GPIO_KEY_VOLUME_UP	PM8921_GPIO_PM_TO_SYS(35)
#define GPIO_KEY_VOLUME_DOWN	PM8921_GPIO_PM_TO_SYS(38)
#define GPIO_KEY_CAM_FOCUS	PM8921_GPIO_PM_TO_SYS(3)
#define GPIO_KEY_CAM_SNAP	PM8921_GPIO_PM_TO_SYS(4)
#define GPIO_KEY_ROTATION	46

static struct gpio_keys_button cdp_keys[] = {
	{
		.code           = KEY_HOME,
		.gpio           = GPIO_KEY_HOME,
		.desc           = "home_key",
		.active_low     = 1,
		.type		= EV_KEY,
		.wakeup		= 1,
		.debounce_interval = 15,
	},
	{
		.code           = KEY_VOLUMEUP,
		.gpio           = GPIO_KEY_VOLUME_UP,
		.desc           = "volume_up_key",
		.active_low     = 1,
		.type		= EV_KEY,
		.wakeup		= 1,
		.debounce_interval = 15,
	},
	{
		.code           = KEY_VOLUMEDOWN,
		.gpio           = GPIO_KEY_VOLUME_DOWN,
		.desc           = "volume_down_key",
		.active_low     = 1,
		.type		= EV_KEY,
		.wakeup		= 1,
		.debounce_interval = 15,
	},
	{
		.code           = SW_ROTATE_LOCK,
		.gpio           = GPIO_KEY_ROTATION,
		.desc           = "rotate_key",
		.active_low     = 1,
		.type		= EV_SW,
		.debounce_interval = 15,
	},
};

static struct gpio_keys_platform_data cdp_keys_data = {
	.buttons        = cdp_keys,
	.nbuttons       = ARRAY_SIZE(cdp_keys),
};

static struct platform_device cdp_kp_pdev = {
	.name           = "gpio-keys",
	.id             = -1,
	.dev            = {
		.platform_data  = &cdp_keys_data,
	},
};

#define DSPS_PIL_GENERIC_NAME          "dsps"
static void __init monarudo_init_dsps(void)
{
       struct msm_dsps_platform_data *pdata =
               msm_dsps_device_8064.dev.platform_data;
       pdata->pil_name = DSPS_PIL_GENERIC_NAME;
       pdata->gpios = NULL;
       pdata->gpios_num = 0;

       platform_device_register(&msm_dsps_device_8064);
}

#define I2C_SURF 1
#define I2C_FFA  (1 << 1)
#define I2C_RUMI (1 << 2)
#define I2C_SIM  (1 << 3)
#define I2C_LIQUID (1 << 4)

struct i2c_registry {
	u8                     machs;
	int                    bus;
	struct i2c_board_info *info;
	int                    len;
};

static int monarudo_mpu3050_sensor_power_LPM(int on)
{
	int rc = 0;

	mutex_lock(&sensor_lock);

	printk(KERN_DEBUG "[MPU][MPL3.3.7] %s, on = %d, "
			  "motion_sensor_vreg_8921_l17 = 0x%p\n",
			  __func__, on, motion_sensor_vreg_8921_l17);

	if (!motion_sensor_vreg_8921_l17)
		_GET_REGULATOR(motion_sensor_vreg_8921_l17, "8921_l17");

	if (on) {
		rc = regulator_set_optimum_mode(motion_sensor_vreg_8921_l17,
						100);
		if (rc < 0) {
			pr_err("[MPU][MPL3.3.7] set_optimum_mode L17 to LPM"
				" failed, rc = %d\n", rc);
			mutex_unlock(&sensor_lock);
			printk(KERN_DEBUG "[MPU][MPL3.3.7]%s unlock 1\n",
					  __func__);
			return -EINVAL;
		}
		rc = regulator_enable(motion_sensor_vreg_8921_l17);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"motion_sensor_vreg_8921_l17", rc);
			return rc;
		}
		printk(KERN_DEBUG "[MPU][MPL3.3.7] %s, Set to Low Power"
			" Mode\n", __func__);
	} else {
		rc = regulator_set_optimum_mode(motion_sensor_vreg_8921_l17,
						100000);
		if (rc < 0) {
			pr_err("[MPU][MPL3.3.7] set_optimum_mode L17 to"
				" Normal mode failed, rc = %d\n", rc);
			mutex_unlock(&sensor_lock);
			printk(KERN_DEBUG "[MPU][MPL3.3.7]%s unlock 2\n",
					  __func__);
			return -EINVAL;
		}
		rc = regulator_enable(motion_sensor_vreg_8921_l17);
		if (rc) {
			pr_err("'%s' regulator enable failed, rc=%d\n",
				"motion_sensor_vreg_8921_l17", rc);
			return rc;
		}
		printk(KERN_DEBUG "[MPU][MPL3.3.7] %s, Set to Normal Mode\n",
			__func__);
	}
	mutex_unlock(&sensor_lock);
	printk(KERN_DEBUG "[MPU][MPL3.3.7]%s unlock 3\n", __func__);
	return 0;
}

static struct mpu3050_platform_data mpu3050_data = {
	.int_config = 0x10,
	.orientation = { -1, 0,  0,
			  0, 1,  0,
			  0, 0, -1 },
	.level_shifter = 0,

	.accel = {
		.get_slave_descr = get_accel_slave_descr,
		.adapt_num = MSM8064_GSBI2_QUP_I2C_BUS_ID, 
		.bus = EXT_SLAVE_BUS_SECONDARY,
		.address = 0x30 >> 1,
			.orientation = { -1, 0,  0,
					  0, 1,  0,
					  0, 0, -1 },

	},
	.compass = {
		.get_slave_descr = get_compass_slave_descr,
		.adapt_num = MSM8064_GSBI2_QUP_I2C_BUS_ID, 
		.bus = EXT_SLAVE_BUS_PRIMARY,
		.address = 0x1A >> 1,
			.orientation = { -1, 0,  0,
					  0, 1,  0,
					  0, 0, -1},
	},
	.power_LPM = monarudo_mpu3050_sensor_power_LPM,
};

static struct i2c_board_info __initdata mpu3050_GSBI12_boardinfo[] = {
	{
		I2C_BOARD_INFO("mpu3050", 0xD0 >> 1),
		.irq = PM8921_GPIO_IRQ(PM8921_IRQ_BASE, GYRO_INT),
		.platform_data = &mpu3050_data,
	},
};
static struct i2c_board_info pwm_i2c_devices[] = {
	{
		I2C_BOARD_INFO("pwm_i2c", 0x70 >> 1),
	},
};

static struct pn544_i2c_platform_data nfc_platform_data = {
	.irq_gpio = NFC_IRQ,
	.ven_gpio = NFC_VEN,
	.firm_gpio = NFC_DL_MODE_XC,
	.ven_isinvert = 1,
};

static struct i2c_board_info pn544_i2c_boardinfo[] = {
	{
		I2C_BOARD_INFO(PN544_I2C_NAME, 0x50 >> 1),
		.platform_data = &nfc_platform_data,
		.irq = MSM_GPIO_TO_INT(NFC_IRQ),
	},
};

static struct i2c_registry monarudo_i2c_devices[] __initdata = {
	{
		I2C_SURF | I2C_FFA,
		APQ_8064_GSBI3_QUP_I2C_BUS_ID,
		msm_i2c_gsbi3_info,
		ARRAY_SIZE(msm_i2c_gsbi3_info),
	},
	{
		I2C_SURF | I2C_FFA,
		APQ_8064_GSBI1_QUP_I2C_BUS_ID,
		msm_i2c_gsbi1_tfa9887_info,
		ARRAY_SIZE(msm_i2c_gsbi1_tfa9887_info),
	},
	{
		I2C_SURF | I2C_FFA,
		APQ_8064_GSBI1_QUP_I2C_BUS_ID,
		msm_i2c_gsbi1_tpa6185_info,
		ARRAY_SIZE(msm_i2c_gsbi1_tpa6185_info),
	},
	{
		I2C_SURF | I2C_FFA,
		APQ_8064_GSBI1_QUP_I2C_BUS_ID,
		msm_i2c_gsbi1_rt5501_info,
		ARRAY_SIZE(msm_i2c_gsbi1_rt5501_info),
	},
	{
		I2C_SURF | I2C_FFA,
		MSM8064_GSBI2_QUP_I2C_BUS_ID,
		i2c_CM36282_devices,
		ARRAY_SIZE(i2c_CM36282_devices),
	},
	{
		I2C_SURF | I2C_FFA,
		MSM8064_GSBI2_QUP_I2C_BUS_ID,
		pn544_i2c_boardinfo,
		ARRAY_SIZE(pn544_i2c_boardinfo),
	},
	{
		I2C_SURF | I2C_FFA,
		MSM8064_GSBI2_QUP_I2C_BUS_ID,
		pwm_i2c_devices,
		ARRAY_SIZE(pwm_i2c_devices),
	},
#ifdef CONFIG_FB_MSM_HDMI_MHL
#ifdef CONFIG_FB_MSM_HDMI_MHL_SII9234
	{
		I2C_SURF | I2C_FFA,
		APQ_8064_GSBI1_QUP_I2C_BUS_ID,
		msm_i2c_mhl_sii9234_info,
		ARRAY_SIZE(msm_i2c_mhl_sii9234_info),
	},
#endif
#endif
};

#ifdef CONFIG_RESET_BY_CABLE_IN
static uint32_t ac_reset_xb_gpio_table[] = {
	GPIO_CFG(AC_WDT_RST_XB, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};
static uint32_t ac_reset_xc_gpio_table[] = {
	GPIO_CFG(AC_WDT_RST_XC, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

void reset_dflipflop(void)
{
	
	if (system_rev == XB) {
		gpio_tlmm_config(ac_reset_xb_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_set_value(AC_WDT_RST_XB, 0);
		pr_info("[CABLE] Clear D Flip-Flop\n");
		udelay(100);
		gpio_set_value(AC_WDT_RST_XB, 1);
		pr_info("[CABLE] Restore D Flip-Flop\n");
	}
	
	if (system_rev > XB) {
		gpio_tlmm_config(ac_reset_xc_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_set_value(AC_WDT_RST_XC, 0);
		pr_info("[CABLE] Clear D Flip-Flop\n");
		udelay(100);
		gpio_set_value(AC_WDT_RST_XC, 1);
		pr_info("[CABLE] Restore D Flip-Flop\n");
	}
}
#endif

static void __init register_i2c_devices(void)
{
	u8 mach_mask = 0;
	int i;

#ifdef CONFIG_MSM_CAMERA
	struct i2c_registry monarudo_camera_i2c_devices = {
		I2C_SURF | I2C_FFA | I2C_LIQUID | I2C_RUMI,
		APQ_8064_GSBI4_QUP_I2C_BUS_ID,
		monarudo_camera_board_info.board_info,
		monarudo_camera_board_info.num_i2c_board_info,
	};
	struct i2c_registry monarudo_camera_i2c_devices_xd = {
		I2C_SURF | I2C_FFA | I2C_LIQUID | I2C_RUMI,
		APQ_8064_GSBI4_QUP_I2C_BUS_ID,
		monarudo_camera_board_info_xd.board_info,
		monarudo_camera_board_info_xd.num_i2c_board_info,
	};
#endif
	
	mach_mask = I2C_SURF;

#ifdef CONFIG_FB_MSM_HDMI_MHL
#ifdef CONFIG_FB_MSM_HDMI_MHL_SII9234
	
	if (system_rev <= XB)
		mhl_sii9234_device_data.gpio_reset = MHL_RSTz_XA;
	else if (system_rev >= XC)
		mhl_sii9234_device_data.gpio_reset = PM8921_GPIO_PM_TO_SYS(MHL_RSTz_XC_XD);
#endif
#endif
	
	for (i = 0; i < ARRAY_SIZE(monarudo_i2c_devices); ++i) {
		if (monarudo_i2c_devices[i].machs & mach_mask)
			i2c_register_board_info(monarudo_i2c_devices[i].bus,
						monarudo_i2c_devices[i].info,
						monarudo_i2c_devices[i].len);
	}

#ifdef CONFIG_FLASHLIGHT_TPS61310
	
	if (system_rev <= XB) {
		if((I2C_SURF | I2C_FFA) & mach_mask) {
			i2c_register_board_info(MSM8064_GSBI2_QUP_I2C_BUS_ID,
				i2c_tps61310_flashlight, ARRAY_SIZE(i2c_tps61310_flashlight));
		}
	}
	
	if (system_rev > XB) {
		if((I2C_SURF | I2C_FFA) & mach_mask) {
			i2c_register_board_info(MSM8064_GSBI2_QUP_I2C_BUS_ID,
				i2c_tps61310_flashlight_XC, ARRAY_SIZE(i2c_tps61310_flashlight_XC));
		}
	}
#endif

#ifdef CONFIG_MSM_CAMERA
	if ( system_rev <= XC ) {
		if (monarudo_camera_i2c_devices.machs & mach_mask)
			i2c_register_board_info(monarudo_camera_i2c_devices.bus,
				monarudo_camera_i2c_devices.info,
				monarudo_camera_i2c_devices.len);
	} else 	if ( system_rev == XD) {
		if (monarudo_camera_i2c_devices_xd.machs & mach_mask)
			i2c_register_board_info(monarudo_camera_i2c_devices_xd.bus,
				monarudo_camera_i2c_devices_xd.info,
				monarudo_camera_i2c_devices_xd.len);
	}

#endif
	if (gy_type == 2) {
		i2c_register_board_info(MSM8064_GSBI2_QUP_I2C_BUS_ID,
				motion_sensor_gsbi_2_info,
				ARRAY_SIZE(motion_sensor_gsbi_2_info));
	} else {
		i2c_register_board_info(MSM8064_GSBI2_QUP_I2C_BUS_ID,
				mpu3050_GSBI12_boardinfo,
				ARRAY_SIZE(mpu3050_GSBI12_boardinfo));
	}
}

static void __init monarudo_common_init(void)
{
	int rc = 0;
	struct kobject *properties_kobj;

	msm_thermal_init(&msm_thermal_pdata);

	if (socinfo_init() < 0)
		pr_err("socinfo_init() failed!\n");

	pr_info("%s: platform_subtype = %d\r\n", __func__,
		socinfo_get_platform_subtype());
	pr_info("%s: socinf version = %u.%u\r\n", __func__,
		SOCINFO_VERSION_MAJOR(socinfo_get_version()),
		SOCINFO_VERSION_MINOR(socinfo_get_version()));

	BUG_ON(msm_rpm_init(&apq8064_rpm_data));
	BUG_ON(msm_rpmrs_levels_init(&msm_rpmrs_data));
	msm_rpmrs_lpm_init(1, 1, 1, 1);
	regulator_suppress_info_printing();
	platform_device_register(&monarudo_device_rpm_regulator);
	if (msm_xo_init())
		pr_err("Failed to initialize XO votes\n");
	
	if (system_rev <= XC)
		clk_ignor_list_add("msm_sdcc.3", "core_clk", &apq8064_clock_init_data);
	else if (system_rev >= XD)
		clk_ignor_list_add("msm_sdcc.3", "core_clk", &apq8064_clock_init_data_r2);
	
	if ( system_rev <= XC )
	msm_clock_init(&apq8064_clock_init_data);
	else if ( system_rev >= XD )
		msm_clock_init(&apq8064_clock_init_data_r2);
	monarudo_init_gpiomux();
#ifdef CONFIG_RESET_BY_CABLE_IN
	pr_info("[CABLE] Enable Ac Reset Function.(%d) \n", system_rev);
	
	if (system_rev == XB) {
		gpio_tlmm_config(ac_reset_xb_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_set_value(AC_WDT_RST_XB, 1);
	}
	
	if (system_rev > XB) {
		gpio_tlmm_config(ac_reset_xc_gpio_table[0], GPIO_CFG_ENABLE);
		gpio_set_value(AC_WDT_RST_XC, 1);
	}
#endif

	
	if (system_rev == XB)
		nfc_platform_data.firm_gpio = PM8921_GPIO_PM_TO_SYS(NFC_DL_MODE_XA_XB);

	monarudo_i2c_init();

	if (system_rev < XB) {
		for (rc = 0; rc < ARRAY_SIZE(msm_i2c_gsbi3_info); rc++) {
			if (!strcmp(msm_i2c_gsbi3_info[rc].type, SYNAPTICS_3200_NAME))
				msm_i2c_gsbi3_info[rc].platform_data = &syn_ts_3k_2p5D_3030_evita_data;
		}
		if (board_build_flag() == 1) {
			for (rc = 0; rc < ARRAY_SIZE(syn_ts_3k_2p5D_3030_evita_data);  rc++)
				syn_ts_3k_2p5D_3030_evita_data[rc].mfg_flag = 1;
		}
	} else {
		if (board_build_flag() == 1) {
			for (rc = 0; rc < ARRAY_SIZE(syn_ts_3k_data);  rc++)
				syn_ts_3k_data[rc].mfg_flag = 1;
		}
	}

#ifdef CONFIG_BT
	
	if (system_rev >= XC) {
		bt_export_bd_address();
		msm_uart_dm6_pdata.wakeup_irq = PM8921_GPIO_IRQ(PM8921_IRQ_BASE, BT_HOST_WAKE_XC);
		msm_device_uart_dm6.name = "msm_serial_hs_brcm";
		msm_device_uart_dm6.dev.platform_data = &msm_uart_dm6_pdata;
	}
#endif

	register_i2c_devices();

	apq8064_device_qup_spi_gsbi5.dev.platform_data =
						&monarudo_qup_spi_gsbi5_pdata;
	monarudo_init_pmic();

	android_usb_pdata.swfi_latency =
			msm_rpmrs_levels[0].latency_us;

	apq8064_device_otg.dev.platform_data = &msm_otg_pdata;
	monarudo_init_buses();
#ifdef CONFIG_HTC_BATT_8960
	htc_battery_cell_init(htc_battery_cells, ARRAY_SIZE(htc_battery_cells));
#endif 

	platform_add_devices(common_devices, ARRAY_SIZE(common_devices));

	if(board_mfg_mode() == 9) {
		if (board_fullramdump_flag())
			device_htc_ramdump.dev.platform_data = &ramdump_data_2G;
		platform_device_register(&device_htc_ramdump);
	}

	if (system_rev < XC)
		platform_device_register(&vibrator_pwm_device);
	else if (system_rev == XC)
		platform_device_register(&vibrator_pwm_device_XC);
	else
        platform_device_register(&vibrator_pwm_device_XD);

	apq8064_device_hsic_host.dev.platform_data = &msm_hsic_pdata;
	msm_hsic_pdata.swfi_latency = msm_rpmrs_levels[0].latency_us;
	device_initialize(&apq8064_device_hsic_host.dev);
	monarudo_pm8xxx_gpio_mpp_init();
	monarudo_init_mmc();


	
	monarudo_wifi_init();
	



	pr_info("%s: Add MDM2 device\n", __func__);
	mdm_8064_device.dev.platform_data = &mdm_platform_data;
	if (system_rev >= XC) {
		mdm_8064_device.resource[5].start = MDM2AP_HSIC_READY_XC;
		mdm_8064_device.resource[5].end = MDM2AP_HSIC_READY_XC;
		mdm_8064_device.resource[6].start = AP2MDM_WAKEUP_XC;
		mdm_8064_device.resource[6].end = AP2MDM_WAKEUP_XC;
	}
	platform_device_register(&mdm_8064_device);

	platform_device_register(&apq8064_slim_ctrl);
	slim_register_board_info(monarudo_slim_devices,
		ARRAY_SIZE(monarudo_slim_devices));
	monarudo_init_dsps();
	msm_spm_init(msm_spm_data, ARRAY_SIZE(msm_spm_data));
	msm_spm_l2_init(msm_spm_l2_data);
#if 0 
	msm_pm_set_platform_data(msm_pm_data, ARRAY_SIZE(msm_pm_data));
	msm_cpuidle_set_states(msm_cstates, ARRAY_SIZE(msm_cstates),
				msm_pm_data);
#endif
	BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));
	msm_pm_init_sleep_status_data(&msm_pm_slp_sts_data);
	properties_kobj = kobject_create_and_add("board_properties", NULL);
	if (properties_kobj) {
		if (system_rev < XB) {
			rc = sysfs_create_group(properties_kobj, &properties_evita_attr_group);
			if (!rc)
				for (rc = 0; rc < ARRAY_SIZE(syn_ts_3k_2p5D_3030_evita_data); rc++) {
					syn_ts_3k_2p5D_3030_evita_data[rc].vk_obj = properties_kobj;
					syn_ts_3k_2p5D_3030_evita_data[rc].vk2Use = &syn_virtual_keys_evita_attr;
				}
		} else {
			rc = sysfs_create_group(properties_kobj, &properties_attr_group);
			if (!rc)
				for (rc = 0; rc < ARRAY_SIZE(syn_ts_3k_data); rc++) {
					syn_ts_3k_data[rc].vk_obj = properties_kobj;
					syn_ts_3k_data[rc].vk2Use = &syn_virtual_keys_attr;
				}
		}
	}

	headset_device_register();

	monarudo_init_keypad();

	if ((get_kernel_flag() & KERNEL_FLAG_PM_MONITOR) ||
		(!(get_kernel_flag() & KERNEL_FLAG_TEST_PWR_SUPPLY) && (!get_tamper_sf()))) {
		htc_monitor_init();
		htc_pm_monitor_init();
		pm_monitor_enabled = 1;
	}
	
	else
		pm_monitor_enabled = 0;

	
	htc_top_monitor_init();
	htc_kernel_top_monitor_init	();
	

#ifdef CONFIG_SUPPORT_USB_SPEAKER
	pm_qos_add_request(&pm_qos_req_dma, PM_QOS_CPU_DMA_LATENCY, PM_QOS_DEFAULT_VALUE);
#endif

}

static void __init monarudo_allocate_memory_regions(void)
{
	monarudo_allocate_fb_region();
}

static void __init monarudo_cdp_init(void)
{
#if 1
	struct pm_gpio pm_config;
#endif
	pr_info("%s: init starts\r\n", __func__);
	msm_tsens_early_init(&apq_tsens_pdata);
	monarudo_common_init();
	ethernet_init();
	msm_rotator_set_split_iommu_domain();
	platform_add_devices(cdp_devices, ARRAY_SIZE(cdp_devices));

#if 1	
#ifdef CONFIG_MSM_CAMERA
#ifdef CONFIG_RAWCHIP
	spi_register_board_info(rawchip_spi_board_info, ARRAY_SIZE(rawchip_spi_board_info));
#endif
#endif
#else
	spi_register_board_info(spi_board_info, ARRAY_SIZE(spi_board_info));
#endif	


	msm_rotator_update_bus_vectors(1920, 1080);
	monarudo_init_fb();
	monarudo_init_gpu();

	platform_add_devices(apq8064_footswitch, apq8064_num_footswitch);
#ifdef CONFIG_MSM_CAMERA
	monarudo_init_cam();
#endif
	platform_device_register(&cdp_kp_pdev);
	
	if (!(board_mfg_mode() == 6 || board_mfg_mode() == 7))
		monarudo_add_usb_devices();

#if 1 
	if (system_rev >= XC) {
		pm_config.direction = PM_GPIO_DIR_OUT;
		pm_config.output_buffer = PM_GPIO_OUT_BUF_CMOS;
		pm_config.output_value = 0;
		pm_config.pull = PM_GPIO_PULL_NO;
		pm_config.vin_sel = PM_GPIO_VIN_S4;
		pm_config.out_strength = PM_GPIO_STRENGTH_LOW;
		pm_config.function = PM_GPIO_FUNC_1;
		pm_config.inv_int_pol = 0;
		pm_config.disable_pin = 0;
		pm8xxx_gpio_config(PM8921_GPIO_PM_TO_SYS(PMGPIO(BCM4330_SLEEP_CLK_XC)), &pm_config);
		mdelay(50);
	}
#endif
}

#define PHY_BASE_ADDR1  0x80600000
#define SIZE_ADDR1      (136 * 1024 * 1024)

#define PHY_BASE_ADDR2  0x89000000
#define SIZE_ADDR2      (63 * 1024 * 1024)

#define PHY_BASE_ADDR3  0x90000000
#define SIZE_ADDR3      (768 * 1024 * 1024)

#define DDR_1GB_SIZE      (1024 * 1024 * 1024)

static void __init monarudo_fixup(struct tag *tags, char **cmdline, struct meminfo *mi)
{
	mem_size_mb = parse_tag_memsize((const struct tag *)tags);
	printk(KERN_DEBUG "%s: mem_size_mb=%u, mfg_mode = %d \n", __func__, mem_size_mb, board_mfg_mode());

	mi->nr_banks = 3;
	mi->bank[0].start = PHY_BASE_ADDR1;
	mi->bank[0].size = SIZE_ADDR1;
	mi->bank[1].start = PHY_BASE_ADDR2;
	mi->bank[1].size = SIZE_ADDR2;
	mi->bank[2].start = PHY_BASE_ADDR3;
	mi->bank[2].size = SIZE_ADDR3;

	if (mem_size_mb == 2048)
		mi->bank[2].size += DDR_1GB_SIZE;

	if (mem_size_mb == 64) {
		mi->nr_banks = 2;
		mi->bank[0].start = PHY_BASE_ADDR1;
		mi->bank[0].size = SIZE_ADDR1;
		mi->bank[1].start = PHY_BASE_ADDR2;
		mi->bank[1].size = SIZE_ADDR2;
	}
}

MACHINE_START(MONARUDO, "HTC_DLX")
	.fixup = monarudo_fixup,
	.map_io = monarudo_map_io,
	.reserve = monarudo_reserve,
	.init_irq = monarudo_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = monarudo_cdp_init,
	.init_early = monarudo_allocate_memory_regions,
	.init_very_early = monarudo_early_reserve,
	.restart = msm_restart,
MACHINE_END
