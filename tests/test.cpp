#include <iostream>
#include <string>
#include <sstream>

#include "../gboy/CPU.h"
#include "json.hpp"

using json = nlohmann::json;

uint16_t convertHexaString(std::string hexstr) {
    uint16_t res = 0;
    std::stringstream ss;
    ss << std::hex << hexstr;
    ss >> res;
    return res;
}

void loadCPURegisters(CentralProcessingUnit* cpu, json registers) {
    if(registers.is_null()) 
        return;

    cpu->b = registers["b"];
    cpu->c = registers["c"];
    cpu->d = registers["d"];
    cpu->e = registers["e"];
    cpu->h = registers["h"];
    cpu->l = registers["l"];
    cpu->accumulator = registers["a"];
}

void loadCPUFlags(CentralProcessingUnit* cpu, json flags) {
    if(flags.is_null()) 
        return;

    cpu->isZero = flags["z"];
    cpu->isSubtract = flags["n"];
    cpu->isHalfCarry = flags["h"];
    cpu->isCarry = flags["c"];
}

void loadMemory(MemoryManagementUnit* mmu, json memory) {
    if(memory.is_null()) 
        return;

    for (auto& element : memory) {
        uint16_t address = convertHexaString(element["at"]);
        uint16_t count = element["next"];
        uint8_t value = element["value"];
        for (int i = 0; i < count; i++) {
            mmu->Write(address + i, value);
        }
    }
}

void checkCPURegisters(CentralProcessingUnit* cpu, json registers) {
    if(registers.is_null()) 
        return;

    if (cpu->b != registers["b"])
        printf("[Register check failed] %d expected in register b but found %d\n", registers["b"].get<int>(), cpu->b);

    if (cpu->c != registers["c"])
        printf("[Register check failed] %d expected in register c but found %d\n", registers["c"].get<int>(), cpu->c);

    if (cpu->d != registers["d"])
        printf("[Register check failed] %d expected in register d but found %d\n", registers["d"].get<int>(), cpu->d);

    if (cpu->e != registers["e"])
        printf("[Register check failed] %d expected in register e but found %d\n", registers["e"].get<int>(), cpu->e);

    if (cpu->h != registers["h"])
        printf("[Register check failed] %d expected in register h but found %d\n", registers["h"].get<int>(), cpu->h);

    if (cpu->l != registers["l"])
        printf("[Register check failed] %d expected in register l but found %d\n", registers["l"].get<int>(), cpu->l);

    if (cpu->accumulator != registers["a"])
        printf("[Register check failed] %d expected in register a but found %d\n", registers["a"].get<int>(), cpu->accumulator);
}

void checkCPUFlags(CentralProcessingUnit* cpu, json flags) {
    if(flags.is_null()) 
        return;

    if (cpu->isZero != flags["z"])
        printf("[Flag check failed] %d expected in zero flag but found %d\n", flags["z"].get<int>(), cpu->isZero);

    if (cpu->isSubtract != flags["n"])
        printf("[Flag check failed] %d expected in subtract flag but found %d\n", flags["n"].get<int>(), cpu->isSubtract);

    if (cpu->isHalfCarry != flags["h"])
        printf("[Flag check failed] %d expected in half carry flag but found %d\n", flags["h"].get<int>(), cpu->isHalfCarry);

    if (cpu->isCarry != flags["c"])
        printf("[Flag check failed] %d expected in half carry flag but found %d\n", flags["c"].get<int>(), cpu->isCarry);
}

void checkMemory(MemoryManagementUnit* mmu, json memory) {
    if(memory.is_null()) 
        return;

    for (auto& element : memory) {
        uint16_t address = convertHexaString(element["at"]);
        uint8_t value = element["value"];
        uint8_t v = mmu->Read(address, true);
        if(v != value)
            printf("[Mem check failed] %d expected at 0x%04x but found %d\n", value, address, v);
    }
}

int main(int argc, char *argv[]) {
    Cartridge *cart = new Cartridge("../roms/tetris.gb");
    MemoryManagementUnit *mmu = new MemoryManagementUnit(cart);
    CentralProcessingUnit *cpu = new CentralProcessingUnit(mmu); 

    std::ifstream testConditionsFile("gb-dmg.json");
    json j;
    testConditionsFile >> j;
    cpu->Print();

    for (auto& element : j["tests"]) {
        auto preload = element["preload"];
        std::cout << "Testing opcode " << element["opcodedata"][0] << "\n";

        loadCPURegisters(cpu, preload["register"]);
        loadCPUFlags(cpu, preload["flag"]);
        loadMemory(mmu, preload["memory"]);
        cpu->stackPointer = convertHexaString(preload["sp"]);
        cpu->programCounter = convertHexaString(preload["pc"]);

        for (uint8_t i = 0; i < element["opcodedata"].size(); i++) {
            uint8_t v = convertHexaString(element["opcodedata"][i]);
            mmu->Write(cpu->programCounter + i, v, true);
        }

        auto check = element["check"];
        uint8_t cycles = cpu->ExecuteInstruction();
        if(cycles != check["cycles"]) {
            printf("[CPU Cycles check failed] %d expected but got %d\n", check["cycles"].get<int>(), cycles);
        }

        checkCPURegisters(cpu, check["register"]);
        checkCPUFlags(cpu, check["flag"]);
        checkMemory(mmu, check["memory"]);
        if(check.contains("sp") && cpu->stackPointer != convertHexaString(check["sp"])) 
            printf("[CPU StackPointer check failed] %s expected but got 0x%04x\n", check["sp"].get<std::string>().c_str(), cpu->stackPointer);
        if(check.contains("pc") && cpu->programCounter != convertHexaString(check["pc"])) 
            printf("[CPU ProgramCounter check failed] %s expected but got 0x%04x\n", check["pc"].get<std::string>().c_str(), cpu->programCounter);

        cpu->Print();
    }
    
    return 0;
}