#ifndef UI_H
#define UI_H

#include "raylib.h"
#include <stdbool.h>

#include "tools.h"
#include "palette.h"

typedef struct UIState {
    bool gridVisible;
    bool colorPickerVisible;
    bool colorPickerForForeground;
    float pickerHue;
    float pickerSaturation;
    float pickerValue;
    float pickerAlpha;
    bool pickerDraggingSV;
    bool pickerDraggingHue;
    bool pickerDraggingAlpha;
    Color pickerLastColor;
    PaletteCollection palettes;
    char paletteFilePath[MAX_PALETTE_PATH_LENGTH];
    char paletteStatusMessage[128];
    int paletteStatusTimer;
} UIState;

void UIInit(UIState *ui);
void UIHandleInput(UIState *ui, ToolState *tools);
void UIDraw(const UIState *ui, const ToolState *tools, float zoom);

#endif // UI_H
