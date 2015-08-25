#ifndef LIBFIU_FIU_H
# define LIBFIU_FIU_H

# include <zos/vfs.h>

# include <fiu/opts.h>
# include <fiu/block_cache.h>

struct fiu_instance;
struct req_fs_create;

struct fiu_ops {
    int (*init)(struct fiu_instance *);
    int (*lookup)(struct fiu_instance*, struct req_lookup *,
                  struct resp_lookup *);
    int (*stat)(struct fiu_instance *, struct req_stat *, struct stat *);
    int (*mount)(struct fiu_instance *, struct req_mount *);
    int (*open)(struct fiu_instance *, struct req_open *, struct resp_open *);
    int (*read)(struct fiu_instance *, struct req_rdwr *, size_t *);
    int (*getdirent)(struct fiu_instance *, struct req_getdirent*,
                     struct dirent *);
    int (*close)(struct fiu_instance *, struct req_close *);
};

/**
 *  \brief  Represents an instance of a file system
 */
struct fiu_instance {
    /**
     *  \brief  The file system
     */
    struct fiu_fs *parent;

    /**
     *  \brief  Private data for the driver
     */
    void *private;

    /**
     *  \brief  The file descriptor that represents the channel
     */
    int channel_fd;

    /**
     *  \brief  The file descriptor that represents the device
     */
    int device_fd;

    /**
     *  \brief  Cache of block
     */
    struct fiu_cache *block_cache;
};

/**
 *  \brief  Represents a file system
 */
struct fiu_fs {
    /**
     *  \brief  The name of the file system
     */
    const char *name;

    /**
     *  \brief  The file descriptor that represents the master channel
     */
    int channel_fd;

    /**
     *  \brief  Set of callbacks that allows file system creation/deletion
     */
    struct fiu_fs_super_ops *super_ops;

    /**
     *  \brief  Set of callbacks that allows file system manipulation
     */
    struct fiu_ops *ops;

    /**
     *  \brief  Capabilities of the file system
     */
    vop_t cap;
};

struct fiu_fs_super_ops {
    int (*create)(struct fiu_fs *, struct req_fs_create *);
};

int fiu_slave_main(struct fiu_instance *fi, const char *device,
                   uint16_t slave_id);

/**
 *  \brief  Main function to operate a master file system
 *
 *  \param  fs      The fs you want to operate
 *  \param  argc    The number of argument
 *  \param  argv    The arguments
 *
 *  \return 0: Everything went well
 */
int fiu_master_main(struct fiu_fs *fs, int argc, char *argv[]);

#endif /* !LIBFIU_FIU_H */
