clipfs (clipboard filesystem)
=============================

This repository holds my ongoing work for a [FUSE][1] filesystem that
provides access to the X11 clipboard. Often I find myself wanting to
interact with my clipboard like a file (for example, to export an image
in GIMP directly to the clipboard, or exporting some BibTeX directly
from Zotero to the clipboard), so my goal here is to facilitate that.

It turns out that the X11 clipboard is pretty dang complicated. Even
Martin Flöser, a master of display servers, [called][2] dealing with the
X11 clipboard "quite complex" and "nothing I'm looking forward to." So I
have ripped the clipboard management code out of [xclip][3], a
command-line X11 clipboard access tool.

The code is GPL2 since the [libfuse "hello world" example][4] and
[xclip][3], both of which I've liberally copypasted from, are GPL2.

getting started
---------------

To mount the pseudo-filesystem:

    $ make
    $ mkdir ~/clipboard
    $ ./clipfs -f ~/clipboard

(`-f` tells libfuse to run in the foreground instead of forking)

Now you can mess with `~/clipboard/clipboard` in another terminal:

    $ cat ~/clipboard/clipboard
    I'd just like to interject for a moment.

Writing to the clipboard is still a WIP, as is dealing with anything
except UTF-8 text.

[1]: https://www.kernel.org/doc/html/latest/filesystems/fuse.html
[2]: https://blog.martin-graesslin.com/blog/2016/07/synchronizing-the-x11-and-wayland-clipboard/
[3]: https://github.com/astrand/xclip/
[4]: https://github.com/libfuse/libfuse/blob/da474fe7d71f01dcde324107ec398a0678c3ff71/example/hello.c
