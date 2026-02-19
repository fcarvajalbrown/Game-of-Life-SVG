#pragma once

#include <vector>
#include <cstdint>

struct Cell {
    bool alive;
    uint32_t age; // generations survived
};

class Grid {
public:
    Grid(int width, int height);

    void randomize(float density = 0.3f);
    void clear();                          // kills all cells
    void set(int x, int y, bool alive);   // sets a single cell
    void step();

    Cell get(int x, int y) const;
    int width() const { return w; }
    int height() const { return h; }

private:
    int w, h;
    std::vector<Cell> current;
    std::vector<Cell> next;

    int index(int x, int y) const { return y * w + x; }
    int countNeighbors(int x, int y) const;
};