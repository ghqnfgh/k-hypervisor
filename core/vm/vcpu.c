#include <core/vm/vcpu.h>
#include <debug.h>
#include <core/timer.h>
#include <arch/armv7.h>
#include <stdio.h>
#include <core/scheduler.h>
#include <core/sched/sched-config.h>
#include <string.h>
#include <stdlib.h>
#include <drivers/vdev/vdev_timer.h>

static struct list_head vcpu_list;
static int nr_vcpus = 0;
static int visit_time = 0;

void vcpu_setup()
{
    LIST_INITHEAD(&vcpu_list);

    if (nr_vcpus != 0) {
        /* Never this happend */
        nr_vcpus = 0;
    }
}

struct vcpu *vcpu_create(vcpu_type_t type)
{
    struct vcpu *vcpu = NULL;
    int i;

    vcpu = malloc(sizeof(struct vcpu));
    if (vcpu == VCPU_CREATE_FAILED) {
        return vcpu;
    }
    memset(vcpu, 0, sizeof(struct vcpu));

    vcpu->vcpuid = nr_vcpus++;
    vcpu->state = VCPU_DEFINED;
    vcpu->type = type;

    // TODO(casionwoo) : Initialize running_time and actual_running_time after time_module created
    // TODO(casionwoo) : Initialize period and deadline after menuconfig module created
    // FIXME(casionwoo): vcpu->map[] will be removed.
    for (i = 0; i < MAX_NR_IRQ; i++) {
        vcpu->map[i].enabled = GUEST_IRQ_DISABLE;
        vcpu->map[i].virq = VIRQ_INVALID;
        vcpu->map[i].pirq = PIRQ_INVALID;
    }

    LIST_ADDTAIL(&vcpu->head, &vcpu_list);

    return vcpu;
}

#define SET_VIRQMAP(map, _pirq, _virq) \
    do {                                 \
        map[_pirq].virq = _virq;   \
        map[_virq].pirq = _pirq;   \
    } while (0)


vcpu_state_t vcpu_init(struct vcpu *vcpu)
{
    arch_regs_init(&vcpu->regs);
    vcpu->regs.cp15.vmpidr |= vcpu->id;


#if CONFIG_VSERIAL
    // TODO(casionwoo): make it neat.
    switch (vcpu->vmid) {
    case 0:
        SET_VIRQMAP(vcpu->map, 37, 37);
        break;

    case 1:
        SET_VIRQMAP(vcpu->map, 39, 37);
        break;

    case 2:
        SET_VIRQMAP(vcpu->map, 40, 37);
        break;

    default:
        debug_print("virq_create error!\n");
        break;
    }
#endif

    // TODO(casionwoo) : Check the return value after scheduler status value defined
    vcpu->pcpuid = sched_vcpu_register(vcpu->vcpuid, schedconf_g_vcpu_to_pcpu_map[vcpu->vcpuid]);
    printf("sched_vcpu_register, vcpuid : %d is registered on pcpuid : %d\n", vcpu->vcpuid, vcpu->pcpuid);
    vcpu->state = VCPU_REGISTERED;

    init_vdev_timer(&vcpu->vtimer);

    return vcpu->state;
}

vcpu_state_t vcpu_start(struct vcpu *vcpu)
{
    if (vcpu->id == 0 && vcpu->type == VCPU_NORMAL) {
        sched_vcpu_attach(vcpu->vcpuid, schedconf_g_vcpu_to_pcpu_map[vcpu->vcpuid]);
        printf("sched_vcpu_register, vcpuid : %d is attatched on pcpuid : %d\n", vcpu->vcpuid, vcpu->pcpuid);
        vcpu->state = VCPU_ACTIVATED;
        return vcpu->state;
    }

    return VCPU_ACTIVATED;
}

