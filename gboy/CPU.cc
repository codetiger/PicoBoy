#include "CPU.h"

CentralProcessingUnit::CentralProcessingUnit(MemoryManagementUnit *m) {
    mmu = m;
    isHalted = false;
    programCounter = 0x00;
    isZero = isSubtract = isCarry = isHalfCarry = false;
    stackPointer = 0x0;
    time = deltaTime = 0;
    interruptMasterFlag = false;
    accumulator = b = c = d = e = h = l = 0;
    const std::string regName[8] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};

    instructionSet[0x0] = new Instruction("NOP", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xcb] = new Instruction("Extended Instruction", 1, 4, &CentralProcessingUnit::instruction_NOP);

    for (int i = 0; i < 8; i++) {
        instructionSet[0xa8 + i] = new Instruction("XOR " + regName[i], 1, (i==6) ? 8 : 4, &CentralProcessingUnit::instruction_XOROP);
        instructionSet[0x06 + i*8] = new Instruction("LD " + regName[i] + " n", 2, (i==6) ? 12 : 8, &CentralProcessingUnit::instruction_LoadMem2Reg);
        instructionSet[0x04 + i*8] = new Instruction("INC " + regName[i], 1, (i==6) ? 12 : 4, &CentralProcessingUnit::instruction_Inc);
        instructionSet[0x05 + i*8] = new Instruction("DEC " + regName[i], 1, (i==6) ? 12 : 4, &CentralProcessingUnit::instruction_Dec);
        instructionSet[0xb8 + i] = new Instruction("CP " + regName[i], 1, (i==6) ? 8 : 4, &CentralProcessingUnit::instruction_Compare);
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

    instructionSet[0x07] = new Instruction("RLCA", 1, 4, &CentralProcessingUnit::instruction_RollLeftCarryA);
    instructionSet[0x0f] = new Instruction("RRCA", 1, 4, &CentralProcessingUnit::instruction_RollRightCarryA);
    instructionSet[0x17] = new Instruction("RLA", 1, 4, &CentralProcessingUnit::instruction_RollLeftA);
    instructionSet[0x1f] = new Instruction("RRA", 1, 4, &CentralProcessingUnit::instruction_RollRightA);
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
    instructionSet[0xf2] = new Instruction("LD A,(FF00+C)", 1, 8, &CentralProcessingUnit::instruction_LoadAnn);

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
    instructionSet[0x39] = new Instruction("ADD HL, SP", 1, 8, &CentralProcessingUnit::instruction_AddPair);

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
    instructionSet[0x76] = new Instruction("HALT", 1, 4, &CentralProcessingUnit::instruction_Halt);
    instructionSet[0xf4] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xd3] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xd8] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xdb] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xdd] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xe3] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xe4] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xe8] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xec] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xed] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xeb] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xfc] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);
    instructionSet[0xfd] = new Instruction("UNDEF", 1, 4, &CentralProcessingUnit::instruction_NOP);

    instructionSet[0x37] = new Instruction("SCF", 1, 4, &CentralProcessingUnit::instruction_SCF);
    instructionSet[0x3f] = new Instruction("CCF", 1, 4, &CentralProcessingUnit::instruction_CCF);

    // printf("List of Opcodes\n");
    // for(std::map<uint16_t, Instruction *>::iterator it = instructionSetExtended.begin(); it != instructionSetExtended.end(); ++it) {
    //     printf("Opcode: 0x%02x Name: %s\n", it->first, it->second->name.c_str());
    // }
}

CentralProcessingUnit::~CentralProcessingUnit() {
}

uint8_t CentralProcessingUnit::getFlags() {
	uint8_t val = 0;
	val |= (((uint8_t)isZero) << 7);
	val |= (((uint8_t)isSubtract) << 6);
	val |= (((uint8_t)isHalfCarry) << 5);
	val |= (((uint8_t)isCarry) << 4);
	return val & 0xf0;
}

bool CentralProcessingUnit::check_bit(const uint8_t value, const uint8_t bit) {
    return (value & (1 << bit)) != 0;
}

void CentralProcessingUnit::setFlags(uint8_t val) {
    val &= 0xf0;
	isZero        = (val >> 7) & 1;
	isSubtract   = (val >> 6) & 1;
	isHalfCarry   = (val >> 5) & 1;
	isCarry       = (val >> 4) & 1;
}

void CentralProcessingUnit::stackPush(uint16_t val) {
    uint8_t hi = (val >> 8) & 0xff;
    uint8_t lo = val & 0xff;
    stackPush(hi, lo);
}

void CentralProcessingUnit::stackPush(uint8_t hi, uint8_t lo) {
    stackPointer--;
    mmu->Write(stackPointer, hi);
    stackPointer--;
    mmu->Write(stackPointer, lo);
}

uint16_t CentralProcessingUnit::stackPop() {
    uint8_t lo = mmu->Read(stackPointer);
    stackPointer++;
    uint8_t hi = mmu->Read(stackPointer);
    stackPointer++;
    uint16_t val = stitch(hi, lo);
    return val;
}

void CentralProcessingUnit::setHL(uint16_t addr) {
    h = (addr >> 8) & 0xff;
    l = addr & 0xff;
}

uint16_t CentralProcessingUnit::getHL() {
    return stitch(h, l);
}

uint16_t CentralProcessingUnit::stitch(uint8_t hi, uint8_t lo) {
    return ((((uint16_t)hi) << 8) | lo);
}

