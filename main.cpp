#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#define MAX(a,b) \
({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SQ_SIDE = 20;

int main(int argc, char* args[]) {
    
    // Setup
    
    {
        SDL_version compiled;
        SDL_version linked;
        
        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        
        printf("Compiled SDL: %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
        printf("Linked SDL: %d.%d.%d\n", linked.major, linked.minor, linked.patch);
        
        const SDL_version *ttf_linked = TTF_Linked_Version();
        SDL_TTF_VERSION(&compiled);
        
        printf("Compiled SDL_TTF: %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
        printf("Linked SDL_TTF: %d.%d.%d\n", ttf_linked->major, ttf_linked->minor, ttf_linked->patch);
        
    }
    
    SDL_Window* window = NULL;
    SDL_Surface* windowSurface = NULL;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    
    if (TTF_Init() != 0) {
        printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }
    
    window = SDL_CreateWindow("thing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    
    TTF_Font *font = TTF_OpenFont("liberation-mono.ttf", 16);
    
    if (!font) {
        printf("Font could not be opened! TTF_Error %s\n", TTF_GetError());
        return -1;
    }
    
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    windowSurface = SDL_GetWindowSurface(window);
    SDL_Surface* blackSq;
    SDL_Surface* whiteSq;
    {
        SDL_PixelFormat pf = *windowSurface->format;
        int rm = pf.Rmask;
        int gm = pf.Gmask;
        int bm = pf.Bmask;
        int am = pf.Amask;
        blackSq = SDL_CreateRGBSurface(0, 1, 1, 32, rm, gm, bm, am);
        whiteSq = SDL_CreateRGBSurface(0, 1, 1, 32, rm, gm, bm, am);
        SDL_FillRect(blackSq, NULL, SDL_MapRGB(blackSq->format, 0, 0, 0));SDL_FillRect(whiteSq, NULL, SDL_MapRGB(whiteSq->format, 0xFF, 0xFF, 0xFF));
    }
    
    // Logic setup
    
    const int start_width = 100;
    const int start_height = 100;
    const int sqs_width = 10;
    const int sqs_height = 10;
    const int max_dim = MAX(sqs_width, sqs_height);
    SDL_Surface *numbers[max_dim + 1] = {0};
    {
        char text[(max_dim / 10) + 2];
        {
            SDL_Color bg = {0xFF, 0xFF, 0xFF}, fg = {0, 0, 0};
            for (int i = 0; i < max_dim; i++) {
                sprintf(text, "%d", i);
                numbers[i] = TTF_RenderText_Shaded(font, text, fg, bg);
                if (text == NULL) {
                    printf("TTF rendering failed! TTF_Error %s\n", TTF_GetError());
                    return -1;
                }
            }
        }
    }
    
    int game[sqs_width][sqs_height] = {0};
    int answer[sqs_width][sqs_height] = {1, 1, 1};
    int top_hints[sqs_width][5] = {0};
    int left_hints[sqs_height][5] = {0};
    {
        int counter = 0;
        for (int x = 0; x < sqs_width; x++) {
            counter = 0;
            for (int y = 0; y < sqs_height; y++) {
                if (answer[x][y] == 0) {
                    if (top_hints[x][counter] == 0) {
                        continue;
                    } else {
                        counter++;
                    }
                } else {
                    top_hints[x][counter]++;
                }
            }
        }
        for (int x = 0; x < sqs_width; x++) {
            counter = 0;
            for (int y = 0; y < sqs_height; y++) {
                if (answer[y][x] == 0) {
                    if (left_hints[x][counter] == 0) {
                        continue;
                    } else {
                        counter++;
                    }
                } else {
                    left_hints[x][counter]++;
                }
            }
        }
    }
    printf("%d%d\n", top_hints[0][0], top_hints[0][1]);
    printf("%d%d %d%d %d%d\n", left_hints[0][0], left_hints[0][1], left_hints[1][0], left_hints[1][1], left_hints[2][0], left_hints[2][1]);
    
    // Game loop
    
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        
        // Event loop
        
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT: {
                    quit = true;
                } break;
                case SDL_MOUSEBUTTONUP: {
                    SDL_MouseButtonEvent ev = e.button;
                    if (ev.x < start_width || ev.y < start_height) {
                        continue;
                    }
                    int x = (ev.x - start_width) / (float) SQ_SIDE;
                    int y = (ev.y - start_height) / (float) SQ_SIDE;
                    if (x < sqs_width && y < sqs_height) {
                        printf("M1 %d %d \n", ev.x, ev.y);
                        game[x][y] = !game[x][y];
                    }
                } break;
                default: {} break;
            }
        }
        
        
        
        // Rendering
        
        SDL_BlitScaled(whiteSq, NULL, windowSurface, NULL);
        SDL_Rect dest;
        
        // Grid
        {
            dest.w = SQ_SIDE * sqs_width + 1;
            dest.h = 1;
            dest.x = start_width - 1;
            for (int y = 0; y <= sqs_height; y++) {
                dest.y = y * SQ_SIDE + start_width - 1;
                SDL_BlitScaled(blackSq, NULL, windowSurface, &dest);
            }
            dest.w = 1;
            dest.h = SQ_SIDE * sqs_height + 1;
            dest.y = start_height - 1;
            for (int x = 0; x <= sqs_height; x++) {
                dest.x = x * SQ_SIDE + start_height - 1;
                SDL_BlitScaled(blackSq, NULL, windowSurface, &dest);
            }
        }
        
        // Marked squares
        dest.w = SQ_SIDE;
        dest.h = SQ_SIDE;
        for (int x = 0; x < sqs_width; x++) {
            for (int y = 0; y < sqs_height; y++) {
                dest.x = x * SQ_SIDE + start_width;
                dest.y = y * SQ_SIDE + start_height;
                if (game[x][y] != 0) {
                    SDL_BlitScaled(blackSq, NULL, windowSurface, &dest);
                }
            }
        }
        dest.x = start_width;
        dest.y = start_height - SQ_SIDE;
        SDL_BlitSurface(numbers[3], NULL, windowSurface, &dest);
        
        SDL_UpdateWindowSurface(window);
    }
    
    TTF_CloseFont(font);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}