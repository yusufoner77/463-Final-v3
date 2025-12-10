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
#include <string>
#include <vector>
#include "hex.h"

using namespace std;

//******************************************************************************
// The purpose of this class is to store the state of the general-purpose registers
// of one RISC-V hart.
//******************************************************************************

class registerfile : public hex
{
public:
    registerfile() { reset(); }

    void reset();
    void set(uint32_t r, int32_t val);
    int32_t get(uint32_t r) const;
    void dump(const string &hdr) const;

private:
    vector<int32_t> regs = vector<int32_t>(32);
};
