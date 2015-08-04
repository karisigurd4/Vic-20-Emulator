#ifndef _H_VIC20KEYMAP
#define _H_VIC20KEYMAP

#include <stdint.h>

class Vic20KeyMap {
public:
	// Via 2 Keyboard interface
	/*VIC-20 KEYBOARD MATRIX

	In my implementation, n = 0xF7EF

	      7f      bf      df      ef      f7      fb      fd      fe

	fe    2       q       CBM     Space   RunStop Control Lft_arr 1
	fd    4       e       s       z       Shift_L a       w       3
	fb    6       t       f       c       x       d       r       5
	f7    8       u       h       b       v       g       y       7
	ef    0       o       k       m       n       j       i       9
	df    -       @       :       .       ,       l       p       +
	bf    Home    Up_arr  =       Shift_R /       ;       *       GBP
	7f    F7      F5      F3      F1      Down    Right   Return  Del
	*/
	/*
	Converts ascii value of key press to a double word containing
	0x9120 - lower byte
	0x9121 - higher byte

	reference: http://vic-20.appspot.com/docs/vickey.txt
	*/
	static uint16_t getVicKey(SDL_Keycode key) {
		switch (key) {
		// FE row
		case SDLK_2: return 0x7FFE; break;
		case 'q': return 0xBFFE; break;
		case SDLK_RCTRL: return 0xDFFE; break; // CBM
		case SDLK_SPACE: return 0xEFFE; break; 
		//case SDLK_LCTRL: return 0xF7FE; break; // Runstop ?
		case SDLK_LCTRL: return 0xFBFE; break;
		case SDLK_LEFT: return 0xFDFE; break;
		case '1': return 0xFEFE; break;

		// FD row
		case '4': return 0x7FFD; break;
		case 'e': return 0xBFFD; break;
		case 's': return 0xDFFD; break;
		case 'z': return 0xEFFD; break;
		case SDLK_LSHIFT: return 0xF7FD; break;
		case 'a': return 0xFBFD; break;
		case 'w': return 0xFDFD; break;
		case '3': return 0xFEFD; break;

		// FB row
		case '6': return 0x7FFB; break;
		case 't': return 0xBFFB; break;
		case 'f': return 0xDFFB; break;
		case 'c': return 0xEFFB; break;
		case 'x': return 0xF7FB; break;
		case 'd': return 0xFBFB; break;
		case 'r': return 0xFDFB; break;
		case '5': return 0xFEFB; break;

		// F7 row
		case '8': return 0x7FF7; break;
		case 'u': return 0xBFF7; break;
		case 'h': return 0xDFF7; break;
		case 'b': return 0xEFF7; break;
		case 'v': return 0xF7F7; break;
		case 'g': return 0xFBF7; break;
		case 'y': return 0xFDF7; break;
		case '7': return 0xFEF7; break;

		// EF row
		case '0': return 0x7FEF; break;
		case 'o': return 0xBFEF; break;
		case 'k': return 0xDFEF; break;
		case 'm': return 0xEFEF; break;
		case 'n': return 0xF7EF; break;
		case 'j': return 0xFBEF; break;
		case 'i': return 0xFDEF; break;
		case '9': return 0xFEEF; break;

		// DF row
		case '-': return 0x7FDF; break;
		case '@': return 0xBFDF; break;
		case ':': return 0xDFDF; break;
		case '.': return 0xEFDF; break;
		case ',': return 0xF7DF; break;
		case 'l': return 0xFBDF; break;
		case 'p': return 0xFDDF; break;
		case '+': return 0xFEDF; break;

		// BF row
		case SDLK_HOME: return 0x7FBF; break;
		case SDLK_UP: return 0xBFBF; break;
		case SDLK_RALT: return 0xDFBF; break;
		case SDLK_RSHIFT: return 0xEFBF; break;
		case '/': return 0xF7BF; break;
		case ';': return 0xFBBF; break;
		case '*': return 0xFDBF; break;
		case SDLK_INSERT: return 0xFEBF; break;

		// 7F row
		case SDLK_F7: return 0x7F7F; break;
		case SDLK_F5: return 0xBF7F; break;
		case SDLK_F3: return 0xDF7F; break;
		case SDLK_F1: return 0xEF7F; break;
		case SDLK_DOWN: return 0xF77F; break;
		case SDLK_RIGHT: return 0xFB7F; break;
		case SDLK_RETURN: return 0xFD7F; break;
		case SDLK_BACKSPACE: return 0xFE7F; break;
		}
	}
};

#endif