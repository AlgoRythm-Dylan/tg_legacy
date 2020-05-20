#include "tg.h"
#include "tgsys.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

	TGContext *context = TG();
	
	wchar_t *str = L"Hello world";
	TGSetCursorVisible(false);

	TGTitle(L"TG Test 😀");

	TGColor yellowForeground = TGColorCreate(TG_GREEN, TGDefaultColor.background);

	TGBuffer textBuf = TGBufCreate(15, 1);
	textBuf.currentAttributes.color = yellowForeground.id;
	TGCalculateAttrs(&textBuf.currentAttributes);
	TGBufClear(&textBuf);
	TGBufAddString(&textBuf, str);

	bool running = true;
	while (running) {

		TGBufClear(&context->drawBuffer);
		TGBufBlit(&textBuf, &context->drawBuffer, (TGPoint) {5, 5});
		//TGBufAddString(&context->drawBuffer, str);

		TGInput input;

		do {
			input = TGGetInput();
			if (input.empty) continue;
			if (input.eventType == TG_EVENT_KEY) {
				if (!input.event.keyEvent.special) {
					if (input.event.keyEvent.key == 'q') {
						running = false;
					}
				}
			}
		} while (!input.last);

		TGUpdate();
	}
	TGEnd();
	return 0;
}