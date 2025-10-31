#include "ui.h"

#include "canvas.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static const int PANEL_X = 12;
static const int PANEL_Y = 12;
static const int PANEL_WIDTH = 260;
static const int TOOL_BUTTON_HEIGHT = 26;
static const int TOOL_BUTTON_SPACING = 6;
static const float TOOL_BUTTON_START_OFFSET = 84.0f;
static const float COLOR_SWATCH_SIZE = 36.0f;
static const float COLOR_HISTORY_SWATCH = 22.0f;
static const float COLOR_HISTORY_SPACING = 6.0f;
static const float COLOR_PICKER_SIZE = 180.0f;
static const float COLOR_PICKER_SLIDER_HEIGHT = 16.0f;
static const float COLOR_PICKER_SPACING = 8.0f;
static const float PALETTE_BUTTON_WIDTH = 72.0f;
static const float PALETTE_BUTTON_HEIGHT = 22.0f;
static const float PALETTE_BUTTON_SPACING = 8.0f;
static const int PALETTE_COLOR_COLUMNS = 6;
static const float PALETTE_COLOR_SIZE = 22.0f;
static const float PALETTE_COLOR_SPACING = 6.0f;
static const int STATUS_TIMER_MAX = 180;

typedef struct UILayout {
    Rectangle panel;
    int textX;
    float infoStartY;
    float infoEndY;
    Rectangle fgSwatchRect;
    Rectangle bgSwatchRect;
    Rectangle fgHistoryRects[COLOR_HISTORY_CAPACITY];
    size_t fgHistoryVisible;
    Rectangle bgHistoryRects[COLOR_HISTORY_CAPACITY];
    size_t bgHistoryVisible;
    Rectangle pickerSVRect;
    Rectangle pickerHueRect;
    Rectangle pickerAlphaRect;
    Rectangle palettePrevRect;
    Rectangle paletteNextRect;
    Rectangle paletteNewRect;
    Rectangle paletteAddFGRect;
    Rectangle paletteAddBGRect;
    Rectangle paletteSaveRect;
    Rectangle paletteLoadRect;
    Rectangle paletteColorRects[MAX_PALETTE_COLORS];
    size_t paletteColorVisible;
    Vector2 paletteNamePos;
    float paletteNameY;
    float paletteFileTextY;
    float statusTextY;
    float contentBottom;
} UILayout;

static Rectangle GetToolButtonRect(size_t index)
{
    float x = PANEL_X + 12.0f;
    float y = PANEL_Y + TOOL_BUTTON_START_OFFSET + (float)index * (TOOL_BUTTON_HEIGHT + TOOL_BUTTON_SPACING);
    float width = PANEL_WIDTH - 24.0f;
    return (Rectangle){ x, y, width, (float)TOOL_BUTTON_HEIGHT };
}

