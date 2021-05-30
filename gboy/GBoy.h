#pragma once

#include "constants.h"
#include "MMU.h"
#include "CPU.h"
#include "Timer.h"
#include "Cartridge.h"
#include "PPU.h"
#include <time.h>

class GBoy {
private:
    MemoryManagementUnit *mmu;
    CentralProcessingUnit *cpu; 
    PixelProcessingUnit *ppu;
    Timer *timer;

public:
    GBoy(std::string path);
    ~GBoy();
    void Print();
    void ExecuteStep();
    bool GetFrameBufferUpdatedFlag();
    void SetFrameBufferUpdatedFlag(bool v);

    void GetFrameBufferColor(uint8_t &red, uint8_t &green, uint8_t &blue, uint8_t x, uint8_t y);
};
