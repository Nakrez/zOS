#ifndef EXT2_FS_H
# define EXT2_FS_H

# include <stdint.h>

# include <fiu/fiu.h>

# include "inode.h"

# define EXT2_SB_MAGIC 0xEF53

struct ext2_superblock {
    uint32_t total_inodes;
    uint32_t total_blocks;
    uint32_t block_reserved;
    uint32_t unallocated_inodes;
    uint32_t unallocated_blocks;
    uint32_t sb_inode;
    uint32_t block_size;
    uint32_t frag_size;
    uint32_t block_per_group;
    uint32_t frag_per_group;
    uint32_t inode_per_group;
    uint32_t last_mount_time;
    uint32_t last_write_time;
    uint16_t mount_number;
    uint16_t mount_number_no_fsck;
    uint16_t magic;
    uint16_t state;
    uint16_t error_behaviour;
    uint16_t minor;
    uint32_t last_check_time;
    uint32_t time_interval_check;
    uint32_t creation_os;
    uint32_t major;
    uint16_t uid_reserved;
    uint16_t gid_reserved;

    /* Extended superblock fields */
    uint32_t first_non_reserved_inode;
    uint16_t sizeof_inode;
    uint16_t sb_group;
    uint32_t optional_features;
    uint32_t required_features;
    uint32_t non_supported_features;
    uint32_t fs_id[4];
    char volume_name[16];
    char last_mount_point[64];
    uint32_t compression_algo;
    uint8_t block_preallocated_files;
    uint8_t block_preallocated_dirs;
    uint16_t unused;
    uint32_t journal_id[4];
    uint32_t journal_inode;
    uint32_t journal_device;
    uint32_t orphan_heap;
    char pad[788];
} __attribute__ ((packed));

struct ext2_group_descriptor {
    uint32_t addr_block_usage_bitmap;
    uint32_t addr_inode_usage_bitmap;
    uint32_t inode_table;
    uint16_t unallocated_blocks;
    uint16_t unallocated_inodes;
    uint16_t directories_in_grp;
    char unused[14];
} __attribute__ ((packed));

struct ext2_dirent {
    uint32_t inode;
    uint16_t size;
    uint8_t name_size_low;
    uint8_t type;
};

struct ext2_cinode;

struct ext2fs {
    int fd;

    struct fiu_internal *fiu;

    size_t block_size;

    struct ext2_cinode *inode_cache;

    struct ext2_superblock sb;

    struct ext2_group_descriptor *grp_table;
};

int ext2fs_initialize(struct ext2fs *ext2, const char *disk);

int ext2fs_lookup(struct fiu_internal *fiu, struct req_lookup *req,
                  struct resp_lookup *response);

int ext2fs_stat(struct fiu_internal *fiu, struct req_stat *req,
                struct stat *response);

int ext2fs_mount(struct fiu_internal *fiu, struct req_mount *req);

#endif /* !EXT2_FS_H */
