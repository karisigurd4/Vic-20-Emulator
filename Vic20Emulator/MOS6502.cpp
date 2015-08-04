#include "MOS6502.h"

// Constructors
MOS6502::MOS6502() : IC(), halt(false) {
	// Set a default clock speed, the default is PAL Vic-20 clock.
	this->setClockSpeed(1108404);
}

MOS6502::~MOS6502() {

}

void MOS6502::initialize() {
	this->AC.write(0);
	this->X.write(0);
	this->Y.write(0);
	this->PC.write(this->memory->readDWord(this->rstVector)); 
	this->SP.write(0xFF);
	this->SR.write(this->decode);
	this->halt = false;
	this->cycles = 0;
}

// Getters
uint64_t MOS6502::getPc() {
	return this->PC.read();
}

uint64_t MOS6502::getInstructions(){
	return this->instructions;
}

bool MOS6502::isHalting() {
	return this->halt;
}

// Executes a single instruction
void MOS6502::executeInstruction() {
	if (!halt) {
		// Get current opcode
		uint8_t opcode = this->memory->readWord(this->PC.read());

		// Increment instruction counter
		instructions++;

		// Decode opcode
		switch (opcode & 0xF0) {
		case 0x00:
			switch (opcode & 0x0F) {
			case 0x00: this->brk(); break;
			case 0x01: this->ora(accessMode::IndirectX); break;
			case 0x05: this->ora(accessMode::ZeroPage); break;
			case 0x06: this->asl(accessMode::ZeroPage); break;
			case 0x08: this->php(); break;
			case 0x09: this->ora(accessMode::Immediate); break;
			case 0x0A: this->asl(accessMode::Accumulator); break;
			case 0x0D: this->ora(accessMode::Absolute); break;
			case 0x0E: this->asl(accessMode::Absolute); break;
			default: this->halt = true;
			}
			break;
		case 0x10:
			switch (opcode & 0x0F) {
			case 0x00: this->bpl(); break;
			case 0x01: this->ora(accessMode::IndirectY); break;
			case 0x05: this->ora(accessMode::ZeroPageX); break;
			case 0x06: this->asl(accessMode::ZeroPageX); break;
			case 0x08: this->clc(); break;
			case 0x09: this->ora(accessMode::AbsoluteY); break;
			case 0x0D: this->ora(accessMode::AbsoluteX); break;
			case 0x0E: this->asl(accessMode::AbsoluteX); break;
			default: this->halt = true;
			}
			break;
		case 0x20:
			switch (opcode & 0x0F) {
			case 0x00: this->jsr(); break;
			case 0x01: this->and(accessMode::IndirectX); break;
			case 0x04: this->bit(accessMode::ZeroPage); break;
			case 0x05: this->and(accessMode::ZeroPage); break;
			case 0x06: this->rol(accessMode::ZeroPage); break;
			case 0x08: this->plp(); break;
			case 0x09: this->and(accessMode::Immediate); break;
			case 0x0A: this->rol(accessMode::Accumulator); break;
			case 0x0C: this->bit(accessMode::Absolute); break;
			case 0x0D: this->and(accessMode::Absolute); break;
			case 0x0E: this->rol(accessMode::Absolute); break;
			default: this->halt = true;
			}
			break;
		case 0x30:
			switch (opcode & 0x0F) {
			case 0x00: this->bmi(); break;
			case 0x01: this->and(accessMode::IndirectY); break;
			case 0x05: this->and(accessMode::ZeroPageX); break;
			case 0x06: this->rol(accessMode::ZeroPageX); break;
			case 0x08: this->sec(); break;
			case 0x09: this->and(accessMode::AbsoluteY); break;
			case 0x0D: this->and(accessMode::AbsoluteX); break;
			case 0x0E: this->rol(accessMode::AbsoluteX); break;
			default: this->halt = true;
			}
			break;
		case 0x40:
			switch (opcode & 0x0F) {
			case 0x00: this->rti(); break;
			case 0x01: this->eor(accessMode::IndirectX); break;
			case 0x05: this->eor(accessMode::ZeroPage); break;
			case 0x06: this->lsr(accessMode::ZeroPage); break;
			case 0x08: this->pha(); break;
			case 0x09: this->eor(accessMode::Immediate); break;
			case 0x0A: this->lsr(accessMode::Accumulator); break;
			case 0x0C: this->jmp(accessMode::Absolute); break;
			case 0x0D: this->eor(accessMode::Absolute); break;
			case 0x0E: this->lsr(accessMode::Absolute); break;
			default: this->halt = true;
			}
			break;
		case 0x50:
			switch (opcode & 0x0F) {
			case 0x00: this->bvc(); break;
			case 0x01: this->eor(accessMode::IndirectY); break;
			case 0x05: this->eor(accessMode::ZeroPageX); break;
			case 0x06: this->lsr(accessMode::ZeroPageX); break;
			case 0x08: this->cli(); break;
			case 0x09: this->eor(accessMode::AbsoluteY); break;
			case 0x0D: this->eor(accessMode::AbsoluteX); break;
			case 0x0E: this->lsr(accessMode::AbsoluteX); break;
			default: this->halt = true;
			}
			break;
		case 0x60:
			switch (opcode & 0x0F) {
			case 0x00: this->rts(); break;
			case 0x01: this->adc(accessMode::IndirectX); break;
			case 0x05: this->adc(accessMode::ZeroPage); break;
			case 0x06: this->ror(accessMode::ZeroPage); break;
			case 0x08: this->pla(); break;
			case 0x09: this->adc(accessMode::Immediate); break;
			case 0x0A: this->ror(accessMode::Accumulator); break;
			case 0x0C: this->jmp(accessMode::Indirect); break;
			case 0x0D: this->adc(accessMode::Absolute); break;
			case 0x0E: this->ror(accessMode::Absolute); break;
			default: this->halt = true;
			}
			break;
		case 0x70:
			switch (opcode & 0x0F) {
			case 0x00: this->bvs(); break;
			case 0x01: this->adc(accessMode::IndirectY); break;
			case 0x05: this->adc(accessMode::ZeroPageX); break;
			case 0x06: this->ror(accessMode::ZeroPageX); break;
			case 0x08: this->sei(); break;
			case 0x09: this->adc(accessMode::AbsoluteY); break;
			case 0x0D: this->adc(accessMode::AbsoluteX); break;
			case 0x0E: this->ror(accessMode::AbsoluteX); break;
			default: this->halt = true;
			}
			break;
		case 0x80:
			switch (opcode & 0x0F) {
			case 0x01: this->sta(accessMode::IndirectX); break;
			case 0x04: this->sty(accessMode::ZeroPage); break;
			case 0x05: this->sta(accessMode::ZeroPage); break;
			case 0x06: this->stx(accessMode::ZeroPage); break;
			case 0x08: this->dey(); break;
			case 0x0A: this->txa(); break;
			case 0x0C: this->sty(accessMode::Absolute); break;
			case 0x0D: this->sta(accessMode::Absolute); break;
			case 0x0E: this->stx(accessMode::Absolute); break;
			default: this->halt = true;
			}
			break;
		case 0x90:
			switch (opcode & 0x0F) {
			case 0x00: this->bcc(); break;
			case 0x01: this->sta(accessMode::IndirectY); break;
			case 0x04: this->sty(accessMode::ZeroPageX); break;
			case 0x05: this->sta(accessMode::ZeroPageX); break;
			case 0x06: this->stx(accessMode::ZeroPageY); break;
			case 0x08: this->tya(); break;
			case 0x09: this->sta(accessMode::AbsoluteY); break;
			case 0x0A: this->txs(); break;
			case 0x0D: this->sta(accessMode::AbsoluteX); break;
			default: this->halt = true;
			}
			break;
		case 0xA0:
			switch (opcode & 0x0F) {
			case 0x00: this->ldy(accessMode::Immediate); break;
			case 0x01: this->lda(accessMode::IndirectX); break;
			case 0x02: this->ldx(accessMode::Immediate); break;
			case 0x04: this->ldy(accessMode::ZeroPage); break;
			case 0x05: this->lda(accessMode::ZeroPage); break;
			case 0x06: this->ldx(accessMode::ZeroPage); break;
			case 0x08: this->tay(); break;
			case 0x09: this->lda(accessMode::Immediate); break;
			case 0x0A: this->tax(); break;
			case 0x0C: this->ldy(accessMode::Absolute); break;
			case 0x0D: this->lda(accessMode::Absolute); break;
			case 0x0E: this->ldx(accessMode::Absolute); break;
			default: this->halt = true;
			}
			break;
		case 0xB0:
			switch (opcode & 0x0F) {
			case 0x00: this->bcs(); break;
			case 0x01: this->lda(accessMode::IndirectY); break;
			case 0x04: this->ldy(accessMode::ZeroPageX); break;
			case 0x05: this->lda(accessMode::ZeroPageX); break;
			case 0x06: this->ldx(accessMode::ZeroPageY); break;
			case 0x08: this->clv(); break;
			case 0x09: this->lda(accessMode::AbsoluteY); break;
			case 0x0A: this->tsx(); break;
			case 0x0C: this->ldy(accessMode::AbsoluteX); break;
			case 0x0D: this->lda(accessMode::AbsoluteX); break;
			case 0x0E: this->ldx(accessMode::AbsoluteY); break;
			default: this->halt = true;
			}
			break;
		case 0xC0:
			switch (opcode & 0x0F) {
			case 0x00: this->cpy(accessMode::Immediate); break;
			case 0x01: this->cmp(accessMode::IndirectX); break;
			case 0x04: this->cpy(accessMode::ZeroPage); break;
			case 0x05: this->cmp(accessMode::ZeroPage); break;
			case 0x06: this->dec(accessMode::ZeroPage); break;
			case 0x08: this->iny(); break;
			case 0x09: this->cmp(accessMode::Immediate); break;
			case 0x0A: this->dex(); break;
			case 0x0C: this->cpy(accessMode::Absolute); break;
			case 0x0D: this->cmp(accessMode::Absolute); break;
			case 0x0E: this->dec(accessMode::Absolute); break;
			default: this->halt = true;
			}
			break;
		case 0xD0:
			switch (opcode & 0x0F) {
			case 0x00: this->bne(); break;
			case 0x01: this->cmp(accessMode::IndirectY); break;
			case 0x05: this->cmp(accessMode::ZeroPageX); break;
			case 0x06: this->dec(accessMode::ZeroPageX); break;
			case 0x08: this->cld(); break;
			case 0x09: this->cmp(accessMode::AbsoluteY); break;
			case 0x0D: this->cmp(accessMode::AbsoluteX); break;
			case 0x0E: this->dec(accessMode::AbsoluteX); break;
			default: this->halt = true;
			}
			break;
		case 0xE0:
			switch (opcode & 0x0F) {
			case 0x00: this->cpx(accessMode::Immediate); break;
			case 0x01: this->sbc(accessMode::IndirectX); break;
			case 0x04: this->cpx(accessMode::ZeroPage); break;
			case 0x05: this->sbc(accessMode::ZeroPage); break;
			case 0x06: this->inc(accessMode::ZeroPage); break;
			case 0x08: this->inx(); break;
			case 0x09: this->sbc(accessMode::Immediate); break;
			case 0x0A: this->nop(); break;
			case 0x0C: this->cpx(accessMode::Absolute); break;
			case 0x0D: this->sbc(accessMode::Absolute); break;
			case 0x0E: this->inc(accessMode::Absolute); break;
			default: this->halt = true;
			}
			break;
		case 0xF0:
			switch (opcode & 0x0F) {
			case 0x00: this->beq(); break;
			case 0x01: this->sbc(accessMode::IndirectY); break;
			case 0x05: this->sbc(accessMode::ZeroPageX); break;
			case 0x06: this->inc(accessMode::ZeroPageX); break;
			case 0x08: this->sed(); break;
			case 0x09: this->sbc(accessMode::AbsoluteY); break;
			case 0x0D: this->sbc(accessMode::AbsoluteX); break;
			case 0x0E: this->inc(accessMode::AbsoluteX); break;
			default: this->halt = true;
			}
			break;
		}
	}
}

