## Building

Building TG is rather simple. For Windows users, a solution
file has been provided. On Linux, you need to install
the ncursesw dev package from your package manager of choice
and you can use gcc for the rest with some compiler arguments:

* `-lncursesw`
* `-ltinfo`
* `-I<path to ncursesw folder, probably in /usr/include>`
* `-D_GNU_SOURCE`

You can use the Makefile as an example, or this example as an
example:

```bash
gcc -o yourprogram tg.c tg.h yourprogram.c -lncursesw -ltinfo -I/usr/include/ncursesw -D_GNU_SOURCE
```

And then, of course, run it with

```bash
sudo chmod +x yourprogram
./yourprogram
```
