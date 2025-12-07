//******************************************************************************
// Yusuf Oner
// z2048138
// CSCI 463
//
// I certify that this is my own work, and where applicable an extension
// of the starter code for the assignment.
//
//******************************************************************************

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include "memory.h"
#include "hex.h"
#include "rv32i_decode.h"
using namespace std;

//******************************************************************************
// Takes a reference to memory object where the binary program is as its parameter.
// It will decode and print each instruction in a loop, and returns nothing.
//******************************************************************************
static void disassemble(const memory &mem)
{
	uint32_t size = mem.get_size();

  for (uint32_t addr = 0; addr < size; addr += 4)
  {
    uint32_t insn = mem.get32(addr);

    cout << hex::to_hex32(addr) << ": " << hex::to_hex32(insn) << "  " << rv32i_decode::decode(addr, insn) << endl;
  }
}

//******************************************************************************
// Prints an error message explaining the correct command-line syntax and then terminates.
// No parameters or return.
//******************************************************************************
static void usage()
{
	cerr << "Usage: rv32i [-m hex-mem-size] infile" << endl;
	cerr << "    -m specify memory size (default = 0x100)" << endl;
	exit(1);
}

int main(int argc, char **argv)
{
  uint32_t memory_limit = 0x100;
  int opt;
  while ((opt = getopt(argc, argv, "m:")) != -1)
  {
    switch (opt)
    {
    	case 'm':
    		{
      		std::istringstream iss(optarg);
      		iss >> std::hex >> memory_limit;
				}
        break;
      default:
				usage(); 
		}
	}
  if (optind >= argc)
      usage();

  memory mem(memory_limit);

  if (!mem.load_file(argv[optind]))
      usage();

  disassemble(mem);
  mem.dump();

	return 0; 
}


