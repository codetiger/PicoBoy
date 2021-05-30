#pragma once

#include "MMU.h"
#include "constants.h"

enum LcdMode {
    HBlank = 0,
    VBlank = 1,
    Oam = 2,
    Transfer = 3
};

class PixelProcessingUnit
{
private:
    MemoryManagementUnit *mmu;
    long cycleCount;

    LcdMode getLCDMode();
    void setLCDMode(LcdMode lcdMode);

    void processOam();
    void processTransfer();
    void processHBlank();
    void processVBlank();

    void writeScanLine(uint8_t line);
    void writeBGLine(uint8_t line);
    void writeWindowLine(uint8_t line);

    uint8_t localFrameBuffer[160][144][3];
    int getColor(int id, uint16_t palette);
public:
    PixelProcessingUnit(MemoryManagementUnit *mmu);
    ~PixelProcessingUnit();
    void Cycle(uint8_t cycles);

    uint8_t FrameBuffer[160][144][3];
    bool HasFrameBufferUpdated;
};
