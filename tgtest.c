#include "tg.h"
#include <stdio.h>
#include <stdlib.h>

int main(){

	TGContext *context = TG();
	TGColor defaultColor = TGColorCreate(TG_WHITE, TG_BLACK);
	context->drawBuffer.currentAttributes.color = defaultColor.id;
	TGCalculateAttrs(&context->drawBuffer.currentAttributes);

	TGCharInfo info = { 0 };
	info.character = 'A';
	info.attributes.color = defaultColor.id;

	while(true){

		TGBufClear(&context->drawBuffer);
		TGBufAddString(&context->drawBuffer, L"Press 'q' to quit");
		//TGBufCell(&context->drawBuffer, 1, 1, info);
		TGSetCursorPosition(0, 0);
		TGUpdate();

		TGInput input = TGGetInput();
		if(input.empty || input.eventType != TG_EVENT_KEY) continue;
		if(!input.event.keyEvent.special && input.event.keyEvent.key == 'q'){
			break;
		}

	}

	TGEnd();
	return 0;

}