void MOS6502::setMemory(MOS6502Memory *memory) {
	this->memory = memory;
}

/* Causes an interrupt */
bool MOS6502::interrupt() {
	// If interrupt disable bit is set, don't do the interrupt
	if (!this->SR.isBitSet(status::Interrupt)) {
		// Push PC and SR to stack
		this->pushDWord(this->PC.read());
		this->pushWord(this->SR.read());		

		// Sets the disable interrupt bit
		this->SR.setBit(status::Interrupt);

		// Clear break flag
		this->SR.clearBit(status::Break);

		// Execute instruction
		this->PC.write(this->memory->readDWord(this->irqVector));

		// Return true, interrupt allowed
		return true;
	}
	else {
		// Return false, interrupts are not allowed
		return false;
	}
}

void MOS6502::nonMaskableInterrupt() {
	// Push PC and SR to stack
	this->pushDWord(this->PC.read());
	this->pushWord(this->SR.read());

	// Sets the disable interrupt bit
	this->SR.setBit(status::Interrupt);

	// Clear break flag
	this->SR.clearBit(status::Break);

	// Execute instruction
	this->PC.write(this->memory->readDWord(this->nmiVector));
}

// MOS 6502 instruction set
/*
ADC  Add Memory to Accumulator with Carry

A + M + C -> A, C                
N Z C I D V
+ + + - - +

addressing    assembler    opc  bytes  cyles
--------------------------------------------
immidiate     ADC #oper     69    2     2
zeropage      ADC oper      65    2     3
zeropage,X    ADC oper,X    75    2     4
absolute      ADC oper      6D    3     4
absolute,X    ADC oper,X    7D    3     4*
absolute,Y    ADC oper,Y    79    3     4*
(indirect,X)  ADC (oper,X)  61    2     6
(indirect),Y  ADC (oper),Y  71    2     5*
*/
void MOS6502::adc(accessMode mode) { 
	switch (mode) {
	case accessMode::Immediate:
		// Execute instruction
		this->executeAdc(this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
		break;
	case accessMode::ZeroPage:
		// Execute instruction
		this->executeAdc(this->getZeroPageValue(0));

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageX:
     	// Execute instruction
		this->executeAdc(this->getZeroPageValue(this->X.read()));

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		this->executeAdc(this->getAbsoluteValue(0));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteX:
		// Execute instruction
		this->executeAdc(this->getAbsoluteValue(this->X.read()));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteY:
		// Execute instruction
		this->executeAdc(this->getAbsoluteValue(this->Y.read()));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::IndirectX:
		// Execute instruction
		this->executeAdc(this->getXIndexedIndirectValue());

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::IndirectY:
		// Execute instruction
		this->executeAdc(this->getYIndirectIndexedValue());

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	}
}

/*
AND  AND Memory with Accumulator

A AND M -> A    

N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
immidiate     AND #oper     29    2     2
zeropage      AND oper      25    2     3
zeropage,X    AND oper,X    35    2     4
absolute      AND oper      2D    3     4
absolute,X    AND oper,X    3D    3     4*
absolute,Y    AND oper,Y    39    3     4*
(indirect,X)  AND (oper,X)  21    2     6
(indirect),Y  AND (oper),Y  31    2     5*
*/
void MOS6502::and(accessMode mode) { 
	switch (mode) {
	case accessMode::Immediate:
		// Execute instruction
		this->AC.write(this->AC.read() & this->nextWord());

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 2);
		break;
	case accessMode::ZeroPage:
		// Execute instruction
		this->AC.write(this->AC.read() & this->getZeroPageValue(0));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageX:
		// Execute instruction
		this->AC.write(this->AC.read() & this->getZeroPageValue(this->X.read()));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		this->AC.write(this->AC.read() & this->getAbsoluteValue(0));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteX:
		// Execute instruction
		this->AC.write(this->AC.read() & this->getAbsoluteValue(this->X.read()));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteY:
		// Execute instruction
		this->AC.write(this->AC.read() & this->getAbsoluteValue(this->Y.read()));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::IndirectX:
		// Execute instruction
		this->AC.write(this->AC.read() & getXIndexedIndirectValue());

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::IndirectY:
		// Execute instruction
		this->AC.write(this->AC.read() & getYIndirectIndexedValue());

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	}
}

