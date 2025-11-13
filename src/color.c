#include "color.h"
#include <math.h>

float ClampFloat(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

int ClampInt(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

ColorHSV ColorToHSV(Color color) {
    ColorHSV hsv;

    float r = color.r / 255.0f;
    float g = color.g / 255.0f;
    float b = color.b / 255.0f;

    float max = fmaxf(fmaxf(r, g), b);
    float min = fminf(fminf(r, g), b);
    float delta = max - min;

    // Calculate Value
    hsv.v = max;

    // Calculate Saturation
    if (max > 0.0f) {
        hsv.s = delta / max;
    } else {
        hsv.s = 0.0f;
    }

    // Calculate Hue
    if (delta == 0.0f) {
        hsv.h = 0.0f; // Undefined, default to 0
    } else {
        if (max == r) {
            hsv.h = 60.0f * fmodf((g - b) / delta, 6.0f);
        } else if (max == g) {
            hsv.h = 60.0f * ((b - r) / delta + 2.0f);
        } else {
            hsv.h = 60.0f * ((r - g) / delta + 4.0f);
        }

        if (hsv.h < 0.0f) {
            hsv.h += 360.0f;
        }
    }

    return hsv;
}

Color HSVToColor(ColorHSV hsv, unsigned char alpha) {
    Color color;

    float c = hsv.v * hsv.s;
    float x = c * (1.0f - fabsf(fmodf(hsv.h / 60.0f, 2.0f) - 1.0f));
    float m = hsv.v - c;

    float r, g, b;

    if (hsv.h < 60.0f) {
        r = c; g = x; b = 0.0f;
    } else if (hsv.h < 120.0f) {
        r = x; g = c; b = 0.0f;
    } else if (hsv.h < 180.0f) {
        r = 0.0f; g = c; b = x;
    } else if (hsv.h < 240.0f) {
        r = 0.0f; g = x; b = c;
    } else if (hsv.h < 300.0f) {
        r = x; g = 0.0f; b = c;
    } else {
        r = c; g = 0.0f; b = x;
    }

    color.r = (unsigned char)((r + m) * 255.0f);
    color.g = (unsigned char)((g + m) * 255.0f);
    color.b = (unsigned char)((b + m) * 255.0f);
    color.a = alpha;

    return color;
}

Color CreateColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}
