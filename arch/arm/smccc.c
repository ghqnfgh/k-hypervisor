#include <arch/armv7.h>
#include <arch/psci.h>
#include <core/scheduler.h>
#include <stdio.h>
#include <drivers/dma/sun4i-dma.h>
#include <core/kmus.h>
#include <size.h>

#include "paging.h"

#define nums 1024
#define SIZ SZ_32K*nums

//static Queue dma_waiting_queue; 
extern struct Queue trans_queue;

int emulate_arm_smccc(struct core_regs *regs)
{
    uint32_t function_id = regs->gpr[0];

    static int kmus_flag_id = 1;
//    struct vmcb *vm = get_current_vm();

    switch(function_id) {
    case 0x000051BA:{
///**********************kmus**************************************
        if(kmus_flag_id == 1){
            vcpuid_t vcpuid = get_current_vcpuid();

            printf("unregsiter: %d\n", vcpuid);
            sched_vcpu_detach(vcpuid, 0);
            sched_vcpu_unregister(vcpuid, 0);

            kmus_snapshot(0, 2, regs);

            kmus_flag_id = 0;
            
            //when do not use dma
//            sched_vcpu_register(0,0);
//            sched_vcpu_attach(0,0);
            
//            printf("is Registered - pcpu : %d\n", sched_vcpu_register(0,0));
        }
            //****************************************************************/
    
        }
        break;
    case 0x000051BB:
        if(kmus_flag_id == 0){
            vcpuid_t vcpuid = get_current_vcpuid();
            int i = 0;
            long long int counter = 0;
            for(i = 0; i < SZ_128M; i+=4){
                if(readl(0x48000000 + i) !=readl( 0x68000000 + i))
                    counter++;
            }
            printf("i : 0x%08x, off_4_counter : %lld\n", i, counter);

            printf("unregsiter: %d\n", vcpuid);
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
