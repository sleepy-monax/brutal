#include <brutal/log.h>
#include "kernel/syscall.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

BrResult br_noop(void)
{
    return BR_SUCCESS;
}

BrResult br_log(char const *message, size_t size)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_group(BrGroupe *groupe, BrCap caps, BrGroupeFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_space(BrSpace *space, BrSpaceFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_vmo(BrVmo *vmo, uintptr_t addr, size_t size, BrMemFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_map(BrSpace space, BrVmo vmo, uintptr_t vaddr, BrMemFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_alloc(BrSpace space, BrVmo vmo, uintptr_t *vaddr, BrMemFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_unmap(BrSpace space, uintptr_t vaddr, size_t size)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_task(BrTask *task, BrSpace space, BrGroupe groupe, BrTaskFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_start(BrTask task, uintptr_t entry)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_exit(BrTask task, uintptr_t exit_value)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_block(BrTask task, BrBlocker *blocker, BrTimeout timeout, BrBlockFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_signal(BrTask task, BrSignal signal)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_bind(BrGroupe groupe, BrTask task, char const *name, size_t size)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_locate(BrTask *task, char const *name, size_t size)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_send(BrTask task, BrMessage *message, BrIpcFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_recv(BrTask task, BrMessage *message, BrTimeout timeout, BrIpcFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_irq(BrTask task, BrIrq irq, BrIrqFlags flags)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_drop(BrTask task, BrCap cap)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_pledge(BrTask task, BrCap cap)
{
    return BR_NOT_IMPLEMENTED;
}

BrResult br_grant(BrTask task, BrCap cap)
{
    return BR_NOT_IMPLEMENTED;
}

#pragma GCC diagnostic pop

typedef BrResult BrSyscallFn();

BrSyscallFn *syscalls[BR_SYSCALL_COUNT] = {
    [BR_SC_NOOP] = br_noop,
    [BR_SC_LOG] = br_log,
    [BR_SC_GROUP] = br_group,
    [BR_SC_SPACE] = br_space,
    [BR_SC_VMO] = br_vmo,
    [BR_SC_MAP] = br_map,
    [BR_SC_ALLOC] = br_alloc,
    [BR_SC_UNMAP] = br_unmap,
    [BR_SC_TASK] = br_task,
    [BR_SC_START] = br_start,
    [BR_SC_EXIT] = br_exit,
    [BR_SC_BLOCK] = br_block,
    [BR_SC_SIGNAL] = br_signal,
    [BR_SC_BIND] = br_bind,
    [BR_SC_LOCATE] = br_locate,
    [BR_SC_SEND] = br_send,
    [BR_SC_RECV] = br_recv,
    [BR_SC_IRQ] = br_irq,
    [BR_SC_DROP] = br_drop,
    [BR_SC_PLEDGE] = br_pledge,
};

Str syscall_to_string(BrSyscall syscall)
{
    static const char *SYSCALL_NAMES[] = {
#define ITER(SYSCALL) #SYSCALL,
        FOREACH_SYSCALLS(ITER)
#undef ITER
    };

    if (syscall >= BR_SYSCALL_COUNT)
    {
        return str_cast("INVALID");
    }

    return str_cast(SYSCALL_NAMES[syscall]);
}

BrResult syscall_dispatch(BrSyscall syscall, BrArg arg1, BrArg arg2, BrArg arg3, BrArg arg4, BrArg arg5)
{
    log("Syscall: {}({p}, {p}, {p}, {p}, {p})...", syscall_to_string(syscall), arg1, arg2, arg3, arg4, arg5);

    if (syscall >= BR_SYSCALL_COUNT)
    {
        return BR_BAD_SYSCALL;
    }

    return syscalls[syscall](arg1, arg2, arg3, arg4, arg5);
}
