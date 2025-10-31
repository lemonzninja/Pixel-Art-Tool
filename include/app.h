#ifndef APP_H
#define APP_H

#include "raylib.h"
#include <stdbool.h>

#include "canvas.h"
#include "tools.h"
#include "ui.h"

typedef struct App {
    Canvas canvas;
    ToolState tools;
    UIState ui;
    Camera2D camera;
    float minZoom;
    float maxZoom;
} App;

bool AppInit(App *app, int screenWidth, int screenHeight);
void AppShutdown(App *app);
void AppUpdate(App *app);
void AppDraw(const App *app);

#endif // APP_H
