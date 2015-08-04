#include "MOS6522.h"
#include <iostream>

MOS6522::MOS6522() : IC()
					, keyPressed(0)
					, shiftPressed(false)
					, cbmPressed(false) {
	// Set clock speed
	this->setClockSpeed(2000000);
}

MOS6522::~MOS6522() {

}

void MOS6522::setMemory(MOS6502Memory *memory) {
	this->memory = memory;

	// Create callbacks
	//this->createCallBacks();
}

void MOS6522::setCpu(MOS6502 *mos6502) {
	this->mos6502 = mos6502;
}

void MOS6522::setKeyPressed(uint16_t key) {
	/* Store high byte first 
		= 0x9120 << 8 | 0x9121
	*/
	keyPressed = key;
}

void MOS6522::setShiftPressed(bool state) {
	this->shiftPressed = state;
}

void MOS6522::setCbmPressed(bool state) {
	this->cbmPressed = state;
}

void MOS6522::setJoyStickPressed(Vic20JoyStickButton button, bool state) {
	this->joyStick[button] = state;
}

void MOS6522::initialize() {
	// Clear joystick memory
	this->memory->writeWord(0x9111, 0xFF);
}

void MOS6522::tick() {
	// Tick timer cycle
	this->tickTimers();

	// Handle input in this cycle
	this->tickInput();

	// Increment cycles counter
	this->cycles += 1;
}

void MOS6522::tickInput() {
	// Handle keyboard input
	// Get keyboard column selection value
	uint8_t via2PortBValue = this->memory->readWord(this->PORTB);

	// Get row and column of key press
	uint8_t column = (keyPressed >> 8) & 0xFF;
	uint8_t row = keyPressed & 0xFF;

	// Check whether keyboard should be scanned
	if (via2PortBValue == 0) {
		// If port b is 0 then the vic is asking whether any keys are pressed on the keyboard
		this->memory->silentWriteWord(this->PORTA, keyPressed == 0 ? 0xFF : row);
		this->memory->silentWriteWord(this->PORTAMIRROR, keyPressed == 0 ? 0xFF : row);
	}
	else {
		// Return keyboard row depending on column state
		this->memory->silentWriteWord(this->PORTA, (via2PortBValue == column ? (row == 0 ? 0xFF : row) : 0xFF));
		this->memory->silentWriteWord(this->PORTAMIRROR, (via2PortBValue == column ? (row == 0 ? 0xFF : row) : 0xFF));
	}

	if ((via2PortBValue == 0xF7) && shiftPressed) {
		// Return row of shift key
		this->memory->silentWriteWord(this->PORTA, 0xFD & (keyPressed == 0 ? 0xFF : (column == 0xF7 ? row : 0xFF)));
		this->memory->silentWriteWord(this->PORTAMIRROR, 0xFD & (keyPressed == 0 ? 0xFF : (column == 0xF7 ? row : 0xFF)));
	} else if (via2PortBValue == 0xDF && cbmPressed) {
		// Return row of cbm key
		this->memory->silentWriteWord(this->PORTA, 0xFE & (keyPressed == 0 ? 0xFF : (column == 0xDF ? row : 0xFF)));
		this->memory->silentWriteWord(this->PORTAMIRROR, 0xFE & (keyPressed == 0 ? 0xFF : (column == 0xDF ? row : 0xFF)));
	}

	// Handle joystick input
	this->memory->silentWriteWord(via1PORTA, 0xFF);
	this->memory->silentWriteWord(via1PORTAMIRROR, 0xFF);

	// If this bit is input, set it to default value, else leave it as it is so it won't mess up keyboard input
	if (!(this->memory->readWord(via2PortBDDR) & 0x80)) {
		this->memory->silentWriteWord(PORTB, this->memory->silentReadWord(PORTB) | 0x80);
	}

	if (joyStick[Vic20JoyStickButton::Fire]) {
		this->memory->silentWriteWord(via1PORTA, this->memory->silentReadWord(via1PORTA) & ~0x20);
		this->memory->silentWriteWord(via1PORTAMIRROR, this->memory->silentReadWord(via1PORTA) & ~0x20);
	}

	if (joyStick[Vic20JoyStickButton::Up]) {
		this->memory->silentWriteWord(via1PORTA, this->memory->silentReadWord(via1PORTA) & ~0x4);
		this->memory->silentWriteWord(via1PORTAMIRROR, this->memory->silentReadWord(via1PORTA) & ~0x4);
	}

	if (joyStick[Vic20JoyStickButton::Down]) {
		this->memory->silentWriteWord(via1PORTA, this->memory->silentReadWord(via1PORTA) & ~0x8);
		this->memory->silentWriteWord(via1PORTAMIRROR, this->memory->silentReadWord(via1PORTA) & ~0x8);
	}

	if (joyStick[Vic20JoyStickButton::Left]) {
		this->memory->silentWriteWord(via1PORTA, this->memory->silentReadWord(via1PORTA) & ~0x10);
		this->memory->silentWriteWord(via1PORTAMIRROR, this->memory->silentReadWord(via1PORTA) & ~0x10);
	}

	if (joyStick[Vic20JoyStickButton::Right]) {
		this->memory->silentWriteWord(PORTB, this->memory->silentReadWord(0x9120) & ~0x80);
	}
}

