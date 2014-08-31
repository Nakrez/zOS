#include <kernel/cpu.h>
#include <kernel/kmalloc.h>
#include <kernel/panic.h>
#include <kernel/console.h>
#include <kernel/process.h>
#include <kernel/zos.h>

static struct cpu *cpus;
static struct process *idle;

static void idle_thread(void)
{
    while (1)
    {
        for (volatile int i = 0; i < 0x1000000; ++i)
            ;
    }
}

void cpu_initialize(void)
{
    cpus = kmalloc(sizeof (struct cpu) * CPU_COUNT);

    if (!cpus)
        kernel_panic("cpu_initialize(): No memory left for cpu structures");

    for (int i = 0; i < CPU_COUNT; ++i)
    {
        cpus[i].id = i;
        scheduler_initialize(&cpus[i].scheduler);

        klist_head_init(&cpus[i].timers);
    }

    console_message(T_OK, "%u CPU initialized", CPU_COUNT);

    idle = process_create(PROCESS_TYPE_KERNEL, (uintptr_t)idle_thread, 0);

    cpus[0].scheduler.idle = klist_elem(idle->threads.next, struct thread,
                                        list);

    klist_del(cpus[0].scheduler.threads.next);

    /* FIXME: Add idle thread to cpu scheduler */
    for (int i = 1; i < CPU_COUNT; ++i)
        thread_create(idle, (uintptr_t)idle_thread, 0, 0, 0);

    console_message(T_OK, "Kernel idle process initialized");
}

void cpu_add_thread(struct thread *thread)
{
    struct cpu *cpu = &cpus[0];
    size_t num_thread = cpus[0].scheduler.thread_num;

    for (int i = 1; i < CPU_COUNT; ++i)
    {
        if (cpus[i].scheduler.thread_num < num_thread)
            cpu = &cpus[i];
    }

    thread->cpu = cpu->id;

    scheduler_add_thread(&cpu->scheduler, thread);
}

void cpu_start(void)
{
    struct cpu *cpu = cpu_get(cpu_id_get());

    if (!cpu)
        kernel_panic("Unable to start CPU");

    /*
     * Init is called here because cpu_start is called by each processor
     * to initialize it specifically
     */
    glue_call(cpu, init, cpu);

    console_message(T_OK, "Cpu %i started", cpu->id);

    scheduler_start(&cpu->scheduler);
}

struct cpu *cpu_get(int id)
{
    struct cpu *cpu = NULL;

    for (int i = 0; i < CPU_COUNT; ++i)
    {
        if (cpus[i].id == id)
            return &cpus[i];
    }

    return cpu;
}
