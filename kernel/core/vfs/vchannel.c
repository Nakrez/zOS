#include <string.h>

#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/panic.h>
#include <kernel/thread.h>

#include <kernel/scheduler/event.h>

#include <kernel/vfs/vchannel.h>

static int cid = 0;

struct vchannel *channel_create(void)
{
    struct vchannel *chan = kmalloc(sizeof (struct vchannel));

    if (!chan)
        return NULL;

    spinlock_init(&chan->inbox_lock);
    spinlock_init(&chan->outbox_lock);

    klist_head_init(&chan->inbox);
    klist_head_init(&chan->outbox);

    chan->cid = cid++;

    return chan;
}

void channel_destroy(struct vchannel *chan)
{
    if (!klist_empty(&chan->inbox) || !klist_empty(&chan->outbox))
        kernel_panic("Destroying channel with message...");

    kfree(chan);

    return;
}
