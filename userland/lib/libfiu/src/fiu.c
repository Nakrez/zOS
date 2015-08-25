#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <zos/device.h>
#include <zos/print.h>
#include <zos/vfs.h>

#include <sys/mount.h>

#include <fiu/fiu.h>

#define BUF_SIZE 256

static int fiu_capabilities(struct fiu_fs *fs)
{
    /* Lookup/Create/Open/Close are mandatory operations */
    if (!fs->ops->lookup || !fs->super_ops->create || !fs->ops->open ||
        !fs->ops->close)
        return -1;

    fs->cap = VFS_OPS_LOOKUP | VFS_OPS_FS_CREATE | VFS_OPS_OPEN |
              VFS_OPS_CLOSE;

    if (fs->ops->stat)
        fs->cap |= VFS_OPS_STAT;

    if (fs->ops->mount)
        fs->cap |= VFS_OPS_MOUNT;

    if (fs->ops->read)
        fs->cap |= VFS_OPS_READ;

    if (fs->ops->getdirent)
        fs->cap |= VFS_OPS_GETDIRENT;

    return 0;
}

void fiu_help(const char *bin_name, FILE *output)
{
    fprintf(output, "%s [OPTS]", bin_name);
    fprintf(output, "The supported options are :\n");
    fprintf(output, "  -h / --help   : Print this help\n");
    fprintf(output, "  -d / --daemon : Daemonize the master file system\n");
}

static void fiu_dispatch(struct fiu_instance *fi, void *buf)
{
    struct msg_header *hdr = buf;

    switch (hdr->op) {
        case VFS_LOOKUP:
            {
                struct resp_lookup resp;

                resp.ret = fi->parent->ops->lookup(fi, (void *)buf, &resp);

                resp.hdr.slave_id = hdr->slave_id;

                write(fi->channel_fd, &resp, sizeof (resp));
            }
            break;
        case VFS_STAT:
            {
                struct resp_stat resp;

                resp.ret = fi->parent->ops->stat(fi, (void *)buf, &resp.stat);

                resp.hdr.slave_id = hdr->slave_id;

                write(fi->channel_fd, &resp, sizeof (resp));
            }
            break;
        case VFS_MOUNT:
            {
                struct resp_mount resp;

                resp.ret = fi->parent->ops->mount(fi, (void *)buf);

                resp.hdr.slave_id = hdr->slave_id;

                write(fi->channel_fd, &resp, sizeof (resp));
            }
            break;
        case VFS_OPEN:
            {
                struct resp_open resp;

                resp.ret = fi->parent->ops->open(fi, (void *)buf, &resp);

                resp.hdr.slave_id = hdr->slave_id;

                write(fi->channel_fd, &resp, sizeof (resp));
            }
            break;
        case VFS_READ:
            {
                struct resp_rdwr resp;

                resp.ret = fi->parent->ops->read(fi, (void *)buf, &resp.size);

                resp.hdr.slave_id = hdr->slave_id;

                write(fi->channel_fd, &resp, sizeof (resp));
            }
            break;
        case VFS_GETDIRENT:
            {
                struct resp_getdirent resp;

                resp.ret = fi->parent->ops->getdirent(fi, (void *)buf,
                                                      &resp.dirent);

                resp.hdr.slave_id = hdr->slave_id;

                write(fi->channel_fd, &resp, sizeof (resp));
            }
            break;
        case VFS_CLOSE:
            {
                struct resp_close resp;

                resp.ret = fi->parent->ops->close(fi, (void *)buf);

                resp.hdr.slave_id = hdr->slave_id;

                write(fi->channel_fd, &resp, sizeof (resp));
            }
            break;
        default:
            {
                char tmp[100];

                sprintf(tmp, "fiu_dispatch(): Unsupported operation (%i, %i)",
                        hdr->op, hdr->slave_id);

                uprint(tmp);
            }
            break;
    }
}

static int fiu_slave_loop(struct fiu_instance *fi)
{
    int ret;
    char *buf;

    buf = malloc(BUF_SIZE);
    if (!buf)
        return -1;

    for (;;) {
        ret = read(fi->channel_fd, buf, BUF_SIZE);
        if (ret < 0) {
            uprint("FIU: Slave loop: Read error");
            continue;
        }

        fiu_dispatch(fi, buf);
    }

    exit(4);
}

int fiu_slave_main(struct fiu_instance *fi, const char *device,
                   uint16_t slave_id)
{
    int ret;
    struct resp_fs_create resp;

    fi->device_fd = open_device(device, 0, 0);
    if (fi->device_fd < 0)
        return fi->device_fd;

    ret = fork();
    if (ret < 0)
        return ret;

    if (ret > 0)
        return 0;

    fi->channel_fd = channel_create("ext2-1");

    strcpy(resp.device, "ext2-1");
    resp.hdr.slave_id = slave_id;
    resp.ret = 0;

    if (fi->channel_fd < 0)
        resp.ret = fi->channel_fd;

    ret = write(fi->parent->channel_fd, &resp, sizeof (struct resp_fs_create));
    if (ret < 0 || fi->channel_fd < 0)
        exit(1);

    ret = fi->parent->ops->init(fi);
    if (ret < 0) {
        uprint("TEST");
        exit(1);
    }

    uprint("Fiu: Slave loop");

    return fiu_slave_loop(fi);
}

static int fiu_master_loop(struct fiu_fs *fs)
{
    int ret;
    char *buf;

    buf = malloc(BUF_SIZE);
    if (!buf)
        return -1;

    for (;;) {
        struct msg_header *hdr;

        ret = read(fs->channel_fd, buf, BUF_SIZE);
        if (ret < 0) {
            uprint("FIU: Master loop: Read error");
            continue;
        }

        hdr = (struct msg_header *)buf;
        if (hdr->op != VFS_FS_CREATE) {
            uprint("Only accepting VFS_FS_CREATE ops");
            continue;
        }

        fs->super_ops->create(fs, (void *)buf);

        uprint("FIU: Master loop: File system instance created");
    }
}

int fiu_master_main(struct fiu_fs *fs, int argc, char *argv[])
{
    int ret;
    struct fiu_opts opts;

    memset(&opts, 0, sizeof (opts));

    ret = fiu_parse_opts(argc, argv, &opts);
    if (ret < 0) {
        fiu_help(argv[0], stderr);
        return ret;
    }

    if (opts.help) {
        fiu_help(argv[0], stdout);
        return 0;
    }

    if (opts.daemon) {
        pid_t pid;

        pid = fork();
        if (pid < 0) {
            uprint("fiu_master_main: Fail to fork()");
            return pid;
        }

        if (pid)
            return 0;
    }

    ret = fiu_capabilities(fs);
    if (ret < 0)
        return ret;

    ret = channel_create(fs->name);
    if (ret < 0) {
        uprint("FIU: Fail to create channel");
        return ret;
    }

    ret = fs_register(fs->name, fs->channel_fd, fs->cap);
    if (ret < 0) {
        uprint("FIU: Fail to register fs");
        return ret;
    }

    return fiu_master_loop(fs);
}
