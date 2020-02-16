# TG
*cross-platform advanced terminal control*

## TOC

* [What does TG do?](#what-does-tg-do)
* [Building](#building)
* [Getting Started](#getting-started)
* [Drawing](#drawing)
* [Reference](#reference)

## What does TG do?

TG allows you to use the terminal sort of like a normal graphical window.
If you are familiar with curses, the Linux library, TG does the same thing,
but for both Windows and Linux, and with a simplified flow. In fact,
the Linux version uses ncurses.

You can:

* Double-buffer the screen (for fast drawing)
* Collect input in a standard way
* Draw to the screen
* Use colors

## Building

Building TG is rather simple. On Windows, just add `tg.h` and `tg.c` to your
project. On Linux, do the same thing, and add the linker option `-lncursesw`.
A makefile has been included as an example.

This is a one-line bash script to compile a program with TG (Assuming
you have a program called `yourprogram.c`):

```bash
gcc -o yourprogram tg.c tg.h yourprogram.c -lncursesw
```

And then, of course, run it with

```bash
sudo chmod +x yourprogram
./yourprogram
```

## Getting started

First of all, download and include `tg.c` and `tg.h`. There are two
functions that enter and exit "TG mode" - which is similar to "curses
mode" if you're familar with that.

```c
#include "tg.h"

int main(){

    TG(); // Start TG
    TGEnd(); // Stop TG

    return 0;
}

```

Congratulations, you have started, and immediately stopped, TG.

Now let's create a program that keeps rendering an "X" in the top
right corner until the "q" key is pressed.

```c
#include <stdbool.h>
#include "tg.h"

int main() {
    TGContext *tg = TG();

    TGCharInfo info;
    info.character = 'X';

    TGColor whiteRed = TGColorCreate(TG_WHITE, TG_RED);
    info.attributes.color = whiteRed.id;
    TGCalculateAttrs(&info.attributes);

    TGTitle("termRant");
    TGSetCursorVisible(false);

    bool running = true;
    while (running){
        TGBufCell(&tg->drawBuffer, 0, 0, info);
        TGUpdate();
        TGInput input = TGGetInput();
        if (!input.empty) {
            if (input.eventType == TG_EVENT_KEY && input.event.keyEvent.key == 'q')
                running = false;
        }
    }

    TGEnd();
    return 0;

}
```

Let's run through this

```c
TGContext *tg = TG();
```

`TG` returns a TGContext struct pointer. A TGContext can be helpful because it contains
the main drawing buffer. It's a good idea to keep a reference to this.

```c
TGCharInfo info;
info.character = 'X';
```

Each drawing buffer is made up of these CharInfo objects. They describe the
attributes of the text cells they represent. In other words, a TGCharInfo is a
text cell's content.

```c
TGColor whiteRed = TGColorCreate(TG_WHITE, TG_RED);
info.attributes.color = whiteRed.id;
TGCalculateAttrs(&info.attributes);
```

Because of Linux, colors need to be created in pairs. Each pair of colors (foreground,
background) is unique and given an ID. You should create these pairs only once. In most
Linux terminals, you're limited to just a few pairs, so if you create a new `TGColor`
every time you render a frame, you'll run out pretty quick.

Next, we set the `TGCharInfo` structure's color to the color ID we created earlier.
Lastly, we need to calculate the attributes of the `TGCharInfo`. This is largely
done for cross-platform reasons. Essentially, we're turning a *description* of
attributes into actual system attributes with this call.


```c
TGTitle("termRant");
TGSetCursorVisible(false);
```

Some visual things just to make the experience a little better. Their function should be
obvious.

```c
bool running = true;
while (running){
    TGBufCell(&tg->drawBuffer, 0, 0, info);
    TGUpdate();
    TGInput input = TGGetInput();
    if (!input.empty) {
        if (input.eventType == TG_EVENT_KEY && input.event.keyEvent.key == 'q') running = false;
    }
}
```

This is a simple input loop. Firstly, we set the (0, 0) position of the main draw buffer
to the `TGCharInfo` we created earlier, and then update the screen. Lastly, we read some
input from the user, and if that event is a key press, and they pressed the letter "q",
we stop running the loop.

```c
TGEnd();
return 0;
```

Of course, we need to clean up our mess after we're done. If you forget `TGEnd`, your
console may behave unexpectedly. On Linux, use the `reset` command to fix this.

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
the functions [TGSetCursorVisible](#tgsetcursorvisible), 
[TGSetCursorPosition](#tgsetcursorposition) and
[TGGetCursorPosition](#tggetcursorposition).

The functions to work with a buffer's virtual cursor are 
[TGBufCursorMove](#tgbufcursormove) and [TGBufCursorPosition](#tgbufcursorposition).
These functions modify the curren position of a buffer's virtual cursor.
If you would like to retrieve the current position of a buffer's virtual cursor,
simply access the member variable `virtualCursorPosition`

# Reference

## TOC

Jump to: [Color](#color-items) • [Buffer](#buffer-items) • 
[Character Cell](#character-cells) • [General](#general-items) •
[Input](#input-items)

### Color Items

| Color | Description | Type
|---|---|---|
| [Colors](#colors) | A list of colors | Constants |
| [TGColor](#tgcolor) | Color struct | Struct |
| [TGColorCreate](#tgcolorcreate) | Create a color (pair) | Function |

### Buffer Items

| Buffer | Description | Type 
|---|---|---|
| [TGBufAddLString](#tgbufaddlstring) | Add char* string | Function |
| [TGBufAddLStringAttr](#tgbufaddlstringattr) | Add char* string with certain attributes | Function |
| [TGBufAddString](#tgbufaddstring) | Add wchar_t* string | Function |
| [TGBufAddStringAttr](#tgbufaddstringattr) | Add wchar_t* string with certain attributes | Function |
| [TGBufAttr](#tgbufattr) | Change attributes for a cell | Function |
| [TGBufCell](#tgbufcell) | Change a cell | Function |
| [TGBufClear](#tgbufclear) | Clear a buffer | Function |
| [TGBufCopy](#tgbufcopy) | Copy one buffer to another | Function |
| [TGBufCreate](#tgbufcreate) | Create a new buffer | Function |
| [TGBufCursorMove](#tgbufcursormove) | Move a buffer's virtual cursor | Function |
| [TGBufCursorPosition](#tgbufcursorposition) | Set a buffer's virtual cursor position | Function |
| [TGBuffer](#tgbuffer) | Buffer struct | Struct |
| [TGBufFree](#tgbuffree) | Clean up a buffer | Function |
| [TGBufSize](#tgbufsize) | Resize a buffer | Function |

### Character Cells

| Character Cells | Description | Type |
|---|---|---|
| [TGAttributes](#tgattributes) | Attributes structure | Struct |
| [TGCalculateAttrs](#tgcalculateattrs) | Calculate system attributes | Function |
| [TGCharInfo](#tgcharinfo) | Individual character cell structure | Struct |


### General Items

| General | Description | Type |
|---|---|---|
| [COORD](#coord) | X and Y coordinate structure | Struct |
| [TG](#tg-function) | Init function | Function |
| [TG_VERSION](#tg_version) | Current TG version | Constant |
| [TG_WINDOWS_MODE](#tg_windows_mode) | `true` if running on Windows | Constant |
| [TGContext](#tgcontext) | Mainly used internally | Struct |
| [TGEnd](#tgend) | Exit TG mode | Function |
| [TGSetCursorVisible](#tgsetcursorvisible) | Set system cursor visibility | Function |
| [TGTitle](#tgtitle) | Set terminal title | Function |
| [TGUpdate](#tgupdate) | Update the screen | Function |


### Input Items

| Input | Description | Type |
|---|---|---|
| [TGGetInput](#tggetinput) | Get one input record from the buffer | Function |
| [TGHandleResizeEvent](#tghandleresizeevent) | Make TG system react to resize | Function |
| [TGInput](#tginput) | An input record | Struct |
| [TGKeyEvent](#tgkeyevent) | Fired for key events | Struct |
| [TGMouseEvent](#tgmouseevent) | Fired for mouse events | Struct |
| [TGResizeEvent](#tgresizeevent) | Fired for resize events | Struct |


### *Needs Documentation* (Sorry, it can be boring sometimes!):

* TGBufAddString
* TGBufAddStringAttr

## Colors

TG supports seven cross-platform colors. They are:

```c
TG_BLACK
TG_RED
TG_GREEN
TG_YELLOW
TG_BLUE
TG_MAGENTA
TG_CYAN
TG_WHITE
```

They must be used with [TGCreateColor](#tgcreatecolor)

## COORD

*struct*

COORD is a structure describing a position in two-dimensional space. It comes from
the Windows standard library, and has only X and Y integer values. They are uppercase
for compatibility with Windows.

| Member | Data Type | Description |
|--------|-----------|-------------|
|X       | int       | X coordinate|
|Y       | int       | Y coordinate|

## TG (function)

*function*

TG Starts it all. TG enters "TG mode" and returns a pointer to the main TGContext

| Return Value | Data Type | Description        |
|--------------|-----------|--------------------|
| TGContext    | TGContext*| The main TG Context|

## TG_VERSION

*constant*

The current version of TG you are using

## TG_WINDOWS_MODE

*constant*

Helpful boolean to tell if you're using the Windows version of TG or not.

## TGAttributes

*struct*

A structure to hold attributes, as listed below

| Member     | Data Type    | Description                     |
|------------|--------------|---------------------------------|
| underlined | bool         | Set to true for underlined text |
| bold       | bool         | Set to true for bold text       |
| color      | unsigned int | Color ID                        |

## TGBufAddLString

*function*

Add a "legacy string" - one byte characters - at the current buffer position
(`virtualCursorPosition`) with the current buffer attributes.

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Buffer     | TGBuffer*    | The buffer to draw to        |
| str        | char*        | String to draw               |

## TGBufAddLStringAttr

*function*

Add a "legacy string" - one byte characters - at the current buffer position
(`virtualCursorPosition`) with the attributes passed to the function.

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Buffer     | TGBuffer*    | The buffer to draw to        |
| str        | char*        | String to draw               |
| attributes | TGAttributes | Attributes to use            |

## TGBufAddString

*function*

Add a wide string at the current buffer position
(`virtualCursorPosition`) with the current buffer attributes.

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Buffer     | TGBuffer*    | The buffer to draw to        |
| str        | wchar_t*     | String to draw               |

## TGBufAddStringAttr

*function*

Add wide string at the current buffer position
(`virtualCursorPosition`) with the attributes passed to the function.

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Buffer     | TGBuffer*    | The buffer to draw to        |
| str        | wchar_t*     | String to draw               |
| attributes | TGAttributes | Attributes to use            |

## TGBufAttr

*function*

Set's a buffer's attributes at a specific location

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Buffer     | TGBuffer*    | The buffer to draw to        |
| x          | int          | X position of cell to update |
| y          | int          | Y position of cell to update |
| Attributes | TGAttributes | Attributes to set            |


Returns `void`

## TGBufCell

*function*

Rather than just changing the attributes of a cell, this changes all of the
info contained in a `TGCharInfo`

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Buffer     | TGBuffer*    | The buffer to draw to        |
| x          | int          | X position of cell to update |
| y          | int          | Y position of cell to update |
| CharInfo   | TGCharInfo   | CharInfo to set              |

## TGBufClear

*function*

Clear a buffer and set it's virtual cursor position to (0, 0)

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Buffer     | TGBuffer*    | The buffer to clear          |

## TGBufCopy

*function*

Duplicate a buffer

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Buffer     | TGBuffer*    | The buffer to duplicate      |



| Return Value | Data Type | Description        |
|--------------|-----------|--------------------|
| New Buffer   | TGBuffer  | Deep copied buffer |

## TGBufCreate

*function*

Create a new buffer

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| width      | int          | Width of new buffer          |
| height     | int          | Height of new buffer         |

| Return Value | Data Type | Description        |
|--------------|-----------|--------------------|
| New Buffer   | TGBuffer  | New buffer         |

## TGBufCursorMove

*function*

Move the cursor left by some amount. Negative values acceptable to go
right. Will not loop back around when it reaches the end of the buffer 
(or the beginning).

| Argument   | Data Type    | Description                     |
|------------|--------------|---------------------------------|
| buffer     | TGBuffer*    | The buffer to alter             |
| amount     | int          | How far left to move the cursor |

## TGBufCursorPosition

*function*

Set the virtual cursor position of a buffer

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| buffer     | TGBuffer*    | The buffer to alter          |
| x          | int          | X dimension of position      |
| y          | int          | Y dimension of position      |


## TGBuffer

*struct*

A buffer structure holding `TGCharInfo` types

| Member                | Data Type   | Description                     |
|-----------------------|-------------|---------------------------------|
| size                  | COORD       | Dimensions of the buffer        |
| buffer                | TGCharInfo* | The actual buffer               |
| length                | unsigned int| Size of buffer in TGCharInfo    |
| currentAttributes     | TGAttributes| Used as a default for new chars |
| virtualCursorPosition | COORD       | Used as a default for new chars |

## TGBufFree

*function*

Free resources allocated to a buffer

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Buffer     | TGBuffer*    | The buffer to free           |

## TGBufSize

*function*

Resize a buffer

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Buffer     | TGBuffer*    | The buffer to size           |
| width      | int          | New width of the buffer      |
| height     | int          | New height of the buffer     |

## TGCalculateAttrs

*function*

Called to convert an attribute "description" into actual system attributes. Must
be called any time you modify a `TGAttributes` struct

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Attributes | TGAttributes*| The attributes to calculate  |

## TGCharInfo

*struct*

An abstract description of a text cell, including color, character,
and other attributes

| Member     | Data Type   | Description                     |
|------------|-------------|---------------------------------|
| character  | unsigned int| Text content of the cell        |
| attributes | TGAttributes| Text attributes struct          |

## TGColor

*struct*

Describes a color

| Member     | Data Type      | Description                     |
|------------|----------------|---------------------------------|
| id         | unsigned int   | Unicode text                    |
| foreground | unsigned short | Foreground color ID             |
| background | unsigned short | Background color ID             |

## TGColorCreate

*function*

Create a color. You should **avoid calling this function multiple times for the same color**.
As noted above (in [getting started](#getting-started)), Linux terminals limit 
the amount of pairs you can make. If you make a new pair each time you render a
frame, you'll run out very quickly.

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Foreground | int          | Foreground color ID          |
| Background | int          | Background color ID          |



| Return Value | Data Type | Description        |
|--------------|-----------|--------------------|
| Color        | TGColor   | New color          |

## TGContext

*struct*

In normal operation, there should only be one context. This structure holds
the main drawing buffer and some other vital information

*Note: typedef WINDOW\* HANDLE; on Linux*

| Member             | Data Type   | Description                     |
|--------------------|-------------|---------------------------------|
| screenBufferHandle | HANDLE      | stdscr on Linux, CreateConsoleScreenBuffer on Windows |
| inputHandle        | HANDLE      | Nothing on Linux, STD_INPUT_HANDLE on Windows |
| drawBuffer         | TGBuffer    | The main draw buffer            |

## TGEnd

*function*

Exits TG mode

## TGGetInput

*function*

Reads input and returns it as a `TGInput` record

There are three types of input:

```c
TG_EVENT_KEY
TG_EVENT_MOUSE
TG_EVENT_RESIZE
```

To determine the type of input, you can read the `eventType` attribute. Each
input type has their own input record.

Please note that resize events MUST be sent to 
[TGHandleResizeEvent](#tghandleresizeevent)

* [TGKeyEvent](#tgkeyevent)
* [TGMouseEvent](#tgmouseevent)
* [TGResizeEvent](#tgresizeevent)

| Return Value | Data Type | Description        |
|--------------|-----------|--------------------|
| Input        | TGInput   | User input         |

## TGHandleResizeEvent

*function*

Resize events aren't handled by TG automatically. When you read a resize
event from `TGGetInput`, you must run that same input trough this function

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| Event      | TGInput      | Event to react to            |


## TGInput

*struct*

Holds information about a user interaction with the terminal. For more
information, see [TGGetInput](#tggetinput)

| Member             | Data Type   | Description                     |
|--------------------|-------------|---------------------------------|
| eventType          | short       | Which event type was read       |
| empty              | bool        | Empty records can be ignored    |
| event              | union       | The various event types         |
| -> keyEvent        | TGKeyEvent  | The key event                   |
| -> mouseEvent      | TGKeyEvent  | The mouse event                 |
| -> resizeEvent     | TGKeyEvent  | The resize event                |

## TGKeyEvent

*struct*

Holds information about when a user presses a key

Some keys are considered "special", such as the arrow keys. Here is a list
of "special" keys:

```c
TG_KEY_UP
TG_KEY_RIGHT
TG_KEY_DOWN
TG_KEY_LEFT
TG_KEY_BACKSPACE
TG_KEY_TAB
TG_KEY_ESCAPE
TG_KEY_PAGE_UP
TG_KEY_PAGE_DOWN
TG_KEY_END
TG_KEY_HOME
TG_KEY_INSERT
TG_KEY_DELETE
```

| Member             | Data Type   | Description                           |
|--------------------|-------------|---------------------------------------|
| key                | unisgned int| Which key was pressed                 |
| ctrlDown           | bool        | If ctrl was pressed                   |
| special            | bool        | If the key is one of the special keys |

## TGMouseEvent

*struct*

Holds information about when the user interacts with the mouse

The supported buttons are:

```c
TG_MOUSE_LEFT
TG_MMB // Middle Mouse Button
TG_MOUSE_RIGHT
```

And the various actions are:

```c
TG_MOUSE_CLICK
TG_MOUSE_MOVE
```

| Member             | Data Type      | Description                     |
|--------------------|----------------|---------------------------------|
| position           | COORD          | Where the mouse event occurred  |
| button             | unsigned short | Which button was pressed        |
| action             | unsigned short | What the user did               |

## TGResizeEvent

*struct*

Holds information about when the user resizes the terminal

**NOTE: IF YOU READ THIS EVENT TYPE, IT MUST BE SENT TO TGHandleResizeEvent**

| Member             | Data Type   | Description                     |
|--------------------|-------------|---------------------------------|
| oldSize            | COORD       | The old size of the terminal    |
| newSize            | COORD       | The new size of the terminal    |

## TGSetCursorVisible

*function*

Sets the cursor to be either visible or invisible. Doesn't seem to be
permanent on all platforms, so call this once per render to be sure.

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| visible    | bool         | Visible if value is truthy   |

## TGTitle

*function*

Set the title of the console.

| Argument   | Data Type    | Description                  |
|------------|--------------|------------------------------|
| title      | char*        | The new title of the console |

## TGUpdate

*function*

Update the screen. No drawing will take visual effect until this
is called.
