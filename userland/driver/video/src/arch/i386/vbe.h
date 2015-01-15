#ifndef VBE_H
# define VBE_H

# define VBE_DISPI_IOPORT_INDEX 0x1CE
# define VBE_DISPI_IOPORT_DATA 0x1CF

# define VBE_DISPI_INDEX_ID 0
# define VBE_DISPI_INDEX_XRES 1
# define VBE_DISPI_INDEX_YRES 2
# define VBE_DISPI_INDEX_BPP 3
# define VBE_DISPI_INDEX_ENABLE 4
# define VBE_DISPI_INDEX_BANK 5
# define VBE_DISPI_INDEX_VIRT_WIDTH 6
# define VBE_DISPI_INDEX_VIRT_HEIGHT 7
# define VBE_DISPI_INDEX_X_OFFSET 8
# define VBE_DISPI_INDEX_Y_OFFSET 9

# define VBE_DISPI_DISABLED 0x00
# define VBE_DISPI_ENABLED 0x01

# define VBE_DISPI_LFB_ENABLED 0x40
# define VBE_DISPI_NOCLEARMEM 0x80

# define VBE_DIPSI_ID0 0xB0C0
# define VBE_DIPSI_ID1 0xB0C1
# define VBE_DIPSI_ID2 0xB0C2
# define VBE_DIPSI_ID3 0xB0C3
# define VBE_DIPSI_ID4 0xB0C4
# define VBE_DIPSI_ID5 0xB0C5

# define VBE_BANK_BASE 0xA0000
# define VBE_BANK_SIZE (64 * 1024) /* 64Kb */

struct vbe {
    char *phy_mem;

    char *buffer;

    uint32_t bank;

    struct {
        uint16_t x;
        uint16_t y;
    } cursor;
};

uint16_t bga_read_register(uint16_t index);
int vbe_initialize(struct video *video);

#endif /* !VBE_H */
