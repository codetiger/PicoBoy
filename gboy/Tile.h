#pragma once

#include "MMU.h"

class Tile {
private:
    MemoryManagementUnit *mmu;
    int buffer[8 * 8 * 2];
    uint8_t pixel_index(uint8_t x, uint8_t y);
    std::vector<uint8_t> get_pixel_line(uint8_t byte1, uint8_t byte2);
public:
    Tile(uint16_t addr, MemoryManagementUnit *mmu, uint8_t size_multiplier = 1);
    ~Tile();
    uint8_t get_pixel(uint8_t x, uint8_t y);
};
