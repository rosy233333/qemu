#ifndef HW_RISCV_LITE_EXECUTOR_H
#define HW_RISCV_LITE_EXECUTOR_H

#include "hw/sysbus.h"
#include "qemu/queue.h"

#define TYPE_RISCV_LITE_EXECUTOR "riscv.lite_executor"

#define RISCV_LITE_EXECUTOR(obj) OBJECT_CHECK(RISCVLiteExecutor, (obj), TYPE_RISCV_LITE_EXECUTOR)

#define MAX_TASK_QUEUE              0x8
#define MAX_TASK_PER_QUEUE          0x100
#define MAX_TASK_QUEUE_GROUP        0x4

#define RISCV_LITE_EXECUTOR_MMIO_SIZE   0x1000000

// MMIO Structure
#define PROCESS_MMIO_OFFSET             0x0
#define PROCESS_MMIO_SIZE               0x1000
#define PROCESS_MMIO_COUNT              0xffd // 4093
#define EXT_INTR_HANDLER_MMIO_OFFSET    ((PROCESS_MMIO_OFFSET) + (PROCESS_MMIO_SIZE) * (PROCESS_MMIO_COUNT))
#define EXT_INTR_HANDLER_MMIO_SIZE      0x2128
#define GLOBAL_RESERVED_MMIO_OFFSET     ((EXT_INTR_HANDLER_MMIO_OFFSET) + (EXT_INTR_HANDLER_MMIO_SIZE))

// Process MMIO Structure
#define PRIO_SCHEDULER_MMIO_OFFSET      0x0
#define PRIO_SCHEDULER_MMIO_SIZE        0x800
#define IPC_HANDLER_MMIO_OFFSET         ((PRIO_SCHEDULER_MMIO_OFFSET) + (PRIO_SCHEDULER_MMIO_SIZE))
#define IPC_HANDLER_MMIO_SIZE           0x100
#define PROCESS_RESERVED_MMIO_OFFSET    ((IPC_HANDLER_MMIO_OFFSET) + (IPC_HANDLER_MMIO_SIZE))

// Priority Scheduler MMIO Structure
#define PS_CONTROL_MMIO_OFFSET          0x0
#define PS_CONTROL_MMIO_SIZE            0x20
#define PS_MEMBUF_MMIO_OFFSET           ((PS_CONTROL_MMIO_OFFSET) + (PS_CONTROL_MMIO_SIZE))
#define PS_MEMBUF_MMIO_SIZE             0x8
#define PS_DEQUEUE_MMIO_OFFSET          ((PS_MEMBUF_MMIO_OFFSET) + (PS_MEMBUF_MMIO_SIZE))
#define PS_DEQUEUE_MMIO_SIZE            0x8
#define PS_ENQUEUE_MMIO_OFFSET          ((PS_DEQUEUE_MMIO_OFFSET) + (PS_DEQUEUE_MMIO_SIZE))
#define PS_ENQUEUE_MMIO_SIZE            0x8
#define PS_ENQUEUE_MMIO_COUNT           0xfa // 250
#define PS_END_MMIO_OFFSET              ((PS_ENQUEUE_MMIO_OFFSET) + (PS_ENQUEUE_MMIO_SIZE) * (PS_ENQUEUE_MMIO_COUNT))

// IPC Handler MMIO Structure
#define IH_CONTROL_MMIO_OFFSET          0x0
#define IH_CONTROL_MMIO_SIZE            0x8
#define IH_MEMBUF_MMIO_OFFSET           ((IH_CONTROL_MMIO_OFFSET) + (IH_CONTROL_MMIO_SIZE))
#define IH_MEMBUF_MMIO_SIZE             0x8
#define IH_MESSAGE_POINTER_MMIO_OFFSET  ((IH_MEMBUF_MMIO_OFFSET) + (IH_MEMBUF_MMIO_SIZE))
#define IH_MESSAGE_POINTER_MMIO_SIZE    0x8
#define IH_BQ_MMIO_OFFSET               ((IH_MESSAGE_POINTER_MMIO_OFFSET) + (IH_MESSAGE_POINTER_MMIO_SIZE))
#define IH_BQ_MMIO_SIZE                 0x8
#define IH_BQ_MMIO_COUNT                0x10 // 16
#define IH_RESERVED_MMIO_OFFSET         ((IH_BQ_MMIO_OFFSET) + (IH_BQ_MMIO_SIZE) * (IH_BQ_MMIO_COUNT))

// External Interrupt Handler MMIO Structure
#define EIH_CONTROL_MMIO_OFFSET         0x0
#define EIH_CONTROL_MMIO_SIZE           0x80
#define EIH_ENQUEUE_MMIO_OFFSET         ((EIH_CONTROL_MMIO_OFFSET) + (EIH_CONTROL_MMIO_SIZE))
#define EIH_ENQUEUE_MMIO_SIZE           0x8
#define EIH_ENQUEUE_MMIO_COUNT          0x400 // 1024
#define EIH_END_MMIO_OFFSET             ((EIH_ENQUEUE_MMIO_OFFSET) + (EIH_ENQUEUE_MMIO_SIZE) * (EIH_ENQUEUE_MMIO_COUNT))

#define WRITE_TEST_OFFSET   0x00000000
#define READ_TEST_OFFSET    0x00000008

typedef QSIMPLEQ_HEAD(, TaskQueueEntry) TaskQueueHead;

struct TaskQueueEntry {
    uint64_t data;
    QSIMPLEQ_ENTRY(TaskQueueEntry) next;
};

typedef struct {
    TaskQueueHead head;
} TaskQueue;

typedef struct RISCVLiteExecutor
{
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion mmio;
    TaskQueue *task_queues;

    /* config */
    uint32_t num_sources; //中断处理相关，中断源的数目？

} RISCVLiteExecutor;

DeviceState *riscv_lite_executor_create(hwaddr addr, uint32_t num_sources);

#endif