#pragma once

#include <syscalls/types.h>

static inline BrResult br_syscall(BrSyscall syscall, BrArg arg1, BrArg arg2, BrArg arg3, BrArg arg4, BrArg arg5)
{
    BrResult result;

    register uint64_t r8 asm("r8") = arg4;
    register uint64_t r9 asm("r9") = arg5;

    asm volatile("syscall"
                 : "=a"(result)
                 : "a"(syscall), "b"(arg1), "d"(arg2), "S"(arg3), "r"(r8), "r"(r9)
                 : "memory", "r11", "rcx");

    return result;
}

/*
    br_noop

    Description
    -----------
    Test syscall that does nothing.

    Parameters
    ----------
    None

    Result
    ------
    - BR_SUCCESS: Successfully did nothing.
*/
static inline BrResult br_noop(void)
{
    return br_syscall(BR_SC_NOOP, 0, 0, 0, 0, 0);
}

/*
    br_log

    Description
    -----------
    Display a message to the kernel logs.

    Parameters
    ----------
    - message: The message to be displayed (Str).

    Result
    ------
    - BR_SUCCESS: The message was successfully written to the logs.
    - BR_BAD_ADDRESS: The address of the message was not located in user memory.
*/
static inline BrResult br_log(char const *message, size_t size)
{
    return br_syscall(BR_SC_LOG, (BrArg)message, size, 0, 0, 0);
}

/*
    br_group

    Description
    -----------
    Create a new group of task.

    Parameters
    ----------
    - groupe: The handle of the groupe if created successfully, or BR_GROUPE_ERROR (out BrGroupe).
    - caps: The default caps of the tasks in the groupe (BrCap).
    - flags: Flags to tweaks to beavio of the groupe (BrGroupeFlags).

    Flags
    -----
    - BR_GROUP_PERMEABLE: Tasks inside this groupe can access services of the parent group.

    Result
    ------
    - BR_SUCCESS: The groupe for successfully created.
    - BR_BAD_ADDRESS: The address of the groupe was not located in user memory.
    - BR_BAD_CAPABILITY: The task don't have the required capabilities.
    - BR_OUT_OF_MEMORY: There was not enoughs memory to complete the operation.
*/
static inline BrResult br_group(BrGroupe *groupe, BrCap caps, BrGroupeFlags flags)
{
    return br_syscall(BR_SC_GROUP, (BrArg)groupe, caps, flags, 0, 0);
}

/*
    br_space

    Description
    -----------
    Create a new address space.

    Parameters
    ----------
    - space: Handle of the space if created successfully, or BR_SPACE_ERROR (out BrSpace).
    - flags: Flags to tweaks to behavior of the space (BrSpaceFlags).

    Result
    ------
    - BR_SUCCESS: Space successfully created.
    - BR_BAD_ADDRESS: The address of the space was not located in user memory.
    - BR_BAD_CAPABILITY: The task don't have the required capabilities.
    - BR_OUT_OF_MEMORY: There was not enoughs memory to complete the operation.
*/
static inline BrResult br_space(BrSpace *space, BrSpaceFlags flags)
{
    return br_syscall(BR_SC_SPACE, (BrArg)space, flags, 0, 0, 0);
}

/*
    br_mobj

    Description
    -----------
    Create a new virtual memory object that can be passed across address spaces.

    Parameters
    ----------
    - mobj: Handle to the MOBJ if created successfully, or BR_MOBJ_ERROR (out BrMObj).
    - addr: Physical address pointed by the MOBJ, used if flags has BR_MOBJ_PMM (uintptr_t).
    - size: Size of the MOBJ (size_t).
    - flags: Flags to tweaks the behavior of the MOBJ (BrMObjFlags).

    FLags
    -----
    - BR_MOBJ_PMM: This MOBJ point to a physical address (require BR_CAP_PMM).

    Result
    ------
    - BR_SUCCESS: The mobj was created successfully.
    - BR_BAD_ADDRESS: The address of the mobj was not located in user memory.
    - BR_BAD_CAPABILITY: The task don't have the required capabilities.
    - BR_OUT_OF_MEMORY: There was not enoughs memory to complete the operation.
*/
static inline BrResult br_mobj(BrMObj *mobj, uintptr_t addr, size_t size, BrMObjFlags flags)
{
    return br_syscall(BR_SC_MOBJ, (BrArg)mobj, addr, size, flags, 0);
}

/*
    br_map

    Description
    -----------
    Map a mobj into an address space.

    Parameters
    ----------
    space: Address where the mobj will be mapped (BrSpace).
    mobj: MOBJ that will be mapped into the address space (BrMObj).
    vaddr: Address where the MOBJ will be mapped (uintptr_t).
    flags: Access flags (BrMemFlags).

    Result
    ------
    - BR_SUCCESS: The mapping was created successfully.
    - BR_BAD_HANDLE: The handle to the space or the mobj is invalid.
    - BR_OUT_OF_MEMORY: There was not enoughs memory to complete the operation.
*/
static inline BrResult br_map(BrSpace space, BrMObj mobj, uintptr_t vaddr, BrMemFlags flags)
{
    return br_syscall(BR_SC_MAP, space, mobj, vaddr, flags, 0);
}

