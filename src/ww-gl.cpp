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
    printf("got %d stencil buffer bits (SDL_GL_GetAttr)\n", attr);
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &attr);
    printf("double buffering %sabled (SDL_GL_GetAttr)\n", (attr?"en":"dis"));

    glGetIntegerv(GL_STENCIL_BITS, &attr);
    printf("got %d stencil buffer bits (glGet)\n", attr);

    int x1, y1, x2, y2;

    /* Acquire a pixel-for-pixel view matrix */
    glMatrixMode(GL_MODELVIEW);
    glOrtho(0, width, 0, height, -1, 1);

    /* Enable the feature we need to store our cellular automata */
    glEnable(GL_STENCIL_TEST);
    glClearColor(0.6,0.6,0.6,1);

    /* WireWorld is commonly ... TODO explain */
    //int ww_history_phase = 1;

    /* Draw four points on the stencil buffer */
    /* Unconditinoally replace stencil buffer contents */
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    /* Unconditionally defend color buffer contents */
    glStencilFunc(GL_NEVER, 1, 255);
    /* Rectangle shape in middle of screen */
    x1 = width/4;
    y1 = height/4;
    x2 = width*3/4;
    y2 = height*3/4;
    /* Draw four points */
    glBegin(GL_POINTS);
    glVertex2d(x1, y1);
    glVertex2d(x2, y1);
    glVertex2d(x2, y2);
    glVertex2d(x1, y2);
    glEnd();

    do {
        /* Paint the color buffer grey60% */
        glClear(GL_COLOR_BUFFER_BIT);
        
        /* Draw stencil buffer to color buffer four times with varied offsets */
        /* Defend color buffer contents with stencil */
        glStencilFunc(GL_EQUAL, 1, 255);
        /* Unconditionally defend stencil buffer */
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        /* Draw four rectangle */
        glColor3d(1,0,0);
        for (int x=-1; x<=1; x++)
        for (int y=-1; y<=1; y++) {
            if (x == y || x == -y) continue;
            /* Adjust raster positions */
            glViewport(x, y, width, height);
            /* Draw rectangle */
            glBegin(GL_QUADS);
            glColor3d(1,0,0);
            glVertex2d(0,     0);
            glVertex2d(width, 0);
            glVertex2d(width, height);
            glVertex2d(0,     height);
            glEnd();
        }
        /* Reset viewport */
        glViewport(0, 0, width, height);

        {int err=glGetError();if(err)printf("GL ERROR: %i\n",err);}

        /* Page flip! */
        SDL_GL_SwapBuffers();
        puts("flip!");

        /* Wait for next SDL event? */
        SDL_WaitEvent(&event);
    } while (event.type != SDL_QUIT);

    SDL_Quit();
    return 0;
}

