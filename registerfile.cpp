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

void registerfile::reset()
{
    regs[0] = 0;
    for (size_t i = 1; i < regs.size(); ++i)
        regs[i] = 0xf0f0f0f0;
}

void registerfile::set(uint32_t r, int32_t val)
{
    if (r == 0 || r >= regs.size())
        return;
    regs[r] = val;
}

int32_t registerfile::get(uint32_t r) const
{
    if (r == 0 || r >= regs.size())
        return 0;
    return regs[r];
}

void registerfile::dump(const std::string &hdr) const
{
    for (int row = 0; row < 4; ++row)
    {
        int base = row * 8;

        std::ostringstream label;
        label << "x" << base;    // "x0", "x8", "x16", "x24"

        std::cout << hdr
                  << std::right << std::setw(3) << label.str() << " ";

        for (int i = 0; i < 8; ++i)
        {
            std::cout << hex::to_hex32(static_cast<uint32_t>(regs[base + i]));

            if (i == 3)
                std::cout << "  ";      // two spaces between 4th and 5th
            else if (i != 7)
                std::cout << ' ';
        }
        std::cout << '\n';
    }
}
