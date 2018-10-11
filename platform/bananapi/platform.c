#include "platform.h"
#include <stdint.h>
#include <serial.h>

// TODO(wonseok): moved header files from arch/arm to proper dir.
#include "../../arch/arm/paging.h"

#include <size.h>
#include <arch/armv7.h>
#include <stdio.h>

#include <config.h>
#include <arch/irq.h>
#include "../../include/arch/gicv2_bit.h"

#include <vm_config.h>

void platform_init()
{
    uint32_t gic_base = (uint32_t)(get_periphbase() & 0x000000FFFFFFFFFFULL);
    int i = 0;

    gic_base = CFG_GIC_BASE_PA;
    paging_add_mapping(gic_base + GICD_OFFSET, gic_base + GICD_OFFSET, MT_DEVICE, SZ_4K);
    paging_add_mapping(gic_base + GICC_OFFSET, gic_base + GICC_OFFSET, MT_DEVICE, SZ_8K);

    paging_add_mapping(gic_base + GICH_OFFSET, gic_base + GICH_OFFSET, MT_DEVICE, SZ_8K);
    paging_add_mapping(gic_base + GICV_OFFSET, gic_base + GICV_OFFSET, MT_DEVICE, SZ_8K);

    // add mapping for serial devices
    paging_add_mapping(0x01c28000, 0x01c28000, MT_DEVICE, SZ_1K);
    // add mapping for dma
    paging_add_mapping(0x01c02000, 0x01c02000, MT_DEVICE, SZ_4K);

    paging_add_mapping(CFG_HYP_START_ADDRESS, CFG_HYP_START_ADDRESS, MT_WRITEBACK_RW_ALLOC, SZ_128M);
    for(i = 0;  i < CONFIG_NR_VMS; i++){
        paging_add_mapping(vm_conf[i].pa_start, vm_conf[i].pa_start, MT_WRITEBACK_RW_ALLOC, vm_conf[i].va_offsets);
    }
}

void console_init()
{
    // TODO(wonseok): add general initialization for console devices.
    serial_init();
}

#include <core/timer.h>

#if defined(CONFIG_DMA_ENGINE) && defined(CONFIG_SUN4I_DMA)
#include <drivers/dma/sun4i-dma.h>
#endif

void dev_init()
{
    // init .text.dev section like vdev.
    timer_hw_init(NS_PL2_PTIMER_IRQ);
#if defined(CONFIG_DMA_ENGINE) && defined(CONFIG_SUN4I_DMA)
    dma_irq_enable();
#endif
}
