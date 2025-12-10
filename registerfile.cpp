//******************************************************************************
// Yusuf Oner
// z2048138
// CSCI 463
//
// I certify that this is my own work, and where applicable an extension
// of the starter code for the assignment.
//
//******************************************************************************

#include "registerfile.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

//******************************************************************************
// This function initializes register x0 to zero, and all other registers to 0xf0f0f0f0.
// Parameters: none.
// Return value: none.
//******************************************************************************
void registerfile::reset()
{
    regs[0] = 0;
    for (size_t i = 1; i < regs.size(); ++i)
        regs[i] = 0xf0f0f0f0;
}

//******************************************************************************
// This function writes a 32-bit value into register r. If r is zero then do nothing.
// Parameters:
//   r   - register index (0–31)
//   val - 32-bit signed value to store in the register
// Return value: none.
//******************************************************************************
void registerfile::set(uint32_t r, int32_t val)
{
    if (r == 0 || r >= regs.size())
        return;
    regs[r] = val;
}

//******************************************************************************
// This function retrieves the value stored in register r. If r is zero then return zero.
// Parameters:
//   r - register index (0–31)
// Return value:
//   The signed 32-bit integer stored in register r
//******************************************************************************
int32_t registerfile::get(uint32_t r) const
{
    if (r == 0 || r >= regs.size())
        return 0;
    return regs[r];
}


//******************************************************************************
// This function prints a dump of the registers
// Parameters:
//   hdr - a string printed at the start of each line
// Return value: none.
//******************************************************************************
void registerfile::dump(const string &hdr) const
{
    for (int row = 0; row < 4; ++row)
    {
        int base = row * 8;

        ostringstream label;
        label << "x" << base;

        cout << hdr
                  << right << setw(3) << label.str() << " ";

        for (int i = 0; i < 8; ++i)
        {
            cout << hex::to_hex32(static_cast<uint32_t>(regs[base + i]));

            if (i == 3)
                cout << "  ";
            else if (i != 7)
                cout << ' ';
        }
        cout << '\n';
    }
}
