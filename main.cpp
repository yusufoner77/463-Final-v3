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
#include "cpu_single_hart.h"

using namespace std;

static void disassemble(const memory &mem)
{
    uint32_t size = mem.get_size();

    for (uint32_t addr = 0; addr < size; addr += 4)
    {
        uint32_t insn = mem.get32(addr);

        cout
            << hex::to_hex32(addr) << ": "
            << hex::to_hex32(insn) << "  "
            << rv32i_decode::decode(addr, insn)
            << endl;
    }
}

static void usage()
{
    cerr << "Usage: rv32i [-d] [-i] [-r] [-z] [-l exec_limit] [-m hex-mem-size] infile" << endl;
    cerr << "    -d  disassemble before simulation" << endl;
    cerr << "    -i  show instructions as they execute" << endl;
    cerr << "    -r  show register dump before each instruction" << endl;
    cerr << "    -z  show final register and memory dump after simulation" << endl;
    cerr << "    -l  limit the number of instructions executed (0 = no limit)" << endl;
    cerr << "    -m  specify memory size in hex (default = 0x100)" << endl;
    exit(1);
}

int main(int argc, char **argv)
{
    uint32_t memory_limit = 0x100;
    uint64_t exec_limit   = 0;

    bool opt_disassemble  = false;   // -d
    bool opt_show_insn    = false;   // -i
    bool opt_show_regs    = false;   // -r
    bool opt_final_dump   = false;   // -z

    int opt;

    while ((opt = getopt(argc, argv, "m:dirzl:")) != -1)
    {
        switch (opt)
        {
            case 'm':
            {
                istringstream iss(optarg);
                iss >> std::hex >> memory_limit;
                if (!iss)
                {
                    cerr << "Bad -m value: " << optarg << endl;
                    usage();
                }
                break;
            }

            case 'd':
                opt_disassemble = true;
                break;

            case 'i':
                opt_show_insn = true;
                break;

            case 'r':
                opt_show_regs = true;
                break;

            case 'z':
                opt_final_dump = true;
                break;

            case 'l':
            {
                istringstream iss(optarg);
                iss >> exec_limit;
                if (!iss)
                {
                    cerr << "Bad -l value: " << optarg << endl;
                    usage();
                }
                break;
            }

            default:
                usage();
        }
    }

    if (optind >= argc)
        usage();

    const char *filename = argv[optind];

    memory mem(memory_limit);
    if (!mem.load_file(filename))
        usage();

    if (opt_disassemble)
        disassemble(mem);

    cpu_single_hart cpu(mem);
    cpu.reset();
    cpu.set_mhartid(0);
    cpu.set_show_instructions(opt_show_insn);
    cpu.set_show_registers(opt_show_regs);

    cpu.run(exec_limit);

    if (opt_final_dump)
    {
        cpu.dump("");
        mem.dump();
    }

    return 0;
}
