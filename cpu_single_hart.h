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

//******************************************************************************
// This is a subclass of rv32i_hart that is used to represent a CPU with a single hart.
//******************************************************************************
class cpu_single_hart : public rv32i_hart
{
public:
    cpu_single_hart(memory &mem) : rv32i_hart(mem) {}

    void run(uint64_t exec_limit);
};
