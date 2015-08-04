#ifndef _H_MOS6522
#define _H_MOS6522

#include "IC.h"
#include "MOS6502Memory.h"
#include "MOS6502.h"

/*
Notes,
This single object will serve as emulation 
for both VIA chips in the VIC-20.

References,
http://vic-20.appspot.com/docs/viausage.txt
http://vic-20.appspot.com/docs/vickey.txt
*/
class MOS6522 : public IC {
public:
	MOS6522();
	~MOS6522();

	// Joystick 
	enum Vic20JoyStickButton { Fire = 0, Up = 1, Down = 2, Left = 3, Right = 4 };

	// Via joystick interface
	void setJoyStickPressed(Vic20JoyStickButton button, bool state);

	// Via 2 Keyboard interface
	void setKeyPressed(uint16_t key);
	void setShiftPressed(bool state);
	void setCbmPressed(bool state);

	// Interface
	void tick();
	void setMemory(MOS6502Memory *memory);
	void setCpu(MOS6502 *mos6502);
	void initialize();

private:
	// Organize operations into separate methods for cleaner code
	void tickTimers();
	void tickInput();
	void createCallBacks();

	// Memory space reference
	MOS6502Memory *memory;

	// Cpu reference
	MOS6502 *mos6502;

	// Via 1 & 2 Joystick
	bool joyStick[5];

	// Via 2 Keyboard 
	uint16_t keyPressed;
	bool shiftPressed;
	bool cbmPressed; 

	// via ddrs
	static const uint16_t via1PortADDR = 0x9113;
	static const uint16_t via2PortADDR = 0x9123;
	static const uint16_t via2PortBDDR = 0x9122; 

	// Via 1 Joystick addresses
	static const uint16_t via1PORTB = 0x9110;
	static const uint16_t via1PORTA = 0x9111;
	static const uint16_t via1PORTAMIRROR = 0x911F;
	static const uint8_t joyStickFireBit = 0x10;
	static const uint8_t joyStickUpBit = 0x2;
	static const uint8_t joyStickDownBit = 0x4;
	static const uint8_t joyStickLeftBit = 0x8;
	static const uint8_t joyStickRightBit = 0x80;

	// Via 2 Keyboard addresses
	static const uint16_t PORTB = 0x9120;
	static const uint16_t PORTA = 0x9121;
	static const uint16_t PORTAMIRROR = 0x912F;

	// Via 2 Timer addresses
	static const uint16_t timer1LowByteLatch = 0x9124;
	static const uint16_t timer1HighByteLatch = 0x9125;
	static const uint16_t timer1DAddress = 0x9126;	
	static const uint16_t timer2DAddress = 0x9118;
	static const uint16_t timer2LowByteLatch = 0x9128;
	static const uint16_t timer2HighByteLatch = 0x9129;

	// Via 2 Registers
	static const uint16_t auxControlAddress = 0x912B;
	static const uint16_t irqFlagsAddress = 0x912D;
	static const uint16_t irqEnableAddress = 0x912E;
};

#endif