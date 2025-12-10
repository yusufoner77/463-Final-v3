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
#include <ostream>

#include "rv32i_decode.h"
#include "memory.h"
#include "registerfile.h"

using namespace std;

//******************************************************************************
// The rv32i_hart class models a single RV32I hardware thread (hart) executing
// instructions out of a simulated memory. It owns a register file, maintains
// the program counter and halt state, and uses the rv32i_decode helpers to
// decode and execute instructions while optionally producing execution traces.
//******************************************************************************
class rv32i_hart : public rv32i_decode
{
public:
    //******************************************************************************
    // This constructor creates a hart and initializes mem
    //
    // Parameters:
    //   m - Reference to the simulated memory object backing this hart.
    //
    // Return value:
    //   None
    //******************************************************************************
    rv32i_hart(memory &m) : mem(m) { }

    // PUBLIC CONTROL - STATUS INTERFACE 

    void reset();                               
    void tick(const string &hdr = "");
    void dump(const string &hdr = "") const;

    //******************************************************************************
    // This function enables or disables instruction tracing. When enabled,
    // each instruction that executes is disassembled and printed along with
    // optional comments to the trace output
    //
    // Parameters:
    //   b - Boolean flag indicating whether to show instructions (true) or
    //       suppress them (false) during execution.
    //
    // Return value:
    //   None
    //******************************************************************************
    void set_show_instructions(bool b) { show_instructions = b; }
    
    
    //******************************************************************************
    // This function enables or disables register tracing. When enabled, the
    // complete register file is dumped before each instruction executes.
    //
    // Parameters:
    //   b - Boolean flag indicating whether to show registers (true) or not
    //       (false) before each instruction.
    //
    // Return value:
    //   None. The internal register-tracing flag is updated.
    //******************************************************************************
    void set_show_registers(bool b)    { show_registers = b; }

    //******************************************************************************
    // This function reports whether the hart has halted execution
    //
    // Parameters:
    //   None
    //
    // Return value:
    //   true  - if the hart is currently halted.
    //   false - if the hart is still able to execute instructions.
    //******************************************************************************
    bool is_halted() const             { return halt; }

    //******************************************************************************
    // This function returns a reference to a string describing the reason
    // why the hart halted
    //
    // Parameters:
    //   None.
    //
    // Return value:
    //   A const reference to the string containing the halt reason description.
    //******************************************************************************
    const string &get_halt_reason() const { return halt_reason; }

    //******************************************************************************
    // This function returns the number of instructions that have been executed
    // by the hart since the last reset().
    //
    // Parameters:
    //   None.
    //
    // Return value:
    //   A 64-bit unsigned integer count of the executed instructions.
    //******************************************************************************
    uint64_t get_insn_counter() const  { return insn_counter; }

    //******************************************************************************
    // This function sets the mhartid value associated with this hart. The
    // mhartid can be used to identify the hart in multi-hart systems, though
    // in this project only a single hart is simulated.
    //
    // Parameters:
    //   i - Integer value to assign to the mhartid field.
    //
    // Return value:
    //   None
    //******************************************************************************
    void set_mhartid(int i)           { mhartid = i; }

protected:
    registerfile regs;
    memory &mem;

private:
    static constexpr int instruction_width = 35;

    void exec(uint32_t insn, ostream *pos);

    void exec_illegal_insn(uint32_t insn, ostream *pos);

 // HELPERS

    void exec_lui(uint32_t insn, ostream *pos);
    void exec_auipc(uint32_t insn, ostream *pos);

    void exec_jal(uint32_t insn, ostream *pos);
    void exec_jalr(uint32_t insn, ostream *pos);

    void exec_beq(uint32_t insn, ostream *pos);
    void exec_bne(uint32_t insn, ostream *pos);
    void exec_blt(uint32_t insn, ostream *pos);
    void exec_bge(uint32_t insn, ostream *pos);
    void exec_bltu(uint32_t insn, ostream *pos);
    void exec_bgeu(uint32_t insn, ostream *pos);

    void exec_lb(uint32_t insn, ostream *pos);
    void exec_lh(uint32_t insn, ostream *pos);
    void exec_lw(uint32_t insn, ostream *pos);
    void exec_lbu(uint32_t insn, ostream *pos);
    void exec_lhu(uint32_t insn, ostream *pos);

    void exec_sb(uint32_t insn, ostream *pos);
    void exec_sh(uint32_t insn, ostream *pos);
    void exec_sw(uint32_t insn, ostream *pos);

    void exec_addi(uint32_t insn, ostream *pos);
    void exec_slti(uint32_t insn, ostream *pos);
    void exec_sltiu(uint32_t insn, ostream *pos);
    void exec_xori(uint32_t insn, ostream *pos);
    void exec_ori(uint32_t insn, ostream *pos);
    void exec_andi(uint32_t insn, ostream *pos);
    void exec_slli(uint32_t insn, ostream *pos);
    void exec_srli(uint32_t insn, ostream *pos);
    void exec_srai(uint32_t insn, ostream *pos);

    void exec_add(uint32_t insn, ostream *pos);
    void exec_sub(uint32_t insn, ostream *pos);
    void exec_sll(uint32_t insn, ostream *pos);
    void exec_slt(uint32_t insn, ostream *pos);
    void exec_sltu(uint32_t insn, ostream *pos);
    void exec_xor(uint32_t insn, ostream *pos);
    void exec_srl(uint32_t insn, ostream *pos);
    void exec_sra(uint32_t insn, ostream *pos);
    void exec_or(uint32_t insn, ostream *pos);
    void exec_and(uint32_t insn, ostream *pos);

    void exec_system(uint32_t insn, ostream *pos);

    void exec_ecall(uint32_t insn, ostream *pos);
    void exec_ebreak(uint32_t insn, ostream *pos);

    void exec_csrrw(uint32_t insn, ostream *pos);
    void exec_csrrs(uint32_t insn, ostream *pos);
    void exec_csrrc(uint32_t insn, ostream *pos);
    void exec_csrrwi(uint32_t insn, ostream *pos);
    void exec_csrrsi(uint32_t insn, ostream *pos);
    void exec_csrrci(uint32_t insn, ostream *pos);

    // CORE STATE

    bool halt              = false;
    string halt_reason = "none";

    bool show_instructions = false;
    bool show_registers    = false;

    uint64_t insn_counter  = 0;
    uint32_t pc            = 0;
    uint32_t mhartid       = 0;
};
