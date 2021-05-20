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

    Cartridge *cartridge;
    std::vector<uint8_t> memory;
};

MemoryManagementUnit::MemoryManagementUnit(Cartridge* cart) {
    printf("Creating Memory for MMU\n");
    cartridge = cart;
    memory = std::vector<uint8_t>(0x10000);
    loadBIOS();
}

uint8_t MemoryManagementUnit::Read(uint16_t addr) {
    if (addr >= 0x0 && addr <= 0x7FFF) {
        if (addr <= 0xFF && memory[0xFF50] != 0x1)
            return memory[addr];

        // printf("Reading Cartridge: 0x%04x Data: 0x%02x\n", addr, cartridge->Read(addr));
        return cartridge->Read(addr);
    }

    return memory[addr];
}

void MemoryManagementUnit::Write(uint16_t addr, uint8_t data) {
    if(addr == 0xFF50)
        printf("Disabling boot procedure\n");
    memory[addr] = data;
}

void MemoryManagementUnit::loadBIOS() {
    std::string path = "./roms/bios.gb";
    printf("Loading Bios: %s\n", path.c_str());
    std::ifstream biosFile;
    biosFile.open(path, std::ifstream::binary);
    biosFile.seekg(0, biosFile.beg);

    uint8_t bios[0x100];
    biosFile.read((char*)&bios[0], 256);
    biosFile.close();

    printf("Loading BIOS Data to MMU\n");
    memcpy(&memory[0x0], &bios[0], sizeof(bios));
}

bool MemoryManagementUnit::ReadIORegisterBit(uint16_t addr, uint8_t flag) { 
    return memory[addr] & flag; 
}

void MemoryManagementUnit::WriteIORegisterBit(uint16_t addr, uint8_t flag, bool value) { 
    if(value) 
        memory[addr] |= flag;
    else 
        memory[addr] &= ~flag;
}
