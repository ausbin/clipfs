#ifndef XCLIP_H
#define XCLIP_H

extern int get_dpy(char *, Display **, Window *);
extern void close_dpy(Display *);
extern int get_clipboard(Display *, Window, unsigned char **, unsigned long *);

#endif
