#ifndef CHIP8_H
#define CHIP8_H

#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <string>

using namespace std;

class Chip8 {
public:
	Chip8() {}

	void initialize();
	void loadRom(string fileName);
	void runCycle();
	void draw(sf::RenderWindow &window);
	void checkForInput(sf::Event eve);

	void run();

	void clearScreen();
	void displayvReg();
	void buttonPress(char *text);
	void drawPixelBits();

	uint8_t vReg[16];            //the 16 data registers
	uint8_t addr[4096];     //represents each memory address
	uint8_t pixels[64*32];                         //the 64x32 screen, 0 = black, 1=white
	uint16_t opcode;
	uint16_t pc;                    //at what address we are
	uint8_t stack[16];       //previous address locations before calling subroutines
	uint8_t sp;               //stack pointer
	uint16_t I;               //index register

	uint8_t delayTimer;
	uint8_t soundTimer;

	ifstream rom;
	bool canDraw=false;
	bool waitForKey=false;
	double totalTime;
	uint8_t frames;
	uint8_t frameCap;

	uint8_t keys[16] = {
	/*	'0','1','2','3',
		'4','5','6','7',
		'8','9','A','B',
		'C','D','E','F'
	*/
	0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0
	};

	uint8_t fonts[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	//SFML/TGUI variables
	tgui::Gui gui;
	sf::Keyboard::Key sfmlKeys[16] = {
		sf::Keyboard::Key::Q,sf::Keyboard::Key::W,sf::Keyboard::Key::E,sf::Keyboard::Key::R,
		sf::Keyboard::Key::T,sf::Keyboard::Key::Y,sf::Keyboard::Key::U,sf::Keyboard::Key::I,
		sf::Keyboard::Key::A,sf::Keyboard::Key::S,sf::Keyboard::Key::D,sf::Keyboard::Key::F,
		sf::Keyboard::Key::G,sf::Keyboard::Key::H,sf::Keyboard::Key::J,sf::Keyboard::Key::K
	};
	
};







#endif