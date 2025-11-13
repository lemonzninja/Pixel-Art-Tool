#include "ui.h"
#include <stdio.h>

#define SLIDER_HEIGHT 20
#define SLIDER_SPACING 10
#define SLIDER_LABEL_WIDTH 30

ColorPicker InitColorPicker(float x, float y, float width, float height) {
    ColorPicker picker;
    picker.currentHSV.h = 0.0f;
    picker.currentHSV.s = 1.0f;
    picker.currentHSV.v = 1.0f;
    picker.alpha = 255;
    picker.isOpen = false;
    picker.bounds = (Rectangle){x, y, width, height};
    picker.activeSlider = 0;

    // Initialize slider positions
    float sliderY = y + 30;
    float sliderWidth = width - SLIDER_LABEL_WIDTH - 20;

    picker.hueSlider = (Rectangle){x + SLIDER_LABEL_WIDTH + 10, sliderY, sliderWidth, SLIDER_HEIGHT};
    sliderY += SLIDER_HEIGHT + SLIDER_SPACING;

    picker.saturationSlider = (Rectangle){x + SLIDER_LABEL_WIDTH + 10, sliderY, sliderWidth, SLIDER_HEIGHT};
    sliderY += SLIDER_HEIGHT + SLIDER_SPACING;

    picker.valueSlider = (Rectangle){x + SLIDER_LABEL_WIDTH + 10, sliderY, sliderWidth, SLIDER_HEIGHT};
    sliderY += SLIDER_HEIGHT + SLIDER_SPACING;

    picker.alphaSlider = (Rectangle){x + SLIDER_LABEL_WIDTH + 10, sliderY, sliderWidth, SLIDER_HEIGHT};

    return picker;
}

bool UpdateColorPicker(ColorPicker* picker, Color* outputColor) {
    if (!picker->isOpen) return false;

    bool colorChanged = false;
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

    // Handle slider dragging
    if (mousePressed) {
        // Check if mouse is over any slider
        if (CheckCollisionPointRec(mousePos, picker->hueSlider)) {
            picker->activeSlider = 1;
        } else if (CheckCollisionPointRec(mousePos, picker->saturationSlider)) {
            picker->activeSlider = 2;
        } else if (CheckCollisionPointRec(mousePos, picker->valueSlider)) {
            picker->activeSlider = 3;
        } else if (CheckCollisionPointRec(mousePos, picker->alphaSlider)) {
            picker->activeSlider = 4;
        }
    }

    if (mouseReleased) {
        picker->activeSlider = 0;
    }

    // Update slider values while dragging
    if (mouseDown && picker->activeSlider > 0) {
        float normalizedValue = 0.0f;

        switch (picker->activeSlider) {
            case 1: // Hue slider
                normalizedValue = (mousePos.x - picker->hueSlider.x) / picker->hueSlider.width;
                normalizedValue = ClampFloat(normalizedValue, 0.0f, 1.0f);
                picker->currentHSV.h = normalizedValue * 360.0f;
                colorChanged = true;
                break;

            case 2: // Saturation slider
                normalizedValue = (mousePos.x - picker->saturationSlider.x) / picker->saturationSlider.width;
                normalizedValue = ClampFloat(normalizedValue, 0.0f, 1.0f);
                picker->currentHSV.s = normalizedValue;
                colorChanged = true;
                break;

            case 3: // Value slider
                normalizedValue = (mousePos.x - picker->valueSlider.x) / picker->valueSlider.width;
                normalizedValue = ClampFloat(normalizedValue, 0.0f, 1.0f);
                picker->currentHSV.v = normalizedValue;
                colorChanged = true;
                break;

            case 4: // Alpha slider
                normalizedValue = (mousePos.x - picker->alphaSlider.x) / picker->alphaSlider.width;
                normalizedValue = ClampFloat(normalizedValue, 0.0f, 1.0f);
                picker->alpha = (unsigned char)(normalizedValue * 255.0f);
                colorChanged = true;
                break;
        }
    }

    // Update output color if changed
    if (colorChanged) {
        *outputColor = HSVToColor(picker->currentHSV, picker->alpha);
    }

    return colorChanged;
}

