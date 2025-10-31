#include "canvas.h"

#include "tools.h"

static bool CanvasCoordsFromMouse(Vector2 mouse, int *x, int *y);
static void UploadCanvasPixel(const AppState *state, int x, int y);

void InitCanvas(AppState *state)
{
    if (!state)
    {
        return;
    }

    state->canvasImage = GenImageColor(CANVAS_PIXEL_WIDTH, CANVAS_PIXEL_HEIGHT, BLANK);
    state->canvasTexture = LoadTextureFromImage(state->canvasImage);
    SetTextureFilter(state->canvasTexture, TEXTURE_FILTER_POINT);
}

void ShutdownCanvas(AppState *state)
{
    if (!state)
    {
        return;
    }

    UnloadTexture(state->canvasTexture);
    UnloadImage(state->canvasImage);
}

void HandleCanvasInput(AppState *state, Vector2 mouse, bool *pixelModified, bool interactingWithUI)
{
    if (!state || !pixelModified)
    {
        return;
    }

    *pixelModified = false;

    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) || interactingWithUI)
    {
        return;
    }

    int canvasX = 0;
    int canvasY = 0;
    if (CanvasCoordsFromMouse(mouse, &canvasX, &canvasY))
    {
        if (ApplyTool(state, canvasX, canvasY))
        {
            *pixelModified = true;
            UploadCanvasPixel(state, canvasX, canvasY);
        }
    }
}

void DrawCanvas(const AppState *state, Vector2 mouse)
{
    if (!state)
    {
        return;
    }

    Rectangle canvasRect = GetCanvasScreenRect();
    DrawTextureEx(state->canvasTexture, (Vector2){ canvasRect.x, canvasRect.y }, 0.0f, (float)CANVAS_ZOOM, WHITE);
    DrawRectangleLinesEx(canvasRect, 2.0f, DARKGRAY);

    int canvasX = 0;
    int canvasY = 0;
    if (CanvasCoordsFromMouse(mouse, &canvasX, &canvasY))
    {
        Rectangle hoverRect = {
            canvasRect.x + canvasX * CANVAS_ZOOM,
            canvasRect.y + canvasY * CANVAS_ZOOM,
            (float)CANVAS_ZOOM,
            (float)CANVAS_ZOOM
        };
        DrawRectangleLinesEx(hoverRect, 1.0f, Fade(BLACK, 0.5f));
    }
}

Rectangle GetCanvasScreenRect(void)
{
    return (Rectangle){
        CANVAS_POSITION.x,
        CANVAS_POSITION.y,
        (float)CANVAS_PIXEL_WIDTH * CANVAS_ZOOM,
        (float)CANVAS_PIXEL_HEIGHT * CANVAS_ZOOM
    };
}

static bool CanvasCoordsFromMouse(Vector2 mouse, int *x, int *y)
{
    Rectangle canvasRect = GetCanvasScreenRect();
    if (!CheckCollisionPointRec(mouse, canvasRect))
    {
        return false;
    }

    float relativeX = (mouse.x - canvasRect.x) / (float)CANVAS_ZOOM;
    float relativeY = (mouse.y - canvasRect.y) / (float)CANVAS_ZOOM;

    int localX = (int)relativeX;
    int localY = (int)relativeY;

    if (localX < 0)
    {
        localX = 0;
    }
    else if (localX >= CANVAS_PIXEL_WIDTH)
    {
        localX = CANVAS_PIXEL_WIDTH - 1;
    }

    if (localY < 0)
    {
        localY = 0;
    }
    else if (localY >= CANVAS_PIXEL_HEIGHT)
    {
        localY = CANVAS_PIXEL_HEIGHT - 1;
    }

    if (x)
    {
        *x = localX;
    }
    if (y)
    {
        *y = localY;
    }

    return true;
}

static void UploadCanvasPixel(const AppState *state, int x, int y)
{
    if (!state || !state->canvasImage.data)
    {
        return;
    }

    if (x < 0 || y < 0 || x >= CANVAS_PIXEL_WIDTH || y >= CANVAS_PIXEL_HEIGHT)
    {
        return;
    }

    const Color *pixels = (const Color *)state->canvasImage.data;
    Color pixel = pixels[y * CANVAS_PIXEL_WIDTH + x];

    Rectangle updateRect = {
        (float)x,
        (float)(CANVAS_PIXEL_HEIGHT - 1 - y),
        1.0f,
        1.0f
    };

    UpdateTextureRec(state->canvasTexture, updateRect, &pixel);
}
