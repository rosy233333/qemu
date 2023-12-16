#ifndef HW_RISCV_LITE_EXECUTOR_H
#define HW_RISCV_LITE_EXECUTOR_H

#include "hw/sysbus.h"

#define TYPE_RISCV_LITE_EXECUTOR "riscv.lite_executor"

#define RISCV_LITE_EXECUTOR(obj) OBJECT_CHECK(RISCVLiteExecutor, (obj), TYPE_RISCV_LITE_EXECUTOR)

#define RISCV_LITE_EXECUTOR_SIZE    0x1000

#define ASYNC_TASK_MAX_NUM          0x10000

typedef struct {
    uint64_t ref;
} AsyncTaskRef;

typedef struct RISCVLiteExecutor
{
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion mmio;
    AsyncTaskRef *tasks;

} RISCVLiteExecutor;

DeviceState *riscv_lite_executor_create(hwaddr addr);

#endif