static bool ColorsEqual(Color a, Color b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

static Vector3 ColorToHSV01(Color color)
{
    float r = color.r / 255.0f;
    float g = color.g / 255.0f;
    float b = color.b / 255.0f;

    float max = fmaxf(fmaxf(r, g), b);
    float min = fminf(fminf(r, g), b);
    float delta = max - min;

    float h = 0.0f;
    if (delta > 0.00001f)
    {
        if (max == r)
        {
            h = fmodf(((g - b) / delta), 6.0f);
        }
        else if (max == g)
        {
            h = ((b - r) / delta) + 2.0f;
        }
        else
        {
            h = ((r - g) / delta) + 4.0f;
        }
        h /= 6.0f;
        if (h < 0.0f) h += 1.0f;
    }

    float s = (max <= 0.0f) ? 0.0f : (delta / max);
    float v = max;

    if (h < 0.0f) h = 0.0f;
    if (h > 1.0f) h = 1.0f;
    if (s < 0.0f) s = 0.0f;
    if (s > 1.0f) s = 1.0f;
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;

    return (Vector3){ h, s, v };
}

static Color ColorFromHSV01(float h, float s, float v, float alpha)
{
    if (h < 0.0f) h = 0.0f;
    if (h > 1.0f) h = 1.0f;
    if (s < 0.0f) s = 0.0f;
    if (s > 1.0f) s = 1.0f;
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;

    float hh = h * 6.0f;
    int sector = (int)floorf(hh);
    float f = hh - sector;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    float r = v;
    float g = t;
    float b = p;

    switch (sector % 6)
    {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }

    Color result = {
        (unsigned char)lroundf(r * 255.0f),
        (unsigned char)lroundf(g * 255.0f),
        (unsigned char)lroundf(b * 255.0f),
        (unsigned char)lroundf(alpha * 255.0f)
    };
    return result;
}

static void SetPickerFromColor(UIState *ui, Color color)
{
    if (!ui) return;
    Vector3 hsv = ColorToHSV01(color);
    ui->pickerHue = hsv.x;
    ui->pickerSaturation = hsv.y;
    ui->pickerValue = hsv.z;
    ui->pickerAlpha = color.a / 255.0f;
    ui->pickerLastColor = color;
}

static void ApplyPickerColor(UIState *ui, ToolState *tools)
{
    if (!ui || !tools) return;
    Color color = ColorFromHSV01(ui->pickerHue, ui->pickerSaturation, ui->pickerValue, ui->pickerAlpha);
    if (ui->colorPickerForForeground)
    {
        ToolsSetForegroundColor(tools, color);
    }
    else
    {
        ToolsSetBackgroundColor(tools, color);
    }
    ui->pickerLastColor = color;
}

static void SetPaletteStatus(UIState *ui, const char *message)
{
    if (!ui) return;
    if (!message) message = "";
    strncpy(ui->paletteStatusMessage, message, sizeof(ui->paletteStatusMessage));
    ui->paletteStatusMessage[sizeof(ui->paletteStatusMessage) - 1] = '\0';
    ui->paletteStatusTimer = STATUS_TIMER_MAX;
}

static void BuildUILayout(const UIState *ui, const ToolState *tools, size_t toolCount, UILayout *layout)
{
    if (!layout) return;
    memset(layout, 0, sizeof(*layout));

    layout->panel = (Rectangle){ (float)PANEL_X, (float)PANEL_Y, (float)PANEL_WIDTH, 0.0f };
    layout->textX = PANEL_X + 12;

    float infoStart = PANEL_Y + TOOL_BUTTON_START_OFFSET + (float)toolCount * (TOOL_BUTTON_HEIGHT + TOOL_BUTTON_SPACING) + 16.0f;
    float infoEnd = infoStart + 4.0f * 16.0f + 20.0f + 20.0f;
    layout->infoStartY = infoStart;
    layout->infoEndY = infoEnd;

    float colorSectionTop = infoEnd + 24.0f;
    layout->fgSwatchRect = (Rectangle){ (float)layout->textX, colorSectionTop + 24.0f, COLOR_SWATCH_SIZE, COLOR_SWATCH_SIZE };
    layout->bgSwatchRect = (Rectangle){ layout->fgSwatchRect.x + COLOR_SWATCH_SIZE + 36.0f, layout->fgSwatchRect.y, COLOR_SWATCH_SIZE, COLOR_SWATCH_SIZE };

    layout->fgHistoryVisible = tools ? tools->foregroundHistory.count : 0;
    float fgHistoryY = layout->fgSwatchRect.y + COLOR_SWATCH_SIZE + 18.0f;
    for (size_t i = 0; i < layout->fgHistoryVisible; ++i)
    {
        float hx = layout->fgSwatchRect.x + (float)i * (COLOR_HISTORY_SWATCH + COLOR_HISTORY_SPACING);
        layout->fgHistoryRects[i] = (Rectangle){ hx, fgHistoryY, COLOR_HISTORY_SWATCH, COLOR_HISTORY_SWATCH };
    }

    layout->bgHistoryVisible = tools ? tools->backgroundHistory.count : 0;
    float bgHistoryY = fgHistoryY + COLOR_HISTORY_SWATCH + 22.0f;
    for (size_t i = 0; i < layout->bgHistoryVisible; ++i)
    {
        float hx = layout->fgSwatchRect.x + (float)i * (COLOR_HISTORY_SWATCH + COLOR_HISTORY_SPACING);
        layout->bgHistoryRects[i] = (Rectangle){ hx, bgHistoryY, COLOR_HISTORY_SWATCH, COLOR_HISTORY_SWATCH };
    }

    float pickerTop = bgHistoryY + COLOR_HISTORY_SWATCH + 28.0f;
    layout->pickerSVRect = (Rectangle){ (float)layout->textX, pickerTop, COLOR_PICKER_SIZE, COLOR_PICKER_SIZE };
    layout->pickerHueRect = (Rectangle){ layout->pickerSVRect.x, layout->pickerSVRect.y + COLOR_PICKER_SIZE + COLOR_PICKER_SPACING, COLOR_PICKER_SIZE, COLOR_PICKER_SLIDER_HEIGHT };
    layout->pickerAlphaRect = (Rectangle){ layout->pickerSVRect.x, layout->pickerHueRect.y + COLOR_PICKER_SLIDER_HEIGHT + COLOR_PICKER_SPACING, COLOR_PICKER_SIZE, COLOR_PICKER_SLIDER_HEIGHT };

    float paletteSectionTop = layout->pickerAlphaRect.y + COLOR_PICKER_SLIDER_HEIGHT + 28.0f;
    layout->paletteNameY = paletteSectionTop;
    float controlsY = paletteSectionTop + 24.0f;
    layout->palettePrevRect = (Rectangle){ (float)layout->textX, controlsY, 28.0f, PALETTE_BUTTON_HEIGHT };
    layout->paletteNextRect = (Rectangle){ layout->palettePrevRect.x + 28.0f + 140.0f, controlsY, 28.0f, PALETTE_BUTTON_HEIGHT };
    layout->paletteNamePos = (Vector2){ layout->palettePrevRect.x + 28.0f + 8.0f, controlsY + 4.0f };

    float newPaletteY = controlsY + PALETTE_BUTTON_HEIGHT + PALETTE_BUTTON_SPACING;
    layout->paletteNewRect = (Rectangle){ (float)layout->textX, newPaletteY, PALETTE_BUTTON_WIDTH, PALETTE_BUTTON_HEIGHT };
    layout->paletteAddFGRect = (Rectangle){ layout->paletteNewRect.x + PALETTE_BUTTON_WIDTH + PALETTE_BUTTON_SPACING, newPaletteY, PALETTE_BUTTON_WIDTH, PALETTE_BUTTON_HEIGHT };
    layout->paletteAddBGRect = (Rectangle){ layout->paletteAddFGRect.x + PALETTE_BUTTON_WIDTH + PALETTE_BUTTON_SPACING, newPaletteY, PALETTE_BUTTON_WIDTH, PALETTE_BUTTON_HEIGHT };

    float saveLoadY = newPaletteY + PALETTE_BUTTON_HEIGHT + PALETTE_BUTTON_SPACING;
    layout->paletteSaveRect = (Rectangle){ (float)layout->textX, saveLoadY, PALETTE_BUTTON_WIDTH, PALETTE_BUTTON_HEIGHT };
    layout->paletteLoadRect = (Rectangle){ layout->paletteSaveRect.x + PALETTE_BUTTON_WIDTH + PALETTE_BUTTON_SPACING, saveLoadY, PALETTE_BUTTON_WIDTH, PALETTE_BUTTON_HEIGHT };

    layout->paletteFileTextY = saveLoadY + PALETTE_BUTTON_HEIGHT + 8.0f;
    layout->statusTextY = layout->paletteFileTextY + 18.0f;

    float gridTop = layout->statusTextY + 20.0f;
    layout->paletteColorVisible = 0;
    if (ui)
    {
        const Palette *palette = PaletteCollectionGetActiveConst(&ui->palettes);
        size_t colorCount = palette ? palette->colorCount : 0;
        layout->paletteColorVisible = colorCount;
        for (size_t i = 0; i < colorCount; ++i)
        {
            size_t row = i / PALETTE_COLOR_COLUMNS;
            size_t col = i % PALETTE_COLOR_COLUMNS;
            float x = layout->textX + (float)col * (PALETTE_COLOR_SIZE + PALETTE_COLOR_SPACING);
            float y = gridTop + (float)row * (PALETTE_COLOR_SIZE + PALETTE_COLOR_SPACING);
            layout->paletteColorRects[i] = (Rectangle){ x, y, PALETTE_COLOR_SIZE, PALETTE_COLOR_SIZE };
        }
        if (colorCount > 0)
        {
            size_t lastIndex = colorCount - 1;
            Rectangle lastRect = layout->paletteColorRects[lastIndex];
            layout->contentBottom = lastRect.y + lastRect.height + 32.0f;
        }
        else
        {
            layout->contentBottom = gridTop + 32.0f;
        }
    }
    else
    {
        layout->contentBottom = gridTop + 32.0f;
    }

    float minHeight = layout->contentBottom - layout->panel.y;
    if (minHeight < 320.0f)
    {
        minHeight = 320.0f;
    }
    layout->panel.height = minHeight;
}

static void DrawColorSwatch(Rectangle rect, Color color)
{
    DrawRectangleRec(rect, color);
    DrawRectangleLinesEx(rect, 1.0f, Fade(BLACK, 0.7f));
}

static void DrawChecker(Rectangle rect, float cellSize)
{
    int columns = (int)(rect.width / cellSize) + 2;
    int rows = (int)(rect.height / cellSize) + 2;
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < columns; ++x)
        {
            if (((x + y) & 1) == 0)
            {
                float rx = rect.x + (float)x * cellSize;
                float ry = rect.y + (float)y * cellSize;
                DrawRectangle((int)rx, (int)ry, (int)cellSize, (int)cellSize, Fade(LIGHTGRAY, 0.5f));
            }
        }
    }
}

