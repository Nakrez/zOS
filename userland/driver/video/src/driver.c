#include <zos/print.h>

#include <driver/driver.h>

#include "driver.h"

static int video_open(struct driver *driver, struct req_open *req,
                      ino_t *inode)
{
    (void)req;

    struct video *video = driver->private;

    if (video->opened)
        return -1;

    *inode = 0;

    return 0;
}

static int video_write(struct driver *driver, struct req_rdwr *req,
                       size_t *size)
{
    struct video *video = driver->private;

    return video->write(video, req, size);
}

static int video_close(struct driver *driver, struct req_close *req)
{
    (void)req;

    struct video *video = driver->private;

    video->opened = 0;

    return 0;
}

static int video_ioctl(struct driver *driver, struct req_ioctl *req,
                       struct resp_ioctl *response)
{
    struct video *video = driver->private;

    response->modify_argp = 0;

    switch (req->request)
    {
        case VIDEO_XRES_GET:
            response->modify_argp = 1;
            response->argp = video->xres;
            break;
        case VIDEO_YRES_GET:
            response->modify_argp = 1;
            response->argp = video->yres;
            break;
        case VIDEO_BPP_GET:
            response->modify_argp = 1;
            response->argp = video->bpp;
            break;

        case VIDEO_XRES_SET:
            if (!req->with_argp)
                return -1;

            video->xres = req->argp;
            video->xres_set(video, req->argp);
            break;
        case VIDEO_YRES_SET:
            if (!req->with_argp)
                return -1;

            video->yres = req->argp;
            video->yres_set(video, req->argp);
            break;
        case VIDEO_BPP_SET:
            if (!req->with_argp)
                return -1;

            video->bpp = req->argp;
            video->bpp_set(video, req->argp);
            break;

        case VIDEO_ENABLE:
            if (!video->xres || !video->yres || !video->bpp)
                return -1;

            video->enable(video);
            break;
        case VIDEO_DISABLE:
            video->disable(video);
            break;
        default:
            return -1;
    }

    return 0;
}

static struct driver_ops video_fops = {
    .open = &video_open,
    .write = &video_write,
    .ioctl = &video_ioctl,
    .close = &video_close,
};

int video_driver_run(struct video *video)
{
    struct driver video_driver;

    if (driver_create("video", 0600, &video_fops, &video_driver) < 0)
    {
        uprint("Video: Fail to spawn video driver");

        return 1;
    }

    video_driver.private = video;

    return driver_loop(&video_driver);
}
