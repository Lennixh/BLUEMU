#include <cstdint>
#include <iostream>

uint16_t program0[4] =
{
    0b1111000000000000, //NOP, 0
    0b1111000000000011, //NOP, 3
    0b1111101010101010, //NOP, 2730
    0x0F05              //NOP, 5
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
bRegister PC = 0b0000000000000000;
bRegister SR;
bRegister Z;

uint8_t clock_pulse = 1;

void dumpRegisters()
{
    std::printf("ACC: %04x DI: %02x DO: %02x DSL: %02x IR: %04x MAR: %04x MBR: %04x PC: %04x SR: %04x Z: %04x\n", ACC, (DI & 0x00FF), (DO & 0x00FF), (DSL & 0x00FF), IR, MAR, MBR, PC, SR, Z);
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

void processClockTick(uint8_t tick)
{
    switch (tick)
    {
        case 1:
            break;

        case 2:
            if (STATE == FETCH)
            {
                PC++;
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
            break;

        case 6:
            break;

        case 7:
            break;

        case 8:
            break;
        
        default:
            break;
        
        uint8_t INS = getInstruction();
        if (INS == 15)
        {
            doNOP(tick);
        }
    }
}

void doCycle()
{
    while (clock_pulse < 9)
    {
        processClockTick(clock_pulse);

        clock_pulse++;
    }
    clock_pulse = 1;
}

int main(int argc, char* argv[])
{
    dumpRegisters();
    return 0;
}