static void DrawButton(Rectangle rect, const char *label, bool highlighted)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    Color fill = highlighted ? Fade(SKYBLUE, 0.6f) : Fade(LIGHTGRAY, hovered ? 0.45f : 0.3f);
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, 1.0f, Fade(BLACK, 0.6f));
    if (label)
    {
        int textWidth = MeasureText(label, 14);
        int textX = (int)(rect.x + rect.width * 0.5f - textWidth * 0.5f);
        int textY = (int)(rect.y + rect.height * 0.5f - 7);
        DrawText(label, textX, textY, 14, RAYWHITE);
    }
}

static void DrawColorPickerWidget(const UIState *ui, const UILayout *layout)
{
    if (!ui || !layout) return;

    DrawText("Color Picker", layout->textX, (int)(layout->pickerSVRect.y - 22.0f), 16, RAYWHITE);

    Rectangle sv = layout->pickerSVRect;
    Color hueColor = ColorFromHSV01(ui->pickerHue, 1.0f, 1.0f, 1.0f);
    DrawRectangleRec(sv, hueColor);
    DrawRectangleGradientEx(sv, WHITE, Fade(WHITE, 0.0f), Fade(WHITE, 0.0f), WHITE);
    DrawRectangleGradientEx(sv, Fade(BLACK, 0.0f), Fade(BLACK, 0.0f), Fade(BLACK, 0.8f), Fade(BLACK, 0.8f));
    DrawRectangleLinesEx(sv, 1.0f, Fade(BLACK, 0.6f));

    float selectorX = sv.x + ui->pickerSaturation * sv.width;
    float selectorY = sv.y + (1.0f - ui->pickerValue) * sv.height;
    DrawCircleLines((int)selectorX, (int)selectorY, 7.0f, BLACK);
    DrawCircleLines((int)selectorX, (int)selectorY, 6.0f, RAYWHITE);

    Rectangle hueRect = layout->pickerHueRect;
    const int segments = 6;
    float segmentWidth = hueRect.width / (float)segments;
    for (int i = 0; i < segments; ++i)
    {
        float startHue = (float)i / (float)segments;
        float endHue = (float)(i + 1) / (float)segments;
        Color startColor = ColorFromHSV01(startHue, 1.0f, 1.0f, 1.0f);
        Color endColor = ColorFromHSV01(endHue, 1.0f, 1.0f, 1.0f);
        Rectangle segmentRect = { hueRect.x + segmentWidth * (float)i, hueRect.y, segmentWidth + 1.0f, hueRect.height };
        DrawRectangleGradientEx(segmentRect, startColor, endColor, endColor, startColor);
    }
    DrawRectangleLinesEx(hueRect, 1.0f, Fade(BLACK, 0.6f));
    float hueX = hueRect.x + ui->pickerHue * hueRect.width;
    DrawLine((int)hueX, (int)hueRect.y - 1, (int)hueX, (int)(hueRect.y + hueRect.height + 1), WHITE);

    Rectangle alphaRect = layout->pickerAlphaRect;
    DrawChecker(alphaRect, 6.0f);
    Color transparentColor = ColorFromHSV01(ui->pickerHue, ui->pickerSaturation, ui->pickerValue, 0.0f);
    Color opaqueColor = ColorFromHSV01(ui->pickerHue, ui->pickerSaturation, ui->pickerValue, 1.0f);
    DrawRectangleGradientEx(alphaRect, transparentColor, opaqueColor, opaqueColor, transparentColor);
    DrawRectangleLinesEx(alphaRect, 1.0f, Fade(BLACK, 0.6f));
    float alphaX = alphaRect.x + ui->pickerAlpha * alphaRect.width;
    DrawLine((int)alphaX, (int)alphaRect.y - 1, (int)alphaX, (int)(alphaRect.y + alphaRect.height + 1), WHITE);

    DrawText("Hue", layout->textX, (int)(hueRect.y - 18.0f), 14, LIGHTGRAY);
    DrawText("Alpha", layout->textX, (int)(alphaRect.y - 18.0f), 14, LIGHTGRAY);
}

