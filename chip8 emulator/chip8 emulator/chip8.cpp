#include "chip8.h"

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
		case 0x0000:
			switch (opcode) {
				case 0x00E0:
					clearScreen();
					break;
				case 0x00EE:
					sp--;
					pc = stack[sp];
					pc += 2;
					break;
				default:
					cout << "unknown " << opcode;         //to be replaced
					break;
			}
			break;
		case 0x1000:
			pc = opcode & 0x0FFF;       //jumps to the address indicated by the last 3 nibbles
			break;
		case 0x2000:
			stack[sp] = pc;            //calls subroutine at the address indicated by the last 3 nibbles
			sp++;
			pc = opcode & 0x0FFF;
			break;
		case 0x3000:
			if ((opcode & 0x00FF) == vReg[opcode & 0x0F00])   //if vx == nn jump 2 bytes worth of instructions
				pc += 4;
			else pc += 2;
			break;
		case 0x4000:
			if ((opcode & 0x00FF) != vReg[opcode & 0x0F00])  //if vx!=nn jump 2 bytes worth of instructions
				pc += 4;
			else pc += 2;
			break;
		case 0x5000:
			if (vReg[opcode & 0x0F00] == vReg[opcode & 0x00F0])       //if vx==vy jump 2 bytes worth of instructions 
				pc += 4;
			else pc += 2;
			break;
		case 0x6000:									
			vReg[opcode & 0x0F00] = opcode & 0x00FF;
			pc += 2;
			break;
		case 0x7000:
			vReg[opcode & 0x0F00] += opcode & 0x00FF;
			pc += 2;
			break;
		case 0x8000:
			switch (opcode & 0x000F) {
				case 0x0000:
					vReg[opcode & 0x0F00] = vReg[opcode & 0x00F0];
					pc += 2;
					break;
				case 0x0001:
					vReg[opcode & 0x0F00] = vReg[opcode & 0x0F00] | vReg[opcode & 0x00F0];
					pc += 2;
					break;
				case 0x0002:
					vReg[opcode & 0x0F00] = vReg[opcode & 0x0F00] & vReg[opcode & 0x00F0];
					pc += 2;
					break;
				case 0x0003:
					vReg[opcode & 0x0F00] = vReg[opcode & 0x0F00] ^ vReg[opcode & 0x00F0];
					pc += 2;
					break;
				case 0x0004: {
					uint16_t cvReg = vReg[opcode & 0x0F00] + vReg[opcode & 0x00F0];   //used for checking if theres a bit carried when summing 2 bytes
					if ((cvReg & 0xFF00) != 0x0)
						vReg[0xF] = 0x1;
					else vReg[0xF] = 0x0;
					vReg[opcode & 0x0F00] = cvReg & 0xFFFF;
					pc += 2;
					break;
				}
				case 0x0005:
					if (vReg[opcode & 0x0F00] > vReg[opcode & 0x00F0])
						vReg[0xF] = 0x1;
					else vReg[0xF] = 0x0;
					vReg[opcode & 0x0F00] -= vReg[opcode & 0x00F0];
					pc += 2;
					break;
				case 0x0006:
					vReg[0xF] = vReg[opcode & 0x00F0] & 0x01;
					vReg[opcode & 0x0F00] = vReg[opcode & 0x00F0] >> 1;
					pc += 2;
					break;
				case 0x0007:
					if (vReg[opcode & 0x00F0] > vReg[opcode & 0x0F00])
						vReg[0xF] = 0x1;
					else vReg[0xF] = 0x0;
					vReg[opcode & 0x0F00] = vReg[opcode & 0x00F0]- vReg[opcode & 0x0F00];
					pc += 2;
					break;
				case 0x000E:
					vReg[0xF] = vReg[opcode & 0x00F0] & 0x80;
					vReg[opcode & 0x0F00] = vReg[opcode & 0x00F0] = vReg[opcode & 0x00F0] << 1;
					pc += 2;
					break;
			}
			break;
		case 0x9000:
			if (vReg[opcode & 0x0F00] != vReg[opcode & 0x00F0])
				pc += 4;
			else pc += 2;
			break;
		case 0xA000:
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		case 0xB000:
			pc = (opcode & 0x0FFF) + vReg[0];
			break;
		case 0xC000:
			vReg[opcode & 0x0F00] = (rand() % 0xFF) & (opcode & 0x00FF);
			pc += 2;
			break;
		case 0xD000: {
			uint8_t xPixel = vReg[opcode & 0x0F00];       //the coordinates of a pixel from the data registry, each byte represents a register value
			uint8_t yPixel = vReg[opcode & 0x00F0];
			for (uint8_t i = 0; i < (opcode & 0x000F); i++) {
				uint8_t spriteRow = addr[I + i];
				for (uint8_t j = 0; j < 0x08; j++)
				{
					uint8_t copy = pixels[i + yPixel][j + xPixel];                                 
					pixels[i + yPixel][j + xPixel] ^= (spriteRow >> j)& 0x1 ;
					if (copy == 0x1 && pixels[i + yPixel][j + xPixel] == 0x0)      //if a pixel switched from set to unset, flag is set to 1
						vReg[0xF] = 0x1;
					else vReg[0xF] = 0x0;
				}
			}
			draw = true;
			pc += 2;
			break;
		}
	}

}

void Chip8::clearScreen() {
	for (int i = 0; i < 32; i++)
		for (int j = 0; j < 64; j++)
			pixels[i][j] = 0;


}