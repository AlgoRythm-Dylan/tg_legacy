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
