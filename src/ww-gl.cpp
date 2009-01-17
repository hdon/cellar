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
int width=1024, height=1024;
SDL_Surface *screen;
SDL_Event event;

#define FAIL(f) do{SDL_Fail(__FILE__,__LINE__,f);}while(0);
inline void SDL_Fail(const char *filename, int line, const char *fn_name) {
    printf("%s:%d %s error: %s\n", filename, line, fn_name, SDL_GetError());
    SDL_Quit();
    exit(-1);
}


#define GL_ERROR_CHECK() do{\
    static int err = glGetError();\
    if (err) printf("%s:%d OpenGL error %d: %s\n",\
        __FILE__, __LINE__, err, gluErrorString(err));\
}while(0);

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

    /* Enable the stencil buffer */
    glEnable(GL_STENCIL_TEST);
    glClearColor(0,0,0,1);

    /* WireWorld is commonly ... TODO explain */
    //int ww_history_phase = 1;

    /* Create a texture map which we'll use for a feedback loop with the
     * color and stencil buffers */
    GLuint feedback_texture;
    glEnable(GL_TEXTURE_2D); /* XXX does this do anything? */
    glGenTextures(1, &feedback_texture);
    glBindTexture(GL_TEXTURE_2D, feedback_texture);
    /* We CANNOT have ANY texture filtering!!! */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    /* I guess we should disable texture wrapping */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    /* We will need fast access to this texture */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, 1);
    /* Allocate texture memory - XXX arguments don't matter because last arg is NULL */
    int twidth = 1024, theight = 1024;
    unsigned char *pixels = (unsigned char*)malloc(twidth*theight);
    for (int i=0; i<twidth*theight; i++)
        pixels[i] = 0;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, twidth, theight, 0,
            GL_LUMINANCE/*XXX*/, GL_UNSIGNED_BYTE/*XXX*/, pixels);
    GL_ERROR_CHECK();

    /* Draw four points on the stencil buffer */
    glDisable(GL_TEXTURE_2D);
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

    /* Enable texture mapping so that we can propagate Wire World sparks */
    glEnable(GL_TEXTURE_2D);
    glColor3d(1,1,1);

    /* Paint the color buffer black */
    glClear(GL_COLOR_BUFFER_BIT);
    
    do {
        /* Draw stencil buffer to color buffer four times with varied offsets */
        /* Defend color buffer contents with stencil */
        glStencilFunc(GL_EQUAL, 1, 255);
        /* Unconditionally defend stencil buffer */
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        /* */
        glDisable(GL_TEXTURE_2D);
        /* Draw four rectangle */
        for (int x=-1; x<=1; x++)
        for (int y=-1; y<=1; y++) {
            if (x == y || x == -y) continue;
            /* Adjust raster positions */
            glViewport(x, y, width, height);
            /* Draw rectangle */
            glBegin(GL_QUADS);
            glVertex2d(0,     0);
            glVertex2d(width, 0);
            glVertex2d(width, height);
            glVertex2d(0,     height);
            glEnd();
        }
        /* Reset viewport */
        glViewport(0, 0, width, height);

        GL_ERROR_CHECK();

        /* Feedback time */
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, twidth, theight);

        /* Draw texture to screen */
        glStencilFunc(GL_ALWAYS, 0, 0);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, feedback_texture);
        /* Draw our rectangle */
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex2d  (0, 0);
        glTexCoord2d(1, 0);
        glVertex2d  (twidth, 0);
        glTexCoord2d(1, 1);
        glVertex2d  (twidth, theight);
        glTexCoord2d(0, 1);
        glVertex2d  (0, theight);
        glEnd();

        /* Page flip! */
        SDL_GL_SwapBuffers();

        /* Wait for next SDL event? */
        SDL_WaitEvent(&event);
    } while (event.type != SDL_QUIT);

    SDL_Quit();
    return 0;
}

