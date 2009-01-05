#include "oglconsole.h"
#include "game.h"
#include "sound.h"
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif
#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>

#define FPS 60

using namespace std;

SDL_Event event;
void conCB(OGLCONSOLE_Console, char*){};

int ScreenWidth=640;
int ScreenHeight=480;
int main(int argc, char **argv)
{
    bool fs = false;
    for (int i=1; i<argc; i++)
    {
        if (argv[i][0]=='-')
            switch(argv[i][1])
            {
                // Long options
                case '-':
                    break;
                case 'f':
                    fs = true;
                    break;
                case 'h':
                    ScreenHeight = atoi(argv[i]+2);
                    break;
                case 'w':
                    ScreenWidth = atoi(argv[i]+2);
                    break;
            }
    }
    int fps_counter = 0, fps_timer = 0;
    int quit = 0;

    srandom(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
    {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Joystick * JS;
    SDL_JoystickEventState(SDL_ENABLE);
    JS=SDL_JoystickOpen(0);

    if (SDL_SetVideoMode(ScreenWidth, ScreenHeight, 32,
                SDL_OPENGL | SDL_HWPALETTE | SDL_DOUBLEBUF | (fs?SDL_FULLSCREEN:0)) == 0)
    {
        printf("SDL_SetVideoMode error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    OGLCONSOLE_Create();
    OGLCONSOLE_EnterKey(conCB);

    Sound :: Init();
    Game :: Init();
    Game :: DefaultSetup();

    SDL_GL_SwapBuffers();

    int next_expected_frame_time = 0;
    bool cursorHidden = false;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    if (cursorHidden) {
                        SDL_ShowCursor(SDL_ENABLE);
                        cursorHidden = false;
                    }
                    break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    if (!cursorHidden) {
                        SDL_ShowCursor(SDL_DISABLE);
                        cursorHidden = true;
                    }
                    break;

            }
            
            if (OGLCONSOLE_SDLEvent(&event)) continue;

            // TODO: Phase in this interface, phase out all others
            if (Game::SDLEvent(&event)) continue;

            switch (event.type)
            {
                case SDL_MOUSEMOTION:
                    Game :: Mouse(event.motion.x, event.motion.y);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    Game :: MouseButton(event.button.button, event.button.state);
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE
                            ||  event.key.keysym.sym == SDLK_q)
                        quit = 1;

                    // TODO: Change this into an FPS display toggle?
                    else if (event.key.keysym.sym == SDLK_f)
                    {
                        int t = SDL_GetTicks();
                        double seconds = (t - fps_timer) / 1000.0;
                        double fps = fps_counter / seconds;

                        OGLCONSOLE_Print("%d frames in %g seconds = %g FPS\n",
                                fps_counter, seconds, fps);

                        fps_timer = t;
                        fps_counter = 0;
                    }
                    break;
            }

            if (event.type == SDL_QUIT)
                quit = 1;
        }

        // Tick game progress
        Game :: Step();

        // If the current time is past the desired frame time, then we skip this
        // frame
        int t = SDL_GetTicks();
        if (t < next_expected_frame_time)
        {
            // Render the screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Game :: Draw();
            OGLCONSOLE_Draw();

            // If the current time is before the desired frame time, then we
            // choke our performance here with a delay
            int u = SDL_GetTicks();
            if (u < next_expected_frame_time)
                // TODO: Put some kind of offset in here?
                SDL_Delay(next_expected_frame_time - u);

            // Determine the next expected frame time
            next_expected_frame_time += 1000 / FPS;

            // Flip screen buffers
            SDL_GL_SwapBuffers();
        }
        else
        {
            // Perhaps some diagnostic output should be made available here?
            // cout << "Below 60 FPS" << endl;

            // Determine the next expected frame time
            next_expected_frame_time += 1000 / FPS;
        }


        fps_counter++;
    }

    OGLCONSOLE_Quit();
    Game :: Quit();
    Sound :: Quit();
    SDL_Quit();
    return 0;
}

