#define DEBUG
#include <stdio.h>
#include <core/vm/vcpu.h>
#include "../../arch/arm/traps.h"
#include <vdev.h>
#include <arch/armv7.h>

/* Common in EC, HSR[31:30] zero */
#define EC_ZERO_CV_BIT 0x01000000
#define EC_ZERO_COND_BIT 0x00E00000
#define EC_ZERO_CV_SHIFT 24
#define EC_ZERO_COND_SHIFT 20
#define IS_VAILD_CV    1

#define MCR_MRC_OPC2_BIT        0x000E0000
#define MCR_MRC_OPC1_BIT        0x0001C000
#define MCR_MRC_CRN_BIT         0x00003C00
#define MCR_MRC_RT_BIT          0x000001E0
#define MCR_MRC_CRM_BIT         0x0000001E
#define MCR_MRC_DIRECTION_BIT   0x00000001

#define MCR_MRC_OPC2_SHIFT      17
#define MCR_MRC_OPC1_SHIFT      14
#define MCR_MRC_CRN_SHIFT       10
#define MCR_MRC_RT_SHIFT        5
#define MCR_MRC_CRM_SHIFT       1
/* Do not use it to shift. */
#define MCR_MRC_DIRECTION_SHIFT 0

#define WFI_WFE_DIRECTION_BIT   0x00000001
#define WFI_WFE_DIRECTION_SHIFT 0 /* Do not use it to shift. */

void emulate_mcr_mrc_cp15(uint32_t iss, uint32_t il)
{
    /*
     * If value of EC bit is equal to 0x3, trapped
     * instruction should be handled here.
     */
    /* unused variable */
    /* uint32_t cv = (iss & EC_ZERO_CV_BIT) >> EC_ZERO_CV_SHIFT; */

    /* uint32_t cond; */
    uint32_t Opc2, Opc1, CRn, Rt, CRm, dir;

    /* warning: variable ‘cond’ set but not used */
    /*
    if (cv == IS_VAILD_CV)
        cond = (iss & EC_ZERO_COND_BIT) >> EC_ZERO_COND_SHIFT;
    else
        cond = 0x0;
    */
    dir = (iss & MCR_MRC_DIRECTION_BIT);
    Opc2 = (iss & MCR_MRC_OPC2_BIT) >> MCR_MRC_OPC2_SHIFT;
    Opc1 = (iss & MCR_MRC_OPC1_BIT) >> MCR_MRC_OPC1_SHIFT;
    CRn = (iss & MCR_MRC_CRN_BIT) >> MCR_MRC_CRN_SHIFT;
    Rt = (iss & MCR_MRC_RT_BIT) >> MCR_MRC_RT_SHIFT;
    CRm = (iss & MCR_MRC_CRM_BIT) >> MCR_MRC_CRM_SHIFT;
    /* Register ordering MRC CP#, OPC1, REG#, CRn, CRm, OPC2 */
    if (dir == 0) {
        printf("MCR ");
        printf("p15, %d, Rt%d, c%d, c%d, %d\n", Opc1, Rt, CRn, CRm, Opc2);
    } else if (dir == 1) {
        printf("MRC ");
        printf("p15, %d, Rt%d, c%d, c%d, %d\n", Opc1, Rt, CRn, CRm, Opc2);
    } else {
        printf("Error: Unknown instructions\n");
    }
}

void emulate_mcr_mrc_cp14(uint32_t iss, uint32_t il)
{
    /*
     * If value of EC bit is equal to 0x3, trapped
     * instruction should be handled here.
     */
    /* unused variable */
    /* uint32_t cv = (iss & EC_ZERO_CV_BIT) >> EC_ZERO_CV_SHIFT; */
    /* unused variable */
    /* uint32_t cond; */
    uint32_t Opc2, Opc1, CRn, Rt, CRm, dir;
    /* warning: variable ‘cond’ set but not used */
    /* if (cv == IS_VAILD_CV)
        cond = (iss & EC_ZERO_COND_BIT) >> EC_ZERO_COND_SHIFT;
    else
        cond = 0x0;
    */
    dir = (iss & MCR_MRC_DIRECTION_BIT);
    Opc2 = (iss & MCR_MRC_OPC2_BIT) >> MCR_MRC_OPC2_SHIFT;
    Opc1 = (iss & MCR_MRC_OPC1_BIT) >> MCR_MRC_OPC1_SHIFT;
    CRn = (iss & MCR_MRC_CRN_BIT) >> MCR_MRC_CRN_SHIFT;
    Rt = (iss & MCR_MRC_RT_BIT) >> MCR_MRC_RT_SHIFT;
    CRm = (iss & MCR_MRC_CRM_BIT) >> MCR_MRC_CRM_SHIFT;
    /* MRC CP#, OPC1, REG#, CRn, CRm, OPC2 */
    if (dir == 0) {
        printf("MCR ");
        printf("p14, %d, Rt%d, c%d, c%d, %d\n", Opc1, Rt, CRn, CRm, Opc2);
    } else if (dir == 1) {
        printf("MRC ");
        printf("p14, %d, Rt%d, c%d, c%d, %d\n", Opc1, Rt, CRn, CRm, Opc2);
    } else {
        printf("Error: Unknown instructions\n");
    }
}

