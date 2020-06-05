#ifndef TG_DRAW_H
#define TG_DRAW_H

#include "tg.h"

typedef struct {
    TGAttributes fillAttrs, strokeAttrs, clearAttrs;
    wchar_t drawChar, clearChar;
    char drawLChar, clearLChar;
    bool legacyMode, strokeDisabled;
    float strokeWidth, cornerRadius;
} TGDrawingContext;

TGDrawingContext TGMakeDrawingContext();

void TGDrawFillPoint(TGBuffer*, TGDrawingContext*, TGPoint);
// TODO: TGDrawStrokePoint which uses stroke settings (width & cornerRadius)
void TGClearRect(TGBuffer*, TGRect);

// TODO: Line, rect, oval, text functions (where performance can be improved over arbitrary shape)
// Clipping
// Arbitrary shapes (polygons)
// Clipping to shape
// Brush functions for each (example): TGDrawBrushPoint
// Transformations & matrix library (hardcoded sizes for perf)

#endif