void CentralProcessingUnit::Print() {
	printf("┌───────────────┬───────────────┐\n");
	printf("│ a = %02x\t│\t\t│\n", accumulator);
	printf("│ b = %02x\t│ c = %02x\t│\n", b, c);
	printf("│ d = %02x\t│ e = %02x\t│\n", d, e);
	printf("│ h = %02x\t│ l = %02x\t│\n", h, l);
	printf("├───────────────┼───────────────┤\n");
	printf("│ sp = %04x\t│ pc = %04x\t│\n", stackPointer, programCounter);
	printf("├───────────────┼───────────────┤\n");
	printf("│ zero = %u\t│ subtract = %u\t│\n", isZero, isSubtract);
	printf("│ carry = %u\t│ hcarry = %u\t│\n", isCarry, isHalfCarry);
	printf("└───────────────┴───────────────┘\n");
}

uint8_t CentralProcessingUnit::readMemoryFromProgramCounter() {
    uint8_t val = mmu->Read(programCounter);
    programCounter++;
    return val;
}

uint8_t CentralProcessingUnit::ExecuteInstruction(uint16_t skipDebug) {
    bool debug = (programCounter >= skipDebug);
    deltaTime = 0;
    memset(data, 0, sizeof(data));
    handleInterrupts();

    if(isHalted)
        return 1;

    if(debug) 
        printf("Executing at 0x%04x", programCounter);
    uint16_t opcode = readMemoryFromProgramCounter();
    if(debug) 
        printf(", OpCode: 0x%02x\n", opcode);
    bool isExtended = false;
    if(opcode == 0xcb) {
        if(debug) 
            printf("Executing at 0x%04x", programCounter);
        opcode = readMemoryFromProgramCounter();
        isExtended = true;
        if(debug) 
            printf(", ExtOpCode: 0x%02x\n", opcode);
    }

    std::map<uint16_t, Instruction*> &iset = isExtended ? instructionSetExtended : instructionSet;
    std::map<uint16_t, Instruction*>::iterator it = iset.find(opcode);
    if (it != iset.end()) {
        Instruction *inst = it->second;
        if(debug) printf("Name: %s, ", inst->name.c_str());
        if(debug && inst->size > 1) printf("Data: ");
        for (size_t i = 0; i < inst->size; i++) {
            data[i] = (i == 0) ? opcode : readMemoryFromProgramCounter();
            if(debug && i > 0) printf("%d, ", data[i]);
        }
        if(debug) printf("\n");
        
        deltaTime = inst->cycles;
        (this->*(inst->code))(data);

        time += deltaTime;
    } else {
        printf("OpCode Not Implemented at 0x%04x, OpCode: 0x%02x Ext: %d\n", programCounter, opcode, isExtended);
        exit(0);
    }

    // if(debug) Print();
    return deltaTime;
}

void CentralProcessingUnit::handleInterrupts() {
    if(!interruptMasterFlag)
        return;

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
        serviceInterrupts(AddrVectorVBlank, FlagInterruptVBlank);
    } else if(lcdInterrupt) {
        serviceInterrupts(AddrVectorLcd, FlagInterruptLcd);
    } else if(timerInterrupt) {
        serviceInterrupts(AddrVectorTimer, FlagInterruptTimer);
    } else if(serialInterrupt) {
        serviceInterrupts(AddrVectorSerial, FlagInterruptSerial);
    } else if(joypadInterrupt) {
        serviceInterrupts(AddrVectorInput, FlagInterruptInput);
    }
}

void CentralProcessingUnit::serviceInterrupts(uint16_t addr, uint8_t flag) {
    mmu->WriteIORegisterBit(AddrRegInterruptFlag, flag, false);
    interruptMasterFlag = false;
    isHalted = false;
    stackPush(programCounter);
    programCounter = addr;
}

void CentralProcessingUnit::instruction_NOP(uint8_t* data) {
}

void CentralProcessingUnit::instruction_LoadSP(uint8_t* data) {
    uint8_t hi, lo;
    if (data[0] == 0x31) {
        lo = data[1];
        hi = data[2];
    } else if (data[0] == 0xf9) {
        lo = l;
        hi = h;
    }
    stackPointer = stitch(hi, lo);
}

void CentralProcessingUnit::instruction_XOROP(uint8_t* data) {
    uint8_t d;
    
    if (data[0] == 0xa8)
        d = b;
    else if (data[0] == 0xa9)
        d = c;
    else if (data[0] == 0xaa)
        d = d;
    else if (data[0] == 0xab)
        d = e;
    else if (data[0] == 0xac)
        d = h;
    else if (data[0] == 0xad)
        d = l;
    else if (data[0] == 0xae) {
        uint16_t addr = getHL();
        d = mmu->Read(addr);
    } else if (data[0] == 0xaf)
        d = accumulator;
    else if (data[0] == 0xee)
        d = data[1];

    accumulator ^= d;
    isZero = (accumulator == 0);
    isCarry = isHalfCarry = isSubtract = false;
}

void CentralProcessingUnit::instruction_LoadPair(uint8_t* data) {
    uint8_t *hi, *lo;
    if (data[0] == 0x01)
        hi = &(b), lo = &(c);
    else if (data[0] == 0x11)
        hi = &(d), lo = &(e);
    else if (data[0] == 0x21)
        hi = &(h), lo = &(l);

    *lo = data[1];
    *hi = data[2];
}

