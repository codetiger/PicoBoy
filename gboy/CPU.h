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
    std::map<uint16_t, Instruction*> instructionSetExtended;

    uint8_t getFlags();
    void setFlags(uint8_t val);
    bool check_bit(const uint8_t value, const uint8_t bit);
    void stackPush(uint16_t val);
    void stackPush(uint8_t hi, uint8_t lo);
    uint16_t stackPop();

    bool instruction_NOP(uint8_t* data);
    bool instruction_XOROP(uint8_t* data);

    bool instruction_Inc(uint8_t* data);
    bool instruction_Dec(uint8_t* data);
    bool instruction_Inc16Bit(uint8_t* data);
    bool instruction_Dec16Bit(uint8_t* data);    
    
    bool instruction_LoadSP(uint8_t* data);
    bool instruction_LoadSPHL(uint8_t* data);
    bool instruction_LoadPair(uint8_t* data);
    bool instruction_Load(uint8_t* data);
    bool instruction_LoadMem2Reg(uint8_t* data);
    bool instruction_LoadAIndirect(uint8_t* data);
    bool instruction_LoadA2Mem(uint8_t* data);
    bool instruction_LoadReg2Reg(uint8_t* data);

    bool instruction_JR(uint8_t* data);
    bool instruction_Call(uint8_t* data);
    bool instruction_Push(uint8_t* data);
    bool instruction_Pop(uint8_t* data);
    bool instruction_Return(uint8_t* data);
    bool instruction_Reti(uint8_t* data);
    
    bool instruction_Compare(uint8_t* data);
    bool instruction_LoadAnn(uint8_t* data);

    bool instruction_Sub(uint8_t* data);
    bool instruction_Add(uint8_t* data);
    bool instruction_Adc(uint8_t* data);
    bool instruction_Jump(uint8_t* data);
    bool instruction_LoadAHL(uint8_t* data);
    bool instruction_LoadHL(uint8_t* data);
    bool instruction_JumpHL(uint8_t* data);

    bool instruction_OR(uint8_t* data);
    bool instruction_AND(uint8_t* data);
    bool instruction_CPL(uint8_t* data);
    bool instruction_Swap(uint8_t* data);
    bool instruction_Reset(uint8_t* data);
    bool instruction_AddPair(uint8_t* data);

    bool instruction_SRL(uint8_t* data);
    bool instruction_DAA(uint8_t* data);

    bool instruction_RollLeft(uint8_t* data);
    bool instruction_RollLeftCarry(uint8_t* data);
    bool instruction_RollRightCarry(uint8_t* data);
    bool instruction_RollRight(uint8_t* data);

    bool instruction_ResetBit(uint8_t* data);
    bool instruction_SetBit(uint8_t* data);
    bool instruction_CheckBit(uint8_t* data);
    bool instruction_SP2Mem(uint8_t* data);
    bool instruction_SCF(uint8_t* data);
    bool instruction_CCF(uint8_t* data);
    bool instruction_SBC(uint8_t* data);
    bool instruction_SLA(uint8_t* data);
    bool instruction_SRA(uint8_t* data);

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
    this->accumulator = b = c = d = e = h = l = 0;
    const std::string regName[] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};

    instructionSet[0x0] = new Instruction("NOP", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xcb] = new Instruction("Extended Instruction", 1, 4, &CentralProcessingUnit::instruction_NOP);

    for (int i = 0; i < 8; i++) {
        instructionSet[0xa8 + i] = new Instruction("XOR " + regName[i], 1, (i==6) ? 8 : 4, &CentralProcessingUnit::instruction_XOROP);
        instructionSet[0x06 + i*8] = new Instruction("LD " + regName[i] + " n", 2, (i==6) ? 12 : 8, &CentralProcessingUnit::instruction_LoadMem2Reg);
        instructionSet[0x04 + i*8] = new Instruction("INC " + regName[i], 1, (i==6) ? 12 : 4, &CentralProcessingUnit::instruction_Inc);
        instructionSet[0x05 + i*8] = new Instruction("DEC " + regName[i], 1, (i==6) ? 12 : 4, &CentralProcessingUnit::instruction_Dec);
        instructionSet[0xb8 + i] = new Instruction("CMP " + regName[i], 1, (i==6) ? 8 : 4, &CentralProcessingUnit::instruction_Compare);
        instructionSet[0x90 + i] = new Instruction("SUB " + regName[i], 1, (i==6) ? 8 : 4, &CentralProcessingUnit::instruction_Sub);
        instructionSet[0x80 + i] = new Instruction("ADD " + regName[i], 1, (i==6) ? 8 : 4, &CentralProcessingUnit::instruction_Add);
        instructionSet[0x88 + i] = new Instruction("ADC " + regName[i], 1, (i==6) ? 8 : 4, &CentralProcessingUnit::instruction_Adc);
        instructionSet[0xb0 + i] = new Instruction("OR " + regName[i], 1, (i==6) ? 8 : 4, &CentralProcessingUnit::instruction_OR);
        instructionSet[0xa0 + i] = new Instruction("AND " + regName[i], 1, (i==6) ? 8 : 4, &CentralProcessingUnit::instruction_AND);
        instructionSet[0x98 + i] = new Instruction("SBC " + regName[i], 1, (i==6) ? 8 : 4, &CentralProcessingUnit::instruction_SBC);
        instructionSetExtended[0x00 + i] = new Instruction("RLC " + regName[i], 1, (i==6) ? 16 : 8, &CentralProcessingUnit::instruction_RollLeftCarry);
        instructionSetExtended[0x08 + i] = new Instruction("RRC " + regName[i], 1, (i==6) ? 16 : 8, &CentralProcessingUnit::instruction_RollRightCarry);
        instructionSetExtended[0x10 + i] = new Instruction("RL " + regName[i], 1, (i==6) ? 16 : 8, &CentralProcessingUnit::instruction_RollLeft);
        instructionSetExtended[0x18 + i] = new Instruction("RR " + regName[i], 1, (i==6) ? 16 : 8, &CentralProcessingUnit::instruction_RollRight);

        instructionSetExtended[0x30 + i] = new Instruction("SWAP " + regName[i], 1, (i==6) ? 16 : 8, &CentralProcessingUnit::instruction_Swap);
        instructionSetExtended[0x38 + i] = new Instruction("SRL " + regName[i], 1, (i==6) ? 16 : 8, &CentralProcessingUnit::instruction_SRL);
        instructionSetExtended[0x20 + i] = new Instruction("SLA " + regName[i], 1, (i==6) ? 16 : 8, &CentralProcessingUnit::instruction_SLA);
        instructionSetExtended[0x28 + i] = new Instruction("SRA " + regName[i], 1, (i==6) ? 16 : 8, &CentralProcessingUnit::instruction_SRA);        
    }

    instructionSet[0x07] = new Instruction("RLCA", 1, 4, &CentralProcessingUnit::instruction_RollLeftCarry);
    instructionSet[0x0f] = new Instruction("RRCA", 1, 4, &CentralProcessingUnit::instruction_RollRightCarry);
    instructionSet[0x17] = new Instruction("RLA", 1, 4, &CentralProcessingUnit::instruction_RollLeft);
    instructionSet[0x1f] = new Instruction("RRA", 1, 4, &CentralProcessingUnit::instruction_RollRight);
    instructionSet[0xee] = new Instruction("XOR A,u8", 2, 8, &CentralProcessingUnit::instruction_XOROP);
    instructionSet[0xde] = new Instruction("SBC A,u8", 2, 8, &CentralProcessingUnit::instruction_SBC);

    for (int i = 0; i < 64; i++) {
        std::string name = std::to_string(i/8) + "," + regName[i%8];
        instructionSet[0x40 + i] = new Instruction("LD " + regName[i/8] + "," + regName[i%8], 1, (i%8) == 6 ? 8 : 4, &CentralProcessingUnit::instruction_LoadReg2Reg);
        instructionSetExtended[0x40 + i] = new Instruction("BIT " + name, 1, (i%8) == 6 ? 16 : 8, &CentralProcessingUnit::instruction_CheckBit);
        instructionSetExtended[0x80 + i] = new Instruction("RES " + name, 1, (i%8) == 6 ? 16 : 8, &CentralProcessingUnit::instruction_ResetBit);
        instructionSetExtended[0xc0 + i] = new Instruction("SET " + name, 1, (i%8) == 6 ? 16 : 8, &CentralProcessingUnit::instruction_SetBit);
    }

    instructionSet[0x01] = new Instruction("LD BC nn", 3, 12, &CentralProcessingUnit::instruction_LoadPair);
    instructionSet[0x11] = new Instruction("LD DE nn", 3, 12, &CentralProcessingUnit::instruction_LoadPair);
    instructionSet[0x21] = new Instruction("LD HL nn", 3, 12, &CentralProcessingUnit::instruction_LoadPair);
    instructionSet[0x31] = new Instruction("LD SP,nn", 3, 12, &CentralProcessingUnit::instruction_LoadSP);

    instructionSet[0x22] = new Instruction("LD (HL+), A", 1, 8, &CentralProcessingUnit::instruction_Load);
    instructionSet[0x32] = new Instruction("LD (HL-) A", 1, 8, &CentralProcessingUnit::instruction_Load);
    instructionSet[0xe0] = new Instruction("LD (FF00+u8),A", 2, 12, &CentralProcessingUnit::instruction_Load);
    instructionSet[0xe2] = new Instruction("LD (FF00+C),A", 1, 8, &CentralProcessingUnit::instruction_Load);
    instructionSet[0xf0] = new Instruction("LD A,(FF00+u8)", 2, 12, &CentralProcessingUnit::instruction_LoadAnn);

    instructionSet[0x0a] = new Instruction("LD A (BC)", 1, 8, &CentralProcessingUnit::instruction_LoadAIndirect);
    instructionSet[0x1a] = new Instruction("LD A (DE)", 1, 8, &CentralProcessingUnit::instruction_LoadAIndirect);
    instructionSet[0xfa] = new Instruction("LD A nn", 3, 16, &CentralProcessingUnit::instruction_LoadAIndirect);

    instructionSet[0x02] = new Instruction("LD (BC) A", 1, 8, &CentralProcessingUnit::instruction_LoadA2Mem);
    instructionSet[0x12] = new Instruction("LD (DE) A", 1, 8, &CentralProcessingUnit::instruction_LoadA2Mem);
    instructionSet[0xea] = new Instruction("LD (nn) A", 3, 16, &CentralProcessingUnit::instruction_LoadA2Mem);

    instructionSet[0x08] = new Instruction("LD (u16),SP", 3, 20, &CentralProcessingUnit::instruction_SP2Mem);

    instructionSet[0xc6] = new Instruction("ADD n", 2, 8, &CentralProcessingUnit::instruction_Add);
    instructionSet[0xd6] = new Instruction("SUB n", 2, 8, &CentralProcessingUnit::instruction_Sub);
    instructionSet[0xe6] = new Instruction("AND n", 2, 8, &CentralProcessingUnit::instruction_AND);
    instructionSet[0xf6] = new Instruction("OR n", 2, 8, &CentralProcessingUnit::instruction_OR);

    instructionSet[0x09] = new Instruction("ADD HL, BC", 1, 8, &CentralProcessingUnit::instruction_AddPair);
    instructionSet[0x19] = new Instruction("ADD HL, DE", 1, 8, &CentralProcessingUnit::instruction_AddPair);
    instructionSet[0x29] = new Instruction("ADD HL, HL", 1, 8, &CentralProcessingUnit::instruction_AddPair);

    instructionSet[0xfe] = new Instruction("CMP n", 2, 8, &CentralProcessingUnit::instruction_Compare);
    instructionSet[0xce] = new Instruction("ADC n", 2, 8, &CentralProcessingUnit::instruction_Adc);

    instructionSet[0x18] = new Instruction("JR n", 2, 12, &CentralProcessingUnit::instruction_JR);
    instructionSet[0x20] = new Instruction("JR NZ n", 2, 8, &CentralProcessingUnit::instruction_JR);
    instructionSet[0x28] = new Instruction("JR Z n", 2, 8, &CentralProcessingUnit::instruction_JR);
    instructionSet[0x30] = new Instruction("JR NC n", 2, 8, &CentralProcessingUnit::instruction_JR);
    instructionSet[0x38] = new Instruction("JR C n", 2, 8, &CentralProcessingUnit::instruction_JR);

    instructionSet[0xc4] = new Instruction("CALL NZ nn", 3, 12, &CentralProcessingUnit::instruction_Call);
    instructionSet[0xcc] = new Instruction("CALL Z nn", 3, 12, &CentralProcessingUnit::instruction_Call);
    instructionSet[0xcd] = new Instruction("CALL nn", 3, 24, &CentralProcessingUnit::instruction_Call);
    instructionSet[0xd4] = new Instruction("CALL NC nn", 3, 24, &CentralProcessingUnit::instruction_Call);
    instructionSet[0xdc] = new Instruction("CALL C nn", 3, 24, &CentralProcessingUnit::instruction_Call);

    instructionSet[0xc5] = new Instruction("PUSH BC", 1, 16, &CentralProcessingUnit::instruction_Push);
    instructionSet[0xd5] = new Instruction("PUSH DE", 1, 16, &CentralProcessingUnit::instruction_Push);
    instructionSet[0xe5] = new Instruction("PUSH HL", 1, 16, &CentralProcessingUnit::instruction_Push);
    instructionSet[0xf5] = new Instruction("PUSH AF", 1, 16, &CentralProcessingUnit::instruction_Push);

    instructionSet[0xc1] = new Instruction("POP BC", 1, 12, &CentralProcessingUnit::instruction_Pop);
    instructionSet[0xd1] = new Instruction("POP DE", 1, 12, &CentralProcessingUnit::instruction_Pop);
    instructionSet[0xe1] = new Instruction("POP HL", 1, 12, &CentralProcessingUnit::instruction_Pop);
    instructionSet[0xf1] = new Instruction("POP AF", 1, 12, &CentralProcessingUnit::instruction_Pop);

    instructionSet[0x03] = new Instruction("INC BC", 1, 8, &CentralProcessingUnit::instruction_Inc16Bit);
    instructionSet[0x13] = new Instruction("INC DE", 1, 8, &CentralProcessingUnit::instruction_Inc16Bit);
    instructionSet[0x23] = new Instruction("INC HL", 1, 8, &CentralProcessingUnit::instruction_Inc16Bit);
    instructionSet[0x33] = new Instruction("INC SP", 1, 8, &CentralProcessingUnit::instruction_Inc16Bit);

    instructionSet[0x0b] = new Instruction("DEC BC", 1, 8, &CentralProcessingUnit::instruction_Dec16Bit);
    instructionSet[0x1b] = new Instruction("DEC DE", 1, 8, &CentralProcessingUnit::instruction_Dec16Bit);
    instructionSet[0x2b] = new Instruction("DEC HL", 1, 8, &CentralProcessingUnit::instruction_Dec16Bit);
    instructionSet[0x3b] = new Instruction("DEC SP", 1, 8, &CentralProcessingUnit::instruction_Dec16Bit);

    instructionSet[0xc0] = new Instruction("RET NZ", 1, 8, &CentralProcessingUnit::instruction_Return);
    instructionSet[0xc8] = new Instruction("RET Z", 1, 8, &CentralProcessingUnit::instruction_Return);
    instructionSet[0xc9] = new Instruction("RET", 1, 8, &CentralProcessingUnit::instruction_Return);
    instructionSet[0xd0] = new Instruction("RET NC", 1, 8, &CentralProcessingUnit::instruction_Return);
    instructionSet[0xd8] = new Instruction("RET C", 1, 8, &CentralProcessingUnit::instruction_Return);

    instructionSet[0xc2] = new Instruction("JP NZ nn", 3, 12, &CentralProcessingUnit::instruction_Jump);
    instructionSet[0xc3] = new Instruction("JP nn", 3, 12, &CentralProcessingUnit::instruction_Jump);
    instructionSet[0xca] = new Instruction("JP Z nn", 3, 12, &CentralProcessingUnit::instruction_Jump);
    instructionSet[0xd2] = new Instruction("JP NC nn", 3, 12, &CentralProcessingUnit::instruction_Jump);
    instructionSet[0xda] = new Instruction("JP CZ nn", 3, 12, &CentralProcessingUnit::instruction_Jump);
    instructionSet[0xe9] = new Instruction("JP HL", 1, 4, &CentralProcessingUnit::instruction_JumpHL);

    instructionSet[0xf3] = new Instruction("DI", 1, 4, &CentralProcessingUnit::instruction_SetInterrupt);
    instructionSet[0xfb] = new Instruction("EI", 1, 4, &CentralProcessingUnit::instruction_SetInterrupt);

    instructionSet[0x2a] = new Instruction("LD HL+ A", 1, 8, &CentralProcessingUnit::instruction_LoadAHL);
    instructionSet[0x3a] = new Instruction("LD HL- A", 1, 8, &CentralProcessingUnit::instruction_LoadAHL);

    instructionSet[0xd9] = new Instruction("RETI", 1, 16, &CentralProcessingUnit::instruction_Reti);
    instructionSet[0x2f] = new Instruction("CPL", 1, 4, &CentralProcessingUnit::instruction_CPL);

    instructionSet[0xc7] = new Instruction("RST 00h", 1, 16, &CentralProcessingUnit::instruction_Reset);
    instructionSet[0xcf] = new Instruction("RST 08h", 1, 16, &CentralProcessingUnit::instruction_Reset);
    instructionSet[0xd7] = new Instruction("RST 10h", 1, 16, &CentralProcessingUnit::instruction_Reset);
    instructionSet[0xdf] = new Instruction("RST 18h", 1, 16, &CentralProcessingUnit::instruction_Reset);
    instructionSet[0xe7] = new Instruction("RST 20h", 1, 16, &CentralProcessingUnit::instruction_Reset);
    instructionSet[0xef] = new Instruction("RST 28h", 1, 16, &CentralProcessingUnit::instruction_Reset);
    instructionSet[0xf7] = new Instruction("RST 30h", 1, 16, &CentralProcessingUnit::instruction_Reset);
    instructionSet[0xff] = new Instruction("RST 38h", 1, 16, &CentralProcessingUnit::instruction_Reset);

    instructionSet[0x27] = new Instruction("DAA", 1, 4, &CentralProcessingUnit::instruction_DAA);
    instructionSet[0xf8] = new Instruction("LD HL,SP+i8", 2, 12, &CentralProcessingUnit::instruction_LoadHL);
    instructionSet[0xf9] = new Instruction("LD SP,HL", 1, 8, &CentralProcessingUnit::instruction_LoadSPHL);

    instructionSet[0x10] = new Instruction("STOP", 2, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0x76] = new Instruction("HALT", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xf4] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xed] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xeb] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);

    instructionSet[0x37] = new Instruction("SCF", 1, 4, &CentralProcessingUnit::instruction_SCF);
    instructionSet[0x3f] = new Instruction("CCF", 1, 4, &CentralProcessingUnit::instruction_CCF);

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