void UIInit(UIState *ui)
{
    if (!ui) return;
    memset(ui, 0, sizeof(*ui));
    ui->gridVisible = true;
    ui->colorPickerVisible = false;
    ui->colorPickerForForeground = true;
    ui->pickerHue = 0.0f;
    ui->pickerSaturation = 0.0f;
    ui->pickerValue = 0.0f;
    ui->pickerAlpha = 1.0f;
    ui->pickerLastColor = BLACK;
    PaletteCollectionInit(&ui->palettes);
    strncpy(ui->paletteFilePath, "palettes/default.pal", sizeof(ui->paletteFilePath));
    ui->paletteFilePath[sizeof(ui->paletteFilePath) - 1] = '\0';
    ui->paletteStatusMessage[0] = '\0';
    ui->paletteStatusTimer = 0;
    PaletteLoadFromFile(&ui->palettes, ui->paletteFilePath);
}

void UIHandleInput(UIState *ui, ToolState *tools)
{
    if (!ui) return;

    if (ui->paletteStatusTimer > 0)
    {
        ui->paletteStatusTimer -= 1;
    }

    if (IsKeyPressed(KEY_G))
    {
        ui->gridVisible = !ui->gridVisible;
    }

    if (!tools) return;

    const ToolDescriptor *descriptors = NULL;
    size_t descriptorCount = ToolsGetDescriptors(&descriptors);
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        for (size_t i = 0; i < descriptorCount; ++i)
        {
            Rectangle buttonRect = GetToolButtonRect(i);
            if (CheckCollisionPointRec(mouse, buttonRect))
            {
                if (tools->activeTool != descriptors[i].type)
                {
                    tools->activeTool = descriptors[i].type;
                    tools->hasDragStart = false;
                    tools->hasLastCursor = false;
                    tools->hasLastPixel = false;
                }
                break;
            }
        }
    }

    UILayout layout = { 0 };
    BuildUILayout(ui, tools, descriptorCount, &layout);

    if (ui->colorPickerVisible)
    {
        Color target = ui->colorPickerForForeground ? tools->foregroundColor : tools->backgroundColor;
        if (!ColorsEqual(target, ui->pickerLastColor))
        {
            SetPickerFromColor(ui, target);
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mouse, layout.fgSwatchRect))
        {
            ui->colorPickerVisible = true;
            ui->colorPickerForForeground = true;
            SetPickerFromColor(ui, tools->foregroundColor);
        }
        else if (CheckCollisionPointRec(mouse, layout.bgSwatchRect))
        {
            ui->colorPickerVisible = true;
            ui->colorPickerForForeground = false;
            SetPickerFromColor(ui, tools->backgroundColor);
        }

        for (size_t i = 0; i < layout.fgHistoryVisible; ++i)
        {
            if (CheckCollisionPointRec(mouse, layout.fgHistoryRects[i]))
            {
                ToolsSetForegroundColor(tools, tools->foregroundHistory.entries[i]);
                if (ui->colorPickerVisible && ui->colorPickerForForeground)
                {
                    SetPickerFromColor(ui, tools->foregroundHistory.entries[i]);
                }
                break;
            }
        }

        for (size_t i = 0; i < layout.bgHistoryVisible; ++i)
        {
            if (CheckCollisionPointRec(mouse, layout.bgHistoryRects[i]))
            {
                ToolsSetBackgroundColor(tools, tools->backgroundHistory.entries[i]);
                if (ui->colorPickerVisible && !ui->colorPickerForForeground)
                {
                    SetPickerFromColor(ui, tools->backgroundHistory.entries[i]);
                }
                break;
            }
        }

        if (ui->colorPickerVisible)
        {
            if (CheckCollisionPointRec(mouse, layout.pickerSVRect))
            {
                ui->pickerDraggingSV = true;
            }
            else if (CheckCollisionPointRec(mouse, layout.pickerHueRect))
            {
                ui->pickerDraggingHue = true;
            }
            else if (CheckCollisionPointRec(mouse, layout.pickerAlphaRect))
            {
                ui->pickerDraggingAlpha = true;
            }
        }

        if (CheckCollisionPointRec(mouse, layout.palettePrevRect))
        {
            PaletteCollectionCycleActive(&ui->palettes, -1);
        }
        else if (CheckCollisionPointRec(mouse, layout.paletteNextRect))
        {
            PaletteCollectionCycleActive(&ui->palettes, 1);
        }
        else if (CheckCollisionPointRec(mouse, layout.paletteNewRect))
        {
            if (!PaletteCollectionCreatePalette(&ui->palettes, NULL))
            {
                SetPaletteStatus(ui, "Maximum palettes reached");
            }
            else
            {
                SetPaletteStatus(ui, "New palette created");
            }
        }
        else if (CheckCollisionPointRec(mouse, layout.paletteAddFGRect))
        {
            const Palette *palette = PaletteCollectionGetActiveConst(&ui->palettes);
            if (palette && palette->colorCount >= MAX_PALETTE_COLORS)
            {
                SetPaletteStatus(ui, "Palette is full");
            }
            else if (PaletteCollectionAddColor(&ui->palettes, tools->foregroundColor))
            {
                SetPaletteStatus(ui, "Foreground added to palette");
            }
            else
            {
                SetPaletteStatus(ui, "Foreground already in palette");
            }
        }
        else if (CheckCollisionPointRec(mouse, layout.paletteAddBGRect))
        {
            const Palette *palette = PaletteCollectionGetActiveConst(&ui->palettes);
            if (palette && palette->colorCount >= MAX_PALETTE_COLORS)
            {
                SetPaletteStatus(ui, "Palette is full");
            }
            else if (PaletteCollectionAddColor(&ui->palettes, tools->backgroundColor))
            {
                SetPaletteStatus(ui, "Background added to palette");
            }
            else
            {
                SetPaletteStatus(ui, "Background already in palette");
            }
        }
        else if (CheckCollisionPointRec(mouse, layout.paletteSaveRect))
        {
            bool success = PaletteSaveToFile(&ui->palettes, ui->paletteFilePath);
            SetPaletteStatus(ui, success ? "Palette saved" : "Failed to save palette");
        }
        else if (CheckCollisionPointRec(mouse, layout.paletteLoadRect))
        {
            bool success = PaletteLoadFromFile(&ui->palettes, ui->paletteFilePath);
            SetPaletteStatus(ui, success ? "Palette loaded" : "Failed to load palette");
        }
        else
        {
            const Palette *palette = PaletteCollectionGetActiveConst(&ui->palettes);
            if (palette)
            {
                for (size_t i = 0; i < palette->colorCount; ++i)
                {
                    if (CheckCollisionPointRec(mouse, layout.paletteColorRects[i]))
                    {
                        ToolsSetForegroundColor(tools, palette->colors[i]);
                        if (ui->colorPickerVisible && ui->colorPickerForForeground)
                        {
                            SetPickerFromColor(ui, palette->colors[i]);
                        }
                        break;
                    }
                }
            }
        }
    }

    if (ui->colorPickerVisible)
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            ui->colorPickerVisible = false;
            ui->pickerDraggingSV = false;
            ui->pickerDraggingHue = false;
            ui->pickerDraggingAlpha = false;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            ui->pickerDraggingSV = false;
            ui->pickerDraggingHue = false;
            ui->pickerDraggingAlpha = false;
        }

        if (ui->pickerDraggingSV || ui->pickerDraggingHue || ui->pickerDraggingAlpha)
        {
            if (ui->pickerDraggingSV)
            {
                float s = (mouse.x - layout.pickerSVRect.x) / layout.pickerSVRect.width;
                float v = 1.0f - (mouse.y - layout.pickerSVRect.y) / layout.pickerSVRect.height;
                if (s < 0.0f) s = 0.0f;
                if (s > 1.0f) s = 1.0f;
                if (v < 0.0f) v = 0.0f;
                if (v > 1.0f) v = 1.0f;
                ui->pickerSaturation = s;
                ui->pickerValue = v;
                ApplyPickerColor(ui, tools);
            }
            if (ui->pickerDraggingHue)
            {
                float h = (mouse.x - layout.pickerHueRect.x) / layout.pickerHueRect.width;
                if (h < 0.0f) h = 0.0f;
                if (h > 1.0f) h = 1.0f;
                ui->pickerHue = h;
                ApplyPickerColor(ui, tools);
            }
            if (ui->pickerDraggingAlpha)
            {
                float a = (mouse.x - layout.pickerAlphaRect.x) / layout.pickerAlphaRect.width;
                if (a < 0.0f) a = 0.0f;
                if (a > 1.0f) a = 1.0f;
                ui->pickerAlpha = a;
                ApplyPickerColor(ui, tools);
            }
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        const Palette *palette = PaletteCollectionGetActiveConst(&ui->palettes);
        if (palette)
        {
            for (size_t i = 0; i < palette->colorCount; ++i)
            {
                if (CheckCollisionPointRec(mouse, layout.paletteColorRects[i]))
                {
                    ToolsSetBackgroundColor(tools, palette->colors[i]);
                    if (ui->colorPickerVisible && !ui->colorPickerForForeground)
                    {
                        SetPickerFromColor(ui, palette->colors[i]);
                    }
                    break;
                }
            }
        }
    }

    if (tools->requestPaletteSave)
    {
        bool success = PaletteSaveToFile(&ui->palettes, ui->paletteFilePath);
        SetPaletteStatus(ui, success ? "Palette saved" : "Failed to save palette");
        tools->requestPaletteSave = false;
    }
    if (tools->requestPaletteLoad)
    {
        bool success = PaletteLoadFromFile(&ui->palettes, ui->paletteFilePath);
        SetPaletteStatus(ui, success ? "Palette loaded" : "Failed to load palette");
        tools->requestPaletteLoad = false;
    }
}

