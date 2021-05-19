#pragma once

#include <chrono>

#include "MMU.h"
#include "CPU.h"

static const int maxCyclesPerFrame = 70368;
static const uint32_t CyclesCpu = 4194304;
static const int64_t OneFrameDurationNSec = 1000000000 / (CyclesCpu / maxCyclesPerFrame);

class GBoy {
private:
    MemoryManagementUnit *mmu;
    CentralProcessingUnit *cpu; 

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
    mmu = new MemoryManagementUnit();
    cpu = new CentralProcessingUnit(mmu);
    // mmu->TestMe();

    this->ExecuteFrame();
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

    int64_t cycles = maxCyclesPerFrame;
    while (cycles > 0) {
        if(!hasFrameStarted) 
            startFrame();

        uint8_t opCycles = cpu->ExecuteInstruction(0x000a);
        opCycles = (opCycles == 0) ? 4 : opCycles;
        // cycles -= opCycles;
    }    
}

