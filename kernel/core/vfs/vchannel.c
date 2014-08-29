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

static int inbox_msg_pop(struct klist *inbox_head, char *buf, size_t size)
{
    int mid;
    struct msg_list *mlist;

    mlist = klist_elem(inbox_head, struct msg_list, list);

    if (mlist->msg->size > size)
        return -EINVAL;

    klist_del(&mlist->list);

    memcpy(buf, mlist->msg + 1, mlist->msg->size);

    mid = mlist->msg->mid;

    kfree(mlist);

    return mid;
}

int channel_recv_request(struct vchannel *chan, char *buf, size_t size)
{
    spinlock_lock(&chan->inbox_lock);

    if (!klist_empty(&chan->inbox))
    {
        int res = inbox_msg_pop(&chan->inbox, buf, size);

        spinlock_unlock(&chan->inbox_lock);

        return res;
    }

    spinlock_unlock(&chan->inbox_lock);

    /* No message was in the channel, so block until we receive one */
    thread_block(thread_current(), SCHED_EV_REQ, chan->cid);

    spinlock_lock(&chan->inbox_lock);

    if (!klist_empty(&chan->inbox))
    {
        int res = inbox_msg_pop(&chan->inbox, buf, size);

        spinlock_unlock(&chan->inbox_lock);

        return res;
    }

    spinlock_unlock(&chan->inbox_lock);

    return -ENODATA;
}

void channel_destroy(struct vchannel *chan)
{
    if (!klist_empty(&chan->inbox) || !klist_empty(&chan->outbox))
        kernel_panic("Destroying channel with message...");

    kfree(chan);

    return;
}
