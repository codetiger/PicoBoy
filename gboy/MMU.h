#pragma once

#include <vector>
#include "Cartridge.h"

class MemoryManagementUnit {
public:
    MemoryManagementUnit(Cartridge* cart);

    uint8_t Read(uint16_t addr);
    void Write(uint16_t addr, uint8_t data);

    bool ReadIORegisterBit(uint16_t addr, uint8_t flag);
    void WriteIORegisterBit(uint16_t addr, uint8_t flag, bool value);
private:
    void loadBIOS();
    void LoadDMA(uint8_t value);

    Cartridge *cartridge;
    uint8_t memory[0x10000];
};
