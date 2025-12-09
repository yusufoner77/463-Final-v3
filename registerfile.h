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

class registerfile : public hex
{
public:
    registerfile() { reset(); }

    void reset();
    void set(uint32_t r, int32_t val);
    int32_t get(uint32_t r) const;
    void dump(const std::string &hdr) const;

private:
    std::vector<int32_t> regs = std::vector<int32_t>(32);
};
