#include <arch/armv7.h>
#include <arch/psci.h>
#include <stdio.h>
#include <drivers/dma/sun4i-dma.h>

#define SIZ 1024

uint32_t src[SIZ] = {0,};
uint32_t dst[SIZ] = {0,};

int emulate_arm_smccc(struct core_regs *regs)
{
    uint32_t function_id = regs->gpr[0];
    int i = 0;
    static int dma_flag_id = 1;

    switch(function_id) {
    case 0x000051BA:
        if(dma_flag_id){
            for(i = 0; i < SIZ; i++){
                src[i] = i;
                dst[i] = -1;
            }
            dma_transfer(0, (uint32_t)src, (uint32_t)dst, sizeof(uint32_t)*SIZ);
            dma_flag_id = 0;
        } else if(dma_flag_id == 0){
            printf("\ndst : ");
            for(i = 0; i < SIZ; i++)
                printf("%d,",dst[i]);
            for(i = 0; i < SIZ; i++)
                if (src[i] != dst[i]) {
                    printf("not yet...\n");
                    break;
                }
            printf("\n");
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
