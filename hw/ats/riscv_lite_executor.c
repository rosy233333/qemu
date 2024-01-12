/*
 * RISC-V LITE EXECUTOR 
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/error-report.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/sysbus.h"
#include "target/riscv/cpu.h"
#include "hw/qdev-properties.h"
#include "qemu/timer.h"
#include "hw/ats/riscv_lite_executor.h"

static uint64_t riscv_lite_executor_read(void *opaque, hwaddr addr, unsigned size)
{
    info_report("READ LITE EXECUTOR: ADDR 0x%lx", addr);

    return 0;
}

static void riscv_lite_executor_write(void *opaque, hwaddr addr, uint64_t value,
                              unsigned size)
{
    info_report("WRITE LITE EXECUTOR: ADDR 0x%lx VALUE 0x%lx", addr, value);
    
}

static const MemoryRegionOps riscv_lite_executor_ops = {
    .read = riscv_lite_executor_read,
    .write = riscv_lite_executor_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_size = 8,
        .max_access_size = 8
    }
};

static void riscv_lite_executor_irq_request(void *opaque, int irq, int level)
{
    info_report("RISCV LITE EXECUTOR RECEIVE IRQ: %d", irq);

    SiFivePLICState *s = opaque;

    // 外部中断到来后的操作，待实现
}

static void riscv_lite_executor_realize(DeviceState *dev, Error **errp)
{
    
    RISCVLiteExecutor *lite_executor = RISCV_LITE_EXECUTOR(dev);

    info_report("RISCV LITE EXECUTOR REALIZE");

    memory_region_init_io(&lite_executor->mmio, OBJECT(dev), &riscv_lite_executor_ops, lite_executor,
                          TYPE_RISCV_LITE_EXECUTOR, RISCV_LITE_EXECUTOR_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &lite_executor->mmio);

    info_report("LOW 0x%x HIGH 0x%x", (uint32_t)lite_executor->mmio.addr, (uint32_t)lite_executor->mmio.size);

    // 如果有为lite executor增添了数组属性，在这里申请空间
    lite_executor->tasks = g_new0(AsyncTaskRef, ASYNC_TASK_MAX_NUM);

    //注册GPIO端口，参考sifive_plic.c:380..387
    {
        qdev_init_gpio_in(dev, riscv_lite_executor_irq_request, lite_executor->num_sources);

        //这几句应该是用于向CPU通知中断的输出端口？所以应该在lite_executor中不需要？
        // s->s_external_irqs = g_malloc(sizeof(qemu_irq) * s->num_harts);
        // qdev_init_gpio_out(dev, s->s_external_irqs, s->num_harts);

        // s->m_external_irqs = g_malloc(sizeof(qemu_irq) * s->num_harts);
        // qdev_init_gpio_out(dev, s->m_external_irqs, s->num_harts);
    }

    // lite executor应该不需要调用riscv_cpu_claim_interrupts函数，因为它不会直接向CPU发中断信号。
}

// 为调度器新增的属性，参考了sifive_plic.c:426..442
static Property riscv_lite_executor_properties[] = {
    /* number of interrupt sources including interrupt source 0 */
    DEFINE_PROP_UINT32("num-sources", RISCVLiteExecutor, num_sources, 1),
    DEFINE_PROP_END_OF_LIST(),
};

static void riscv_lite_executor_class_init(ObjectClass *obj, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(obj);

    // 为调度器注册新增的属性
    device_class_set_props(dc, riscv_lite_executor_properties);

    dc->realize = riscv_lite_executor_realize;
}

static const TypeInfo riscv_lite_executor_info = {
    .name          = TYPE_RISCV_LITE_EXECUTOR,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(RISCVLiteExecutor),
    .class_init    = riscv_lite_executor_class_init,
};

DeviceState *riscv_lite_executor_create(hwaddr addr, uint32_t num_sources)
{
    qemu_log("CREATE LITE EXECUTOR\n"); // 改了create的大小写:)

    DeviceState *dev = qdev_new(TYPE_RISCV_LITE_EXECUTOR);

    // 设置新增的属性
    qdev_prop_set_uint32(dev, "num-sources", num_sources);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, addr);

    // 因为lite_executor没有和CPU连接，所以不需要调用`qdev_connect_gpio_out`连接到CPU的中断线。

    return dev;
}

static void riscv_lite_executor_register_types(void)
{
    type_register_static(&riscv_lite_executor_info);
}

type_init(riscv_lite_executor_register_types)