void UIDraw(const UIState *ui, const ToolState *tools, float zoom)
{
    if (!ui || !tools) return;

    const ToolDescriptor *descriptors = NULL;
    size_t descriptorCount = ToolsGetDescriptors(&descriptors);

    UILayout layout = { 0 };
    BuildUILayout(ui, tools, descriptorCount, &layout);

    DrawRectangle((int)layout.panel.x, (int)layout.panel.y, (int)layout.panel.width, (int)layout.panel.height, Fade(DARKGRAY, 0.85f));
    DrawRectangleLines((int)layout.panel.x, (int)layout.panel.y, (int)layout.panel.width, (int)layout.panel.height, Fade(BLACK, 0.6f));

    int textX = layout.textX;
    int textY = PANEL_Y + 12;
    DrawText("Pixel Art Tool", textX, textY, 20, RAYWHITE);

    textY += 28;
    DrawText(TextFormat("Active: %s (%s)", ToolsGetToolName(tools->activeTool), ToolsGetToolShortcutLabel(tools->activeTool)), textX, textY, 16, RAYWHITE);

    textY += 28;
    DrawText("Tools", textX, textY, 18, RAYWHITE);

    Color buttonOutline = Fade(BLACK, 0.5f);
    Vector2 mouse = GetMousePosition();
    for (size_t i = 0; i < descriptorCount; ++i)
    {
        Rectangle buttonRect = GetToolButtonRect(i);
        bool isActive = (tools->activeTool == descriptors[i].type);
        bool isHovered = CheckCollisionPointRec(mouse, buttonRect);

        Color fillColor = isActive ? Fade(SKYBLUE, 0.6f) : Fade(LIGHTGRAY, 0.25f);
        if (!isActive && isHovered) fillColor = Fade(LIGHTGRAY, 0.4f);

        DrawRectangleRec(buttonRect, fillColor);
        DrawRectangleLinesEx(buttonRect, 1.0f, buttonOutline);

        DrawText(TextFormat("%s (%s)", descriptors[i].name, descriptors[i].shortcutLabel), (int)buttonRect.x + 8, (int)buttonRect.y + 5, 16, RAYWHITE);
    }

    int infoY = (int)layout.infoStartY;
    DrawText("Left click: draw/apply", textX, infoY, 14, LIGHTGRAY);
    infoY += 16;
    DrawText("Right click: eyedropper", textX, infoY, 14, LIGHTGRAY);
    infoY += 16;
    DrawText("Shapes: click and drag", textX, infoY, 14, LIGHTGRAY);
    infoY += 16;
    DrawText("Bucket fill: click area", textX, infoY, 14, LIGHTGRAY);
    infoY += 20;
    DrawText(TextFormat("Zoom: %.0f%%", zoom * 100.0f), textX, infoY, 16, RAYWHITE);
    infoY += 20;
    DrawText(TextFormat("Grid: %s (G)", ui->gridVisible ? "On" : "Off"), textX, infoY, 16, RAYWHITE);

    DrawText("Colors", textX, (int)layout.fgSwatchRect.y - 20, 18, RAYWHITE);
    DrawText("FG", (int)layout.fgSwatchRect.x, (int)layout.fgSwatchRect.y - 18, 14, LIGHTGRAY);
    DrawText("BG", (int)layout.bgSwatchRect.x, (int)layout.bgSwatchRect.y - 18, 14, LIGHTGRAY);
    DrawColorSwatch(layout.fgSwatchRect, tools->foregroundColor);
    DrawColorSwatch(layout.bgSwatchRect, tools->backgroundColor);

    DrawText(TextFormat("FG #%02X%02X%02X%02X", tools->foregroundColor.r, tools->foregroundColor.g, tools->foregroundColor.b, tools->foregroundColor.a), textX, (int)(layout.fgSwatchRect.y + COLOR_SWATCH_SIZE + 4.0f), 14, LIGHTGRAY);
    DrawText(TextFormat("BG #%02X%02X%02X%02X", tools->backgroundColor.r, tools->backgroundColor.g, tools->backgroundColor.b, tools->backgroundColor.a), textX, (int)(layout.bgSwatchRect.y + COLOR_SWATCH_SIZE + 4.0f), 14, LIGHTGRAY);

    float fgHistoryLabelY = layout.fgHistoryVisible > 0 ? layout.fgHistoryRects[0].y - 18.0f : layout.fgSwatchRect.y + COLOR_SWATCH_SIZE + 52.0f;
    DrawText("Recent FG", textX, (int)fgHistoryLabelY, 14, LIGHTGRAY);
    for (size_t i = 0; i < layout.fgHistoryVisible; ++i)
    {
        Rectangle rect = layout.fgHistoryRects[i];
        DrawRectangleRec(rect, tools->foregroundHistory.entries[i]);
        Color outline = ColorsEqual(tools->foregroundHistory.entries[i], tools->foregroundColor) ? Fade(SKYBLUE, 0.8f) : Fade(BLACK, 0.6f);
        DrawRectangleLinesEx(rect, 1.0f, outline);
    }

    float bgHistoryLabelY = layout.bgHistoryVisible > 0 ? layout.bgHistoryRects[0].y - 18.0f : layout.bgSwatchRect.y + COLOR_SWATCH_SIZE + 92.0f;
    DrawText("Recent BG", textX, (int)bgHistoryLabelY, 14, LIGHTGRAY);
    for (size_t i = 0; i < layout.bgHistoryVisible; ++i)
    {
        Rectangle rect = layout.bgHistoryRects[i];
        DrawRectangleRec(rect, tools->backgroundHistory.entries[i]);
        Color outline = ColorsEqual(tools->backgroundHistory.entries[i], tools->backgroundColor) ? Fade(SKYBLUE, 0.8f) : Fade(BLACK, 0.6f);
        DrawRectangleLinesEx(rect, 1.0f, outline);
    }

    if (ui->colorPickerVisible)
    {
        DrawColorPickerWidget(ui, &layout);
    }
    else
    {
        DrawText("Click a swatch to edit color", textX, (int)(layout.pickerSVRect.y), 14, LIGHTGRAY);
    }

    DrawText("Palettes", textX, (int)layout.paletteNameY, 18, RAYWHITE);
    DrawButton(layout.palettePrevRect, "<", false);
    DrawButton(layout.paletteNextRect, ">", false);
    const Palette *activePalette = PaletteCollectionGetActiveConst(&ui->palettes);
    const char *paletteName = activePalette ? activePalette->name : "None";
    DrawText(TextFormat("%s", paletteName), (int)layout.paletteNamePos.x, (int)layout.paletteNamePos.y, 16, RAYWHITE);

    DrawButton(layout.paletteNewRect, "New", false);
    DrawButton(layout.paletteAddFGRect, "Add FG", false);
    DrawButton(layout.paletteAddBGRect, "Add BG", false);
    DrawButton(layout.paletteSaveRect, "Save", false);
    DrawButton(layout.paletteLoadRect, "Load", false);

    DrawText(TextFormat("File: %s", ui->paletteFilePath), textX, (int)layout.paletteFileTextY, 14, LIGHTGRAY);
    if (ui->paletteStatusTimer > 0 && ui->paletteStatusMessage[0] != '\0')
    {
        DrawText(ui->paletteStatusMessage, textX, (int)layout.statusTextY, 14, SKYBLUE);
    }

    if (activePalette)
    {
        for (size_t i = 0; i < activePalette->colorCount; ++i)
        {
            Rectangle rect = layout.paletteColorRects[i];
            DrawRectangleRec(rect, activePalette->colors[i]);
            DrawRectangleLinesEx(rect, 1.0f, Fade(BLACK, 0.6f));
        }
    }

    DrawFPS(PANEL_X + 12, (int)(layout.panel.y + layout.panel.height - 24));
}
