#ifndef EXT2_INODE_H
# define EXT2_INODE_H

# include <stdint.h>

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

struct ext2fs;

int inode_block_data(struct ext2fs *ext2, struct ext2_inode *inode,
                     uint64_t offset, uint32_t *block);

#endif /* !EXT2_INODE_H */
