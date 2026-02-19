#include "grid.h"
#include <cstdlib>
#include <ctime>

Grid::Grid(int width, int height)
    : w(width), h(height),
      current(width * height, {false, 0}), // all cells start dead
      next(width * height, {false, 0})      // buffer for next generation
{}

// Randomly seeds the grid based on a density probability (0.0 - 1.0)
void Grid::randomize(float density) {
    srand((unsigned)time(nullptr));
    for (auto& cell : current) {
        cell.alive = ((float)rand() / RAND_MAX) < density;
        cell.age   = 0;
    }
}

// Kills all cells — used before placing a named pattern
void Grid::clear() {
    for (auto& cell : current)
        cell = {false, 0};
}

// Sets a single cell alive or dead — used by pattern placement in main.cpp
// Silently ignores out-of-bounds coordinates
void Grid::set(int x, int y, bool alive) {
    if (x < 0 || x >= w || y < 0 || y >= h) return;
    current[index(x, y)] = {alive, 0};
}

// Counts living neighbors using toroidal (wrap-around) topology
// so cells at edges treat the opposite edge as adjacent
int Grid::countNeighbors(int x, int y) const {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue; // skip self
            int nx = (x + dx + w) % w;        // wrap x
            int ny = (y + dy + h) % h;        // wrap y
            if (current[index(nx, ny)].alive) count++;
        }
    }
    return count;
}

// Advances the simulation by one generation using standard GoL rules:
//   - Live cell with 2 or 3 neighbors survives
//   - Dead cell with exactly 3 neighbors is born
//   - All other cells die or stay dead
// Age increments each generation a cell stays alive, resets on death
void Grid::step() {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int      n     = countNeighbors(x, y);
            bool     alive = current[index(x, y)].alive;
            uint32_t age   = current[index(x, y)].age;

            bool nextAlive = alive
                ? (n == 2 || n == 3)  // survival rule
                : (n == 3);           // birth rule

            next[index(x, y)] = {
                nextAlive,
                nextAlive ? age + 1 : 0  // age up or reset
            };
        }
    }
    // Swap buffers — next becomes current, avoiding extra allocation
    std::swap(current, next);
}

// Returns a copy of the cell at (x, y) for read-only access by the renderer
Cell Grid::get(int x, int y) const {
    return current[index(x, y)];
}