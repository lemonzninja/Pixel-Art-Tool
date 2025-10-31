#ifndef CANVAS_H
#define CANVAS_H

#include <stdbool.h>

#include "raylib.h"
#include "app_state.h"

void InitCanvas(AppState *state);
void ShutdownCanvas(AppState *state);
void HandleCanvasInput(AppState *state, Vector2 mouse, bool *pixelModified, bool interactingWithUI);
void DrawCanvas(const AppState *state, Vector2 mouse);
Rectangle GetCanvasScreenRect(void);

#endif // CANVAS_H
