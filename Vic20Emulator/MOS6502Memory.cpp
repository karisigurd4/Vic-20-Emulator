#include "MOS6502Memory.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdint.h>

MOS6502Memory::MOS6502Memory() : size(0xFFFF) {
	this->clear();
}

MOS6502Memory::~MOS6502Memory() {

}

uint16_t MOS6502Memory::getSize() {
	return this->size;
}

void MOS6502Memory::writeDWord(uint16_t location, uint16_t value) {
	if (this->isMemorySpaceAvailable(location)) {
		this->memory[location] = value;
		this->memory[location + 1] = value >> 8 & 0xFF;
	}	
}

void MOS6502Memory::writeWord(uint16_t location, uint8_t value) {
	if (this->isMemorySpaceAvailable(location))
		this->memory[location] = value;
}

uint16_t MOS6502Memory::readDWord(uint16_t location) {
	if (this->isMemorySpaceAvailable(location)) {
		return this->memory[location] | this->memory[location + 1] << 8;
	}		
}

uint8_t MOS6502Memory::readWord(uint16_t location) {
	if (this->isMemorySpaceAvailable(location))
		return this->memory[location];
}

void MOS6502Memory::silentWriteDWord(uint16_t location, uint16_t value) {
	this->memory[location] = value;
	this->memory[location + 1] = value >> 8 & 0xFF;
}

void MOS6502Memory::silentWriteWord(uint16_t location, uint8_t value) {
	this->memory[location] = value;
}

uint16_t MOS6502Memory::silentReadDWord(uint16_t location) {
	return this->memory[location] | this->memory[location + 1] << 8;
}

uint8_t MOS6502Memory::silentReadWord(uint16_t location) {
	return this->memory[location];
}

void MOS6502Memory::clear() {
	for (int i = 0; i < this->getSize(); i++)
		this->memory[i] = 0x00;
}

bool MOS6502Memory::load(const char location[], uint16_t atMemoryLocation) {
	// Create necessary variables to load the file
	FILE *pFile;
	long lSize;
	
	// Error check whether the file opens correctly
	if (!(pFile = fopen(location, "rb")))
		return false;

	// obtain file size
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// Allocate a buffer to store memory
	unsigned char *buffer = (unsigned char*)malloc(sizeof(unsigned char)*lSize);
	if (buffer == NULL)
		return false;

	// Read file contents into buffer
	if (fread(buffer, 1, lSize, pFile) != lSize)
		return false;

	// Move file data from buffer into memory
	for (int i = 0; i < lSize; i++)
		memory[i + atMemoryLocation] = buffer[i];

	// Close stream
	fclose(pFile);

	// Free buffer
	free(buffer);

	// Returns true
	return true;
}

void MOS6502Memory::save(const char file[], long int iSize) {
	FILE *pFile = fopen(file, "wb");
	int written = fwrite(memory, sizeof(unsigned char), iSize, pFile);
	fclose(pFile);
}

void MOS6502Memory::setUnavailableMemorySpace(MemorySpace unavailableMemory) {
	this->unavailableMemory.push_back(unavailableMemory);
}

bool MOS6502Memory::isMemorySpaceAvailable(uint16_t location) {
	// Check if memory is actually available
	bool available = true;
	for (uint16_t i = 0; i < this->unavailableMemory.size(); i++) {
		if (location >= this->unavailableMemory.at(i).lowAddress && location < this->unavailableMemory.at(i).highAddress) {
			available = false;
			break;
		}
	}

	return available;
}
