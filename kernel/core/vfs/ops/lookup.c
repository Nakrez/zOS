#include <string.h>

#include <kernel/errno.h>
#include <kernel/kmalloc.h>

#include <kernel/vfs/vops.h>
#include <kernel/vfs/fs.h>
#include <kernel/vfs/mount.h>

int vfs_lookup(const char *path, int uid, int gid, struct resp_lookup *res,
               struct mount_entry **mount_pt)
{
    int ret;
    int processed = 0;
    struct mount_entry *root = vfs_root_get();
    char *copied_path = kmalloc(strlen(path) + 1);

    if (!copied_path)
        return -ENOMEM;

    strcpy(copied_path, path);

    while (1)
    {
        if (!root || !root->used)
        {
            kfree(copied_path);

            return -ENOENT;
        }

        if (!root->ops->lookup)
        {
            kfree(copied_path);

            return -ENOSYS;
        }

        ret = root->ops->lookup(root, path + processed, uid, gid, res);

        if (ret < 0)
        {
            kfree(copied_path);

            return ret;
        }

        processed += res->processed;

        if (res->ret == RES_OK || res->ret == RES_KO)
            break;
        else
        {
            copied_path[res->processed - 1] = 0;
            root = vfs_mount_pt_get(copied_path);
            copied_path[res->processed - 1] = '/';
        }
    }

    *mount_pt = root;

    kfree(copied_path);

    return processed;
}
