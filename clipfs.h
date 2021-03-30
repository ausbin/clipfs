#ifndef CLIPFS_H
#define CLIPFS_H

#define ROOT "/"
#define BASE_PREFIX "clipboard"
#define FULL_PREFIX (ROOT BASE_PREFIX)

#define ROOT_MODE 0700
#define FILE_MODE 0600

typedef struct {
    const char *display_name;
    int show_help;
} options_t;

#endif
