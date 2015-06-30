/*
 * zOS
 * Copyright (C) 2014 - 2015 Baptiste Covolato
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zOS.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file    kernel/core/fs/tmpfs.c
 * \brief   Implementation of temporary filesystem
 *
 * \todo    Code is shared between tmpfs_mkdir/tmpfs_mknod
 * \todo    Make tmpfs fit on the data size given
 * \todo    tmpfs_cleanup()
 * \todo    Handle time creation/access/status changed
 *
 * \author  Baptiste Covolato
 */

#include <string.h>

#include <kernel/errno.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/tmpfs.h>

#include <kernel/fs/vfs/message.h>
#include <kernel/fs/vfs/mount.h>

#include <arch/spinlock.h>

# define TMPFS_DATA_SIZE (256 * KB)

/**
 * \brief   Allocate an inode into a tmpfs
 *
 * \param   sb  The super block of the tmpfs
 *
 * \return  The number of the inode if success
 * \return  -1 if error
 */
static int tmpfs_alloc_inode(struct tmpfs_sb *sb)
{
    spinlock_lock(&sb->inode_table_lock);

    /* Find a free spot */
    for (size_t i = 0; i < sb->inode_table_size; ++i)
    {
        if (!sb->inode_table[i])
        {
            if (!(sb->inode_table[i] = kmalloc(sizeof (struct tmpfs_node))))
                goto error;

            spinlock_unlock(&sb->inode_table_lock);

            return i;
        }
    }

    /* Reallocate the inode table */
    struct tmpfs_node **tmp = krealloc(sb->inode_table,
                                       sizeof (struct tmpfs_node *) *
                                       sb->inode_table_size * 2);
    /* The index of the future new spot in the inode table */
    size_t index = sb->inode_table_size;

    if (!tmp)
        goto error;

    sb->inode_table = tmp;

    /* Set the new part of the table as NULL */
    memset(sb->inode_table + sb->inode_table_size, 0, sb->inode_table_size *
           sizeof (struct tmpfs_node *));

    sb->inode_table_size *= 2;

    if (!(sb->inode_table[index] = kmalloc(sizeof (struct tmpfs_node))))
        goto error;

    spinlock_unlock(&sb->inode_table_lock);

    return index;

error:
    spinlock_unlock(&sb->inode_table_lock);

    /* Only memory allocation problem could occur */
    return -ENOMEM;
}

/**
 * \brief   Add a new entry to a directory
 *
 * \param   dir     The directory
 * \param   name    The name of the entry
 * \param   inode   The inode of the entry
 *
 * \return  0: Success
 * \return  -ENOMEM: Not enough memory
 */
static int tmpfs_add_dirent(struct tmpfs_node *dir, const char *name,
                            ino_t inode)
{
    struct tmpfs_dirent *dirent;

    if (!(dirent = kmalloc(sizeof (struct tmpfs_dirent))))
        return -ENOMEM;

    if (!(dirent->name = kmalloc(strlen(name) + 1)))
    {
        kfree(dirent);

        return -ENOMEM;
    }

    dirent->inode = inode;
    strcpy(dirent->name, name);

    klist_add_back(&dir->entries, &dirent->next);

    return 0;
}

/**
 * \brief   This function initialize directory block content with dir "." and
 *          ".."
 *
 * \todo    cleanup on error
 *
 * \param   dir         The node of the directory
 * \param   parent      The inode number of the parent directory
 * \param   dir_inode   The inode number of the directory
 *
 * \return  0: Success
 * \return  -ENOMEM: Not enough memory
 */
static int tmpfs_init_dir_content(struct tmpfs_node *dir, ino_t parent,
                                  ino_t dir_inode)
{
    int ret;

    klist_head_init(&dir->entries);

    if ((ret = tmpfs_add_dirent(dir, ".", dir_inode)) < 0)
        return ret;

    if ((ret = tmpfs_add_dirent(dir, "..", parent)) < 0)
        return ret;

    return 0;
}

