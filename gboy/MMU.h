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

MemoryManagementUnit::MemoryManagementUnit(Cartridge* cart) {
    cartridge = cart;
    loadBIOS();
}

uint8_t MemoryManagementUnit::Read(uint16_t addr) {
    if (addr <= 0x7FFF) {
        if (addr <= 0xFF && memory[0xFF50] != 0x1)
            return memory[addr];

        // printf("Reading Cartridge: 0x%04x Data: 0x%02x\n", addr, cartridge->Read(addr));
        return cartridge->Read(addr);
    } else if(0x8000 <= addr && addr <= 0x9FFF) {
        // printf("Reading from: 0x%04x\n", addr);
        return memory[addr]; // VRAM
    } else if(0xA000 <= addr && addr <= 0xBFFF) {
        printf("Reading from: 0x%04x\n", addr);
        return memory[addr]; // External Cartridge RAM
    } else if(0xC000 <= addr && addr <= 0xDFFF) {
        // printf("Reading from: 0x%04x\n", addr);
        return memory[addr]; // Internal Work RAM
    } else if(0xE000 <= addr && addr <= 0xFDFF) {
        printf("Reading from: 0x%04x\n", addr);
        return memory[addr-0x2000]; // Mirrored Work RAM
    } else if(0xFE00 <= addr && addr <= 0xFE9F) {
        printf("Reading from: 0x%04x\n", addr);
        return memory[addr]; // OAM
    } else if(0xFEA0 <= addr && addr <= 0xFEFF) {
        printf("Reading from: 0x%04x\n", addr);
        return 0xFF; // Unusable
    } else if(0xFF00 <= addr && addr <= 0xFF7F) {
        return memory[addr]; // Mapped IO
    } else if(0xFF80 <= addr && addr <= 0xFFFE) {
        return memory[addr]; // Zero Page RAM
    } else if(addr == 0xFFFF) {
        // printf("Reading from: 0x%04x\n", addr);
        return memory[addr]; // Interrupt Enable Register
    } 
    
    printf("Attempting to read from unmapped memory location\n");
    return 0x0;
}

void MemoryManagementUnit::Write(uint16_t addr, uint8_t data) {
    if (addr == AddrRegDma) {
        // printf("Loading DMA\n");
        LoadDMA(data);
    } else if(addr < 0x8000 || (addr >= 0xfea0 && addr < 0xfeff)) {
        // printf("Attempting to write to readonly: 0x%04x\n", addr);
        return; // Read only area
    } else if(0x8000 <= addr && addr <= 0x9FFF) {
        // printf("Writing to: 0x%04x\n", addr);
        memory[addr] = data; // VRAM
    } else if(addr >= 0xe000 && addr < 0xfe00) {
        memory[addr] = data;
        memory[addr - 0x2000] = data; //echo RAM
    } else {
        if(addr == 0xFF50)
            printf("Disabling boot procedure\n");
        memory[addr] = data;
    }
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

    memcpy(&memory, &bios, sizeof(bios));
    printf("Loaded BIOS Data to MMU\n");
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

void MemoryManagementUnit::LoadDMA(uint8_t value) {
    uint16_t addr = value * 0x0100;
	for (int i = 0x0; i < 0xa0; i++)
        memory[0xfe00 + i] = memory[addr + i];
}
