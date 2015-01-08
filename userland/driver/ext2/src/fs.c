#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <zos/print.h>

#include "fs.h"
#include "inode_cache.h"
#include "block.h"

# define EXT2FS_OPEN_TIMEOUT 1000
# define EXT2FS_OPEN_RETRY 50

static int ext2fs_load_group_table(struct ext2fs *ext2)
{
    int ret;
    size_t group_nb;
    size_t inode_nb;
    size_t gt_size;
    size_t offset;

    group_nb = (ext2->sb.total_blocks / ext2->sb.block_per_group) +
               ((ext2->sb.total_blocks % ext2->sb.block_per_group) ? 1 : 0);

    inode_nb = (ext2->sb.total_inodes / ext2->sb.inode_per_group) +
               ((ext2->sb.total_inodes % ext2->sb.inode_per_group) ? 1 : 0);

    gt_size = (group_nb > inode_nb ? group_nb : inode_nb);

    ext2->grp_table = malloc(sizeof (struct ext2_group_descriptor) * gt_size);

    if (!ext2->grp_table)
        return 0;

    offset = ext2->block_size == 1024 ? 2048 : ext2->sb.block_size;

    if (lseek(ext2->fd, offset, SEEK_SET) < 0)
        return 0;

    ret = read(ext2->fd, ext2->grp_table,
               sizeof (struct ext2_group_descriptor) * gt_size);

    if (ret < 0)
        return 0;

    return (size_t)ret == sizeof (struct ext2_group_descriptor) * gt_size;
}

int ext2fs_initialize(struct ext2fs *ext2, const char *disk)
{
    int ret;
    int read_size = 0;
    int timeout = 0;

    /* Wait for the disk driver to be alive */
    while (timeout < EXT2FS_OPEN_TIMEOUT)
    {
        ext2->fd = open(disk, 0, 0);

        if (ext2->fd >= 0)
            break;

        timeout += EXT2FS_OPEN_RETRY;
        usleep(EXT2FS_OPEN_RETRY);
    }

    if (ext2->fd < 0)
        return 0;

    /* Superblock is always 1024 bytes after the beginning */
    if (lseek(ext2->fd, 1024, SEEK_CUR) < 0)
        return 0;

    while (read_size != sizeof (struct ext2_superblock))
    {
        ret = read(ext2->fd, ((char *)&ext2->sb) + read_size,
                   sizeof (struct ext2_superblock) - read_size);

        if (ret < 0)
            break;

        read_size += ret;
    }

    if (ret < 0)
        return 0;

    if (ext2->sb.magic != EXT2_SB_MAGIC)
        return 0;

    ext2->block_size = 1024 << ext2->sb.block_size;

    if (!ext2fs_load_group_table(ext2))
        return 0;

    if (!ext2_icache_initialize(ext2))
        return 0;

    return (fiu_cache_initialize(&ext2->fiu, 64, ext2->block_size,
                                 ext2_block_fetch, ext2_block_flush) == 0);
}

uint32_t inode_find_in_dir(struct ext2fs *ext2, struct ext2_inode *inode,
                           const char *name)
{
    uint32_t offset = 0;
    uint32_t offset_block = 0;
    uint32_t block_num = 0;
    uint32_t res = 0;
    void *block = NULL;
    struct ext2_dirent *dirent;
    char *dirent_name;

    if (!inode_block_data(ext2, inode, 0, &block_num))
        return 0;

    if (!(block = fiu_cache_request(&ext2->fiu, block_num)))
        return 0;

    dirent = block;

    while (offset < inode->lower_size)
    {
        dirent_name = (char *)dirent + sizeof (struct ext2_dirent);

        if (dirent->size + offset_block > ext2->block_size)
        {
            uprint("Dirent overlap between 2 blocks");

            return 0;
        }

        if (!strncmp(dirent_name, name, dirent->name_size_low) &&
            strlen(name) == dirent->name_size_low)
        {
            res = dirent->inode;

            break;
        }

        offset_block += dirent->size;
        offset += dirent->size;

        if (offset_block >= ext2->block_size)
        {
            offset_block = offset % ext2->block_size;

            fiu_cache_release(&ext2->fiu, block_num);

            if (!inode_block_data(ext2, inode, offset, &block_num))
                return 0;

            if (!(block = fiu_cache_request(&ext2->fiu, block_num)))
                return 0;
        }

        dirent = (void *)((char *)block + offset_block);
    }

    fiu_cache_release(&ext2->fiu, block_num);

    return res;
}

