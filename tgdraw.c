#include "tgdraw.h"

TGDrawingContext TGMakeDrawingContext(){
    TGDrawingContext ctx;
    ctx.fill = (TGCharInfo){ .character = ' ' };
    ctx.stroke = (TGCharInfo){ .character = ' ' };
    ctx.clear = (TGCharInfo){ .character = ' ' };
    ctx.strokeDisabled = false;
    ctx.strokeWidth = 0.0f;
    ctx.cornerRadius = 0.0f;
    return ctx;
}

void TGDrawFillPoint(TGBuffer* buffer, TGDrawingContext* ctx, TGPoint location){
    TGBufCell(buffer, location.X, location.Y, ctx->fill);
}

void TGClearRect(TGBuffer* buffer, TGRect rect){

}