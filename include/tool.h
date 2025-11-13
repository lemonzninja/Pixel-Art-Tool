/**
 * tool.h
 *
 * Drawing Tool System for Pixel Art Tool
 * Handles pencil/brush, eraser, and other drawing tools
 */

#ifndef TOOL_H
#define TOOL_H

#include "raylib.h"
#include "canvas.h"
#include "camera.h"
#include <stdbool.h>

/**
 * Tool types available in the application
 */
typedef enum {
    TOOL_PENCIL,    // Draw with foreground color
    TOOL_ERASER,    // Erase pixels (set to transparent)
    TOOL_EYEDROPPER // Sample color from canvas
} ToolType;

/**
 * ToolState structure
 * Manages current drawing tool, colors, and drawing state
 */
typedef struct {
    ToolType currentTool;       // Currently selected tool
    Color foregroundColor;      // Primary drawing color
    Color backgroundColor;      // Secondary color (for right-click, future use)

    // Drawing state
    bool isDrawing;             // Whether user is currently drawing
    int lastPixelX;             // Last drawn pixel X coordinate
    int lastPixelY;             // Last drawn pixel Y coordinate
    bool hasLastPixel;          // Whether we have a valid last pixel position
} ToolState;

/**
 * Create and initialize a new tool state
 * Default tool: TOOL_PENCIL
 * Default foreground: BLACK
 * Default background: WHITE
 *
 * @return Pointer to newly created ToolState (must be freed with DestroyToolState)
 */
ToolState* CreateToolState(void);

/**
 * Destroy tool state and free memory
 *
 * @param state ToolState to destroy
 */
void DestroyToolState(ToolState* state);

/**
 * Set the current drawing tool
 *
 * @param state ToolState to update
 * @param tool Tool type to set
 */
void SetCurrentTool(ToolState* state, ToolType tool);

/**
 * Get the current drawing tool
 *
 * @param state ToolState to query
 * @return Current tool type
 */
ToolType GetCurrentTool(ToolState* state);

/**
 * Set the foreground color
 *
 * @param state ToolState to update
 * @param color Color to set
 */
void SetForegroundColor(ToolState* state, Color color);

/**
 * Set the background color
 *
 * @param state ToolState to update
 * @param color Color to set
 */
void SetBackgroundColor(ToolState* state, Color color);

/**
 * Get the current foreground color
 *
 * @param state ToolState to query
 * @return Current foreground color
 */
Color GetForegroundColor(ToolState* state);

/**
 * Get the current background color
 *
 * @param state ToolState to query
 * @return Current background color
 */
Color GetBackgroundColor(ToolState* state);

/**
 * Update tool state based on user input
 * Handles:
 * - Tool switching with keyboard shortcuts (B for brush/pencil, E for eraser)
 * - Mouse input for drawing
 * - Click and drag drawing
 *
 * @param state ToolState to update
 * @param canvas Canvas to draw on
 * @param camera Camera for coordinate conversion
 * @param pixelSize Base pixel size (before zoom)
 */
void UpdateToolState(ToolState* state, Canvas* canvas, CanvasCamera* camera, int pixelSize);

/**
 * Draw a single pixel with the current tool at the given canvas coordinates
 *
 * @param state ToolState containing current tool and colors
 * @param canvas Canvas to draw on
 * @param pixelX Canvas X coordinate
 * @param pixelY Canvas Y coordinate
 */
void DrawPixelWithTool(ToolState* state, Canvas* canvas, int pixelX, int pixelY);

/**
 * Get the name of the current tool as a string
 *
 * @param state ToolState to query
 * @return Tool name string (e.g., "Pencil", "Eraser")
 */
const char* GetToolName(ToolState* state);

/**
 * Swap foreground and background colors
 *
 * @param state ToolState to update
 */
void SwapColors(ToolState* state);

#endif // TOOL_H
