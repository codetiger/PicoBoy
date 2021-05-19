
#pragma once

#include "MMU.h"


class Timer {
private:
    MemoryManagementUnit *mmu;
    uint32_t oldFrequency = 1024;
    uint16_t timaStartClock = 0;
    bool timaStarted = false;
    bool lastVisiblePulse = false;
    bool pendingOverflow = false;
public:
    Timer(MemoryManagementUnit *mmu);
    ~Timer();
};

Timer::Timer(MemoryManagementUnit *mmu) {
    this->mmu = mmu;
}

Timer::~Timer() {
}

void Timer::cycle(uint8_t cycles) {
    uint16_t internalClock = (mmu->Read8BitData(0xFF04) << 8) | mmu->Read8BitData(0xFF03);
    internalClock += cycles;
    mmu->Write8BitData(0xFF04, (internalClock & 0xFF00) >> 8);
    mmu->Write8BitData(0xFF03, (internalClock & 0x00FF));

    handleTima(cycles, internalClock);
}

void Timer::handleTima(uint8_t cycles, uint16_t internalClock) {
    if(!mmu.readIORegisterBit(Const::AddrRegTAC, Const::FlagTimerStart))
        timaStarted = false;
    else if(mmu.readIORegisterBit(Const::AddrRegTAC, Const::FlagTimerStart))
    {
        // If the timer just stared, we will save the clock count at start
        // This way we don't increment the timer to early, because the internal
        // clock may be not zero at this time.
        uint32_t previousClock = internalClock - cycles;
        if(!timaStarted)
        {
            timaStarted = true;
            timaStartClock = internalClock;
            // The timer just started in the processed t-cycles
            // So just handle the last t-cycle
            previousClock = internalClock - 1;
        }
                
        uint16_t bitToSelect = (Const::CyclesCpu / getTimerFrequency()) >> 1;
        for(int clock = previousClock + 1; clock <= internalClock; clock++)
        {
            // Handle pending overflows, triggered by previous loops
            handleOverflow();
            bool currentPulse = clock & bitToSelect;
            if(lastVisiblePulse && !currentPulse)
            {                    
                uint8_t timerValue = mmu.read(Const::AddrRegTIMA) + 1;
                pendingOverflow = timerValue == 0x00;
                mmu.write(Const::AddrRegTIMA, timerValue);          
            }
            lastVisiblePulse = currentPulse;
        }               
    }  
}