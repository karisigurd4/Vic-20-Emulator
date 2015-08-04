#ifndef _H_VIC20ROM
#define _H_VIC20ROM

#include "MOS6502Memory.h"

class Vic20Rom : public MOS6502Memory {
public:
	Vic20Rom();
	~Vic20Rom();

	// Override load method
	bool load(const char location[], uint16_t atMemoryLocation);

	// Getters
	uint16_t getLoadAddress();
	uint16_t getStartAddress();
	uint16_t getEndAddress();

private:
	// Stored in first two locations of rom
	uint16_t loadAddress;
	
	// Store start and end addresses of rom
	uint16_t startAddress;
	uint16_t endAddress;
};

#endif