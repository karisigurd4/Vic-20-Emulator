#ifndef _H_MOS6502MEMORY
#define _H_MOS6502MEMORY

#include <stdint.h>
#include <vector>

// Used to restrict access to certain space of memory
struct MemorySpace {
	MemorySpace() {};
	MemorySpace(uint16_t l, uint16_t h) : lowAddress(l), highAddress(h) {}

	uint16_t lowAddress;
	uint16_t highAddress;
};

class MOS6502Memory {
public:
	// Constructors
	MOS6502Memory();
	~MOS6502Memory();

	// Getters / Setters
	uint16_t getSize();

	// Interface
	uint8_t readWord(uint16_t location);
	uint16_t readDWord(uint16_t location);
	void writeWord(uint16_t location, uint8_t value);
	void writeDWord(uint16_t location, uint16_t value);
	void clear();

	// Silent methods operate on memory directly without regard to available memory space 
	uint8_t silentReadWord(uint16_t location);
	uint16_t silentReadDWord(uint16_t location);
	void silentWriteWord(uint16_t location, uint8_t value);
	void silentWriteDWord(uint16_t location, uint16_t value);

	// Controls access to certain parts of memory
	void setUnavailableMemorySpace(MemorySpace unavailableMemory);
	inline bool isMemorySpaceAvailable(uint16_t location);

	// I/O
	bool load(const char location[], uint16_t atMemoryLocation);
	void save(const char file[], long int iSize);

protected:
	uint8_t memory[0xFFFF];
	uint16_t size;

	// Stores restricted memory space
	std::vector<MemorySpace> unavailableMemory;

};

#endif