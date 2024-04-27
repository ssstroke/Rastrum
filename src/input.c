#include "input.h"

#define STEP 0.2f

SDL_bool InputProcess(GameObject* player) {
    SDL_Event event;
    
    while (SDL_PollEvent(&event) == 1) {
        switch (event.type) {
        case SDL_QUIT:
            return SDL_TRUE;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_UP:
                player->transform->m[3][2] -= STEP;
                break;                        
            case SDLK_LEFT:                   
                player->transform->m[3][0] -= STEP;
                break;                        
            case SDLK_DOWN:                   
                player->transform->m[3][2] += STEP;
                break;                        
            case SDLK_RIGHT:                  
                player->transform->m[3][0] += STEP;
                break;
            }
            break;
        }
    }

    return SDL_FALSE;
}
