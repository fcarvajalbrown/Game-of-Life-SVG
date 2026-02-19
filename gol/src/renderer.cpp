#include "renderer.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ─── Stage Classification ────────────────────────────────────────────────────

CellStage getStage(uint32_t age) {
    if (age <= 5)  return CellStage::Infant;
    if (age <= 20) return CellStage::Adult;
    return CellStage::Ancient;
}

// ─── Constructor ─────────────────────────────────────────────────────────────

Renderer::Renderer(SDL_Renderer* sdl, int cellSize)
    : r(sdl), cellSize(cellSize)
{}

// ─── Main Draw Pass ──────────────────────────────────────────────────────────

void Renderer::drawGrid(const Grid& grid) {
    // Clear with dark background
    SDL_SetRenderDrawColor(r, 10, 10, 15, 255);
    SDL_RenderClear(r);

    for (int y = 0; y < grid.height(); y++) {
        for (int x = 0; x < grid.width(); x++) {
            Cell cell = grid.get(x, y);
            if (!cell.alive) continue;

            // Pixel position of cell center
            int px = x * cellSize + cellSize / 2;
            int py = y * cellSize + cellSize / 2;

            // Count living neighbors for Adult polygon sides
            int neighbors = 0;
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = (x + dx + grid.width())  % grid.width();
                    int ny = (y + dy + grid.height()) % grid.height();
                    if (grid.get(nx, ny).alive) neighbors++;
                }

            switch (getStage(cell.age)) {
                case CellStage::Infant:  drawInfant (r, px, py, cell.age);  break;
                case CellStage::Adult:   drawAdult  (r, px, py, neighbors); break;
                case CellStage::Ancient: drawAncient(r, px, py, cell.age);  break;
            }
        }
    }

    SDL_RenderPresent(r);
}

// ─── Infant: vibrating dot ───────────────────────────────────────────────────
// Radius oscillates slightly based on age to simulate a pulse

void Renderer::drawInfant(SDL_Renderer* r, int px, int py, uint32_t age) {
    // Soft green, fades in with age
    Uint8 alpha = (Uint8)(50 + age * 40);
    SDL_SetRenderDrawColor(r, 80, 220, 120, alpha);

    // Radius vibrates: base + small sine wobble keyed to age
    int radius = cellSize / 4 + (int)(1.5f * sinf((float)age * 1.2f));
    drawFilledCircle(r, px, py, radius);
}

// ─── Adult: polygon with n sides = neighbor count ────────────────────────────
// More neighbors → more complex shape

void Renderer::drawAdult(SDL_Renderer* r, int px, int py, int neighbors) {
    // Clamp sides between 3 (triangle) and 8 (octagon)
    int sides = std::max(3, std::min(neighbors, 8));

    // Cyan-blue palette
    SDL_SetRenderDrawColor(r, 100, 180, 255, 220);

    int radius = (cellSize / 2) - 1;
    drawPolygon(r, px, py, radius, sides);
}

// ─── Ancient: polar flower r = cos(k * theta) ────────────────────────────────
// k is derived from age so the flower complexity grows over time

void Renderer::drawAncient(SDL_Renderer* r, int px, int py, uint32_t age) {
    // Gold/amber color for ancient cells
    SDL_SetRenderDrawColor(r, 255, 200, 60, 255);

    // k cycles through 2-6 petals based on age
    int k = 2 + (int)(age / 10) % 5;
    int radius = (cellSize / 2) - 1;
    drawPolarFlower(r, px, py, radius, k);
}

// ─── Primitives ──────────────────────────────────────────────────────────────

// Midpoint circle algorithm for filled circle
void Renderer::drawFilledCircle(SDL_Renderer* r, int cx, int cy, int radius) {
    for (int dy = -radius; dy <= radius; dy++) {
        int dx = (int)sqrtf((float)(radius * radius - dy * dy));
        SDL_RenderDrawLine(r, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

// Regular polygon centered at (cx, cy) with given sides and radius
void Renderer::drawPolygon(SDL_Renderer* r, int cx, int cy, int radius, int sides) {
    float step = 2.0f * M_PI / sides;
    float offset = -M_PI / 2.0f; // start pointing up

    for (int i = 0; i < sides; i++) {
        float a1 = offset + i * step;
        float a2 = offset + (i + 1) * step;

        int x1 = cx + (int)(radius * cosf(a1));
        int y1 = cy + (int)(radius * sinf(a1));
        int x2 = cx + (int)(radius * cosf(a2));
        int y2 = cy + (int)(radius * sinf(a2));

        SDL_RenderDrawLine(r, x1, y1, x2, y2);
    }
}

// Polar rose curve: r = cos(k * theta), sampled and drawn as connected lines
void Renderer::drawPolarFlower(SDL_Renderer* r, int cx, int cy, int radius, int k) {
    const int steps = 256;
    float prevX = 0, prevY = 0;

    for (int i = 0; i <= steps; i++) {
        float theta = 2.0f * M_PI * i / steps;
        float rr    = radius * fabsf(cosf(k * theta)); // abs for full petals

        float x = cx + rr * cosf(theta);
        float y = cy + rr * sinf(theta);

        if (i > 0)
            SDL_RenderDrawLine(r, (int)prevX, (int)prevY, (int)x, (int)y);

        prevX = x;
        prevY = y;
    }
}