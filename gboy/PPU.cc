#include "PPU.h"

PixelProcessingUnit::PixelProcessingUnit(MemoryManagementUnit *mmu) {
    this->mmu = mmu;
    this->cycleCount = 0;
    HasFrameBufferUpdated = false;
}

PixelProcessingUnit::~PixelProcessingUnit() {
}

LcdMode PixelProcessingUnit::getLCDMode() {
    bool hi = mmu->ReadIORegisterBit(AddrRegLcdStatus, FlagLcdStatusModeHigh);
    bool lo = mmu->ReadIORegisterBit(AddrRegLcdStatus, FlagLcdStatusModeLow);

    uint8_t lcdModeValue = (((uint8_t)hi) << 1) | lo;
    LcdMode lcdMode = static_cast<LcdMode>(lcdModeValue);
    return lcdMode;
}

void PixelProcessingUnit::setLCDMode(LcdMode lcdMode) {
    uint8_t lcdModeValue = static_cast<uint8_t>(lcdMode);
    mmu->WriteIORegisterBit(AddrRegLcdStatus, FlagLcdStatusModeHigh, (lcdModeValue >> 1) & 0x1);
    mmu->WriteIORegisterBit(AddrRegLcdStatus, FlagLcdStatusModeLow, lcdModeValue & 0x1);
}

void PixelProcessingUnit::Cycle(uint8_t cycles) {
    cycleCount += cycles;
    LcdMode currentMode = getLCDMode();

    switch (currentMode) {
        case Oam:
            processOam();
            break;
        case Transfer:
            processTransfer();
            break;
        case HBlank:
            processHBlank();
            break;
        case VBlank:
            processVBlank();
            break;
    }
}

void PixelProcessingUnit::processOam() {
    if(cycleCount >= CyclesOam) {
        // printf("OAM\n");
        cycleCount = cycleCount % CyclesOam;
        setLCDMode(Transfer);
    }    
}

void PixelProcessingUnit::processTransfer() {
    if(cycleCount >= CyclesTransfer) {
        // printf("Transfer\n");
        cycleCount = cycleCount % CyclesTransfer;
        setLCDMode(HBlank);

        bool hblank_interrupt = mmu->ReadIORegisterBit(AddrRegLcdStatus, FlagLcdStatusHBlankInterruptOn);
        if (hblank_interrupt)
            mmu->WriteIORegisterBit(AddrRegInterruptFlag, FlagInterruptLcd, true);

        uint8_t lcdy = mmu->Read(AddrRegLcdY);
        uint8_t lcdyc = mmu->Read(AddrRegLcdYCompare);
        if(lcdy == lcdyc && mmu->ReadIORegisterBit(AddrRegLcdStatus, FlagLcdStatusLcdYCInterruptOn))
            mmu->WriteIORegisterBit(AddrRegInterruptFlag, FlagInterruptLcd, true);
        mmu->WriteIORegisterBit(AddrRegLcdStatus, FlagLcdStatusCoincidence, (lcdy == lcdyc)); 
    }    
}
    
void PixelProcessingUnit::processHBlank() {
    if(cycleCount >= CyclesHBlank) {
        // printf("HBlank\n");
        uint8_t lcdy = mmu->Read(AddrRegLcdY);
        writeScanLine(lcdy);
        mmu->Write(AddrRegLcdY, lcdy + 1);
        lcdy++;

        cycleCount = cycleCount % CyclesHBlank;

        if (lcdy == 144) {
            setLCDMode(VBlank);
            mmu->WriteIORegisterBit(AddrRegInterruptFlag, FlagInterruptVBlank, true);
        } else {
            setLCDMode(Oam);
        }
    }       
}
    
void PixelProcessingUnit::processVBlank() {
    if(cycleCount >= CyclesVBlank) {
        // printf("VBlank\n");
        uint8_t lcdy = mmu->Read(AddrRegLcdY);
        mmu->Write(AddrRegLcdY, lcdy + 1);
        lcdy++;
        cycleCount = cycleCount % CyclesVBlank; 

        if (lcdy == 154) {
            HasFrameBufferUpdated = true;
            memcpy(&FrameBuffer, &localFrameBuffer, sizeof(localFrameBuffer));
            mmu->Write(AddrRegLcdY, 0);
            setLCDMode(Oam);
        };
    }    
}

void PixelProcessingUnit::writeScanLine(uint8_t line) {
    if(!mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlLcdOn))
        return;

    if(mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlBgOn))
        writeBGLine(line);

    if(mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlWindowOn))
        writeWindowLine(line);
} 

