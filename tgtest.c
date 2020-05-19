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
		TGBufAddString(&context->drawBuffer, str);

		TGInput input = TGGetInput();

		if (!(input.empty || input.eventType != TG_EVENT_KEY) && !input.event.keyEvent.special){
			if(input.event.keyEvent.key == L'q'){
				running = false;
			}
		}

		TGUpdate();
	}
	TGEnd();
	return 0;
}