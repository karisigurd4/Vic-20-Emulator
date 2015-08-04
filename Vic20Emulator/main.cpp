//Using SDL, SDL OpenGL, standard IO, and, strings
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL\GLU.h>
#include <stdio.h>
#include <string>

// Additional IO
#include <iostream>
#include "GamesListReader.h"

// Vic 20 emulation includes
#include "MOS6502.h"
#include "MOS6561.h"
#include "MOS6522.h"
#include "Vic20KeyMap.h"
#include "Vic20Rom.h"

// Timers
#include <chrono>

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes matrices and clear color
bool initGL();

//Input handler
void handleKeys(unsigned char key, int x, int y);

// Loads a program into the vic memory
void loadProgram();

// Renders the startup screen
void renderStartup();

// Renders a string using character memory
void renderString(float x, float y, std::string string);

//Renders vic graphics memory to screen
void renderVic();

// Draws a character from vic memory to the screen
void drawCharacter(uint16_t pointer, bool multiColour, MOS6561ColourMemory colour, float x, float y);

// Polls input
void pollInput(SDL_Event &e, bool &quit);

// Makes a single pixel on the screen
void makePixel(float x, float y, bool multicolour);

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

// Start screen list index
int selectedIndex = 0;

// Rendering constants
float columnRatio, rowRatio, pixelXSize, pixelYSize;
float screenOffsetX = 0.23, screenOffsetY = 0.19;

// Rendering optimization
float screenCoords[30][30][2];
int oldColumn, oldRow;

// Rendering timing
clock_t lastRender;

// Emulation objects
MOS6502 mos;
MOS6561 mos6561;
MOS6522 mos6522;
MOS6502Memory *memory = new MOS6502Memory();
clock_t oldTime = clock();
bool romLoaded = false;
bool emulationActive = false;

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		//Enable text input
		SDL_StartTextInput();

		// Remove comments to change memory configuration
		//memory->setUnavailableMemorySpace(MemorySpace(0x400, 0x1000)); // 3k expansion ram area
		//memory->setUnavailableMemorySpace(MemorySpace(0x2000, 0x4000)); // 8k expansion ram block 1
		//memory->setUnavailableMemorySpace(MemorySpace(0x4000, 0x6000)); // 8k expansion ram block 2
		//memory->setUnavailableMemorySpace(MemorySpace(0x6000, 0x8000)); // 8k expansion ram block 2

		// Initialize emulation objects
		mos.setMemory(memory);
		mos6561.setMemory(memory);
		mos6522.setMemory(memory);
		mos6522.setCpu(&mos);
		mos6522.initialize();
		mos.initialize();

		// Initialize all the things
		loadProgram();

		// Show startup screen
		emulationActive = false;
		
		// Execute boot sequence before doing much else
		while (mos.getCycles() < 20000000)
			mos.executeInstruction();

		//While application is running
		while (!quit)
		{
			// Reset keyboard input memory before Toggling
			mos6522.setKeyPressed(0x0000);

			//Handle events on queue
			pollInput(e, quit);

			// If column or row count changes, we need to update the screeen coordinate arrays
			if (oldColumn != mos6561.getColumns() || oldRow != mos6561.getRows()) {		
				columnRatio = 1.535 / mos6561.getColumns();
				rowRatio = 1.61 / mos6561.getRows();

				pixelXSize = 8 * mos6561.getColumns();
				pixelYSize = 8.5 * mos6561.getColumns();

				// Generate screen coordinate table
				for (int i = 0; i < 30; i++) {
					for (int x = 0; x < 30; x++) {
						screenCoords[i][x][0] = -1 + x * columnRatio + screenOffsetX + 0.005;
						screenCoords[i][x][1] = -1 + i * rowRatio + screenOffsetY + 0.01;
					}
				}

				oldColumn = mos6561.getColumns();
				oldRow = mos6561.getRows();
			}

			if (emulationActive) {		
				// Execute one scanline worth of cpu time
				do  {
					
					// bind emulation to clock speed

					// Cpu clock is four times less than 6561 clock
					mos.executeInstruction();

					// The via runs at twice the speed of the cpu
					mos6522.tick();

					// Continue updating video memory
					mos6561.tick();		

					if (mos6561.isLineReady()) {
						// Draw current line
						renderVic();

						// Clear new line flag
						mos6561.renderLine();
					}
				} while ((!mos6561.isFrameReady()));

				// Swap buffers if frame ready
				if (mos6561.isFrameReady()) {
					glEnd();

					glEnable(GL_LIGHTING);
					glEnable(GL_TEXTURE_2D);

					SDL_GL_SwapWindow(gWindow);

					mos6561.renderFrame();

					//Clear color buffer
					glClear(GL_COLOR_BUFFER_BIT);
					glDisable(GL_LIGHTING);
					glDisable(GL_TEXTURE_2D);

					// set border color
					glClearColor(mos6561.getBorderColour().r
						, mos6561.getBorderColour().g
						, mos6561.getBorderColour().b,
						1.0f);

					// Set for screen box colour
					glColor3f(mos6561.getBackgroundColour().r
						, mos6561.getBackgroundColour().g
						, mos6561.getBackgroundColour().b);

					//Draw screen box
					glBegin(GL_QUADS);
					glVertex2f(screenOffsetX - 1, screenOffsetY - 1);
					glVertex2f(1 - screenOffsetX, screenOffsetY - 1);
					glVertex2f(1 - screenOffsetX, 1 - screenOffsetY);
					glVertex2f(screenOffsetX - 1, 1 - screenOffsetY);
				}
				
			}
			else {
				// If emulation isn't active, render startup screen
				renderStartup();

				//Update screen
				SDL_GL_SwapWindow(gWindow);
			}


		}

		//Disable text input
		SDL_StopTextInput();
	}	

	//Free resources and close SDL
	close();

	// Delete memory pointer
	delete memory;

	return 0;
}

