#include "tg.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

	TGContext *context = TG();
	/*TGColor defaultColor = TGColorCreate(TG_WHITE, TG_BLACK);
	context->drawBuffer.currentAttributes.color = defaultColor.id;
	TGCalculateAttrs(&context->drawBuffer.currentAttributes);*/
	
	wchar_t *str = L"😀";
	TGSetCursorVisible(false);

	TGTitle(L"TG Test 😀");

	while (true) {

		TGBufClear(&context->drawBuffer);
		TGBufAddString(&context->drawBuffer, str);

		TGInput input = TGGetInput();
		if (input.empty || input.eventType != TG_EVENT_KEY) continue;
		if (!input.event.keyEvent.special && input.event.keyEvent.key == 'q') {
			break;
		}

		TGUpdate();

	}
	TGEnd();

	return 0;
}