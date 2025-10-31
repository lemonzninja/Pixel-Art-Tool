## Purpose

This short guide helps AI coding agents become productive in this single-file Raylib C project.
It focuses on the concrete build, run, and code patterns discovered in the repository so suggestions and edits are accurate.

## Big picture

- Entry point: `main.c` — a single-file Raylib example using a `Camera`, `UpdateDrawFrame()` and the standard Raylib main loop. Treat `main.c` as the primary surface for changes.
- Build system: top-level `Makefile` (Raylib example Makefile). The project follows the Raylib example layout (sources under `src/`, object files under `obj/`, `resources/` for assets).

## Key files to reference

- `main.c` — runtime logic and examples of Raylib usage (camera, drawing, Update/Draw split).
- `Makefile` — important variables and conventions (PROJECT_NAME, RAYLIB_PATH, OBJS, BUILD_MODE, CC, MAKE). Read this before changing build behavior.
- `resources/` — bundled assets; used by HTML5 builds (see Makefile `--preload-file resources`).

## Concrete build & debug workflows

- Primary platform in workspace: Windows (Makefile sets `MAKE = mingw32-make` and `COMPILER_PATH` pointing to `C:/raylib/w64devkit/bin`).
- VS Code tasks available (use Task Runner):
  - `build debug` — runs `mingw32-make.exe` with `BUILD_MODE=DEBUG`. (Task args set RAYLIB_PATH=C:/raylib/raylib)
  - `build release` — runs `mingw32-make.exe` with default `BUILD_MODE`.

Examples (Windows PowerShell):

    mingw32-make.exe RAYLIB_PATH=C:/raylib/raylib PROJECT_NAME=main OBJS=main.c BUILD_MODE=DEBUG

Notes:
- `PROJECT_NAME` selects the target binary name; `OBJS` is computed from sources (Makefile uses patterns and `src/` directory by default). The tasks in this workspace pass `PROJECT_NAME=${fileBasenameNoExtension}` and `OBJS=${fileBasenameNoExtension}.c`.
- If you change build flags, prefer adjusting variables in the `Makefile` rather than hardcoding compiler flags in generated edits.

## Codebase conventions & patterns

- Source layout: sources expected under `src/` (Makefile compiles `src/*.c` into `obj/`), but the workspace presently contains a top-level `main.c` — maintain the top-level file's structure when editing.
- Use Raylib patterns shown in `main.c`: split update/draw into a static `UpdateDrawFrame()`; maintain the `camera` global and `cubePosition` pattern when adding more scenes or input handling.
- Keep resource references relative to `resources/` — the Makefile references `--preload-file resources` for web builds; avoid hardcoding absolute paths to assets.

## Integration points & external dependencies

- raylib: The Makefile expects a local raylib copy (controlled via `RAYLIB_PATH`) or an installed library. Default tasks set `RAYLIB_PATH=C:/raylib/raylib` on Windows.
- Compiler/toolchain: On Windows the Makefile expects MinGW (mingw32-make) and a compiler in `COMPILER_PATH` (default `C:/raylib/w64devkit/bin`).

## Example edits an AI should perform (concrete, safe rules)

1. When adding a new C source file:
   - Place it in `src/` and reference only functions/headers needed.
   - Let the `Makefile` pattern pick it up (it uses a recursive wildcard to find `*.c`).
2. When changing build flags or libraries:
   - Prefer editing `Makefile` variables like `CFLAGS`, `LDLIBS`, `RAYLIB_PATH`, `BUILD_MODE`.
   - Avoid editing generated compiler invocations directly in commits.
3. When adding assets:
   - Put assets into `resources/` and ensure web builds include `--preload-file resources` (already present in Makefile for web).

## What I did and what I did NOT change

- Created this file because no repo-level agent instructions or README guidance were found.
- I did not modify source or Makefile contents.

## If unclear or missing

- Tell me which workflows you'd like added (e.g., step-by-step debug with GDB/MinGW, CI build matrix, or cross-platform instructions). I can extend these notes with examples and test commands.

---
Reference: read `main.c` and `Makefile` in repository root for examples used above.
