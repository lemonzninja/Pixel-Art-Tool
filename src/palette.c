#include "palette.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MKDIR(path) mkdir(path, 0755)
#endif

static void TrimNewline(char *line)
{
    if (!line) return;
    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
    {
        line[len - 1] = '\0';
        --len;
    }
}

static void EnsureDirectoryForPath(const char *filePath)
{
    if (!filePath) return;

    char buffer[MAX_PALETTE_PATH_LENGTH];
    strncpy(buffer, filePath, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *lastSlash = strrchr(buffer, '/');
#if defined(_WIN32)
    char *lastBackslash = strrchr(buffer, '\\');
    if (!lastSlash || (lastBackslash && lastBackslash > lastSlash))
    {
        lastSlash = lastBackslash;
    }
#endif

    if (lastSlash)
    {
        *lastSlash = '\0';
        if (buffer[0] != '\0')
        {
            MKDIR(buffer);
        }
    }
}

void PaletteCollectionInit(PaletteCollection *collection)
{
    if (!collection) return;
    memset(collection, 0, sizeof(*collection));
    collection->paletteCount = 1;
    collection->activePaletteIndex = 0;
    strncpy(collection->palettes[0].name, "Default", MAX_PALETTE_NAME_LENGTH);
    collection->palettes[0].name[MAX_PALETTE_NAME_LENGTH - 1] = '\0';
    collection->palettes[0].colorCount = 0;
}

Palette *PaletteCollectionGetActive(PaletteCollection *collection)
{
    if (!collection) return NULL;
    if (collection->paletteCount == 0) return NULL;
    if (collection->activePaletteIndex < 0 || collection->activePaletteIndex >= (int)collection->paletteCount)
    {
        collection->activePaletteIndex = 0;
    }
    return &collection->palettes[collection->activePaletteIndex];
}

const Palette *PaletteCollectionGetActiveConst(const PaletteCollection *collection)
{
    return PaletteCollectionGetActive((PaletteCollection *)collection);
}

bool PaletteCollectionSetActive(PaletteCollection *collection, int index)
{
    if (!collection) return false;
    if (index < 0 || index >= (int)collection->paletteCount) return false;
    collection->activePaletteIndex = index;
    return true;
}

bool PaletteCollectionCycleActive(PaletteCollection *collection, int direction)
{
    if (!collection || collection->paletteCount == 0) return false;
    int count = (int)collection->paletteCount;
    int index = collection->activePaletteIndex + direction;
    while (index < 0) index += count;
    while (index >= count) index -= count;
    if (index == collection->activePaletteIndex) return false;
    collection->activePaletteIndex = index;
    return true;
}

bool PaletteCollectionCreatePalette(PaletteCollection *collection, const char *name)
{
    if (!collection) return false;
    if (collection->paletteCount >= MAX_PALETTES) return false;
    Palette *palette = &collection->palettes[collection->paletteCount];
    memset(palette, 0, sizeof(*palette));
    if (name && name[0] != '\0')
    {
        strncpy(palette->name, name, MAX_PALETTE_NAME_LENGTH);
    }
    else
    {
        snprintf(palette->name, MAX_PALETTE_NAME_LENGTH, "Palette %zu", collection->paletteCount + 1);
    }
    palette->name[MAX_PALETTE_NAME_LENGTH - 1] = '\0';
    palette->colorCount = 0;
    collection->activePaletteIndex = (int)collection->paletteCount;
    collection->paletteCount += 1;
    return true;
}

bool PaletteCollectionAddColor(PaletteCollection *collection, Color color)
{
    if (!collection) return false;
    Palette *palette = PaletteCollectionGetActive(collection);
    if (!palette) return false;
    if (palette->colorCount >= MAX_PALETTE_COLORS) return false;

    for (size_t i = 0; i < palette->colorCount; ++i)
    {
        Color existing = palette->colors[i];
        if (existing.r == color.r && existing.g == color.g && existing.b == color.b && existing.a == color.a)
        {
            return false;
        }
    }

    palette->colors[palette->colorCount++] = color;
    return true;
}

void PaletteCollectionClear(PaletteCollection *collection)
{
    if (!collection) return;
    for (size_t i = 0; i < collection->paletteCount; ++i)
    {
        collection->palettes[i].colorCount = 0;
    }
}

bool PaletteSaveToFile(const PaletteCollection *collection, const char *filePath)
{
    if (!collection || !filePath || filePath[0] == '\0') return false;
    EnsureDirectoryForPath(filePath);

    FILE *file = fopen(filePath, "w");
    if (!file) return false;

    fprintf(file, "# Pixel Art Tool palette file\n");
    fprintf(file, "# Format:\n# palette <name>\n# color r g b a\n");

    for (size_t i = 0; i < collection->paletteCount; ++i)
    {
        const Palette *palette = &collection->palettes[i];
        if (palette->name[0] == '\0') continue;
        fprintf(file, "palette %s\n", palette->name);
        for (size_t c = 0; c < palette->colorCount; ++c)
        {
            Color color = palette->colors[c];
            fprintf(file, "color %u %u %u %u\n", color.r, color.g, color.b, color.a);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return true;
}

static bool ParseColorLine(const char *line, Color *outColor)
{
    if (!line || !outColor) return false;
    unsigned int r = 0, g = 0, b = 0, a = 255;
    int parsed = sscanf(line, "%u %u %u %u", &r, &g, &b, &a);
    if (parsed < 3) return false;
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    if (a > 255) a = 255;
    outColor->r = (unsigned char)r;
    outColor->g = (unsigned char)g;
    outColor->b = (unsigned char)b;
    outColor->a = (unsigned char)a;
    return true;
}

bool PaletteLoadFromFile(PaletteCollection *collection, const char *filePath)
{
    if (!collection || !filePath || filePath[0] == '\0') return false;

    FILE *file = fopen(filePath, "r");
    if (!file) return false;

    PaletteCollectionClear(collection);
    collection->paletteCount = 0;
    collection->activePaletteIndex = 0;

    char line[256];
    Palette *current = NULL;

    while (fgets(line, sizeof(line), file))
    {
        TrimNewline(line);
        const char *trimmed = line;
        while (*trimmed && isspace((unsigned char)*trimmed)) ++trimmed;
        if (*trimmed == '\0' || *trimmed == '#') continue;

        if (strncmp(trimmed, "palette", 7) == 0 && isspace((unsigned char)trimmed[7]))
        {
            trimmed += 7;
            while (*trimmed && isspace((unsigned char)*trimmed)) ++trimmed;
            if (collection->paletteCount >= MAX_PALETTES) continue;
            PaletteCollectionCreatePalette(collection, trimmed);
            current = PaletteCollectionGetActive(collection);
            continue;
        }

        if (strncmp(trimmed, "color", 5) == 0 && isspace((unsigned char)trimmed[5]))
        {
            if (!current)
            {
                if (collection->paletteCount == 0)
                {
                    PaletteCollectionCreatePalette(collection, "Loaded");
                    current = PaletteCollectionGetActive(collection);
                }
                else
                {
                    current = PaletteCollectionGetActive(collection);
                }
            }

            trimmed += 5;
            while (*trimmed && isspace((unsigned char)*trimmed)) ++trimmed;
            Color parsed = { 0 };
            if (ParseColorLine(trimmed, &parsed))
            {
                PaletteCollectionAddColor(collection, parsed);
            }
            continue;
        }
    }

    fclose(file);

    if (collection->paletteCount == 0)
    {
        PaletteCollectionInit(collection);
    }
    else
    {
        PaletteCollectionSetActive(collection, 0);
    }

    return true;
}