bool CentralProcessingUnit::check_bit(const uint8_t value, const uint8_t bit) {
    return (value & (1 << bit)) != 0;
}

void CentralProcessingUnit::setFlags(uint8_t val) {
	this->isZero        = (val >> 7) & 1;
	this->isSubtract   = (val >> 6) & 1;
	this->isHalfCarry   = (val >> 5) & 1;
	this->isCarry       = (val >> 4) & 1;
}

void CentralProcessingUnit::stackPush(uint16_t val) {
    uint8_t hi = (val >> 8) & 0xff;
    uint8_t lo = val & 0xff;
    this->stackPush(hi, lo);
}

void CentralProcessingUnit::stackPush(uint8_t hi, uint8_t lo) {
    this->stackPointer--;
    mmu->Write(this->stackPointer, hi);
    this->stackPointer--;
    mmu->Write(this->stackPointer, lo);
}

uint16_t CentralProcessingUnit::stackPop() {
    uint8_t lo = mmu->Read(this->stackPointer);
    this->stackPointer++;
    uint8_t hi = mmu->Read(this->stackPointer);
    this->stackPointer++;
    uint16_t val = hi;
    val = (val << 8) | lo;
    return val;
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
    bool debug = (this->ProgramCounter >= skipDebug);
    this->deltaTime = 0;

    handleInterrupts();

    if(!isHalted) {
        uint16_t opcode = mmu->Read(this->ProgramCounter);
        if(this->ProgramCounter == 0x0 && this->time != 0) {
            printf("Starting execution at 0x0000\n");
            exit(0);
        }
        bool isExtended = false;
        if(debug) printf("Executing at 0x%04x, OpCode: 0x%02x ", this->ProgramCounter, opcode);
        if(opcode == 0xcb) {
            this->ProgramCounter++;
            opcode = mmu->Read(this->ProgramCounter);
            isExtended = true;
            if(debug) printf("Extended OpCode: 0x%04x ", opcode);
        }

        std::map<uint16_t, Instruction*> &iset = isExtended ? instructionSetExtended : instructionSet;
        std::map<uint16_t, Instruction*>::iterator it = iset.find(opcode);
        if (it != iset.end()) {
            Instruction *inst = it->second;
            if(debug) printf("Name: %s, ", inst->name.c_str());
            uint8_t data[inst->size];
            if(debug) printf("Data: ");
            for (size_t i = 0; i < inst->size; i++) {
                data[i] = mmu->Read(this->ProgramCounter + i);
                if(debug && i > 0) printf("%d, ", data[i]);
            }
            if(debug) printf("\n");
            
            this->deltaTime = inst->cycles;
            if((this->*(inst->code))(data)) {
                this->ProgramCounter += inst->size;
            }
            this->time += this->deltaTime;
        } else {
            printf("OpCode Not Implemented at 0x%04x, OpCode: 0x%02x Ext: %d\n", this->ProgramCounter, opcode, isExtended);
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
    this->stackPush(this->ProgramCounter);
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
    this->stackPointer = (((uint16_t)hi) << 8) | lo;
    return true;
}

bool CentralProcessingUnit::instruction_XOROP(uint8_t* data) {
    uint8_t d;
    
    if (data[0] == 0xa8)
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
    } else if (data[0] == 0xaf)
        d = this->accumulator;
    else if (data[0] == 0xee)
        d = data[1];

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
        case 0x22: {
            uint16_t addr = (((uint16_t)this->h)<<8) | this->l;
            mmu->Write(addr, this->accumulator);
            addr++;
            this->h = (addr >> 8) & 0xff;
            this->l = addr & 0xff;
            break;
        }
        case 0x32: {
            uint16_t addr = (((uint16_t)this->h) << 8) | this->l;
            mmu->Write(addr, this->accumulator);
            addr--;
            this->h = (addr >> 8) & 0xff;
            this->l = addr & 0xff;
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
        default:
            break;
    }
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

    if (condition) {
        this->deltaTime = 12;
        int8_t val = static_cast<int8_t>(data[1]);
        this->ProgramCounter += val;
    }
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
    else if (data[0] == 0x36) {
        uint16_t addr = (((uint16_t)(this->h)) << 8) |(this -> l);
        mmu->Write(addr, data[1]);
        return true;
    } else if (data[0] == 0x3e)
        reg = &(this->accumulator);

    *reg = data[1];
    return true;
}

bool CentralProcessingUnit::instruction_Inc(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x04)
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
        val++;
        this->isHalfCarry = ((val & 0xf) == 1);
        this->isZero = (val == 0);
        this->isSubtract = 0;
        mmu->Write(addr, val);
        return true;
    } else if (data[0] == 0x3c)
        reg = &(this->accumulator);

    (*reg)++;
    this->isHalfCarry = (((*reg) & 0xf) == 0);
    this->isZero = (*reg) == 0;
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
        val--;
        this->isHalfCarry = ((val & 0xf) == 1);
        this->isZero = (val == 0);
        this->isSubtract = 1;
        mmu->Write(addr, val);
        return true;
    }

    (*reg)--;
    this->isHalfCarry = (((*reg) & 0xf) == 0);
    this->isZero = (*reg) == 0;
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

