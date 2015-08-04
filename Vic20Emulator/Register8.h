#ifndef _H_REGISTER8
#define _H_REGISTER8

#include "Register.h";

class Register8 : Register {
public:
	Register8();
	~Register8();

	// Interface
	void write(uint16_t);
	uint16_t read();
	void setBit(uint16_t bit);
	void clearBit(uint16_t bit);
	bool isBitSet(uint16_t bit);

private:
	uint8_t value;
};

#endif