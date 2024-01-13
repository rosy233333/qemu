#ifndef HW_RISCV_LITE_EXECUTOR_H
#define HW_RISCV_LITE_EXECUTOR_H

#include "hw/sysbus.h"
#include "qemu/queue.h"

#define TYPE_RISCV_LITE_EXECUTOR "riscv.lite_executor"

#define RISCV_LITE_EXECUTOR(obj) OBJECT_CHECK(RISCVLiteExecutor, (obj), TYPE_RISCV_LITE_EXECUTOR)

#define RISCV_LITE_EXECUTOR_SIZE    0x1000000

#define MAX_TASK_QUEUE              0x8
#define MAX_TASK_PER_QUEUE          0x100

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

} RISCVLiteExecutor;

DeviceState *riscv_lite_executor_create(hwaddr addr);

#endif