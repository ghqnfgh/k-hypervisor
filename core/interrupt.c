#include <stdio.h>
#include <debug_print.h>

#include <hvmm_types.h>
#include <core/vm.h>
#include <core/vm/virq.h>
#include <core/interrupt.h>
#include <arch/armv7.h>

#include <rtsm-config.h>
#include <core/vm/vcpu.h>
#include "../arch/arm/arch_init.h"
#include <core/vm/virq.h>

#define VIRQ_MIN_VALID_PIRQ 16
#define VIRQ_NUM_MAX_PIRQS  MAX_IRQS

#define VALID_PIRQ(pirq) \
    (pirq >= VIRQ_MIN_VALID_PIRQ && pirq < VIRQ_NUM_MAX_PIRQS)

/**< IRQ handler */
static interrupt_handler_t interrupt_handlers[MAX_PPI_IRQS];

const int32_t interrupt_check_guest_irq(uint32_t pirq)
{
    int vmid, nr_vcpu;
    struct virqmap_entry *map;

    for (vmid = 0; vmid < NUM_GUESTS_STATIC; vmid++) {
        struct vmcb *vm = vm_find(vmid);

        for (nr_vcpu = 0; nr_vcpu < vm->num_vcpus; nr_vcpu++) {
            struct vcpu *vcpu = vm->vcpu[nr_vcpu];

            map = vcpu->virq.guest_virqmap->map;
            if (map[pirq].virq != VIRQ_INVALID) {
                return GUEST_IRQ;
            }
        }
    }

    return HOST_IRQ;
}

// TODO(casionwoo): Will be moved into vgic.c?
//static struct vgic_status _vgic_status[NUM_GUESTS_STATIC];

void interrupt_init()
{
    arch_irq_init();
}

void register_irq_handler(uint32_t irq, interrupt_handler_t handler)
{
    if (irq < MAX_IRQS)
        interrupt_handlers[irq] = handler;
}

#define __UART_IRQ_DEBUG__
static void interrupt_inject_enabled_guest(int num_of_guests, uint32_t irq)
{
    vcpuid_t vcpuid;
    uint32_t virq;
    struct vcpu *vcpu;

    for (vcpuid = 0; vcpuid < num_of_guests; vcpuid++) {
        vcpu = vcpu_find(vcpuid);
        virq = pirq_to_enabled_virq(&vcpu->virq, irq);
        if (virq == VIRQ_INVALID) {
            continue;
        }
#ifdef __UART_IRQ_DEBUG__
        if (irq != 34) {
            printf("vmid %d: pirq %d virq %d\n", vcpuid, irq, virq);
        }
#endif
        virq_inject(vcpuid, virq, irq, INJECT_HW);
    }
}

// The function as below will be moved into hvc_irq function.
void interrupt_service_routine(int irq, void *current_regs, void *pdata)
{
    struct arch_regs *regs = (struct arch_regs *)current_regs;

    if (irq < MAX_IRQS) {
        if (interrupt_check_guest_irq(irq) == GUEST_IRQ) {

            /* IRQ INJECTION */
            /* priority drop only for hanlding irq in guest */
            gic_completion_irq(irq);
            interrupt_inject_enabled_guest(NUM_GUESTS_STATIC, irq);
        } else {
            /* host irq */
            if (interrupt_handlers[irq])
                interrupt_handlers[irq](irq, regs, 0);

            gic_completion_irq(irq);
            gic_deactivate_irq(irq);
        }
    } else {
        debug_print("interrupt:no pending irq:%x\n", irq);
    }
}