/*
ASL  Shift Left One Bit (Memory or Accumulator)

C <- [76543210] <- 0            
N Z C I D V
+ + + - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
accumulator   ASL A         0A    1     2
zeropage      ASL oper      06    2     5
zeropage,X    ASL oper,X    16    2     6
absolute      ASL oper      0E    3     6
absolute,X    ASL oper,X    1E    3     7
*/
void MOS6502::asl(accessMode mode) { 
	switch (mode) {
	case accessMode::Accumulator:
		// Set carry
		this->statusCheck(status::Carry, this->AC.read() & 0x80);

		// Execute instruction
		this->AC.write((this->AC.read() << 1));

		// Set processor status
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(1, 2);
		break;
	case accessMode::ZeroPage: 
		// Set carry
		this->statusCheck(status::Carry, this->getZeroPageValue(0) & 0x80);

		// Execute instruction
		this->setZeroPageValue((this->getZeroPageValue(0) << 1), 0);

		// Set processor status
		this->negative(this->getZeroPageValue(0));
		this->zero(this->getZeroPageValue(0));

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	case accessMode::ZeroPageX:
		// Set carry
		this->statusCheck(status::Carry, this->getZeroPageValue(this->X.read()) & 0x80);

		// Execute instruction
		this->setZeroPageValue((this->getZeroPageValue(this->X.read()) << 1), this->X.read());

		this->negative(this->getZeroPageValue(this->X.read()));
		this->zero(this->getZeroPageValue(this->X.read()));

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::Absolute:
		// Set carry
		this->statusCheck(status::Carry, this->getAbsoluteValue(0) & 0x80);

		// Execute instruction
		this->setAbsoluteValue((this->getAbsoluteValue(0) << 1), 0);

		// Set processor status
		this->negative(this->getAbsoluteValue(0));
		this->zero(this->getAbsoluteValue(0));

		// Increment program counter and cycles
		this->step(3, 6);
		break;
	case accessMode::AbsoluteX:
		// Set carry
		this->statusCheck(status::Carry, this->getAbsoluteValue(this->X.read()) & 0x80);

		// Execute instruction
		this->setAbsoluteValue((this->getAbsoluteValue(this->X.read()) << 1), this->X.read());

		this->negative(this->getAbsoluteValue(this->X.read()));
		this->zero(this->getAbsoluteValue(this->X.read()));

		// Increment program counter and cycles
		this->step(3, 7);
		break;
	}
}

