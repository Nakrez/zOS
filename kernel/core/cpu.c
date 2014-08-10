#include <kernel/cpu.h>
#include <kernel/kmalloc.h>
#include <kernel/panic.h>
#include <kernel/console.h>
#include <kernel/process.h>

static struct cpu *cpus;
static struct process *idle;

static void idle_thread(void)
{
    char message[5];

    message[0] = 'I';
    message[1] = 'D';
    message[2] = 'L';
    message[3] = 'E';
    message[4] = 0;

    while (1)
    {
        for (volatile int i = 0; i < 0x1000000; ++i)
            ;

        __asm__ __volatile__("mov %0, %%ebx\n"
                             "mov $1, %%eax\n"
                             "int $0x80\n"
                             :
                             : "r" (message));
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
    }

    console_message(T_OK, "%u CPU initialized", CPU_COUNT);

    idle = process_create(PROCESS_TYPE_KERNEL, (uintptr_t)idle_thread, 0);

    cpus[0].scheduler.idle = klist_elem(idle->threads.next, struct thread,
                                        list);

    klist_del(cpus[0].scheduler.threads.next);

    /* FIXME: Add idle thread to cpu scheduler */
    for (int i = 1; i < CPU_COUNT; ++i)
        thread_create(idle, (uintptr_t)idle_thread);

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
    struct cpu *cpu = NULL;

    for (int i = 0; i < CPU_COUNT; ++i)
    {
        if (cpus[i].id == cpu_id_get())
        {
            cpu = &cpus[i];
            break;
        }
    }

    if (!cpu)
        kernel_panic("Unable to start CPU");

    /*
     * Init is called here because cpu_start is called by each processor
     * to initialize it specifically
     */
    cpu_glue_dispatcher.init(cpu);

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
