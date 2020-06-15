<!-- Generated 2020-06-14 19:05:03.068347 -->
# TG
*The Terminal Graphics library, which provides*
*cross-platform, standardized advanced terminal control*
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

Now let's create a program that keeps rendering some complex text
until the 'q' button is pressed. The string "Hey Terminal! 😀😀😀😀"
includes emojis, which your terminal is likely not to support.
If this is the case, you can simply remove them from the example.

```c
#include "tg.h"
#include "tgsys.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

	// Start TG and store a reference to the TG Context
    TGContext *context = TG();
	
	// Create a wide string and make the cursor invisible
	wchar_t *str = L"Hey Terminal! 😀😀😀😀";
	TGSetCursorVisible(false);

	// Set the console title
	TGTitle(L"TG Test 😀");

	// Main graphics loop
	bool running = true;
	while (running) {

		// Clear the screen, ready for a new render!
		TGBufClear(&context->drawBuffer);

		// Main input loop
		TGInput input;
		do {
			input = TGGetInput();
			if (input.empty) continue; // Because of Windows, some inputs may be empty
			if (input.eventType == TG_EVENT_KEY) {
				if (!input.event.keyEvent.special) {
					if (input.event.keyEvent.key == 'q') {
						running = false; // Exit graphics loop on 'q' press
					}
				}
			}
		} while (!input.last);

		// Display a wide string
		TGBufAddString(&context->drawBuffer, str);

		// Show all changes
		TGUpdate();
	}

	// Cleanup
	TGEnd();
	return 0;

}
```

Some info that you may need to know:

Because of Linux, colors need to be created in pairs. Each pair of colors (foreground,
background) is unique and given an ID. You should create these pairs only once. In many
Linux terminals, you're limited to just a few pairs, so if you create a new `TGColor`
every time you render a frame, you'll run out quickly.

Of course, we need to clean up our mess after we're done. If you forget `TGEnd`, your
console may behave unexpectedly. On Linux, use the `reset` command to fix this, and it
may need a `clear` too!
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
# Reference

## TOC

