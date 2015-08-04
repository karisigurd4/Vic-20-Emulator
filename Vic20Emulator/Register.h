#ifndef _H_REGISTER
#define _H_REGISTER

#include <stdint.h>

/*
Defines interface for register classes.
*/
class Register {
public:
	// Constructors
	Register() {};
	~Register() {};

	// Interface
	void write(uint16_t);
	uint16_t read();
	void setBit(uint16_t bit);
	void clearBit(uint16_t bit);
	bool isBitSet(uint16_t bit);
};

#endif