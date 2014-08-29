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

int channel_send_request(struct vchannel *chan, struct message *msg)
{
    struct msg_list *list;

    if (!(list = kmalloc(sizeof (struct msg_list))))
        return -ENOMEM;

    list->msg = msg;

    spinlock_lock(&chan->inbox_lock);

    klist_add_back(&chan->inbox, &list->list);

    spinlock_unlock(&chan->inbox_lock);

    /* Notify blocked thread that a new message income */
    scheduler_event_notify(SCHED_EV_REQ, chan->cid);

    return 0;
}

static int inbox_msg_pop(struct klist *inbox_head, char *buf, size_t size)
{
    int mid;
    struct msg_list *mlist;

    mlist = klist_elem(inbox_head->next, struct msg_list, list);

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

static int outbox_msg_pop(struct klist *outbox_head, uint32_t req_id,
                          struct message **msg)
{
    klist_for_each(outbox_head, mlist, list)
    {
        struct msg_list *list = klist_elem(mlist, struct msg_list, list);

        if (((struct answer_hdr *)(list->msg + 1))->req_id == req_id)
        {
            klist_del(&list->list);

            *msg = list->msg;

            kfree(list);

            return 1;
        }
    }

    return 0;
}

int channel_recv_response(struct vchannel *chan, uint32_t req_id,
                          struct message **msg)
{
    spinlock_lock(&chan->outbox_lock);

    if (!klist_empty(&chan->outbox) &&
        outbox_msg_pop(&chan->outbox, req_id, msg))
    {
        spinlock_unlock(&chan->outbox_lock);

        return 0;
    }

    spinlock_unlock(&chan->outbox_lock);

    thread_block(thread_current(), SCHED_EV_RESP, req_id);

    spinlock_lock(&chan->outbox_lock);

    if (!klist_empty(&chan->outbox) &&
        outbox_msg_pop(&chan->outbox, req_id, msg))
    {
        spinlock_unlock(&chan->outbox_lock);

        return 0;
    }

    spinlock_unlock(&chan->outbox_lock);

    return -ENODATA;
}

void channel_destroy(struct vchannel *chan)
{
    if (!klist_empty(&chan->inbox) || !klist_empty(&chan->outbox))
        kernel_panic("Destroying channel with message...");

    kfree(chan);

    return;
}
