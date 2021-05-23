#pragma once

#include <vector>
#include <map>
#include "MMU.h"

class CentralProcessingUnit {
private:
    uint8_t accumulator;
	uint8_t b, c, d, e, h, l;
    uint16_t stackPointer;
    uint16_t time, deltaTime;
    bool interruptMasterFlag;

    bool isZero, isSubtract, isCarry, isHalfCarry;
    bool isHalted;
    MemoryManagementUnit *mmu;

    class Instruction {
    public:
        std::string name;
        int size;
        int cycles;
        bool (CentralProcessingUnit::*code)(uint8_t*);

        Instruction(std::string name, int size, int cycles, bool(CentralProcessingUnit::*c)(uint8_t*)) {
            this->code = c;
            this->name = name;
            this->size = size;
            this->cycles = cycles;
        }
    };

    std::map<uint16_t, Instruction*> instructionSet;

    uint8_t getFlags();
    void setFlags(uint8_t val);

    bool instruction_NOP(uint8_t* data);
    bool instruction_XOROP(uint8_t* data);
    bool instruction_BIT7(uint8_t* data);

    bool instruction_Inc(uint8_t* data);
    bool instruction_Dec(uint8_t* data);
    bool instruction_Inc16Bit(uint8_t* data);
    bool instruction_Dec16Bit(uint8_t* data);    
    
    bool instruction_LoadSP(uint8_t* data);
    bool instruction_LoadPair(uint8_t* data);
    bool instruction_Load(uint8_t* data);
    bool instruction_LoadMem2Reg(uint8_t* data);
    bool instruction_LoadReg2Mem(uint8_t* data);
    bool instruction_LoadAIndirect(uint8_t* data);
    bool instruction_LoadA2Mem(uint8_t* data);
    bool instruction_LoadReg2Reg(uint8_t* data);

    bool instruction_JR(uint8_t* data);
    bool instruction_Call(uint8_t* data);
    bool instruction_Push(uint8_t* data);
    bool instruction_Pop(uint8_t* data);
    bool instruction_Rotate(uint8_t* data);
    bool instruction_Return(uint8_t* data);
    bool instruction_Reti(uint8_t* data);
    
    bool instruction_Compare(uint8_t* data);
    bool instruction_LoadAnn(uint8_t* data);

    bool instruction_Sub(uint8_t* data);
    bool instruction_Add(uint8_t* data);
    bool instruction_Jump(uint8_t* data);
    bool instruction_LoadAHL(uint8_t* data);
    bool instruction_JumpHL(uint8_t* data);

    bool instruction_OR(uint8_t* data);
    bool instruction_AND(uint8_t* data);
    bool instruction_CPL(uint8_t* data);
    bool instruction_Swap(uint8_t* data);
    bool instruction_Reset(uint8_t* data);
    bool instruction_AddPair(uint8_t* data);

    bool instruction_SRL(uint8_t* data);

    bool instruction_ResetBit0(uint8_t* data);

    bool instruction_SetInterrupt(uint8_t* data);
    void handleInterrupts();
    void serviceInterrupts(uint16_t addr, uint8_t flag);

public:
	uint16_t ProgramCounter;

    CentralProcessingUnit(MemoryManagementUnit *mmu);
    ~CentralProcessingUnit();
    void Print();

    uint8_t ExecuteInstruction(uint16_t skipDebug);
};

