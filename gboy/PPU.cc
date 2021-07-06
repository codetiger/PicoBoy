#include "PPU.h"

PixelProcessingUnit::PixelProcessingUnit(MemoryManagementUnit *mmu) {
    this->mmu = mmu;
    cycleCount = 0;
    currentLine = 0;
    mmu->Write(AddrRegLcdY, 0, true);
    HasFrameBufferUpdated = false;
    setLCDMode(VBLANK);
}

PixelProcessingUnit::~PixelProcessingUnit() {
}

bool PixelProcessingUnit::check_bit(const uint8_t value, const uint8_t bit) {
    return (value & (1 << bit)) != 0;
}

void PixelProcessingUnit::setLCDMode(LcdMode mode) {
    currentMode = mode;
    bool hi = (((uint8_t)mode) >> 1) & 0x1;
    bool lo = ((uint8_t)mode) & 0x1;
    mmu->WriteIORegisterBit(AddrRegLcdStatus, FlagLcdStatusModeHigh, hi);
    mmu->WriteIORegisterBit(AddrRegLcdStatus, FlagLcdStatusModeLow, lo);
}

void PixelProcessingUnit::Cycle(uint8_t cycles) {
    cycleCount += cycles;

    switch (currentMode) {
        case HBLANK:
            processHBlank();
            break;
        case VBLANK:
            processVBlank();
            break;
        case ACCESS_OAM:
            processOam();
            break;
        case ACCESS_VRAM:
            processTransfer();
            break;
    }
}

void PixelProcessingUnit::updateLine() {
    currentLine++;
    uint8_t line = mmu->Read(AddrRegLcdY);
    line++;
    if (currentLine > 153)
        currentLine = line = 0;
    mmu->Write(AddrRegLcdY, line, true);
}

void PixelProcessingUnit::processHBlank() {
    if(cycleCount >= CyclesHBlank) {
        cycleCount = cycleCount % CyclesHBlank;
        updateLine();

        if (currentLine == 143) {
            setLCDMode(VBLANK);
            mmu->WriteIORegisterBit(AddrRegInterruptFlag, FlagInterruptVBlank, true);
            // writeSprites();
            updateFrameBuffer();
            HasFrameBufferUpdated = true;
        } else {
            setLCDMode(ACCESS_OAM);
        }
    }       
}

void PixelProcessingUnit::processVBlank() {
    if(cycleCount >= CyclesVBlank) {
        cycleCount = cycleCount % CyclesVBlank; 

        if (currentLine == 0) {
            setLCDMode(ACCESS_OAM);
        } else {
            updateLine();
            if(currentLine == 0)
                setLCDMode(ACCESS_OAM);
        }
    }    
}

void PixelProcessingUnit::processOam() {
    if(cycleCount >= CyclesOam) {
        cycleCount = cycleCount % CyclesOam;
        setLCDMode(ACCESS_VRAM);
    }    
}

void PixelProcessingUnit::processTransfer() {
    if(cycleCount >= CyclesTransfer) {
        cycleCount = cycleCount % CyclesTransfer;
        setLCDMode(HBLANK);

        writeBGWindowLine(currentLine);
        // writeSprites();
        // bool hblank_interrupt = mmu->ReadIORegisterBit(AddrRegLcdStatus, FlagLcdStatusHBlankInterruptOn);
        // if (hblank_interrupt)
        //     mmu->WriteIORegisterBit(AddrRegInterruptFlag, FlagInterruptLcd, true);

        // uint8_t currentLine = mmu->Read(AddrRegLcdY);
        // uint8_t currentLineCompare = mmu->Read(AddrRegLcdYCompare);
        // if(currentLine == currentLineCompare && mmu->ReadIORegisterBit(AddrRegLcdStatus, FlagLcdStatusLcdYCInterruptOn))
        //     mmu->WriteIORegisterBit(AddrRegInterruptFlag, FlagInterruptLcd, true);
        // mmu->WriteIORegisterBit(AddrRegLcdStatus, FlagLcdStatusCoincidence, (currentLine == currentLineCompare));
    }    
}

void PixelProcessingUnit::updateFrameBuffer() {
    for (uint8_t x = 0; x < 160; x++) {
        for (uint8_t y = 0; y < 144; y++) {
            uint8_t color = localFrameBuffer[x][y];
            int red, blue, green;
            if (color == 0) // white
                red = 0x9b, green = 0xbc, blue = 0x0f;
            else if (color == 1) // light gray
                red = 0x8b, green = 0xac, blue = 0x0f;
            else if (color == 2) // dark gray
                red = 0x30, green = 0x62, blue = 0x30;
            else
                red = 0x0f, green = 0x38, blue = 0x0f;

            FrameBuffer[x][y][0] = red;
            FrameBuffer[x][y][1] = green;
            FrameBuffer[x][y][2] = blue;
        }
    } 
    memset(localFrameBuffer, 0, sizeof(localFrameBuffer));
}

