#ifndef MACHINE_HEADER
#define MACHINE_HEADER
#include "grid.h"
namespace Game
{
    class MachineGrid : public BaseGrid
    {
        public:
            MachineGrid(int W, int H):BaseGrid(W,H){}
            void Step();
            void Draw();
            void DestroyAllSparks();
            void SparkStep(const int x, const int y);
            // MachineSparks travel through MachineWires. They accomplish this
            // using 3 rules. 1) MachineWire adjascent to a MachingSpark becomes
            // a MachineSpark. 2) MachineSparks become MachineHotWires. 3)
            // MachineHotWires become MachineWires.
            enum { MachineEmpty, MachineMetal,
                MachineWire, MachineHotWire,    // spark adjascently
                MachineSWire, MachineHotSWire,  // spark " and diagonally
                MachineOWire, MachineHotOWire,  // odd wires. (only sparked by an odd number of sparks)
                MachineSpark, MachineSSpark,    // various sparks
                MachineOSpark, KillerSpark,
                MachineLifer,
                MachineKiller,                  // absorbs all neighborly life cells
                MachineClocker,
                MachineNumber };
            static const int LowSpark, HighSpark;
            static const int DeSparkingTable[15];
            static const int SparkSteppingTable[15];
    };
};
#endif

