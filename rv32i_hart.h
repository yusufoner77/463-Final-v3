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

class rv32i_hart : public rv32i_decode
{
public:
    rv32i_hart(memory &m) : mem(m) { }

    // --------- public control / status interface ---------

    void reset();                               // reset PC, regs, counters, and halt state
    void tick(const std::string &hdr = "");     // execute one instruction
    void dump(const std::string &hdr = "") const;

    void set_show_instructions(bool b) { show_instructions = b; }
    void set_show_registers(bool b)    { show_registers = b; }

    bool is_halted() const             { return halt; }
    const std::string &get_halt_reason() const { return halt_reason; }

    uint64_t get_insn_counter() const  { return insn_counter; }

    void set_mhartid(int i)           { mhartid = i; }

protected:
    registerfile regs;   // 32 general-purpose registers
    memory &mem;         // reference to simulated memory

private:
    // Width of the instruction field when printing trace lines
    static constexpr int instruction_width = 35;

    // Core executor called by tick()
    void exec(uint32_t insn, std::ostream *pos);

    // Illegal / unimplemented instruction handler
    void exec_illegal_insn(uint32_t insn, std::ostream *pos);

 // HELPERS

    // U-type
    void exec_lui(uint32_t insn, std::ostream *pos);
    void exec_auipc(uint32_t insn, std::ostream *pos);

    // J-type
    void exec_jal(uint32_t insn, std::ostream *pos);
    void exec_jalr(uint32_t insn, std::ostream *pos);

    // B-type (branches) – beq, bne, blt, bge, bltu, bgeu
    void exec_beq(uint32_t insn, std::ostream *pos);
    void exec_bne(uint32_t insn, std::ostream *pos);
    void exec_blt(uint32_t insn, std::ostream *pos);
    void exec_bge(uint32_t insn, std::ostream *pos);
    void exec_bltu(uint32_t insn, std::ostream *pos);
    void exec_bgeu(uint32_t insn, std::ostream *pos);

    // I-type loads – lb, lh, lw, lbu, lhu
    void exec_lb(uint32_t insn, std::ostream *pos);
    void exec_lh(uint32_t insn, std::ostream *pos);
    void exec_lw(uint32_t insn, std::ostream *pos);
    void exec_lbu(uint32_t insn, std::ostream *pos);
    void exec_lhu(uint32_t insn, std::ostream *pos);

    // S-type stores – sb, sh, sw
    void exec_sb(uint32_t insn, std::ostream *pos);
    void exec_sh(uint32_t insn, std::ostream *pos);
    void exec_sw(uint32_t insn, std::ostream *pos);

    void exec_addi(uint32_t insn, std::ostream *pos);
    void exec_slti(uint32_t insn, std::ostream *pos);
    void exec_sltiu(uint32_t insn, std::ostream *pos);
    void exec_xori(uint32_t insn, std::ostream *pos);
    void exec_ori(uint32_t insn, std::ostream *pos);
    void exec_andi(uint32_t insn, std::ostream *pos);
    void exec_slli(uint32_t insn, std::ostream *pos);
    void exec_srli(uint32_t insn, std::ostream *pos);
    void exec_srai(uint32_t insn, std::ostream *pos);

    void exec_add(uint32_t insn, std::ostream *pos);
    void exec_sub(uint32_t insn, std::ostream *pos);
    void exec_sll(uint32_t insn, std::ostream *pos);
    void exec_slt(uint32_t insn, std::ostream *pos);
    void exec_sltu(uint32_t insn, std::ostream *pos);
    void exec_xor(uint32_t insn, std::ostream *pos);
    void exec_srl(uint32_t insn, std::ostream *pos);
    void exec_sra(uint32_t insn, std::ostream *pos);
    void exec_or(uint32_t insn, std::ostream *pos);
    void exec_and(uint32_t insn, std::ostream *pos);

    void exec_system(uint32_t insn, std::ostream *pos);  // common system decoder

    void exec_ecall(uint32_t insn, std::ostream *pos);
    void exec_ebreak(uint32_t insn, std::ostream *pos);

    void exec_csrrw(uint32_t insn, std::ostream *pos);
    void exec_csrrs(uint32_t insn, std::ostream *pos);
    void exec_csrrc(uint32_t insn, std::ostream *pos);
    void exec_csrrwi(uint32_t insn, std::ostream *pos);
    void exec_csrrsi(uint32_t insn, std::ostream *pos);
    void exec_csrrci(uint32_t insn, std::ostream *pos);

    // ====== CORE STATE ======

    bool halt              = false;
    std::string halt_reason = "none";

    bool show_instructions = false;   // print each instruction as it executes
    bool show_registers    = false;   // dump registers before each instruction

    uint64_t insn_counter  = 0;       // count of executed instructions
    uint32_t pc            = 0;       // program counter
    uint32_t mhartid       = 0;       // hart ID
};