bool CentralProcessingUnit::instruction_LoadAIndirect(uint8_t* data) {
    uint16_t hi, lo;
    if (data[0] == 0x0a)
        hi = this->b, lo = this->c;
    else if (data[0] == 0x1a)
        hi = this->d, lo = this->e;
    else if (data[0] == 0xfa)
        hi = data[2], lo = data[1];

    uint16_t addr = (((uint16_t)hi) << 8) | lo;
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

    uint16_t addr = (((uint16_t)hi) << 8) | lo;
    mmu->Write(addr, this->accumulator);
    return true;
}

bool CentralProcessingUnit::instruction_LoadReg2Reg(uint8_t* data) {
    uint8_t dreg = (data[0] & 0xf) % 8;
    uint8_t dval = (data[0] - 0x40) / 8;

    uint8_t val = 0;
    if (dreg == 0x0)
        val = this->b;
    else if (dreg == 0x1)
        val = this->c;
    else if (dreg == 0x2)
        val = this->d;
    else if (dreg == 0x3)
        val = this->e;
    else if (dreg == 0x4)
        val = this->h;
    else if (dreg == 0x5)
        val = this->l;
    else if (dreg == 0x6) {
        uint16_t addr = ((uint16_t)(this->h) << 8) | (this->l);
        val = mmu->Read(addr);
    } else if (dreg == 0x7)
        val = this->accumulator;

    if(dval == 0x0)
        this->b = val;
    else if(dval == 0x1)
        this->c = val;
    else if(dval == 0x2)
        this->d = val;
    else if(dval == 0x3)
        this->e = val;
    else if(dval == 0x4)
        this->h = val;
    else if(dval == 0x5)
        this->l = val;
    else if(dval == 0x6) {
        uint16_t addr = (((uint16_t)(this->h)) << 8) | (this -> l);
        mmu->Write(addr, val);
    } else if(dval == 0x7)
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
        this->stackPush(this->ProgramCounter + 3);

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

    this->stackPush(hi, lo);
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
        this->ProgramCounter = this->stackPop();
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
    if (data[0] == 0xb8)
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
        n = mmu->Read((((uint16_t)this->h) << 8) | (this->l));
    else if (data[0] == 0xbf)
        n = this->accumulator;
    else if (data[0] == 0xfe)
        n = data[1];

    this->isZero = (this->accumulator == n);
    this->isSubtract = true;
    this->isHalfCarry = ((this->accumulator & 0xf) - (n & 0xf)) < 0;
    this->isCarry = (this->accumulator < n);
    return true;
}

