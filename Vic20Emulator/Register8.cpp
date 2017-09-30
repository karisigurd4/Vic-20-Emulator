#include "Register8.h"
#include <stdint.h>

Register8::Register8() : value(0) {

}

Register8::~Register8() {

}

void Register8::write(uint16_t value) {
	this->value = value & 0x00FF;
}

uint16_t Register8::read() {
	return this->value;
}

void Register8::setBit(uint16_t bit) {
	this->value |= 1 << bit;
}

void Register8::clearBit(uint16_t bit) {
	this->value &= ~(1 << bit);
}

bool Register8::isBitSet(uint16_t bit) {
	if (((this->value >> bit) & 1) == 1) {
		return true;
	}
	else {
		return false;
	}
}
