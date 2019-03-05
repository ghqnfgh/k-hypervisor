#include <arch/armv7.h>
#include <arch/psci.h>
#include <core/scheduler.h>
#include <stdio.h>
#include <drivers/dma/sun4i-dma.h>
#include <core/kmus.h>
#include <size.h>

#include "paging.h"

extern struct Queue trans_queue;

int emulate_arm_smccc(struct core_regs *regs)
{
    uint32_t function_id = regs->gpr[0];

    static int kmus_flag_id = 1;

    switch(function_id) {
    case 0x000051BA:{
        if(kmus_flag_id == 1){
            vcpuid_t vcpuid = get_current_vcpuid();

            sched_vcpu_detach(vcpuid, 0);
            sched_vcpu_unregister(vcpuid, 0);

            kmus_snapshot(0, 2, regs);

            kmus_flag_id = 0;
        }
    
        }
        break;
    case 0x000051BB:
        if(kmus_flag_id == 0){
            vcpuid_t vcpuid = get_current_vcpuid();
            sched_vcpu_detach(vcpuid, 0);
            sched_vcpu_unregister(vcpuid, 0);
            kmus_snapshot(2, 0, regs);
//            kmus_flag_id = 1;
        }
        break;


#ifdef CONFIG_ARM_PSCI
    case PSCI_FN_CPU_ON:
        regs->gpr[0] = emulate_psci_cpu_on(regs);
        break;
#endif
    default:
        break;
    }

    return 0;
}
