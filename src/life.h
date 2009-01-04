#ifndef LIFE_HEADER
#define LIFE_HEADER
#include "grid.h"
namespace Game
{
    class LifeGrid : public BaseGrid
    {
        public:
            LifeGrid(int W, int H):BaseGrid(W,H){}
            void Step();
            void Draw();
    };
};
#endif

