#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "constants.h"

class Cartridge
{
private:
    std::vector<uint8_t> data;
    uint8_t selectedBank;
    long cartridgeSize;
    bool supported;
public:
    Cartridge(const std::string path);
    ~Cartridge();

    uint8_t Read(const uint16_t addr);
    void selectRomBank(const uint8_t bank);
};

const uint16_t AddrCartType = 0x0147;
const uint16_t AddrCartSwitchTriggerStart = 0x2000;
const uint16_t AddrCartSwitchTriggerEnd = 0x3FFF;
const uint16_t AddrSwitchBankStart = 0x4000;
const uint16_t AddrSwitchBankEnd = 0x7FFF;

const uint8_t CartTypeRom = 0x0;
const uint8_t CartTypeMBC1 = 0x1;
