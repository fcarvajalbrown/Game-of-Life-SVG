#include <SDL2/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "grid.h"
#include "renderer.h"

// ─── Globals ─────────────────────────────────────────────────────────────────
// Kept global so the Emscripten main loop callback can access them

static SDL_Window*   window   = nullptr;
static SDL_Renderer* sdlr     = nullptr;
static Grid*         grid     = nullptr;
static Renderer*     renderer = nullptr;

static int   simSpeed = 100; // ms between steps
static Uint32 lastStep = 0;

// ─── Starter Patterns ────────────────────────────────────────────────────────

static void placeGlider(Grid& g) {
    g.clear();
    // Classic glider centered near top-left
    int cx = 2, cy = 2;
    g.set(cx+1, cy+0, true);
    g.set(cx+2, cy+1, true);
    g.set(cx+0, cy+2, true);
    g.set(cx+1, cy+2, true);
    g.set(cx+2, cy+2, true);
}

static void placeBlinker(Grid& g) {
    g.clear();
    // Horizontal blinker centered in grid
    int cx = g.width() / 2, cy = g.height() / 2;
    g.set(cx-1, cy, true);
    g.set(cx,   cy, true);
    g.set(cx+1, cy, true);
}

static void placeRPentomino(Grid& g) {
    g.clear();
    // R-pentomino centered in grid — chaotic long-lived pattern
    int cx = g.width() / 2, cy = g.height() / 2;
    g.set(cx+1, cy-1, true);
    g.set(cx+2, cy-1, true);
    g.set(cx+0, cy+0, true);
    g.set(cx+1, cy+0, true);
    g.set(cx+1, cy+1, true);
}

// Seeds the grid based on the pattern index from ui.js:
//   0 = Random, 1 = Glider, 2 = Blinker, 3 = R-Pentomino
static void seedPattern(Grid& g, float density, int pattern) {
    switch (pattern) {
        case 1:  placeGlider(g);    break;
        case 2:  placeBlinker(g);   break;
        case 3:  placeRPentomino(g);break;
        default: g.randomize(density); break;
    }
}

// ─── Main Loop ───────────────────────────────────────────────────────────────
// Emscripten requires a flat callback — no while(true) or the tab freezes

static void mainLoop() {
    Uint32 now = SDL_GetTicks();

    // Advance simulation at the configured speed
    if (now - lastStep >= (Uint32)simSpeed) {
        grid->step();
        lastStep = now;
    }

    // Draw every frame regardless of sim speed
    renderer->drawGrid(*grid);
}

// ─── WASM Export ─────────────────────────────────────────────────────────────
// Called from ui.js via Module.ccall('gol_init', ...) after the user hits Start

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void gol_init(int gridSize, float density, int speed, int pattern) {
        simSpeed = speed;

        // Determine cell size so the grid fills a fixed 800x800 canvas
        int canvasSize = 800;
        int cellSize   = canvasSize / gridSize;

        // Init SDL window and renderer targeting the HTML canvas
        SDL_Init(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer(canvasSize, canvasSize, 0, &window, &sdlr);

        // Allocate simulation and renderer
        grid     = new Grid(gridSize, gridSize);
        renderer = new Renderer(sdlr, cellSize);

        seedPattern(*grid, density, pattern);

        lastStep = SDL_GetTicks();

        // Hand control to Emscripten's scheduler — 0 fps = run as fast as possible
        emscripten_set_main_loop(mainLoop, 0, 1);
    }
}