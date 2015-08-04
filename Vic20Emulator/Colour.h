#ifndef _H_Colour
#define _H_Colour

enum MOS6561ColourMemory{ Black = 0, White = 1, Red = 2, Cyan = 3, Purple = 4, Green = 5, Blue = 6, Yellow = 7 
						, Orange = 8, LightOrange = 9, Pink = 10, LightCyan = 11, LightPurple = 12
						, LightGreen = 13, LightBlue = 14, LightYellow = 15 };

struct Colour {
	Colour() : r(0), g(0), b(0) {}
	Colour(float r, float g, float b) : r(r), g(g), b(b) {}

	float r, g, b;
};

#endif