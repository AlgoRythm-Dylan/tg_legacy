#include <stdio.h>
#include "tg.h"

int main() {
	TGContext *tg = TG();
	CHAR_INFO info;
	info.Char.AsciiChar = 'X';
	TGBufCell(&tg->drawBuffer, 0, 0, info);
	TGColor red = TGColorCreate(TG_WHITE, TG_RED);
	TGAttributes attrs = { .color = red.id };
	TGBufAttr(&tg->drawBuffer, 0, 0, attrs);
	TGUpdate();
	TGTitle("termRant");
	TGSetCursorVisible(false);
	bool running = true;
	while (running) {
		TGUpdate();
		TGInput input = TGGetInput();
		if (!input.empty) {
			if (input.eventType == TG_EVENT_MOUSE/* && input.event.mouseEvent.button == TG_MOUSE_LEFT*/) running = false;
		}
	}
	TGEnd();
}