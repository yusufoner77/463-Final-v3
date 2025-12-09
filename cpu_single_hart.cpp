//******************************************************************************
// Yusuf Oner
// z2048138
// CSCI 463
//
// I certify that this is my own work, and where applicable an extension
// of the starter code for the assignment.
//
//******************************************************************************

#include "cpu_single_hart.h"
#include <iostream>

using std::cout;
using std::endl;

void cpu_single_hart::run(uint64_t exec_limit)
{
    regs.set(2, static_cast<int32_t>(mem.get_size()));

    if (exec_limit == 0)
    {
        while (!is_halted())
            tick("");         
    }
    else
    {
        while (!is_halted() && get_insn_counter() < exec_limit)
            tick("");
    }

    if (is_halted())
        std::cout << "Execution terminated. Reason: "
                  << get_halt_reason() << "\n";
                  
    cout << get_insn_counter() << " instructions executed" << endl;
}
