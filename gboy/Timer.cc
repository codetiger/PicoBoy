#include "Timer.h"

Timer::Timer(MemoryManagementUnit *mmu) {
    this->mmu = mmu;

    oldFrequency = 1024;
    timaStartClock = 0;
    timaStarted = false;
    lastVisiblePulse = false;
    pendingOverflow = false;
}

Timer::~Timer() {
}

void Timer::Cycle(uint8_t cycles) {
    uint16_t internalClock = (mmu->Read(0xFF04) << 8) | mmu->Read(0xFF03);
    internalClock += cycles;
    mmu->Write(0xFF04, (internalClock & 0xFF00) >> 8);
    mmu->Write(0xFF03, (internalClock & 0x00FF));

    handleTima(cycles, internalClock);
}

void Timer::handleTima(uint8_t cycles, uint16_t internalClock) {
    bool hasTimerStarted = mmu->ReadIORegisterBit(AddrRegTAC, FlagTimerStart);
    if(!hasTimerStarted)
        timaStarted = false;
    else {
        uint32_t previousClock = internalClock - cycles;
        if(!timaStarted) {
            timaStarted = true;
            timaStartClock = internalClock;
            previousClock = internalClock - 1;
        }
        
        uint16_t bitToSelect = (CyclesCpu / getTimerFrequency()) >> 1;
        for(int clock = previousClock + 1; clock <= internalClock; clock++) {
            handleOverflow();
            bool currentPulse = clock & bitToSelect;
            if(lastVisiblePulse && !currentPulse) {                    
                uint8_t timerValue = mmu->Read(AddrRegTIMA) + 1;
                pendingOverflow = timerValue == 0x00;
                mmu->Write(AddrRegTIMA, timerValue);          
            }
            lastVisiblePulse = currentPulse;
        }               
    }  
}


void Timer::handleOverflow() {
    if(pendingOverflow) {
        pendingOverflow = false;
        mmu->Write(AddrRegTIMA, mmu->Read(AddrRegTMA));
        mmu->WriteIORegisterBit(AddrRegInterruptFlag, FlagInterruptTimer, true);
    }
}

uint32_t Timer::getTimerFrequency() {
    uint32_t frequency = 0;
    uint8_t regTac = mmu->Read(AddrRegTAC);
    uint8_t setFrequency = regTac & FlagTimerClockMode;
    switch(setFrequency) {
        case 0:
            frequency = 4096;
            break;
        case 1:
            frequency = 262144;
            break;
        case 2:
            frequency = 65536;
            break;
        case 3:
            frequency = 16384;
            break;
    }
    return frequency;
}