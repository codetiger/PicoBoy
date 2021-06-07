#pragma once

#include <vector>

const uint32_t CyclesCpu = 4194304;
const uint32_t CyclesFrame = 70368;   
const uint16_t Cycles256Hz = CyclesCpu / 256;
const uint16_t Cycles128Hz = CyclesCpu / 128;
const uint32_t Cycles64Hz = CyclesCpu / 64;

const int64_t OneFrameDurationNSec = 1000000000 / (CyclesCpu / CyclesFrame);



// const uint8_t 