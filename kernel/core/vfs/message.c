#include <kernel/kmalloc.h>

#include <kernel/vfs/message.h>

#include <arch/spinlock.h>

static struct klist msg_cache = { &msg_cache, &msg_cache };
static spinlock_t cache_lock = SPINLOCK_INIT;

static uint32_t mid = 0;
static spinlock_t mid_lock = SPINLOCK_INIT;

struct message *message_alloc(size_t size)
{
    struct message *res;

    spinlock_lock(&cache_lock);

    klist_for_each(&msg_cache, mlist, list)
    {
        struct free_msg *msg = klist_elem(mlist, struct free_msg, list);

        if (msg->msg->max_size >= size)
        {
            klist_del(&msg->list);

            spinlock_unlock(&cache_lock);

            res = msg->msg;

            kfree(msg);

            return res;
        }
    }

    spinlock_unlock(&cache_lock);

    res = kmalloc(sizeof (struct message) + size);

    if (!res)
        return NULL;

    spinlock_lock(&mid_lock);

    res->mid = mid++;

    spinlock_unlock(&mid_lock);

    res->size = size;
    res->max_size = size;

    return res;
}

void message_free(struct message *msg)
{
    struct free_msg *free;

    if (!(free = kmalloc(sizeof (struct free_msg))))
    {
        /*
         * The mid is lost because we can't add it to the cache so destroy
         * the message.
         */

        kfree(msg);

        return;
    }

    free->msg = msg;

    spinlock_lock(&cache_lock);

    klist_add(&msg_cache, &free->list);

    spinlock_unlock(&cache_lock);
}
