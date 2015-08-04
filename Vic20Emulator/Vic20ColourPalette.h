#ifndef _H_VIC20COLOURPALETTE
#define _H_VIC20COLOURPALETTE

#include "Colour.h"

/* Seems cleanest to leave this as a class with a static method */
class Vic20ColourPalette {
public:
	static Colour getColour(MOS6561ColourMemory colour) {
		switch (colour) {
		case 0: return Colour(0.0f, 0.0f, 0.0f); break;
		case 1: return Colour(1.0f, 1.0f, 1.0f);  break;
		case 2: return Colour(convertToGlFloat(182), convertToGlFloat(31), convertToGlFloat(33));  break;
		case 3: return Colour(convertToGlFloat(77), convertToGlFloat(240), convertToGlFloat(255));  break;
		case 4: return Colour(convertToGlFloat(180), convertToGlFloat(63), convertToGlFloat(255));  break;
		case 5: return Colour(convertToGlFloat(68), convertToGlFloat(226), convertToGlFloat(55));  break;
		case 6: return Colour(convertToGlFloat(15), convertToGlFloat(87), convertToGlFloat(247));  break;
		case 7: return Colour(convertToGlFloat(220), convertToGlFloat(215), convertToGlFloat(27));  break;
		case 8: return Colour(convertToGlFloat(202), convertToGlFloat(84), convertToGlFloat(0));  break;
		case 9: return Colour(convertToGlFloat(233), convertToGlFloat(176), convertToGlFloat(114));  break;
		case 10: return Colour(convertToGlFloat(231), convertToGlFloat(146), convertToGlFloat(147));  break;
		case 11: return Colour(convertToGlFloat(154), convertToGlFloat(247), convertToGlFloat(253));  break; 
		case 12: return Colour(convertToGlFloat(224), convertToGlFloat(159), convertToGlFloat(255));  break;
		case 13: return Colour(convertToGlFloat(143), convertToGlFloat(228), convertToGlFloat(147));  break;
		case 14: return Colour(convertToGlFloat(130), convertToGlFloat(144), convertToGlFloat(255));  break;
		case 15: return Colour(convertToGlFloat(229), convertToGlFloat(222), convertToGlFloat(133));  break;
		default: return Colour(0, 0, 0); break;
		}
	}

	// Helper
	static float convertToGlFloat(float x) {
		return x / 255;
	}
};

#endif 