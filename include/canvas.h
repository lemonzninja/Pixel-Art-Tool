/**************************************************************************************
*                   - Core Canvas System -                                                    
*  Create a 2D array or dynamic structure to store pixel data (colors)                                                                                 
*  Render the pixel grid using DrawRectangle() or a texture-based approach                                                                     
*  Add grid overlay that can be toggled on/off             
**************************************************************************************/

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