void CentralProcessingUnit::instruction_Load(uint8_t* data) {
    if (data[0] == 0x22) {
        uint16_t addr = getHL();
        mmu->Write(addr, accumulator);
        setHL(++addr);
    } else if (data[0] == 0x32) {
        uint16_t addr = getHL();
        mmu->Write(addr, accumulator);
        setHL(--addr);
    } else if (data[0] == 0xe2) {
        uint16_t addr = 0xFF00;
        mmu->Write(addr + c, accumulator);
    } else if (data[0] == 0xe0) {
        uint16_t addr = 0xFF00;
        mmu->Write(addr + data[1], accumulator);
    }
}

void CentralProcessingUnit::instruction_JR(uint8_t* data) {
    bool condition = false;
    if (data[0] == 0x18)
        condition = true;
    else if (data[0] == 0x20)
        condition = !(isZero);
    else if (data[0] == 0x28)
        condition = (isZero);
    else if (data[0] == 0x30)
        condition = !(isCarry);
    else if (data[0] == 0x38)
        condition = (isCarry);

    if (condition) {
        deltaTime = 12;
        int8_t val = static_cast<int8_t>(data[1]);
        uint16_t new_pc = static_cast<uint16_t>(programCounter + val);
        programCounter = new_pc;
    }
}

void CentralProcessingUnit::instruction_LoadMem2Reg(uint8_t* data) {
    uint8_t* reg;
    if (data[0] == 0x06)
        reg = &(b);
    else if (data[0] == 0x0e)
        reg = &(c);
    else if (data[0] == 0x16)
        reg = &(d);
    else if (data[0] == 0x1e)
        reg = &(e);
    else if (data[0] == 0x26)
        reg = &(h);
    else if (data[0] == 0x2e)
        reg = &(l);
    else if (data[0] == 0x36) {
        uint16_t addr = getHL();
        mmu->Write(addr, data[1]);
        return;
    } else if (data[0] == 0x3e)
        reg = &(accumulator);

    *reg = data[1];
}

void CentralProcessingUnit::instruction_Inc(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x04)
        reg = &( b);
    else if (data[0] == 0x0c)
        reg = &( c);
    else if (data[0] == 0x14)
        reg = &( d);
    else if (data[0] == 0x1c)
        reg = &( e);
    else if (data[0] == 0x24)
        reg = &( h);
    else if (data[0] == 0x2c)
        reg = &( l);
    else if (data[0] == 0x34) {
        uint16_t addr = getHL();
        uint8_t val = mmu->Read(addr);
        val++;
        mmu->Write(addr, val);
        isZero = (val == 0);
        isHalfCarry = ((val & 0xf) == 0);
        isSubtract = false;
        return;
    } else if (data[0] == 0x3c)
        reg = &(accumulator);

    (*reg)++;
    isHalfCarry = (((*reg) & 0xf) == 0);
    isZero = (*reg) == 0;
    isSubtract = false;
}

void CentralProcessingUnit::instruction_Dec(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x05)
        reg = &(b);
    else if (data[0] == 0x0d)
        reg = &(c);
    else if (data[0] == 0x15)
        reg = &(d);
    else if (data[0] == 0x1d)
        reg = &(e);
    else if (data[0] == 0x25)
        reg = &(h);
    else if (data[0] == 0x2d)
        reg = &(l);
    else if (data[0] == 0x35) {
        uint16_t addr = getHL();
        uint8_t val = mmu->Read(addr);
        val--;
        isHalfCarry = ((val & 0x0f) == 0x0f);
        isZero = (val == 0);
        isSubtract = true;
        mmu->Write(addr, val);
        return;
    } else if (data[0] == 0x3d)
        reg = &(accumulator);

    (*reg)--;
    isHalfCarry = (((*reg) & 0x0f) == 0x0f);
    isZero = (*reg) == 0;
    isSubtract = true;
}

void CentralProcessingUnit::instruction_Inc16Bit(uint8_t* data) {
    uint8_t *hi, *lo;
    if (data[0] == 0x03)
        hi = &(b), lo = &(c);
    else if (data[0] == 0x13)
        hi = &(d), lo = &(e);
    else if (data[0] == 0x23)
        hi = &(h), lo = &(l);
    else if (data[0] == 0x33) {
        stackPointer++;
        return;
    }

    uint16_t val = stitch(*hi, *lo);
    val++;
    *hi = (val >> 8) & 0xff;
    *lo = val & 0xff;
}

void CentralProcessingUnit::instruction_Dec16Bit(uint8_t* data) {
    uint8_t *hi, *lo;
    if (data[0] == 0x0b)
        hi = &(b), lo = &(c);
    else if (data[0] == 0x1b)
        hi = &(d), lo = &(e);
    else if (data[0] == 0x2b)
        hi = &(h), lo = &(l);
    else if (data[0] == 0x3b) {
        stackPointer--;
        return;
    }

    uint16_t val = stitch(*hi, *lo);
    val--;
    *hi = (val >> 8) & 0xff;
    *lo = val & 0xff;
}

void CentralProcessingUnit::instruction_LoadAIndirect(uint8_t* data) {
    uint16_t hi, lo;
    if (data[0] == 0x0a)
        hi = b, lo = c;
    else if (data[0] == 0x1a)
        hi = d, lo = e;
    else if (data[0] == 0xfa)
        hi = data[2], lo = data[1];

    uint16_t addr = stitch(hi, lo);
    accumulator = mmu->Read(addr);
}

void CentralProcessingUnit::instruction_LoadA2Mem(uint8_t* data) {
    uint16_t hi, lo;
    if (data[0] == 0x02)
        hi = b, lo = c;
    else if (data[0] == 0x12)
        hi = d, lo = e;
    else if (data[0] == 0xea)
        hi = data[2], lo = data[1];

    uint16_t addr = stitch(hi, lo);
    mmu->Write(addr, accumulator);
}