/*
BCC  Branch on Carry Clear

branch on C = 0                  
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
relative      BCC oper      90    2     2**
*/
void MOS6502::bcc() { 
	// Execute instruction
	if (!this->SR.isBitSet(this->status::Carry)) {
		// Branches are relative, signed jump
		this->PC.write(PC.read() + (int8_t) this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
	} else 
		// Increment program counter and cycles
		this->step(2, 2);
}

/*
BCS  Branch on Carry Set

branch on C = 1                  
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
relative      BCS oper      B0    2     2**
*/
void MOS6502::bcs() { 
	// Execute instruction
	if (this->SR.isBitSet(this->status::Carry)) {
		// Branches are relative, signed jump
		this->PC.write(PC.read() + (int8_t) this->nextWord());
		
		// Increment program counter and cycles
		this->step(2, 2);
	}
	else
		// Increment program counter and cycles
		this->step(2, 2);
}

/*
BEQ  Branch on Result Zero

branch on Z = 1                  
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
relative      BEQ oper      F0    2     2**

*/
void MOS6502::beq() { 
	// Execute instruction
	if (this->SR.isBitSet(status::Zero)) {
		// Branches are relative, signed jump
		this->PC.write(PC.read() + (int8_t) this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
	}
	else
		// Increment program counter and cycles
		this->step(2, 2);
}

/*
BIT  Test Bits in Memory with Accumulator

bits 7 and 6 of operand are transfered to bit 7 and 6 of SR (N,V);
the zeroflag is set to the result of operand AND accumulator.

A AND M, M7 -> N, M6 -> V        
N Z C I D V
M7 + - - - M6

addressing    assembler    opc  bytes  cyles
--------------------------------------------
zeropage      BIT oper      24    2     3
absolute      BIT oper      2C    3     4
*/
void MOS6502::bit(accessMode mode) { 
	uint16_t value = 0;
	uint16_t andA = 0;

	switch (mode) {
	case accessMode::ZeroPage:
		// Store temporary register value
		value = this->getZeroPageValue(0);

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::Absolute: 
		// Store temporary register value
		value = this->getAbsoluteValue(0);

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	}

	// Perform bit tests
	andA = value & this->AC.read();
	this->statusCheck(status::Negative, andA & 0x80);
	this->statusCheck(status::Overflow, andA & 0x40);
	this->SR.write((this->SR.read() & 0x3F) | (uint8_t)(value & 0xC0));
	this->statusCheck(status::Zero, !andA);
}

/*
BMI  Branch on Result Minus

branch on N = 1                  
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
relative      BMI oper      30    2     2**
*/
void MOS6502::bmi() { 
	// Execute instruction
	if (this->SR.isBitSet(status::Negative)) {
		// Branches are relative, signed jump
		this->PC.write(PC.read() + (int8_t) this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
	} else
		// Increment program counter and cycles
		this->step(2, 2);
}

/*
BNE  Branch on Result not Zero

branch on Z = 0
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
relative      BNE oper      D0    2     2**
*/
void MOS6502::bne() { 
	// Execute instruction
	if (!this->SR.isBitSet(this->status::Zero)) {
		// Branches are relative, signed jump
		this->PC.write(PC.read() + (int8_t) this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
	}
	else
		// Increment program counter and cycles
		this->step(2, 2);
}

/*
BPL  Branch on Result Plus

branch on N = 0                 
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
relative      BPL oper      10    2     2**
*/
void MOS6502::bpl() { 
	// Execute instruction
	if (!this->SR.isBitSet(this->status::Negative)) {
		// Branches are relative, signed jump
		this->PC.write(PC.read() + (int8_t) this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
	}
	else
		// Increment program counter and cycles
		this->step(2, 2);
}

/*
BRK  Force Break

interrupt,                       N Z C I D V
push PC+2, push SR               - - - 1 - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       BRK           00    1     7
*/
void MOS6502::brk() { 
	// Push 
	this->pushDWord(this->PC.read() + 1);
	this->pushWord(this->SR.read() | 0x10);

	// Set interrupt flag
	this->statusCheck(status::Interrupt, true);

	// Execute instruction
	this->PC.write(this->memory->readDWord(irqVector));

	// Increment program counter and cycles
	this->step(1, 7);
}

/*
BVC  Branch on Overflow Clear

branch on V = 0                  
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
relative      BVC oper      50    2     2**
*/
void MOS6502::bvc() { 
	// Execute instruction
	if (!this->SR.isBitSet(this->status::Overflow)) {
		// Branches are relative, signed jump
		this->PC.write(PC.read() + (int8_t) this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
	}
	else
		// Increment program counter and cycles
		this->step(2, 2);
}

/*
BVS  Branch on Overflow Set

branch on V = 1
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
relative      BVC oper      70    2     2**
*/
void MOS6502::bvs() { 
	// Execute instruction
	if (this->SR.isBitSet(this->status::Overflow)) {
		// Branches are relative, signed jump
		this->PC.write(PC.read() + (int8_t) this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
	}
	else
		// Increment program counter and cycles
		this->step(2, 2);
}

/*
CLC  Clear Carry Flag

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       CLC           18    1     2
*/
void MOS6502::clc() { 
	// Execute instruction
	this->SR.clearBit(status::Carry);

	// Increment program counter and cycles
	this->step(1, 2);
}

/*
CLD  Clear Decimal Mode

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       CLD           D8    1     2
*/
void MOS6502::cld() { 
	// Execute instruction
	this->SR.clearBit(status::Decimal);

	// Increment program counter and cycles
	this->step(1, 2);
}

/**
CLI  Clear Interrupt Disable Bit

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       CLI           58    1     2
*/
void MOS6502::cli() {
	// Execute instruction
	this->SR.clearBit(status::Interrupt);

	// Increment program counter and cycles
	this->step(1, 2);
}

/*
CLV  Clear Overflow Flag

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       CLV           B8    1     2
*/
void MOS6502::clv() { 
	// Execute instruction
	this->SR.clearBit(status::Overflow);

	// Increment program counter and cycles
	this->step(1, 2);
}

/*
CMP  Compare Memory with Accumulator

A - M                            
N Z C I D V
+ + + - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
immidiate     CMP #oper     C9    2     2
zeropage      CMP oper      C5    2     3
zeropage,X    CMP oper,X    D5    2     4
absolute      CMP oper      CD    3     4
absolute,X    CMP oper,X    DD    3     4*
absolute,Y    CMP oper,Y    D9    3     4*
(indirect,X)  CMP (oper,X)  C1    2     6
(indirect),Y  CMP (oper),Y  D1    2     5*
*/
void MOS6502::cmp(accessMode mode) { 
	uint8_t value;

	switch (mode) {
	case accessMode::Immediate:
		// Execute instruction
		value = this->nextWord();

		// Increment program counter and cycles
		this->step(2, 2);
		break;
	case accessMode::ZeroPage:
		// Execute instruction
		value = this->getZeroPageValue(0);

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageX:
		// Execute instruction
		value = this->getZeroPageValue(this->X.read());

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		value = this->getAbsoluteValue(0);

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteX:
		// Execute instruction
		value = this->getAbsoluteValue(this->X.read());

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteY:
		// Execute instruction
		value = this->getAbsoluteValue(this->Y.read());

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::IndirectX:
		// Execute instruction
		value = this->getXIndexedIndirectValue();

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::IndirectY:
		// Execute instruction
		value = this->getYIndirectIndexedValue();

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	}

	// Execute instruction on fetched value
	uint16_t temp = this->AC.read() - value;

	// Set processor status
	this->statusCheck(status::Negative, temp & 0x80);
	this->statusCheck(status::Carry, this->AC.read() >= value);
	this->statusCheck(status::Zero, !(temp & 0xFF));
}

/*
CPX  Compare Memory and Index X

X - M                            
N Z C I D V
+ + + - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
immidiate     CPX #oper     E0    2     2
zeropage      CPX oper      E4    2     3
absolute      CPX oper      EC    3     4
*/
void MOS6502::cpx(accessMode mode) {
	uint8_t value;

	switch (mode) {
	case accessMode::Immediate:
		// Signed operation
		value = this->nextWord();

		// Increment program counter and cycles
		this->step(2, 2);
		break;
	case accessMode::ZeroPage:
		// Signed operation
		value = this->getZeroPageValue(0);

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::Absolute:
		// Signed operation
		value = this->getAbsoluteValue(0);

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	}

	// Execute instruction on fetched value
	uint16_t temp = this->X.read() - value;

	// Set processor status
	this->statusCheck(status::Negative, temp & 0x80);
	this->statusCheck(status::Carry, this->X.read() >= value);
	this->statusCheck(status::Zero, !(temp & 0xFF));
}

/*
CPY  Compare Memory and Index Y

Y - M                            
N Z C I D V
+ + + - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
immidiate     CPY #oper     C0    2     2
zeropage      CPY oper      C4    2     3
absolute      CPY oper      CC    3     4
*/
void MOS6502::cpy(accessMode mode) {
	uint8_t value;

	switch (mode) {
	case accessMode::Immediate:
		// Signed operation
		value = this->nextWord();

		// Increment program counter and cycles
		this->step(2, 2);
		break;
	case accessMode::ZeroPage:
		// Signed operation
		value = this->getZeroPageValue(0);

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::Absolute:
		// Signed operation
		value = this->getAbsoluteValue(0);

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	}

	// Execute instruction on fetched value
	uint16_t temp = this->Y.read() - value;

	// Set processor status
	this->statusCheck(status::Negative, temp & 0x80);
	this->statusCheck(status::Carry, this->Y.read() >= value);
	this->statusCheck(status::Zero, !(temp & 0xFF));
}

/*
DEC  Decrement Memory by One

M - 1 -> M                       
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
zeropage      DEC oper      C6    2     5
zeropage,X    DEC oper,X    D6    2     6
absolute      DEC oper      CE    3     3
absolute,X    DEC oper,X    DE    3     7
*/
void MOS6502::dec(accessMode mode) {
	switch (mode) {
	case accessMode::ZeroPage:
		this->setZeroPageValue((this->getZeroPageValue(0) - 1) % 256, 0);

		// Status register tests
		this->negative(this->getZeroPageValue(0));
		this->zero(this->getZeroPageValue(0));

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	case accessMode::ZeroPageX:
		this->setZeroPageValue((this->getZeroPageValue(this->X.read()) - 1) % 256, this->X.read());

		// Status register tests
		this->negative(this->getZeroPageValue(this->X.read()));
		this->zero(this->getZeroPageValue(this->X.read()));

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::Absolute:
		this->setAbsoluteValue((this->getAbsoluteValue(0) - 1) % 256, 0);

		// Status register tests
		this->negative(this->getAbsoluteValue(0));
		this->zero(this->getAbsoluteValue(0));

		// Increment program counter and cycles
		this->step(3, 3);
		break;
	case accessMode::AbsoluteX:
		this->setAbsoluteValue((this->getAbsoluteValue(this->X.read()) - 1) % 256, this->X.read());

		// Status register tests
		this->negative(this->getAbsoluteValue(this->X.read()));
		this->zero(this->getAbsoluteValue(this->X.read()));

		// Increment program counter and cycles
		this->step(3, 7);
		break;
	}
}

/*
DEX  Decrement Index X by One

X - 1 -> X                       
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       DEC           CA    1     2
*/
void MOS6502::dex() { 
	// Execute instruction
	this->X.write((this->X.read() - 1) % 256);

	// Increment program counter and cycles
	this->step(1, 2);

	// Status register tests
	this->negative(this->X.read());
	this->zero(this->X.read());
}

/*
DEY  Decrement Index Y by One

Y - 1 -> Y                       
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       DEC           88    1     2
*/
void MOS6502::dey() { 
	// Execute instruction
	this->Y.write((this->Y.read() - 1) % 256);

	// Increment program counter and cycles
	this->step(1, 2);

	// Status register tests
	this->negative(this->Y.read());
	this->zero(this->Y.read());
}

/*
EOR  Exclusive-OR Memory with Accumulator

A EOR M -> A                     
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
immidiate     EOR #oper     49    2     2
zeropage      EOR oper      45    2     3
zeropage,X    EOR oper,X    55    2     4
absolute      EOR oper      4D    3     4
absolute,X    EOR oper,X    5D    3     4*
absolute,Y    EOR oper,Y    59    3     4*
(indirect,X)  EOR (oper,X)  41    2     6
(indirect),Y  EOR (oper),Y  51    2     5*
*/
void MOS6502::eor(accessMode mode) {
	switch (mode) {
	case accessMode::Immediate:
		// Execute instruction
		this->AC.write(this->AC.read() ^ this->nextWord());

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 2);
		break;
	case accessMode::ZeroPage:
		// Execute instruction
		this->AC.write(this->AC.read() ^ this->getZeroPageValue(0));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageX:
		// Execute instruction
		this->AC.write(this->AC.read() ^ this->getZeroPageValue(this->X.read()));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		this->AC.write(this->AC.read() ^ this->getAbsoluteValue(0));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteX:
		// Execute instruction
		this->AC.write(this->AC.read() ^ this->getAbsoluteValue(this->X.read()));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteY:
		// Execute instruction
		this->AC.write(this->AC.read() ^ this->getAbsoluteValue(this->Y.read()));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::IndirectX:
		// Execute instruction
		this->AC.write(this->AC.read() ^ getXIndexedIndirectValue());

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::IndirectY:
		// Execute instruction
		this->AC.write(this->AC.read() ^ getYIndirectIndexedValue());

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	}
}

/*
INC  Increment Memory by One

M + 1 -> M                       
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
zeropage      INC oper      E6    2     5
zeropage,X    INC oper,X    F6    2     6
absolute      INC oper      EE    3     6
absolute,X    INC oper,X    FE    3     7
*/
void MOS6502::inc(accessMode mode) { 
	switch (mode) {
	case accessMode::ZeroPage:
		this->setZeroPageValue(this->getZeroPageValue(0) + 1, 0);

		// Status register tests
		this->negative(this->getZeroPageValue(0));
		this->zero(this->getZeroPageValue(0));

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	case accessMode::ZeroPageX:
		this->setZeroPageValue(this->getZeroPageValue(this->X.read()) + 1, this->X.read());

		// Status register tests
		this->negative(this->getZeroPageValue(this->X.read()));
		this->zero(this->getZeroPageValue(this->X.read()));

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::Absolute:
		this->setAbsoluteValue(this->getAbsoluteValue(0) + 1, 0);

		// Status register tests
		this->negative(this->getAbsoluteValue(0));
		this->zero(this->getAbsoluteValue(0));

		// Increment program counter and cycles
		this->step(3, 6);
		break;
	case accessMode::AbsoluteX:
		this->setAbsoluteValue(this->getAbsoluteValue(this->X.read()) + 1, this->X.read());

		// Status register tests
		this->negative(this->getAbsoluteValue(this->X.read()));
		this->zero(this->getAbsoluteValue(this->X.read()));

		// Increment program counter and cycles
		this->step(3, 7);
		break;
	}
}

/*
INX  Increment Index X by One

X + 1 -> X                       
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       INX           E8    1     2
*/
void MOS6502::inx() { 
	// Execute instruction
	this->X.write(this->X.read() + 1);

	// Increment program counter and cycles
	this->step(1, 2);

	// Status register tests
	this->negative(this->X.read());
	this->zero(this->X.read());
}

/*
INY  Increment Index Y by One

Y + 1 -> Y                       N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       INY           C8    1     2
*/
void MOS6502::iny() { 
	// Execute instruction
	this->Y.write(this->Y.read() + 1);

	// Increment program counter and cycles
	this->step(1, 2);

	// Status register tests
	this->negative(this->Y.read());
	this->zero(this->Y.read());
}

/*
JMP  Jump to New Location

(PC+1) -> PCL                    N Z C I D V
(PC+2) -> PCH                    - - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
absolute      JMP oper      4C    3     3
indirect      JMP (oper)    6C    3     5
*/
void MOS6502::jmp(accessMode mode) { 
	switch (mode) {
	case accessMode::Absolute:
		// Halt on loop
		if (this->PC.read() == this->nextDWord()) {
			halt = true;
		}
			
		// Execute instruction
		this->PC.write(this->nextDWord());

		// Increment program counter and cycles
		this->step(0, 3);
		break;
	case accessMode::Indirect:
		uint16_t nextDWord = this->nextDWord();
		uint16_t realAddress, lEff, hEff;

		// Decode address
		lEff = this->memory->readWord(nextDWord);
		hEff = this->memory->readWord((nextDWord & 0xFF00) + ((nextDWord + 1) & 0x00FF)); 
		realAddress = lEff + 0x100 * hEff;

		// Halt on loop
		if (this->PC.read() == realAddress) {
			halt = true;
		}

		this->PC.write(realAddress);

		// Increment program counter and cycles
		this->step(0, 5);
		break;
	}
}

/*
JSR  Jump to New Location Saving Return Address

push (PC+2),                     
(PC+1) -> PCL                    
(PC+2) -> PCH

N Z C I D V
- - - - - -

Notes, please read the documentation on jsr and how
it stores address - 1 for various nifty things.

addressing    assembler    opc  bytes  cyles
--------------------------------------------
absolute      JSR oper      20    3     6
*/
void MOS6502::jsr() {
	// push pc
	this->pushDWord(this->PC.read()+2);

	// Set pc
	this->PC.write(this->nextDWord());
}

/*
LDA  Load Accumulator with Memory

M -> A                           
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
immidiate     LDA #oper     A9    2     2
zeropage      LDA oper      A5    2     3
zeropage,X    LDA oper,X    B5    2     4
absolute      LDA oper      AD    3     4
absolute,X    LDA oper,X    BD    3     4*
absolute,Y    LDA oper,Y    B9    3     4*
(indirect,X)  LDA (oper,X)  A1    2     6
(indirect),Y  LDA (oper),Y  B1    2     5*
*/
void MOS6502::lda(accessMode mode) {
	switch (mode) {
		case accessMode::Immediate:
		// Execute instruction
		this->AC.write(this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
		break;
	case accessMode::ZeroPage:
		// Execute instruction
		this->AC.write(this->getZeroPageValue(0));
	
		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageX:
		// Execute instruction
		this->AC.write(this->getZeroPageValue(this->X.read()));

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		this->AC.write(this->getAbsoluteValue(0));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteX:
		// Execute instruction
		this->AC.write(this->getAbsoluteValue(this->X.read()));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteY:
		// Execute instruction
		this->AC.write(this->getAbsoluteValue(this->Y.read()));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::IndirectX:
		// Execute instruction
		this->AC.write(this->getXIndexedIndirectValue());

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::IndirectY:
		// Execute instruction
		this->AC.write(this->getYIndirectIndexedValue());

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	}

	this->negative(this->AC.read());
	this->zero(this->AC.read());
}

/*
LDX  Load Index X with Memory

M -> X                          
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
immidiate     LDX #oper     A2    2     2
zeropage      LDX oper      A6    2     3
zeropage,Y    LDX oper,Y    B6    2     4
absolute      LDX oper      AE    3     4
absolute,Y    LDX oper,Y    BE    3     4*
*/
void MOS6502::ldx(accessMode mode) {
	switch (mode) {
	case accessMode::Immediate:
		// Execute instruction
		this->X.write(this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
		break;
	case accessMode::ZeroPage:
		// Execute instruction
		this->X.write(this->getZeroPageValue(0));

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageY:
		// Execute instruction
		this->X.write(this->getZeroPageValue(this->Y.read()));

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		this->X.write(this->getAbsoluteValue(0));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteY:
		// Execute instruction
		this->X.write(this->getAbsoluteValue(this->Y.read()));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	}

	// Set processor status register
	this->negative(this->X.read());
	this->zero(this->X.read());

}

/*
LDY  Load Index Y with Memory

M -> Y                           N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
immidiate     LDY #oper     A0    2     2
zeropage      LDY oper      A4    2     3
zeropage,X    LDY oper,X    B4    2     4
absolute      LDY oper      AC    3     4
absolute,X    LDY oper,X    BC    3     4*
*/
void MOS6502::ldy(accessMode mode) { 
	switch (mode) {
	case accessMode::Immediate:
		// Execute instruction
		this->Y.write(this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
		break;
	case accessMode::ZeroPage:
		// Execute instruction
		this->Y.write(this->getZeroPageValue(0));

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageX:
		// Execute instruction
		this->Y.write(this->getZeroPageValue(this->X.read()));

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		this->Y.write(this->getAbsoluteValue(0));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteX:
		// Execute instruction
		this->Y.write(this->getAbsoluteValue(this->X.read()));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	}

	// Set processor status register
	this->negative(this->Y.read());
	this->zero(this->Y.read());
}

/*
LSR  Shift One Bit Right (Memory or Accumulator)

0 -> [76543210] -> C             
N Z C I D V
- + + - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
accumulator   LSR A         4A    1     2
zeropage      LSR oper      46    2     5
zeropage,X    LSR oper,X    56    2     6
absolute      LSR oper      4E    3     6
absolute,X    LSR oper,X    5E    3     7
*/
void MOS6502::lsr(accessMode mode) {
	switch (mode) {
	case accessMode::Accumulator:
		// Set carry
		this->statusCheck(status::Carry, this->AC.read() & 0x01);

		// Execute instruction
		this->AC.write((this->AC.read() >> 1));

		// Set processor status
		this->negative(0);
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(1, 2);
		break;
	case accessMode::ZeroPage:
		// Set carry
		this->statusCheck(status::Carry, (this->getZeroPageValue(0) & 0x01));

		// Execute instruction
		this->setZeroPageValue((this->getZeroPageValue(0) >> 1), 0);

		// Set processor status
		this->negative(0);
		this->zero(this->getZeroPageValue(0));

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	case accessMode::ZeroPageX:
		// Set carry
		this->statusCheck(status::Carry, (this->getZeroPageValue(this->X.read()) & 0x01));

		// Execute instruction
		this->setZeroPageValue((this->getZeroPageValue(this->X.read()) >> 1), this->X.read());

		this->negative(0);
		this->zero(this->getZeroPageValue(this->X.read()));

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::Absolute:
		// Set carry
		this->statusCheck(status::Carry, (this->getAbsoluteValue(0) & 0x01));

		// Execute instruction
		this->setAbsoluteValue((this->getAbsoluteValue(0) >> 1), 0);

		// Set processor status
		this->negative(0);
		this->zero(this->getAbsoluteValue(0));

		// Increment program counter and cycles
		this->step(3, 6);
		break;
	case accessMode::AbsoluteX:
		// Set carry
		this->statusCheck(status::Carry, (this->getAbsoluteValue(this->X.read()) & 0x01));

		// Execute instruction
		this->setAbsoluteValue((this->getAbsoluteValue(this->X.read()) >> 1), this->X.read());

		this->negative(0);
		this->zero(this->getAbsoluteValue(this->X.read()));

		// Increment program counter and cycles
		this->step(3, 7);
		break;
	}
}

/*
NOP  No Operation

---                              
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       NOP           EA    1     2
*/
void MOS6502::nop() {
	// Increment program counter and cycles
	this->step(1, 2);
}

/*
ORA  OR Memory with Accumulator

A OR M -> A                      
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
immidiate     ORA #oper     09    2     2
zeropage      ORA oper      05    2     3
zeropage,X    ORA oper,X    15    2     4
absolute      ORA oper      0D    3     4
absolute,X    ORA oper,X    1D    3     4*
absolute,Y    ORA oper,Y    19    3     4*
(indirect,X)  ORA (oper,X)  01    2     6
(indirect),Y  ORA (oper),Y  11    2     5*
*/
void MOS6502::ora(accessMode mode) {
	switch (mode) {
	case accessMode::Immediate:
		// Execute instruction
		this->AC.write(this->AC.read() | this->nextWord());

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 2);
		break;
	case accessMode::ZeroPage:
		// Execute instruction
		this->AC.write(this->AC.read() | this->getZeroPageValue(0));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageX:
		// Execute instruction
		this->AC.write(this->AC.read() | this->getZeroPageValue(this->X.read()));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		this->AC.write(this->AC.read() | this->getAbsoluteValue(0));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteX:
		// Execute instruction
		this->AC.write(this->AC.read() | this->getAbsoluteValue(this->X.read()));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteY:
		// Execute instruction
		this->AC.write(this->AC.read() | this->getAbsoluteValue(this->Y.read()));

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::IndirectX:
		// Execute instruction
		this->AC.write(this->AC.read() | getXIndexedIndirectValue());

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::IndirectY:
		// Execute instruction
		this->AC.write(this->AC.read() | getYIndirectIndexedValue());

		// Set status register
		this->negative(this->AC.read());
		this->zero(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	}
}

/*
PHA  Push Accumulator on Stack

push A                           
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       PHA           48    1     3
*/
void MOS6502::pha() {
	// Execute instruction
	this->pushWord(this->AC.read());

	// Increment program counter and cycles
	this->step(1, 3);
}

/*
PHP  Push Processor Status on Stack

push SR                          
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       PHP           08    1     3
*/
void MOS6502::php() {
	// Execute instruction
	this->pushWord(this->SR.read() );

	// Increment program counter and cycles
	this->step(1, 3);
}

/*
PLA  Pull Accumulator from Stack

pull A                          
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       PLA           68    1     4
*/
void MOS6502::pla() {
	// Execute instruction
	this->AC.write(this->popWord());

	// Status register tests
	this->statusCheck(status::Negative, this->AC.read() & 0x80);
	this->statusCheck(status::Zero, !this->AC.read());

	// Increment program counter and cycles
	this->step(1, 4);
}

/*
PLP  Pull Processor Status from Stack

pull SR 
from stack

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       PHP           28    1     4
*/
void MOS6502::plp() {
	// Execute instruction
	this->SR.write(this->popWord() | decode);

	// Increment program counter and cycles
	this->step(1, 4);
}

/*
ROL  Rotate One Bit Left (Memory or Accumulator)

C <- [76543210] <- C             
N Z C I D V
+ + + - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
accumulator   ROL A         2A    1     2
zeropage      ROL oper      26    2     5
zeropage,X    ROL oper,X    36    2     6
absolute      ROL oper      2E    3     6
absolute,X    ROL oper,X    3E    3     7
*/
void MOS6502::rol(accessMode mode) { 
	uint16_t value = 0;

	switch (mode) {
	case accessMode::Accumulator:
		value = this->AC.read();

		// Shift left one bit 
		value = value << 1;

		// If carry set, add 1
		if (this->SR.isBitSet(status::Carry)) value |= 0x01;
		this->statusCheck(status::Carry, value > 0xFF);

		// Write back
		this->AC.write(value);

		// Increment program counter and cycles
		this->step(1, 2);
		break;
	case accessMode::ZeroPage:
		value = this->getZeroPageValue(0);

		// Shift left one bit 
		value = value << 1;

		// If carry set, add 1
		if (this->SR.isBitSet(status::Carry)) value |= 0x01;
		this->statusCheck(status::Carry, value > 0xFF);

		// Write back
		this->setZeroPageValue(value, 0);

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	case accessMode::ZeroPageX:
		value = this->getZeroPageValue(this->X.read());

		// Shift left one bit 
		value = value << 1;

		// If carry set, add 1
		if (this->SR.isBitSet(status::Carry)) value |= 0x01;
		this->statusCheck(status::Carry, value > 0xFF);

		// Write back
		this->setZeroPageValue(value, this->X.read());

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::Absolute:
		value = this->getAbsoluteValue(0);

		// Shift left one bit 
		value = value << 1;

		// If carry set, add 1
		if (this->SR.isBitSet(status::Carry)) value |= 0x01;
		this->statusCheck(status::Carry, value > 0xFF);

		// Write back
		this->setAbsoluteValue(value, 0);

		// Increment program counter and cycles
		this->step(3, 6);
		break;
	case accessMode::AbsoluteX:
		value = this->getAbsoluteValue(this->X.read());

		// Shift left one bit 
		value = value << 1;

		// If carry set, add 1
		if (this->SR.isBitSet(status::Carry)) value |= 0x01;
		this->statusCheck(status::Carry, value > 0xFF);

		// Write back
		this->setAbsoluteValue(value, this->X.read());

		// Increment program counter and cycles
		this->step(3, 7);
		break;
	}

	// SR status checks
	this->statusCheck(status::Negative, value & 0x80);
	this->statusCheck(status::Zero, !value);

}

/*
ROR  Rotate One Bit Right (Memory or Accumulator)

C -> [76543210] -> C             
N Z C I D V
+ + + - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
accumulator   ROR A         6A    1     2
zeropage      ROR oper      66    2     5
zeropage,X    ROR oper,X    76    2     6
absolute      ROR oper      6E    3     6
absolute,X    ROR oper,X    7E    3     7
*/
void MOS6502::ror(accessMode mode) { 
	uint16_t value = 0;

	switch (mode) {
	case accessMode::Accumulator:
		value = this->AC.read();

		// If carry set, add 1
		if (this->SR.isBitSet(status::Carry)) value |= 0x100;
		this->statusCheck(status::Carry, value & 0x01);

		// Shift right one bit 
		value = value >> 1;
		value = value & 0xFF;

		// Write back
		this->AC.write(value & 0xFF);

		// Increment program counter and cycles
		this->step(1, 2);
		break;
	case accessMode::ZeroPage:
		value = this->getZeroPageValue(0);

		// If carry set, add 1
		if (this->SR.isBitSet(status::Carry)) value |= 0x100;
		this->statusCheck(status::Carry, value & 0x01);

		// Shift right one bit 
		value = value >> 1;
		value = value & 0xFF;

		// Write back
		this->setZeroPageValue(value, 0);

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	case accessMode::ZeroPageX:
		value = this->getZeroPageValue(this->X.read());

		// If carry set, add 1
		if (this->SR.isBitSet(status::Carry)) value |= 0x100;
		this->statusCheck(status::Carry, value & 0x01);

		// Shift right one bit 
		value = value >> 1;
		value = value & 0xFF;

		// Write back
		this->setZeroPageValue(value, this->X.read());

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::Absolute:
		value = this->getAbsoluteValue(0);

		// If carry set, add 1
		if (this->SR.isBitSet(status::Carry)) value |= 0x100;
		this->statusCheck(status::Carry, value & 0x01);

		// Shift right one bit 
		value = value >> 1;
		value = value & 0xFF;

		// Write back
		this->setAbsoluteValue(value, 0);

		// Increment program counter and cycles
		this->step(3, 6);
		break;
	case accessMode::AbsoluteX:
		value = this->getAbsoluteValue(this->X.read());

		// If carry set, add 1
		if (this->SR.isBitSet(status::Carry)) value |= 0x100;
		this->statusCheck(status::Carry, value & 0x01);

		// Shift right one bit 
		value = value >> 1;
		value = value & 0xFF;

		// Write back
		this->setAbsoluteValue(value, this->X.read());

		// Increment program counter and cycles
		this->step(3, 7);
		break;
	}

	// SR status checks
	this->statusCheck(status::Negative, value & 0x80);
	this->statusCheck(status::Zero, !value);
}

/*
RTI  Return from Interrupt

pull SR, pull PC                 N Z C I D V
from stack

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       RTI           40    1     6
*/
void MOS6502::rti() { 
	this->SR.write(this->popWord());
	this->PC.write(this->popDWord());
	
	// Increment program counter and cycles
	this->step(0, 6);
}

/*
RTS  Return from Subroutine

pull PC, PC+1 -> PC              
N Z C I D V
- - - - - -

See comments on jsr.

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       RTS           60    1     6
*/
void MOS6502::rts() { 
	// Execute instruction
	this->PC.write(this->popDWord());

	// Increment program counter and cycles
	this->step(1, 6);
}

/*
SBC  Subtract Memory from Accumulator with Borrow

A - M - C -> A                   
N Z C I D V
+ + + - - +

addressing    assembler    opc  bytes  cyles
--------------------------------------------
immidiate     SBC #oper     E9    2     2
zeropage      SBC oper      E5    2     3
zeropage,X    SBC oper,X    F5    2     4
absolute      SBC oper      ED    3     4
absolute,X    SBC oper,X    FD    3     4*
absolute,Y    SBC oper,Y    F9    3     4*
(indirect,X)  SBC (oper,X)  E1    2     6
(indirect),Y  SBC (oper),Y  F1    2     5*
*/
void MOS6502::sbc(accessMode mode) { 
	switch (mode) {
	case accessMode::Immediate:
		// Execute instruction
		this->executeSbc(this->nextWord());

		// Increment program counter and cycles
		this->step(2, 2);
		break;
	case accessMode::ZeroPage:
		// Execute instruction
		this->executeSbc(this->getZeroPageValue(0));

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageX:
		// Execute instruction
		this->executeSbc(this->getZeroPageValue(this->X.read()));

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		this->executeSbc(this->getAbsoluteValue(0));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteX:
		// Execute instruction
		this->executeSbc(this->getAbsoluteValue(this->X.read()));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteY:
		// Execute instruction
		this->executeSbc(this->getAbsoluteValue(this->Y.read()));

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::IndirectX:
		// Execute instruction
		this->executeSbc(this->getXIndexedIndirectValue());

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::IndirectY:
		// Execute instruction
		this->executeSbc(this->getYIndirectIndexedValue());

		// Increment program counter and cycles
		this->step(2, 5);
		break;
	}
}

/*
SEC  Set Carry Flag

1 -> C                           
N Z C I D V
- - 1 - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       SEC           38    1     2
*/
void MOS6502::sec() { 
	// Execute instruction
	this->SR.setBit(status::Carry);

	// Increment program counter and cycles
	this->step(1, 2);
}

/*
SED  Set Decimal Flag

1 -> D                           
N Z C I D V
- - - - 1 -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       SED           F8    1     2
*/
void MOS6502::sed() { 
	// Execute instruction
	this->SR.setBit(status::Decimal);

	// Increment program counter and cycles
	this->step(1, 2);
}

/*
SEI  Set Interrupt Disable Status

1 -> I                          
N Z C I D V
- - - 1 - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       SEI           78    1     2
*/
void MOS6502::sei() { 
	// Execute instruction
	this->SR.setBit(status::Interrupt);

	// Increment program counter and cycles
	this->step(1, 2);
}

/*
STA  Store Accumulator in Memory

A -> M                           
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
zeropage      STA oper      85    2     3
zeropage,X    STA oper,X    95    2     4
absolute      STA oper      8D    3     4
absolute,X    STA oper,X    9D    3     5
absolute,Y    STA oper,Y    99    3     5
(indirect,X)  STA (oper,X)  81    2     6
(indirect),Y  STA (oper),Y  91    2     6

*/
void MOS6502::sta(accessMode mode) { 
	switch (mode) {
	case accessMode::ZeroPage:
		// Execute instruction
		this->setZeroPageValue(this->AC.read(), 0);

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageX:
		// Execute instruction
		this->setZeroPageValue(this->AC.read(), this->X.read());

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		this->setAbsoluteValue(this->AC.read(), 0);

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	case accessMode::AbsoluteX:
		// Execute instruction
		this->setAbsoluteValue(this->AC.read(), this->X.read());

		// Increment program counter and cycles
		this->step(3, 5);
		break;
	case accessMode::AbsoluteY:
		// Execute instruction
		this->setAbsoluteValue(this->AC.read(), this->Y.read());

		// Increment program counter and cycles
		this->step(3, 5);
		break;
	case accessMode::IndirectX:
		// Execute instruction
		this->setXIndexedIndirectValue(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	case accessMode::IndirectY:
		// Execute instruction
		this->setYIndirectIndexedValue(this->AC.read());

		// Increment program counter and cycles
		this->step(2, 6);
		break;
	}
}

/*
STX  Store Index X in Memory

X -> M                           
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
zeropage      STX oper      86    2     3
zeropage,Y    STX oper,Y    96    2     4
absolute      STX oper      8E    3     4
*/
void MOS6502::stx(accessMode mode) { 
	switch (mode) {
	case accessMode::ZeroPage:
		// Execute instruction
		this->setZeroPageValue(this->X.read(), 0);

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageY:
		// Execute instruction
		this->setZeroPageValue(this->X.read(), this->Y.read());

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		this->setAbsoluteValue(this->X.read(), 0);

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	}
}

/*
STY  Sore Index Y in Memory

Y -> M                           
N Z C I D V
- - - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
zeropage      STY oper      84    2     3
zeropage,X    STY oper,X    94    2     4
absolute      STY oper      8C    3     4
*/
void MOS6502::sty(accessMode mode) {
	switch (mode) {
	case accessMode::ZeroPage:
		// Execute instruction
		this->setZeroPageValue(this->Y.read(), 0);

		// Increment program counter and cycles
		this->step(2, 3);
		break;
	case accessMode::ZeroPageX:
		// Execute instruction
		this->setZeroPageValue(this->Y.read(), this->X.read());

		// Increment program counter and cycles
		this->step(2, 4);
		break;
	case accessMode::Absolute:
		// Execute instruction
		this->setAbsoluteValue(this->Y.read(), 0);

		// Increment program counter and cycles
		this->step(3, 4);
		break;
	}
}

/*
TAX  Transfer Accumulator to Index X

A -> X                           
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       TAX           AA    1     2
*/
void MOS6502::tax() { 
	// Execute instruction
	this->X.write(this->AC.read());

	// Increment program counter and cycles
	this->step(1, 2);

	// Status register tests
	this->negative(this->X.read());
	this->zero(this->X.read());
}

/*
TAY  Transfer Accumulator to Index Y

A -> Y                           
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       TAY           A8    1     2
*/
void MOS6502::tay() { 
	// Execute instruction
	this->Y.write(this->AC.read());

	// Increment program counter and cycles
	this->step(1, 2);;

	// Status register tests
	this->negative(this->Y.read());
	this->zero(this->Y.read());
}

/*
TSX  Transfer Stack Pointer to Index X

SP -> X                          
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       TSX           BA    1     2
*/
void MOS6502::tsx() { 
	// Execute instruction
	this->X.write(this->SP.read());

	// Increment program counter and cycles
	this->step(1, 2);

	// Status register tests
	this->negative(this->X.read());
	this->zero(this->X.read());
}

/*
TXA  Transfer Index X to Accumulator

X -> A                           
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       TXA           8A    1     2
*/
void MOS6502::txa() { 
	// Execute instruction
	this->AC.write(this->X.read());

	// Increment program counter and cycles
	this->step(1, 2);

	// Status register tests
	this->negative(this->AC.read());
	this->zero(this->AC.read());
}

/*
TXS  Transfer Index X to Stack Register

X -> SP                         
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       TXS           9A    1     2
*/
void MOS6502::txs() { 
	// Execute instruction
	this->SP.write(this->X.read());

	// Increment program counter and cycles
	this->step(1, 2);

	// Status register tests
	this->negative(this->SP.read());
	this->zero(this->SP.read());
}

/*
TYA  Transfer Index Y to Accumulator

Y -> A                           
N Z C I D V
+ + - - - -

addressing    assembler    opc  bytes  cyles
--------------------------------------------
implied       TYA           98    1     2
*/
void MOS6502::tya() { 
	// Execute instruction
	this->AC.write(this->Y.read());

	// Increment program counter and cycles
	this->step(1, 2);

	// Status register tests
	this->negative(this->AC.read());
	this->zero(this->AC.read());
}
