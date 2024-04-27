#include "input.h"

#include "main.h"

#define STEP 0.02f

SDL_bool InputProcess(GameObject* player) {
    SDL_Event event;
    while (SDL_PollEvent(&event) == 1) {
        switch (event.type) {
        case SDL_QUIT:
            return SDL_TRUE;

        case SDL_MOUSEMOTION:
            // [0, 799] -> [-1, 1] -> [-6, 6]
            //

            player->transform->m[3][0] = ( (float)(event.motion.x - WINDOW_WIDTH / 2) / (float)(WINDOW_WIDTH / 2) ) * 6.0f;
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                return SDL_TRUE;
        }
    }

    return SDL_FALSE;
}