void CentralProcessingUnit::instruction_LoadReg2Reg(uint8_t* data) {
    uint8_t dreg = (data[0] & 0xf) % 8;
    uint8_t dval = (data[0] - 0x40) / 8;

    uint8_t val = 0;
    if (dreg == 0x0)
        val = b;
    else if (dreg == 0x1)
        val = c;
    else if (dreg == 0x2)
        val = d;
    else if (dreg == 0x3)
        val = e;
    else if (dreg == 0x4)
        val = h;
    else if (dreg == 0x5)
        val = l;
    else if (dreg == 0x6) {
        uint16_t addr = getHL();
        val = mmu->Read(addr);
    } else if (dreg == 0x7)
        val = accumulator;

    if(dval == 0x0)
        b = val;
    else if(dval == 0x1)
        c = val;
    else if(dval == 0x2)
        d = val;
    else if(dval == 0x3)
        e = val;
    else if(dval == 0x4)
        h = val;
    else if(dval == 0x5)
        l = val;
    else if(dval == 0x6) {
        uint16_t addr = getHL();
        mmu->Write(addr, val);
    } else if(dval == 0x7)
        accumulator = val;
}

void CentralProcessingUnit::instruction_Call(uint8_t* data) {
    bool condition;
    if (data[0] == 0xcd)
        condition = true;
    else if (data[0] == 0xc4)
        condition = (isZero == 0);
    else if (data[0] == 0xcc)
        condition = isZero;
    else if (data[0] == 0xd4)
        condition = (isCarry == 0);
    else if (data[0] == 0xdc)
        condition = isCarry;

    if (condition) {
        deltaTime = 24;
        stackPush(programCounter);

        uint16_t next = stitch(data[2], data[1]);
        programCounter = next;
    }
}

void CentralProcessingUnit::instruction_Push(uint8_t* data) {
    uint8_t hi, lo;
    if (data[0] == 0xc5)
        hi = b, lo = c;
    else if (data[0] == 0xd5)
        hi = d, lo = e;
    else if (data[0] == 0xe5)
        hi = h, lo = l;
    else if (data[0] == 0xf5)
        hi = (accumulator), lo = getFlags();

    stackPush(hi, lo);
}

void CentralProcessingUnit::instruction_Pop(uint8_t* data) {
    uint8_t *hi, *lo;
    if (data[0] == 0xc1)
        hi = &(b), lo = &(c);
    else if (data[0] == 0xd1)
        hi = &(d), lo = &(e);
    else if (data[0] == 0xe1)
        hi = &(h), lo = &(l);
    else if (data[0] == 0xf1) {
        accumulator = mmu->Read(stackPointer + 1);
        setFlags(mmu->Read(stackPointer));
        stackPointer += 2;
        return;
    }

    *lo = mmu->Read(stackPointer);
    *hi = mmu->Read(stackPointer + 1);
    stackPointer += 2;
}

void CentralProcessingUnit::instruction_Return(uint8_t* data) {
    bool condition = true;
    if (data[0] == 0xc0)
        condition = !(isZero);
    else if (data[0] == 0xc8)
        condition = isZero;
    else if (data[0] == 0xd0)
        condition = !(isCarry);
    else if (data[0] == 0xd8)
        condition = isCarry;

    if (condition) {
        programCounter = stackPop();
        if (data[0] == 0xc9)
            deltaTime = 16;
        else
            deltaTime = 20;
    }
}

void CentralProcessingUnit::instruction_Compare(uint8_t* data) {
    uint8_t n = 0;
    if (data[0] == 0xb8)
        n = b;
    else if (data[0] == 0xb9)
        n = c;
    else if (data[0] == 0xba)
        n = d;
    else if (data[0] == 0xbb)
        n = e;
    else if (data[0] == 0xbc)
        n = h;
    else if (data[0] == 0xbd)
        n = l;
    else if (data[0] == 0xbe)
        n = mmu->Read(getHL());
    else if (data[0] == 0xbf)
        n = accumulator;
    else if (data[0] == 0xfe)
        n = data[1];

    uint8_t reg = accumulator;
    uint8_t result = static_cast<uint8_t>(reg - n);

    isZero = (result == 0);
    isSubtract = true;
    isHalfCarry = (((reg & 0xf) - (n & 0xf)) < 0);
    isCarry = (reg < n);
}

void CentralProcessingUnit::instruction_LoadAnn(uint8_t* data) {
    uint16_t addr = 0xff00;
    if(data[0] == 0xf0)
        accumulator = mmu->Read(addr + data[1]);
    else if(data[0] == 0xf2)
        accumulator = mmu->Read(addr + c);
}

void CentralProcessingUnit::instruction_LoadSPHL(uint8_t* data) {
    stackPointer = getHL();
}

void CentralProcessingUnit::instruction_Sub(uint8_t* data) {
    uint8_t d;
    
    if (data[0] == 0x90)
        d = b;
    else if (data[0] == 0x91)
        d = c;
    else if (data[0] == 0x92)
        d = d;
    else if (data[0] == 0x93)
        d = e;
    else if (data[0] == 0x94)
        d = h;
    else if (data[0] == 0x95)
        d = l;
    else if (data[0] == 0x96) {
        uint16_t addr = getHL();
        d = mmu->Read(addr);
    } else if (data[0] == 0x97)
        d = accumulator;
    else if (data[0] == 0xd6)
        d = data[1];

    isZero = (accumulator == d);
    isSubtract = true;
    isCarry = (accumulator < d);
    isHalfCarry = ((accumulator & 0xf) - (d & 0xf)) < 0;
    accumulator = accumulator - d;
}

