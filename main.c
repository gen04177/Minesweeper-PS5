#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 640 // 16 tiles * 32 pixels per tile + panel
#define SCREEN_HEIGHT 512 // 16 tiles * 32 pixels per tile
#define TILE_SIZE 32
#define GRID_WIDTH 16
#define GRID_HEIGHT 16
#define NUM_MINES 40

typedef struct {
    int x, y;
    bool hasMine;
    bool revealed;
    bool flagged;
    int neighboringMines;
} Tile;

Tile grid[GRID_WIDTH][GRID_HEIGHT];
int cursorX = 0, cursorY = 0;
bool gameLost = false;
bool gameWon = false;
bool gameLoading = false;
Uint32 gameEndTime = 0;
Uint32 startTime = 0;
int bestTime = 999;
int remainingMines = NUM_MINES;

void initGrid() {

    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            grid[x][y].x = x;
            grid[x][y].y = y;
            grid[x][y].hasMine = false;
            grid[x][y].revealed = false;
            grid[x][y].flagged = false;
            grid[x][y].neighboringMines = 0;
        }
    }

    int minesPlaced = 0;
    srand(time(NULL));
    while (minesPlaced < NUM_MINES) {
        int x = rand() % GRID_WIDTH;
        int y = rand() % GRID_HEIGHT;
        if (!grid[x][y].hasMine) {
            grid[x][y].hasMine = true;
            minesPlaced++;
        }
    }

    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            if (grid[x][y].hasMine) {
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        int nx = x + dx;
                        int ny = y + dy;
                        if (nx >= 0 && ny >= 0 && nx < GRID_WIDTH && ny < GRID_HEIGHT) {
                            grid[nx][ny].neighboringMines++;
                        }
                    }
                }
            }
        }
    }
    gameLost = false;
    gameWon = false;
    gameLoading = false;
    startTime = SDL_GetTicks();
    remainingMines = NUM_MINES;
}

void renderGrid(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Rect tileRect = {0, 0, TILE_SIZE, TILE_SIZE};
    SDL_Surface *tileSurface;
    SDL_Texture *tileTexture;
    SDL_Color textColor = {255, 255, 255, 255};

    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            tileRect.x = x * TILE_SIZE;
            tileRect.y = y * TILE_SIZE;

            if (grid[x][y].revealed) {
                if (grid[x][y].hasMine) {
                    tileSurface = TTF_RenderText_Solid(font, "M", textColor);
                } else {
                    char number[2];
                    sprintf(number, "%d", grid[x][y].neighboringMines);
                    tileSurface = TTF_RenderText_Solid(font, number, textColor);
                }
            } else if (grid[x][y].flagged) {
                tileSurface = IMG_Load("/data/minesweeper/flag.jpg");
            } else {
                tileSurface = IMG_Load("/data/minesweeper/tile.jpg");
            }
            tileTexture = SDL_CreateTextureFromSurface(renderer, tileSurface);
            SDL_FreeSurface(tileSurface);
            SDL_RenderCopy(renderer, tileTexture, NULL, &tileRect);
            SDL_DestroyTexture(tileTexture);
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < 3; i++) {
	    SDL_Rect cursorRect = {cursorX * TILE_SIZE - i, cursorY * TILE_SIZE - i, TILE_SIZE + 2 * i, TILE_SIZE + 2 * i};
    	    SDL_RenderDrawRect(renderer, &cursorRect);
    }
}

void renderEndGame(SDL_Renderer *renderer, const char *imagePath) {
    SDL_Surface *endSurface = IMG_Load(imagePath);
    SDL_Texture *endTexture = SDL_CreateTextureFromSurface(renderer, endSurface);
    int width = endSurface->w;
    int height = endSurface->h;
    SDL_Rect dstrect = { (SCREEN_WIDTH - width) / 2, 10, width, height };
    SDL_FreeSurface(endSurface);
    SDL_RenderCopy(renderer, endTexture, NULL, &dstrect);
    SDL_DestroyTexture(endTexture);
}

