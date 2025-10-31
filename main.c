/*******************************************************************************************
*
*
*
*
********************************************************************************************/

#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Types Definition
//----------------------------------------------------------------------------------

typedef enum ToolType {
    TOOL_PENCIL = 0,
    TOOL_ERASER = 1
} ToolType;

typedef struct AppState {
    RenderTexture2D canvasTarget;
    ToolType activeTool;
    int activeColorIndex;
    Color palette[6];
    int paletteCount;
} AppState;

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 450;
static const int CANVAS_PIXEL_WIDTH = 64;
static const int CANVAS_PIXEL_HEIGHT = 64;
static const int CANVAS_ZOOM = 8;
static const Vector2 CANVAS_POSITION = { 40.0f, 40.0f };

static AppState gState = { 0 };

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);          // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib pixel art");

    gState.canvasTarget = LoadRenderTexture(CANVAS_PIXEL_WIDTH, CANVAS_PIXEL_HEIGHT);
    gState.canvasTarget.texture.filter = TEXTURE_FILTER_POINT;

    BeginTextureMode(gState.canvasTarget);
        ClearBackground(BLANK);
    EndTextureMode();

    gState.palette[0] = BLACK;
    gState.palette[1] = WHITE;
    gState.palette[2] = RED;
    gState.palette[3] = GREEN;
    gState.palette[4] = BLUE;
    gState.palette[5] = YELLOW;
    gState.paletteCount = 6;
    gState.activeColorIndex = 0;
    gState.activeTool = TOOL_PENCIL;

    //--------------------------------------------------------------------------------------

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(gState.canvasTarget);

    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    Vector2 mouse = GetMousePosition();
    Rectangle canvasScreenRect = {
        CANVAS_POSITION.x,
        CANVAS_POSITION.y,
        (float)CANVAS_PIXEL_WIDTH * CANVAS_ZOOM,
        (float)CANVAS_PIXEL_HEIGHT * CANVAS_ZOOM
    };

    const float paletteCellSize = 32.0f;
    const float paletteSpacing = 12.0f;
    Vector2 paletteOrigin = {
        CANVAS_POSITION.x + canvasScreenRect.width + 40.0f,
        CANVAS_POSITION.y
    };

    bool interactingWithUI = false;

    // Handle palette selection via mouse.
    for (int i = 0; i < gState.paletteCount; i++)
    {
        Rectangle paletteRect = {
            paletteOrigin.x,
            paletteOrigin.y + i * (paletteCellSize + paletteSpacing),
            paletteCellSize,
            paletteCellSize
        };

        if (CheckCollisionPointRec(mouse, paletteRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            gState.activeColorIndex = i;
            interactingWithUI = true;
        }
    }

    // Handle tool selection via mouse buttons on the UI.
    Rectangle toolButtonSize = { paletteOrigin.x, paletteOrigin.y + gState.paletteCount * (paletteCellSize + paletteSpacing) + 40.0f, 140.0f, 32.0f };
    Rectangle pencilButton = toolButtonSize;
    Rectangle eraserButton = {
        toolButtonSize.x,
        toolButtonSize.y + toolButtonSize.height + 12.0f,
        toolButtonSize.width,
        toolButtonSize.height
    };

    if (CheckCollisionPointRec(mouse, pencilButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        gState.activeTool = TOOL_PENCIL;
        interactingWithUI = true;
    }

    if (CheckCollisionPointRec(mouse, eraserButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        gState.activeTool = TOOL_ERASER;
        interactingWithUI = true;
    }

    // Handle keyboard shortcuts for tools.
    if (IsKeyPressed(KEY_ONE)) gState.activeTool = TOOL_PENCIL;
    if (IsKeyPressed(KEY_TWO)) gState.activeTool = TOOL_ERASER;

    bool mouseOnCanvas = CheckCollisionPointRec(mouse, canvasScreenRect);

    if (mouseOnCanvas && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !interactingWithUI)
    {
        int canvasX = (int)((mouse.x - CANVAS_POSITION.x) / CANVAS_ZOOM);
        int canvasY = (int)((mouse.y - CANVAS_POSITION.y) / CANVAS_ZOOM);

        if (canvasX < 0) canvasX = 0;
        if (canvasY < 0) canvasY = 0;
        if (canvasX >= CANVAS_PIXEL_WIDTH) canvasX = CANVAS_PIXEL_WIDTH - 1;
        if (canvasY >= CANVAS_PIXEL_HEIGHT) canvasY = CANVAS_PIXEL_HEIGHT - 1;

        BeginTextureMode(gState.canvasTarget);
            if (gState.activeTool == TOOL_PENCIL)
            {
                DrawRectangle(canvasX, canvasY, 1, 1, gState.palette[gState.activeColorIndex]);
            }
            else if (gState.activeTool == TOOL_ERASER)
            {
                DrawRectangle(canvasX, canvasY, 1, 1, BLANK);
            }
        EndTextureMode();
    }

    BeginDrawing();

        ClearBackground(RAYWHITE);

        Rectangle source = { 0.0f, 0.0f, (float)CANVAS_PIXEL_WIDTH, -(float)CANVAS_PIXEL_HEIGHT };
        Rectangle dest = { canvasScreenRect.x, canvasScreenRect.y, canvasScreenRect.width, canvasScreenRect.height };
        DrawTexturePro(gState.canvasTarget.texture, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f);

        // Draw canvas border and hover indicator.
        DrawRectangleLinesEx(canvasScreenRect, 2.0f, DARKGRAY);
        if (mouseOnCanvas)
        {
            int hoverX = (int)((mouse.x - CANVAS_POSITION.x) / CANVAS_ZOOM);
            int hoverY = (int)((mouse.y - CANVAS_POSITION.y) / CANVAS_ZOOM);
            if (hoverX >= 0 && hoverX < CANVAS_PIXEL_WIDTH && hoverY >= 0 && hoverY < CANVAS_PIXEL_HEIGHT)
            {
                Rectangle hoverRect = {
                    CANVAS_POSITION.x + hoverX * CANVAS_ZOOM,
                    CANVAS_POSITION.y + hoverY * CANVAS_ZOOM,
                    (float)CANVAS_ZOOM,
                    (float)CANVAS_ZOOM
                };
                DrawRectangleLinesEx(hoverRect, 1.0f, Fade(BLACK, 0.5f));
            }
        }

        // Draw palette UI.
        for (int i = 0; i < gState.paletteCount; i++)
        {
            Rectangle paletteRect = {
                paletteOrigin.x,
                paletteOrigin.y + i * (paletteCellSize + paletteSpacing),
                paletteCellSize,
                paletteCellSize
            };

            DrawRectangleRec(paletteRect, gState.palette[i]);
            DrawRectangleLinesEx(paletteRect, 2.0f, BLACK);

            if (i == gState.activeColorIndex)
            {
                DrawRectangleLinesEx((Rectangle){ paletteRect.x - 3, paletteRect.y - 3, paletteRect.width + 6, paletteRect.height + 6 }, 2.0f, GOLD);
            }
        }

        DrawRectangleRec(pencilButton, Fade(LIGHTGRAY, gState.activeTool == TOOL_PENCIL ? 0.6f : 0.2f));
        DrawRectangleLinesEx(pencilButton, 2.0f, DARKGRAY);
        DrawText("Pencil [1]", (int)pencilButton.x + 8, (int)pencilButton.y + 8, 16, BLACK);

        DrawRectangleRec(eraserButton, Fade(LIGHTGRAY, gState.activeTool == TOOL_ERASER ? 0.6f : 0.2f));
        DrawRectangleLinesEx(eraserButton, 2.0f, DARKGRAY);
        DrawText("Eraser [2]", (int)eraserButton.x + 8, (int)eraserButton.y + 8, 16, BLACK);

        DrawText("Left click to draw. Click palette to change color.", 40, SCREEN_HEIGHT - 60, 16, DARKGRAY);

        DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}