void CentralProcessingUnit::instruction_Add(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0x80)
        d = b;
    else if (data[0] == 0x81)
        d = c;
    else if (data[0] == 0x82)
        d = d;
    else if (data[0] == 0x83)
        d = e;
    else if (data[0] == 0x84)
        d = h;
    else if (data[0] == 0x85)
        d = l;
    else if (data[0] == 0x86) {
        uint16_t addr = getHL();
        d = mmu->Read(addr);
    } else if (data[0] == 0x87)
        d = accumulator;
    else if (data[0] == 0xc6)
        d = data[1];

    uint16_t val = accumulator + d;

    isZero = (val & 0xff) == 0;
    isCarry = (val & 0x100) != 0;
    isHalfCarry = (accumulator & 0xf) + (d & 0xf) > 0xf;
    isSubtract = false;

    accumulator = val;
}

void CentralProcessingUnit::instruction_Adc(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0x88)
        d = b;
    else if (data[0] == 0x89)
        d = c;
    else if (data[0] == 0x8a)
        d = d;
    else if (data[0] == 0x8b)
        d = e;
    else if (data[0] == 0x8c)
        d = h;
    else if (data[0] == 0x8d)
        d = l;
    else if (data[0] == 0x8e) {
        uint16_t addr = getHL();
        d = mmu->Read(addr);
    } else if (data[0] == 0x8f)
        d = accumulator;
    else if (data[0] == 0xce)
        d = data[1];

    uint8_t reg = accumulator;
    uint8_t carry = isCarry;

    uint result_full = reg + d + carry;
    uint8_t result = static_cast<uint8_t>(result_full);

    isZero = (result == 0);
    isSubtract = false;
    isHalfCarry = (((reg & 0xf) + (d & 0xf) + carry) > 0xf);
    isCarry = (result_full > 0xff);
    accumulator = result;
}

void CentralProcessingUnit::instruction_Jump(uint8_t* data) {
    bool condition = false;
    if (data[0] == 0xc3)
        condition = true;
    else if (data[0] == 0xc2)
        condition = !(isZero);
    else if (data[0] == 0xca)
        condition = (isZero);
    else if (data[0] == 0xd2)
        condition = !(isCarry);
    else if (data[0] == 0xda)
        condition = (isCarry);

    if (condition) {
        programCounter = stitch(data[2], data[1]);
        deltaTime = 16;
    }
}

void CentralProcessingUnit::instruction_SetInterrupt(uint8_t* data) {
    if (data[0] == 0xfb)
        interruptMasterFlag = true;
    else if (data[0] == 0xf3)
        interruptMasterFlag = false;
}

void CentralProcessingUnit::instruction_LoadAHL(uint8_t* data) {
    uint16_t addr = getHL();
    accumulator = mmu->Read(addr);
    if(data[0] == 0x2a)
        addr++;
    else if(data[0] == 0x3a)
        addr--;
    setHL(addr);
}

void CentralProcessingUnit::instruction_LoadHL(uint8_t* data) {
    int8_t value = static_cast<int8_t>(data[1]);
    int16_t result = stackPointer + value;
    isZero = false;
    isSubtract = false;
    isHalfCarry = (((stackPointer ^ value ^ (result & 0xFFFF)) & 0x10) == 0x10);
    isCarry = (((stackPointer ^ value ^ (result & 0xFFFF)) & 0x100) == 0x100);
    setHL(result);
}

void CentralProcessingUnit::instruction_OR(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0xb0)
        d = b;
    else if (data[0] == 0xb1)
        d = c;
    else if (data[0] == 0xb2)
        d = d;
    else if (data[0] == 0xb3)
        d = e;
    else if (data[0] == 0xb4)
        d = h;
    else if (data[0] == 0xb5)
        d = l;
    else if (data[0] == 0xb6) {
        uint16_t addr = getHL();
        d = mmu->Read(addr);
    } else if (data[0] == 0xb7)
        d = accumulator;
    else if (data[0] == 0xf6)
        d = data[1];

    uint8_t res = accumulator;
    res = res | d;
    accumulator = res;
    isZero = (res == 0);
    isCarry = isHalfCarry = isSubtract = false;
}

void CentralProcessingUnit::instruction_AND(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0xa0)
        d = b;
    else if (data[0] == 0xa1)
        d = c;
    else if (data[0] == 0xa2)
        d = d;
    else if (data[0] == 0xa3)
        d = e;
    else if (data[0] == 0xa4)
        d = h;
    else if (data[0] == 0xa5)
        d = l;
    else if (data[0] == 0xa6) {
        uint16_t addr = getHL();
        d = mmu->Read(addr);
    } else if (data[0] == 0xa7)
        d = accumulator;
    else if (data[0] == 0xe6)
        d = data[1];

    accumulator &= d;
    isZero = (accumulator == 0);
    isHalfCarry = true;
    isCarry = isSubtract = 0;
}

