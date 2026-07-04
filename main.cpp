#include <cstdint>
#include <iostream>
#include <cstring>

uint16_t program0[4] = 
{
	0xF000, // NOP 000
	0xF003, // NOP 003
	0xF000, // NOP 000
	0xF005  // NOP 005
};

typedef enum 
{
    FETCH,
    EXECUTE,
} State;

State STATE = FETCH;

#define RAM_LENGTH 4096
uint16_t RAM[RAM_LENGTH];

typedef uint16_t bRegister;

bRegister ACC;
bRegister DI;
bRegister DO;
bRegister DSL;
bRegister IR;
bRegister MAR;
bRegister MBR;
bRegister PC = 0x00;
bRegister SR;
bRegister Z;


void dumpRegisters()
{
    std::printf("PC: %04x ACC: %04x IR: %04x Z: %04x MAR: %04x MBR: %04x DSL: %02x DI: %02x DO: %02x\n", PC, ACC, IR, Z, MAR, MBR, (DSL & 0x00FF), (DI & 0x00FF), (DO & 0x00FF));
}



void doNOP(uint8_t tick)
{
    if (tick == 8)
    {
        MAR = PC;
    }
}



uint8_t getInstruction()
{
    return ((IR & 0b1111000000000000) >> 12);
}



void processTick(uint8_t tick)
{
    switch (tick)
    {
        case 1:
            break;

        case 2:
            if (STATE == FETCH)
            {
                PC+=1;
            }
            break;

        case 3:
            if (STATE == FETCH)
            {
                MBR = 0x00;
            }
            break;

        case 4:
            if (STATE == FETCH)
            {
                IR = 0x00;
                MBR = RAM[MAR];
            }
            break;

        case 5:
            if (STATE == FETCH)
            {
                IR = MBR;
            }
            break;

        case 6:
            break;

        case 7:
            break;

        case 8:
            break;
        
        default:
            break;
        
    }
    uint8_t INS = getInstruction();
    if (INS == 15)
    {
        doNOP(tick);
    }
}


uint8_t clock_pulse = 1;

void doCycle()
{
    while (clock_pulse < 9)
    {
        processTick(clock_pulse);
        clock_pulse++;
    }
    clock_pulse = 1;
}



void runProgram(const uint16_t* program)
{
    std::printf("Copying a program to RAM\n");
    memset(RAM, 0x00, RAM_LENGTH * sizeof(uint16_t));
    memmove(RAM, program, (RAM_LENGTH * sizeof(uint16_t)));
    for (;;)
    {
        doCycle();
        dumpRegisters();
    }
}



int main(int argc, char* argv[])
{
    runProgram(program0);
    return 0;
}