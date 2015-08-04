#ifndef _H_REGISTER16
#define _H_REGISTER16

#include "Register.h";

class Register16 : Register {
public:
	Register16();
	~Register16();

	// Interface
	void write(uint16_t);
	uint16_t read();
	void setBit(uint16_t bit);
	void clearBit(uint16_t bit);
	bool isBitSet(uint16_t bit);

private:
	uint16_t value;
};

#endif