void MOS6522::tickTimers() {
	// Grab values of timer related registers
	uint8_t interruptEnable = this->memory->readWord(this->irqEnableAddress);
	uint8_t interruptFlags = this->memory->readWord(this->irqFlagsAddress);
	uint8_t auxControl = this->memory->readWord(this->auxControlAddress);

	// Get timer values
	uint16_t timer1 = this->memory->silentReadDWord(this->timer1DAddress);
	uint16_t timer2 = this->memory->silentReadDWord(this->timer2DAddress);

	// If timer is active, decrement it
	if (timer1 > 0)
		this->memory->silentWriteDWord(this->timer1DAddress, --timer1);

	if (timer2 > 0 && this->memory->silentReadWord(this->timer2HighByteLatch) != 0) {
		// Decrement t2
		this->memory->silentWriteDWord(this->timer2DAddress, --timer2);
	}

	// If timeout
	if (timer1 == 0) {
		// Set interrupt flag
		this->memory->silentWriteWord(this->irqFlagsAddress, interruptFlags | 0x40);
		interruptFlags = this->memory->readWord(this->irqFlagsAddress);
	}

	if (timer2 == 0) {
		// Reset high byte latch
		if (this->memory->readWord(this->timer2HighByteLatch) != 0) {
			this->memory->silentWriteDWord(this->timer2DAddress, this->memory->silentReadDWord(this->timer2LowByteLatch));
			this->memory->silentWriteDWord(this->timer2HighByteLatch, 0);

			// Set interrupt flag
			this->memory->silentWriteWord(this->irqFlagsAddress, interruptFlags | 0x20);
			interruptFlags = this->memory->readWord(this->irqFlagsAddress);
		}
	}

	// Check if an interrupt request exists in the system
	if (interruptFlags & interruptEnable & 0x7F) {
		// Send an interrupt request to the cpu
		if (mos6502->interrupt()) {
			// Post interrupt operation
			if (timer1 == 0) {
				// Determine what to do with timer based on auxiliary control register
				if (auxControl & 0x40) {
					// Continuous interrupt
					this->memory->silentWriteDWord(this->timer1DAddress, this->memory->silentReadDWord(this->timer1LowByteLatch));
				}

				// Reset interrupt flag
				this->memory->silentWriteWord(irqFlagsAddress, this->memory->silentReadWord(irqFlagsAddress) & ~0x40);
			}

			if (timer2 == 0) {
				// Reset interrupt flag
				this->memory->silentWriteWord(irqFlagsAddress, this->memory->silentReadWord(irqFlagsAddress) & ~0x20);
			}
		}
	}
}

