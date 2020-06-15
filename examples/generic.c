#include "../tg.h"
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

	// Create a yellow and <default background> color pair
	TGColor yellowForeground = TGColorCreate(TG_YELLOW, TGDefaultColor.background);

	// Create a buffer and render the wide string to it.
	// This way, we can take the rendered buffer and directly output it.
	// This should be faster in theory.
	TGBuffer textBuf = TGBufCreate(25, 1);
	textBuf.currentAttributes.color = yellowForeground.id;
	TGCalculateAttrs(&textBuf.currentAttributes); // After applying color, compile the attrs
	TGBufClear(&textBuf); // Buffers are not cleared when they are created
	TGBufAddString(&textBuf, str); // Actually render the string to the buffer

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

		// Display string from pre-rendered buffer
		TGBufBlit(&textBuf, &context->drawBuffer, (TGPoint) {5, 5});

		// Show all changes
		TGUpdate();
	}

	// Cleanup
	TGEnd();
	return 0;
}