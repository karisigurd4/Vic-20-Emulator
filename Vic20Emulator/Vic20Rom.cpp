#include "Vic20Rom.h"

Vic20Rom::Vic20Rom() : MOS6502Memory() {

}

Vic20Rom::~Vic20Rom() {

}

uint16_t Vic20Rom::getLoadAddress() {
	return this->loadAddress;
}

uint16_t Vic20Rom::getStartAddress() {
	return this->startAddress;
}

uint16_t Vic20Rom::getEndAddress() {
	return this->endAddress;
}

bool Vic20Rom::load(const char location[], uint16_t atMemoryLocation) {
	// Set start address property
	this->startAddress = atMemoryLocation;

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

	// Find load address stored in first two bytes of rom
	this->loadAddress = buffer[0] | buffer[1] << 8;

	// Move file data from buffer into memory
	int i = 0;
	for (; i < lSize - 2; i++)
		memory[i + atMemoryLocation] = buffer[i + 2];

	// Set end address property
	this->endAddress = i;

	// Close stream
	fclose(pFile);

	// Free buffer
	free(buffer);

	// Returns true
	return true;
}