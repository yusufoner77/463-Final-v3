//******************************************************************************
// Yusuf Oner
// z2048138
// CSCI 463
//
// I certify that this is my own work, and where applicable an extension
// of the starter code for the assignment.
//
//******************************************************************************

#pragma once

#include <cstdint>
#include "rv32i_hart.h"
#include "memory.h"

// A CPU that contains a single RV32I hart.
class cpu_single_hart : public rv32i_hart
{
public:
    // Pass the memory reference up to the base-class constructor
    cpu_single_hart(memory &mem)
        : rv32i_hart(mem)
    {
    }

    // Run the hart until halted or exec_limit is reached.
    // If exec_limit == 0: run until is_halted() is true.
    // If exec_limit != 0: run until is_halted() or exec_limit instructions executed.
    void run(uint64_t exec_limit);
};
