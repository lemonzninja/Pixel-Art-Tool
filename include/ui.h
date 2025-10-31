#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include "raylib.h"
#include "app_state.h"

void HandlePaletteUI(AppState *state, Vector2 mouse, bool *interactingWithUI);
void HandleToolUI(AppState *state, Vector2 mouse, bool *interactingWithUI);
void DrawUI(const AppState *state);

#endif // UI_H
