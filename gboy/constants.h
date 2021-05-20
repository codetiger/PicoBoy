#pragma once

#include <vector>

const uint16_t AddrRegTIMA = 0xFF05;
const uint16_t AddrRegTMA = 0xFF06;
const uint16_t AddrRegTAC = 0xFF07;
const uint16_t AddrRegInterruptFlag = 0xFF0F;

const uint8_t FlagInterruptTimer = 4;
const uint8_t FlagTimerClockMode = 3;
const uint8_t FlagTimerStart = 4;

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