/**
 * \brief   Initialize root node of a tmpfs
 *
 * \todo    Do something for the root uid, gid and default perm
 * \todo    Free resources when error
 * \todo    ".." should point to the previous dir
 *
 * \param   sb  The super block of the tmpfs
 *
 * \return  0: OK
 * \return  -ENOMEM: Not enough memory
 */
static int tmpfs_init_root(struct tmpfs_sb *sb)
{
    struct tmpfs_node *root;
    int ret;

    /* Allocate the root node structure */
    if (tmpfs_alloc_inode(sb) != TMPFS_ROOT_INODE)
        return -ENOMEM;

    root = sb->inode_table[TMPFS_ROOT_INODE];

    /* \0 */
    if (!(root->name = kmalloc(1)))
        return -ENOMEM;

    *(root->name) = '\0';

    root->inode = TMPFS_ROOT_INODE;

    root->uid = 0;
    root->gid = 0;
    root->type_perm = TMPFS_TYPE_DIR | VFS_PERM_USER_RWX;

    spinlock_init(&root->lock);

    /* Create "." and ".." both poiting on root */
    ret = tmpfs_init_dir_content(root, TMPFS_ROOT_INODE, TMPFS_ROOT_INODE);

    if (ret < 0)
        return ret;

    return 0;
}

static void *tmpfs_initialize(void)
{
    struct tmpfs_sb *sb = kmalloc(sizeof (struct tmpfs_sb));

    if (!sb)
        return NULL;

    /* Set this in case of error */
    sb->blocks = NULL;
    sb->block_free = NULL;
    sb->inode_table = NULL;

    /* Allocate inode table */
    sb->inode_table = kmalloc(sizeof (struct tmpfs_node *) *
                              TMPFS_INODE_TABLE_INIT_SIZE);

    if (!sb->inode_table)
        goto error;

    sb->inode_table_size = TMPFS_INODE_TABLE_INIT_SIZE;

    /* Allocate datas */
    /* FIXME: Make size of tmpfs dynamic instead of TMPFS_DATA_SIZE */
    if (!(sb->blocks = kmalloc(TMPFS_DATA_SIZE)))
        goto error;

    /* Allocate bitmap */
    if (!(sb->block_free = kmalloc(TMPFS_DATA_SIZE / TMPFS_BLOCK_SIZE)))
        goto error;

    spinlock_init(&sb->inode_table_lock);
    spinlock_init(&sb->lock_block);

    /* Initialize inode table with NULL ptrs */
    memset(sb->inode_table, 0, sizeof (struct tmpfs_node *) *
           TMPFS_INODE_TABLE_INIT_SIZE);

    /* Mark all blocks as free */
    memset(sb->block_free, 1, TMPFS_DATA_SIZE / TMPFS_BLOCK_SIZE);

    if (tmpfs_init_root(sb) < 0)
        goto error;

    return sb;

error:
    if (sb)
    {
        kfree(sb->inode_table);
        kfree(sb->blocks);
        kfree(sb->block_free);
        kfree(sb);
    }

    return NULL;
}

