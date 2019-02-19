#include <core/vm/vm.h>
#include <debug.h>
#include <stdio.h>
#include <string.h>
#include <atags.h>
#include <size.h>
#include <core/scheduler.h>

void print_vm(struct vmcb *vm);

static struct list_head vm_list;
static vmid_t vm_count;

void vm_setup()
{
    vm_count = 0;
    LIST_INITHEAD(&vm_list);

    vcpu_setup();
#ifdef CONFIG_ATAGS
    atags_setup();
#endif
}

vmid_t vm_create(uint8_t num_vcpus, vmcb_type_t vmcb_type)
{
    int i;
    struct vmcb *vm = NULL;

    vm = malloc(sizeof(struct vmcb));
    if (vm == NULL) {
        return VM_CREATE_FAILED;
    }
    memset(vm, 0, sizeof(struct vmcb));

    vm->vmid = vm_count++;
    vm->state = DEFINED;
    vm->num_vcpus = num_vcpus;
    vm->type = vmcb_type;

    vm->vcpu = malloc(sizeof(struct vcpu *) * vm->num_vcpus);
    if (vm->vcpu == NULL) {
        return VM_CREATE_FAILED;
    }

    for (i = 0; i < vm->num_vcpus; i++) {
        if ((vm->vcpu[i] = vcpu_create(vm->type)) == VCPU_CREATE_FAILED) {
            free(vm);
            return VM_CREATE_FAILED;
        }
        vm->vcpu[i]->vmid = vm->vmid;
        vm->vcpu[i]->id = i;
    }

    vmem_create(&vm->vmem, vm->vmid);
    vdev_create(&vm->vdevs, vm->vmid);

    LIST_ADDTAIL(&vm->head, &vm_list);

    return vm->vmid;
}

#include <drivers/gic-v2.h>
#include "../../include/arch/gicv2_bit.h"

vmcb_state_t vm_init(vmid_t vmid)
{
    struct vmcb *vm = vm_find(vmid);
    if (vm == NO_VM_FOUND) {
        return VM_NOT_EXISTED;
    }

    int i;
    for (i = 0; i < vm->num_vcpus; i++) {
        if (vcpu_init(vm->vcpu[i]) != VCPU_REGISTERED) {
            return vm->state;
        }
    }

    vm->state = HALTED;

    vmem_init(&vm->vmem, vmid);

    return vm->state;
}

vmcb_state_t vm_start(vmid_t vmid)
{
    struct vmcb *vm = vm_find(vmid);
    
    if (vm == NO_VM_FOUND) {
        return VM_NOT_EXISTED;
    }

    int i;
    for (i = 0; i < vm->num_vcpus; i++) {
        if (vcpu_start(vm->vcpu[i]) != VCPU_ACTIVATED) {
            return vm->state;
        }
    }

    vm->state = RUNNING;

    return vm->state;
}

vmcb_state_t vm_suspend(vmid_t vmid, struct core_regs *regs)
{
    struct vmcb *vm = vm_find(vmid);
    if (vm == NO_VM_FOUND) {
        return VM_NOT_EXISTED;
    }

    vm_save(vmid);

    int i;
    for (i = 0; i < vm->num_vcpus; i++) {
        if (vcpu_suspend(vm->vcpu[i], regs) != VCPU_REGISTERED) {
            return vm->state;
        }
    }

    vm->state = HALTED;

    return vm->state;
}

vmcb_state_t vm_delete(vmid_t vmid)
{
    int i;
    struct vmcb *vm = vm_find(vmid);

    if (vm == NO_VM_FOUND) {
        return VM_NOT_EXISTED;
    }

    vdev_delete(&vm->vdevs);
    LIST_DEL(&vm->head);

    for (i = 0; i < vm->num_vcpus; i++) {
        if (vcpu_delete(vm->vcpu[i]) != VCPU_UNDEFINED) {
            return vm->state;
        }
    }


    free(vm);

    return UNDEFINED;
}

void vm_save(vmid_t save_vmid)
{
    struct vmcb *vm = vm_find(save_vmid);
    if (vm == NO_VM_FOUND) {
        debug_print("[%s]: NO VM FOUND %d\n", __func__, save_vmid);
    }

    vmem_save();
}

void vm_restore(vmid_t restore_vmid)
{
    struct vmcb *vm = vm_find(restore_vmid);
    if (vm == NO_VM_FOUND) {
        debug_print("[%s]: NO VM FOUND %d\n", __func__, restore_vmid);
    }

    vmem_restore(&vm->vmem);
}

void vm_copy(vmid_t from, vmid_t to, struct core_regs *regs)
{
    struct vmcb *vm_from = vm_find(from);
    struct vmcb *vm_to = vm_find(to);
    if (vm_from == NO_VM_FOUND || vm_to == NO_VM_FOUND) {
        printf("%s[%d] vm_find failed\n");
        return ;
    }


    int i;
    for (i = 0; i < vm_from->num_vcpus; i++)
        vcpu_copy(vm_from->vcpu[i], vm_to->vcpu[i], regs);

    vmem_copy(&vm_from->vmem, &vm_to->vmem);

    vdev_copy(&vm_from->vdevs, &vm_to->vdevs);

    vm_to->state = vm_from->state;
    vm_to->type = vm_from->type;
}

struct vmcb *vm_find(vmid_t vmid)
{
    struct vmcb *vm = NULL;
    list_for_each_entry(struct vmcb, vm, &vm_list, head) {
        if (vm->vmid == vmid) {
            return vm;
        }
    }
    return NO_VM_FOUND;
}

void print_all_vm()
{
    struct vmcb *vm = NULL;
    list_for_each_entry(struct vmcb, vm, &vm_list, head) {
        print_vm(vm);
    }
}

void print_vm(struct vmcb *vm)
{
    debug_print("ADDR  : 0x%p\n", vm);
    debug_print("VMID  : %d\n", vm->vmid);
    debug_print("STATE : %d\n", vm->state);
}

struct list_head *get_all_vms()
{
    return &vm_list;
}