void CentralProcessingUnit::instruction_SRL(uint8_t* data) {
    uint8_t* reg;
    if (data[0] == 0x38)
        reg = &(b);
    else if (data[0] == 0x39)
        reg = &(c);
    else if (data[0] == 0x3a)
        reg = &(d);
    else if (data[0] == 0x3b)
        reg = &(e);
    else if (data[0] == 0x3c)
        reg = &(h);
    else if (data[0] == 0x3d)
        reg = &(l);
    else if (data[0] == 0x3e) {
        uint16_t addr = getHL();
        uint8_t val = mmu->Read(addr);
        isCarry = val & 1;
        val >>= 1;
        mmu->Write(addr, val);
        isZero = (val == 0);
        isHalfCarry = isSubtract = 0;
        return;
    } else if (data[0] == 0x3f)
        reg = &(accumulator);

    isHalfCarry = isSubtract = 0;
    uint8_t val = (*reg) >> 1;
    isZero = (val == 0);
    isCarry = (*reg) & 1;
    *reg = val;
}

void CentralProcessingUnit::instruction_Reti(uint8_t* data) {
    programCounter = stackPop();
    interruptMasterFlag = true;
    deltaTime = 16;
}

void CentralProcessingUnit::instruction_CPL(uint8_t* data) {
    accumulator = ~(accumulator);
    isSubtract = isHalfCarry = true;
}

void CentralProcessingUnit::instruction_Swap(uint8_t* data) {
    isSubtract = isHalfCarry = isCarry = false;
    uint8_t* reg;
    if (data[0] == 0x30)
        reg = &(b);
    else if (data[0] == 0x31)
        reg = &(c);
    else if (data[0] == 0x32)
        reg = &(d);
    else if (data[0] == 0x33)
        reg = &(e);
    else if (data[0] == 0x34)
        reg = &(h);
    else if (data[0] == 0x35)
        reg = &(l);
    else if (data[0] == 0x36) {
        uint16_t addr = getHL();
        uint8_t data = mmu->Read(addr);
        uint8_t val = (data >> 4);
        val |= (data << 4);
        mmu->Write(addr, val);
        isZero = (val == 0);
        return;
    } else if (data[0] == 0x37)
        reg = &(accumulator);

    uint8_t val = (*reg) >> 4;
    val |= ((*reg) << 4);
    *reg = val;
    isZero = (val == 0);
}

void CentralProcessingUnit::instruction_Reset(uint8_t* data) {
    deltaTime = 16;
    stackPush(programCounter);

    if (data[0] == 0xc7)
        programCounter = 0;
    else if (data[0] == 0xcf)
        programCounter = 0x08;
    else if (data[0] == 0xd7)
        programCounter = 0x10;
    else if (data[0] == 0xdf)
        programCounter = 0x18;
    else if (data[0] == 0xe7)
        programCounter = 0x20;
    else if (data[0] == 0xef)
        programCounter = 0x28;
    else if (data[0] == 0xf7)
        programCounter = 0x30;
    else if (data[0] == 0xff)
        programCounter = 0x38;
}

void CentralProcessingUnit::instruction_AddPair(uint8_t* data) {
    uint8_t hi, lo;
    if (data[0] == 0x09)
        hi = b, lo = c;
    else if (data[0] == 0x19)
        hi = d, lo = e;
    else if (data[0] == 0x29)
        hi = h, lo = l;
    else if (data[0] == 0x39)
        hi = (stackPointer) >> 8, lo = (stackPointer) & 0xff;

    uint16_t reg = getHL();
    uint16_t val = stitch(hi, lo);
    uint result = reg + val;

    isSubtract = false;
    isHalfCarry = ((reg & 0xfff) + (val & 0xfff) > 0xfff);
    isCarry = ((result & 0x10000) != 0);
    setHL(result);
}

void CentralProcessingUnit::instruction_JumpHL(uint8_t* data) {
    uint16_t addr = getHL();
    programCounter = addr;
}

void CentralProcessingUnit::instruction_ResetBit(uint8_t* data) {
    uint8_t dreg = (data[0] & 0xf) % 8;
    uint8_t dbit = (data[0] - 0x80) / 8;
    uint8_t* reg;
    if (dreg == 0x0)
        reg = &(b);
    else if (dreg == 0x1)
        reg = &(c);
    else if (dreg == 0x2)
        reg = &(d);
    else if (dreg == 0x3)
        reg = &(e);
    else if (dreg == 0x4)
        reg = &(h);
    else if (dreg == 0x5)
        reg = &(l);
    else if (dreg == 0x6) {
        uint16_t addr = getHL();
        uint8_t val = mmu->Read(addr);
        val &= ~(1 << dbit);
        mmu->Write(addr, val);
        return;
    } else if (dreg == 0x7)
        reg = &(accumulator);

    (*reg) &= ~(1 << dbit);
}

void CentralProcessingUnit::instruction_SetBit(uint8_t* data) {
    uint8_t dreg = (data[0] & 0xf) % 8;
    uint8_t dbit = (data[0] - 0xc0) / 8;
    uint8_t* reg;
    if (dreg == 0x0)
        reg = &(b);
    else if (dreg == 0x1)
        reg = &(c);
    else if (dreg == 0x2)
        reg = &(d);
    else if (dreg == 0x3)
        reg = &(e);
    else if (dreg == 0x4)
        reg = &(h);
    else if (dreg == 0x5)
        reg = &(l);
    else if (dreg == 0x6) {
        uint16_t addr = getHL();
        uint8_t val = mmu->Read(addr);
        val |= (1 << dbit);
        mmu->Write(addr, val);
        return;
    } else if (dreg == 0x7)
        reg = &(accumulator);

    (*reg) |= (1 << dbit);
}

void CentralProcessingUnit::instruction_SP2Mem(uint8_t* data) {
    uint16_t addr = stitch(data[2], data[1]);
    mmu->Write(addr, stackPointer);
}