void emulate_mcr_mrc_cp10(uint32_t iss, uint32_t il)
{
    /*
     * If value of EC bit is equal to 0x3,
     * trapped instruction should be handled here.
     */
    /* unused variable */
    /* uint32_t cv = (iss & EC_ZERO_CV_BIT) >> EC_ZERO_CV_SHIFT; */
    /* unused variable */
    /* uint32_t cond; */
    uint32_t Opc2, Opc1, CRn, Rt, CRm, dir;
    /* warning: variable ‘cond’ set but not used */
    /*
    if (cv == IS_VAILD_CV)
        cond = (iss & EC_ZERO_COND_BIT) >> EC_ZERO_COND_SHIFT;
    else
        cond = 0x0;
    */
    dir = (iss & MCR_MRC_DIRECTION_BIT);
    Opc2 = (iss & MCR_MRC_OPC2_BIT) >> MCR_MRC_OPC2_SHIFT;
    Opc1 = (iss & MCR_MRC_OPC1_BIT) >> MCR_MRC_OPC1_SHIFT;
    CRn = (iss & MCR_MRC_CRN_BIT) >> MCR_MRC_CRN_SHIFT;
    Rt = (iss & MCR_MRC_RT_BIT) >> MCR_MRC_RT_SHIFT;
    CRm = (iss & MCR_MRC_CRM_BIT) >> MCR_MRC_CRM_SHIFT;
    /* MRC CP#, OPC1, REG#, CRn, CRm, OPC2 */
    if (dir == 0) {
        printf("MCR ");
        printf("p10, %d, Rt%d, c%d, c%d, %d\n", Opc1, Rt, CRn, CRm, Opc2);
    } else if (dir == 1) {
        printf("VMRS ");
        printf("p10, %d, Rt%d, c%d, c%d, %d\n", Opc1, Rt, CRn, CRm, Opc2);
    } else {
        printf("Error: Unknown instructions\n");
    }
}

/*
 * When HCR.TWI is set to 1, and the processor is
 * in a Non-secure mode other than Hyp mode,
 * execution of a WFI instruction generates a Hyp Trap exception.
 *
 * Syntax, WFI{<c>}{<q>}
 *
 * if ConditionPassed() then
 *         EncodingSpecificOperations();
 *         if HaveVirtExt() && !IsSecure() && \
 *              !CurrentModeIsHyp() && HCR.TWI == '1' then
 *             HSRString = Zeros(25);
 *             HSRString<0> = '0';
 *             WriteHSR('000001', HSRString);
 *             TakeHypTrapException();
 *         else
 *             WaitForInterrupt();
 *
 *     In kernel implementation,
 *         #define wfi()    __asm__ __volatile__ ("wfi" : : : "memory")
 */

/* When HCR.TWE is set to 1, and the processor is
 * in a Non-secure mode other than Hyp mode,
 * execution of a WFE instruction generates a Hyp Trap exception.
 */

void emulate_wfi_wfe(uint32_t iss, uint32_t il)
{
    /* unused variable */
    /* uint32_t cv = (iss & EC_ZERO_CV_BIT) >> EC_ZERO_CV_SHIFT; */
    /* unused variable */
    /* uint32_t cond; */
    uint32_t direction;
    /* warning: variable ‘cond’ set but not used */
    /*
    if (cv == IS_VAILD_CV)
        cond = (iss & EC_ZERO_COND_BIT) >> EC_ZERO_COND_SHIFT;
    else
        cond = 0x0;
    */
    if (il == 0) {
        printf("16-bit Thumb instruction\n");
    } else {
        printf("32-bit ARM instruction\n");
    }

    direction = (iss & WFI_WFE_DIRECTION_BIT);
    if (direction == 0) {
        printf("WFI trapped.\n");
    } else {
        printf("WFE trapped.\n");
    }

}