void PixelProcessingUnit::writeBGWindowLine(uint8_t line) {
    if(!mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlLcdOn))
        return;

    if(line >= 144)
        return;

    uint16_t mapStart = mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlBgMap) ? AddrBgMap0Start : AddrBgMap1Start;
    bool isSignedIndex = !mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlBgData);
    uint16_t dataStart = isSignedIndex ? AddrTileData0Start : AddrTileData1Start;

    uint8_t scrollX = mmu->Read(AddrRegScrollX);
    uint8_t scrollY = mmu->Read(AddrRegScrollY);
    uint8_t windowX = mmu->Read(AddrRegWindowX) - 7;
    uint8_t windowY = mmu->Read(AddrRegWindowY);

    bool usingWindow = (line >= windowY) && mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlWindowOn);
    uint16_t tilemap = mmu->ReadIORegisterBit(AddrRegLcdControl, usingWindow ? FlagLcdControlWindowMap : FlagLcdControlBgMap) ? AddrBgMap1Start : AddrBgMap0Start;

    uint8_t y = scrollY + line;
    if (usingWindow)
        y = line - windowY;

    uint16_t tile_row = ((uint8_t)(y / 8));

    for (int i = 0; i < 160; i++) {
        uint8_t x = i + scrollX;
        if (usingWindow && i >= windowX)
            x = i - windowX;

        uint16_t tile_column = x / 8;

        uint16_t addr = tilemap + tile_row * 32 + tile_column;
        int16_t tilenum;
        if (isSignedIndex)
            tilenum = (int8_t) mmu->Read(addr);
        else
            tilenum = mmu->Read(addr);

        uint16_t tile_address;
        if (!isSignedIndex)
            tile_address = dataStart + (tilenum * 16);
        else
            tile_address = dataStart + ((tilenum + 128) * 16);

        uint8_t lno = y % 8;
        uint8_t byte1 = mmu->Read(tile_address + lno * 2);
        uint8_t byte2 = mmu->Read(tile_address + lno * 2 + 1);

        uint8_t req_bit = 7 - (x % 8);
        uint8_t bit1 = (byte1 >> req_bit) & 1;
        uint8_t bit2 = (byte2 >> req_bit) & 1;
        uint8_t colorid = (bit1 << 1) | bit2;
        localFrameBuffer[i][line] = getColor(colorid, AddrRegBgPalette);
    }
}

void PixelProcessingUnit::writeSprites() {
    if(!mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlLcdOn))
        return;

    if(!mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlObjOn))
        return;

    for (uint8_t sprite_n = 0; sprite_n < 40; sprite_n++)
        drawSprite(sprite_n);
}

void PixelProcessingUnit::drawSprite(const uint8_t sprite_n) {
    uint16_t offset_in_oam = sprite_n * 4;
    uint16_t oam_start = AddrOAMStart + offset_in_oam;

    uint8_t sprite_y = mmu->Read(oam_start);
    uint8_t sprite_x = mmu->Read(oam_start + 1);

    if (sprite_y == 0 || sprite_y >= 160) { return; }
    if (sprite_x == 0 || sprite_x >= 168) { return; }

    uint8_t sprite_size_multiplier = mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlObjSize) ? 2 : 1;

    uint16_t tile_set_location = AddrTileData1Start;

    uint8_t pattern_n = mmu->Read(oam_start + 2);
    uint8_t sprite_attrs = mmu->Read(oam_start + 3);

    /* Bits 0-3 are used only for CGB */
    bool use_palette_1 = check_bit(sprite_attrs, 4);
    bool flip_x = check_bit(sprite_attrs, 5);
    bool flip_y = check_bit(sprite_attrs, 6);
    bool obj_behind_bg = check_bit(sprite_attrs, 7);

    uint8_t tile_offset = pattern_n * 8;
    uint16_t pattern_address = tile_set_location + tile_offset;

    Tile tile(pattern_address, mmu, sprite_size_multiplier);
    int start_y = sprite_y - 16;
    int start_x = sprite_x - 8;

    for (uint8_t y = 0; y < 8 * sprite_size_multiplier; y++) {
        for (uint8_t x = 0; x < 8; x++) {
            uint8_t maybe_flipped_y = !flip_y ? y : (8 * sprite_size_multiplier) - y - 1;
            uint8_t maybe_flipped_x = !flip_x ? x : 8 - x - 1;

            uint8_t gb_color = tile.get_pixel(maybe_flipped_x, maybe_flipped_y);
            if (gb_color == 0) // Color 0 is transparent
                continue;

            uint8_t screen_x = start_x + x;
            uint8_t screen_y = start_y + y;
            if (screen_x >= 160 || screen_y >= 144)
                continue; 

            uint8_t existing_pixel = localFrameBuffer[screen_x][screen_y];
            if (obj_behind_bg && existing_pixel != 0) 
                continue;

            uint8_t screen_color = getColor(gb_color, use_palette_1 ? AddrRegSprite1Palette : AddrRegSprite0Palette);
            localFrameBuffer[screen_x][screen_y] = screen_color;
        }
    }
}

int PixelProcessingUnit::getColor(int id, uint16_t palette) {
    uint8_t data = mmu->Read(palette);
    int hi = 2 * id + 1, lo = 2 * id;
    int bit1 = (data >> hi) & 1;
    int bit0 = (data >> lo) & 1;
    return (bit1 << 1) | bit0;
}
