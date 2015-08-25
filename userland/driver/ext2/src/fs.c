#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <zos/print.h>

#include "fs.h"
#include "inode_cache.h"
#include "block.h"

static int ext2fs_load_group_table(struct ext2fs *ext2)
{
    int ret;
    size_t group_nb;
    size_t inode_nb;
    size_t gt_size;
    size_t offset;
    struct fiu_instance *fi = ext2->fi;

    group_nb = (ext2->sb.total_blocks / ext2->sb.block_per_group) +
               ((ext2->sb.total_blocks % ext2->sb.block_per_group) ? 1 : 0);

    inode_nb = (ext2->sb.total_inodes / ext2->sb.inode_per_group) +
               ((ext2->sb.total_inodes % ext2->sb.inode_per_group) ? 1 : 0);

    gt_size = (group_nb > inode_nb ? group_nb : inode_nb);

    ext2->grp_table = malloc(sizeof (struct ext2_group_descriptor) * gt_size);

    if (!ext2->grp_table)
        /* ENOMEM */
        return -1;

    offset = ext2->block_size == 1024 ? 2048 : ext2->block_size;

    ret = lseek(fi->device_fd, offset, SEEK_SET);
    if (ret < 0)
        return ret;

    ret = read(fi->device_fd, ext2->grp_table,
               sizeof (struct ext2_group_descriptor) * gt_size);
    if (ret < 0)
        return ret;

    if ((size_t)ret != sizeof (struct ext2_group_descriptor) * gt_size)
        return -1;

    return 0;
}

int ext2fs_create(struct fiu_fs *fs, struct req_fs_create *req)
{
    (void) fs;
    (void) req;
    int ret;
    struct ext2fs *ext2;
    struct fiu_instance fi;

    ext2 = malloc(sizeof (struct ext2fs));
    if (!ext2)
        /* XXX: ENOMEM */
        return -1;

    fi.parent = fs;
    fi.private = ext2;

    ext2->fi = &fi;

    ret = fiu_slave_main(&fi, req->device, req->hdr.slave_id);
    if (ret < 0) {
        free(ext2);
        return ret;
    }

    return fi.channel_fd;
}

int ext2fs_initialize(struct fiu_instance *fi)
{
    int ret;
    int read_size = 0;
    struct ext2fs *ext2 = fi->private;

    /* Superblock is always 1024 bytes after the beginning */
    ret = lseek(fi->device_fd, 1024, SEEK_CUR);
    if (ret < 0)
        return ret;

    while (read_size != sizeof (struct ext2_superblock))
    {
        ret = read(fi->device_fd, ((char *)&ext2->sb) + read_size,
                   sizeof (struct ext2_superblock) - read_size);
        if (ret < 0)
            return ret;

        read_size += ret;
    }

    if (ext2->sb.magic != EXT2_SB_MAGIC)
        return -1;

    ext2->block_size = 1024 << ext2->sb.block_size;

    ret = ext2fs_load_group_table(ext2);
    if (ret < 0)
        return ret;

    ret = ext2_icache_initialize(ext2);
    if (ret < 0)
        return ret;

    return fiu_cache_initialize(ext2->fi, 64, ext2->block_size,
                                ext2_block_fetch, ext2_block_flush);
}

uint32_t inode_find_in_dir(struct ext2fs *ext2, struct ext2_inode *inode,
                           const char *name)
{
    int ret;
    uint32_t offset = 0;
    uint32_t offset_block = 0;
    uint32_t block_num = 0;
    uint32_t res = 0;
    void *block = NULL;
    struct ext2_dirent *dirent;
    char *dirent_name;

    ret = inode_block_data(ext2, inode, 0, &block_num);
    if (ret < 0)
        return ret;

    block = fiu_cache_request(ext2->fi, block_num);
    if (!block)
        return -1;

    dirent = block;

    while (offset < inode->lower_size)
    {
        dirent_name = (char *)dirent + sizeof (struct ext2_dirent);

        if (dirent->size + offset_block > ext2->block_size) {
            uprint("Dirent overlap between 2 blocks");
            return -1;
        }

        if (!strncmp(dirent_name, name, dirent->name_size_low) &&
            strlen(name) == dirent->name_size_low) {
            res = dirent->inode;
            break;
        }

        offset_block += dirent->size;
        offset += dirent->size;

        if (offset_block >= ext2->block_size) {
            offset_block = offset % ext2->block_size;

            fiu_cache_release(ext2->fi, block_num);

            ret = inode_block_data(ext2, inode, offset, &block_num);
            if (ret < 0)
                return -1;

            block = fiu_cache_request(ext2->fi, block_num);
            if (!block)
                return 0;
        }

        dirent = (void *)((char *)block + offset_block);
    }

    fiu_cache_release(ext2->fi, block_num);

    return res;
}