void CentralProcessingUnit::instruction_CheckBit(uint8_t* data) {
    uint8_t dreg = (data[0] & 0xf) % 8;
    uint8_t dbit = (data[0] - 0x40) / 8;

    uint8_t val;
    if (dreg == 0x0)
        val = b;
    else if (dreg == 0x1)
        val = c;
    else if (dreg == 0x2)
        val = d;
    else if (dreg == 0x3)
        val = e;
    else if (dreg == 0x4)
        val = h;
    else if (dreg == 0x5)
        val = l;
    else if (dreg == 0x6) {
        uint16_t addr = getHL();
        val = mmu->Read(addr);
    } else if (dreg == 0x7)
        val = accumulator;

    isZero = (val & (1 << dbit)) == 0;
    isHalfCarry = 1;
    isSubtract = 0;
}

void CentralProcessingUnit::instruction_RollLeftA(uint8_t* data) {
    instruction_RollLeft(data);
    isZero = false;
}

void CentralProcessingUnit::instruction_RollLeft(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x10)
        reg = &(b);
    else if (data[0] == 0x11)
        reg = &(c);
    else if (data[0] == 0x12)
        reg = &(d);
    else if (data[0] == 0x13)
        reg = &(e);
    else if (data[0] == 0x14)
        reg = &(h);
    else if (data[0] == 0x15)
        reg = &(l);
    else if (data[0] == 0x16) {
        uint16_t addr = getHL();
        uint8_t val = mmu->Read(addr);

        int temp = isCarry;
        isCarry = (val >> 7) & 1;
        val = val << 1 | temp;
        isZero = (val == 0);
        isHalfCarry = false;
        isSubtract = false;

        mmu->Write(addr, val);
        return;
    } else if (data[0] == 0x17)
        reg = &(accumulator);

    int temp = isCarry;
    isCarry = check_bit((*reg), 7);
    *reg = (*reg) << 1 | temp;
    isZero = (*reg) == 0;
    isHalfCarry = false;
    isSubtract = false;
}

void CentralProcessingUnit::instruction_RollRightA(uint8_t* data) {
    instruction_RollRight(data);
    isZero = false;
}

void CentralProcessingUnit::instruction_RollRight(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x18)
        reg = &(b);
    else if (data[0] == 0x19)
        reg = &(c);
    else if (data[0] == 0x1a)
        reg = &(d);
    else if (data[0] == 0x1b)
        reg = &(e);
    else if (data[0] == 0x1c)
        reg = &(h);
    else if (data[0] == 0x1d)
        reg = &(l);
    else if (data[0] == 0x1e) {
        uint16_t addr = getHL();
        uint8_t val = mmu->Read(addr);

        int temp = isCarry;
        isCarry = check_bit(val, 0);
        val = val >> 1 | (temp << 7);
        isZero = (val == 0);
        isHalfCarry = false;
        isSubtract = false;

        mmu->Write(addr, val);
        return;
    } else if (data[0] == 0x1f)
        reg = &(accumulator);

    int temp = isCarry;
    isCarry = check_bit((*reg), 0);
    *reg = (*reg) >> 1 | (temp << 7);
    isZero = (*reg) == 0;
    isHalfCarry = false;
    isSubtract = false;
}

void CentralProcessingUnit::instruction_RollLeftCarryA(uint8_t* data) {
    instruction_RollLeftCarry(data);
    isZero = false;
}

void CentralProcessingUnit::instruction_RollLeftCarry(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x00)
        reg = &(b);
    else if (data[0] == 0x01)
        reg = &(c);
    else if (data[0] == 0x02)
        reg = &(d);
    else if (data[0] == 0x03)
        reg = &(e);
    else if (data[0] == 0x04)
        reg = &(h);
    else if (data[0] == 0x05)
        reg = &(l);
    else if (data[0] == 0x06) {
        uint16_t addr = getHL();
        uint8_t val = mmu->Read(addr);

        uint8_t carry_flag = check_bit(val, 7);
        uint8_t truncated_bit = check_bit(val, 7);
        val = static_cast<uint8_t>((val << 1) | truncated_bit);

        isCarry = carry_flag;
        isZero = val == 0;
        isHalfCarry = false;
        isSubtract = false;
        mmu->Write(addr, val);
        return;
    } else if (data[0] == 0x07)
        reg = &(accumulator);

    uint8_t carry_flag = check_bit((*reg), 7);
    uint8_t truncated_bit = check_bit((*reg), 7);
    (*reg) = static_cast<uint8_t>(((*reg) << 1) | truncated_bit);

    isCarry = carry_flag;
    isZero = (*reg) == 0;
    isHalfCarry = false;
    isSubtract = false;
}

void CentralProcessingUnit::instruction_RollRightCarryA(uint8_t* data) {
    instruction_RollRightCarry(data);
    isZero = false;
}

