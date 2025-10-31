#include "app.h"

#include "raymath.h"

#include <math.h>

static void UpdateCamera(App *app)
{
    if (!app) return;

    Camera2D *camera = &app->camera;

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
    {
        Vector2 delta = GetMouseDelta();
        camera->target.x -= delta.x / camera->zoom;
        camera->target.y -= delta.y / camera->zoom;
    }

    Vector2 keyboardMove = { 0.0f, 0.0f };
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) keyboardMove.x += 1.0f;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) keyboardMove.x -= 1.0f;
    if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) keyboardMove.y -= 1.0f;
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) keyboardMove.y += 1.0f;

    if (keyboardMove.x != 0.0f || keyboardMove.y != 0.0f)
    {
        const float moveSpeed = 10.0f;
        camera->target.x += keyboardMove.x * moveSpeed / camera->zoom;
        camera->target.y += keyboardMove.y * moveSpeed / camera->zoom;
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f)
    {
        Vector2 mouseWorldBefore = GetScreenToWorld2D(GetMousePosition(), *camera);
        float newZoom = camera->zoom + wheel * camera->zoom * 0.1f;
        if (newZoom < app->minZoom) newZoom = app->minZoom;
        if (newZoom > app->maxZoom) newZoom = app->maxZoom;

        if (fabsf(newZoom - camera->zoom) > 0.0001f)
        {
            camera->zoom = newZoom;
            Vector2 mouseWorldAfter = GetScreenToWorld2D(GetMousePosition(), *camera);
            Vector2 delta = Vector2Subtract(mouseWorldBefore, mouseWorldAfter);
            camera->target = Vector2Add(camera->target, delta);
        }
    }
}

bool AppInit(App *app, int screenWidth, int screenHeight)
{
    if (!app) return false;

    SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(screenWidth, screenHeight, "Pixel Art Tool");
    SetTargetFPS(60);

    ToolsInit(&app->tools);
    UIInit(&app->ui);

    const int canvasWidth = 64;
    const int canvasHeight = 64;
    const int pixelSize = 12;
    if (!CanvasInit(&app->canvas, canvasWidth, canvasHeight, pixelSize, BLANK, (Color){ 30, 30, 30, 255 }))
    {
        CloseWindow();
        return false;
    }

    app->camera = (Camera2D){ 0 };
    app->camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    app->camera.target = (Vector2){ CanvasGetWidthPixels(&app->canvas) / 2.0f, CanvasGetHeightPixels(&app->canvas) / 2.0f };
    app->camera.rotation = 0.0f;
    app->camera.zoom = 1.0f;

    app->minZoom = 0.25f;
    app->maxZoom = 8.0f;

    return true;
}

void AppShutdown(App *app)
{
    if (!app) return;
    CanvasUnload(&app->canvas);
    CloseWindow();
}

void AppUpdate(App *app)
{
    if (!app) return;

    UpdateCamera(app);
    ToolsHandleInput(&app->tools);
    UIHandleInput(&app->ui, &app->tools);

    if (IsKeyPressed(KEY_C))
    {
        CanvasClear(&app->canvas, BLANK);
    }

    ToolsApplyToCanvas(&app->tools, &app->camera, &app->canvas);
}

void AppDraw(const App *app)
{
    if (!app) return;

    BeginDrawing();
    ClearBackground((Color){ 20, 20, 24, 255 });

    BeginMode2D(app->camera);
    CanvasDraw(&app->canvas);
    if (app->ui.gridVisible)
    {
        float thickness = 1.0f / app->camera.zoom;
        if (thickness < 0.25f) thickness = 0.25f;
        CanvasDrawGrid(&app->canvas, Fade(LIGHTGRAY, 0.6f), thickness);
    }

    const float borderWidth = CanvasGetWidthPixels(&app->canvas);
    const float borderHeight = CanvasGetHeightPixels(&app->canvas);
    if (borderWidth > 0.0f && borderHeight > 0.0f)
    {
        float outlineThickness = 2.0f / app->camera.zoom;
        if (outlineThickness < 0.5f) outlineThickness = 0.5f;
        Rectangle borderRect = { 0.0f, 0.0f, borderWidth, borderHeight };
        DrawRectangleLinesEx(borderRect, outlineThickness, Fade(RAYWHITE, 0.6f));
    }

    Vector2 mousePosition = GetMousePosition();
    Vector2 worldPosition = GetScreenToWorld2D(mousePosition, app->camera);
    int hoverX = 0;
    int hoverY = 0;
    bool hoverValid = CanvasWorldToPixel(&app->canvas, worldPosition, &hoverX, &hoverY);
    if (!hoverValid && app->tools.hasLastCursor)
    {
        hoverX = app->tools.lastCursorX;
        hoverY = app->tools.lastCursorY;
        hoverValid = CanvasContains(&app->canvas, hoverX, hoverY);
    }

    if (hoverValid)
    {
        Rectangle pixelRect = {
            (float)(hoverX * app->canvas.pixelSize),
            (float)(hoverY * app->canvas.pixelSize),
            (float)app->canvas.pixelSize,
            (float)app->canvas.pixelSize
        };
        Color hoverFill = Fade(SKYBLUE, 0.25f);
        Color hoverOutline = Fade(SKYBLUE, 0.9f);
        float hoverThickness = 2.0f / app->camera.zoom;
        if (hoverThickness < 0.75f) hoverThickness = 0.75f;
        DrawRectangleRec(pixelRect, hoverFill);
        DrawRectangleLinesEx(pixelRect, hoverThickness, hoverOutline);
    }
    EndMode2D();

    UIDraw(&app->ui, &app->tools, app->camera.zoom);

    EndDrawing();
}