bool CentralProcessingUnit::instruction_LoadAnn(uint8_t* data) {
    this->accumulator = mmu->Read(data[1] + (uint16_t)0xff00);
    return true;
}

bool CentralProcessingUnit::instruction_LoadSPHL(uint8_t* data) {
    uint16_t addr = this->h;
    addr = (addr << 8) | this->l;
    this->stackPointer = addr;
    return true;
}

bool CentralProcessingUnit::instruction_Sub(uint8_t* data) {
    uint8_t d;
    
    if (data[0] == 0x90)
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
    } else if (data[0] == 0x97)
        d = this->accumulator;
    else if (data[0] == 0xd6)
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
    if (data[0] == 0x80)
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
    } else if (data[0] == 0x87)
        d = this->accumulator;
    else if (data[0] == 0xc6)
        d = data[1];

    uint16_t val = this->accumulator + d;

    this->isZero = (val & 0xff) == 0;
    this->isCarry = (val & 0x100) != 0;
    this->isHalfCarry = (this->accumulator & 0xf) + (d & 0xf) > 0xf;
    this->isSubtract = 0;

    this->accumulator = val;
    return true;
}

bool CentralProcessingUnit::instruction_Adc(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0x88)
        d = this->b;
    else if (data[0] == 0x89)
        d = this->c;
    else if (data[0] == 0x8a)
        d = this->d;
    else if (data[0] == 0x8b)
        d = this->e;
    else if (data[0] == 0x8c)
        d = this->h;
    else if (data[0] == 0x8d)
        d = this->l;
    else if (data[0] == 0x8e) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        d = mmu->Read(addr);
    } else if (data[0] == 0x8f)
        d = this->accumulator;
    else if (data[0] == 0xce)
        d = data[1];

    uint16_t val = d + this->accumulator + this->isCarry;
    this->isZero = (val & 0xff) == 0;
    this->isCarry = (val & 0x100) != 0;
    this->isHalfCarry = ((this->accumulator & 0xf) + (d & 0xf)) > 0xf;
    this->isSubtract = 0;

    this->accumulator = val;
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
    if(data[0] == 0x2a)
        addr++;
    else if(data[0] == 0x3a)
        addr--;
    this->h = (addr >> 8) & 0xff;
    this->l = addr & 0xff;
    return true;
}

