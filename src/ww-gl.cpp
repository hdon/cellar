#include <SDL.h>
#include <SDL_image.h>
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

Uint32 video_flags = SDL_OPENGL;
int width=640, height=480;
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

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,        1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,        2);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,      0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,     0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    0);

    if (SDL_SetVideoMode(width, height, 0, video_flags) == 0)
        FAIL("SDL_SetVideoMode()");

    int attr;
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &attr);
    printf("got %d stencil buffer bits\n", attr);
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &attr);
    printf("double buffering %sabled\n", (attr?"en":"dis"));

    /* Acquire a pixel-for-pixel view matrix */
    glMatrixMode(GL_MODELVIEW);
    glOrtho(0, width, 0, height, -1, 1);

    /* Enable the feature we need to store our cellular automata */
    //glEnable(GL_STENCIL_TEST);
    //glStencilFunc(GL_EQUAL, 

    do {
        /* Paint the color buffer black */
        glClear(GL_COLOR_BUFFER_BIT);
        
        /* Paint the color buffer white */
        glBegin(GL_QUADS);
        glColor3d(1,1,1);
        glVertex2d(0,     0);
        glVertex2d(width, 0);
        glVertex2d(width, height);
        glVertex2d(0,     height);
        glEnd();

        /* Draw some white dots */
        int x1 = width/4;
        int y1 = height/4;
        int x2 = width*3/4;
        int y2 = height*3/4;

        glBegin(GL_POINTS);
        glColor3d(0,0,0);

        for (int x=-1; x<=1; x++)
        for (int y=-1; y<=1; y++) {
            if (x == y || x == -y) continue;
            glVertex2d(x1+x, y1+y);
            glVertex2d(x2+x, y1+y);
            glVertex2d(x2+x, y2+y);
            glVertex2d(x1+x, y2+y);
        }
        glEnd();

        SDL_GL_SwapBuffers();

        SDL_WaitEvent(&event);
    } while (event.type != SDL_QUIT);

    SDL_Quit();
    return 0;
}

