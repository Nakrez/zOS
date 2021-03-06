#include <string.h>

#include <kernel/errno.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/message.h>
#include <kernel/fs/vfs/mount.h>

int vfs_lookup(struct thread *t, const char *path, struct resp_lookup *res,
               struct mount_entry **mount_pt)
{
    int ret;
    int processed = 0;
    int path_len = strlen(path);
    struct mount_entry *root = vfs_root_get();
    char *copied_path = kmalloc(path_len + 1);
    uid_t uid;
    gid_t gid;

    /* Kernel request */
    if (!t) {
        uid = 0;
        gid = 0;
    } else {
        uid = t->uid;
        gid = t->gid;
    }

    if (!copied_path)
        return -ENOMEM;

    strcpy(copied_path, path);

    for (;;) {
        if (!root || !root->used) {
            kfree(copied_path);
            return -ENOENT;
        }

        if (!root->fi->parent->fs_ops->lookup) {
            kfree(copied_path);
            return -ENOSYS;
        }

        ret = root->fi->parent->fs_ops->lookup(root, path + processed, uid,
                                               gid, res);

        if (ret < 0) {
            kfree(copied_path);
            return ret;
        }

        processed += res->processed;

        if (!processed || processed > path_len) {
            kfree(copied_path);
            return -EBADE;
        }

        if (res->ret == RES_OK || res->ret == RES_KO)
            break;

        char old = copied_path[res->processed];

        copied_path[res->processed] = 0;
        root = vfs_mount_pt_get(copied_path);
        copied_path[res->processed] = old;
    }

    *mount_pt = root;

    res->processed = processed;

    kfree(copied_path);

    return processed;
}