void renderPanel(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color textColor = {255, 255, 255, 255};
    char buffer[128];
    SDL_Surface *textSurface;
    SDL_Texture *textTexture;
    SDL_Rect textRect;

    // Render time
    Uint32 elapsedTime = (SDL_GetTicks() - startTime) / 1000;
    sprintf(buffer, "Time: %d s", elapsedTime);
    textSurface = TTF_RenderText_Solid(font, buffer, textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = (SDL_Rect){SCREEN_WIDTH - 120, 50, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Render best time
    sprintf(buffer, "Best Time:");
    textSurface = TTF_RenderText_Solid(font, buffer, textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect.y += 50;
    textRect.x = SCREEN_WIDTH - 120;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    sprintf(buffer, "%d s", bestTime);
    textSurface = TTF_RenderText_Solid(font, buffer, textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect.y += 30;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Render remaining mines
    sprintf(buffer, "Remaining Mines: %d", remainingMines);
    textSurface = TTF_RenderText_Solid(font, buffer, textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect.y += 50;
    textRect.x = SCREEN_WIDTH - 120;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void revealTile(int x, int y) {
    if (x >= 0 && y >= 0 && x < GRID_WIDTH && y < GRID_HEIGHT && !grid[x][y].revealed && !grid[x][y].flagged) {
        grid[x][y].revealed = true;
        if (grid[x][y].hasMine) {

            gameLost = true;
            gameEndTime = SDL_GetTicks() + 3000;
        } else {
            if (grid[x][y].neighboringMines == 0) {
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        if (dx != 0 || dy != 0) {
                            revealTile(x + dx, y + dy);
                        }
                    }
                }
            }

            bool won = true;
            for (int i = 0; i < GRID_WIDTH; i++) {
                for (int j = 0; j < GRID_HEIGHT; j++) {
                    if (!grid[i][j].revealed && !grid[i][j].hasMine) {
                        won = false;
                        break;
                    }
                }
                if (!won) break;
            }
            if (won) {
                gameWon = true;
                gameEndTime = SDL_GetTicks() + 3000;

		Uint32 elapsedTime = (SDL_GetTicks() - startTime) / 1000;

                if (elapsedTime < bestTime) {
                    bestTime = elapsedTime;
                }
            }
        }
    }
}

void handleControllerInput(SDL_Event *e) {
    if (e->type == SDL_CONTROLLERBUTTONDOWN) {
        switch (e->cbutton.button) {
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                cursorY = (cursorY > 0) ? cursorY - 1 : cursorY;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                cursorY = (cursorY < GRID_HEIGHT - 1) ? cursorY + 1 : cursorY;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                cursorX = (cursorX > 0) ? cursorX - 1 : cursorX;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                cursorX = (cursorX < GRID_WIDTH - 1) ? cursorX + 1 : cursorX;
                break;
            case SDL_CONTROLLER_BUTTON_A:
                revealTile(cursorX, cursorY);
                break;
            case SDL_CONTROLLER_BUTTON_B:
                if (!grid[cursorX][cursorY].revealed) {
                    if (!grid[cursorX][cursorY].flagged && remainingMines > 0) {
                        grid[cursorX][cursorY].flagged = true;
                        remainingMines--;
                    } else if (grid[cursorX][cursorY].flagged) {
                        grid[cursorX][cursorY].flagged = false;
                        remainingMines++;
                    }
                }
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    if (TTF_Init() == -1) {
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    TTF_Font *font = TTF_OpenFont("/data/minesweeper/arial.ttf", 24);
    if (font == NULL) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    SDL_GameController *controller = NULL;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) {
                break;
            }
        }
    }

    if (!controller) {
        printf("No game controller found!\n");
        return -1;
    }

    initGrid();

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else {
                handleControllerInput(&e);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (gameLost) {
            renderEndGame(renderer, "/data/minesweeper/fail.jpg");
        } else if (gameWon) {
            renderEndGame(renderer, "/data/minesweeper/victory.jpg");
        } else {
            renderGrid(renderer, font);
            renderPanel(renderer, font);

        }

        SDL_RenderPresent(renderer);

        if ((gameLost || gameWon) && SDL_GetTicks() > gameEndTime) {

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_Surface *loadingSurface = TTF_RenderText_Solid(font, "Loading game...", (SDL_Color){255, 255, 255, 255});
            SDL_Texture *loadingTexture = SDL_CreateTextureFromSurface(renderer, loadingSurface);
            SDL_Rect loadingRect = { (SCREEN_WIDTH - loadingSurface->w) / 2, (SCREEN_HEIGHT - loadingSurface->h) / 2, loadingSurface->w, loadingSurface->h };
            SDL_FreeSurface(loadingSurface);
            SDL_RenderCopy(renderer, loadingTexture, NULL, &loadingRect);
            SDL_DestroyTexture(loadingTexture);
            SDL_RenderPresent(renderer);

            SDL_Delay(2000);
            initGrid();
        }
    }

    SDL_GameControllerClose(controller);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