void pollInput(SDL_Event &e, bool &quit) {
	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			quit = true;
		}
		//Handle keypress with current mouse position
		else if (e.type == SDL_KEYDOWN)
		{
			// Handle some keys separately
			switch (e.key.keysym.sym) {
			case SDLK_LSHIFT:
				mos6522.setShiftPressed(true);
				break;
			case SDLK_LCTRL:
				emulationActive = !emulationActive;
				break;
			case SDLK_LALT:
				for (int i = 0; i < 0xFFFF; i++) {
					int b = rand() % 600;
					if (b == 0) {
						int a = rand() % 256;
						memory->writeWord(i, a);
					}
				}

				break;
			case SDLK_RCTRL:
				mos6522.setCbmPressed(true);
				break;
			case SDLK_ESCAPE:
				quit = true;
				break;
			case SDLK_KP_8:
				if (!emulationActive) {
					selectedIndex = (selectedIndex - 1) % GamesListReader::getSize();
					if (selectedIndex < 0) selectedIndex = GamesListReader::getSize() - 1;
				}
				else {
					mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Up, true);
				}
				break;
			case SDLK_KP_5:
				if (!emulationActive) {
					selectedIndex = (selectedIndex + 1) % GamesListReader::getSize();
				}
				else {
					mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Down, true);
				}
				break;
			case SDLK_KP_4:
				mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Left, true);
				break;
			case SDLK_KP_6:
				mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Right, true);
				break;
			case SDLK_KP_0:
				if (!emulationActive) {
					loadProgram();
				}
				else {
					mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Fire, true);
				}
				break;
			default:
				mos6522.setKeyPressed(Vic20KeyMap::getVicKey(e.key.keysym.sym));
				break;
			}
		}
		else if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym) {
			case SDLK_LSHIFT:
				mos6522.setShiftPressed(false);
				break;
			case SDLK_RCTRL:
				mos6522.setCbmPressed(false);
				break;
			case SDLK_KP_8:
				mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Up, false);
				break;
			case SDLK_KP_5:
				mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Down, false);
				break;
			case SDLK_KP_4:
				mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Left, false);
				break;
			case SDLK_KP_6:
				mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Right, false);
				break;
			case SDLK_KP_0:
				mos6522.setJoyStickPressed(MOS6522::Vic20JoyStickButton::Fire, false);
				break;
			}
		}
	}
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	SDL_Init(SDL_INIT_VIDEO);

	//Use OpenGL 2.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	//Create window
	gWindow = SDL_CreateWindow("Derp Vic 20", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	//Create context
	gContext = SDL_GL_CreateContext(gWindow);
			
	//Use Vsync
	//SDL_GL_SetSwapInterval(1);
			
	//Initialize OpenGL
	initGL();

	return success;
}


