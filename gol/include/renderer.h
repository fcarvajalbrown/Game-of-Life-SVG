#pragma once

#include <SDL2/SDL.h>
#include "grid.h"

enum class CellStage {
    Infant,  // age 1-5:  vibrating dot
    Adult,   // age 6-20: polygon with n sides = neighbor count
    Ancient  // age 21+:  polar flower r = cos(k*theta)
};

CellStage getStage(uint32_t age);

class Renderer {
public:
    Renderer(SDL_Renderer* sdl, int cellSize);

    void drawGrid(const Grid& grid);

private:
    SDL_Renderer* r;
    int cellSize;

    void drawInfant (SDL_Renderer* r, int px, int py, uint32_t age);
    void drawAdult  (SDL_Renderer* r, int px, int py, int neighbors);
    void drawAncient(SDL_Renderer* r, int px, int py, uint32_t age);

    void drawFilledCircle  (SDL_Renderer* r, int cx, int cy, int radius);
    void drawPolygon       (SDL_Renderer* r, int cx, int cy, int radius, int sides);
    void drawPolarFlower   (SDL_Renderer* r, int cx, int cy, int radius, int k);
};