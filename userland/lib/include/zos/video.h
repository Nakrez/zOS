#ifndef ZOS_VIDEO_H
# define ZOS_VIDEO_H

# include <stdint.h>

# include <driver/driver.h>

# define VIDEO_XRES_GET 1
# define VIDEO_YRES_GET 2
# define VIDEO_BPP_GET 3

# define VIDEO_XRES_SET 4
# define VIDEO_YRES_SET 5
# define VIDEO_BPP_SET 6

# define VIDEO_ENABLE 7
# define VIDEO_DISABLE 8

struct video {
    uint16_t xres;
    uint16_t yres;
    uint8_t bpp;

    void *private;

    int (*read)(struct video *, struct req_rdwr *, size_t *);
    int (*write)(struct video *, int, struct req_rdwr *, size_t *);

    int (*xres_set)(struct video *, int);
    int (*yres_set)(struct video *, int);
    int (*bpp_set)(struct video *, int);

    int (*enable)(struct video *);
    int (*disable)(struct video *);
};

#endif /* !ZOS_VIDEO_H */
