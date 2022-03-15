#include "cpu.h"

int main()
{
    CPU cpu;
    for (;;)
        cpu.runNextInstruction();

    return 0;
}
