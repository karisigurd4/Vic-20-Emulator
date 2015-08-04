#include "MOS6561.h"
#include <math.h>

#include <iostream>

MOS6561::MOS6561() : IC(), curCol(0), curRow(0), lineReady(false), frameReady(true) {
	// Initialize character vectors
	for (int i = 0; i < 36; i++) {
		// Create a new row vector to push 
		std::vector<MOS6561CharacterPointer> newRow;
		this->characterMatrix.push_back(newRow);
		for (int x = 0; x < 33; x++) {
			// Push a new character pointer on the current row vector
			MOS6561CharacterPointer newPointer;
			this->characterMatrix.at(i).push_back(newPointer);
		}
	}

	// Set clock speed
	this->setClockSpeed(1108000);
}

MOS6561::~MOS6561() {

}

void MOS6561::setMemory(MOS6502Memory *memory) {
	this->memory = memory;
}

bool MOS6561::isLineReady() {
	return this->lineReady;
}

/**
Returns the current row that has been fully updated
*/
uint8_t MOS6561::getLine() {
	return this->line;
}

void MOS6561::renderLine() {
	this->lineReady = false;
}

void MOS6561::renderFrame() {
	this->frameReady = false;
}

bool MOS6561::isFrameReady() {
	return this->frameReady;
}

bool MOS6561::getSizeRegister() {
	return this->memory->readWord(rasterRowsSizeRegister) & 0x1;
}

void MOS6561::tick() {
	// Increment raster counter 
	uint16_t raster = this->memory->readWord(0x9004) << 1 | this->memory->readWord(0x9003) >> 7;
	if (cycles % cyclesPerScanline == 0) {
		if (raster <= 312) {
			raster += 1;
		}
		else {
			raster = 0;	
		}

		this->memory->writeWord(0x9003, (raster & 0x1) << 7 | (memory->readWord(0x9003) & 0x7F));
		this->memory->writeWord(0x9004, raster >> 1);
	}

	// If not blanking, update screen
	if (raster > 21 && raster < this->getRows() * 8) {		
		// Update maintained video memory every eight raster lines 
		if (cycles % (cyclesPerScanline * 8) == 0) {
			for (int i = 0; i < this->getColumns(); i++) {
				// Read the next character pointer from memory and assign to our own character pointer matrix
				uint16_t screenPage = ((this->memory->readWord(0x9005) & 0xF0) << 6 | (this->memory->readWord(0x9002) & 0x80) << 2) & ~0x2000;
				this->characterMatrix.at(this->curRow).at(this->curCol).pointer = this->memory->readWord(screenPage + (this->curRow * this->getColumns()) + (this->curCol));

				// Assign colour value to character matrix by reading colour memory
				uint16_t colourPage = 0x9400 + ((this->memory->readWord(0x9002) & 0x80) << 2);
				this->characterMatrix.at(curRow).at(curCol).colourPointer = (this->memory->readWord(colourPage + (this->curRow * this->getColumns()) + this->curCol) & 0x7);

				// Set multicolour attribute of character pointer
				if (this->memory->readWord(colourPage + (this->curRow * this->getColumns()) + this->curCol) & 0x8)
					this->characterMatrix.at(curRow).at(curCol).multiColour = true;
				else
					this->characterMatrix.at(curRow).at(curCol).multiColour = false;
				
				// Increment column for next tick
				this->curCol += 1;

				// End of line reached
				if (this->curCol >= this->getColumns()) {
					this->curCol = 0;
					line = this->curRow++;
					lineReady = true;
				}

				// End of screen reached
				if (this->curRow >= this->getRows()) {
					this->curRow = 0;
					frameReady = true;
				}
			}
		}
	}

	// Increment cycle counter
	this->cycles += 1;

}

bool MOS6561::isMultiColour(uint8_t x, uint8_t y) {
	return this->characterMatrix.at(y).at(x).multiColour;
}

MOS6561CharacterMatrix MOS6561::getCharacterMatrix() {
	return this->characterMatrix;
}

Colour MOS6561::getBorderColour() {
	return Vic20ColourPalette::getColour(MOS6561ColourMemory(this->memory->readWord(colourRegister) & 0x7));
}

Colour MOS6561::getBackgroundColour() {
	return Vic20ColourPalette::getColour(MOS6561ColourMemory((this->memory->readWord(colourRegister) & 0xF0) >> 4));
}

Colour MOS6561::getAuxiliaryColour() {
	return Vic20ColourPalette::getColour(MOS6561ColourMemory((this->memory->readWord(auxiliaryColourRegister) & 0xF0) >> 4));
}

uint16_t MOS6561::getCharacterRom() {
	// Which character memory space is used depends on the low nibble of register @ 0x9005
	uint8_t cm = (memory->readWord(0x9005) & 0xF);

	switch (cm) {
	case 0:return 0x8000;break;
	case 1:return 0x8400;break;
	case 2:return 0x8800;break;
	case 3:return 0x8C00;break;
	case 4:return 0x0000;break;
	case 12:return 0x1000;break;
	case 13:return 0x1400;break;
	case 14:return 0x1800;break;
	case 15:return 0x1C00;break;
	}
}

uint16_t MOS6561::getCharacterPointer(uint8_t x, uint8_t y) {
	// Handle wraparound to 0x8000 
	uint16_t offset = 0;
	uint16_t pointer = (this->getCharacterRom()) + (this->getCharacterMatrix().at(y).at(x).pointer * 8);

	// If it's writing into +0x2000 from 0x1C000 probably
	if (pointer > 0x2000 && pointer < 0x3000)
		offset = 0x6000;

	return pointer + offset;
}

uint8_t MOS6561::getRows() {
	return (this->memory->readWord(this->rasterRowsSizeRegister) & 0x7E) >> 1;
}

uint8_t MOS6561::getColumns() {
	return this->memory->readWord(this->columnsRegister) & 0x7F;
}