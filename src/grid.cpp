#include "grid.h"
#include "game.h"
#include <string.h>
#ifdef __MACH__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
namespace Game
{
    BaseGrid::BaseGrid(int W, int H)
    {
        w = W;
        h = H;
        Grid = new char[w*h];
        Brig = new char[w*h];
        Frig = new char[w*h];
        Clear();
    }
    BaseGrid::~BaseGrid()
    {
        delete[] Grid;
        delete[] Brig;
        delete[] Frig;
    }
    void BaseGrid::Clear()
    {
        memset(Grid, 0, w*h);
        memset(Brig, 0, w*h);
        memset(Frig, -1, w*h);
    }
    void BaseGrid::PStep()
    {
        char *temp = Grid;
        Grid = Brig;
        Brig = temp;
        memset(Brig, 0, w*h);

        for (int y=0; y<h; y++)
            for (int x=0; x<w; x++)
                if (*Fell(x,y)>=0) {
                    *Gell(x,y) = *Fell(x,y);
                    *Fell(x,y) = -1;
                }
    }
    bool BaseGrid::HaveDiagonals(int x, int y, int c)
    {
        if (*Gell(x-1,y-1)==c) return true;
        if (*Gell(x-1,y+1)==c) return true;
        if (*Gell(x+1,y-1)==c) return true;
        if (*Gell(x+1,y+1)==c) return true;
        return false;
    }
    bool BaseGrid::HaveNeighbor(int x, int y, int c)
    {
        if (*Gell(x-1,y-1)==c) return true;
        if (*Gell(x-1,y  )==c) return true;
        if (*Gell(x-1,y+1)==c) return true;
        if (*Gell(x+1,y-1)==c) return true;
        if (*Gell(x+1,y  )==c) return true;
        if (*Gell(x+1,y+1)==c) return true;
        if (*Gell(x  ,y-1)==c) return true;
        if (*Gell(x  ,y+1)==c) return true;
        return false;
    }
    bool BaseGrid::RangeAdjascent(int x, int y, int low, int high)
    {
        int c;
        c = *Gell(x-1,y  ); if (c>=low && c<=high) return true;
        c = *Gell(x+1,y  ); if (c>=low && c<=high) return true;
        c = *Gell(x  ,y-1); if (c>=low && c<=high) return true;
        c = *Gell(x  ,y+1); if (c>=low && c<=high) return true;
        return false;
    }
    int BaseGrid::CountRangeNeighbor(int x, int y, int low, int high)
    {
        int c,d=0;
        c = *Gell(x-1,y-1); if (c>=low && c<=high)  d++;
        c = *Gell(x-1,y  ); if (c>=low && c<=high)  d++;
        c = *Gell(x-1,y+1); if (c>=low && c<=high)  d++;
        c = *Gell(x+1,y-1); if (c>=low && c<=high)  d++;
        c = *Gell(x+1,y  ); if (c>=low && c<=high)  d++;
        c = *Gell(x+1,y+1); if (c>=low && c<=high)  d++;
        c = *Gell(x  ,y-1); if (c>=low && c<=high)  d++;
        c = *Gell(x  ,y+1); if (c>=low && c<=high)  d++;
        return d;
    }
    bool BaseGrid::RangeNeighbor(int x, int y, int low, int high)
    {
        int c;
        c = *Gell(x-1,y-1); if (c>=low && c<=high)  return true;
        c = *Gell(x-1,y  ); if (c>=low && c<=high)  return true;
        c = *Gell(x-1,y+1); if (c>=low && c<=high)  return true;
        c = *Gell(x+1,y-1); if (c>=low && c<=high)  return true;
        c = *Gell(x+1,y  ); if (c>=low && c<=high)  return true;
        c = *Gell(x+1,y+1); if (c>=low && c<=high)  return true;
        c = *Gell(x  ,y-1); if (c>=low && c<=high)  return true;
        c = *Gell(x  ,y+1); if (c>=low && c<=high)  return true;
        return false;
    }
    bool BaseGrid::HaveAdjascent(int x, int y, int c)
    {
        if (*Gell(x-1,y  )==c) return true;
        if (*Gell(x+1,y  )==c) return true;
        if (*Gell(x  ,y-1)==c) return true;
        if (*Gell(x  ,y+1)==c) return true;
        return false;
    }
};

