#ifndef _H_MOS6502
#define _H_MOS6502

#include "IC.h"
#include "Register16.h"
#include "Register8.h"
#include "MOS6502Memory.h"

#include <stdint.h>

class MOS6502 : public IC {
public:
	// Constructors
	MOS6502();
	~MOS6502();

	// User interface
	void initialize();
	void setMemory(MOS6502Memory *memory);

	// Executes next instruction in memory
	void executeInstruction();

	// Interrupts
	bool interrupt();
	void nonMaskableInterrupt();

	// Getters
	uint64_t getPc();
	uint64_t getInstructions();
	bool isHalting();

	// Enums
	enum accessMode {Accumulator, Immediate, ZeroPage, ZeroPageX, ZeroPageY, Absolute, AbsoluteX, AbsoluteY, Indirect, IndirectX, IndirectY};
	enum status { Negative = 7, Overflow = 6, Break = 4, Decimal = 3, Interrupt = 2, Zero = 1, Carry = 0 };

private:
	// MOS 6502 instruction set	
	void adc(accessMode);
	void and(accessMode);
	void asl(accessMode);
	void bcc();
	void bcs();
	void beq();
	void bit(accessMode);
	void bmi();
	void bne();
	void bpl();
	void brk();
	void bvc();
	void bvs();
	void clc();
	void cld();
	void cli();
	void clv();
	void cmp(accessMode);
	void cpx(accessMode);
	void cpy(accessMode);
	void dec(accessMode);
	void dex();
	void dey();
	void eor(accessMode);
	void inc(accessMode);
	void inx();
	void iny();
	void jmp(accessMode);
	void jsr();
	void lda(accessMode);
	void ldx(accessMode);
	void ldy(accessMode);
	void lsr(accessMode);
	void nop();
	void ora(accessMode);
	void pha();
	void php();
	void pla();
	void plp();
	void rol(accessMode);
	void ror(accessMode);
	void rti();
	void rts();
	void sbc(accessMode);
	void sec();
	void sed();
	void sei();
	void sta(accessMode);
	void stx(accessMode);
	void sty(accessMode);
	void tax();
	void tay();
	void tsx();
	void txa();
	void txs();
	void tya();

	// Status register result tests
	inline void statusCheck(status s, bool expression) {
		if (expression) {
			this->SR.setBit(s);
		}
		else {
			this->SR.clearBit(s);
		}
	}

	inline void negative(unsigned int result) {
		if (result & 0x80)
			this->SR.setBit(status::Negative);
		else
			this->SR.clearBit(status::Negative);
	}

	inline void zero(unsigned int result) {
		if (!(result & 0xFF)) {
			this->SR.setBit(status::Zero);
		}
		else {
			this->SR.clearBit(status::Zero);
		}
	}

	inline void carry(uint16_t result) {
		if (result & 0x100) {
			this->SR.setBit(status::Carry);
		} else {
			this->SR.clearBit(status::Carry);
		}
	}

	void executeAdc(uint16_t value) {
		uint16_t temp = (uint16_t) this->AC.read() + (uint16_t)value;

		// Add one on carry set
		if (this->SR.isBitSet(status::Carry)) {temp += 1;}		

		this->statusCheck(status::Zero, !(temp & 0xff));

		// On decimal set
		if (this->SR.isBitSet(status::Decimal)) {
			if (((this->AC.read() & 0xF) + (value & 0xF) + (this->SR.isBitSet(status::Carry) ? 1 : 0)) > 9) temp += 6;
			this->statusCheck(status::Negative, (temp & 0x80));
			this->statusCheck(status::Overflow, !((this->AC.read() ^ value) & 0x80) && ((this->AC.read() ^ temp) & 0x80));
			if (temp > 0x99) {
				temp += 96;
			}
			this->statusCheck(status::Carry, (temp > 0x99));
		}
		else {
			// Non decimal check
			this->statusCheck(status::Overflow, !((this->AC.read() ^ value) & 0x80) && ((this->AC.read() ^ temp) & 0x80));
			this->statusCheck(status::Carry, (temp > 0xFF));
			this->statusCheck(status::Negative, (temp & 0x80));
		}

		this->AC.write(temp);
	}

	void executeSbc(uint16_t value) {
		uint16_t temp = (uint16_t) this->AC.read() - (uint16_t)value;

		// Minus one on carry unset
		if (!this->SR.isBitSet(status::Carry)) {temp -= 1;}

		// Status checks
		this->statusCheck(status::Negative, temp & 0x80);
		this->statusCheck(status::Zero, !(temp & 0xFF));
		this->statusCheck(status::Overflow, !((this->AC.read() ^ value) & 0x80) && ((this->AC.read() ^ temp) & 0x80));

		// On decimal set
		if (this->SR.isBitSet(status::Decimal)) {
			if (((this->AC.read() & 0x0F) - (this->SR.isBitSet(status::Carry) ? 0 : 1)) < (value & 0x0F)) temp -= 6;
			if (temp > 0x99)
			{
				temp -= 0x60;
			}
		}

		// Happens after decimal check
		this->statusCheck(status::Carry, temp < 0x100);

		this->AC.write(temp & 0xFF);

	}

