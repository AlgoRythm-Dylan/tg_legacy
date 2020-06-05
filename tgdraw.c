#include "tgdraw.h"

TGDrawingContext TGMakeDrawingContext(){
    TGDrawingContext ctx;
    ctx.fillAttrs = (TGAttributes){ 0 };
    ctx.strokeAttrs = (TGAttributes){ 0 };
    ctx.drawChar = L' ';
    ctx.drawLChar = ' ';
    ctx.clearChar = L' ';
    ctx.clearLChar = ' ';
    ctx.legacyMode = false;
    ctx.strokeDisabled = false;
    ctx.strokeWidth = 0.0f;
    ctx.cornerRadius = 0.0f;
    return ctx;
}

void TGDrawFillPoint(TGBuffer* buffer, TGDrawingContext* ctx, TGPoint location){

}

void TGClearRect(TGBuffer* buffer, TGRect rect){

}