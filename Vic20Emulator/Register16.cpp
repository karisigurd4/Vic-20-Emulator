#include "Register16.h"
#include <stdint.h>

Register16::Register16() : value(0) {

}

Register16::~Register16() {

}

void Register16::write(uint16_t value) {
	this->value = value;
}

uint16_t Register16::read() {
	return this->value;
}

void Register16::setBit(uint16_t bit) {
	this->value |= 1 << bit;
}

void Register16::clearBit(uint16_t bit) {
	this->value &= ~(1 << bit);
}

bool Register16::isBitSet(uint16_t bit) {
	if (((this->value >> bit) & 1) == 1) {
		return true;
	}
	else {
		return false;
	}
}
