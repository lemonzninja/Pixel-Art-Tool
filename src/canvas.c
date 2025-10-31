/**************************************************************************************
*                   - Core Canvas System -                                                    
*  Create a 2D array or dynamic structure to store pixel data (colors)                                                                                 
*  Render the pixel grid using DrawRectangle() or a texture-based approach                                                                     
*  Add grid overlay that can be toggled on/off             
**************************************************************************************/

#include "canvas.h"
#include "tools.h"

static bool CanvasCoordsFromMouse(const AppState *state, Vector2 mouse, int *x, int *y);
static Rectangle CanvasPixelToScreenRect(int x, int y);
static void DrawCanvasBackground(Rectangle canvasRect);

void InitCanvas(AppState *state)
{
    if (!state)
    {
        return;
    }

    state->canvas.width = CANVAS_PIXEL_WIDTH;
    state->canvas.height = CANVAS_PIXEL_HEIGHT;

    int pixelCount = state->canvas.width * state->canvas.height;
    state->canvas.pixels = MemAlloc((unsigned int)pixelCount * sizeof(Color));
    if (!state->canvas.pixels)
    {
        state->canvas.width = 0;
        state->canvas.height = 0;
        return;
    }

    for (int i = 0; i < pixelCount; i++)
    {
        state->canvas.pixels[i] = BLANK;
    }
}

void ShutdownCanvas(AppState *state)
{
    if (!state)
    {
        return;
    }

    if (state->canvas.pixels)
    {
        MemFree(state->canvas.pixels);
        state->canvas.pixels = NULL;
    }

    state->canvas.width = 0;
    state->canvas.height = 0;
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
    if (CanvasCoordsFromMouse(state, mouse, &canvasX, &canvasY))
    {
        if (ApplyTool(state, canvasX, canvasY))
        {
            *pixelModified = true;
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
    DrawCanvasBackground(canvasRect);

    const Canvas *canvas = &state->canvas;
    if (canvas->pixels)
    {
        for (int y = 0; y < canvas->height; y++)
        {
            for (int x = 0; x < canvas->width; x++)
            {
                Color pixel = canvas->pixels[y * canvas->width + x];
                if (pixel.a == 0)
                {
                    continue;
                }

                Rectangle pixelRect = CanvasPixelToScreenRect(x, y);
                DrawRectangleRec(pixelRect, pixel);
            }
        }
    }
    DrawRectangleLinesEx(canvasRect, 2.0f, DARKGRAY);

    int canvasX = 0;
    int canvasY = 0;
    if (CanvasCoordsFromMouse(state, mouse, &canvasX, &canvasY))
    {
        Rectangle hoverRect = CanvasPixelToScreenRect(canvasX, canvasY);
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

static bool CanvasCoordsFromMouse(const AppState *state, Vector2 mouse, int *x, int *y)
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

    int maxWidth = state ? state->canvas.width : CANVAS_PIXEL_WIDTH;
    int maxHeight = state ? state->canvas.height : CANVAS_PIXEL_HEIGHT;

    if (localX < 0)
    {
        localX = 0;
    }
    else if (localX >= maxWidth)
    {
        localX = maxWidth - 1;
    }

    if (localY < 0)
    {
        localY = 0;
    }
    else if (localY >= maxHeight)
    {
        localY = maxHeight - 1;
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

static Rectangle CanvasPixelToScreenRect(int x, int y)
{
    return (Rectangle){
        CANVAS_POSITION.x + x * CANVAS_ZOOM,
        CANVAS_POSITION.y + y * CANVAS_ZOOM,
        (float)CANVAS_ZOOM,
        (float)CANVAS_ZOOM
    };
}

static void DrawCanvasBackground(Rectangle canvasRect)
{
    const int tileSize = CANVAS_ZOOM;
    const Color lightTile = { 220, 220, 220, 255 };
    const Color darkTile = { 200, 200, 200, 255 };

    int tilesX = (int)(canvasRect.width / tileSize);
    int tilesY = (int)(canvasRect.height / tileSize);

    for (int y = 0; y < tilesY; y++)
    {
        for (int x = 0; x < tilesX; x++)
        {
            Rectangle tileRect = {
                canvasRect.x + x * tileSize,
                canvasRect.y + y * tileSize,
                (float)tileSize,
                (float)tileSize
            };

            bool isLight = ((x + y) % 2) == 0;
            DrawRectangleRec(tileRect, isLight ? lightTile : darkTile);
        }
    }
}
