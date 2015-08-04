#ifndef _H_MOS6561
#define	_H_MOS6561

#include <stdint.h>
#include <vector>

#include "IC.h"
#include "MOS6502Memory.h"
#include "Colour.h"
#include "Vic20ColourPalette.h"

/*
References, 
http://vic-20.appspot.com/docs/6561.txt
http://vic-20.appspot.com/docs/vicregs.txt
http://vic-20.appspot.com/docs/6561help.txt
*/

// Used by the video matrix to maintain current frame
struct MOS6561CharacterPointer {
	uint16_t pointer = 0;
	uint8_t colourPointer = 0;
	bool multiColour = false;

	MOS6561ColourMemory getColorName() {
		return MOS6561ColourMemory(colourPointer);
	}

	Colour getColourValues() {
		return Vic20ColourPalette::getColour(getColorName());
	}
};

// Typedef for readability
typedef std::vector<std::vector<MOS6561CharacterPointer>> MOS6561CharacterMatrix;

class MOS6561 : public IC {
public:
	MOS6561();
	~MOS6561();

	// Interface
	void tick();
	void initialize();
	void setMemory(MOS6502Memory *memory);

	// Getters, mostly depend on memory
	MOS6561CharacterMatrix getCharacterMatrix();
	uint16_t getCharacterPointer(uint8_t x, uint8_t y);
	bool isMultiColour(uint8_t x, uint8_t y);
	bool isLineReady();
	uint8_t getLine();
	void renderLine();
	bool isFrameReady();
	bool getSizeRegister();
	void renderFrame();
	uint16_t getCharacterRom();
	uint8_t getRows();
	uint8_t getColumns();
	Colour getBorderColour();
	Colour getBackgroundColour();
	Colour getAuxiliaryColour();
private:
	// Memory space reference
	MOS6502Memory *memory;

	// Video matrix that contains character set pointers, rows x columns
	MOS6561CharacterMatrix characterMatrix;

	// Used as counters
	uint8_t curCol, curRow;

	// Determines if a full frame has been drawn
	bool frameReady;
	bool lineReady;

	// Denotes which line has been drawn
	uint8_t line;

	// const definitions for easy access
	static const uint16_t colourRegister = 0x900F;
	static const uint16_t auxiliaryColourRegister = 0x900E;
	static const uint16_t screenColourRegister = 0x9005;
	static const uint16_t screenLowRegister = 0x9002;
	static const uint16_t rasterRowsSizeRegister = 0x9003;
	static const uint16_t columnsRegister = 0x9002;
	static const uint8_t cyclesPerScanline = 71;
};

#endif