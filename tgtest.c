#include <stdio.h>
#include "tg.h"

int main() {
	TGContext *tg = TG();
	tg->drawBuffer.currentAttributes.color = TGColorCreate(TG_WHITE, TG_RED).id;
	TGCalculateAttrs(&tg->drawBuffer.currentAttributes);
	TGTitle("termRant");
	TGAttributes otherAttributes;
	otherAttributes.color = TGColorCreate(TG_BLACK, TG_GREEN).id;
	TGCalculateAttrs(&otherAttributes);
	bool running = true;
	while (running) {
		TGSetCursorVisible(false);
		TGBufClear(&tg->drawBuffer);
		TGBufAddLString(&tg->drawBuffer, "This is happening!");
		TGBufAddLStringAttr(&tg->drawBuffer, "WOO NEW COLOR", otherAttributes);
		TGBufAddLString(&tg->drawBuffer, "Back to the other color");
		TGUpdate();
		TGInput input = TGGetInput();
		if (!input.empty && input.eventType == TG_EVENT_KEY) {
			if (input.event.keyEvent.special && input.event.keyEvent.key == TG_KEY_DOWN)
				running = false;
		}
		else if (input.eventType == TG_EVENT_RESIZE) {
			TGHandleResizeEvent(input);
		}
	}
	TGEnd();
}