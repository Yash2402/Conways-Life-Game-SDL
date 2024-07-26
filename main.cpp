#include <iostream>
#include <vector>
#include <random>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define TITLE             "Game Of Life"
#define WIDTH             1200
#define HEIGHT            1200
#define FPS               20

int aliveNeighbors(const std::vector<std::vector<int>>& scene, int i, int j);

int main(int argc, char *argv[]){

    double period = (1.0 / (double)FPS) * 1000;
    int milliPeriod = (int)period;
    int sleep;

    SDL_Init(SDL_INIT_EVERYTHING); // Initialize SDL

    SDL_Window* window = SDL_CreateWindow(TITLE, 0, 0, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event e;

    Uint32 lastTick;
    Uint32 currentTick;
    
    std::random_device device;
    std::uniform_int_distribution<int> dist(0, 1);

    float cell_size = 20.0f;
    int rows = (int)WIDTH / cell_size - 2;
    int cols = (int)HEIGHT / cell_size - 2;

    std::vector<std::vector<int>> scene(rows, std::vector<int>(cols));

    // Random initialization of the scene
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            scene[i][j] = dist(device);
        }
    }

    bool running = true;
    bool update = false;

    while (running) {
        lastTick = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN){
                if (e.key.keysym.sym == SDLK_SPACE) update = !update;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                mouseX = (mouseX - cell_size)/(int)cell_size;
                mouseY = (mouseY - cell_size)/(int)cell_size;
                scene[mouseX][mouseY] = 1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 100);
        SDL_RenderClear(renderer);

        // Draw grid
        for (int i = 1; i < (int)WIDTH / cell_size; i++) {
            lineRGBA(renderer, cell_size, cell_size * i, WIDTH - cell_size, cell_size * i, 110, 110, 110, 50);
            lineRGBA(renderer, cell_size * i, cell_size, cell_size * i, HEIGHT - cell_size, 110, 110, 110, 50);
        }
        roundedRectangleRGBA(renderer, 0, 0, WIDTH - 2, HEIGHT - 2, 16, 200, 200, 200, 255);

        // Update scene according to Conway's Game of Life rules
        if (update){
            std::vector<std::vector<int>> newScene = scene;

            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    int aliveNeigh = aliveNeighbors(scene, i, j);
                    if (scene[i][j] == 1) {
                        if (aliveNeigh < 2 || aliveNeigh > 3) {
                            newScene[i][j] = 0; // Cell dies
                        }
                    } else {
                        if (aliveNeigh == 3) {
                            newScene[i][j] = 1; // Cell becomes alive
                        }
                    }
                }
            }

            scene = newScene;
        }
        // Draw cells
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                SDL_SetRenderDrawColor(renderer, scene[i][j] * 255, scene[i][j] * 255, scene[i][j] * 255, scene[i][j] * 155);
                SDL_FRect cell = {cell_size + cell_size * i + 1, cell_size + cell_size * j + 1, cell_size - 2, cell_size - 2};
                SDL_RenderFillRectF(renderer, &cell);
            }
        }

        SDL_RenderPresent(renderer);

        currentTick = SDL_GetTicks();
        sleep = milliPeriod - (currentTick - lastTick);
        if (sleep < 0) sleep = 0;
        SDL_Delay(sleep);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int aliveNeighbors(const std::vector<std::vector<int>>& scene, int i, int j) {
    int aliveNeighbour = 0;
    for (int a = -1; a <= 1; a++) {
        for (int b = -1; b <= 1; b++) {
            Uint32 ni = i + a;
            Uint32 nj = j + b;

            // Boundary check
            if (ni >= 0 && ni < scene.size() && nj >= 0 && nj < scene[0].size() && !(a == 0 && b == 0)) {
                aliveNeighbour += scene[ni][nj];
            }
        }
    }
    return aliveNeighbour;
}

