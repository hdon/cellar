#ifndef GRID_HEADER
#define GRID_HEADER
namespace Game
{
    class BaseGrid
    {
        public:
            BaseGrid(int W, int H);
            virtual ~BaseGrid();
            // TODO: THIS MUST BE OPTIMIZED IN A _BIG_ WAY
            //char *Gell(int x, int y){return Grid+(x+y*w);};
            //char *Bell(int x, int y){return Brig+(x+y*w);};
            char *Gell(int x, int y) {
                if(x<0)x+=w; else if (x>=w)x-=w;
                if(y<0)y+=h; else if (y>=h)y-=h;
                return Grid+(x+y*w);
            };
            char *Bell(int x, int y) {
                if(x<0)x+=w; else if (x>=w)x-=w;
                if(y<0)y+=h; else if (y>=h)y-=h;
                return Brig+(x+y*w);
            };
            char *Fell(int x, int y) {
                if(x<0)x+=w; else if (x>=w)x-=w;
                if(y<0)y+=h; else if (y>=h)y-=h;
                return Frig+(x+y*w);
            };
            bool HaveNeighbor(int x, int y, int c);
            bool HaveAdjascent(int x, int y, int c);
            bool HaveDiagonals(int x, int y, int c);
            bool RangeAdjascent(int x, int y, int low, int high);
            bool RangeNeighbor(int x, int y, int low, int high);
            int CountRangeNeighbor(int x, int y, int low, int high);
            void Clear();

            virtual void Step()=0;
            void PStep();
            virtual void Draw()=0;
            int w, h;
            char *Grid;
            char *Brig;
            char *Frig;
    };
};
#endif