bool initGL()
{
	GLenum error = GL_NO_ERROR;

	//Initialize Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Flip screen
	glScalef(1.0, -1.0, 1.0);

	//Initialize Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	return true;
}

void handleKeys(unsigned char key, int x, int y)
{
	mos6522.setKeyPressed(Vic20KeyMap::getVicKey(key));
}

void loadProgram() {
	// Clear memory and reload kernal, character rom and basic
	memory->clear();
	memory->load("Kernal/characters.901460-03.bin", 0x8000);
	memory->load("Kernal/basic.901486-01.bin", 0xC000);
	memory->load("Kernal/kernal.901486-07.bin", 0xE000);

	// Load program
	Vic20Rom program;
	for (int i = 0; i < 2; i++) {
		// Convert path to char*
		char *path = new char[GamesListReader::getPath(selectedIndex, i).length()];
		std::strcpy(path, GamesListReader::getPath(selectedIndex, i).c_str());

		program.load(path, 0);
		// Plug rom into the emulation memory space
		for (int i = 0; i < program.getEndAddress() - program.getStartAddress(); i++) {
			memory->writeWord(program.getLoadAddress() + i, program.readWord(i));
		}
	}

	// Set basic memory pointers
	memory->writeDWord(0x2B, program.getStartAddress());
	memory->writeDWord(0x2D, program.getStartAddress());
	memory->writeDWord(0x2F, program.getEndAddress());
	memory->writeDWord(0x31, program.getEndAddress());
	memory->writeDWord(0x33, program.getEndAddress());
	memory->writeDWord(0x37, 0xFFFF);
	memory->writeDWord(0xAE, program.getEndAddress());

	// Reset cpu
	mos.initialize();

	// Enable emulation
	emulationActive = true;

	// Execute boot sequence before doing much else
	while (mos.getCycles() < 20000)
		mos.executeInstruction();
}

void makePixel(float x, float y, bool multicolour) {
	float xSize = pixelXSize;

	if (multicolour)
		xSize /= 2;

	glVertex2f(x + -1.0 / xSize, y + -1.0 / pixelYSize);
	glVertex2f(x + 1.0 / xSize, y + -1.0 / pixelYSize);
	glVertex2f(x + 1.0 / xSize, y + 1.0 / pixelYSize);
	glVertex2f(x + -1.0 / xSize, y + 1.0 / pixelYSize);
}

void drawCharacter(uint16_t pointer, bool multiColour, MOS6561ColourMemory colour, float x, float y) {
	// Determine character size based on size register of 6561
	uint8_t size = mos6561.getSizeRegister() ? 2 : 8;

	// Get next bytes in memory from pointer
	uint8_t *characterBytes = new uint8_t[8];
	for (int i = 0; i < 8; i++) {
		characterBytes[i] = memory->readWord(pointer + i);
	}

	if (!multiColour) {
		// Set character colour
		glColor3f(Vic20ColourPalette::getColour(colour).r
			, Vic20ColourPalette::getColour(colour).g
			, Vic20ColourPalette::getColour(colour).b);

		// Render top to bottom
		for (int i = 0; i < 8; i++) {
			for (int a = 0; a < 8; a++) {
				if ((characterBytes[i] << a) & 0x80) {
					makePixel(x + a * 0.0092, y + i * 0.0087, false);
				}
			}
		}
	}
	else {
		// Render top to bottom
		for (int i = 0; i < 8; i++) {
			for (int a = 0; a < 8; a += 2) {
				if ((characterBytes[i] << a) & 0xC0) {
					// Set colour
					switch (((characterBytes[i] << a) & 0xC0) >> 6) {
					case 0:
						glColor3f(mos6561.getBackgroundColour().r
							, mos6561.getBackgroundColour().g
							, mos6561.getBackgroundColour().b);
						break;
					case 1:
						glColor3f(mos6561.getBorderColour().r
							, mos6561.getBorderColour().g
							, mos6561.getBorderColour().b);
						break;
					case 2: 
						// Set character colour
						glColor3f(Vic20ColourPalette::getColour(colour).r
							, Vic20ColourPalette::getColour(colour).g
							, Vic20ColourPalette::getColour(colour).b);
						break;
					case 3: 
						glColor3f(mos6561.getAuxiliaryColour().r
							, mos6561.getBorderColour().g
							, mos6561.getBorderColour().b);
						break;
					}

					makePixel(x + a * 0.0092, y + i * 0.0087, true);
				}
			}
		}
	}

	// Delete character array after using it
	delete[] characterBytes;
}