vcpu_state_t vcpu_suspend(struct vcpu *vcpu, struct core_regs *regs)
{
    // save the vcpu context for resuming in the future
    vcpu_save(vcpu, regs);

    // detach the vcpu from run queue not to be scheduled
    sched_vcpu_detach(vcpu->vcpuid, schedconf_g_vcpu_to_pcpu_map[vcpu->vcpuid]);
    printf("sched_vcpu_detach, vcpuid : %d is detatched on pcpuid : %d\n", vcpu->vcpuid, vcpu->pcpuid);

    vcpu->state = VCPU_REGISTERED;
    return vcpu->state;
}

vcpu_state_t vcpu_delete(struct vcpu *vcpu)
{
    sched_vcpu_unregister(vcpu->vcpuid, schedconf_g_vcpu_to_pcpu_map[vcpu->vcpuid]);
    printf("sched_vcpu_unregister, vcpuid : %d is unregister on pcpuid : %d\n", vcpu->vcpuid, vcpu->pcpuid);

    LIST_DEL(&vcpu->head);

    free(vcpu);

    return VCPU_UNDEFINED;
}


#include <irq-chip.h>
#include "../../include/arch/gicv2_bit.h"

void vcpu_save(struct vcpu *vcpu, struct core_regs *regs)
{
    arch_regs_save(&vcpu->regs, regs);

    int i;
    for (i = 0; i < GICv2.num_lr; i++) {
        vcpu->lr[i] = GICH_READ(GICH_LR(i));
    }

    vcpu->vmcr = GICH_READ(GICH_VMCR);

    virq_hw->disable();
}

void vcpu_restore(struct vcpu *vcpu, struct core_regs *regs)
{
    arch_regs_restore(&vcpu->regs, regs);

    int i;
    for (i = 0; i < GICv2.num_lr; i++) {
        GICH_WRITE(GICH_LR(i), vcpu->lr[i]);
    }

    GICH_WRITE(GICH_VMCR, vcpu->vmcr);

    virq_hw->forward_pending_irq(vcpu->vcpuid);
    virq_hw->enable();
}

void vcpu_copy(struct vcpu *from, struct vcpu *to, struct core_regs *regs)
{
    int i;
    for (i = 0; i < GICv2.num_lr; i++)
        to->lr[i] = from->lr[i];

    to->vmcr = from->vmcr;
    to->regs.cp15.vmpidr = from->regs.cp15.vmpidr;
    arch_regs_copy(&from->regs, &to->regs, regs);

    for (i = 0; i < MAX_NR_IRQ; i++) {
        to->map[i].enabled = from->map[i].enabled;
        to->map[i].virq = from->map[i].virq;
        to->map[i].pirq = from->map[i].pirq;
    }
}

struct vcpu *vcpu_find(vcpuid_t vcpuid)
{
    visit_time++;
//    printf("[now nr_vcpus == %d]\n", nr_vcpus);
//    if (nr_vcpus != 3)
//        printf("wtf?\n");
//    if(visit_time %= 300 && nr_vcpus == 1) 
//        printf("[now nr_vcpus == 1]\n");
//    if(visit_time %= 300 && nr_vcpus == 2) 
//        printf("[now nr_vcpus == 2]\n");
    if(nr_vcpus == 1 && visit_time > 600){
        uint32_t pcpu = smp_processor_id();
        extern struct scheduler *sched[NR_CPUS];
        printf("curr:%x, next:%x \n", sched[pcpu]->current_vcpuid, sched[pcpu]->next_vcpuid);
    }
        
    struct vcpu *vcpu = NULL;
    list_for_each_entry(struct vcpu, vcpu, &vcpu_list, head) {
        if (vcpu->vcpuid == vcpuid) {
            return vcpu;
        }
    }
    return NULL;
}

void print_all_vcpu()
{
    struct vcpu *vcpu = NULL;
    list_for_each_entry(struct vcpu, vcpu, &vcpu_list, head) {
        print_vcpu(vcpu);
    }
}

void print_vcpu(struct vcpu *vcpu)
{
    printf("ADDR  : 0x%p\n", vcpu);
    printf("VCPUID  : %d\n", vcpu->vcpuid);
    printf("VMID  : %d\n", vcpu->vmid);
    printf("STATE  : %d\n", vcpu->state);
}

struct list_head *get_all_vcpus()
{
    return &vcpu_list;
}
