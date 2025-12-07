//******************************************************************************
// Yusuf Oner
// z2048138
// CSCI 463
//
// I certify that this is my own work, and where applicable an extension
// of the starter code for the assignment.
//
//******************************************************************************

#include "memory.h"
#include "hex.h"
#include <iostream>
#include <fstream>
using namespace std;

//******************************************************************************
// Takes uint32_t siz as parameter and allocate siz bytes in the mem vector and 
// initialize every byte/element to 0xa5. Implements rounding logic for formatting and alignment
//******************************************************************************
memory::memory(uint32_t siz)
{
  siz = (siz+15)&0xfffffff0;
  mem.resize(siz, 0xa5);
}

//******************************************************************************
// destructor for cleanup (no param)
//******************************************************************************
memory::~memory()
{
  // vector should handle cleanup automatically
}

//******************************************************************************
// Takes a uint32_t address and return true if the given address i does not represent 
// an element that is present in the mem vector
//******************************************************************************
bool memory::check_illegal(uint32_t addr) const
{
  if (addr >= mem.size())
    {
        cout << "WARNING: Address out of range: " << hex::to_hex0x32(addr) << endl;
        return true;
    }
    return false;
}

//******************************************************************************
// Return the (rounded up) number of bytes in the simulated memory (no param)
//******************************************************************************
uint32_t memory::get_size() const
{
  return mem.size();
}

//******************************************************************************
// Check to see if the given addr (uint32_t parameter) is in your mem by calling check_illegal(). 
// If addr is in the valid range then return the value of the byte from your simulated memory
// at the given address. If addr is not in the valid range then return zero to the caller.
//******************************************************************************
uint8_t memory::get8(uint32_t addr) const
{
  if (check_illegal(addr))
    return 0;
  
  return mem[addr];
}

//******************************************************************************
// Takes a uint32_t address as a parameter, and calls get8() function twice to get
// two bytes and then combine them in little-endian1 order to return a 16-bit uint16_t
//******************************************************************************
uint16_t memory::get16(uint32_t addr) const
{
  uint16_t lsb = get8(addr);
  uint16_t msb = get8(addr + 1);

  return (msb << 8) | lsb;
}

//******************************************************************************
// Takes a uint32_t address as a parameter, and calls get16() function twice and combine  
// the results in little-endian order to return a uint32_t
//******************************************************************************
uint32_t memory::get32(uint32_t addr) const 
{
  uint32_t lsb = get16(addr);
  uint32_t msb = get16(addr + 2);
  
  return (msb << 16) | lsb;
}

//******************************************************************************
// Takes a uint32_t address as a parameter, this function will call get8() and then  
// return the sign-extended value of the byte as a 32-bit signed integer.
//******************************************************************************
int32_t memory::get8_sx(uint32_t addr) const 
{
  uint8_t value = get8(addr);
  return static_cast<int8_t>(value);
}

//******************************************************************************
// Takes a uint32_t address as a parameter, this function will call get16() and then  
// return the sign-extended value of the 16-bit value as a 32-bit signed integer
//******************************************************************************
int32_t memory::get16_sx(uint32_t addr) const
{
  uint16_t value = get16(addr);
  return static_cast<int16_t>(value);
}

//******************************************************************************
// Takes a uint32_t address as a parameter, this function will call get32() and then 
// return the value as a 32-bit signed integer.
//******************************************************************************
int32_t memory::get32_sx(uint32_t addr) const
{
  return get32(addr);
}

//******************************************************************************
// Takes a uint32_t address and a uint8_t value as a parameter. This function will call 
// check_illegal() to verify the addr argument is valid. If addr is valid then set the 
// byte in the simulated memory at that address to the given val. If addr is not
// valid then discard the data and return to the caller. Returns nothing (void)
//******************************************************************************
void memory::set8(uint32_t addr, uint8_t val)
{
  if (check_illegal(addr))
    return;
  
  else
    mem[addr] = val;
}

//******************************************************************************
// Takes a uint32_t address and a uint16_t value as a parameter.This function will call
// set8() twice to store the given val in little-endian order into the
// simulated memory starting at the address given in the addr argument. Returns nothing.
//******************************************************************************
void memory::set16(uint32_t addr, uint16_t val)
{
  set8(addr, val & 0xFF);
  set8(addr + 1, (val >> 8) & 0xFF);
}

//******************************************************************************
// Takes a uint32_t address and a uint32_t value as a parameter. This function will call 
// set16() twice to store the given val in little-endian order into the
// simulated memory starting at the address given in the addr argument. Returns nothing.
//******************************************************************************
void memory::set32(uint32_t addr, uint32_t val)
{
  set16(addr, val & 0xFFFF);
  set16(addr + 2, (val >> 16) & 0xFFFF);
}

//******************************************************************************
// Dump the entire contents of your simulated memory in hex with the corresponding ASCII2
// characters on the right. No parameter or return.
//******************************************************************************
void memory::dump() const
{
    const uint32_t bytes_per_line = 16;

    for (uint32_t addr = 0; addr < mem.size(); addr += bytes_per_line)
    {
        cout << hex::to_hex32(addr) << ": ";

        for (uint32_t j = 0; j < bytes_per_line; ++j)
        {
            if (addr + j < mem.size())
                cout << hex::to_hex8(mem[addr + j]) << " ";
            else
                cout << "   ";

            if (j == 7)
              cout << " ";
        }

        cout << "*";

        for (uint32_t j = 0; j < bytes_per_line; ++j)
        {
            if (addr + j < mem.size())
            {
                uint8_t ch = get8(addr + j);
                ch = isprint(ch) ? ch : '.';
                cout << static_cast<char>(ch);
            }
        }

        cout << "*";
        cout << endl;
    }
}

//******************************************************************************
// Open the file named fname in binary mode and read its contents into your simulated memory.
// Takes a string filename as its parameter and returns true if it can open and false if not.
//******************************************************************************
bool memory::load_file(const string &fname)
{
    ifstream infile(fname, ios::in | ios::binary);
    if (!infile)
    {
        cerr << "Can't open file '" << fname << "' for reading." << endl;
        return false;
    }

    infile >> noskipws;
    uint8_t byte = 0;
    uint32_t addr = 0;

    while (infile >> byte)
    {
        if (check_illegal(addr))
        {
            cerr << "Program too big." << endl;
            infile.close();
            return false;
        }

        mem[addr] = byte;
        ++addr;
    }

    infile.close();
    return true;
}