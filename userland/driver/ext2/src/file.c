#include <string.h>

#include "file.h"
#include "inode_cache.h"

# define MIN3(a, b, c) (a < b ? (a < c ? a : c) : (b < c ? b : c))

int ext2fs_open(struct fiu_internal *fiu, struct req_open *req,
                struct resp_open *response)
{
    struct ext2fs *ext2;
    struct ext2_inode *inode;

    ext2 = fiu->private;

    if (!(inode = ext2_icache_request(ext2, req->inode)))
        return -1;

    response->inode = req->inode;

    ext2_icache_release(ext2, req->inode);

    return 0;
}

int ext2fs_read(struct fiu_internal *fiu, struct req_rdwr *req, size_t *size)
{
    uint64_t file_size;
    uint32_t data_block;
    int read_size;
    struct ext2fs *ext2 = fiu->private;
    struct ext2_inode *inode = ext2_icache_request(ext2, req->inode);
    void *block;

    if (!inode)
        return -1;

    *size = 0;

    file_size = inode->lower_size;

    if (ext2->sb.minor > 0 || ext2->sb.major > 0)
    {
        if (!(inode->type_perm & EXT2_TYPE_DIRECTORY))
            file_size |= ((uint64_t)inode->directory_acl) << 32;
    }

    while (*size < req->size && req->off < file_size)
    {
        if (!inode_block_data(ext2, inode, req->off, &data_block))
        {
            ext2_icache_release(ext2, req->inode);

            return -1;
        }

        if (!(block = fiu_cache_request(&ext2->fiu, data_block)))
        {
            ext2_icache_release(ext2, req->inode);

            return -1;
        }

        read_size = MIN3(req->size,
                         ext2->block_size - (req->off % ext2->block_size),
                         file_size - req->off);

        memcpy(req->data, block, read_size);

        req->off += read_size;
        *size += read_size;
        req->data = (char *)req->data + read_size;

        fiu_cache_release(&ext2->fiu, data_block);
    }

    ext2_icache_release(ext2, req->inode);

    return 0;
}

int ext2fs_getdirent(struct fiu_internal *fiu, struct req_getdirent *req,
                     struct dirent *dirent)
{
    int ret;
    void *block = NULL;
    size_t dir_offset = 0;
    size_t block_offset = 0;
    uint32_t data_block;
    struct ext2fs *ext2 = (void *)fiu->private;
    struct ext2_inode *inode;
    struct ext2_dirent *ext2_dir;

    /* FIXME: EINVAL */
    if (req->index < 0)
        return -1;

    inode = ext2_icache_request(ext2, req->inode);

    /* FIXME: ??? */
    if (!inode)
        return -1;

    /* Fetch directory first data block */
    if (!inode_block_data(ext2, inode, 0, &data_block))
    {
        ret = -1;

        goto end;
    }

    if (!(block = fiu_cache_request(&ext2->fiu, data_block)))
    {
        ret = -1;

        goto end;
    }

    int i = 0;

    while (1)
    {
        ext2_dir = (void *)((uintptr_t)block + block_offset);

        if (i == req->index)
            break;

        dir_offset += ext2_dir->size;
        block_offset += ext2_dir->size;

        if (dir_offset >= inode->lower_size)
        {
            ret = DIR_END;

            goto end;
        }

        if (block_offset >= ext2->block_size)
        {
            fiu_cache_release(fiu, data_block);

            block = NULL;
            block_offset = dir_offset % ext2->block_size;

            if (!inode_block_data(ext2, inode, dir_offset, &data_block))
            {
                ret = -1;

                goto end;
            }

            if (!(block = fiu_cache_request(&ext2->fiu, data_block)))
            {
                ret = -1;

                goto end;
            }
        }

        ++i;
    }

    dirent->d_ino = ext2_dir->inode;

    memcpy(dirent->d_name, ext2_dir + 1, ext2_dir->name_size_low);

    dirent->d_name[ext2_dir->name_size_low] = 0;

    ret = DIR_OK;

end:
    if (block)
        fiu_cache_release(fiu, data_block);

    ext2_icache_release(ext2, req->inode);

    return ret;
}

int ext2fs_close(struct fiu_internal *fiu, struct req_close *req)
{
    (void)fiu;
    (void)req;

    return 0;
}