void CentralProcessingUnit::instruction_RollRightCarry(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x08)
        reg = &(b);
    else if (data[0] == 0x09)
        reg = &(c);
    else if (data[0] == 0x0a)
        reg = &(d);
    else if (data[0] == 0x0b)
        reg = &(e);
    else if (data[0] == 0x0c)
        reg = &(h);
    else if (data[0] == 0x0d)
        reg = &(l);
    else if (data[0] == 0x0e) {
        uint16_t addr = getHL();
        uint8_t val = mmu->Read(addr);

        uint8_t carry_flag = check_bit(val, 0);
        uint8_t truncated_bit = check_bit(val, 0);
        val = static_cast<uint8_t>((val >> 1) | (truncated_bit << 7));

        isCarry = carry_flag;
        isZero = (val == 0);
        isHalfCarry = false;
        isSubtract = false;
        mmu->Write(addr, val);
        return;
    } else if (data[0] == 0x0f)
        reg = &(accumulator);

    uint8_t carry_flag = check_bit(*reg, 0);
    uint8_t truncated_bit = check_bit(*reg, 0);
    *reg = static_cast<uint8_t>(((*reg) >> 1) | (truncated_bit << 7));

    isCarry = carry_flag;
    isZero = (*reg) == 0;
    isHalfCarry = false;
    isSubtract = false;
}

void CentralProcessingUnit::instruction_DAA(uint8_t* data) {
    uint8_t reg = accumulator;
    uint16_t correction = isCarry ? 0x60 : 0x00;

    if (isHalfCarry || (!isSubtract && ((reg & 0x0F) > 9)))
        correction |= 0x06;

    if (isCarry || (!isSubtract && (reg > 0x99)))
        correction |= 0x60;

    if (isSubtract)
        reg = static_cast<uint8_t>(reg - correction);
    else
        reg = static_cast<uint8_t>(reg + correction);

    if (((correction << 2) & 0x100) != 0)
        isCarry = true;

    isHalfCarry = false;
    isZero = (reg == 0);
    accumulator = static_cast<uint8_t>(reg);
}

void CentralProcessingUnit::instruction_SCF(uint8_t* data) {
    isCarry = true;
    isHalfCarry = isSubtract = false;
}

void CentralProcessingUnit::instruction_CCF(uint8_t* data) {
    isCarry = !isCarry;
    isHalfCarry = isSubtract = false;
}

void CentralProcessingUnit::instruction_SBC(uint8_t* data) {
    uint8_t d;
    if (data[0] == 0x98)
        d = b;
    else if (data[0] == 0x99)
        d = c;
    else if (data[0] == 0x9a)
        d = d;
    else if (data[0] == 0x9b)
        d = e;
    else if (data[0] == 0x9c)
        d = h;
    else if (data[0] == 0x9d)
        d = l;
    else if (data[0] == 0x9e) {
        uint16_t addr = getHL();
        d = mmu->Read(addr);
    } else if (data[0] == 0x9f)
        d = accumulator;
    else if (data[0] == 0xde)
        d = data[1];

    uint8_t carry = isCarry;
    int result_full = (int)accumulator - d - carry;
    uint8_t result = static_cast<uint8_t>(result_full);

    isZero = (result == 0);
    isSubtract = true;
    isCarry = (result_full < 0);
    isHalfCarry = (((accumulator & 0xf) - (d & 0xf) - carry) < 0);
    accumulator = result;
}

void CentralProcessingUnit::instruction_SLA(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x20)
        reg = &b;
    else if (data[0] == 0x21)
        reg = &c;
    else if (data[0] == 0x22)
        reg = &d;
    else if (data[0] == 0x23)
        reg = &e;
    else if (data[0] == 0x24)
        reg = &h;
    else if (data[0] == 0x25)
        reg = &l;
    else if (data[0] == 0x26) {
        uint16_t addr = getHL();
        uint8_t d = mmu->Read(addr);
        uint8_t carry_bit = check_bit(d, 7);
        uint8_t result = static_cast<uint8_t>(d << 1);
        isZero = (result == 0);
        isSubtract = false;
        isHalfCarry = false;
        isCarry = carry_bit;
        mmu->Write(addr, result);
        return;
    } else if (data[0] == 0x27)
        reg = &accumulator;

    uint8_t carry_bit = check_bit(*reg, 7);
    uint8_t result = static_cast<uint8_t>(*reg << 1);
    isZero = (result == 0);
    isSubtract = false;
    isHalfCarry = false;
    isCarry = carry_bit;
    *reg = result;
}

void CentralProcessingUnit::instruction_SRA(uint8_t* data) {
    uint8_t *reg;
    if (data[0] == 0x28)
        reg = &b;
    else if (data[0] == 0x29)
        reg = &c;
    else if (data[0] == 0x2a)
        reg = &d;
    else if (data[0] == 0x2b)
        reg = &e;
    else if (data[0] == 0x2c)
        reg = &h;
    else if (data[0] == 0x2d)
        reg = &l;
    else if (data[0] == 0x2e) {
        uint16_t addr = getHL();
        uint8_t d = mmu->Read(addr);
        uint8_t carry_bit = check_bit(d, 0);
        uint8_t top_bit = check_bit(d, 7);
        uint8_t result = static_cast<uint8_t>(d >> 1);
        if(top_bit)
            result |= (1 << 7);
        else
            result &= ~(1 << 7);
        isZero = (result == 0);
        isSubtract = false;
        isHalfCarry = false;
        isCarry = carry_bit;
        mmu->Write(addr, result);
        return;
    } else if (data[0] == 0x2f)
        reg = &accumulator;

    uint8_t carry_bit = check_bit(*reg, 0);
    uint8_t top_bit = check_bit(*reg, 7);
    uint8_t result = static_cast<uint8_t>((*reg) >> 1);
    if(top_bit)
        result |= (1 << 7);
    else
        result &= ~(1 << 7);

    isZero = (result == 0);
    isSubtract = false;
    isHalfCarry = false;
    isCarry = carry_bit;
    *reg = result;
}

void CentralProcessingUnit::instruction_Halt(uint8_t* data) {
    isHalted = true;
}