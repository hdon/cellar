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

    /* Acquire a pixel-for-pixel view matrix */
    glMatrixMode(GL_MODELVIEW);
    glOrtho(0, width, 0, height, -1, 1);

    /* Enable the feature we need to store our cellular automata */
    glEnable(GL_STENCIL_TEST);

    /* WireWorld is commonly ... TODO explain */
    int ww_history_phase = 1;

    do {
        int x1, y1, x2, y2;

        /* Paint the color buffer black */
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        /* Clear the stencil buffer to 0 */
        //?
        
        /* Draw white rect encompassing entire display */
        /* Unconditionally replace color buffer contents */
        glStencilFunc(GL_ALWAYS, 0, 0);
        /* Unconditionally defend stencil buffer contents */
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        /* Draw rectangle */
        glBegin(GL_QUADS);
        glColor3d(1,1,1);
        glVertex2d(0,     0);
        glVertex2d(width, 0);
        glVertex2d(width, height);
        glVertex2d(0,     height);
        glEnd();

        /* Draw a rectangle onto the upper-right corner of stencil buffer */
        /* Unconditionally defend color buffer contents */
        glStencilFunc(GL_NEVER, 1, -1);
        /* Unconditionally replace stencil buffer contents with value 1 */
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
        /* Rectangle shape in upper-right screen quadrant */
        x1 = width/2;
        y1 = height/2;
        x2 = width;
        y2 = height;
        /* Draw rectangle */
        glBegin(GL_QUADS);
        glVertex2d(x1, y1);
        glVertex2d(x2, y1);
        glVertex2d(x2, y2);
        glVertex2d(x1, y2);
        glEnd();

        /* Draw some dots to the color buffer */
        glColor3d(1,0,0);
        /* Unconditinoally defend stencil buffer contents */
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        /* Defend color buffer contents with stencil test */
        glStencilFunc(GL_NOTEQUAL, ww_history_phase, ww_history_phase);
        ww_history_phase ^= 3;
        /* Rectangle shape in middle of screen */
        x1 = width/4;
        y1 = height/4;
        x2 = width*3/4;
        y2 = height*3/4;
        /* Draw sixteen points */
        for (int x=-1; x<=1; x++)
        for (int y=-1; y<=1; y++) {
            /* Adjust raster positions of dots */
            if (x == y || x == -y) continue;
            glViewport(x, y, width, height);
            /* Plot points */
            glBegin(GL_POINTS);
            glVertex2d(x1, y1);
            glVertex2d(x2, y1);
            glVertex2d(x2, y2);
            glVertex2d(x1, y2);
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

