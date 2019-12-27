#include <stdio.h>
#include "tg.h"
#include <unistd.h>

int main(){
    TGContext *tg = TG();
    CHAR_INFO info;
    info.Char.AsciiChar = 'X';
    TGBufCell(&tg->drawBuffer, 0, 0, info);
    TGColor red = TGColorCreate(TG_WHITE, TG_RED);
    TGAttributes attrs = { .color = red.id };
    TGBufAttr(&tg->drawBuffer, 0, 0, attrs);
    TGUpdate();
    TGTitle("D E V R A N T");
    bool running = true;
    while(running){
        TGUpdate();
        TGInput input = TGGetInput();
        if(!input.empty){
            if(input.eventType == TG_EVENT_KEY && input.event.keyEvent.key == 'a') running = false;
        }
    }
    TGEnd();
}