void renderVic()
{
	for (int x = 0; x < mos6561.getColumns(); x++) {
			drawCharacter(mos6561.getCharacterPointer(x, mos6561.getLine())
				, mos6561.isMultiColour(x, mos6561.getLine())
				, mos6561.getCharacterMatrix().at(mos6561.getLine()).at(x).getColorName()
				, screenCoords[mos6561.getLine()][x][0], screenCoords[mos6561.getLine()][x][1]);
	}
}

void renderStartup() {
	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	// set border color
	glClearColor(Vic20ColourPalette::getColour(MOS6561ColourMemory::Blue).r
		, Vic20ColourPalette::getColour(MOS6561ColourMemory::Blue).g
		, Vic20ColourPalette::getColour(MOS6561ColourMemory::Blue).b,
		1.0f);

	// Set for screen box colour
	glColor3f(Vic20ColourPalette::getColour(MOS6561ColourMemory::Black).r
		, Vic20ColourPalette::getColour(MOS6561ColourMemory::Black).g
		, Vic20ColourPalette::getColour(MOS6561ColourMemory::Black).b);

	//Draw screen box
	glBegin(GL_QUADS);
	glVertex2f(screenOffsetX - 1, screenOffsetY - 1);
	glVertex2f(1 - screenOffsetX, screenOffsetY - 1);
	glVertex2f(1 - screenOffsetX, 1 - screenOffsetY);
	glVertex2f(screenOffsetX - 1, 1 - screenOffsetY);

	// Render top bar text
	glColor3f(Vic20ColourPalette::getColour(MOS6561ColourMemory::White).r
		, Vic20ColourPalette::getColour(MOS6561ColourMemory::White).g
		, Vic20ColourPalette::getColour(MOS6561ColourMemory::White).b);

	renderString(screenOffsetX - 1 + 0.05, screenOffsetY - 1 + 0.05, "VIC-20 Emulator");

	for (int i = 0; i < 14; i++) {
		if (i + selectedIndex == selectedIndex) {
			renderString(screenOffsetX - 1 + 0.05, screenOffsetY - 1 + 0.20 + i * 0.10, ">");
		}
		
		renderString(screenOffsetX - 1 + 0.15, screenOffsetY - 1 + 0.20 + i * 0.10, GamesListReader::getName(i + selectedIndex));
	}
		
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}

void renderString(float x, float y, std::string string) {
	for (int i = 0; i < string.length(); i++) {
		if (string.at(i) != ' ') {
			if (string.at(i) > 64 && string.at(i) <= 96) {
				drawCharacter(0x8000 + (string.at(i) * 8) - 512, false, MOS6561ColourMemory::White, x + (0.07 * i), y);
			}
			else if (string.at(i) > 96) {
				drawCharacter(0x8000 + (string.at(i) * 8) - 768, false, MOS6561ColourMemory::White, x + (0.07 * i), y);
			}
			else {
				drawCharacter(0x8000 + (string.at(i) * 8), false, MOS6561ColourMemory::White, x + (0.07 * i), y);
			}
		}
	}
}

void close()
{
	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}