#define FUSE_USE_VERSION 31

#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <fuse.h>
#include "clipfs.h"

static int _open(const char *path, struct fuse_file_info *fi) {
    (void) fi;

    if (strncmp(path, FULL_PREFIX, sizeof FULL_PREFIX)) {
        return -ENOENT;
    }

    return 0;
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

    memset(stbuf, 0, sizeof (struct stat));

    if (!strcmp(path, ROOT)) {
        stbuf->st_mode = S_IFDIR | ROOT_MODE;
        stbuf->st_nlink = 2;
    } else if (!strcmp(path, FULL_PREFIX)) {
        stbuf->st_mode = S_IFREG | FILE_MODE;
        stbuf->st_nlink = 1;
        // TODO: Replace this with clipboard size
        stbuf->st_size = 0;
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

    if (options.show_help) {
        show_help(argv[0]);
        fuse_opt_add_arg(&args, "--help");
        args.argv[0][0] = '\0';
    }

    struct fuse_operations operations = {
        .open = _open,
        .opendir = _opendir,
        .readdir = _readdir,
        .getattr = _getattr,
    };

    ret = fuse_main(args.argc, args.argv, &operations, NULL);
    fuse_opt_free_args(&args);
    return ret;
}
