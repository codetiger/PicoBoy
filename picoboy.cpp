#include <iostream>
#include <string>

#include <SDL.h>
#include <SDL_timer.h>

#include "./gboy/GBoy.h"

int main(int argc, char *argv[]){
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    const uint8_t scale = 2;

    std::string romPath = "../roms/tetris.gb";
    if(argc >= 2)
        romPath = argv[1];
    GBoy *gb = new GBoy(romPath);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    SDL_CreateWindowAndRenderer(160*scale, 144*scale, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "PicoBoy");

    SDL_Texture *gb_screen_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        160, 144
    );

    bool quit = false;
    while (!quit) {
        gb->ExecuteStep();

        if(gb->GetFrameBufferUpdatedFlag()) {
            SDL_PollEvent(&event);
            if (event.type == SDL_QUIT)
                quit = true;

            SDL_RenderClear(renderer);
            void* pixels_ptr;
            int pitch;
            SDL_LockTexture(gb_screen_texture, nullptr, &pixels_ptr, &pitch);

            uint32_t* pixels = static_cast<uint32_t*>(pixels_ptr);
            for (int x = 0; x < 160; ++x) {
                for (int y = 0; y < 144; ++y) {
                    uint8_t red, green, blue;
                    gb->GetFrameBufferColor(red, green, blue, x, y);
                    pixels[160 * y + x] = ((uint32_t)red << 16) | ((uint32_t)green << 8) | ((uint32_t)blue << 0);
                }
            }
            gb->SetFrameBufferUpdatedFlag(false);

            SDL_UnlockTexture(gb_screen_texture);

            SDL_Rect dest_rect = { 0, 0, 160*scale, 144*scale };
            SDL_RenderCopy(renderer, gb_screen_texture, nullptr, &dest_rect);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
