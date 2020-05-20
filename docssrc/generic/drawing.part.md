## Drawing

The current version of TG lacks lots of ease-of-use features for drawing, planned for
the future. That being said, you already have the tools you need to do anything you
want, you'll just need to implement your own drawing functionality if you want to
easily do things like lines and rectangles.

As for what is already implemented, it is important to understand the cursor system
in TG. TG has two types of cursors: the actual cursor (provided by the terminal
emulator) and virtual cursors. Each buffer has it's own virtual cursor, which does
not display. A buffer's virtual cursor is where new characters are drawn by default.
Every time you clear a buffer, the virtual cursor for that buffer is set to (0, 0).

The system cursor DOES display (by default), and to control the system cursor, see
the functions `TGSetCursorVisible`, 
`TGSetCursorPosition` and
`TGGetCursorPosition`.

The functions to work with a buffer's virtual cursor are 
`TGBufCursorMove` and `TGBufCursorPosition`.
These functions modify the current position of a buffer's virtual cursor.
If you would like to retrieve the current position of a buffer's virtual cursor,
simply access the member variable `virtualCursorPosition`

Most functions have a wide and "legacy" variant. For example,
`TGTitle` accepts wide characters, and `TGLTitle` accepts a normal `char` pointer

The easiest way to draw is to use the `TGBufAddString` function. This will write
a wide string to a given `TGBuffer`. A faster option is to create a buffer,
write to it, and then `TGBufBlit` it to the context's drawing buffer. This way,
we are rendering the text only once, and after that, we're just copying system
buffers. This cuts out half (or more!) of the operations required to display something
