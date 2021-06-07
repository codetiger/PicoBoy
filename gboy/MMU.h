#pragma once

#include <vector>
#include "Cartridge.h"

class MemoryManagementUnit {
public:
    MemoryManagementUnit(Cartridge* cart);

    uint8_t Read(uint16_t addr);
    void Write(uint16_t addr, uint8_t data, bool isPPUWrite = false);

    bool ReadIORegisterBit(uint16_t addr, uint8_t flag);
    void WriteIORegisterBit(uint16_t addr, uint8_t flag, bool value);
private:
    void loadBIOS();
    void LoadDMA(uint8_t value);

    Cartridge *cartridge;
    uint8_t memory[0x10000];
};

const uint16_t AddrRegLcdControl = 0xFF40;
const uint16_t AddrRegLcdStatus = 0xFF41;
const uint16_t AddrRegScrollY = 0xFF42;
const uint16_t AddrRegScrollX = 0xFF43;
const uint16_t AddrRegLcdY = 0xFF44;
const uint16_t AddrRegLcdYCompare = 0xFF45;    
const uint16_t AddrRegDma = 0xFF46;
const uint16_t AddrRegBgPalette = 0xFF47;   
const uint16_t AddrRegSprite0Palette = 0xFF48;
const uint16_t AddrRegSprite1Palette = 0xFF49;
const uint16_t AddrRegWindowY = 0xFF4a;
const uint16_t AddrRegWindowX = 0xFF4b;

const uint16_t AddrTileData0Start = 0x8800;
const uint16_t AddrTileData1Start = 0x8000;
const uint16_t AddrBgMap0Start = 0x9800;
const uint16_t AddrBgMap1Start = 0x9C00;     
const uint16_t AddrOAMStart = 0xFE00;

// Interrupt Vectors
const uint16_t AddrVectorVBlank = 0x40;
const uint16_t AddrVectorLcd = 0x48;
const uint16_t AddrVectorTimer = 0x50;
const uint16_t AddrVectorSerial = 0x58;
const uint16_t AddrVectorInput = 0x60;

const uint16_t AddrRegTIMA = 0xFF05;
const uint16_t AddrRegTMA = 0xFF06;
const uint16_t AddrRegTAC = 0xFF07;
const uint16_t AddrRegInterruptFlag = 0xFF0F;
const uint16_t AddrRegInterruptEnabled = 0xFFFF;

const uint8_t FlagInterruptInput = 5;    
const uint8_t FlagInterruptSerial = 4;
const uint8_t FlagInterruptTimer = 3;
const uint8_t FlagInterruptLcd = 2;
const uint8_t FlagInterruptVBlank = 1;

const uint8_t FlagLcdStatusLcdYCInterruptOn     = 6;   // Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
const uint8_t FlagLcdStatusOamInterruptOn       = 5;   // Bit 5 - Mode 2 OAM Interrupt         (1=Enable) (Read/Write)
const uint8_t FlagLcdStatusVBlankInterruptOn    = 4;   // Bit 4 - Mode 1 V-Blank Interrupt     (1=Enable) (Read/Write)
const uint8_t FlagLcdStatusHBlankInterruptOn    = 3;   // Bit 3 - Mode 0 H-Blank Interrupt     (1=Enable) (Read/Write)
const uint8_t FlagLcdStatusCoincidence          = 2;   // Bit 2 - Coincidence Flag  (0:LYC<>LY, 1:LYC=LY) (Read Only)
const uint8_t FlagLcdStatusModeHigh             = 1;   //Bit 1-0 - Mode Flag       (Mode 0-3, see below) (Read Only)
const uint8_t FlagLcdStatusModeLow              = 0;   //        00: During H-Blank
                                                            //        01: During V-Blank
                                                            //        10: During Searching OAM-RAM
                                                            //        11: During Transfering Data to LCD Driver

const uint8_t FlagLcdControlLcdOn       = 7;   // Bit 7 - LCD Display Enable             (0=Off, 1=On)
const uint8_t FlagLcdControlWindowMap   = 6;   // Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
const uint8_t FlagLcdControlWindowOn    = 5;   // Bit 5 - Window Display Enable          (0=Off, 1=On)
const uint8_t FlagLcdControlBgData      = 4;   // Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
const uint8_t FlagLcdControlBgMap       = 3;   // Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
const uint8_t FlagLcdControlObjSize     = 2;   // Bit 2 - OBJ (Sprite) Size              (0=8x8, 1=8x16)
const uint8_t FlagLcdControlObjOn       = 1;   // Bit 1 - OBJ (Sprite) Display Enable    (0=Off, 1=On)
const uint8_t FlagLcdControlBgOn        = 0;   // Bit 0 - BG Display                     (0=Off, 1=On)