Jump to: [Color](#color-items) • [Buffer](#buffer-items) • 
[Character Cell](#character-cells) • [General](#general-items) •
[Input](#input-items)
## Color Items
|Item|Description|Type|
|---|---|---|
|[Colors](#colors)|A list of colors|Constants|
|[TGColor](#tgcolor)|The color structure|Struct|
|[TGColorCreate](#tgcolorcreate)|Create a color (pair)|Function|
|[TGDefaultColor](#tgdefaultcolor)|The terminal's default color|Extern|
|[TG_COLOR_NAMES](#tg_color_names)|A list of color names|Constant|
## Buffer Items
|Item|Description|Type|
|---|---|---|
|[TGBufAddLString](#tgbufaddlstring)|Add char* string|Function|
|[TGBufAddLStringAttr](#tgbufaddlstringattr)|Add char* string with certain attributes|Function|
|[TGBufAddString](#tgbufaddstring)|Add wchar_t* string|Function|
|[TGBufAddStringAttr](#tgbufaddstringattr)|Add wchar_t* string with certain attributes|Function|
|[TGBufAttr](#tgbufattr)|Change attributes for a cell|Function|
|[TGBufCell](#tgbufcell)|Set a cell's content|Function|
|[TGBufClear](#tgbufclear)|Clear a buffer|Function|
|[TGBufCreate](#tgbufcreate)|Create a new buffer|Function|
|[TGBufCursorMove](#tgbufcursormove)|Move a buffer's virtual cursor|Function|
|[TGBufCursorPosition](#tgbufcursorposition)|Set a buffer's virtual cursor position|Function|
|[TGBufFree](#tgbuffree)|Clean up a buffer|Function|
|[TGBufSize](#tgbufsize)|Resize a buffer|Function|
|[TGBuffer](#tgbuffer)|Buffer struct|Struct|
## Character Cells
|Item|Description|Type|
|---|---|---|
|[TGAttributes](#tgattributes)|Attributes structure|Struct|
|[TGCalculateAttrs](#tgcalculateattrs)|Calculate system attributes|Function|
|[TGCharInfo](#tgcharinfo)|Individual character cell structure|Struct|
## General Items
|Item|Description|Type|
|---|---|---|
|[COORD](#coord)|X and Y coordinate structure|Struct|
|[TG](#tg)|Init function|Function|
|[TGVersion](#tgversion)|Current TG version|Constant|
## All reference items (Alphabetical)
## COORD
*Struct*

COORD is a structure describing a position in two-dimensional space. It comes from the Windows standard library, and has only X and Y integer values. They are uppercase for compatibility with Windows.
|Member|Data Type|Description|
|---|---|---|
|X|int|X coordinate|
|Y|int|Y coordinate|
## Colors
*Constants*

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

They must be used with `TGCreateColor`
## TG
*Function*

TG Starts it all. TG enters "TG mode" and returns a pointer to the main TGContext
**Return value**: The main `TGContext`

## TGAttributes
*Struct*

A structure to hold attributes, as listed below
|Member|Data Type|Description|
|---|---|---|
|underlined|bool|Text will be underlined if `true`|
|bold|bool|Text will be bold if `true`|
|color|unsigned int|TG Color ID to be used|
## TGBufAddLString
*Function*

Add a "legacy string" - one byte characters - at the current buffer position (`virtualCursorPosition`) with the current buffer attributes.
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Buffer|TGBuffer*|The buffer to draw to|
|str|char*|String to draw|
## TGBufAddLStringAttr
*Function*

Add a "legacy string" - one byte characters - at  the current buffer position (`virtualCursorPosition`) with the attributes passed to the function.
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Buffer|TGBuffer*|The buffer to draw to|
|str|char*|String to draw|
|attributes|TGAttributes|Attributes to use|
## TGBufAddString
*Function*

Add a wide string at the current buffer position (virtualCursorPosition) with the current buffer attributes.
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Buffer|TGBuffer*|The buffer to draw to|
|str|wchar_t*|String to draw|
## TGBufAddStringAttr
*Function*

Add wide string at the current buffer position (virtualCursorPosition) with the attributes passed to the function.
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Buffer|TGBuffer*|The buffer to draw to|
|str|wchar_t*|String to draw|
|attributes|TGAttributes|Attributes to use|
## TGBufAttr
*Function*

Set’s a buffer's attributes at a specific location
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Buffer|TGBuffer*|The buffer to draw to|
|x|int|X position of the cell to update|
|y|int|Y position of the cell to update|
|Attributes|TGAttribtues|Attributes to set|
## TGBufCell
*Function*

Set a cell's content
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Buffer|TGBuffer*|The buffer to draw to|
|x|int|X position of the cell to update|
|y|int|Y position of the cell to update|
|CharInfo|TGCharInfo|CharInfo to set|
## TGBufClear
*Function*

Clear a buffer and set it's virtual cursor position to (0, 0)
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Buffer|TGBuffer*|The buffer to clear|
## TGBufCreate
*Function*

Create a new buffer
|Usage|Item|Data Type|Description|
|---|---|---|---|
|width|int|Width of new buffer|
|height|int|Height of new buffer|
**Return value**: A new `TGBuffer` struct

## TGBufCursorMove
*Function*

Move the cursor right by some amount. Negative values acceptable to go left. Will not loop back around when it reaches the end of the buffer (or the beginning).
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Buffer|TGBuffer*|The buffer to alter|
|amount|int|How many cells to move the cursor|
## TGBufCursorPosition
*Function*

Set the virtual cursor position of a buffer
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Buffer|TGBuffer*|The buffer to alter|
|x|int|X dimension of position|
|y|int|Y dimension of position|
## TGBufFree
*Function*

Free resources allocated to a buffer
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Buffer|TGBuffer*|The buffer to free|
## TGBufSize
*Function*

Resize a buffer
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Buffer|TGBuffer*|The buffer to resize|
|width|int|New width of the buffer|
|height|int|New height of the buffer|
## TGBuffer
*Struct*

A buffer structure holding `TGCharInfo` types
|Member|Data Type|Description|
|---|---|---|
|size|COORD|Dimensions of the buffer|
|buffer|TGCharInfo*|Buffer data|
|length|int|Buffer size|
|currentAttributes|TGAttributes|Default attributes to be used|
|virtualCursorPosition|COORD|Where new characters will be added|
## TGCalculateAttrs
*Function*

Called to convert an attribute "description" into  actual system attributes. Must be called any time you modify a TGAttributes struct
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Attribtues|TGAttribtues*|The attributes to calculate|
## TGCharInfo
*Struct*

An abstract description of a text cell, including color, character, and other attributes
|Member|Data Type|Description|
|---|---|---|
|character|unsigned int|text content of the cell|
|attributes|TGAttributes|Text attributes struct|
## TGColor
*Struct*

Describes a color
|Member|Data Type|Description|
|---|---|---|
|id|unsigned int|Color ID|
|Foreground|unsigned short|Foreground color ID|
|Background|unsigned short|Backgorund color ID|
## TGColorCreate
*Function*

Create a color for a cell. Every color is described as a pair of colors.
You should avoid calling this function multiple
times for the same color. Linux terminals limit the amount of
pairs you can make. If you make a new pair each time you render
a frame, you’ll run out very quickly.
|Usage|Item|Data Type|Description|
|---|---|---|---|
|Foreground|int|Foreground color ID|
|Background|int|Background color ID|
**Return Value**: `TGColor` struct

## TGDefaultColor
*Extern*

A TGColor structure describing the console's default colors
## TGVersion
*Constant*

The current version of TG you are using
## TG_COLOR_NAMES
*Constant*

Can be used to convert a standard color to their English names (in all caps). In order, they are:

```c
"BLACK"
"RED"
"GREEN"
"YELLOW"
"BLUE"
"MAGENTA"
"CYAN"
"WHITE"
```

Therefore, `TG_COLOR_NAMES[TG_BLACK]` is `"BLACK"`
