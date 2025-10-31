#ifndef PALETTE_H
#define PALETTE_H

#include "raylib.h"

#include <stdbool.h>
#include <stddef.h>

#define MAX_PALETTE_NAME_LENGTH 64
#define MAX_PALETTE_COLORS 64
#define MAX_PALETTES 8
#define MAX_PALETTE_PATH_LENGTH 260

typedef struct Palette {
    char name[MAX_PALETTE_NAME_LENGTH];
    Color colors[MAX_PALETTE_COLORS];
    size_t colorCount;
} Palette;

typedef struct PaletteCollection {
    Palette palettes[MAX_PALETTES];
    size_t paletteCount;
    int activePaletteIndex;
} PaletteCollection;

void PaletteCollectionInit(PaletteCollection *collection);
Palette *PaletteCollectionGetActive(PaletteCollection *collection);
const Palette *PaletteCollectionGetActiveConst(const PaletteCollection *collection);
bool PaletteCollectionSetActive(PaletteCollection *collection, int index);
bool PaletteCollectionCycleActive(PaletteCollection *collection, int direction);
bool PaletteCollectionCreatePalette(PaletteCollection *collection, const char *name);
bool PaletteCollectionAddColor(PaletteCollection *collection, Color color);
void PaletteCollectionClear(PaletteCollection *collection);

bool PaletteSaveToFile(const PaletteCollection *collection, const char *filePath);
bool PaletteLoadFromFile(PaletteCollection *collection, const char *filePath);

#endif // PALETTE_H