static int tmpfs_lookup(struct mount_entry *root, const char *path,
                        uid_t uid, gid_t gid, struct resp_lookup *ret)
{
    (void)uid;
    (void)gid;

    int found = 0;
    char *part;
    char *path_left;
    char *path_copy;
    struct tmpfs_sb *sb = root->private;
    struct tmpfs_node *node = sb->inode_table[TMPFS_ROOT_INODE];
    struct tmpfs_dirent *dirent;

    ret->processed = 0;

    /* Do we look for root */
    if (!strcmp(path, "") || !strcmp(path, "/"))
    {
        if (!strcmp(path, "/"))
            ret->processed = 1;

        ret->inode = TMPFS_ROOT_INODE;
        ret->dev = TMPFS_DEV_ID;
        ret->ret = RES_OK;

        /* ret->processed = 1; */

        return 0;
    }

    if (!(path_copy = kmalloc(strlen(path) + 1)))
        return -ENOMEM;

    if (*path == '/')
        ret->processed = 1;

    /* Ignore leading / */
    if (*path == '/')
        strcpy(path_copy, path + 1);
    else
        strcpy(path_copy, path);

    part = strtok_r(path_copy, "/", &path_left);

    while (part)
    {
        struct tmpfs_node *tmp = node;
        spinlock_lock(&node->lock);

        klist_for_each_elem(&node->entries, dirent, next)
        {
            if (!strcmp(part, dirent->name))
            {
                /* Get inode structure from inode table */
                node = sb->inode_table[dirent->inode];

                found = 1;

                break;
            }
        }

        spinlock_unlock(&tmp->lock);

        if (!found)
        {
            kfree(path_copy);

            ret->inode = node->inode;
            ret->ret = RES_KO;

            return 0;
        }

        found = 0;

        ret->processed += path_left - part;

        /* Something is mounted on this directory */
        if (node->type_perm & TMPFS_TYPE_MOUNT)
        {
            ret->ret = RES_ENTER_MOUNT;

            /* When the type is a mount point the size is the device id */
            ret->dev = node->size;
            ret->inode = node->inode;

            /*
             * If we enter a mount point we did not processed the / which is
             * the root inside the mount point
             */
            if (path[ret->processed - 1] == '/')
                --ret->processed;

            kfree(path_copy);

            return 0;
        }

        part = strtok_r(NULL, "/", &path_left);
    }

    ret->inode = node->inode;
    ret->ret = RES_OK;
    ret->dev = node->size;

    kfree(path_copy);

    return 0;
}

static int tmpfs_mkdir(struct mount_entry *root, const char *path, ino_t inode,
                       uid_t uid, gid_t gid, mode_t mode)
{
    int ret;
    int new_dir_inode;
    struct tmpfs_sb *sb = root->private;
    struct tmpfs_node *dir = sb->inode_table[inode];
    struct tmpfs_node *new_dir;

    if (!*path)
        return -EEXIST;

    if (strchr(path, '/'))
        return -ENOENT;

    /* Allocate new inode */
    if ((new_dir_inode = tmpfs_alloc_inode(sb)) < 0)
        return -ENOMEM;

    new_dir = sb->inode_table[new_dir_inode];

    /* FIXME: Remove new_dir_inode */
    if (!(new_dir->name = kmalloc(strlen(path) + 1)))
        return -ENOMEM;

    /* Fill/Init inode structure */
    strcpy(new_dir->name, path);

    new_dir->inode = new_dir_inode;
    new_dir->type_perm = TMPFS_TYPE_DIR | (mode & (VFS_PERM_USER_RWX |
                                                   VFS_PERM_GROUP_RWX |
                                                   VFS_PERM_OTHER_RWX));
    new_dir->uid = uid;
    new_dir->gid = gid;

    klist_head_init(&new_dir->entries);
    spinlock_init(&new_dir->lock);

    /* We add it to it parent dirent list */
    /* FIXME: Cleanup */
    if ((ret = tmpfs_add_dirent(dir, path, new_dir->inode)) < 0)
        return ret;

    return 0;
}

static int tmpfs_mknod(struct mount_entry *root, const char *path, ino_t inode,
                       uid_t uid, gid_t gid, mode_t mode, dev_t dev)
{
    int ret;
    int new_ent_inode;
    struct tmpfs_sb *sb = root->private;
    struct tmpfs_node *dir = sb->inode_table[inode];
    struct tmpfs_node *new_ent;

    if (!*path)
        return -EEXIST;

    if (strchr(path, '/'))
        return -ENOENT;

    /* Allocate new inode */
    if ((new_ent_inode = tmpfs_alloc_inode(sb)) < 0)
        return -ENOMEM;

    new_ent = sb->inode_table[new_ent_inode];

    /* FIXME: Remove new_ent_inode */
    if (!(new_ent->name = kmalloc(strlen(path) + 1)))
        return -ENOMEM;

    /* Fill/Init inode structure */
    strcpy(new_ent->name, path);

    new_ent->inode = new_ent_inode;
    new_ent->type_perm = TMPFS_TYPE_DEV | (mode & (VFS_PERM_USER_RWX |
                                                   VFS_PERM_GROUP_RWX |
                                                   VFS_PERM_OTHER_RWX));
    new_ent->uid = uid;
    new_ent->gid = gid;
    new_ent->size = dev;

    spinlock_init(&new_ent->lock);

    /* We add it to it parent dirent list */
    /* FIXME: Cleanup */
    if ((ret = tmpfs_add_dirent(dir, path, new_ent->inode)) < 0)
        return ret;

    return 0;
}

