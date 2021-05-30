#include "Cartridge.h"

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