	// Increment PC and cycles
	inline void step(uint8_t pc, uint8_t cycles) {
		this->PC.write(this->PC.read() + pc);
		this->cycles += cycles;
	}

	// Relative program counter methods
	inline uint8_t nextWord() {
		return this->memory->readWord(this->PC.read() + 1);
	}

	inline uint16_t nextDWord() {
		return this->memory->readDWord(this->PC.read() + 1); 
	}

	// Addressing methods
	inline uint8_t getZeroPageValue(uint8_t offset) {
		return this->memory->readWord((uint8_t)(this->nextWord() + offset) % 256);
	}

	inline uint8_t getAbsoluteValue(uint16_t offset) {
		return this->memory->readWord((this->nextDWord() + offset));
	}

	inline void setZeroPageValue(uint8_t value, uint8_t offset) {
		this->memory->writeWord((uint8_t)(this->nextWord() + offset) % 256, value);
	}

	inline void setAbsoluteValue(uint8_t value, uint16_t offset) {
		this->memory->writeWord((this->nextDWord() + offset), value);
	}

	uint8_t getXIndexedIndirectValue() {
		uint16_t lZero, hZero, addr;

		// Done instead of reading dword directly for zero page wraparound
		lZero = (this->nextWord() + this->X.read()) % 256;
		hZero = (lZero + 1) % 256;

		// Combine address
		addr = this->memory->readWord(lZero) + (this->memory->readWord(hZero) << 8);

		// Return value
		return this->memory->readWord(addr);
	}

	uint8_t getYIndirectIndexedValue() {
		uint16_t lZero, hZero, addr;

		// Done instead of reading dword directly for zero page wraparound
		lZero = this->nextWord();
		hZero = (lZero + 1) % 256;

		// Combine address
		addr = this->memory->readWord(lZero) + (this->memory->readWord(hZero) << 8) + this->Y.read();

		// Return value
		return this->memory->readWord(addr);
	}

	void setXIndexedIndirectValue(uint8_t value) {
		uint16_t lZero, hZero, addr;

		// Done instead of reading dword directly for zero page wraparound
		lZero = (this->nextWord() + this->X.read()) % 256;
		hZero = (lZero + 1) % 256;

		// Combine address
		addr = this->memory->readWord(lZero) + (this->memory->readWord(hZero) << 8);

		// Set value
		this->memory->writeWord(addr, value);
	}

	void setYIndirectIndexedValue(uint8_t value) {
		uint16_t lZero, hZero, addr;

		// Done instead of reading dword directly for zero page wraparound
		lZero = this->nextWord();
		hZero = (lZero + 1) % 256;

		// Combine address
		addr = this->memory->readWord(lZero) + (this->memory->readWord(hZero) << 8) + this->Y.read();

		// Set value
		this->memory->writeWord(addr, value);
	}

	// Push pop methods
	inline void pushWord(uint8_t value) {
		this->memory->writeWord(stackBase + this->SP.read(), value);
		if(SP.read() == 0x00) SP.write(0xFF); else SP.write(SP.read() - 1);
	};

	inline void pushDWord(uint16_t value) {
		uint16_t l = value & 0xFF;
		uint16_t h = (value >> 8) & 0xFF;
		this->pushWord(h);
		this->pushWord(l);
	};

	inline uint8_t popWord() {
		if (SP.read() == 0xFF) SP.write(0x00); else SP.write(SP.read() + 1);
		uint8_t value = this->memory->readWord(stackBase + SP.read());	
		return value;
	};

	inline uint16_t popDWord() {
		uint16_t l = this->popWord();
		uint16_t h = this->popWord();
		uint16_t value = (h << 8) | l;
		return value;
	};

	// Registers
	Register16 PC;
	Register8 AC, X, Y, SR, SP;

	// Memory
	MOS6502Memory *memory;

	// Instruction counter
	uint64_t instructions;

	// Properties
	bool halt;

	// Constants
	static const uint16_t stackBase = 0x0100;
	static const uint16_t nmiVector = 0xFFFA;
	static const uint16_t rstVector = 0xFFFC;
	static const uint16_t irqVector = 0xFFFE;
	static const uint8_t decode = 0x20;
};

#endif