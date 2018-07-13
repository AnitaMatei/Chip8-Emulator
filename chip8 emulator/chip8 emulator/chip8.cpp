#include "chip8.h"


void Chip8::clearScreen() {
	for (int i = 0; i < 64*32; i++)
			pixels[i] = 0;
}

void Chip8::initialize() {
	srand(time(NULL));
	clearScreen();
	I = 0x0;
	pc = 0x200;
	sp = 0x0;
	delayTimer = 0;
	soundTimer = 0;

	for (int i = 0; i < 16; i++)
	{
		stack[i] = vReg[i] = 0x0;
	}
	for (int i = 0; i < 80; i++)
	{
		addr[i] = fonts[i];
	}
	for (int i = 80; i < 4096; i++)
	{
		addr[i] = 0;
	}
}

void Chip8::loadRom(string fileName) {
	rom.open(fileName.c_str(), std::ios::binary);
	char addressByte[1];
	int count = 0;
	while (!rom.eof())
	{
		rom.read(addressByte, 1);
		addr[count+0x200] = addressByte[0];
		count++;
	}
	rom.close();
}




void Chip8::runCycle() {
	opcode = addr[pc] << 8 | addr[pc + 1];
	

	switch (opcode & 0xF000) {          //organizing cases by the first nibble
		//0x0---
		case 0x0000:
			switch (opcode) {
				//0x00E0
				case 0x00E0:		//Clears the screen.
					clearScreen();
					break;
				//0x00EE
				case 0x00EE:	//Returns from a subroutine.
					sp--;
					pc = stack[sp];
					pc += 2;
					break;
			
				default:
					cout << "unknown " << opcode;         //to be replaced
					break;
			}
			break;
		//0x1NNN
		case 0x1000:
			pc = opcode & 0x0FFF;       //jumps to the address indicated by the last 3 nibbles
			break;
		//0x2NNN
		case 0x2000:
			stack[sp] = pc;            //calls subroutine at the address indicated by the last 3 nibbles
			sp++;
			pc = opcode & 0x0FFF;
			break;
		//0x3XNN
		case 0x3000:
			if ((opcode & 0x00FF) == vReg[opcode & 0x0F00>>8])   //if VX == NN skip an opcode
				pc += 4;
			else pc += 2;
			break;
		//0x4XNN
		case 0x4000:
			if ((opcode & 0x00FF) != vReg[opcode & 0x0F00>>8])  //if VX!=NN skip an opcode
				pc += 4;
			else pc += 2;
			break;
		//0x5XY0
		case 0x5000:
			if (vReg[opcode & 0x0F00>>8] == vReg[opcode & 0x0F00>>4])       //if VX==VY skip an opcode
				pc += 4;
			else pc += 2;
			break;
		//0x6XNN
		case 0x6000:									//sets VX to NN.
			vReg[opcode & 0x0F00>>8] = opcode & 0x00FF;
			pc += 2;
			break;
		//0x7XNN
		case 0x7000:							//adds NN to VX. 
			vReg[opcode & 0x0F00>>8] += opcode & 0x00FF;
			pc += 2;
			break;
		//0x8XY-
		case 0x8000:								
			switch (opcode & 0x000F) {
				//0x8XY0									
				case 0x0000:					//sets VX to the value of VY
					vReg[opcode & 0x0F00>>8] = vReg[opcode & 0x0F00>>4];
					pc += 2;
					break;
				//0x8XY1
				case 0x0001:				//sets VX to VX or VY.
					vReg[opcode & 0x0F00>>8] = vReg[opcode & 0x0F00>>8] | vReg[opcode & 0x0F00>>4];
					pc += 2;
					break;
				//0x8XY2
				case 0x0002:				//Sets VX to VX and VY
					vReg[opcode & 0x0F00>>8] = vReg[opcode & 0x0F00>>8] & vReg[opcode & 0x0F00>>4];
					pc += 2;
					break;
				//0x8XY3
				case 0x0003:			//Sets VX to VX xor VY.
					vReg[opcode & 0x0F00>>8] = vReg[opcode & 0x0F00>>8] ^ vReg[opcode & 0x0F00>>4];
					pc += 2;
					break;
				//0x8XY4
				case 0x0004: {								//Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
					uint16_t cvReg = vReg[opcode & 0x0F00>>8] + vReg[opcode & 0x00F0>>4];   //used for checking if theres a bit carried when summing 2 bytes
					if ((cvReg & 0xFF00) != 0x0)
						vReg[0xF] = 0x1;
					else vReg[0xF] = 0x0;
					vReg[opcode & 0x0F00>>8] = cvReg & 0xFFFF;
					pc += 2;
					break;
				}
				//0x8XY5
				case 0x0005:					//VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
					if (vReg[opcode & 0x0F00>>8] > vReg[opcode & 0x00F0>>4])
						vReg[0xF] = 0x1;
					else vReg[0xF] = 0x0;
					vReg[opcode & 0x0F00>>8] -= vReg[opcode & 0x00F0>>4];
					pc += 2;
					break;
				//0x8XY6
				case 0x0006:			//Shifts VY right by one and stores the result to VX (VY remains unchanged).
										//VF is set to the value of the least significant bit of VY before the shift.
					vReg[0xF] = vReg[opcode & 0x00F0>>4] & 0x01;
					vReg[opcode & 0x0F00>>8] = vReg[opcode & 0x00F0>>4] >> 1;
					pc += 2;
					break;
				//0x8XY7
				case 0x0007:			//Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
					if (vReg[opcode & 0x00F0>>4] > vReg[opcode & 0x0F00>>8])
						vReg[0xF] = 0x1;
					else vReg[0xF] = 0x0;
					vReg[opcode & 0x0F00>>8] = vReg[opcode & 0x00F0>>4]- vReg[opcode & 0x0F00>>8];
					pc += 2;
					break;
				//0x8XYE
				case 0x000E:			//shifts VY's bits to the left by one and copies the result to VX.
										// VF is set to the value of the MSB of VY before the shift.
					vReg[0xF] = vReg[opcode & 0x00F0>>4] & 0x80;
					vReg[opcode & 0x0F00>>8] = vReg[opcode & 0x00F0>>4] = vReg[opcode & 0x00F0>>4] << 1;
					pc += 2;
					break;
			}
			break;
			//0x9XY0
		case 0x9000:			//skip an opcode if VX!=VY
			if (vReg[opcode & 0x0F00>>8] != vReg[opcode & 0x00F0>>4])
				pc += 4;
			else pc += 2;
			break;
			//0xANNN
		case 0xA000:			//sets I to the address NNN
			I = opcode & 0x0FFF;
			pc += 2;
			break;
			//0xBNNN
		case 0xB000:			//jumps to the address NNN+V0
			pc = (opcode & 0x0FFF) + vReg[0];
			break;
			//0xCXNN
		case 0xC000:			//sets vx to the result of a random number & NN
			vReg[opcode & 0x0F00>>8] = (rand() % 0xFF) & (opcode & 0x00FF);
			pc += 2;
			break;
			//0xDXYN
		case 0xD000: {			//draws a sprite of 8 x N at (VX,VY). if any pixel goes from 1 to 0 VF=1, 0 otherwise
			uint8_t xPixel = vReg[opcode & 0x0F00>>8];       //the coordinates of a pixel from the data registry, each byte represents a register value
			uint8_t yPixel = vReg[opcode & 0x00F0>>4];
			for (uint8_t i = 0; i < (opcode & 0x000F); i++) {
				uint8_t spriteRow = addr[I + i];
				for (uint8_t j = 0; j < 0x08; j++)
				{                                
					pixels[i + yPixel + (j + xPixel) * 64] ^= (spriteRow >> j) & 0x1;
					if (pixels[i + yPixel+(j + xPixel)*64]==0x01)      //if a pixel switched from set to unset, VF is set to 1
						vReg[0xF] = 0x1;
					else vReg[0xF] = 0x0;
				}
			}
			draw = true;
			pc += 2;
			break;
		}
		//0xEX--
		case 0xE000:	
			switch (opcode & 0x000F) {			
				//0xEX9E
				case 0x000E:			//skips an opcode if the key in VX is pressed
					if (keys[vReg[opcode & 0x0F00>>8]] == 0x1)
						pc += 4;
					else pc += 2;
					break;
				//0xEXA1
				case 0x0001:			////skips an opcode if the key in VX is not pressed
					if (keys[vReg[opcode & 0x0F00>>8]] != 0x1)
						pc += 4;
					else pc += 2;
					break;
			}
			break;
		//0xF---
		case 0xF000:
			switch (opcode & 0x00FF) {
				//0xFX07
				case 0x0007:			//sets VX to the value of the delay timer
					vReg[opcode & 0x0F00>>8] = delayTimer;
					pc += 2;
					break;
				//0xFX0A
				case 0x000A:			//program pauses while waiting for a key press
					waitForKey = true;
					pc += 2;
					break;
				//0xFX15
				case 0x0015:			//sets delay timer to VX
					delayTimer = vReg[opcode & 0x0F00>>8];
					pc += 2;
					break;
				//0xFX18
				case 0x0018:			//sets sound timer to VX
					soundTimer = vReg[opcode & 0x0F00>>8];
					break;
				//0xFX1E
				case 0x001E:			//adds VX to I
					I += vReg[opcode & 0x0F00>>8];
					break;
				//0xFX29
				case 0x0029:			//sets I to the location of the character in VX
					I = vReg[opcode & 0x0F00 >> 8] * 0x05;			//each character is 8x5 bits
					break;

			}
			break;
	}

}