int ext2fs_lookup(struct fiu_instance *fi, struct req_lookup *req,
                  struct resp_lookup *resp)
{
    struct ext2fs *ext2 = fi->private;
    struct ext2_inode *inode = ext2_icache_request(ext2, 2);
    uint32_t inode_nb = 2;
    uint32_t tmp;
    char *path_complete;
    char *part;
    char *path_left;

    resp->ret = -1;
    resp->processed = 0;

    memset(&resp->inode, 0, sizeof (struct inode));

    resp->inode.dev = -1;

    if (!inode)
        return LOOKUP_RES_KO;

    path_complete = malloc(strlen(req->path) + 1);
    if (!path_complete)
        return LOOKUP_RES_KO;

    strcpy(path_complete, req->path);

    while (*req->path == '/') {
        ++req->path;
        ++resp->processed;
    }

    part = strtok_r(req->path, "/", &path_left);

    for (;;) {
        if ((inode->type_perm & EXT2_TYPE_DIRECTORY) != EXT2_TYPE_DIRECTORY) {
            ext2_icache_release(ext2, inode_nb);
            break;
        }

        if ((inode->type_perm & EXT2_TYPE_MOUNT_PT) == EXT2_TYPE_MOUNT_PT) {
            resp->ret = LOOKUP_RES_ENTER_MOUNT;
            resp->inode.dev = inode->lower_size;
            ext2_icache_release(ext2, inode_nb);

            /*
             * If we enter a mount point we did not processed the / which is
             * the root inside the mount point
             */
            if (path_complete[resp->processed - 1] == '/')
                --resp->processed;

            break;
        }

        tmp = inode_find_in_dir(ext2, inode, part);
        if (!tmp) {
            ext2_icache_release(ext2, inode_nb);
            break;
        }

        ext2_icache_release(ext2, inode_nb);

        inode_nb = tmp;

        inode = ext2_icache_request(ext2, inode_nb);
        if (!inode)
            break;

        resp->processed += path_left - part;

        part = strtok_r(NULL, "/", &path_left);
        if (!part) {
            /* Special case: we asked directly for a mount point */
            if ((inode->type_perm & EXT2_TYPE_MOUNT_PT) ==
                 EXT2_TYPE_MOUNT_PT) {
                resp->ret = LOOKUP_RES_ENTER_MOUNT;
                resp->inode.dev = inode->lower_size;

                /*
                 * If we enter a mount point we did not processed the / which
                 * is the root inside the mount point
                 */
                if (path_complete[resp->processed - 1] == '/')
                    --resp->processed;
            }

            ext2_icache_release(ext2, inode_nb);

            break;
        }
    }

    resp->inode.inode = inode_nb;

    free(path_complete);

    if (resp->ret == -1) {
        if (resp->processed == req->path_size)
            return LOOKUP_RES_OK;

        return LOOKUP_RES_KO;
    }

    return resp->ret;
}

int ext2fs_stat(struct fiu_instance *fi, struct req_stat *req,
                struct stat *resp)
{
    struct ext2fs *ext2 = fi->private;
    struct ext2_inode *inode;

    if (!(inode = ext2_icache_request(ext2, req->inode)))
        return -1;

    resp->st_dev = 0;
    resp->st_ino = req->inode;
    resp->st_mode = inode->type_perm;
    resp->st_nlink = inode->hardlinks_count;
    resp->st_uid = inode->uid;
    resp->st_gid = inode->gid;
    resp->st_rdev = 0;
    resp->st_size = inode->lower_size;

    if ((ext2->sb.minor > 0 || ext2->sb.major > 0) &&
        !(inode->type_perm & EXT2_TYPE_DIRECTORY))
            resp->st_size |= ((uint64_t)inode->directory_acl) << 32;

    resp->st_blksize = ext2->block_size;
    resp->st_blocks = inode->disk_sector_size;
    resp->st_atime = inode->last_access;
    resp->st_mtime = inode->last_modification;
    resp->st_ctime = inode->last_access;

    ext2_icache_release(ext2, req->inode);

    return 0;
}

int ext2fs_mount(struct fiu_instance *fi, struct req_mount *req)
{
    struct ext2fs *ext2 = fi->private;
    struct ext2_inode *inode;

    /* FIXME: ENOENT */
    if (!(inode = ext2_icache_request(ext2, req->inode)))
        return -1;

    if ((inode->type_perm & EXT2_TYPE_DIRECTORY) != EXT2_TYPE_DIRECTORY)
    {
        ext2_icache_release(ext2, req->inode);

        /* FIXME: ENOTDIR */
        return -1;
    }

    inode->type_perm |= EXT2_TYPE_MOUNT_PT;
    inode->lower_size = req->mount_nb;

    /* The inode is not released to keep these mount information */

    return 0;
}