bool CentralProcessingUnit::instruction_LoadHL(uint8_t* data) {
    int8_t value = data[1];
    int result = static_cast<int>(this->stackPointer + value);
    this->isZero = false;
    this->isSubtract = false;
    this->isHalfCarry = (((this->stackPointer ^ value ^ (result & 0xFFFF)) & 0x10) == 0x10);
    this->isCarry = (((this->stackPointer ^ value ^ (result & 0xFFFF)) & 0x100) == 0x100);

    this->h = (result >> 8) & 0xff;
    this->l = result & 0xff;
    return true;
}

bool CentralProcessingUnit::instruction_OR(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0xb0)
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
    } else if (data[0] == 0xb7)
        d = this->accumulator;
    else if (data[0] == 0xf6)
        d = data[1];

    this->accumulator |= d;
    this->isZero = (this->accumulator == 0);
    this->isCarry = this->isHalfCarry = this->isSubtract = 0;
    return true;
}

bool CentralProcessingUnit::instruction_AND(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0xa0)
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
    } else if (data[0] == 0xa7)
        d = this->accumulator;
    else if (data[0] == 0xe6)
        d = data[1];

    this->accumulator &= d;
    this->isZero = (this->accumulator == 0);
    this->isHalfCarry = true;
    this->isCarry = this->isSubtract = 0;
    return true;
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
    this->ProgramCounter = this->stackPop();
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
    this->isSubtract = this->isHalfCarry = this->isCarry = false;
    uint8_t* reg;
    if (data[0] == 0x30)
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
    } else if (data[0] == 0x37)
        reg = &(this->accumulator);

    uint8_t val = (*reg) >> 4;
    val |= ((*reg) << 4);
    *reg = val;
    this->isZero = (val == 0);
    return true;
}

