#include <SDL.h>
#include <SDL_image.h>
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

Uint32 video_flags = SDL_OPENGL | SDL_RESIZABLE;
SDL_Surface *screen;
SDL_Event event;

#define FAIL(f) do{SDL_Fail(__FILE__,__LINE__,f);}while(0);
inline void SDL_Fail(const char *filename, int line, const char *fn_name) {
    printf("%s:%d %s error: %s\n", filename, line, fn_name, SDL_GetError());
    SDL_Quit();
    exit(-1);
}

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
        FAIL("SDL_Init()");

    if (SDL_SetVideoMode(640, 480, 0, video_flags) == 0)
        FAIL("SDL_SetVideoMode()");

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,        0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,        1);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,      0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,     0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    0);
    glDisable(GL_DEPTH_TEST);

    do {
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_WaitEvent(&event);
    } while (event.type != SDL_QUIT);

    SDL_Quit();
    return 0;
}

