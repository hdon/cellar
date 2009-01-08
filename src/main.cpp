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
#include <SDL_image.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>

#define FPS 30

using namespace std;

int ScreenWidth=640;
int ScreenHeight=480;
Uint32 video_flags = SDL_OPENGL|SDL_RESIZABLE|SDL_DOUBLEBUF;

SDL_Event event;
void conCB(OGLCONSOLE_Console console, char* cmd) {
    SDL_Surface * image;

    image = IMG_Load(cmd);
    if (!image) {
        OGLCONSOLE_Print("Unable to load image from file \"%s\"", cmd);
        return;
    }

    // Next we'll be taking stock of all colors used in the image, while loading
    // temporary values into the Wire World (Machine) grid
    SDL_PixelFormat *format = image->format;
    int x=0, y=0, npixels=0, i, bpp = format->BytesPerPixel, pitch = image->pitch;
    int w = image->w, h = image->h;
    OGLCONSOLE_Print("Image file \"%s\" is %dx%d with %d bytes color\n", cmd, w, h, bpp);

    // Ignore unreasonably large images
    if ((w > 6000) || (h > 6000)) return;

    // Resize display
    ScreenWidth = w;// * 4;
    ScreenHeight = h;// * 4;
    if (SDL_SetVideoMode(ScreenWidth, ScreenHeight, 32, video_flags) == 0)
    {
        printf("SDL_SetVideoMode error: %s\n", SDL_GetError());
        SDL_Quit();
        return; // TODO exit?
    }
    glViewport(0, 0, ScreenWidth, ScreenHeight);

    // Resize game using image dimensions
    Game::DefaultSetup(w, h, true, false);
    
    Uint32 pixel, pixels[64];
    static char naive_translation_table[4] = {
        Game::MachineGrid::MachineEmpty,
        Game::MachineGrid::MachineWire,
        Game::MachineGrid::MachineSpark,
        Game::MachineGrid::MachineHotWire,
    };
    do {
        // Extract pixel value
        void* row = (void*) (((Uint8*)image->pixels) + pitch * y);
        switch (bpp) {
            case 1:
            pixel = ((Uint8*)row)[x];
            break;
            case 2:
            pixel = ((Uint16*)row)[x];
            break;
            case 3:
            pixel = ((((Uint8*)row) + x*3)[0] >> 16) ||
                    ((((Uint8*)row) + x*3)[1] >> 8 ) ||
                    (((Uint8*)row) + x*3)[2];
            break;
            case 4:
            pixel = ((Uint32*)row)[x];
            break;
            default:
                printf("%s:%d error: cannot process image with %d bytes of color\n", __FILE__, __LINE__, bpp);
                exit(0);
        }

        // Search for pixel value in previously seen pixel values
        for (i=0; i<npixels; i++) {
            if (pixels[i] == pixel) break;
        }
        if ((i >= npixels) || (npixels == 0)) {
            // new pixel. add it to the list
            pixels[npixels] = pixel;

            // assign Wire World cell value
            *Game::machineGrid->Gell(x, y) = naive_translation_table[npixels];

            // increment pixel count
            npixels++;
        } else {
            // old pixel value; look it up
            int n=0;
            while (pixels[n] != pixel) n++;

            // assign Wire World cell value
            *Game::machineGrid->Gell(x, y) = naive_translation_table[n];
        }

        
        // Advance cursor
        if (++x >= w) {
            x = 0;
            if (++y >= h) break;
        }
    } while (npixels<=4);

    SDL_FreeSurface(image);
    OGLCONSOLE_Print("found %d different colors in image file \"%s\"", npixels, cmd);

/*
    // Okay, so we've taken stock of all colors, if we have a reasonable number, we'll
    // examine their colors and try to intuit their meanings
    if (npixels == 4) {
        map<Uint32,int> translation;

        // Translate arbitrary pixel values into naive RGB intensity values
        for (i=0; i<npixels; i++) {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[i], format, &r, &g, &b);
            pixels[i] = r+g+b;
        }
        for (i=0; i<npixels; i++) {
            
        }

        // Interpret image as Brian Silverman's Wire World
        // Assume darkest to brightest are: empty, conductor, electron tail, electron head
        for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) {
            *Game::machineGrid->Gell(x, y) =
                translation[(int)*Game::machineGrid->Gell(x, y)];
        }
    }*/
};

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

    if (fs) video_flags |= SDL_FULLSCREEN;

    if (SDL_SetVideoMode(ScreenWidth, ScreenHeight, 32, video_flags) == 0)
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
                case SDL_VIDEORESIZE:
                    printf("video resize %dx%d\n", event.resize.w, event.resize.h);
                    ScreenWidth = event.resize.w;
                    ScreenHeight = event.resize.h;
                    SDL_SetVideoMode
                        (ScreenWidth, ScreenHeight, 32, video_flags);
                    glViewport(0, 0, ScreenWidth, ScreenHeight);
                    break;
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

