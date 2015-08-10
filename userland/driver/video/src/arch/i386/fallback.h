#ifndef FALLBACK_H
# define FALLBACK_H

# define FALLBACK_ADDR 0xA0000

# define FALLBACK_XRES 80
# define FALLBACK_YRES 25
# define FALLBACK_BPP 2

# define FALLBACK_SIZE (FALLBACK_XRES * FALLBACK_YRES * FALLBACK_BPP)
# define FALLBACK_AREA_SIZE (64 * 4096)
# define FALLBACK_DATA_OFF (0xB8000 - FALLBACK_ADDR)

struct fallback {
    void *phy_area;
    void *phy_data;

    struct {
        int x;
        int y;
    } cursor;
};

int fallback_initialize(struct video *video);

#endif /* !FALLBACK_H */
