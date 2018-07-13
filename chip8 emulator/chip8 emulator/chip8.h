#ifndef CHIP8_H
#define CHIP8_H

#include <iostream>
#include <fstream>

using namespace std;

class Chip8 {


	void loadRom(string fileName);
	void runCycle();
	void clearScreen();

	uint8_t vReg[16];            //the 16 data registers
	uint8_t addr[4096];     //the 4096 nibbles which represent each memory address
	uint8_t pixels[32][64];                         //the 64x32 screen, 0 = black, 1=white
	uint16_t opcode;
	uint16_t pc;                    //where in the address we are
	uint8_t stack[16];       //previous address locations before calling subroutines
	uint8_t sp;               //stack pointer
	uint16_t I;               //index register

	uint8_t delayTimer;
	uint8_t soundTimer;

	ifstream rom;
	bool draw;


	
	
};







#endif