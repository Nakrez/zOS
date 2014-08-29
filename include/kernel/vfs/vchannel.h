#ifndef VFS_VCHANNEL_H
# define VFS_VCHANNEL_H

# include <kernel/klist.h>

# include <kernel/vfs/message.h>

# include <arch/spinlock.h>

struct msg_list
{
    struct message *msg;

    struct klist list;
};

struct vchannel {
    int cid;

    spinlock_t inbox_lock;
    spinlock_t outbox_lock;

    struct klist inbox;

    struct klist outbox;
};

/*
 * Create a new channel, return the channel is success, NULL otherwise
 */
struct vchannel *channel_create(void);

/*
 * Destroy a channel
 */
void channel_destroy(struct vchannel *chan);

#endif /* !VFS_VCHANNEL_H */
