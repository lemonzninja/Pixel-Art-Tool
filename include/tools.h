#ifndef TOOLS_H
#define TOOLS_H

#include <stdbool.h>

#include "app_state.h"

void HandleToolShortcuts(AppState *state);
bool ApplyTool(AppState *state, int canvasX, int canvasY);

#endif // TOOLS_H
