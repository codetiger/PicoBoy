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
	uint16_t programCounter;

    bool isZero, isSubtract, isCarry, isHalfCarry;
    bool isHalted;
    MemoryManagementUnit *mmu;

    class Instruction {
    public:
        std::string name;
        int size;
        int cycles;
        void (CentralProcessingUnit::*code)(uint8_t*);

        Instruction(std::string name, int size, int cycles, void(CentralProcessingUnit::*c)(uint8_t*)) {
            this->code = c;
            this->name = name;
            this->size = size;
            this->cycles = cycles;
        }
    };

    std::map<uint16_t, Instruction*> instructionSet;
    std::map<uint16_t, Instruction*> instructionSetExtended;
    uint8_t data[8];

    uint8_t getFlags();
    void setFlags(uint8_t val);
    bool check_bit(const uint8_t value, const uint8_t bit);
    void stackPush(uint16_t val);
    void stackPush(uint8_t hi, uint8_t lo);
    uint16_t stackPop();
    uint8_t readMemoryFromProgramCounter();
    void setHL(uint16_t addr);
    uint16_t getHL();
    uint16_t stitch(uint8_t hi, uint8_t lo);

    void instruction_NOP(uint8_t* data);
    void instruction_XOROP(uint8_t* data);

    void instruction_Inc(uint8_t* data);
    void instruction_Dec(uint8_t* data);
    void instruction_Inc16Bit(uint8_t* data);
    void instruction_Dec16Bit(uint8_t* data);    
    
    void instruction_LoadSP(uint8_t* data);
    void instruction_LoadSPHL(uint8_t* data);
    void instruction_LoadPair(uint8_t* data);
    void instruction_Load(uint8_t* data);
    void instruction_LoadMem2Reg(uint8_t* data);
    void instruction_LoadAIndirect(uint8_t* data);
    void instruction_LoadA2Mem(uint8_t* data);
    void instruction_LoadReg2Reg(uint8_t* data);
    void instruction_LoadAHL(uint8_t* data);
    void instruction_LoadHL(uint8_t* data);

    void instruction_JR(uint8_t* data);
    void instruction_Call(uint8_t* data);
    void instruction_Push(uint8_t* data);
    void instruction_Pop(uint8_t* data);
    void instruction_Return(uint8_t* data);
    void instruction_Reti(uint8_t* data);
    
    void instruction_Compare(uint8_t* data);
    void instruction_LoadAnn(uint8_t* data);

    void instruction_Sub(uint8_t* data);
    void instruction_Add(uint8_t* data);
    void instruction_Adc(uint8_t* data);
    void instruction_Jump(uint8_t* data);
    void instruction_JumpHL(uint8_t* data);

    void instruction_OR(uint8_t* data);
    void instruction_AND(uint8_t* data);
    void instruction_CPL(uint8_t* data);
    void instruction_Swap(uint8_t* data);
    void instruction_Reset(uint8_t* data);
    void instruction_AddPair(uint8_t* data);

    void instruction_SRL(uint8_t* data);
    void instruction_DAA(uint8_t* data);

    void instruction_RollLeft(uint8_t* data);
    void instruction_RollLeftA(uint8_t* data);
    void instruction_RollLeftCarry(uint8_t* data);
    void instruction_RollLeftCarryA(uint8_t* data);
    void instruction_RollRightCarry(uint8_t* data);
    void instruction_RollRightCarryA(uint8_t* data);
    void instruction_RollRight(uint8_t* data);
    void instruction_RollRightA(uint8_t* data);

    void instruction_ResetBit(uint8_t* data);
    void instruction_SetBit(uint8_t* data);
    void instruction_CheckBit(uint8_t* data);
    void instruction_SP2Mem(uint8_t* data);
    void instruction_SCF(uint8_t* data);
    void instruction_CCF(uint8_t* data);
    void instruction_SBC(uint8_t* data);
    void instruction_SLA(uint8_t* data);
    void instruction_SRA(uint8_t* data);

    void instruction_Halt(uint8_t* data);

    void instruction_SetInterrupt(uint8_t* data);
    void handleInterrupts();
    void serviceInterrupts(uint16_t addr, uint8_t flag);

public:
    CentralProcessingUnit(MemoryManagementUnit *mmu);
    ~CentralProcessingUnit();
    void Print();

    uint8_t ExecuteInstruction(uint16_t skipDebug);
};
