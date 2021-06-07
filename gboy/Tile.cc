#include "Tile.h"

Tile::Tile(uint16_t addr, MemoryManagementUnit *mmu, uint8_t size_multiplier) {
    for (uint8_t x = 0; x < 8; x++) {
        for (uint8_t y = 0; y < 8 * size_multiplier; y++) {
            buffer[pixel_index(x, y)] = 0;
        }
    }

    for (uint8_t tile_line = 0; tile_line < 8 * size_multiplier; tile_line++) {
        uint8_t index_into_tile = 2 * tile_line;
        uint16_t line_start = addr + index_into_tile;

        uint8_t pixels_1 = mmu->Read(line_start);
        uint8_t pixels_2 = mmu->Read(line_start + 1);

        std::vector<uint8_t> pixel_line = get_pixel_line(pixels_1, pixels_2);

        for (uint8_t x = 0; x < 8; x++)
            buffer[pixel_index(x, tile_line)] = pixel_line[x];
    }
}

uint8_t Tile::pixel_index(uint8_t x, uint8_t y) {
    return (y * 8) + x;
}

Tile::~Tile() {
}

std::vector<uint8_t> Tile::get_pixel_line(uint8_t byte1, uint8_t byte2) {
    std::vector<uint8_t> pixel_line;
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t req_bit = 7 - i;
        uint8_t bit1 = (byte1 >> req_bit) & 1;
        uint8_t bit2 = (byte2 >> req_bit) & 1;
        uint8_t colorid = (bit2 << 1) | bit1;
        pixel_line.push_back(colorid);
    }

    return pixel_line;
}

uint8_t Tile::get_pixel(uint8_t x, uint8_t y) {
    return buffer[pixel_index(x, y)];
}
