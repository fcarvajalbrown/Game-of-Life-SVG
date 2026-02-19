# Game of Life SVG

A PNG-independent, procedurally rendered Conway's Game of Life compiled to WebAssembly via Emscripten. Cells are drawn as living organisms that morph based on their age — no external assets, no image files.

> **This project was an exercise in learning C++ → WebAssembly → GitHub Pages deployment**, exploring how far a cellular automaton can be pushed visually using only code-generated graphics.

---

## What is Conway's Game of Life?

Invented by mathematician John Conway in 1970, the Game of Life is a zero-player simulation. From four simple rules about birth, survival, and death, complex emergent behavior arises — gliders that travel across the grid, oscillators that pulse forever, and chaotic patterns that stabilize after hundreds of generations.

---

## What makes this version different?

Instead of drawing cells as plain squares, each cell is rendered based on its **age** (how many generations it has survived):

| Stage | Age | Shape | Color |
|-------|-----|-------|-------|
| Infant | 1–5 gen | Pulsing dot | 🟢 Green |
| Adult | 6–20 gen | Polygon (sides = neighbor count) | 🔵 Cyan |
| Ancient | 21+ gen | Polar flower `r = cos(kθ)` | 🟡 Gold |

A CSS **gooey filter** (blur + contrast) makes adjacent cells fuse into organic membranes.

---

## Architecture

### Level 1 — System Context

```mermaid
C4Context
  title System Context — Bio-Digital Game of Life

  Person(user, "User", "Runs simulation in a browser")
  System(gol, "Bio-Digital GoL", "WebAssembly simulation rendered on HTML Canvas")
  System_Ext(ghpages, "GitHub Pages", "Static file hosting")

  Rel(user, gol, "Interacts via browser")
  Rel(ghpages, gol, "Serves index.html, WASM, JS, CSS")

  UpdateRelStyle(user, gol, $offsetY="-20")
  UpdateRelStyle(ghpages, gol, $offsetY="20")
```

---

### Level 2 — Container

```mermaid
C4Container
  title Container — Bio-Digital Game of Life

  Person(user, "User")

  Container(html, "index.html", "HTML", "Emscripten shell, SVG gooey filter, canvas mount")
  Container(uijs, "ui.js", "JavaScript", "Intro screen, parameter panel, WASM bridge")
  Container(uicss, "ui.css", "CSS", "Panel styling, gooey filter classes")
  Container(wasm, "index.wasm", "WebAssembly", "Compiled C++ simulation and renderer")
  Container(js, "index.js", "JavaScript", "Emscripten runtime glue")

  Rel(user, html, "Loads in browser")
  Rel(html, uijs, "Loads at runtime")
  Rel(html, uicss, "Loads at runtime")
  Rel(html, js, "Loads async")
  Rel(uijs, wasm, "Calls gol_init via ccall")
  Rel(js, wasm, "Instantiates WASM module")

  UpdateRelStyle(user, html, $offsetY="-20")
  UpdateRelStyle(uijs, wasm, $offsetX="10", $offsetY="-10")
  UpdateRelStyle(js, wasm, $offsetX="-10", $offsetY="10")
```

---

### Level 3 — Component

```mermaid
C4Component
  title Component — WebAssembly Module (C++)

  Container_Boundary(wasm, "index.wasm") {
    Component(main, "main.cpp", "C++ / Emscripten", "Exports gol_init, owns main loop via emscripten_set_main_loop")
    Component(grid, "Grid", "C++ class", "GoL simulation: step(), randomize(), set(), get()")
    Component(renderer, "Renderer", "C++ class", "Age-based morphology: Infant dot, Adult polygon, Ancient flower")
    Component(sdl, "SDL2 (Emscripten port)", "Library", "Canvas rendering target, event loop")
  }

  Rel(main, grid, "Creates and steps")
  Rel(main, renderer, "Calls drawGrid each frame")
  Rel(renderer, sdl, "Draws via SDL_Renderer")
  Rel(grid, renderer, "Provides Cell state and age")

  UpdateRelStyle(main, grid, $offsetX="-10", $offsetY="-10")
  UpdateRelStyle(main, renderer, $offsetX="10", $offsetY="-10")
  UpdateRelStyle(renderer, sdl, $offsetY="10")
  UpdateRelStyle(grid, renderer, $offsetX="20", $offsetY="0")
```

---

### Level 4 — Code

```mermaid
C4Component
  title Code — Grid Class

  Component(grid_h, "grid.h", "Header", "Declares Cell struct, Grid class interface")
  Component(grid_cpp, "grid.cpp", "Implementation", "randomize(), clear(), set(), step(), countNeighbors()")
  Component(cell, "Cell struct", "Data", "alive: bool, age: uint32_t")
  Component(buffers, "current / next buffers", "std::vector<Cell>", "Double-buffer swap for lock-free generation stepping")

  Rel(grid_h, cell, "Defines")
  Rel(grid_cpp, buffers, "Reads current, writes next, swaps")
  Rel(grid_cpp, cell, "Reads alive and age per cell")

  UpdateRelStyle(grid_h, cell, $offsetY="-10")
  UpdateRelStyle(grid_cpp, buffers, $offsetX="10", $offsetY="10")
  UpdateRelStyle(grid_cpp, cell, $offsetX="-10", $offsetY="10")
```

---

## File Structure

```
gol/
├── src/
│   ├── main.cpp        # Emscripten entry, main loop, gol_init export
│   ├── grid.cpp        # GoL simulation logic
│   └── renderer.cpp    # Age-based morphology drawing
├── include/
│   ├── grid.h
│   └── renderer.h
├── shell.html          # Emscripten HTML shell, SVG gooey filter
├── ui.js               # Intro screen, parameter panel, WASM bridge
├── ui.css              # Styling
├── Makefile
└── index.html          # Build output (gitignored)
```

---

## Building locally

Requires [Emscripten](https://emscripten.org/docs/getting_started/downloads.html).

```bash
# Activate Emscripten environment (Windows)
cd emsdk && .\emsdk_env.bat && cd ..\gol

# Compile
em++ -std=c++17 -O2 -Iinclude \
  src/main.cpp src/grid.cpp src/renderer.cpp \
  -s WASM=1 -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 \
  -s EXPORTED_FUNCTIONS='["_gol_init"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
  --shell-file shell.html -o index.html

# Serve
emrun index.html
```

---

## Deployment

Pushes to `main` automatically build and deploy via GitHub Actions to GitHub Pages.

---

## License

MIT