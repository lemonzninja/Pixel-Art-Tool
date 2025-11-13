## Purpose

Guide AI agents to become productive in a pixel art editor project built with Raylib in C. The project is currently in early foundation phase and will evolve into a modular codebase following the architecture defined in `Plan`.

## Big Picture

**Project Goal**: Build an Aseprite-like pixel art editor with layers, animation, drawing tools, and file I/O.

**Current State**: Foundation phase. Entry point is `src/main.c` with a basic Raylib window and `UpdateDrawFrame()` loop. The planned architecture (see `Plan` file) divides functionality into separate modules: `canvas`, `layer`, `frame`, `tools`, `ui`, `color`, `file_io`, `history`, `camera`, and `selection`.

**Architecture Pattern**: Modular C with immediate-mode update/draw separation—all game-loop logic splits into `UpdateDrawFrame()` called from `main()`. Store application state in global or passed structures; initialize all Raylib systems in `main()` before the loop.

## Key Files & Their Roles

- **`src/main.c`** — Entry point; manages window initialization, main loop, and calls `UpdateDrawFrame()`. Currently minimal; will grow to orchestrate state and module callbacks.
- **`Plan`** — Detailed 16-week development roadmap with phases, data structures, and file layout. Read this before major architectural changes.
- **`README.md`** — High-level project description and planned module breakdown (core, gfx, input, ui, tools, io, platform).
- **`Makefile`** — Raylib example Makefile; compiles `src/*.c` to `obj/*.o`. Key variables: `RAYLIB_PATH`, `BUILD_MODE`, `PROJECT_NAME`.
- **`resources/`** — Asset directory (currently empty); referenced by web builds via `--preload-file resources` in Makefile.

## Build & Run Workflows

**Platform**: Windows (primary). MinGW toolchain via `C:/raylib/w64devkit/bin`. Raylib at `C:/raylib/raylib`.

**VS Code Tasks** (preferred):
- `build debug` — `mingw32-make.exe BUILD_MODE=DEBUG` (symbols, no optimization)
- `build release` — `mingw32-make.exe BUILD_MODE=RELEASE` (optimized, stripped)

**Manual (PowerShell)**:
```powershell
mingw32-make.exe RAYLIB_PATH=C:/raylib/raylib BUILD_MODE=DEBUG
./pixel_art_tool.exe  # Run after build
```

**Notes**:
- Makefile auto-discovers `src/*.c` files via recursive wildcard; new modules are picked up automatically.
- `PROJECT_NAME` in Makefile defaults to `pixel_art_tool`; output binary is `pixel_art_tool.exe` on Windows.
- Debug builds include `-g -O0`; release builds use `-s -O1` for smaller binaries.

## Codebase Conventions

1. **Module Structure**: Place new `.c/.h` pairs in `src/`. Follow naming: `module_name.c` / `module_name.h`.
2. **Update/Draw Split**: Use static functions inside modules called from `UpdateDrawFrame()`. Keep input handling separate from rendering.
3. **Global State**: Core state (canvas, current tool, layers, etc.) lives in a central `Project` or `AppState` struct (see `Plan` for proposed structure). Pass as needed to module functions.
4. **Header Guards**: Use `#ifndef MODULE_H / #define MODULE_H / ... / #endif` pattern.
5. **Raylib Conventions**:
   - Use `Color` type from raylib for color (includes alpha).
   - Use `Vector2` for positions; `Rectangle` for bounds.
   - Camera/viewport managed separately (planned `camera.c` module).
   - Prefer `DrawRectangle()` for debug; use `RenderTexture2D` for optimized canvas rendering (Phase 9 optimization).

## Integration Points & Dependencies

- **raylib**: Linked via `LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm` (Windows). Headers at `$(RAYLIB_PATH)/src`.
- **External libraries**: stb headers bundled in Raylib (e.g., stb_image for PNG). Include via Raylib.
- **C Standard**: C99 (`-std=c99` in CFLAGS). Avoid C11-only features.

## Practical Editing Rules

1. **Add a new module** (e.g., `tools.c`):
   - Create `src/tools.c` and `include/tools.h`.
   - Define structures and functions in header; implement in `.c`.
   - Include `raylib.h` in `.c` file if using Raylib types.
   - Call module functions from `UpdateDrawFrame()` or other modules.

2. **Modify build behavior**:
   - Edit `Makefile` variables (`CFLAGS`, `LDLIBS`, `RAYLIB_PATH`) directly.
   - Do not hardcode paths in source or generate compiler commands in .c files.

3. **Add assets** (icons, fonts):
   - Place in `resources/` directory.
   - Reference as `"resources/filename"` in code.
   - Makefile web build automatically includes `--preload-file resources`.

4. **Update `main.c`**:
   - Keep initialization (window, Raylib systems) in `main()`.
   - Grow `UpdateDrawFrame()` only if coordinating module calls; delegate logic to module functions.

## Phases & Milestones (from Plan)

- **Phase 1–2** : Canvas & basic drawing tools
- **Phase 3–4** : UI system & layers
- **Phase 5–6** : Undo/redo & animation
- **Phase 7–8** : File I/O & advanced tools
- **Phase 9**   : Optimization & polish

Current focus: Phase 1 (canvas system, basic pixel rendering, camera/zoom).
