#ifndef UI_H
#define UI_H

#include <raylib.h>
#include <stdbool.h>
#include "color.h"

// Color picker state
typedef struct {
    ColorHSV currentHSV;
    unsigned char alpha;
    bool isOpen;
    Rectangle bounds;

    // Slider positions
    Rectangle hueSlider;
    Rectangle saturationSlider;
    Rectangle valueSlider;
    Rectangle alphaSlider;

    // Which slider is being dragged
    int activeSlider; // 0=none, 1=hue, 2=sat, 3=val, 4=alpha
} ColorPicker;

// Initialize color picker
ColorPicker InitColorPicker(float x, float y, float width, float height);

// Update color picker (handles input)
// Returns true if a color was selected/changed
bool UpdateColorPicker(ColorPicker* picker, Color* outputColor);

// Draw color picker UI
void DrawColorPicker(ColorPicker* picker, Color currentColor);

// Draw foreground/background color swatches
void DrawColorSwatches(float x, float y, float size, Color foreground, Color background);

// Check if mouse is over color picker
bool IsMouseOverColorPicker(ColorPicker* picker);

// Toggle color picker visibility
void ToggleColorPicker(ColorPicker* picker);

// Set color picker color
void SetColorPickerColor(ColorPicker* picker, Color color);

#endif // UI_H
