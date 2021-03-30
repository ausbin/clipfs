#define FUSE_USE_VERSION 31

#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <fuse.h>
#include "clipfs.h"
#include "xclip.h"

static int _open(const char *path, struct fuse_file_info *fi) {
    (void) fi;

    if (strncmp(path, FULL_PREFIX, sizeof FULL_PREFIX)) {
        return -ENOENT;
    }

    return 0;
}

static int _read(const char *path, char *buf, size_t size, off_t offset,
                 struct fuse_file_info *fi) {
    (void) fi;

    if (strncmp(path, FULL_PREFIX, sizeof FULL_PREFIX)) {
        return -ENOENT;
    }
    if (offset < 0) {
        return -EINVAL;
    }
    size_t off = (size_t) offset;

    struct fuse_context *fctx = fuse_get_context();
    ctx_t *ctx = fctx->private_data;

    unsigned char *sel_buf;
    unsigned long sel_len;
    if (get_clipboard(ctx->dpy, ctx->win, &sel_buf, &sel_len) < 0) {
        return -ENOTRECOVERABLE;
    }

    int ret = 0;
    if (off < sel_len) {
        if (off + size > sel_len) {
            size = sel_len - offset;
        }
        memcpy(buf, sel_buf + off, size);
        ret = size;
    }

    free(sel_buf);

    return ret;
}

static int _opendir(const char *path, struct fuse_file_info *fi) {
    (void) fi;

    if (strcmp(path, ROOT)) {
        return -ENOENT;
    }

    return 0;
}

static int _readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                    off_t offset, struct fuse_file_info *fi,
                    enum fuse_readdir_flags flags) {
    (void) path;
    (void) offset;
    (void) fi;
    (void) flags;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    filler(buf, BASE_PREFIX, NULL, 0, 0);

    return 0;
}

static int _getattr(const char *path, struct stat *stbuf,
                    struct fuse_file_info *fi) {
    (void) fi;

    struct fuse_context *fctx = fuse_get_context();
    ctx_t *ctx = fctx->private_data;

    memset(stbuf, 0, sizeof (struct stat));

    if (!strcmp(path, ROOT)) {
        stbuf->st_mode = S_IFDIR | ROOT_MODE;
        stbuf->st_nlink = 2;
    } else if (!strcmp(path, FULL_PREFIX)) {
        unsigned char *sel_buf;
        unsigned long sel_len;
        if (get_clipboard(ctx->dpy, ctx->win, &sel_buf, &sel_len) < 0) {
            return -ENOTRECOVERABLE;
        }
        // Don't even need this
        free(sel_buf);

        stbuf->st_mode = S_IFREG | FILE_MODE;
        stbuf->st_nlink = 1;
        stbuf->st_size = sel_len;
    } else {
        return -ENOENT;
    }

    return 0;
}

static void show_help(const char *prog) {
    printf("usage: %s [options] <mountpoint>\n"
           "\n"
           "clipfs specific options:\n"
           "    --display=D            X display to use\n"
           "\n", prog);
}

static int setup_ctx(options_t *options, ctx_t *ctx) {
    return get_dpy(options->display_name, &ctx->dpy, &ctx->win);
}

// Much of this is based on the following libfuse example:
// https://github.com/libfuse/libfuse/blob/da474fe7d71f01dcde324107ec398a0678c3ff71/example/hello.c
int main(int argc, char **argv) {
    int ret;
    options_t options;
    memset(&options, 0, sizeof options);
    struct fuse_opt option_spec[] = {
        {"--display=%s", offsetof(options_t, display_name), 1},
        {"-h", offsetof(options_t, show_help), 1},
        {"--help", offsetof(options_t, show_help), 1},
        FUSE_OPT_END
    };

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    if (fuse_opt_parse(&args, &options, option_spec, NULL) < 0) {
        return 1;
    }

    ctx_t ctx;
    memset(&ctx, 0, sizeof ctx);

    if (options.show_help) {
        show_help(argv[0]);
        fuse_opt_add_arg(&args, "--help");
        args.argv[0][0] = '\0';
    } else {
        if (setup_ctx(&options, &ctx) < 0) {
            return 1;
        }
    }

    struct fuse_operations operations = {
        .open = _open,
        .read = _read,
        .opendir = _opendir,
        .readdir = _readdir,
        .getattr = _getattr,
    };

    ret = fuse_main(args.argc, args.argv, &operations, &ctx);
    fuse_opt_free_args(&args);
    return ret;
}
