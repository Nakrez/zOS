#ifndef FS_FIU_H
# define FS_FIU_H

# include <kernel/types.h>

# include <kernel/fs/vfs.h>

extern struct fs_super_operation fiu_fs_super_ops;
extern struct fs_operation fiu_fs_ops;
extern struct file_operation fiu_f_ops;

struct channel;

/**
 *  \brief  Private data used for FIU registered file systems
 */
struct fiu_fs {
    /**
     *  \brief  The channel used to communicate with fs controller
     */
    struct channel *master;

    /**
     *  \brief  Operation supported by the file system
     */
    vop_t ops;
};

/**
 *  \brief  Private data used for FIU instantiated file systems
 */
struct fiu_fs_instance {
    /**
     *  \brief  The channel used to communicate with the fs instance
     */
    struct channel *channel;
};

/**
 *  \brief  Private data used for FIU files
 */
struct fiu_file_private {
    /**
     *  \brief  The channel slave used to communication with the fs
     *          responsible for the file
     */
    struct channel_slave *slave;

    /**
     *  \brief  Operation supported by the file system
     */
    vop_t ops;

    /**
     *  \brief  Private data use reference counting for deallocation
     */
    int ref;
};

/**
 *  \brief  Private data used in device
 */
struct fiu_device_private {
    /**
     *  \brief  The channel used to communicate with the device
     */
    struct channel *master;
};

#endif /* !FS_FIU_H */