CentralProcessingUnit::CentralProcessingUnit(MemoryManagementUnit *mmu) {
    this->mmu = mmu;
    this->isHalted = false;
    this->ProgramCounter = 0x0;
    isZero = isSubtract = isCarry = isHalfCarry = false;
    this->stackPointer = 0x0;
    this->time = this->deltaTime = 0;
    this->interruptMasterFlag = false;
    instructionSet.insert(std::make_pair(0x0, new Instruction("NOP", 1, 4, &CentralProcessingUnit::instruction_NOP)));
    instructionSet.insert(std::make_pair(0x31, new Instruction("LD SP,nn", 3, 12, &CentralProcessingUnit::instruction_LoadSP)));

    instructionSet.insert(std::make_pair(0xa8, new Instruction("XOR B", 1, 4, &CentralProcessingUnit::instruction_XOROP)));
    instructionSet.insert(std::make_pair(0xa9, new Instruction("XOR C", 1, 4, &CentralProcessingUnit::instruction_XOROP)));
    instructionSet.insert(std::make_pair(0xaa, new Instruction("XOR D", 1, 4, &CentralProcessingUnit::instruction_XOROP)));
    instructionSet.insert(std::make_pair(0xab, new Instruction("XOR E", 1, 4, &CentralProcessingUnit::instruction_XOROP)));
    instructionSet.insert(std::make_pair(0xac, new Instruction("XOR H", 1, 4, &CentralProcessingUnit::instruction_XOROP)));
    instructionSet.insert(std::make_pair(0xad, new Instruction("XOR L", 1, 4, &CentralProcessingUnit::instruction_XOROP)));
    instructionSet.insert(std::make_pair(0xae, new Instruction("XOR XOR (HL)", 1, 8, &CentralProcessingUnit::instruction_XOROP)));
    instructionSet.insert(std::make_pair(0xaf, new Instruction("XOR A", 1, 4, &CentralProcessingUnit::instruction_XOROP)));

    instructionSet.insert(std::make_pair(0x01, new Instruction("LD BC nn", 3, 12, &CentralProcessingUnit::instruction_LoadPair)));
    instructionSet.insert(std::make_pair(0x11, new Instruction("LD DE nn", 3, 12, &CentralProcessingUnit::instruction_LoadPair)));
    instructionSet.insert(std::make_pair(0x21, new Instruction("LD HL nn", 3, 12, &CentralProcessingUnit::instruction_LoadPair)));

    instructionSet.insert(std::make_pair(0x32, new Instruction("LD (HL-) A", 1, 8, &CentralProcessingUnit::instruction_Load)));
    instructionSet.insert(std::make_pair(0xe2, new Instruction("LD ($FF00+C),A", 1, 8, &CentralProcessingUnit::instruction_Load)));
    instructionSet.insert(std::make_pair(0xe0, new Instruction("LD ($FF00+n),A", 2, 12, &CentralProcessingUnit::instruction_Load)));
    instructionSet.insert(std::make_pair(0x22, new Instruction("LD (HL+), A", 1, 8, &CentralProcessingUnit::instruction_Load)));

    instructionSet.insert(std::make_pair(0xcb, new Instruction("Extended Instruction", 1, 1, &CentralProcessingUnit::instruction_NOP)));

    instructionSet.insert(std::make_pair(0x177, new Instruction("BIT 7, B", 1, 8, &CentralProcessingUnit::instruction_BIT7)));
    instructionSet.insert(std::make_pair(0x178, new Instruction("BIT 7, C", 1, 8, &CentralProcessingUnit::instruction_BIT7)));
    instructionSet.insert(std::make_pair(0x179, new Instruction("BIT 7, D", 1, 8, &CentralProcessingUnit::instruction_BIT7)));
    instructionSet.insert(std::make_pair(0x17a, new Instruction("BIT 7, E", 1, 8, &CentralProcessingUnit::instruction_BIT7)));
    instructionSet.insert(std::make_pair(0x17b, new Instruction("BIT 7, H", 1, 8, &CentralProcessingUnit::instruction_BIT7)));
    instructionSet.insert(std::make_pair(0x17c, new Instruction("BIT 7, L", 1, 8, &CentralProcessingUnit::instruction_BIT7)));
    instructionSet.insert(std::make_pair(0x17d, new Instruction("BIT 7, (HL)", 1, 16, &CentralProcessingUnit::instruction_BIT7)));
    instructionSet.insert(std::make_pair(0x17e, new Instruction("BIT 7, A", 1, 8, &CentralProcessingUnit::instruction_BIT7)));
    
    instructionSet.insert(std::make_pair(0x18, new Instruction("JR n", 2, 8, &CentralProcessingUnit::instruction_JR)));
    instructionSet.insert(std::make_pair(0x20, new Instruction("JR NZ n", 2, 8, &CentralProcessingUnit::instruction_JR)));
    instructionSet.insert(std::make_pair(0x28, new Instruction("JR Z n", 2, 8, &CentralProcessingUnit::instruction_JR)));
    instructionSet.insert(std::make_pair(0x30, new Instruction("JR NC n", 2, 8, &CentralProcessingUnit::instruction_JR)));
    instructionSet.insert(std::make_pair(0x38, new Instruction("JR C n", 2, 8, &CentralProcessingUnit::instruction_JR)));

    instructionSet.insert(std::make_pair(0x06, new Instruction("LD B n", 2, 8, &CentralProcessingUnit::instruction_LoadMem2Reg)));
    instructionSet.insert(std::make_pair(0x0e, new Instruction("LD C n", 2, 8, &CentralProcessingUnit::instruction_LoadMem2Reg)));
    instructionSet.insert(std::make_pair(0x16, new Instruction("LD D n", 2, 8, &CentralProcessingUnit::instruction_LoadMem2Reg)));
    instructionSet.insert(std::make_pair(0x1e, new Instruction("LD E n", 2, 8, &CentralProcessingUnit::instruction_LoadMem2Reg)));
    instructionSet.insert(std::make_pair(0x26, new Instruction("LD H n", 2, 8, &CentralProcessingUnit::instruction_LoadMem2Reg)));
    instructionSet.insert(std::make_pair(0x2e, new Instruction("LD L n", 2, 8, &CentralProcessingUnit::instruction_LoadMem2Reg)));
    instructionSet.insert(std::make_pair(0x3e, new Instruction("LD A n", 2, 8, &CentralProcessingUnit::instruction_LoadMem2Reg)));

    instructionSet.insert(std::make_pair(0x04, new Instruction("INC B", 1, 4, &CentralProcessingUnit::instruction_Inc)));
    instructionSet.insert(std::make_pair(0x0c, new Instruction("INC C", 1, 4, &CentralProcessingUnit::instruction_Inc)));
    instructionSet.insert(std::make_pair(0x14, new Instruction("INC D", 1, 4, &CentralProcessingUnit::instruction_Inc)));
    instructionSet.insert(std::make_pair(0x1c, new Instruction("INC E", 1, 4, &CentralProcessingUnit::instruction_Inc)));
    instructionSet.insert(std::make_pair(0x24, new Instruction("INC H", 1, 4, &CentralProcessingUnit::instruction_Inc)));
    instructionSet.insert(std::make_pair(0x2c, new Instruction("INC L", 1, 4, &CentralProcessingUnit::instruction_Inc)));
    instructionSet.insert(std::make_pair(0x34, new Instruction("INC (HL)", 1, 12, &CentralProcessingUnit::instruction_Inc)));
    instructionSet.insert(std::make_pair(0x3c, new Instruction("INC A", 1, 4, &CentralProcessingUnit::instruction_Inc)));

    instructionSet.insert(std::make_pair(0x36, new Instruction("LD (HL) n", 2, 12, &CentralProcessingUnit::instruction_LoadReg2Mem)));
    instructionSet.insert(std::make_pair(0x70, new Instruction("LD (HL) B", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Mem)));
    instructionSet.insert(std::make_pair(0x71, new Instruction("LD (HL) C", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Mem)));
    instructionSet.insert(std::make_pair(0x72, new Instruction("LD (HL) D", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Mem)));
    instructionSet.insert(std::make_pair(0x73, new Instruction("LD (HL) E", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Mem)));
    instructionSet.insert(std::make_pair(0x74, new Instruction("LD (HL) H", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Mem)));
    instructionSet.insert(std::make_pair(0x75, new Instruction("LD (HL) L", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Mem)));
    instructionSet.insert(std::make_pair(0x77, new Instruction("LD (HL) A", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Mem)));

    instructionSet.insert(std::make_pair(0x0a, new Instruction("LD A (BC)", 1, 8, &CentralProcessingUnit::instruction_LoadAIndirect)));
    instructionSet.insert(std::make_pair(0x1a, new Instruction("LD A (DE)", 1, 8, &CentralProcessingUnit::instruction_LoadAIndirect)));
    instructionSet.insert(std::make_pair(0x7e, new Instruction("LD A (HL)", 1, 8, &CentralProcessingUnit::instruction_LoadAIndirect)));
    instructionSet.insert(std::make_pair(0xfa, new Instruction("LD A nn", 3, 16, &CentralProcessingUnit::instruction_LoadAIndirect)));

    instructionSet.insert(std::make_pair(0xc4, new Instruction("CALL NZ nn", 3, 12, &CentralProcessingUnit::instruction_Call)));
    instructionSet.insert(std::make_pair(0xcc, new Instruction("CALL Z nn", 3, 12, &CentralProcessingUnit::instruction_Call)));
    instructionSet.insert(std::make_pair(0xcd, new Instruction("CALL nn", 3, 24, &CentralProcessingUnit::instruction_Call)));
    instructionSet.insert(std::make_pair(0xd4, new Instruction("CALL NC nn", 3, 24, &CentralProcessingUnit::instruction_Call)));
    instructionSet.insert(std::make_pair(0xdc, new Instruction("CALL C nn", 3, 24, &CentralProcessingUnit::instruction_Call)));

    instructionSet.insert(std::make_pair(0x40, new Instruction("LD B B", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x41, new Instruction("LD B C", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x42, new Instruction("LD B D", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x43, new Instruction("LD B E", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x44, new Instruction("LD B H", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x45, new Instruction("LD B L", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x46, new Instruction("LD B (HL)", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x47, new Instruction("LD B A", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));

    instructionSet.insert(std::make_pair(0x48, new Instruction("LD C B", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x49, new Instruction("LD C C", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x4a, new Instruction("LD C D", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x4b, new Instruction("LD C E", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x4c, new Instruction("LD C H", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x4d, new Instruction("LD C L", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x4e, new Instruction("LD C (HL)", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x4f, new Instruction("LD C A", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));

    instructionSet.insert(std::make_pair(0x50, new Instruction("LD D B", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x51, new Instruction("LD D C", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x52, new Instruction("LD D D", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x53, new Instruction("LD D E", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x54, new Instruction("LD D H", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x55, new Instruction("LD D L", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x56, new Instruction("LD D (HL)", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x57, new Instruction("LD D A", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
        
    instructionSet.insert(std::make_pair(0x58, new Instruction("LD E B", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x59, new Instruction("LD E C", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x5a, new Instruction("LD E D", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x5b, new Instruction("LD E E", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x5c, new Instruction("LD E H", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x5d, new Instruction("LD E L", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x5e, new Instruction("LD E (HL)", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x5f, new Instruction("LD E A", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));

    instructionSet.insert(std::make_pair(0x60, new Instruction("LD H B", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x61, new Instruction("LD H C", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x62, new Instruction("LD H D", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x63, new Instruction("LD H E", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x64, new Instruction("LD H H", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x65, new Instruction("LD H L", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x66, new Instruction("LD H (HL)", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x67, new Instruction("LD H A", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));

    instructionSet.insert(std::make_pair(0x68, new Instruction("LD L B", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x69, new Instruction("LD L C", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x6a, new Instruction("LD L D", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x6b, new Instruction("LD L E", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x6c, new Instruction("LD L H", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x6d, new Instruction("LD L L", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x6e, new Instruction("LD L (HL)", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x6f, new Instruction("LD L A", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));

    instructionSet.insert(std::make_pair(0x78, new Instruction("LD A B", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x79, new Instruction("LD A C", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x7a, new Instruction("LD A D", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x7b, new Instruction("LD A E", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x7c, new Instruction("LD A H", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x7d, new Instruction("LD A L", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x7e, new Instruction("LD A (HL)", 1, 8, &CentralProcessingUnit::instruction_LoadReg2Reg)));
    instructionSet.insert(std::make_pair(0x7f, new Instruction("LD A A", 1, 4, &CentralProcessingUnit::instruction_LoadReg2Reg)));

    instructionSet.insert(std::make_pair(0xc5, new Instruction("PUSH BC", 1, 16, &CentralProcessingUnit::instruction_Push)));
    instructionSet.insert(std::make_pair(0xd5, new Instruction("PUSH DE", 1, 16, &CentralProcessingUnit::instruction_Push)));
    instructionSet.insert(std::make_pair(0xe5, new Instruction("PUSH HL", 1, 16, &CentralProcessingUnit::instruction_Push)));
    instructionSet.insert(std::make_pair(0xf5, new Instruction("PUSH AF", 1, 16, &CentralProcessingUnit::instruction_Push)));

    instructionSet.insert(std::make_pair(0x10F, new Instruction("RL B", 1, 8, &CentralProcessingUnit::instruction_Rotate)));
    instructionSet.insert(std::make_pair(0x110, new Instruction("RL C", 1, 8, &CentralProcessingUnit::instruction_Rotate)));
    instructionSet.insert(std::make_pair(0x111, new Instruction("RL D", 1, 8, &CentralProcessingUnit::instruction_Rotate)));
    instructionSet.insert(std::make_pair(0x112, new Instruction("RL E", 1, 8, &CentralProcessingUnit::instruction_Rotate)));
    instructionSet.insert(std::make_pair(0x113, new Instruction("RL H", 1, 8, &CentralProcessingUnit::instruction_Rotate)));
    instructionSet.insert(std::make_pair(0x114, new Instruction("RL L", 1, 8, &CentralProcessingUnit::instruction_Rotate)));
    instructionSet.insert(std::make_pair(0x115, new Instruction("RL (HL)", 1, 16, &CentralProcessingUnit::instruction_Rotate)));
    instructionSet.insert(std::make_pair(0x116, new Instruction("RL A", 1, 8, &CentralProcessingUnit::instruction_Rotate)));

    instructionSet.insert(std::make_pair(0x17, new Instruction("RLA", 1, 4, &CentralProcessingUnit::instruction_Rotate)));

    instructionSet.insert(std::make_pair(0xc1, new Instruction("POP BC", 1, 12, &CentralProcessingUnit::instruction_Pop)));
    instructionSet.insert(std::make_pair(0xd1, new Instruction("POP DE", 1, 12, &CentralProcessingUnit::instruction_Pop)));
    instructionSet.insert(std::make_pair(0xe1, new Instruction("POP HL", 1, 12, &CentralProcessingUnit::instruction_Pop)));
    instructionSet.insert(std::make_pair(0xf1, new Instruction("POP AF", 1, 12, &CentralProcessingUnit::instruction_Pop)));

    instructionSet.insert(std::make_pair(0x05, new Instruction("DEC B", 1, 4, &CentralProcessingUnit::instruction_Dec)));
    instructionSet.insert(std::make_pair(0x0d, new Instruction("DEC C", 1, 4, &CentralProcessingUnit::instruction_Dec)));
    instructionSet.insert(std::make_pair(0x15, new Instruction("DEC D", 1, 4, &CentralProcessingUnit::instruction_Dec)));
    instructionSet.insert(std::make_pair(0x1d, new Instruction("DEC E", 1, 4, &CentralProcessingUnit::instruction_Dec)));
    instructionSet.insert(std::make_pair(0x25, new Instruction("DEC H", 1, 4, &CentralProcessingUnit::instruction_Dec)));
    instructionSet.insert(std::make_pair(0x2d, new Instruction("DEC L", 1, 4, &CentralProcessingUnit::instruction_Dec)));
    instructionSet.insert(std::make_pair(0x35, new Instruction("DEC (HL)", 1, 12, &CentralProcessingUnit::instruction_Dec)));
    instructionSet.insert(std::make_pair(0x3d, new Instruction("DEC A", 1, 4, &CentralProcessingUnit::instruction_Dec)));

    instructionSet.insert(std::make_pair(0x03, new Instruction("INC BC", 1, 8, &CentralProcessingUnit::instruction_Inc16Bit)));
    instructionSet.insert(std::make_pair(0x13, new Instruction("INC DE", 1, 8, &CentralProcessingUnit::instruction_Inc16Bit)));
    instructionSet.insert(std::make_pair(0x23, new Instruction("INC HL", 1, 8, &CentralProcessingUnit::instruction_Inc16Bit)));
    instructionSet.insert(std::make_pair(0x33, new Instruction("INC SP", 1, 8, &CentralProcessingUnit::instruction_Inc16Bit)));

    instructionSet.insert(std::make_pair(0x0b, new Instruction("DEC BC", 1, 8, &CentralProcessingUnit::instruction_Dec16Bit)));
    instructionSet.insert(std::make_pair(0x1b, new Instruction("DEC DE", 1, 8, &CentralProcessingUnit::instruction_Dec16Bit)));
    instructionSet.insert(std::make_pair(0x2b, new Instruction("DEC HL", 1, 8, &CentralProcessingUnit::instruction_Dec16Bit)));
    instructionSet.insert(std::make_pair(0x3b, new Instruction("DEC SP", 1, 8, &CentralProcessingUnit::instruction_Dec16Bit)));

    instructionSet.insert(std::make_pair(0xc0, new Instruction("RET NZ", 1, 8, &CentralProcessingUnit::instruction_Return)));
    instructionSet.insert(std::make_pair(0xc8, new Instruction("RET Z", 1, 8, &CentralProcessingUnit::instruction_Return)));
    instructionSet.insert(std::make_pair(0xc9, new Instruction("RET", 1, 8, &CentralProcessingUnit::instruction_Return)));
    instructionSet.insert(std::make_pair(0xd0, new Instruction("RET NC", 1, 8, &CentralProcessingUnit::instruction_Return)));
    instructionSet.insert(std::make_pair(0xd8, new Instruction("RET C", 1, 8, &CentralProcessingUnit::instruction_Return)));

    instructionSet.insert(std::make_pair(0xb8, new Instruction("CMP B", 1, 4, &CentralProcessingUnit::instruction_Compare)));
    instructionSet.insert(std::make_pair(0xb9, new Instruction("CMP C", 1, 4, &CentralProcessingUnit::instruction_Compare)));
    instructionSet.insert(std::make_pair(0xba, new Instruction("CMP D", 1, 4, &CentralProcessingUnit::instruction_Compare)));
    instructionSet.insert(std::make_pair(0xbb, new Instruction("CMP E", 1, 4, &CentralProcessingUnit::instruction_Compare)));
    instructionSet.insert(std::make_pair(0xbc, new Instruction("CMP H", 1, 4, &CentralProcessingUnit::instruction_Compare)));
    instructionSet.insert(std::make_pair(0xbd, new Instruction("CMP L", 1, 4, &CentralProcessingUnit::instruction_Compare)));
    instructionSet.insert(std::make_pair(0xbe, new Instruction("CMP (HL)", 1, 8, &CentralProcessingUnit::instruction_Compare)));
    instructionSet.insert(std::make_pair(0xbf, new Instruction("CMP A", 1, 4, &CentralProcessingUnit::instruction_Compare)));
    instructionSet.insert(std::make_pair(0xfe, new Instruction("CMP n", 2, 8, &CentralProcessingUnit::instruction_Compare)));

    instructionSet.insert(std::make_pair(0x02, new Instruction("LD (BC) A", 1, 8, &CentralProcessingUnit::instruction_LoadA2Mem)));
    instructionSet.insert(std::make_pair(0x12, new Instruction("LD (DE) A", 1, 8, &CentralProcessingUnit::instruction_LoadA2Mem)));
    instructionSet.insert(std::make_pair(0xea, new Instruction("LD (nn) A", 3, 16, &CentralProcessingUnit::instruction_LoadA2Mem)));

    instructionSet.insert(std::make_pair(0xf0, new Instruction("LD A,(FF00+u8)", 2, 12, &CentralProcessingUnit::instruction_LoadAnn)));

    instructionSet.insert(std::make_pair(0x90, new Instruction("SUB B", 1, 4, &CentralProcessingUnit::instruction_Sub)));
    instructionSet.insert(std::make_pair(0x91, new Instruction("SUB C", 1, 4, &CentralProcessingUnit::instruction_Sub)));
    instructionSet.insert(std::make_pair(0x92, new Instruction("SUB D", 1, 4, &CentralProcessingUnit::instruction_Sub)));
    instructionSet.insert(std::make_pair(0x93, new Instruction("SUB E", 1, 4, &CentralProcessingUnit::instruction_Sub)));
    instructionSet.insert(std::make_pair(0x94, new Instruction("SUB H", 1, 4, &CentralProcessingUnit::instruction_Sub)));
    instructionSet.insert(std::make_pair(0x95, new Instruction("SUB L", 1, 4, &CentralProcessingUnit::instruction_Sub)));
    instructionSet.insert(std::make_pair(0x96, new Instruction("SUB (HL)", 1, 8, &CentralProcessingUnit::instruction_Sub)));
    instructionSet.insert(std::make_pair(0x97, new Instruction("SUB A", 1, 4, &CentralProcessingUnit::instruction_Sub)));
    instructionSet.insert(std::make_pair(0xd6, new Instruction("SUB n", 2, 8, &CentralProcessingUnit::instruction_Sub)));

    instructionSet.insert(std::make_pair(0x80, new Instruction("ADD B", 1, 4, &CentralProcessingUnit::instruction_Add)));
    instructionSet.insert(std::make_pair(0x81, new Instruction("ADD C", 1, 4, &CentralProcessingUnit::instruction_Add)));
    instructionSet.insert(std::make_pair(0x82, new Instruction("ADD D", 1, 4, &CentralProcessingUnit::instruction_Add)));
    instructionSet.insert(std::make_pair(0x83, new Instruction("ADD E", 1, 4, &CentralProcessingUnit::instruction_Add)));
    instructionSet.insert(std::make_pair(0x84, new Instruction("ADD H", 1, 4, &CentralProcessingUnit::instruction_Add)));
    instructionSet.insert(std::make_pair(0x85, new Instruction("ADD L", 1, 4, &CentralProcessingUnit::instruction_Add)));
    instructionSet.insert(std::make_pair(0x86, new Instruction("ADD (HL)", 1, 8, &CentralProcessingUnit::instruction_Add)));
    instructionSet.insert(std::make_pair(0x87, new Instruction("ADD A", 1, 4, &CentralProcessingUnit::instruction_Add)));
    instructionSet.insert(std::make_pair(0xc6, new Instruction("ADD n", 2, 8, &CentralProcessingUnit::instruction_Add)));

    instructionSet.insert(std::make_pair(0xc2, new Instruction("JP NZ nn", 3, 12, &CentralProcessingUnit::instruction_Jump)));
    instructionSet.insert(std::make_pair(0xc3, new Instruction("JP nn", 3, 12, &CentralProcessingUnit::instruction_Jump)));
    instructionSet.insert(std::make_pair(0xca, new Instruction("JP Z nn", 3, 12, &CentralProcessingUnit::instruction_Jump)));
    instructionSet.insert(std::make_pair(0xd2, new Instruction("JP NC nn", 3, 12, &CentralProcessingUnit::instruction_Jump)));
    instructionSet.insert(std::make_pair(0xda, new Instruction("JP CZ nn", 3, 12, &CentralProcessingUnit::instruction_Jump)));

    instructionSet.insert(std::make_pair(0xf3, new Instruction("DI", 1, 4, &CentralProcessingUnit::instruction_SetInterrupt)));
    instructionSet.insert(std::make_pair(0xfb, new Instruction("EI", 1, 4, &CentralProcessingUnit::instruction_SetInterrupt)));

    instructionSet.insert(std::make_pair(0x2a, new Instruction("LDI HL A", 1, 8, &CentralProcessingUnit::instruction_LoadAHL)));

    instructionSet.insert(std::make_pair(0xb0, new Instruction("OR B", 1, 4, &CentralProcessingUnit::instruction_OR)));
    instructionSet.insert(std::make_pair(0xb1, new Instruction("OR C", 1, 4, &CentralProcessingUnit::instruction_OR)));
    instructionSet.insert(std::make_pair(0xb2, new Instruction("OR D", 1, 4, &CentralProcessingUnit::instruction_OR)));
    instructionSet.insert(std::make_pair(0xb3, new Instruction("OR E", 1, 4, &CentralProcessingUnit::instruction_OR)));
    instructionSet.insert(std::make_pair(0xb4, new Instruction("OR H", 1, 4, &CentralProcessingUnit::instruction_OR)));
    instructionSet.insert(std::make_pair(0xb5, new Instruction("OR L", 1, 4, &CentralProcessingUnit::instruction_OR)));
    instructionSet.insert(std::make_pair(0xb6, new Instruction("OR (HL)", 1, 8, &CentralProcessingUnit::instruction_OR)));
    instructionSet.insert(std::make_pair(0xb7, new Instruction("OR A", 1, 4, &CentralProcessingUnit::instruction_OR)));
    instructionSet.insert(std::make_pair(0xf6, new Instruction("OR n", 2, 8, &CentralProcessingUnit::instruction_OR)));

    instructionSet.insert(std::make_pair(0xa0, new Instruction("AND B", 1, 4, &CentralProcessingUnit::instruction_AND)));
    instructionSet.insert(std::make_pair(0xa1, new Instruction("AND C", 1, 4, &CentralProcessingUnit::instruction_AND)));
    instructionSet.insert(std::make_pair(0xa2, new Instruction("AND D", 1, 4, &CentralProcessingUnit::instruction_AND)));
    instructionSet.insert(std::make_pair(0xa3, new Instruction("AND E", 1, 4, &CentralProcessingUnit::instruction_AND)));
    instructionSet.insert(std::make_pair(0xa4, new Instruction("AND H", 1, 4, &CentralProcessingUnit::instruction_AND)));
    instructionSet.insert(std::make_pair(0xa5, new Instruction("AND L", 1, 4, &CentralProcessingUnit::instruction_AND)));
    instructionSet.insert(std::make_pair(0xa6, new Instruction("AND (HL)", 1, 8, &CentralProcessingUnit::instruction_AND)));
    instructionSet.insert(std::make_pair(0xa7, new Instruction("AND A", 1, 4, &CentralProcessingUnit::instruction_AND)));
    instructionSet.insert(std::make_pair(0xe6, new Instruction("AND n", 2, 8, &CentralProcessingUnit::instruction_AND)));

    instructionSet.insert(std::make_pair(0x137, new Instruction("SRL B", 1, 8, &CentralProcessingUnit::instruction_SRL)));
    instructionSet.insert(std::make_pair(0x138, new Instruction("SRL C", 1, 8, &CentralProcessingUnit::instruction_SRL)));
    instructionSet.insert(std::make_pair(0x139, new Instruction("SRL D", 1, 8, &CentralProcessingUnit::instruction_SRL)));
    instructionSet.insert(std::make_pair(0x13a, new Instruction("SRL E", 1, 8, &CentralProcessingUnit::instruction_SRL)));
    instructionSet.insert(std::make_pair(0x13b, new Instruction("SRL H", 1, 8, &CentralProcessingUnit::instruction_SRL)));
    instructionSet.insert(std::make_pair(0x13c, new Instruction("SRL L", 1, 8, &CentralProcessingUnit::instruction_SRL)));
    instructionSet.insert(std::make_pair(0x13d, new Instruction("SRL (HL)", 1, 16, &CentralProcessingUnit::instruction_SRL)));
    instructionSet.insert(std::make_pair(0x13e, new Instruction("SRL A", 1, 8, &CentralProcessingUnit::instruction_SRL)));

    instructionSet.insert(std::make_pair(0xd9, new Instruction("RETI", 1, 16, &CentralProcessingUnit::instruction_Reti)));
    instructionSet.insert(std::make_pair(0x2f, new Instruction("CPL", 1, 4, &CentralProcessingUnit::instruction_CPL)));

    instructionSet.insert(std::make_pair(0x12f, new Instruction("SWAP B", 1, 8, &CentralProcessingUnit::instruction_Swap)));
    instructionSet.insert(std::make_pair(0x130, new Instruction("SWAP C", 1, 8, &CentralProcessingUnit::instruction_Swap)));
    instructionSet.insert(std::make_pair(0x131, new Instruction("SWAP D", 1, 8, &CentralProcessingUnit::instruction_Swap)));
    instructionSet.insert(std::make_pair(0x132, new Instruction("SWAP E", 1, 8, &CentralProcessingUnit::instruction_Swap)));
    instructionSet.insert(std::make_pair(0x133, new Instruction("SWAP H", 1, 8, &CentralProcessingUnit::instruction_Swap)));
    instructionSet.insert(std::make_pair(0x134, new Instruction("SWAP L", 1, 8, &CentralProcessingUnit::instruction_Swap)));
    instructionSet.insert(std::make_pair(0x135, new Instruction("SWAP (HL)", 1, 16, &CentralProcessingUnit::instruction_Swap)));
    instructionSet.insert(std::make_pair(0x136, new Instruction("SWAP A", 1, 8, &CentralProcessingUnit::instruction_Swap)));

    instructionSet.insert(std::make_pair(0xc7, new Instruction("RST 00h", 1, 16, &CentralProcessingUnit::instruction_Reset)));
    instructionSet.insert(std::make_pair(0xcf, new Instruction("RST 08h", 1, 16, &CentralProcessingUnit::instruction_Reset)));
    instructionSet.insert(std::make_pair(0xd7, new Instruction("RST 10h", 1, 16, &CentralProcessingUnit::instruction_Reset)));
    instructionSet.insert(std::make_pair(0xdf, new Instruction("RST 18h", 1, 16, &CentralProcessingUnit::instruction_Reset)));
    instructionSet.insert(std::make_pair(0xe7, new Instruction("RST 20h", 1, 16, &CentralProcessingUnit::instruction_Reset)));
    instructionSet.insert(std::make_pair(0xef, new Instruction("RST 28h", 1, 16, &CentralProcessingUnit::instruction_Reset)));
    instructionSet.insert(std::make_pair(0xf7, new Instruction("RST 30h", 1, 16, &CentralProcessingUnit::instruction_Reset)));
    instructionSet.insert(std::make_pair(0xff, new Instruction("RST 38h", 1, 16, &CentralProcessingUnit::instruction_Reset)));

    instructionSet.insert(std::make_pair(0x09, new Instruction("ADD HL, BC", 1, 8, &CentralProcessingUnit::instruction_AddPair)));
    instructionSet.insert(std::make_pair(0x19, new Instruction("ADD HL, DE", 1, 8, &CentralProcessingUnit::instruction_AddPair)));
    instructionSet.insert(std::make_pair(0x29, new Instruction("ADD HL, HL", 1, 8, &CentralProcessingUnit::instruction_AddPair)));

    instructionSet.insert(std::make_pair(0xe9, new Instruction("JP HL", 1, 4, &CentralProcessingUnit::instruction_JumpHL)));

    instructionSet.insert(std::make_pair(0x17f, new Instruction("RES 0,B", 1, 8, &CentralProcessingUnit::instruction_ResetBit0)));
    instructionSet.insert(std::make_pair(0x180, new Instruction("RES 0,C", 1, 8, &CentralProcessingUnit::instruction_ResetBit0)));
    instructionSet.insert(std::make_pair(0x181, new Instruction("RES 0,D", 1, 8, &CentralProcessingUnit::instruction_ResetBit0)));
    instructionSet.insert(std::make_pair(0x182, new Instruction("RES 0,E", 1, 8, &CentralProcessingUnit::instruction_ResetBit0)));
    instructionSet.insert(std::make_pair(0x183, new Instruction("RES 0,H", 1, 8, &CentralProcessingUnit::instruction_ResetBit0)));
    instructionSet.insert(std::make_pair(0x184, new Instruction("RES 0,L", 1, 8, &CentralProcessingUnit::instruction_ResetBit0)));
    instructionSet.insert(std::make_pair(0x185, new Instruction("RES 0,(HL)", 1, 16, &CentralProcessingUnit::instruction_ResetBit0)));
    instructionSet.insert(std::make_pair(0x186, new Instruction("RES 0,A", 1, 8, &CentralProcessingUnit::instruction_ResetBit0)));

}

CentralProcessingUnit::~CentralProcessingUnit() {
}

uint8_t CentralProcessingUnit::getFlags() {
	uint8_t val = 0;
	val |= (this->isZero << 7);
	val |= (this->isSubtract << 6);
	val |= (this->isHalfCarry << 5);
	val |= (this->isCarry << 4);
	return val;
}

void CentralProcessingUnit::setFlags(uint8_t val) {
	this->isZero        = (val >> 7) & 1;
	this->isSubtract   = (val >> 6) & 1;
	this->isHalfCarry   = (val >> 5) & 1;
	this->isCarry       = (val >> 4) & 1;
}

void CentralProcessingUnit::Print() {
	printf("┌───────────────┬───────────────┐\n");
	printf("│ a = %02x\t│\t\t│\n", accumulator);
	printf("│ b = %02x\t│ c = %02x\t│\n", b, c);
	printf("│ d = %02x\t│ e = %02x\t│\n", d, e);
	printf("│ h = %02x\t│ l = %02x\t│\n", h, l);
	printf("├───────────────┼───────────────┤\n");
	printf("│ sp = %04x\t│ pc = %04x\t│\n", stackPointer, ProgramCounter);
	printf("├───────────────┼───────────────┤\n");
	printf("│ zero = %u\t│ subtract = %u\t│\n", isZero, isSubtract);
	printf("│ carry = %u\t│ hcarry = %u\t│\n", isCarry, isHalfCarry);
	printf("└───────────────┴───────────────┘\n");
}

uint8_t CentralProcessingUnit::ExecuteInstruction(uint16_t skipDebug) {
    bool debug = (this->ProgramCounter > skipDebug);
    this->deltaTime = 0;

    handleInterrupts();

    if(!isHalted) {
        uint16_t opcode = mmu->Read(this->ProgramCounter);
        if(this->ProgramCounter == 0x0 && this->time != 0) {
            printf("Starting execution at 0x0000\n");
            exit(0);
        }

        if(debug) printf("Executing at 0x%04x, OpCode: 0x%02x ", this->ProgramCounter, opcode);
        if(opcode == 0xcb) {
            this->ProgramCounter++;
            opcode = mmu->Read(this->ProgramCounter) + 0xff;
            if(debug) printf("Extended OpCode: 0x%04x ", opcode);
        }

        std::map<uint16_t, Instruction*>::iterator it = instructionSet.find(opcode);
        if (it != instructionSet.end()) {
            Instruction *inst = it->second;
            if(debug) printf("Name: %s, ", inst->name.c_str());
            uint8_t data[inst->size];
            if(debug) printf("Data: ");
            for (size_t i = 0; i < inst->size; i++) {
                data[i] = mmu->Read(this->ProgramCounter + i);
                if(debug && i > 0) printf("%d, ", data[i]);
            }
            if(debug) printf("\n");
            
            if((this->*(inst->code))(data)) {
                this->deltaTime = inst->cycles;
                this->ProgramCounter += inst->size;
            }
            this->time += this->deltaTime;
        } else {
            printf("OpCode Not Implemented at 0x%04x, OpCode: 0x%02x\n", this->ProgramCounter, opcode);
            exit(0);
        }
    }

    if(debug) Print();

    return this->deltaTime;
}

void CentralProcessingUnit::handleInterrupts() {
    if(!this->interruptMasterFlag)
        return;

    // printf("Handling Interrupt\n");
    this->interruptMasterFlag = false;

    uint16_t interruptAddr = 0;
    uint8_t interruptFlag = 0;

    bool vBlankInterrupt =  mmu->ReadIORegisterBit(AddrRegInterruptEnabled, FlagInterruptVBlank)
                            && mmu->ReadIORegisterBit(AddrRegInterruptFlag, FlagInterruptVBlank);
    bool lcdInterrupt =     mmu->ReadIORegisterBit(AddrRegInterruptEnabled, FlagInterruptLcd)
                            && mmu->ReadIORegisterBit(AddrRegInterruptFlag, FlagInterruptLcd);
    bool timerInterrupt =   mmu->ReadIORegisterBit(AddrRegInterruptEnabled, FlagInterruptTimer)
                            && mmu->ReadIORegisterBit(AddrRegInterruptFlag, FlagInterruptTimer);
    bool serialInterrupt =  mmu->ReadIORegisterBit(AddrRegInterruptEnabled, FlagInterruptSerial)
                            && mmu->ReadIORegisterBit(AddrRegInterruptFlag, FlagInterruptSerial);
    bool joypadInterrupt =  mmu->ReadIORegisterBit(AddrRegInterruptEnabled, FlagInterruptInput)
                            && mmu->ReadIORegisterBit(AddrRegInterruptFlag, FlagInterruptInput);

    if(vBlankInterrupt) {
        interruptAddr = AddrVectorVBlank;
        interruptFlag = FlagInterruptVBlank;
        isHalted = false;
    } else if(lcdInterrupt) {
        interruptAddr = AddrVectorLcd;
        interruptFlag = FlagInterruptLcd;
        isHalted = false;
    } else if(timerInterrupt) {
        interruptAddr = AddrVectorTimer;
        interruptFlag = FlagInterruptTimer;
        isHalted = false;
    } else if(serialInterrupt) {
        interruptAddr = AddrVectorSerial;
        interruptFlag = FlagInterruptSerial;
        isHalted = false;
    } else if(joypadInterrupt) {
        interruptAddr = AddrVectorInput;
        interruptFlag = FlagInterruptInput;
        isHalted = false;
    }
    
    if(interruptAddr != 0)
        serviceInterrupts(interruptAddr, interruptFlag);
}

void CentralProcessingUnit::serviceInterrupts(uint16_t addr, uint8_t flag) {
    mmu->WriteIORegisterBit(AddrRegInterruptFlag, flag, false);

    this->stackPointer--;
    mmu->Write(this->stackPointer, this->ProgramCounter >> 8);
    this->stackPointer--;
    mmu->Write(this->stackPointer, this->ProgramCounter);

    this->ProgramCounter = addr;
}

bool CentralProcessingUnit::instruction_NOP(uint8_t* data) {
    return true;
}

bool CentralProcessingUnit::instruction_LoadSP(uint8_t* data) {
    uint8_t hi, lo;
    if (data[0] == 0x31) {
        lo = data[1];
        hi = data[2];
    } else if (data[0] == 0xf9) {
        lo = this->l;
        hi = this->h;
    }
    this->stackPointer = (hi << 8) | lo;
    return true;
}

bool CentralProcessingUnit::instruction_XOROP(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0xaf)
        d = this->accumulator;
    else if (data[0] == 0xa8)
        d = this->b;
    else if (data[0] == 0xa9)
        d = this->c;
    else if (data[0] == 0xaa)
        d = this->d;
    else if (data[0] == 0xab)
        d = this->e;
    else if (data[0] == 0xac)
        d = this->h;
    else if (data[0] == 0xad)
        d = this->l;
    else if (data[0] == 0xae) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        d = mmu->Read(addr);
    } else if (data[0] == 0xee)
        d = data[1];

    // xor a and data and put it in a
    this->accumulator ^= d;
    this->isZero = (this->accumulator == 0);
    this->isCarry = this->isHalfCarry = this->isSubtract = 0;
    return true;
}

bool CentralProcessingUnit::instruction_LoadPair(uint8_t* data) {
    uint8_t *hi, *lo;
    if (data[0] == 0x01)
        hi = &(this->b), lo = &(this->c);
    else if (data[0] == 0x11)
        hi = &(this->d), lo = &(this->e);
    else if (data[0] == 0x21)
        hi = &(this->h), lo = &(this->l);

    *lo = data[1];
    *hi = data[2];
    return true;
}

bool CentralProcessingUnit::instruction_Load(uint8_t* data) {
    switch (data[0]) {
        case 0x32: {
            uint16_t addr = (((uint16_t)this->h) << 8) | this->l;
            mmu->Write(addr, this->accumulator);
            addr--;
            this->l = addr & 0xff;
            this->h = addr >> 8;
            break;
        }
        case 0xe2: {
            mmu->Write(0xFF00 + (uint16_t)this->c, this->accumulator);
            break;
        }
        case 0xe0: {
            mmu->Write(0xff00 + (uint16_t)data[1], this->accumulator);
            break;
        }
        case 0x22: {
            uint16_t addr = (((uint16_t)this->h)<<8) | this->l;
            mmu->Write(addr, this->accumulator);
            addr++;
            this->h = (addr >> 8);
            this->l = addr;
            break;
        }
        default:
            break;
    }
    return true;
}

bool CentralProcessingUnit::instruction_BIT7(uint8_t* data) {
    uint8_t* reg;
    if (data[0] == 0x7f)
        reg = &(this->accumulator);
    else if (data[0] == 0x78)
        reg = &(this->b);
    else if (data[0] == 0x79)
        reg = &(this->c);
    else if (data[0] == 0x7a)
        reg = &(this->d);
    else if (data[0] == 0x7b)
        reg = &(this->e);
    else if (data[0] == 0x7c)
        reg = &(this->h);
    else if (data[0] == 0x7d)
        reg = &(this->l);
    else if (data[0] == 0x7e) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        uint8_t val = mmu->Read(addr);
        this->isZero = (val & (1 << 7)) == 0;
        this->isHalfCarry = 1;
        this->isSubtract = 0;
        return true;
    }
    this->isZero = ((*reg) & (1 << 7)) == 0;
    this->isHalfCarry = 1;
    this->isSubtract = 0;
    return true;
}

bool CentralProcessingUnit::instruction_JR(uint8_t* data) {
    bool condition = false;
    if (data[0] == 0x18)
        condition = true;
    else if (data[0] == 0x20)
        condition = !(this->isZero);
    else if (data[0] == 0x28)
        condition = (this->isZero);
    else if (data[0] == 0x30)
        condition = !(this->isCarry);
    else if (data[0] == 0x38)
        condition = (this->isCarry);

    if (condition)
        this->ProgramCounter += (int8_t)data[1];

    return true;
}

bool CentralProcessingUnit::instruction_LoadMem2Reg(uint8_t* data) {
    uint8_t* reg;
    if (data[0] == 0x06)
        reg = &(this->b);
    else if (data[0] == 0x0e)
        reg = &(this->c);
    else if (data[0] == 0x16)
        reg = &(this->d);
    else if (data[0] == 0x1e)
        reg = &(this->e);
    else if (data[0] == 0x26)
        reg = &(this->h);
    else if (data[0] == 0x2e)
        reg = &(this->l);
    else if (data[0] == 0x3e)
        reg = &(this->accumulator);

    *reg = data[1];
    return true;
}

bool CentralProcessingUnit::instruction_Inc(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x3c)
        reg = &(this->accumulator);
    else if (data[0] == 0x04)
        reg = &(this-> b);
    else if (data[0] == 0x0c)
        reg = &(this-> c);
    else if (data[0] == 0x14)
        reg = &(this-> d);
    else if (data[0] == 0x1c)
        reg = &(this-> e);
    else if (data[0] == 0x24)
        reg = &(this-> h);
    else if (data[0] == 0x2c)
        reg = &(this-> l);
    else if (data[0] == 0x34) {
        uint16_t addr = (((uint16_t)this->h)<<8) | (this->l);
        uint8_t val = mmu->Read(addr);
        this->isHalfCarry = ((val & 0xf) == 1);
        val++;
        this->isZero = (val == 0);
        this->isSubtract = 0;
        mmu->Write(addr, val);
        return true;
    }

    this->isHalfCarry = (((*reg) & 0xf) == 1);
    (*reg)++;
    this->isZero = (*reg == 0);
    this->isSubtract = 0;
    return true;
}

bool CentralProcessingUnit::instruction_Dec(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x3d)
        reg = &(this->accumulator);
    else if (data[0] == 0x05)
        reg = &(this->b);
    else if (data[0] == 0x0D)
        reg = &(this->c);
    else if (data[0] == 0x15)
        reg = &(this->d);
    else if (data[0] == 0x1d)
        reg = &(this->e);
    else if (data[0] == 0x25)
        reg = &(this->h);
    else if (data[0] == 0x2d)
        reg = &(this->l);
    else if (data[0] == 0x35) {
        uint16_t addr = (((uint16_t)this->h)<<8) | (this->l);
        uint8_t val = mmu->Read(addr);
        this->isHalfCarry = ((val & 0xf) == 1);
        val--;
        this->isZero = (val == 0);
        this->isSubtract = 1;
        mmu->Write(addr, val);
        return true;
    }

    this->isHalfCarry = (((*reg) & 0xf) == 0);
    (*reg)--;
    this->isZero = (*reg == 0);
    this->isSubtract = 1;
    return true;
}

bool CentralProcessingUnit::instruction_Inc16Bit(uint8_t* data) {
    uint8_t *hi, *lo;
    if (data[0] == 0x03)
        hi = &(this->b), lo = &(this->c);
    else if (data[0] == 0x13)
        hi = &(this->d), lo = &(this->e);
    else if (data[0] == 0x23)
        hi = &(this->h), lo = &(this->l);
    else if (data[0] == 0x33) {
        this->stackPointer++;
        return true;
    }

    uint16_t val = (((uint16_t)(*hi)) << 8) | (*lo);
    val++;
    *hi = val >> 8;
    *lo = val;
    return true;
}

bool CentralProcessingUnit::instruction_Dec16Bit(uint8_t* data) {
    uint8_t *hi, *lo;
    if (data[0] == 0x0b)
        hi = &(this->b), lo = &(this->c);
    else if (data[0] == 0x1b)
        hi = &(this->d), lo = &(this->e);
    else if (data[0] == 0x2b)
        hi = &(this->h), lo = &(this->l);
    else if (data[0] == 0x3b) {
        this->stackPointer--;
        return true;
    }

    uint16_t val = (((uint16_t)(*hi)) << 8) | (*lo);
    val--;
    *hi = val >> 8;
    *lo = val;
    return true;
}

bool CentralProcessingUnit::instruction_LoadReg2Mem(uint8_t* data) {
    uint8_t from;
    if (data[0] == 0x36)
        from = data[1];
    else if (data[0] == 0x70)
        from = this->b;
    else if (data[0] == 0x71)
        from = this->c;
    else if (data[0] == 0x72)
        from = this->d;
    else if (data[0] == 0x73)
        from = this->e;
    else if (data[0] == 0x74)
        from = this->h;
    else if (data[0] == 0x75)
        from = this->l;
    else if (data[0] == 0x77)
        from = this->accumulator;

    uint16_t addr = (((uint16_t)(this->h)) << 8) |(this -> l);
    mmu->Write(addr, from);
    return true;
}

bool CentralProcessingUnit::instruction_LoadAIndirect(uint8_t* data) {
    uint16_t hi, lo;
    if (data[0] == 0x0a)
        hi = this->b, lo = this->c;
    else if (data[0] == 0x1a)
        hi = this->d, lo = this->e;
    else if (data[0] == 0x7e)
        hi = this->h, lo = this->l;
    else if (data[0] == 0xfa)
        hi = data[2], lo = data[1];

    uint16_t addr = (hi << 8) | lo;
    this->accumulator = mmu->Read(addr);
    return true;
}

bool CentralProcessingUnit::instruction_LoadA2Mem(uint8_t* data) {
    uint16_t hi, lo;
    if (data[0] == 0x02)
        hi = this->b, lo = this->c;
    else if (data[0] == 0x12)
        hi = this->d, lo = this->e;
    else if (data[0] == 0xea)
        hi = data[2], lo = data[1];

    uint16_t addr = (hi << 8) | lo;
    mmu->Write(addr, this->accumulator);
    return true;
}

bool CentralProcessingUnit::instruction_LoadReg2Reg(uint8_t* data) {
    uint8_t val = 0;
    if (data[0] == 0x40 || data[0] == 0x48 || data[0] == 0x50 || data[0] == 0x58 || data[0] == 0x60 || data[0] == 0x68 || data[0] == 0x78)
        val = this->b;
    else if (data[0] == 0x41 || data[0] == 0x49 || data[0] == 0x51 || data[0] == 0x59 || data[0] == 0x61 || data[0] == 0x69 || data[0] == 0x79)
        val = this->c;
    else if (data[0] == 0x42 || data[0] == 0x4a || data[0] == 0x52 || data[0] == 0x5a || data[0] == 0x62 || data[0] == 0x6a || data[0] == 0x7a)
        val = this->d;
    else if (data[0] == 0x43 || data[0] == 0x4b || data[0] == 0x53 || data[0] == 0x5b || data[0] == 0x63 || data[0] == 0x6b || data[0] == 0x7b)
        val = this->e;
    else if (data[0] == 0x44 || data[0] == 0x4c || data[0] == 0x54 || data[0] == 0x5c || data[0] == 0x64 || data[0] == 0x6c || data[0] == 0x7c)
        val = this->h;
    else if (data[0] == 0x45 || data[0] == 0x4d || data[0] == 0x55 || data[0] == 0x5d || data[0] == 0x65 || data[0] == 0x6d || data[0] == 0x7d)
        val = this->l;
    else if (data[0] == 0x46 || data[0] == 0x4e || data[0] == 0x56 || data[0] == 0x5e || data[0] == 0x66 || data[0] == 0x6e || data[0] == 0x7e) {
        uint16_t addr = ((uint16_t)(this->h) << 8) | (this->l);
        val = mmu->Read(addr);
    } else if (data[0] == 0x47 || data[0] == 0x4f || data[0] == 0x57 || data[0] == 0x5f || data[0] == 0x67 || data[0] == 0x6f || data[0] == 0x7f)
        val = this->accumulator;

    if(data[0] >= 0x40 && data[0] <= 0x47)
        this->b = val;
    else if(data[0] >= 0x48 && data[0] <= 0x4f)
        this->c = val;
    else if(data[0] >= 0x50 && data[0] <= 0x57)
        this->d = val;
    else if(data[0] >= 0x58 && data[0] <= 0x5f)
        this->e = val;
    else if(data[0] >= 0x60 && data[0] <= 0x67)
        this->h = val;
    else if(data[0] >= 0x68 && data[0] <= 0x6f)
        this->l = val;
    else if(data[0] >= 0x78 && data[0] <= 0x7f)
        this->accumulator = val;

    return true;
}

bool CentralProcessingUnit::instruction_Call(uint8_t* data) {
    bool condition;
    if (data[0] == 0xcd)
        condition = true;
    else if (data[0] == 0xc4)
        condition = (this->isZero == 0);
    else if (data[0] == 0xcc)
        condition = this->isZero;
    else if (data[0] == 0xd4)
        condition = (this->isCarry == 0);
    else if (data[0] == 0xdc)
        condition = this->isCarry;

    if (condition) {
        this->deltaTime = 24;

        this->stackPointer--;
        mmu->Write(this->stackPointer, (this->ProgramCounter + 3) >> 8);
        this->stackPointer--;
        mmu->Write(this->stackPointer, (this->ProgramCounter + 3));

        uint16_t next = data[2];
        next = (next << 8) | (data[1]);
        this->ProgramCounter = next;
        return false;
    }
    return true;
}

bool CentralProcessingUnit::instruction_Push(uint8_t* data) {
    uint8_t hi, lo;
    if (data[0] == 0xf5)
        hi = (this->accumulator), lo = this->getFlags();
    else if (data[0] == 0xc5)
        hi = this->b, lo = this->c;
    else if (data[0] == 0xd5)
        hi = this->d, lo = this->e;
    else if (data[0] == 0xe5)
        hi = this->h, lo = this->l;

    this->stackPointer--;
    mmu->Write(this->stackPointer, hi);
    this->stackPointer--;
    mmu->Write(this->stackPointer, lo);
    return true;
}

bool CentralProcessingUnit::instruction_Pop(uint8_t* data) {
    uint8_t *hi, *lo;
    if (data[0] == 0xc1)
        hi = &(this->b), lo = &(this->c);
    else if (data[0] == 0xd1)
        hi = &(this->d), lo = &(this->e);
    else if (data[0] == 0xe1)
        hi = &(this->h), lo = &(this->l);
    else if (data[0] == 0xf1) {
        this->accumulator = mmu->Read(this->stackPointer + 1);
        this->setFlags(mmu->Read(this->stackPointer));
        this->stackPointer += 2;
        return true;
    }

    *lo = mmu->Read(this->stackPointer);
    *hi = mmu->Read(this->stackPointer + 1);
    this->stackPointer += 2;
    return true;
}

bool CentralProcessingUnit::instruction_Rotate(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x17)
        reg = &(this->accumulator);
    else if (data[0] == 0x10)
        reg = &(this->b);
    else if (data[0] == 0x11)
        reg = &(this->c);
    else if (data[0] == 0x12)
        reg = &(this->d);
    else if (data[0] == 0x13)
        reg = &(this->e);
    else if (data[0] == 0x14)
        reg = &(this->h);
    else if (data[0] == 0x15)
        reg = &(this->l);
    else if (data[0] == 0x16) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        uint8_t val = mmu->Read(addr);

        int temp = this->isCarry;
        this->isCarry = (val >> 7) & 1;
        val = val << 1 | temp;
        this->isZero = (val == 0);

        mmu->Write(addr, val);
        return true;
    }

    int temp = this->isCarry;
    this->isCarry = ((*reg) >> 7) & 1;
    *reg = (*reg) << 1 | temp;
    this->isZero = *reg == 0;

    return true;
}

bool CentralProcessingUnit::instruction_Return(uint8_t* data) {
    bool condition = true;
    if (data[0] == 0xc0)
        condition = !(this->isZero);
    else if (data[0] == 0xc8)
        condition = this->isZero;
    else if (data[0] == 0xd0)
        condition = !(this->isCarry);
    else if (data[0] == 0xd8)
        condition = this->isCarry;

    if (condition) {
        uint16_t lo = mmu->Read(this->stackPointer);
        uint16_t hi = mmu->Read(this->stackPointer + 1);
        this->stackPointer += 2;
        this->ProgramCounter = (hi << 8) | lo;
        if (data[0] == 0xc9)
            this->deltaTime = 16;
        else
            this->deltaTime = 20;
        return false;
    }

    return true;
}

bool CentralProcessingUnit::instruction_Compare(uint8_t* data) {
    uint8_t n = 0;
    if (data[0] == 0xbf)
        n = this->accumulator;
    else if (data[0] == 0xb8)
        n = this->b;
    else if (data[0] == 0xb9)
        n = this->c;
    else if (data[0] == 0xba)
        n = this->d;
    else if (data[0] == 0xbb)
        n = this->e;
    else if (data[0] == 0xbc)
        n = this->h;
    else if (data[0] == 0xbd)
        n = this->l;
    else if (data[0] == 0xbe)
        n = mmu->Read((((uint16_t)this->h) << 8) | (this -> l));
    else if (data[0] == 0xfe)
        n = data[1];

    uint8_t val = this->accumulator - n;
    this->isZero = (val == 0);
    this->isSubtract = true;
    this->isHalfCarry = ((val & 0xf) > ((accumulator) & 0xf));
    this->isCarry = (this->accumulator < n);
    return true;
}

bool CentralProcessingUnit::instruction_LoadAnn(uint8_t* data) {
    this->accumulator = mmu->Read(data[1] + (uint16_t)0xff00);
    return true;
}

bool CentralProcessingUnit::instruction_Sub(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0x97)
        d = this->accumulator;
    else if (data[0] == 0x90)
        d = this->b;
    else if (data[0] == 0x91)
        d = this->c;
    else if (data[0] == 0x92)
        d = this->d;
    else if (data[0] == 0x93)
        d = this->e;
    else if (data[0] == 0x94)
        d = this->h;
    else if (data[0] == 0x95)
        d = this->l;
    else if (data[0] == 0x96) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        d = mmu->Read(addr);
    } else if (data[0] == 0xd6)
        d = data[1];

    this->isZero = (this->accumulator == d);
    this->isSubtract = 1;
    this->isCarry = (this->accumulator < d);
    this->isHalfCarry = (this->accumulator & 0xf) < (d & 0xf);
    this->accumulator = this->accumulator - d;
    return true;
}

bool CentralProcessingUnit::instruction_Add(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0x87)
        d = this->accumulator;
    else if (data[0] == 0x80)
        d = this->b;
    else if (data[0] == 0x81)
        d = this->c;
    else if (data[0] == 0x82)
        d = this->d;
    else if (data[0] == 0x83)
        d = this->e;
    else if (data[0] == 0x84)
        d = this->h;
    else if (data[0] == 0x85)
        d = this->l;
    else if (data[0] == 0x86) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        d = mmu->Read(addr);
    } else if (data[0] == 0xc6)
        d = data[1];

    uint16_t val = d;
    val += this->accumulator;

    this->isZero = (val & 0xff) == 0;
    this->isCarry = (val >> 8) & 1;
    uint8_t half = (this->accumulator & 0xf) + (d & 0xf);
    this->isHalfCarry = (half >> 4) & 1;
    this->isSubtract = 0;

    this->accumulator += d;
    return true;
}

bool CentralProcessingUnit::instruction_Jump(uint8_t* data) {
    bool condition = false;
    if (data[0] == 0xc3)
        condition = true;
    else if (data[0] == 0xc2)
        condition = !(this->isZero);
    else if (data[0] == 0xca)
        condition = (this->isZero);
    else if (data[0] == 0xd2)
        condition = !(this->isCarry);
    else if (data[0] == 0xda)
        condition = (this->isCarry);

    if (condition) {
        uint16_t lo = data[1];
        uint16_t hi = data[2];
        this->ProgramCounter = (hi << 8) | lo;
        this->deltaTime = 16;
        return false;
    }

    return true;
}

bool CentralProcessingUnit::instruction_SetInterrupt(uint8_t* data) {
    if (data[0] == 0xfb)
        this->interruptMasterFlag = true;
    else if (data[0] == 0xf3)
        this->interruptMasterFlag = false;
    return true;
}

bool CentralProcessingUnit::instruction_LoadAHL(uint8_t* data) {
    uint16_t addr = this->h;
    addr = (addr << 8) | this->l;
    this->accumulator = mmu->Read(addr);
    addr++;
    this->h = (addr >> 8) & 0xff;
    this->l = addr & 0xff;
    return true;
}

bool CentralProcessingUnit::instruction_OR(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0xb7)
        d = this->accumulator;
    else if (data[0] == 0xb0)
        d = this->b;
    else if (data[0] == 0xb1)
        d = this->c;
    else if (data[0] == 0xb2)
        d = this->d;
    else if (data[0] == 0xb3)
        d = this->e;
    else if (data[0] == 0xb4)
        d = this->h;
    else if (data[0] == 0xb5)
        d = this->l;
    else if (data[0] == 0xb6) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        d = mmu->Read(addr);
    } else if (data[0] == 0xf6)
        d = data[1];

    this->accumulator |= d;
    this->isZero = (this->accumulator == 0);
    this->isCarry = this->isHalfCarry = this->isSubtract = 0;
    return true;
}

bool CentralProcessingUnit::instruction_AND(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0xa7)
        d = this->accumulator;
    else if (data[0] == 0xa0)
        d = this->b;
    else if (data[0] == 0xa1)
        d = this->c;
    else if (data[0] == 0xa2)
        d = this->d;
    else if (data[0] == 0xa3)
        d = this->e;
    else if (data[0] == 0xa4)
        d = this->h;
    else if (data[0] == 0xa5)
        d = this->l;
    else if (data[0] == 0xa6) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        d = mmu->Read(addr);
    } else if (data[0] == 0xe6)
        d = data[1];

    this->accumulator &= d;
    this->isZero = (this->accumulator == 0);
    this->isHalfCarry = 1;
    this->isCarry = this->isSubtract = 0;
    return 1;
}

bool CentralProcessingUnit::instruction_SRL(uint8_t* data) {
    uint8_t* reg;
    if (data[0] == 0x38)
        reg = &(this->b);
    else if (data[0] == 0x39)
        reg = &(this->c);
    else if (data[0] == 0x3a)
        reg = &(this->d);
    else if (data[0] == 0x3b)
        reg = &(this->e);
    else if (data[0] == 0x3c)
        reg = &(this->h);
    else if (data[0] == 0x3d)
        reg = &(this->l);
    else if (data[0] == 0x3e) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        uint8_t val = mmu->Read(addr);
        this->isCarry = val & 1;
        val >>= 1;
        mmu->Write(addr, val);
        this->isZero = (val == 0);
        this->isHalfCarry = this->isSubtract = 0;
        return true;
    } else if (data[0] == 0x3f)
        reg = &(this->accumulator);

    this->isHalfCarry = this->isSubtract = 0;
    uint8_t val = (*reg) >> 1;
    this->isZero = (val == 0);
    this->isCarry = (*reg) & 1;
    *reg = val;
    return true;
}

bool CentralProcessingUnit::instruction_Reti(uint8_t* data) {
    uint16_t lo = mmu->Read(this->stackPointer);
    uint16_t hi = mmu->Read(this->stackPointer + 1);
    this->stackPointer += 2;
    this->ProgramCounter = (hi << 8) | lo;
    this->interruptMasterFlag = true;
    this->deltaTime = 16;
    return false;
}

bool CentralProcessingUnit::instruction_CPL(uint8_t* data) {
    this->accumulator = ~(this->accumulator);
    this->isSubtract = this->isHalfCarry = true;
    return true;
}

bool CentralProcessingUnit::instruction_Swap(uint8_t* data) {
    this->isSubtract = this->isHalfCarry = this->isCarry = 0;
    uint8_t* reg;
    if (data[0] == 0x37)
        reg = &(this->accumulator);
    else if (data[0] == 0x30)
        reg = &(this->b);
    else if (data[0] == 0x31)
        reg = &(this->c);
    else if (data[0] == 0x32)
        reg = &(this->d);
    else if (data[0] == 0x33)
        reg = &(this->e);
    else if (data[0] == 0x34)
        reg = &(this->h);
    else if (data[0] == 0x35)
        reg = &(this->l);
    else if (data[0] == 0x36) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        uint8_t data = mmu->Read(addr);
        uint8_t val = (data >> 4);
        val |= (data << 4);
        mmu->Write(addr, val);
        this->isZero = (val == 0);
        return true;
    }

    uint8_t val = (*reg) >> 4;
    val |= ((*reg) << 4);
    *reg = val;
    this->isZero = (val == 0);
    return true;
}

bool CentralProcessingUnit::instruction_Reset(uint8_t* data) {
    this->deltaTime = 16;
    uint16_t val = this->ProgramCounter + 1;
    uint8_t lo =  val & 0xff;
    uint8_t hi = (val >> 8) & 0xff;
    this->stackPointer--;
    mmu->Write(this->stackPointer, hi);
    this->stackPointer--;
    mmu->Write(this->stackPointer, lo);

    if (data[0] == 0xc7)
        this->ProgramCounter = 0;
    else if (data[0] == 0xcf)
        this->ProgramCounter = 0x08;
    else if (data[0] == 0xd7)
        this->ProgramCounter = 0x10;
    else if (data[0] == 0xdf)
        this->ProgramCounter = 0x18;
    else if (data[0] == 0xe7)
        this->ProgramCounter = 0x20;
    else if (data[0] == 0xef)
        this->ProgramCounter = 0x28;
    else if (data[0] == 0xf7)
        this->ProgramCounter = 0x30;
    else if (data[0] == 0xff)
        this->ProgramCounter = 0x38;

    return false;
}

bool CentralProcessingUnit::instruction_AddPair(uint8_t* data) {
    uint16_t hi, lo;
    if (data[0] == 0x09)
        hi = this->b, lo = this->c;
    else if (data[0] == 0x19)
        hi = this->d, lo = this->e;
    else if (data[0] == 0x29)
        hi = this->h, lo = this->l;
    else if (data[0] == 0x39)
        hi = (this->stackPointer) >> 8, lo = (this->stackPointer) & 0xff;

    uint16_t val = (hi << 8) | lo;
    uint16_t hl = (((uint16_t)(this->h)) << 8) | this->l;
    this->isSubtract = 0;
    this->isHalfCarry = (((val & 0xfff) + (hl & 0xfff)) >> 12) & 1; // set if carry from bit 11
    this->isCarry = (((uint32_t)val + hl) >> 16) & 1; // set if carry from bit 15
    uint16_t ans = val + hl;
    this->h = ans >> 8;
    this->l = ans & 0xff;
    return true;
}

bool CentralProcessingUnit::instruction_JumpHL(uint8_t* data) {
    uint16_t addr = this->h;
    addr = (addr << 8) | this->l;
    this->ProgramCounter = addr;
    return false;
}

bool CentralProcessingUnit::instruction_ResetBit0(uint8_t* data) {
    uint8_t* reg;
    if (data[0] == 0x80)
        reg = &(this->b);
    else if (data[0] == 0x81)
        reg = &(this->c);
    else if (data[0] == 0x82)
        reg = &(this->d);
    else if (data[0] == 0x83)
        reg = &(this->e);
    else if (data[0] == 0x84)
        reg = &(this->h);
    else if (data[0] == 0x85)
        reg = &(this->l);
    else if (data[0] == 0x86) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        uint8_t val = mmu->Read(addr);
        val ^= 1;
        mmu->Write(addr, val);
        return true;
    } else if (data[0] == 0x87)
        reg = &(this->accumulator);

    (*reg) ^= 1;
    return true;
}