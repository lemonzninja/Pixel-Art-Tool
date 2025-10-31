#ifndef UI_H
#define UI_H

#include "raylib.h"
#include <stdbool.h>

#include "tools.h"

typedef struct UIState {
    bool gridVisible;
} UIState;

void UIInit(UIState *ui);
void UIHandleInput(UIState *ui);
void UIDraw(const UIState *ui, const ToolState *tools, float zoom);

#endif // UI_H
