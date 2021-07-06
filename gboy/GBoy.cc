#include "GBoy.h"

GBoy::GBoy(std::string path) {
    Cartridge *cart = new Cartridge(path);
    mmu = new MemoryManagementUnit(cart);
    cpu = new CentralProcessingUnit(mmu);
    ppu = new PixelProcessingUnit(mmu);
    timer = new Timer(mmu);
}

GBoy::~GBoy() {
    delete mmu;
    delete cpu;
    delete ppu;
}

void GBoy::ExecuteStep() {
    uint8_t opCycles = cpu->ExecuteInstruction(0xffff);
    ppu->Cycle(opCycles);
    timer->Cycle(opCycles);
}

void GBoy::GetFrameBufferColor(uint8_t &red, uint8_t &green, uint8_t &blue, uint8_t x, uint8_t y) {
    red = ppu->FrameBuffer[x][y][0];
    green = ppu->FrameBuffer[x][y][1];
    blue = ppu->FrameBuffer[x][y][2];
}

bool GBoy::GetFrameBufferUpdatedFlag() {
    return ppu->HasFrameBufferUpdated;
}

void GBoy::SetFrameBufferUpdatedFlag(bool v) {
    ppu->HasFrameBufferUpdated = v;
}
