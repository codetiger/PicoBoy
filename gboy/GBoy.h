#pragma once

#include <chrono>

#include "constants.h"
#include "MMU.h"
#include "CPU.h"
#include "Timer.h"
#include "Cartridge.h"

class GBoy {
private:
    MemoryManagementUnit *mmu;
    CentralProcessingUnit *cpu; 
    Timer *timer;

    std::chrono::time_point<std::chrono::high_resolution_clock> frameStartTime;
    bool hasFrameStarted;

    bool isBooting;

    void startFrame();
    bool shouldWaitForFrame();

public:
    GBoy();
    ~GBoy();
    void Print();
    void ExecuteFrame();
};

GBoy::GBoy() {
    Cartridge *cart = new Cartridge("./roms/tetris.gb");
    mmu = new MemoryManagementUnit(cart);
    cpu = new CentralProcessingUnit(mmu);
    timer = new Timer(mmu);
}

GBoy::~GBoy() {
    delete mmu;
    delete cpu;
}

void GBoy::startFrame() {
    hasFrameStarted = true;
    frameStartTime = std::chrono::high_resolution_clock::now();
}

bool GBoy::shouldWaitForFrame() {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds chronoOne = std::chrono::nanoseconds(OneFrameDurationNSec);
    std::chrono::nanoseconds elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - frameStartTime);    
    std::chrono::nanoseconds shouldSleep = std::chrono::duration_cast<std::chrono::nanoseconds>(chronoOne - elapsed);

    return shouldSleep.count() > 0;
}

void GBoy::ExecuteFrame() {
    if(hasFrameStarted) while(shouldWaitForFrame());

    int64_t cycles = CyclesFrame;
    while (cycles > 0) {
        if(!hasFrameStarted) 
            startFrame();

        uint8_t opCycles = cpu->ExecuteInstruction(0x0000);
        opCycles = (opCycles == 0) ? 4 : opCycles;

        timer->cycle(opCycles);
        cycles -= opCycles;
    }    
}