void ext2_root_remount(struct fiu_internal *fiu, struct req_root_remount *req)
{
    int ret;
    struct ext2fs *ext2 = fiu->private;
    struct req_lookup reql;
    struct resp_lookup resp;
    struct ext2_inode *inode;

    reql.path = req->path;
    reql.path_size = strlen(req->path);
    reql.uid = 0;
    reql.gid = 0;

    ret = ext2fs_lookup(fiu, &reql, &resp);

    if (ret != LOOKUP_RES_OK)
        return;

    if (!(inode = ext2_icache_request(ext2, resp.inode)))
        return;

    if ((inode->type_perm & EXT2_TYPE_DIRECTORY) != EXT2_TYPE_DIRECTORY)
    {
        ext2_icache_release(ext2, resp.inode);

        return;
    }

    inode->type_perm |= EXT2_TYPE_MOUNT_PT;
    inode->lower_size = req->mount_pt;
}

int ext2fs_lookup(struct fiu_internal *fiu, struct req_lookup *req,
                  struct resp_lookup *response)
{
    struct ext2fs *ext2 = fiu->private;
    struct ext2_inode *inode = ext2_icache_request(ext2, 2);
    uint32_t inode_nb = 2;
    uint32_t tmp;
    char *path_complete = malloc(strlen(req->path) + 1);
    char *part;
    char *path_left;

    response->ret = -1;
    response->processed = 0;
    response->dev = -1;

    if (!inode)
        return LOOKUP_RES_KO;

    if (!(path_complete = malloc(strlen(req->path) + 1)))
        return LOOKUP_RES_KO;

    strcpy(path_complete, req->path);

    while (*req->path == '/')
    {
        ++req->path;
        ++response->processed;
    }

    part = strtok_r(req->path, "/", &path_left);

    while (1)
    {
        if ((inode->type_perm & EXT2_TYPE_DIRECTORY) != EXT2_TYPE_DIRECTORY)
        {
            ext2_icache_release(ext2, inode_nb);

            break;
        }

        if ((inode->type_perm & EXT2_TYPE_MOUNT_PT) == EXT2_TYPE_MOUNT_PT)
        {
            response->ret = LOOKUP_RES_ENTER_MOUNT;
            response->dev = inode->lower_size;
            ext2_icache_release(ext2, inode_nb);

            /*
             * If we enter a mount point we did not processed the / which is
             * the root inside the mount point
             */
            if (path_complete[response->processed - 1] == '/')
                --response->processed;

            break;
        }

        if (!(tmp = inode_find_in_dir(ext2, inode, part)))
        {
            ext2_icache_release(ext2, inode_nb);

            break;
        }

        ext2_icache_release(ext2, inode_nb);

        inode_nb = tmp;

        if (!(inode = ext2_icache_request(ext2, inode_nb)))
            break;

        response->processed += path_left - part;

        if (!(part = strtok_r(NULL, "/", &path_left)))
        {
            /* Special case: we asked directly for a mount point */
            if ((inode->type_perm & EXT2_TYPE_MOUNT_PT) == EXT2_TYPE_MOUNT_PT)
            {
                response->ret = LOOKUP_RES_ENTER_MOUNT;
                response->dev = inode->lower_size;

                /*
                 * If we enter a mount point we did not processed the / which
                 * is the root inside the mount point
                 */
                if (path_complete[response->processed - 1] == '/')
                    --response->processed;
            }

            ext2_icache_release(ext2, inode_nb);

            break;
        }
    }

    response->inode = inode_nb;

    free(path_complete);

    if (response->ret == -1)
    {
        if (response->processed == req->path_size)
            return LOOKUP_RES_OK;
        else
            return LOOKUP_RES_KO;
    }

    return response->ret;
}

int ext2fs_stat(struct fiu_internal *fiu, struct req_stat *req,
                struct stat *response)
{
    struct ext2fs *ext2 = fiu->private;
    struct ext2_inode *inode;

    if (!(inode = ext2_icache_request(ext2, req->inode)))
        return -1;

    response->st_dev = fiu->dev_id;
    response->st_ino = req->inode;
    response->st_mode = inode->type_perm;
    response->st_nlink = inode->hardlinks_count;
    response->st_uid = inode->uid;
    response->st_gid = inode->gid;
    response->st_rdev = 0;
    response->st_size = inode->lower_size;

    if (ext2->sb.minor > 0 || ext2->sb.major > 0)
    {
        if (!(inode->type_perm & EXT2_TYPE_DIRECTORY))
            response->st_size |= ((uint64_t)inode->directory_acl) << 32;
    }

    response->st_blksize = ext2->block_size;
    response->st_blocks = inode->disk_sector_size;
    response->st_atime = inode->last_access;
    response->st_mtime = inode->last_modification;
    response->st_ctime = inode->last_access;

    ext2_icache_release(ext2, req->inode);

    return 0;
}