void MOS6522::createCallBacks() {
	/*
	class Via1PortACallBack : public MOS6502MemoryCallBack {
	public:
		Via1PortACallBack(MOS6502Memory *memory) : MOS6502MemoryCallBack(memory, via1PORTA) {}
		uint8_t Via1PortACallBack::executeRead() {
			// If this register is read, bit 7 is set
			return this->memory->silentReadWord(this->address);
		}

		void Via1PortACallBack::executeWrite(uint8_t value) {
			this->memory->silentWriteWord(via1PORTA, value);
			this->memory->silentWriteWord(via1PORTAMIRROR, value);
		}
	};
	Via1PortACallBack *via1PortACallBack = new Via1PortACallBack(this->memory);
	this->memory->addCallBack(via1PortACallBack);

	class Via1PortAMirrorCallBack : public MOS6502MemoryCallBack {
	public:
		Via1PortAMirrorCallBack(MOS6502Memory *memory) : MOS6502MemoryCallBack(memory, via1PORTAMIRROR) {}
		uint8_t Via1PortAMirrorCallBack::executeRead() {
			// If this register is read, bit 7 is set
			return this->memory->silentReadWord(this->address);
		}

		void Via1PortAMirrorCallBack::executeWrite(uint8_t value) {
			this->memory->silentWriteWord(via1PORTA, value);
			this->memory->silentWriteWord(via1PORTAMIRROR, value);
		}
	};
	Via1PortAMirrorCallBack *via1PortAMirrorCallBack = new Via1PortAMirrorCallBack(this->memory);
	this->memory->addCallBack(via1PortAMirrorCallBack);

	class Via2PortACallBack : public MOS6502MemoryCallBack {
	public:
		Via2PortACallBack(MOS6502Memory *memory) : MOS6502MemoryCallBack(memory, PORTA) {}
		uint8_t Via2PortACallBack::executeRead() {
			// If this register is read, bit 7 is set
			return this->memory->silentReadWord(this->address);
		}

		void Via2PortACallBack::executeWrite(uint8_t value) {
			this->memory->silentWriteWord(PORTA, value);
			this->memory->silentWriteWord(PORTAMIRROR, value);
		}
	};
	Via2PortACallBack *via2PortACallBack = new Via2PortACallBack(this->memory);
	this->memory->addCallBack(via2PortACallBack);

	class Via2PortAMirrorCallBack : public MOS6502MemoryCallBack {
	public:
		Via2PortAMirrorCallBack(MOS6502Memory *memory) : MOS6502MemoryCallBack(memory, PORTAMIRROR) {}
		uint8_t Via2PortAMirrorCallBack::executeRead() {
			// If this register is read, bit 7 is set
			return this->memory->silentReadWord(this->address);
		}

		void Via2PortAMirrorCallBack::executeWrite(uint8_t value) {
			this->memory->silentWriteWord(PORTA, value);
			this->memory->silentWriteWord(PORTAMIRROR, value);
		}
	};
	Via2PortAMirrorCallBack *via2PortAMirrorCallBack = new Via2PortAMirrorCallBack(this->memory);
	this->memory->addCallBack(via2PortAMirrorCallBack);
	
	
	// Create a callback that performs correct operations on writes to the interrupt enable register
	class InterruptEnableCallBack : public MOS6502MemoryCallBack {
	public:
		InterruptEnableCallBack(MOS6502Memory *memory) : MOS6502MemoryCallBack(memory, irqEnableAddress) {}
		uint8_t InterruptEnableCallBack::executeRead() {
			// If this register is read, bit 7 is set
			return this->memory->silentReadWord(this->address) | 0x80;
		}

		void InterruptEnableCallBack::executeWrite(uint8_t value) {
			// If the 7th bit is 1 during a write, each 1 in bits 0-6 sets the corresponding bit in IER
			if (value & 0x80) {
				this->memory->silentWriteWord(this->address, this->memory->silentReadWord(irqEnableAddress) | value);
			}
			// If the 7th bit is 0 during a write, each 1 in bits 0-6 clears the corresponding bit in IER
			else {
				this->memory->silentWriteWord(this->address, this->memory->silentReadWord(irqEnableAddress) & ~value);
			}

		}
	};
	InterruptEnableCallBack *interruptEnableCallBack = new InterruptEnableCallBack(this->memory);
	this->memory->addCallBack(interruptEnableCallBack);

	class InterruptFlagsCallBack : public MOS6502MemoryCallBack {
	public:
		InterruptFlagsCallBack(MOS6502Memory *memory) : MOS6502MemoryCallBack(memory, irqFlagsAddress) {}
		uint8_t InterruptFlagsCallBack::executeRead() {
			// If any bit in IFR is set then top bit must be set
			if (this->memory->silentReadWord(irqFlagsAddress) != 0) {
				return this->memory->silentReadWord(irqFlagsAddress) | 0x80;
			}
			else
				return this->memory->silentReadWord(irqFlagsAddress);
		}

		void InterruptFlagsCallBack::executeWrite(uint8_t value) {
			// individual  flag  bits may be cleared by writing a "1" into the appropriate bit of the IFR.
			this->memory->silentWriteWord(address, this->memory->readWord(this->address) & ~value);
		}
	};
	InterruptFlagsCallBack *interruptFlagsCallBack = new InterruptFlagsCallBack(this->memory);
	this->memory->addCallBack(interruptFlagsCallBack);

	// Create a callback that moves contents of low and high latch to timer on write to high latch
	class Timer1HighLatchCallBack : public MOS6502MemoryCallBack {
	public:
		Timer1HighLatchCallBack(MOS6502Memory *memory) : MOS6502MemoryCallBack(memory, timer1HighByteLatch) {}
		uint8_t Timer1HighLatchCallBack::executeRead() {
			return this->memory->silentReadWord(this->address);
		}

		void Timer1HighLatchCallBack::executeWrite(uint8_t value) {
			// On write to high byte latch, transfer latch contents to timer and activate the timer.
			this->memory->silentWriteWord(this->address, value);

			// Move contents
			this->memory->silentWriteDWord(timer1DAddress, memory->silentReadDWord(timer1LowByteLatch));

			// Enable timer
			this->memory->silentWriteWord(irqFlagsAddress, this->memory->silentReadWord(irqFlagsAddress) & ~0x40);
		}
	};
	Timer1HighLatchCallBack *highByteLatchCallBack = new Timer1HighLatchCallBack(this->memory);
	this->memory->addCallBack(highByteLatchCallBack);

	// Create a callback that resets timer 1 on read of low byte
	class Timer1LowByteCallBack : public MOS6502MemoryCallBack {
	public:
		Timer1LowByteCallBack(MOS6502Memory *memory) : MOS6502MemoryCallBack(memory, timer1DAddress) {}
		uint8_t Timer1LowByteCallBack::executeRead() {
			// On read of low byte, the timer interrupt enable is reset
			this->memory->silentWriteWord(irqFlagsAddress, this->memory->silentReadWord(irqFlagsAddress) & ~0x40);

			return this->memory->silentReadWord(this->address);
		}

		void Timer1LowByteCallBack::executeWrite(uint8_t value) {
			this->memory->silentWriteWord(this->address, value);
		}
	};
	Timer1LowByteCallBack *lowByteCallBack = new Timer1LowByteCallBack(this->memory);
	this->memory->addCallBack(lowByteCallBack);
	*/
}