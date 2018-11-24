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

//void *src_ = (void *)0x90000000;
//void *dst_ = (void *)0xa0000000;
//static Queue dma_waiting_queue; 
extern struct Queue trans_queue;

int emulate_arm_smccc(struct core_regs *regs)
{
    uint32_t function_id = regs->gpr[0];
/************************dma_test********************************
    uint32_t *src = src_;
    uint32_t *dst = dst_;
    int i = 0;
    static int dma_flag_id = 1;
    int counter = 0;
    dma_wait *value = (dma_wait*)malloc(sizeof(dma_wait));
****************************************************************/

    static int kmus_flag_id = 1;
//    struct vmcb *vm = get_current_vm();

    switch(function_id) {
    case 0x000051BA:
/************************dma_test********************************
        if(dma_flag_id){

            for(i = 0; i < SIZ; i++){
                src[i] = i;
                dst[i] = -1;
            }

            InitQueue(&trans_queue);
            chain_enqueue((uint32_t)src, (uint32_t)dst, SIZ*sizeof(uint32_t));
            *value = Dequeue(&trans_queue);
            printf("%s, 0x%08x, 0x%08x, 0x%08x\n",__func__, value->src_addr, value->dst_addr, value->bc);
            dma_transfer(0, (uint32_t)value->src_addr, (uint32_t)value->dst_addr, value->bc);
            dma_flag_id = 0;
        } else if(dma_flag_id == 0){
            for(i = 0; i < SIZ; i++)
                if (src[i] == dst[i]) {
                    counter++;
                }

            for (i = 0; i < nums; i++) {
                int j;
                for (j = 0; j < SZ_32K; j++) {
                    if (src[i*SZ_32K + j] != dst[i*SZ_32K + j])
                        break;
                }
                if (j == SZ_32K) {
                    printf("correct %d\n", i);
                }
            }
****************************************************************/
///**********************kmus**************************************
        if(kmus_flag_id){
//            sched_vcpu_unregister(0,0);
//            printf("after unregister -----------------------------\n");

            kmus_snapshot(0, 2, regs);
//            printf("after snapshot -----------------------------\n");

            kmus_flag_id = 0;
            
//            printf("is Registered - pcpu : %d\n", sched_vcpu_register(0,0));
            
        } else if(kmus_flag_id == 0){
            int i = 0;
            int counter = 0;
            for(i = 0; i < SZ_128M; i+=4){
                if(readl(0x48000000 + i) !=readl( 0x68000000 + i))
                    counter++;
                
            }
            printf("i : 0x%08x, off_4_counter : 0x%08x\n", i/4, counter);
            /*
            counter = 0;
            for(i = 0; i < SZ_128M; i+=8){
                if((0x48000000 + i) == ( 0x58000000 + i))
                    counter++;
                
            }
            printf("i : 0x%08x, off_8_counter : 0x%08x\n", i, counter);
            counter = 0;
            for(i = 0; i < SZ_128M; i+=16){
                if((0x48000000 + i) == ( 0x58000000 + i))
                    counter++;
                
            }
            printf("i : 0x%08x, off_16_counter : 0x%08x\n", i, counter);*/
//            kmus_snapshot(1, 0, regs);
//            kmus_flag_id = 0;
//****************************************************************/
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
