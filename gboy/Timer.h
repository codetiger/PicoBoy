#pragma once

#include "constants.h"
#include "MMU.h"

class Timer {
private:
    MemoryManagementUnit *mmu;
    uint32_t oldFrequency;
    uint16_t timaStartClock;
    bool timaStarted;
    bool lastVisiblePulse;
    bool pendingOverflow;

    void handleTima(uint8_t cycles, uint16_t internalClock);
    void handleOverflow();
    uint32_t getTimerFrequency();
public:
    Timer(MemoryManagementUnit *mmu);
    ~Timer();
    void Cycle(uint8_t cycles);
};

const uint8_t FlagTimerClockMode = 3;
const uint8_t FlagTimerStart = 4;
