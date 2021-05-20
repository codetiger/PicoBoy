
#pragma once

#include <string>
#include <vector>
#include <fstream>

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

Cartridge::Cartridge(const std::string path) {
    selectedBank = 1;
    supported = false;
    cartridgeSize = 0;

    printf("Loading Cartridge: %s\n", path.c_str());
    std::ifstream cartridgeFile;
    cartridgeFile.open(path, std::ifstream::binary);
        
    cartridgeFile.seekg(0, cartridgeFile.end);
    cartridgeSize = (unsigned long) cartridgeFile.tellg();
    cartridgeFile.seekg(0, cartridgeFile.beg);

    printf("Cartridge Size: %ld\n", cartridgeSize);
    data = std::vector<uint8_t>(cartridgeSize);
    cartridgeFile.read((char*)&data[0], cartridgeSize);
    cartridgeFile.close();

    supported = data[AddrCartType] >= CartTypeRom && data[AddrCartType] <= CartTypeMBC1;
    printf("Cartridge Supported: %d\n", supported);
}

Cartridge::~Cartridge() {
}

uint8_t Cartridge::Read(const uint16_t addr) {
    if(addr >= 0x4000 && addr <= 0x7FFF)
        return data[addr + ((selectedBank - 1) * 0x4000)];
    else
        return data[addr];
}

void Cartridge::selectRomBank(const uint8_t bank) {
    selectedBank = bank;
}