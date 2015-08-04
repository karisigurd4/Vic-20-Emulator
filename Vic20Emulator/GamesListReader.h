#ifndef _H_GAMESLISTREADER
#define _H_GAMESLISTREADER

#include <iostream>
#include <fstream>
#include <string>

class GamesListReader {
public:
	// Returns the name of the game at the index provided
	static std::string getName(int index) {
		std::ifstream gamesList("Games/Games.txt");

		// Seems like a hacky way of doing this
		std::string fullPath;
		for (int i = 0; i <= index; i++) {
			std::getline(gamesList, fullPath);
		}

		// Extract name
		std::string name = fullPath.substr(0, fullPath.find('/'));

		return name;
	}

	// Returns the full path of the specified game index
	static std::string getPath(int index, int rom) {
		std::ifstream gamesList("Games/Games.txt");

		// Seems like a hacky way of doing this
		std::string fullPath;
		for (int i = 0; i <= index; i++) {
			std::getline(gamesList, fullPath);
		}

		if (rom == 0) {
			return "Games/" + fullPath.substr(0, fullPath.find('#'));
		}
		else if (rom == 1) {
			return "Games/" + fullPath.substr(fullPath.find('#') + 1, fullPath.length());
		}
		
	}

	// Returns how many games are specified in the game list file
	static int getSize() {
		int lines = 0;
		std::string line;

		std::ifstream gamesList("Games/Games.txt");
		while (std::getline(gamesList, line))
			lines++;

		// Returns the number of counted lines
		return lines;
	}
};

#endif