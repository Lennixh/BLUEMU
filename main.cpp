#include <cstdint>
#include <iostream>
#include <cstring>
#include <vector>

uint16_t program0[0xC] = {
	0xF000, // #0 (NOP)
	0xF003, // #1 (NOP)
	0xA004, // #2 (JMP->#4)
	0xA006, // #3 (JMP->#6)
	0x9010, // #4 (JMA->#10) shouldn't happen
	0xA003, // #5 (JMP->#3)
    0xE000, // #6 (CSA)
    0x8009, // #7 (SRJ->#9)
    0xF676, // #8 (NOP)
    0x900B, // #9 (JMA->#B)
    0xF696, // #A (NOP)
    0x0000  // #B (HLT)
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
bRegister SR = 0b1000000000000111;
bRegister Z;

#define ON true
#define OFF false
bool RUN = ON;


void dumpRegisters()
{
    std::printf("PC: %04x ACC: %04x IR: %04x Z: %04x MAR: %04x MBR: %04x DSL: %02x DI: %02x DO: %02x\n", PC, ACC, IR, Z, MAR, MBR, (DSL & 0x00FF), (DI & 0x00FF), (DO & 0x00FF));
}

void doHLT(uint8_t tick)
{
    if (tick == 7)
    {
        RUN = OFF;
    }
    else if (tick == 8)
    {
        MAR = PC;
    }
}

void doADD(uint8_t tick)
{

}

void doXOR(uint8_t tick)
{

}

void doAND(uint8_t tick)
{
    
}

void doIOR(uint8_t tick)
{
    
}

void doNOT(uint8_t tick)
{
    
}

void doLDA(uint8_t tick)
{
    
}

void doSTA(uint8_t tick)
{
    
}

void doSRJ(uint8_t tick)
{
    if (tick == 6)
    {
        ACC = (ACC | (PC & 0x0FFF));
    }
    else if (tick == 7)
    {
        PC = 0x00;
    }
    else if (tick == 8)
    {
        PC = (IR & 0x0FFF);
        MAR = PC;
    }
}

void doJMA(uint8_t tick)
{
    if (tick == 6)
    {
        if (1 == ((ACC >> 15) & 1))
        {
            PC = 0x00;
        }
    }
    else if (tick == 7)
    {
        if (1 == ((ACC >> 15) & 1))
        {
            PC = (IR & 0x0FFF);
        }
    }
    else if (tick == 8)
    {
        MAR = PC;
    }
}

void doJMP(uint8_t tick)
{
    if (tick == 6)
    {
        PC = 0x00;
    } 
    else if (tick == 7) 
    {
        PC = (IR & 0x0FFF);
    }
    else if (tick == 8)
    {
        MAR = PC;
    }   
}

void doINP(uint8_t tick)
{

}

void doOUT(uint8_t tick)
{

}

void doRAL(uint8_t tick)
{

}

void doCSA(uint8_t tick)
{
    if (tick == 6)
    {
        ACC = 0x00;
    }
    else if (tick == 7)
    {
        ACC = SR;
    }
    else if (tick == 8)
    {
        MAR = PC;
    }   
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


std::vector<void (*)(uint8_t tick)> instructionTable 
{
    doHLT,
    doADD,
    doXOR,
    doAND,
    doIOR,
    doNOT,
    doLDA,
    doSTA,
    doSRJ,
    doJMA,
    doJMP,
    doINP,
    doOUT,
    doRAL,
    doCSA,
    doNOP
};


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
    (*instructionTable[INS])(tick);
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
        if(RUN == OFF)
        {
            goto quit;
        }
        doCycle();
        dumpRegisters();
    }
    quit: std::cout << "Finished execution\n";
}



int main(int argc, char* argv[])
{
    runProgram(program0);
    return 0;
}