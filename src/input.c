#include "input.h"

#define STEP 0.1f

SDL_bool InputProcess(Mesh* rocket) {
    SDL_Event event;
    
    while (SDL_PollEvent(&event) == 1) {
        switch (event.type) {
        case SDL_QUIT:
            return SDL_TRUE;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_UP:
                rocket->transform->m[3][2] -= STEP;
                break;                        
            case SDLK_LEFT:                   
                rocket->transform->m[3][0] -= STEP;
                break;                        
            case SDLK_DOWN:                   
                rocket->transform->m[3][2] += STEP;
                break;                        
            case SDLK_RIGHT:                  
                rocket->transform->m[3][0] += STEP;
                break;
            }
            break;
        }
    }

    return SDL_FALSE;
}
