#pragma once

#include "MMU.h"
#include "Tile.h"
#include "constants.h"

enum LcdMode {
    HBLANK = 0,
    VBLANK = 1,
    ACCESS_OAM = 2,
    ACCESS_VRAM = 3,
};

class PixelProcessingUnit
{
private:
    MemoryManagementUnit *mmu;
    long cycleCount;
    LcdMode currentMode;
    uint8_t currentLine;

    void setLCDMode(LcdMode mode);

    void updateLine();
    void processOam();
    void processTransfer();
    void processHBlank();
    void processVBlank();

    void writeBGWindowLine(uint8_t line);
    void writeSprites();
    void drawSprite(const uint8_t sprite_n);
    void updateFrameBuffer();

    uint8_t localFrameBuffer[160][144];
    int getColor(int id, uint16_t palette);
    bool check_bit(const uint8_t value, const uint8_t bit);
    
public:
    PixelProcessingUnit(MemoryManagementUnit *mmu);
    ~PixelProcessingUnit();
    void Cycle(uint8_t cycles);

    uint8_t FrameBuffer[160][144][3];
    bool HasFrameBufferUpdated;
};

const uint16_t CyclesHBlank = 204;     // Mode 0 (H-Blank) 204 cycles per Scanline
const uint16_t CyclesVBlank = 456;     // Mode 1 (V-Blank) 4560 cycles per Frame 4560/10 times per Frame
const uint16_t CyclesOam = 80;         // Mode 2 (OAM Search) 80 cycles per Scanline
const uint16_t CyclesTransfer = 172;   // Mode 3 (Transfer LCD) 173 cycles per Scanline