bool CentralProcessingUnit::instruction_Reset(uint8_t* data) {
    this->deltaTime = 16;
    this->stackPush(this->ProgramCounter + 1);

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
    uint16_t ans = val + hl;
    this->isSubtract = 0;
    this->isHalfCarry = ((hl & 0xfff) + (val & 0xfff)) > 0xfff;
    this->isCarry = (ans & 0x10000) != 0;
    this->h = ans >> 8;
    this->l = ans & 0xff;
    return true;
}

bool CentralProcessingUnit::instruction_JumpHL(uint8_t* data) {
    uint16_t addr = this->h;
    addr = (addr << 8) | this->l;
    this->ProgramCounter = addr;
    this->deltaTime = 4;
    return false;
}

bool CentralProcessingUnit::instruction_ResetBit(uint8_t* data) {
    uint8_t dreg = (data[0] & 0xf) % 8;
    uint8_t dbit = (data[0] - 0x80) / 8;
    uint8_t* reg;
    if (dreg == 0x0)
        reg = &(this->b);
    else if (dreg == 0x1)
        reg = &(this->c);
    else if (dreg == 0x2)
        reg = &(this->d);
    else if (dreg == 0x3)
        reg = &(this->e);
    else if (dreg == 0x4)
        reg = &(this->h);
    else if (dreg == 0x5)
        reg = &(this->l);
    else if (dreg == 0x6) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        uint8_t val = mmu->Read(addr);
        val &= ~(1 << dbit);
        mmu->Write(addr, val);
        return true;
    } else if (dreg == 0x7)
        reg = &(this->accumulator);

    (*reg) &= ~(1 << dbit);
    return true;
}

