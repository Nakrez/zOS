#ifndef FS_FIU_H
# define FS_FIU_H

# include <kernel/types.h>

# include <kernel/fs/vfs.h>

extern struct fs_super_operation fiu_fs_super_ops;
extern struct fs_operation fiu_fs_ops;
extern struct file_operation fiu_f_ops;

struct channel;

struct fiu_fs_private {
    /**
     *  \brief  The channel used to communicate with fs controller
     */
    struct channel *master;

    /**
     *  \brief  Operation supported by the file system
     */
    vop_t ops;
};

#endif /* !FS_FIU_H */
