#pragma once

#include <vector>

const uint16_t AddrRegTIMA = 0xFF05;
const uint16_t AddrRegTMA = 0xFF06;
const uint16_t AddrRegTAC = 0xFF07;
const uint16_t AddrRegInterruptFlag = 0xFF0F;
const uint16_t AddrRegInterruptEnabled = 0xFFFF;

const uint8_t FlagTimerClockMode = 3;
const uint8_t FlagTimerStart = 4;

const uint8_t FlagInterruptInput = 16;    
const uint8_t FlagInterruptSerial = 8;
const uint8_t FlagInterruptTimer = 4;
const uint8_t FlagInterruptLcd = 2;
const uint8_t FlagInterruptVBlank = 1;


const uint32_t CyclesCpu = 4194304;
const uint32_t CyclesFrame = 70368;   
const uint16_t CyclesHBlank = 204;     // Mode 0 (H-Blank) 204 cycles per Scanline
const uint16_t CyclesVBlank = 456;     // Mode 1 (V-Blank) 4560 cycles per Frame 4560/10 times per Frame
const uint16_t CyclesOam = 80;         // Mode 2 (OAM Search) 80 cycles per Scanline
const uint16_t CyclesTransfer = 173;   // Mode 3 (Transfer LCD) 173 cycles per Scanline
const uint16_t Cycles256Hz = CyclesCpu / 256;
const uint16_t Cycles128Hz = CyclesCpu / 128;
const uint32_t Cycles64Hz = CyclesCpu / 64;

const int64_t OneFrameDurationNSec = 1000000000 / (CyclesCpu / CyclesFrame);

const uint16_t AddrRegLcdControl = 0xFF40;
const uint16_t AddrRegLcdStatus = 0xFF41;
const uint16_t AddrRegScrollY = 0xFF42;
const uint16_t AddrRegScrollX = 0xFF43;
const uint16_t AddrRegWindowY = 0xFF4a;
const uint16_t AddrRegWindowX = 0xFF4b;
const uint16_t AddrRegLcdY = 0xFF44;
const uint16_t AddrRegLcdYCompare = 0xFF45;    
const uint16_t AddrRegDma = 0xFF46;
const uint16_t AddrRegBgPalette = 0xFF47;   // Bit 7-6 - Data for Dot Data 11 (Normally darkest color
                                            // Bit 5-4 - Data for Dot Data 10
                                            // Bit 3-2 - Data for Dot Data 01
                                            // Bit 1-0 - Data for Dot Data 00 (Normally lightest color)

const uint8_t FlagLcdStatusLcdYCInterruptOn = 64;    // Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
const uint8_t FlagLcdStatusOamInterruptOn = 32;      // Bit 5 - Mode 2 OAM Interrupt         (1=Enable) (Read/Write)
const uint8_t FlagLcdStatusVBlankInterruptOn = 16;   // Bit 4 - Mode 1 V-Blank Interrupt     (1=Enable) (Read/Write)
const uint8_t FlagLcdStatusHBlankInterruptOn = 8;    // Bit 3 - Mode 0 H-Blank Interrupt     (1=Enable) (Read/Write)
const uint8_t FlagLcdStatusCoincidence = 4;          // Bit 2 - Coincidence Flag  (0:LYC<>LY, 1:LYC=LY) (Read Only)
const uint8_t FlagLcdStatusModeHigh = 2;             //Bit 1-0 - Mode Flag       (Mode 0-3, see below) (Read Only)
const uint8_t FlagLcdStatusModeLow = 1;             //        00: During H-Blank
                                                    //        01: During V-Blank
                                                    //        10: During Searching OAM-RAM
                                                    //        11: During Transfering Data to LCD Driver

const uint8_t FlagLcdControlLcdOn = 128;     // Bit 7 - LCD Display Enable             (0=Off, 1=On)
const uint8_t FlagLcdControlWindowMap = 64;  // Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
const uint8_t FlagLcdControlWindowOn = 32;   // Bit 5 - Window Display Enable          (0=Off, 1=On)
const uint8_t FlagLcdControlBgData = 16;     // Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
const uint8_t FlagLcdControlBgMap = 8;       // Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
const uint8_t FlagLcdControlObjSize = 4;     // Bit 2 - OBJ (Sprite) Size              (0=8x8, 1=8x16)
const uint8_t FlagLcdControlObjOn = 2;       // Bit 1 - OBJ (Sprite) Display Enable    (0=Off, 1=On)
const uint8_t FlagLcdControlBgOn = 1;        // Bit 0 - BG Display                     (0=Off, 1=On)


const uint16_t AddrTileData0Start = 0x8800;
const uint16_t AddrTileData0End = 0x97FF;
const uint16_t AddrTileData1Start = 0x8000;
const uint16_t AddrTileData1End = 0x8FFF;
const uint16_t AddrBgMap0Start = 0x9800;
const uint16_t AddrBgMap0End = 0x9BFF;
const uint16_t AddrBgMap1Start = 0x9C00;     
const uint16_t AddrBgMap1End = 0x9FFF;
const uint16_t AddrOAMStart = 0xFE00;
const uint16_t AddrOAMEnd = 0xFE9F;


// Interrupt Vectors
const uint16_t AddrVectorVBlank = 0x40;
const uint16_t AddrVectorLcd = 0x48;
const uint16_t AddrVectorTimer = 0x50;
const uint16_t AddrVectorSerial = 0x58;
const uint16_t AddrVectorInput = 0x60;


// const uint8_t 