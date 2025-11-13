/**
 * camera.h
 *
 * Camera/Viewport System for Pixel Art Tool
 * Handles pan and zoom operations for canvas navigation
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include <stdbool.h>

/**
 * CanvasCamera structure
 * Manages viewport position and zoom level for canvas navigation
 */
typedef struct {
    Vector2 position;      // Camera offset (top-left corner position)
    float zoom;            // Zoom level (1.0 = 100%, 2.0 = 200%, etc.)
    float minZoom;         // Minimum allowed zoom level
    float maxZoom;         // Maximum allowed zoom level

    // Pan state
    bool isPanning;        // Whether user is currently panning
    Vector2 panStartPos;   // Mouse position when pan started
    Vector2 camStartPos;   // Camera position when pan started
} CanvasCamera;

/**
 * Create and initialize a new canvas camera
 * Default zoom: 1.0x
 * Default zoom range: 0.5x to 16.0x
 *
 * @return Pointer to newly created CanvasCamera (must be freed with DestroyCanvasCamera)
 */
CanvasCamera* CreateCanvasCamera(void);

/**
 * Destroy canvas camera and free memory
 *
 * @param camera CanvasCamera to destroy
 */
void DestroyCanvasCamera(CanvasCamera* camera);

/**
 * Update canvas camera based on user input
 * Handles:
 * - Pan with middle mouse button drag
 * - Pan with spacebar + left mouse drag
 * - Zoom with mouse wheel (centered on cursor)
 *
 * @param camera CanvasCamera to update
 */
void UpdateCanvasCamera(CanvasCamera* camera);

/**
 * Pan the canvas camera by a delta amount
 *
 * @param camera CanvasCamera to pan
 * @param delta Movement delta in screen coordinates
 */
void PanCanvasCamera(CanvasCamera* camera, Vector2 delta);

/**
 * Zoom the canvas camera by a factor, centered on a specific point
 * Automatically clamps to min/max zoom levels
 *
 * @param camera CanvasCamera to zoom
 * @param zoomFactor Zoom multiplier (e.g., 1.1 for 10% zoom in)
 * @param centerScreen Screen position to zoom towards (usually mouse position)
 */
void ZoomCanvasCamera(CanvasCamera* camera, float zoomFactor, Vector2 centerScreen);

/**
 * Clamp canvas camera zoom to valid range [minZoom, maxZoom]
 *
 * @param camera CanvasCamera to clamp
 */
void ClampCanvasCameraZoom(CanvasCamera* camera);

/**
 * Reset canvas camera to default state
 * Position: (0, 0)
 * Zoom: 1.0x
 *
 * @param camera CanvasCamera to reset
 */
void ResetCanvasCamera(CanvasCamera* camera);

/**
 * Get canvas camera zoom percentage as integer (e.g., 100 for 1.0x zoom)
 *
 * @param camera CanvasCamera to query
 * @return Zoom level as percentage
 */
int GetCanvasCameraZoomPercent(CanvasCamera* camera);

#endif // CAMERA_H
