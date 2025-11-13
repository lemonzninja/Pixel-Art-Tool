/**
 * camera.c
 *
 * Implementation of Camera/Viewport System
 */

#include "camera.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <math.h>

// Default camera settings
#define DEFAULT_ZOOM 1.0f
#define DEFAULT_MIN_ZOOM 0.5f
#define DEFAULT_MAX_ZOOM 16.0f
#define ZOOM_INCREMENT 1.1f  // 10% per scroll tick

/**
 * Create and initialize a new canvas camera
 */
CanvasCamera* CreateCanvasCamera(void) {
    CanvasCamera* camera = (CanvasCamera*)malloc(sizeof(CanvasCamera));
    if (camera == NULL) {
        return NULL;
    }

    // Initialize camera with defaults
    camera->position = (Vector2){0.0f, 0.0f};
    camera->zoom = DEFAULT_ZOOM;
    camera->minZoom = DEFAULT_MIN_ZOOM;
    camera->maxZoom = DEFAULT_MAX_ZOOM;
    camera->isPanning = false;
    camera->panStartPos = (Vector2){0.0f, 0.0f};
    camera->camStartPos = (Vector2){0.0f, 0.0f};

    return camera;
}

/**
 * Destroy canvas camera and free memory
 */
void DestroyCanvasCamera(CanvasCamera* camera) {
    if (camera != NULL) {
        free(camera);
    }
}

/**
 * Pan the canvas camera by a delta amount
 */
void PanCanvasCamera(CanvasCamera* camera, Vector2 delta) {
    if (camera == NULL) return;

    camera->position.x += delta.x;
    camera->position.y += delta.y;
}

/**
 * Clamp canvas camera zoom to valid range
 */
void ClampCanvasCameraZoom(CanvasCamera* camera) {
    if (camera == NULL) return;

    if (camera->zoom < camera->minZoom) {
        camera->zoom = camera->minZoom;
    }
    if (camera->zoom > camera->maxZoom) {
        camera->zoom = camera->maxZoom;
    }
}

/**
 * Zoom the canvas camera by a factor, centered on a specific point
 */
void ZoomCanvasCamera(CanvasCamera* camera, float zoomFactor, Vector2 centerScreen) {
    if (camera == NULL) return;

    // Calculate world position of the zoom center before zoom
    Vector2 worldPosBefore = {
        (centerScreen.x - camera->position.x) / camera->zoom,
        (centerScreen.y - camera->position.y) / camera->zoom
    };

    // Apply zoom
    camera->zoom *= zoomFactor;
    ClampCanvasCameraZoom(camera);

    // Calculate world position of the zoom center after zoom
    Vector2 worldPosAfter = {
        (centerScreen.x - camera->position.x) / camera->zoom,
        (centerScreen.y - camera->position.y) / camera->zoom
    };

    // Adjust camera position to keep the same world point under the cursor
    Vector2 worldDiff = {
        worldPosBefore.x - worldPosAfter.x,
        worldPosBefore.y - worldPosAfter.y
    };

    camera->position.x += worldDiff.x * camera->zoom;
    camera->position.y += worldDiff.y * camera->zoom;
}

/**
 * Reset canvas camera to default state
 */
void ResetCanvasCamera(CanvasCamera* camera) {
    if (camera == NULL) return;

    camera->position = (Vector2){0.0f, 0.0f};
    camera->zoom = DEFAULT_ZOOM;
    camera->isPanning = false;
}

/**
 * Get canvas camera zoom percentage as integer
 */
int GetCanvasCameraZoomPercent(CanvasCamera* camera) {
    if (camera == NULL) return 100;
    return (int)(camera->zoom * 100.0f);
}

/**
 * Update canvas camera based on user input
 */
void UpdateCanvasCamera(CanvasCamera* camera) {
    if (camera == NULL) return;

    // --- Handle Pan Input ---

    // Check if panning should start
    bool middleMousePressed = IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE);
    bool spacebarAndLeftMousePressed = IsKeyDown(KEY_SPACE) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    if (middleMousePressed || spacebarAndLeftMousePressed) {
        camera->isPanning = true;
        camera->panStartPos = GetMousePosition();
        camera->camStartPos = camera->position;
    }

    // Check if panning should continue
    bool middleMouseDown = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);
    bool spacebarAndLeftMouseDown = IsKeyDown(KEY_SPACE) && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    if (camera->isPanning) {
        if (middleMouseDown || spacebarAndLeftMouseDown) {
            // Calculate pan delta
            Vector2 currentMousePos = GetMousePosition();
            Vector2 mouseDelta = {
                currentMousePos.x - camera->panStartPos.x,
                currentMousePos.y - camera->panStartPos.y
            };

            // Update camera position (relative to starting position)
            camera->position.x = camera->camStartPos.x + mouseDelta.x;
            camera->position.y = camera->camStartPos.y + mouseDelta.y;
        } else {
            // Stop panning if mouse button released
            camera->isPanning = false;
        }
    }

    // --- Handle Zoom Input ---

    float wheelMove = GetMouseWheelMove();
    if (wheelMove != 0.0f) {
        // Determine zoom factor based on scroll direction
        float zoomFactor = (wheelMove > 0) ? ZOOM_INCREMENT : (1.0f / ZOOM_INCREMENT);

        // Zoom centered on mouse cursor
        Vector2 mousePos = GetMousePosition();
        ZoomCanvasCamera(camera, zoomFactor, mousePos);
    }

    // --- Handle Reset (optional: R key to reset camera) ---

    if (IsKeyPressed(KEY_R)) {
        ResetCanvasCamera(camera);
    }
}
