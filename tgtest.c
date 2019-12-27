#include <stdio.h>
#include "tg.h"

int main() {
	TGContext *tg = TG();
	TGCharInfo info;
	info.UnicodeChar = 'X';
	info.AsciiChar = 'X';
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
			if (input.eventType == TG_EVENT_KEY && input.event.keyEvent.key == 'q') running = false;
		}
	}
	TGEnd();
}