/*
    br_alloc

    Description
    -----------
    Find some space in the target address and map the mobj into .

    Parameters
    ----------
    space: Address space where the mobj will be mapped (BrSpace).
    mobj: MOBJ that will be mapped into the address space (BrMObj).
    vaddr: Address where the MOBJ was mapped(out uintptr_t).
    flags: Access flags (BrMemFlags).

    Result
    ------
    - BR_SUCCESS: The mapping was created successfully.
    - BR_BAD_HANDLE: The handle to the space or the mobj is invalid.
    - BR_OUT_OF_MEMORY: There was not enoughs memory to complete the operation.
*/
static inline BrResult br_alloc(BrSpace space, BrMObj mobj, uintptr_t *vaddr, BrMemFlags flags)
{
    return br_syscall(BR_SC_ALLOC, space, mobj, (BrArg)vaddr, flags, 0);
}

/*
    br_unmap

    Description
    -----------
    Umap a memory range from an address space.

    Parameters
    ----------
    space: Address space  where the memory will be unmapped (BrSpace).
    vaddr: Start of the range that will be unmapped (uintptr_t).
    size: Size of the range that will ne unmapped (out uintptr_t).

    Result
    ------
    - BR_SUCCESS: The memory was unmapped successfully.
    - BR_BAD_HANDLE: The handle to the space is invalid.
*/
static inline BrResult br_unmap(BrSpace space, uintptr_t vaddr, size_t size)
{
    return br_syscall(BR_SC_UNMAP, space, vaddr, size, 0, 0);
}

/*
    br_task

    Description
    -----------
    Create a new task.

    Parameters
    ----------
    - task: Handle to the task if created successfully, or BR_MOBJ_ERROR (out BrTask).
    - space: Address space of the task (BrSpace).
    - groupe: Parent groupe of the task.
    - flags: Flags to tweaks the behavior of the task (BrTaskFlags).

    Result
    ------
    - BR_SUCCESS: The task was created successfully.
    - BR_BAD_HANDLE: The handle to the space or the groupe is invalid.
    - BR_BAD_ADDRESS:  The address of the task was not located in user memory.
*/
static inline BrResult br_task(BrTask *task, BrSpace space, BrGroupe groupe, BrTaskFlags flags)
{
    return br_syscall(BR_SC_TASK, (BrArg)task, space, groupe, flags, 0);
}

/*
    br_start

    Description
    -----------
    Begin the execution of a task.

    Parameters
    ----------
    - task: Task to start (BrTask).
    - ip: The instruction pointer of the task.
    - sp: The stack pointer of the task.

    Result
    ------
    - BR_SUCCESS: The task was started successfully.
    - BR_BAD_HANDLE: The handle to the task is invalid.
*/
static inline BrResult br_start(BrTask task, uintptr_t ip, uintptr_t sp)
{
    return br_syscall(BR_SC_START, task, ip, sp, 0, 0);
}

static inline BrResult br_exit(BrTask task, uintptr_t exit_value)
{
    return br_syscall(BR_SC_EXIT, task, exit_value, 0, 0, 0);
}

static inline BrResult br_bind(BrGroupe groupe, BrTask task, char const *name, size_t size)
{
    return br_syscall(BR_SC_BIND, groupe, task, (BrArg)name, size, 0);
}

static inline BrResult br_locate(BrTask *task, char const *name, size_t size)
{
    return br_syscall(BR_SC_LOCATE, (BrArg)task, (BrArg)name, size, 0, 0);
}

static inline BrResult br_send(BrTask task, BrMessage *message, BrIpcFlags flags)
{
    return br_syscall(BR_SC_SEND, task, (BrArg)message, flags, 0, 0);
}

static inline BrResult br_recv(BrTask task, BrMessage *message, BrTimeout timeout, BrIpcFlags flags)
{
    return br_syscall(BR_SC_RECV, task, (BrArg)message, timeout, flags, 0);
}

static inline BrResult br_irq(BrTask task, BrIrq irq, BrIrqFlags flags)
{
    return br_syscall(BR_SC_IRQ, task, irq, flags, 0, 0);
}

static inline BrResult br_drop(BrTask task, BrCap cap)
{
    return br_syscall(BR_SC_DROP, task, cap, 0, 0, 0);
}

static inline BrResult br_close(BrHandle handle)
{
    return br_syscall(BR_SC_CLOSE, handle, 0, 0, 0, 0);
}