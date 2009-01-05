#include "game.h"
#include <stdlib.h>
#include "machine.h"
#include <string.h>
#ifdef __MACH__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "life.h"
namespace Game
{
    extern LifeGrid *lifeGrid;
    const int MachineGrid::LowSpark = MachineSpark;
    const int MachineGrid::HighSpark = KillerSpark;

    void MachineGrid::Step()
    {
        memset(Brig, 0, w*h);
        for (int x=0; x<w; x++)
            for (int y=0; y<h; y++)
            {
                switch (*Gell(x,y))
                {
                    default:
                        *Bell(x,y) = *Gell(x,y);
                        break;
                    case MachineOSpark:
                        *Bell(x,y) = MachineHotOWire;
                        break;
                    case MachineHotOWire:
                        *Bell(x,y) = MachineOWire;
                        break;
                    case MachineOWire:
                        if (CountRangeNeighbor(x,y,LowSpark,HighSpark)&1==1)
                            *Bell(x,y) = MachineOSpark;
                        else
                            *Bell(x,y) = MachineOWire;
                        break;
                    case MachineClocker:
                        *Bell(x,y) = *Gell(x,y);
                        {
                            const int clockrot[9][2] = {
                                {-1,-1},
                                { 0,-1},
                                { 1,-1},
                                { 1, 0},
                                { 1, 1},
                                { 0, 1},
                                {-1, 1},
                                {-1, 0},
                                {-1,-1}
                            };
                            for (int i=0; i<8; i++) {
                                int x1 = x + clockrot[i][0];
                                int y1 = y + clockrot[i][1];
                                int x2 = x + clockrot[i+1][0];
                                int y2 = y + clockrot[i+1][1];

                                if (*Gell(x1,y1) != MachineClocker && *Gell(x2,y2) != MachineClocker)
                                    *Fell(x2,y2) = *Gell(x1,y1);
                            }
                        }
                        break;
                    case MachineSSpark:
                        *Bell(x,y) = MachineHotSWire;
                        break;
                    case MachineHotSWire:
                        *Bell(x,y) = MachineSWire;
                        break;
                    case MachineSWire:
                        if (RangeNeighbor(x,y, LowSpark, HighSpark))
                            *Bell(x,y) = MachineSSpark;
                        else *Bell(x,y) = MachineSWire;
                        break;

                    case MachineSpark:
                        *Bell(x,y) = MachineHotWire;
                        break;
                    case MachineHotWire:
                        *Bell(x,y) = MachineWire;
                        break;
                    case MachineWire: {
                        int n = RangeNeighbor(x,y, LowSpark, HighSpark);
                        *Bell(x,y) = (n == 1) || (n == 2) ? MachineSpark:MachineWire;
                        } break;

                    case MachineLifer:
                        if (RangeAdjascent(x,y, LowSpark, HighSpark))
                            MakeLife(x,y);
                        *Bell(x,y) = MachineLifer;
                        break;

                    case MachineKiller:
                        if (lifeGrid->HaveNeighbor(x,y,1))
                        {
                            *lifeGrid->Bell(x-1,y-1) = 0;
                            *lifeGrid->Bell(x-1,y  ) = 0;
                            *lifeGrid->Bell(x-1,y+1) = 0;
                            *lifeGrid->Bell(x+1,y-1) = 0;
                            *lifeGrid->Bell(x+1,y  ) = 0;
                            *lifeGrid->Bell(x+1,y+1) = 0;
                            *lifeGrid->Bell(x  ,y-1) = 0;
                            *lifeGrid->Bell(x  ,y+1) = 0;
                            *lifeGrid->Bell(x  ,y  ) = 0;
                            *Bell(x,y) = KillerSpark;
                        }
                        else *Bell(x,y) = MachineKiller;
                        break;

                    case KillerSpark:
                        *Bell(x,y) = MachineKiller;
                        break;
                }
            }
    }
    void MachineGrid::Draw()
    {
        for (int x=0; x<w; x++)
            for (int y=0; y<h; y++)
            {
                const int colors[MachineNumber][3] =
                {
                    {  0,  0,  0},  // Empty
                    { 90, 90, 90},  // Metal

                    { 57, 46, 12},  // wire
                    {234,191, 48},  // hot wire

                    { 69, 19, 11},  // "s" wire
                    {230, 64, 37},  // hot "s" wire

                    { 63, 33, 24},  // "i" wire
                    {141, 80, 59},  // hot "i" wire

                    {255,255,  0},  // spark
                    {255,255,  0},  // "s" spark
                    {255,255,  0},  // "i" spark
                    {255,255,  0},  // killer spark

                    {  0, 90,  0},  // Lifer
                    {255,  0,  0},  // Killer
                    {127,  0,255}   // Clocker
                };

                int c = *Gell(x,y);
                if (c >= MachineNumber) glColor3ub(random()%256,random()%256,random()%256);
                else glColor3ub(colors[c][0],colors[c][1],colors[c][2]);
                DrawCell(x,y);
            }
    }

    const int MachineGrid::SparkSteppingTable[15] =
    {
        MachineEmpty, MachineMetal, MachineSpark, MachineWire, MachineSSpark, MachineSWire,
        MachineOSpark, MachineOWire, MachineHotWire, MachineHotSWire, MachineHotOWire,
        MachineKiller, MachineLifer, KillerSpark
    };
    const int MachineGrid::DeSparkingTable[15] =
    {
        MachineEmpty, MachineMetal, MachineWire, MachineWire, MachineSWire, MachineSWire,
        MachineOWire, MachineOWire, MachineWire, MachineSWire, MachineOWire, MachineKiller,
        MachineLifer, MachineKiller
    };

    void MachineGrid::SparkStep(const int x, const int y)
    {
        if (x<0 || x>=w || y<0 || y>=h) return;
        *Gell(x,y) = SparkSteppingTable[(int)*Gell(x,y)];
    }
    
    void MachineGrid::DestroyAllSparks()
    {
        for (int x=0; x<w; x++)
            for (int y=0; y<h; y++)
                *Gell(x,y) = DeSparkingTable[(int)*Gell(x,y)];
    }
};

