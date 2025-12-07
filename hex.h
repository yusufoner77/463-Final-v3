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

//******************************************************************************
// contains some utility functions for formatting numbers as hex strings
// for printing
//******************************************************************************

class hex
{
  public:
    static std::string to_hex8(uint8_t i);
    static std::string to_hex32(uint32_t i);
    static std::string to_hex0x32(uint32_t i);
    static std::string to_hex0x20(uint32_t i);
    static std::string to_hex0x12(uint32_t i);
};