bool CentralProcessingUnit::instruction_SetBit(uint8_t* data) {
    uint8_t dreg = (data[0] & 0xf) % 8;
    uint8_t dbit = (data[0] - 0x80) / 8;
    uint8_t* reg;
    if (dreg == 0x0)
        reg = &(this->b);
    else if (dreg == 0x1)
        reg = &(this->c);
    else if (dreg == 0x2)
        reg = &(this->d);
    else if (dreg == 0x3)
        reg = &(this->e);
    else if (dreg == 0x4)
        reg = &(this->h);
    else if (dreg == 0x5)
        reg = &(this->l);
    else if (dreg == 0x6) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        uint8_t val = mmu->Read(addr);
        val |= (1 << dbit);
        mmu->Write(addr, val);
        return true;
    } else if (dreg == 0x7)
        reg = &(this->accumulator);

    (*reg) |= (1 << dbit);
    return true;
}

bool CentralProcessingUnit::instruction_SP2Mem(uint8_t* data) {
    uint16_t lo = data[1];
    uint16_t hi = data[2];
    uint16_t addr = (((uint16_t)hi) << 8) | lo;
    mmu->Write(addr, this->stackPointer);
    return true;
}

bool CentralProcessingUnit::instruction_CheckBit(uint8_t* data) {
    uint8_t dreg = (data[0] & 0xf) % 8;
    uint8_t dbit = (data[0] - 0x40) / 8;

    uint8_t val;
    if (dreg == 0x0)
        val = this->b;
    else if (dreg == 0x1)
        val = this->c;
    else if (dreg == 0x2)
        val = this->d;
    else if (dreg == 0x3)
        val = this->e;
    else if (dreg == 0x4)
        val = this->h;
    else if (dreg == 0x5)
        val = this->l;
    else if (dreg == 0x6) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        val = mmu->Read(addr);
    } else if (dreg == 0x7)
        val = this->accumulator;

    this->isZero = (val & (1 << dbit)) == 0;
    this->isHalfCarry = 1;
    this->isSubtract = 0;
    return true;
}

bool CentralProcessingUnit::instruction_RollLeft(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x10)
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
        this->isHalfCarry = false;
        this->isSubtract = false;

        mmu->Write(addr, val);
        return true;
    } else if (data[0] == 0x17)
        reg = &(this->accumulator);

    int temp = this->isCarry;
    this->isCarry = check_bit((*reg), 7);
    *reg = (*reg) << 1 | temp;
    this->isZero = (*reg) == 0;
    this->isHalfCarry = false;
    this->isSubtract = false;

    return true;
}

bool CentralProcessingUnit::instruction_RollRight(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x10)
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
        this->isCarry = check_bit(val, 0);
        val = val >> 1 | (temp << 7);
        this->isZero = (val == 0);
        this->isHalfCarry = false;
        this->isSubtract = false;

        mmu->Write(addr, val);
        return true;
    } else if (data[0] == 0x17)
        reg = &(this->accumulator);

    int temp = this->isCarry;
    this->isCarry = check_bit((*reg), 0);
    *reg = (*reg) >> 1 | (temp << 7);
    this->isZero = (*reg) == 0;
    this->isHalfCarry = false;
    this->isSubtract = false;

    return true;
}

bool CentralProcessingUnit::instruction_RollLeftCarry(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x00)
        reg = &(this->b);
    else if (data[0] == 0x01)
        reg = &(this->c);
    else if (data[0] == 0x02)
        reg = &(this->d);
    else if (data[0] == 0x03)
        reg = &(this->e);
    else if (data[0] == 0x04)
        reg = &(this->h);
    else if (data[0] == 0x05)
        reg = &(this->l);
    else if (data[0] == 0x06) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        uint8_t val = mmu->Read(addr);

        uint8_t carry_flag = check_bit(val, 7);
        uint8_t truncated_bit = check_bit(val, 7);
        val = static_cast<uint8_t>((val << 1) | truncated_bit);

        this->isCarry = carry_flag;
        this->isZero = (*reg) == 0;
        this->isHalfCarry = false;
        this->isSubtract = false;

        mmu->Write(addr, val);
        return true;
    } else if (data[0] == 0x07)
        reg = &(this->accumulator);

    uint8_t carry_flag = check_bit((*reg), 7);
    uint8_t truncated_bit = check_bit((*reg), 7);
    (*reg) = static_cast<uint8_t>(((*reg) << 1) | truncated_bit);

    this->isCarry = carry_flag;
    this->isZero = (*reg) == 0;
    this->isHalfCarry = false;
    this->isSubtract = false;

    return true;
}