static int32_t vdev_cp_read(struct arch_vdev_trigger_info *info)
{
    uint32_t ec = info->ec;
    uint32_t hsr = read_hsr();

    switch (ec) {
    case TRAP_EC_ZERO_UNKNOWN:
        printf("Unknown reason: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_WFI_WFE:
        printf("Trapped WFI or WFE instruction: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_MCR_MRC_CP15:
        printf("Trapped MCR or MRC access to CP15: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_MCRR_MRRC_CP15:
        printf("Trapped MCRR or MRRC access to CP15: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_MCR_MRC_CP14:
        printf("Trapped MCR or MRC access to CP14: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_LDC_STC_CP14:
        printf("Trapped LDC or STC access to CP14: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_HCRTR_CP0_CP13:
        printf("HCPTR-trapped access to CP0-CP13: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_MRC_VMRS_CP10:
        printf(
            "Trapped MRC or VMRS access to CP10, for ID group traps: 0x%08x\n",
            hsr);
        break;
    case TRAP_EC_ZERO_BXJ:
        printf("Trapped BXJ instruction: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_MRRC_CP14:
        printf("Trapped MRRC access to CP14: 0x%08x\n", hsr);
        break;
    case TRAP_EC_NON_ZERO_SVC:
        printf("Supervisor Call exception routed to Hyp mode: 0x%08x\n", hsr);
        break;
    }

    return 0;
}

static int32_t vdev_cp_write(struct arch_vdev_trigger_info *info)
{
    uint32_t ec = info->ec;
    uint32_t hsr = read_hsr();

    switch (ec) {
    case TRAP_EC_ZERO_UNKNOWN:
        printf("Unknown reason: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_WFI_WFE:
        printf("Trapped WFI or WFE instruction: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_MCR_MRC_CP15:
        printf("Trapped MCR or MRC access to CP15: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_MCRR_MRRC_CP15:
        printf("Trapped MCRR or MRRC access to CP15: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_MCR_MRC_CP14:
        printf("Trapped MCR or MRC access to CP14: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_LDC_STC_CP14:
        printf("Trapped LDC or STC access to CP14: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_HCRTR_CP0_CP13:
        printf("HCPTR-trapped access to CP0-CP13: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_MRC_VMRS_CP10:
        printf(
            "Trapped MRC or VMRS access to CP10, for ID group traps: 0x%08x\n",
            hsr);
        break;
    case TRAP_EC_ZERO_BXJ:
        printf("Trapped BXJ instruction: 0x%08x\n", hsr);
        break;
    case TRAP_EC_ZERO_MRRC_CP14:
        printf("Trapped MRRC access to CP14: 0x%08x\n", hsr);
        break;
    case TRAP_EC_NON_ZERO_SVC:
        printf("Supervisor Call exception routed to Hyp mode: 0x%08x\n", hsr);
        break;
    }

    return 0;
}

static hvmm_status_t vdev_cp_post(struct core_regs *regs)
{
    uint8_t isize = 4;

    if (regs->cpsr & 0x20) { /* Thumb */
        isize = 2;
    }

    regs->pc += isize;

    return 0;
}

static int32_t vdev_cp_check(struct arch_vdev_trigger_info *info)
{
    uint32_t ec = info->ec;

    if (ec == TRAP_EC_ZERO_MCR_MRC_CP15 ||
            ec == TRAP_EC_ZERO_MCRR_MRRC_CP15 ||
            ec == TRAP_EC_ZERO_MCR_MRC_CP14 ||
            ec == TRAP_EC_ZERO_HCRTR_CP0_CP13 ||
            ec == TRAP_EC_ZERO_MRC_VMRS_CP10 ||
            ec == TRAP_EC_ZERO_MRRC_CP14 ||
            ec == TRAP_EC_ZERO_MCR_MRC_CP14 ||
            ec == TRAP_EC_ZERO_LDC_STC_CP14) {
        return 0;
    }

    return VDEV_NOT_FOUND;
}

static hvmm_status_t vdev_cp_reset_values(void)
{
    printf("vdev init:'%s'\n", __func__);
    return HVMM_STATUS_SUCCESS;
}

struct vdev_ops _vdev_cp_ops = {
    .init = vdev_cp_reset_values,
    .check = vdev_cp_check,
    .read = vdev_cp_read,
    .write = vdev_cp_write,
    .post = vdev_cp_post,
};

struct vdev_module _vdev_cp_module = {
    .name = "K-Hypervisor vDevice CP Module",
    .author = "Kookmin Univ.",
    .ops = &_vdev_cp_ops,
};

hvmm_status_t vdev_cp_init()
{
    hvmm_status_t result = HVMM_STATUS_BUSY;

    result = vdev_register(VDEV_LEVEL_HIGH, &_vdev_cp_module);
    if (result == HVMM_STATUS_SUCCESS) {
        printf("vdev registered:'%s'\n", _vdev_cp_module.name);
    } else {
        printf("%s: Unable to register vdev:'%s' code=%x\n",
               __func__, _vdev_cp_module.name, result);
    }

    return result;
}
vdev_module_high_init(vdev_cp_init);
