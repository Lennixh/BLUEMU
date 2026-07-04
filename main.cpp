#include <cstdint>
#include <iostream>
#include <cstring>
#include <vector>

uint16_t program0[0x17] = {
	0xF000, // #0 (NOP)
	0xF003, // #1 (NOP)
	0xA004, // #2 (JMP->#4)
	0xA006, // #3 (JMP->#6)
	0x9010, // #4 (JMA->#10) shouldn't happen
	0xA003, // #5 (JMP->#3)
    0xE000, // #6 (CSA)
    0x8009, // #7 (SRJ->#9)
    0x0000, // #8 (HLT)
    0x900B, // #9 (JMA->#B)
    0x0010, // #A (DATA->16)
    0xFFF0, // #B (DATA->-16)
    0xD000, // #C (RAL)
    0x5000, // #D (NOT)
    0xD001, // #E (RAL)
    0xB010, // #F (INP)
    0x5000, // #10 (NOT)
    0xC000, // #11 (OUT)
    0x6008, // #12 (LDA->#8)
    0x7016, // #13 (STA->#16)
    0x100A, // #14 (ADD->#A)
    0x100B, // #15 (ADD->#B)
    0xF000  // #16 (NOP)
};

typedef enum 
{
    FETCH,
    EXECUTE,
} State;

State STATE = FETCH;

typedef enum
{
    ON,
    OFF,
} PowerState;

PowerState power = ON;

#define RAM_LENGTH 4096
uint16_t RAM[RAM_LENGTH];

typedef uint16_t bRegister;

bRegister ACC;
bRegister DIL = 0x0045;
bRegister DOL;
bRegister DSL;
bRegister IR;
bRegister MAR;
bRegister MBR;
bRegister PC = 0x00;
bRegister SR = 0b1000000000000111;
bRegister Z;

bool TRA = false;
const bool R = true;      // ALWAYS READY B)

void dumpRegisters()
{
    std::printf("PC: %04x ACC: %04x IR: %04x Z: %04x MAR: %04x MBR: %04x DSL: %02x DIL: %02x DOL: %02x\n", PC, ACC, IR, Z, MAR, MBR, (DSL & 0x00FF), (DIL & 0x00FF), (DOL & 0x00FF));
}

void doHLT(uint8_t tick)
{
    if (tick == 7)
    {
        power = OFF;
    }
    else if (tick == 8)
    {
        MAR = PC;
    }
}

void doADD(uint8_t tick)
{
    if (STATE == FETCH)
    {
        if (tick == 6)
        {
            Z = 0x00;
        }
        else if (tick == 7)
        {
            Z = ACC;
        }
        else if (tick == 8)
        {
            MAR = (IR & 0x0FFF);
            STATE = EXECUTE;
        }
    }
    else
    {
        if (tick == 3)
        {
            ACC = 0x00;
            MBR = 0x00;
        }
        else if (tick == 4)
        {
            MBR = RAM[MAR];
        }
        else if (tick == 7)
        {
            uint32_t res = Z + MBR;
            if ((Z & 0x8000) && (MBR & 0x8000) && !(res & 0x8000))
            {
                power = OFF; //Underflow
            }
            else if (!(Z & 0x8000) && !(MBR & 0x8000) && (res & 0x8000))
            {
                power = OFF; //Overflow
            }
            ACC = (uint16_t)res;
        }
        else if (tick == 8)
        {
            STATE = FETCH;
            MAR = PC;
        }
    }
}

void doXOR(uint8_t tick)
{
    if (STATE == FETCH)
    {
        if (tick == 6)
        {
            Z = 0x00;
        }
        else if (tick == 7)
        {
            Z = ACC;
        }
        else if (tick == 8)
        {
            MAR = (IR & 0x0FFF);
            STATE = EXECUTE;
        }
    }
    else
    {
        if (tick == 3)
        {
            ACC = 0x00;
            MBR = 0x00;
        }
        else if (tick == 4)
        {
            MBR = RAM[MAR];
        }
        else if (tick == 7)
        {
            ACC = MBR ^ Z;
        }
        else if (tick == 8)
        {
            STATE = FETCH;
            MAR = PC;
        }
    }
}

void doAND(uint8_t tick)
{
    if (STATE == FETCH)
    {
        if (tick == 6)
        {
            Z = 0x00;
        }
        else if (tick == 7)
        {
            Z = ACC;
        }
        else if (tick == 8)
        {
            MAR = (IR & 0x0FFF);
            STATE = EXECUTE;
        }
    }
    else
    {
        if (tick == 3)
        {
            ACC = 0x00;
            MBR = 0x00;
        }
        else if (tick == 4)
        {
            MBR = RAM[MAR];
        }
        else if (tick == 7)
        {
            ACC = MBR & Z;
        }
        else if (tick == 8)
        {
            STATE = FETCH;
            MAR = PC;
        }
    }
}