bool CentralProcessingUnit::instruction_RollRightCarry(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x00)
        reg = &(this->b);
    else if (data[0] == 0x01)
        reg = &(this->c);
    else if (data[0] == 0x02)
        reg = &(this->d);
    else if (data[0] == 0x03)
        reg = &(this->e);
    else if (data[0] == 0x04)
        reg = &(this->h);
    else if (data[0] == 0x05)
        reg = &(this->l);
    else if (data[0] == 0x06) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        uint8_t val = mmu->Read(addr);

        uint8_t carry_flag = check_bit(val, 0);
        uint8_t truncated_bit = check_bit(val, 0);
        val = (((val) >> 1) | (truncated_bit << 7));

        this->isCarry = carry_flag;
        this->isZero = (val == 0);
        this->isHalfCarry = false;
        this->isSubtract = false;
        mmu->Write(addr, val);
        return true;
    } else if (data[0] == 0x07)
        reg = &(this->accumulator);

    uint8_t carry_flag = check_bit(*reg, 0);
    uint8_t truncated_bit = check_bit(*reg, 0);
    *reg = static_cast<uint8_t>(((*reg) >> 1) | (truncated_bit << 7));

    this->isCarry = carry_flag;
    this->isZero = (*reg) == 0;
    this->isHalfCarry = false;
    this->isSubtract = false;

    return true;
}

bool CentralProcessingUnit::instruction_DAA(uint8_t* data) {
    int tmp = this->accumulator;
    if(!this->isSubtract) {
        if(this->isHalfCarry || (tmp & 0x0f) > 9)
            tmp += 6;
        if(this->isCarry || tmp > 0x9f)
            tmp += 0x60;
    } else {
        if(this->isHalfCarry) {
            tmp -= 6;
            if(this->isCarry)
                tmp &= 0xff;
        }
        if(this->isCarry)
            tmp -= 0x60;
    }

    if ((tmp & 0x100) != 0)
        this->isCarry = true;
    this->isHalfCarry = false;
    this->isZero = tmp == 0;

	this->accumulator = tmp & 0xFF;
    return true;
}

bool CentralProcessingUnit::instruction_SCF(uint8_t* data) {
    this->isCarry = true;
    this->isHalfCarry = this->isSubtract = false;
    return true;
}

bool CentralProcessingUnit::instruction_CCF(uint8_t* data) {
    this->isCarry = !this->isCarry;
    this->isHalfCarry = this->isSubtract = false;
    return true;
}

bool CentralProcessingUnit::instruction_SBC(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0x98)
        d = this->b;
    else if (data[0] == 0x99)
        d = this->c;
    else if (data[0] == 0x9a)
        d = this->d;
    else if (data[0] == 0x9b)
        d = this->e;
    else if (data[0] == 0x9c)
        d = this->h;
    else if (data[0] == 0x9d)
        d = this->l;
    else if (data[0] == 0x9e) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        d = mmu->Read(addr);
    } else if (data[0] == 0x9f)
        d = this->accumulator;
    else if (data[0] == 0xde)
        d = data[1];

    uint8_t carry = this->isCarry;
    int result_full = (int)this->accumulator - d - carry;
    uint8_t result = static_cast<uint8_t>(result_full);

    this->isZero = (result == 0);
    this->isSubtract = true;
    this->isCarry = (result_full < 0);
    this->isHalfCarry = (((this->accumulator & 0xf) - (d & 0xf) - carry) < 0);
    this->accumulator = result;

    return true;
}

bool CentralProcessingUnit::instruction_SLA(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x20)
        reg = &this->b;
    else if (data[0] == 0x21)
        reg = &this->c;
    else if (data[0] == 0x22)
        reg = &this->d;
    else if (data[0] == 0x23)
        reg = &this->e;
    else if (data[0] == 0x24)
        reg = &this->h;
    else if (data[0] == 0x25)
        reg = &this->l;
    else if (data[0] == 0x26) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        uint8_t d = mmu->Read(addr);
        uint8_t carry_bit = check_bit(d, 7);
        uint8_t result = static_cast<uint8_t>(d << 1);
        this->isZero = (result == 0);
        this->isSubtract = false;
        this->isHalfCarry = false;
        this->isCarry = carry_bit;
        mmu->Write(addr, result);
    } else if (data[0] == 0x27)
        reg = &this->accumulator;

    uint8_t carry_bit = check_bit(*reg, 7);
    uint8_t result = static_cast<uint8_t>(*reg << 1);
    this->isZero = (result == 0);
    this->isSubtract = false;
    this->isHalfCarry = false;
    this->isCarry = carry_bit;
    *reg = result;
    return true;
}

bool CentralProcessingUnit::instruction_SRA(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x20)
        reg = &this->b;
    else if (data[0] == 0x21)
        reg = &this->c;
    else if (data[0] == 0x22)
        reg = &this->d;
    else if (data[0] == 0x23)
        reg = &this->e;
    else if (data[0] == 0x24)
        reg = &this->h;
    else if (data[0] == 0x25)
        reg = &this->l;
    else if (data[0] == 0x26) {
        uint16_t addr = this->h;
        addr = (addr << 8) | this->l;
        uint8_t d = mmu->Read(addr);
        uint8_t carry_bit = check_bit(d, 7);
        uint8_t result = static_cast<uint8_t>(d << 1);
        this->isZero = (result == 0);
        this->isSubtract = false;
        this->isHalfCarry = false;
        this->isCarry = carry_bit;
        mmu->Write(addr, result);
    } else if (data[0] == 0x27)
        reg = &this->accumulator;

    uint8_t carry_bit = check_bit(*reg, 0);
    uint8_t top_bit = check_bit(*reg, 7);
    uint8_t result = static_cast<uint8_t>((*reg) >> 1);
    if(top_bit)
        result |= (1 << 7);
    else
        result &= ~(1 << 7);

    this->isZero = (result == 0);
    this->isSubtract = false;
    this->isHalfCarry = false;
    this->isCarry = carry_bit;
    *reg = result;
    return true;
}