void PixelProcessingUnit::writeBGLine(uint8_t line) {
    bool use_tile_set_zero = mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlBgData);
    bool use_tile_map_zero = !mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlBgMap);

    uint16_t tile_set_address = use_tile_set_zero ? AddrTileData1Start : AddrTileData0Start;
    uint16_t tile_map_address = use_tile_map_zero ? AddrBgMap0Start : AddrBgMap1Start;

    uint screen_y = line;

    for (uint screen_x = 0; screen_x < 160; screen_x++) {
        uint scrolled_x = screen_x + mmu->Read(AddrRegScrollX);
        uint scrolled_y = screen_y + mmu->Read(AddrRegScrollY);
        uint bg_map_x = scrolled_x % 256;
        uint by_map_y = scrolled_y % 256;
        uint tile_x = bg_map_x / 8;
        uint tile_y = by_map_y / 8;
        uint tile_pixel_x = bg_map_x % 8;
        uint tile_pixel_y = by_map_y % 8;

        /* Work out the address of the tile ID from the tile map */
        uint tile_index = tile_y * 32 + tile_x;
        uint16_t tile_id_address = tile_map_address + tile_index;
        uint8_t tile_id = mmu->Read(tile_id_address);

        uint tile_data_mem_offset = use_tile_set_zero ? tile_id * 16
            : (static_cast<int8_t>(tile_id) + 128) * 16;

        uint tile_data_line_offset = tile_pixel_y * 2;
        uint16_t tile_line_data_start_address = tile_set_address + tile_data_mem_offset + tile_data_line_offset;

        uint8_t pixels_1 = mmu->Read(tile_line_data_start_address);
        uint8_t pixels_2 = mmu->Read(tile_line_data_start_address + 1);

        uint8_t req_bit = 7 - (tile_pixel_x % 8);
        uint8_t bit1 = (pixels_1 >> req_bit) & 1;
        uint8_t bit2 = (pixels_2 >> req_bit) & 1;
        uint8_t colorid = (bit1 << 1) | bit2;
        int color = getColor(colorid, AddrRegBgPalette);

        int red, blue, green;
        if (color == 0) // white
            red = 0x9b, blue = 0xbc, green = 0x0f;
        else if (color == 1) // light gray
            red = 0x8b, blue = 0xac, green = 0x0f;
        else if (color == 2) // dark gray
            red = 0x30, blue = 0x62, green = 0x30;
        else
            red = 0x0f, blue = 0x38, green = 0x0f;

        localFrameBuffer[screen_x][screen_y][0] = red;
        localFrameBuffer[screen_x][screen_y][1] = blue;
        localFrameBuffer[screen_x][screen_y][2] = green;
    }
}

void PixelProcessingUnit::writeWindowLine(uint8_t line) {
    printf("writeWindowLine\n");
    bool use_tile_set_zero = mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlBgData);
    bool use_tile_map_zero = !mmu->ReadIORegisterBit(AddrRegLcdControl, FlagLcdControlWindowMap);

    uint16_t tile_set_address = use_tile_set_zero ? AddrTileData1Start : AddrTileData0Start;
    uint16_t tile_map_address = use_tile_map_zero ? AddrBgMap0Start : AddrBgMap1Start;

    uint screen_y = line;
    uint8_t window_x = mmu->Read(AddrRegWindowX);
    uint8_t window_y = mmu->Read(AddrRegWindowY);
    uint scrolled_y = screen_y - window_y;

    if (scrolled_y >= 144) { return; }

    for (uint screen_x = 0; screen_x < 160; screen_x++) {
        uint scrolled_x = screen_x + window_x - 7;

        uint tile_x = scrolled_x / 8;
        uint tile_y = scrolled_y / 8;
        uint tile_pixel_x = scrolled_x % 8;
        uint tile_pixel_y = scrolled_y % 8;

        uint tile_index = tile_y * 32 + tile_x;
        uint16_t tile_id_address = tile_map_address + tile_index;

        uint8_t tile_id = mmu->Read(tile_id_address);
        uint tile_data_mem_offset = use_tile_set_zero
            ? tile_id * 16
            : (static_cast<int8_t>(tile_id) + 128) * 16;

        uint tile_data_line_offset = tile_pixel_y * 2;
        uint16_t tile_line_data_start_address = tile_set_address + tile_data_mem_offset + tile_data_line_offset;

        uint8_t pixels_1 = mmu->Read(tile_line_data_start_address);
        uint8_t pixels_2 = mmu->Read(tile_line_data_start_address + 1);

        uint8_t req_bit = 7 - (tile_pixel_x % 8);
        uint8_t bit1 = (pixels_1 >> req_bit) & 1;
        uint8_t bit2 = (pixels_2 >> req_bit) & 1;
        uint8_t colorid = (bit1 << 1) | bit2;
        int color = getColor(colorid, AddrRegBgPalette);

        int red, blue, green;
        if (color == 0) // white
            red = 0x9b, blue = 0xbc, green = 0x0f;
        else if (color == 1) // light gray
            red = 0x8b, blue = 0xac, green = 0x0f;
        else if (color == 2) // dark gray
            red = 0x30, blue = 0x62, green = 0x30;
        else
            red = 0x0f, blue = 0x38, green = 0x0f;

        localFrameBuffer[screen_x][screen_y][0] = red;
        localFrameBuffer[screen_x][screen_y][1] = blue;
        localFrameBuffer[screen_x][screen_y][2] = green;
    }
}

int PixelProcessingUnit::getColor(int id, uint16_t palette) {
    uint8_t data = mmu->Read(palette);
    int hi = 2 * id + 1, lo = 2 * id;
    int bit1 = (data >> hi) & 1;
    int bit0 = (data >> lo) & 1;
    return (bit1 << 1) | bit0;
}