void DrawColorPicker(ColorPicker* picker, Color currentColor) {
    if (!picker->isOpen) return;

    // Draw background panel
    DrawRectangleRec(picker->bounds, (Color){50, 50, 50, 240});
    DrawRectangleLinesEx(picker->bounds, 2, LIGHTGRAY);

    // Draw title
    DrawText("Color Picker", picker->bounds.x + 10, picker->bounds.y + 5, 20, WHITE);

    float labelX = picker->bounds.x + 10;
    float valueX = picker->bounds.x + picker->bounds.width - 50;

    // Draw Hue slider
    DrawText("H:", labelX, picker->hueSlider.y + 3, 16, WHITE);

    // Draw hue gradient background
    for (int i = 0; i < picker->hueSlider.width; i++) {
        float hue = (i / picker->hueSlider.width) * 360.0f;
        ColorHSV hsv = {hue, 1.0f, 1.0f};
        Color hueColor = HSVToColor(hsv, 255);
        DrawRectangle(picker->hueSlider.x + i, picker->hueSlider.y, 1, picker->hueSlider.height, hueColor);
    }

    DrawRectangleLinesEx(picker->hueSlider, 1, WHITE);

    // Draw hue handle
    float hueHandleX = picker->hueSlider.x + (picker->currentHSV.h / 360.0f) * picker->hueSlider.width;
    DrawRectangle(hueHandleX - 2, picker->hueSlider.y, 4, picker->hueSlider.height, BLACK);
    DrawText(TextFormat("%.0f", picker->currentHSV.h), valueX, picker->hueSlider.y + 3, 16, WHITE);

    // Draw Saturation slider
    DrawText("S:", labelX, picker->saturationSlider.y + 3, 16, WHITE);

    // Draw saturation gradient
    for (int i = 0; i < picker->saturationSlider.width; i++) {
        float sat = i / picker->saturationSlider.width;
        ColorHSV hsv = {picker->currentHSV.h, sat, picker->currentHSV.v};
        Color satColor = HSVToColor(hsv, 255);
        DrawRectangle(picker->saturationSlider.x + i, picker->saturationSlider.y, 1, picker->saturationSlider.height, satColor);
    }

    DrawRectangleLinesEx(picker->saturationSlider, 1, WHITE);

    // Draw saturation handle
    float satHandleX = picker->saturationSlider.x + picker->currentHSV.s * picker->saturationSlider.width;
    DrawRectangle(satHandleX - 2, picker->saturationSlider.y, 4, picker->saturationSlider.height, BLACK);
    DrawText(TextFormat("%.2f", picker->currentHSV.s), valueX, picker->saturationSlider.y + 3, 16, WHITE);

    // Draw Value slider
    DrawText("V:", labelX, picker->valueSlider.y + 3, 16, WHITE);

    // Draw value gradient
    for (int i = 0; i < picker->valueSlider.width; i++) {
        float val = i / picker->valueSlider.width;
        ColorHSV hsv = {picker->currentHSV.h, picker->currentHSV.s, val};
        Color valColor = HSVToColor(hsv, 255);
        DrawRectangle(picker->valueSlider.x + i, picker->valueSlider.y, 1, picker->valueSlider.height, valColor);
    }

    DrawRectangleLinesEx(picker->valueSlider, 1, WHITE);

    // Draw value handle
    float valHandleX = picker->valueSlider.x + picker->currentHSV.v * picker->valueSlider.width;
    DrawRectangle(valHandleX - 2, picker->valueSlider.y, 4, picker->valueSlider.height, BLACK);
    DrawText(TextFormat("%.2f", picker->currentHSV.v), valueX, picker->valueSlider.y + 3, 16, WHITE);

    // Draw Alpha slider
    DrawText("A:", labelX, picker->alphaSlider.y + 3, 16, WHITE);

    // Draw alpha gradient with checkerboard background
    for (int i = 0; i < picker->alphaSlider.width; i++) {
        // Checkerboard pattern
        int checkSize = 4;
        int checkX = i / checkSize;
        int checkY = 0;
        Color checkColor = ((checkX + checkY) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
        DrawRectangle(picker->alphaSlider.x + i, picker->alphaSlider.y, 1, picker->alphaSlider.height, checkColor);

        // Alpha color overlay
        float alpha = i / picker->alphaSlider.width;
        Color alphaColor = HSVToColor(picker->currentHSV, (unsigned char)(alpha * 255));
        DrawRectangle(picker->alphaSlider.x + i, picker->alphaSlider.y, 1, picker->alphaSlider.height, alphaColor);
    }

    DrawRectangleLinesEx(picker->alphaSlider, 1, WHITE);

    // Draw alpha handle
    float alphaHandleX = picker->alphaSlider.x + (picker->alpha / 255.0f) * picker->alphaSlider.width;
    DrawRectangle(alphaHandleX - 2, picker->alphaSlider.y, 4, picker->alphaSlider.height, BLACK);
    DrawText(TextFormat("%d", picker->alpha), valueX, picker->alphaSlider.y + 3, 16, WHITE);

    // Draw color preview
    float previewY = picker->alphaSlider.y + SLIDER_HEIGHT + SLIDER_SPACING + 10;
    float previewSize = 60;
    float previewX = picker->bounds.x + (picker->bounds.width - previewSize) / 2;

    // Checkerboard background for preview
    int checkSize = 8;
    for (int y = 0; y < previewSize; y += checkSize) {
        for (int x = 0; x < previewSize; x += checkSize) {
            Color checkColor = (((x / checkSize) + (y / checkSize)) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
            DrawRectangle(previewX + x, previewY + y, checkSize, checkSize, checkColor);
        }
    }

    DrawRectangle(previewX, previewY, previewSize, previewSize, currentColor);
    DrawRectangleLinesEx((Rectangle){previewX, previewY, previewSize, previewSize}, 2, WHITE);
}

void DrawColorSwatches(float x, float y, float size, Color foreground, Color background) {
    // Draw background color swatch (larger, behind)
    float bgSize = size;
    float bgX = x + size * 0.3f;
    float bgY = y + size * 0.3f;

    // Checkerboard for background swatch
    int checkSize = 4;
    for (int cy = 0; cy < bgSize; cy += checkSize) {
        for (int cx = 0; cx < bgSize; cx += checkSize) {
            Color checkColor = (((cx / checkSize) + (cy / checkSize)) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
            DrawRectangle(bgX + cx, bgY + cy, checkSize, checkSize, checkColor);
        }
    }

    DrawRectangle(bgX, bgY, bgSize, bgSize, background);
    DrawRectangleLines(bgX, bgY, bgSize, bgSize, BLACK);

    // Draw foreground color swatch (smaller, in front)
    float fgSize = size;
    float fgX = x;
    float fgY = y;

    // Checkerboard for foreground swatch
    for (int cy = 0; cy < fgSize; cy += checkSize) {
        for (int cx = 0; cx < fgSize; cx += checkSize) {
            Color checkColor = (((cx / checkSize) + (cy / checkSize)) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
            DrawRectangle(fgX + cx, fgY + cy, checkSize, checkSize, checkColor);
        }
    }

    DrawRectangle(fgX, fgY, fgSize, fgSize, foreground);
    DrawRectangleLines(fgX, fgY, fgSize, fgSize, BLACK);
}

bool IsMouseOverColorPicker(ColorPicker* picker) {
    if (!picker->isOpen) return false;
    return CheckCollisionPointRec(GetMousePosition(), picker->bounds);
}

void ToggleColorPicker(ColorPicker* picker) {
    picker->isOpen = !picker->isOpen;
}

void SetColorPickerColor(ColorPicker* picker, Color color) {
    picker->currentHSV = ColorToHSV(color);
    picker->alpha = color.a;
}