static int tmpfs_stat(struct mount_entry *root, uid_t uid, gid_t gid,
                      ino_t inode, struct stat *stat)
{
    (void) uid;
    (void) gid;

    struct tmpfs_sb *sb = root->private;
    struct tmpfs_node *node = sb->inode_table[inode];

    if (!node)
        return -ENOENT;

    stat->st_dev = TMPFS_DEV_ID;
    stat->st_ino = inode;

    /* Convert TMPFS file type to VFS file type */
    stat->st_mode = node->type_perm & 0777;

    if (node->type_perm & TMPFS_TYPE_DIR)
        stat->st_mode |= VFS_FTYPE_DIR;
    if (node->type_perm & TMPFS_TYPE_DEV)
        stat->st_mode |= VFS_FTYPE_DEV;
    if (node->type_perm & TMPFS_TYPE_FILE)
        stat->st_mode |= VFS_FTYPE_FILE;

    stat->st_nlink = 0;
    stat->st_uid = node->uid;
    stat->st_gid = node->gid;

    /* This field will be valid only if the node is a device */
    stat->st_rdev = node->size;

    stat->st_size = node->size;

    stat->st_blksize = node->size / TMPFS_BLOCK_SIZE;
    stat->st_blocks = node->size / 512;

    stat->st_atime = 0;
    stat->st_mtime = 0;
    stat->st_ctime = 0;

    return 0;
}

static int tmpfs_mount(struct mount_entry *root, ino_t inode, int mount_pt_nb)
{
    struct tmpfs_sb *sb = root->private;
    struct tmpfs_node *dir = sb->inode_table[inode];

    if (!(dir->type_perm & TMPFS_TYPE_DIR))
        return -ENOTDIR;

    /* TODO: Check busy */
    dir->size = mount_pt_nb;
    dir->type_perm |= TMPFS_TYPE_MOUNT;

    return 0;
}

static int tmpfs_open(struct mount_entry *root, ino_t inode, pid_t pid,
                      uid_t uid, gid_t gid, int flags, mode_t mode)
{
    (void)root;
    (void)inode;
    (void)pid;
    (void)uid;
    (void)gid;
    (void)flags;
    (void)mode;

    return 0;
}

static int tmpfs_getdirent(struct mount_entry *root, ino_t inode,
                           struct dirent *dirent, int index)
{
    struct tmpfs_sb *sb = root->private;
    struct tmpfs_node *dir = sb->inode_table[inode];
    struct tmpfs_dirent *ent;
    int i = 0;

    /* XXX: Possible ? */
    if (!dir)
        return -ENOENT;

    if (index < 0)
        return -EINVAL;

    spinlock_lock(&dir->lock);

    klist_for_each_elem(&dir->entries, ent, next)
    {
        if (i == index)
        {
            spinlock_unlock(&dir->lock);

            dirent->d_ino = ent->inode;
            strcpy(dirent->d_name, ent->name);

            /* FIXME: DIR_OK */
            return 1;
        }

        ++i;
    }

    spinlock_unlock(&dir->lock);

    /* FIXME: DIR_KO */
    return 0;
}

static int tmpfs_close(struct mount_entry *root, ino_t inode)
{
    (void) root;
    (void) inode;

    return 0;
}

static void tmpfs_cleanup(void *data)
{
    (void)data;
}

struct fs_operation tmpfs_fs_ops = {
    .init = tmpfs_initialize,
    .lookup = tmpfs_lookup,
    .mkdir = tmpfs_mkdir,
    .mknod = tmpfs_mknod,
    .stat = tmpfs_stat,
    .mount = tmpfs_mount,
    .open = tmpfs_open,
    .getdirent = tmpfs_getdirent,
    .close = tmpfs_close,
    .cleanup = tmpfs_cleanup,
};
