#ifndef __VM_H__
#define __VM_H__

#include <lib/list.h>
#include <types.h>
#include <core/vm/vcpu.h>
#include <core/vm/vmem.h>
#include <vdev.h>

#define SIZE_OF_NAME        32
#define NO_VM_FOUND         NULL
#define VM_NOT_EXISTED      (-1)
#define VM_CREATE_FAILED    (-2)

typedef enum vmcb_type {
    VM_NORMAL,
    VM_KMUS,
} vmcb_type_t;

typedef enum vmcb_state {
    UNDEFINED,
    DEFINED,
    HALTED,
    RUNNING,
    SUSPENDED
} vmcb_state_t;

struct vmcb {
    vmid_t vmid;
    char name[SIZE_OF_NAME];

    uint8_t num_vcpus;

    struct vcpu **vcpu;
    struct vmem vmem;

    struct vdev_instance vdevs;

    vmcb_type_t type;
    vmcb_state_t state;
    struct list_head head;
};

void vm_setup();
vmid_t vm_create(uint8_t num_vcpu, vmcb_type_t vmcb_type);
vmcb_state_t vm_init(vmid_t vmid);
vmcb_state_t vm_start(vmid_t vmid);
vmcb_state_t vm_suspend(vmid_t vmid, struct core_regs *regs);
vmcb_state_t vm_delete(vmid_t vmid);

// TODO(casionwoo) : resume, shutdown
void vm_save(vmid_t vmid);
void vm_restore(vmid_t vmid);

void vm_copy(vmid_t from, vmid_t to, struct core_regs *regs);

struct vmcb *vm_find(vmid_t vmid);
void print_all_vm();

struct list_head *get_all_vms();

#endif /* __VM_H__ */

