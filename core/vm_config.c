#include <vm_config.h>
#include <config.h>
#include <size.h>

struct vm_config vm_conf[] = {
    { 1, SZ_128M, CFG_MEMMAP_GUEST0_ATAGS_OFFSET, VM_NORMAL },
    { 1, SZ_128M, CFG_MEMMAP_GUEST0_ATAGS_OFFSET + SZ_256M, VM_KMUS }
};