void doIOR(uint8_t tick)
{
        if (STATE == FETCH)
    {
        if (tick == 6)
        {
            Z = 0x00;
        }
        else if (tick == 7)
        {
            Z = ACC;
        }
        else if (tick == 8)
        {
            MAR = (IR & 0x0FFF);
            STATE = EXECUTE;
        }
    }
    else
    {
        if (tick == 3)
        {
            ACC = 0x00;
            MBR = 0x00;
        }
        else if (tick == 4)
        {
            MBR = RAM[MAR];
        }
        else if (tick == 7)
        {
            ACC = MBR | Z;
        }
        else if (tick == 8)
        {
            STATE = FETCH;
            MAR = PC;
        }
    }
}

void doNOT(uint8_t tick)
{
    if (STATE == FETCH)
    {
        if (tick == 6)
        {
            Z = 0x00;
        }
        else if (tick == 7)
        {
            Z = ACC;
        }
        else if (tick == 8)
        {
            STATE = EXECUTE;
        }
    }
    else
    {
        if (tick == 1)
        {
            ACC = 0x00;
        }
        else if (tick == 2)
        {
            ACC = ~Z;
        } 
        else if (tick == 8)
        {
            MAR = PC;
            STATE = FETCH;
        }
    }
}

void doLDA(uint8_t tick)
{
    if (STATE == FETCH)
    {
        if (tick == 8)
        {
            STATE = EXECUTE;
            MAR = (IR & 0x0FFF);
        }
    }
    else
    {
        if (tick == 2)
        {
            ACC = 0x00;
        }
        else if (tick == 3)
        {
            MBR = 0x00;
        }
        else if (tick == 5)
        {
            MBR = RAM[MAR];
            ACC = MBR;
        }
        else if (tick == 8)
        {
            STATE = FETCH;
            MAR = PC;
        }
    }
}

void doSTA(uint8_t tick)
{
    if (STATE == FETCH)
    {
        if (tick == 8)
        {
            STATE = EXECUTE;
            MAR = (IR & 0x0FFF);
        }
    }
    else
    {
        if (tick == 4)
        {
            MBR = 0;
        }
        else if (tick == 5)
        {
            MBR = ACC;
            RAM[MAR] = MBR;
        }
        else if (tick == 8)
        {
            STATE = FETCH;
            MAR = PC;
        }
    }
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
    if (STATE == FETCH)
    {
        
        if (tick == 6)
        {
            
            ACC = 0x00;
            DSL = (IR & 0x003f);
        }
        else if (tick == 7)
        {
            TRA = true;
        }
        else if (tick == 8)
        {
            STATE = EXECUTE;
        }
    }
    else
    {
        
        if (tick == 5)
        {
            if (R == true)
            {
                ACC = DIL;
            }
        }
        else if (tick == 6)
        {
            if (R == true)
            {
                TRA = false;
            }
        }
        else if (tick == 8)
        {
            if (TRA == false)
            {
                STATE = FETCH;
                MAR = PC;
            }
        }
    }
}

void doOUT(uint8_t tick)
{
    if (STATE == FETCH)
    {
        if (tick == 6)
        {
            DOL = ((ACC >> 8) & 0x00FF);
            DSL = (IR & 0x003f);
        }
        else if (tick == 7)
        {
            TRA = true;
        }
        else if (tick == 8)
        {
            STATE = EXECUTE;
        }
    }
    else
    {
        if (tick == 6)
        {
            if (R == true)
            {
                TRA = false;
            }
        }
        else if (tick == 8)
        {
            if (TRA == false)
            {
                STATE = FETCH;
                MAR = PC;
            }
        }
    }
}

void doRAL(uint8_t tick)
{
    if (STATE == FETCH)
    {
        if (tick == 6)
        {
            Z = 0x00;
        }
        else if (tick == 7)
        {
            Z = ACC;
        }
        else if (tick == 8)
        {
            STATE = EXECUTE;
        }
    }
    else
    {
        if (tick == 1)
        {
            ACC = 0x00;
        }
        else if (tick == 2)
        {
            ACC = ((Z & 0x8000) >> 15) | (Z * 2);
        }
        else if (tick == 8)
        {
            MAR = PC;
            STATE = FETCH;
        }
    }
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



void pressStart()
{
    power = ON;
}

void pressStop()
{
    power = OFF;
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
            else if (STATE == EXECUTE)
            {

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


void powerProgram(const uint16_t* program)
{
    std::printf("Copying a program to RAM\n");
    memset(RAM, 0x00, RAM_LENGTH * sizeof(uint16_t));
    memmove(RAM, program, (RAM_LENGTH * sizeof(uint16_t)));
    for (;;)
    {
        if(power == OFF)
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
    powerProgram(program0);
    return 0;
}