#include "life.h"
#include "game.h"
#include <string.h>
#ifdef __MACH__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
namespace Game
{
    void LifeGrid::Step()
    {
        for (int x=0; x<w; x++)
            for (int y=0; y<h; y++)
            {
                int n = 0;
                if (*Gell(x-1,y-1)) n++;
                if (*Gell(x  ,y-1)) n++;
                if (*Gell(x+1,y-1)) n++;
                if (*Gell(x-1,y+1)) n++;
                if (*Gell(x  ,y+1)) n++;
                if (*Gell(x+1,y+1)) n++;
                if (*Gell(x-1,y  )) n++;
                if (*Gell(x+1,y  )) n++;
                if (n == 2) *Bell(x,y) = *Gell(x,y);
                else if (n == 3) *Bell(x,y) = 1;
                else *Bell(x,y) = 0;
            }
    }
    void LifeGrid::Draw()
    {
        for (int x=0; x<w; x++)
            for (int y=0; y<h; y++)
            {
                if (*Gell(x,y)) glColor3ub(0,255,0);
                else glColor3ub(0,0,0);
                DrawCell(x,y);
            }
    }
};

