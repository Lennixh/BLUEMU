#include <cstdint>

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

uint8_t clock_pulse = 1;

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

int main()
{
    return 0;
}