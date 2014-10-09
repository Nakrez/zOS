#ifndef EXT2_FS_H
# define EXT2_FS_H

# include <stdint.h>

# include <fiu/fiu.h>

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

# define EXT2_TYPE_FIFO 0x1000
# define EXT2_TYPE_CHARDEV 0x2000
# define EXT2_TYPE_DIRECTORY 0x4000
# define EXT2_TYPE_BLOCKDEV 0x6000
# define EXT2_TYPE_FILE 0x8000
# define EXT2_TYPE_SYMLINK 0xA000
# define EXT2_TYPE_USOCKET 0xC000

# define EXT2_PERM_OTHERX 00001
# define EXT2_PERM_OTHERW 00002
# define EXT2_PERM_OTHERR 00004
# define EXT2_PERM_GROUPX 00010
# define EXT2_PERM_GROUPW 00020
# define EXT2_PERM_GROUPR 00040
# define EXT2_PERM_USERX 00100
# define EXT2_PERM_USERW 00200
# define EXT2_PERM_USERR 00400
# define EXT2_PERM_STICKY 01000
# define EXT2_PERM_SGID 02000
# define EXT2_PERM_SUID 04000

struct ext2_inode {
    uint16_t type_perm;
    uint16_t uid;
    uint32_t lower_size;
    uint32_t last_access;
    uint32_t creation_time;
    uint32_t last_modification;
    uint32_t deletion_time;
    uint16_t gid;
    uint16_t hardlinks_count;
    uint32_t disk_sector_size;
    uint32_t flags;
    uint32_t os_specific1;
    uint32_t dbp[12];
    uint32_t singly_ibp;
    uint32_t doubly_ibp;
    uint32_t triply_ibp;
    uint32_t generation_number;
    uint32_t file_acl;
    uint32_t directory_acl;
    uint32_t block_address;
    uint16_t os_specific2[6];
};

struct ext2_cinode;

struct ext2fs {
    int fd;

    struct fiu_internal fiu;

    size_t block_size;

    struct ext2_cinode *inode_cache;

    struct ext2_superblock sb;

    struct ext2_group_descriptor *grp_table;
};

int ext2fs_initialize(struct ext2fs *ext2, const char *disk);

#endif /* !EXT2_FS_H */
