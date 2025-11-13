#ifndef COLOR_H
#define COLOR_H

#include <raylib.h>

// HSV color representation (Hue, Saturation, Value)
typedef struct {
    float h; // Hue: 0.0 - 360.0
    float s; // Saturation: 0.0 - 1.0
    float v; // Value: 0.0 - 1.0
} ColorHSV;

// Convert RGB color to HSV
ColorHSV ColorToHSV(Color color);

// Convert HSV to RGB color
Color HSVToColor(ColorHSV hsv, unsigned char alpha);

// Create a color from RGBA values
Color CreateColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

// Clamp a float value between min and max
float ClampFloat(float value, float min, float max);

// Clamp an int value between min and max
int ClampInt(int value, int min, int max);

#endif // COLOR_H
