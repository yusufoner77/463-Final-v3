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

#include <string>
#include <cstdint>
#include "hex.h"

//******************************************************************************
// Extracts fields from a 32-bit instruction and translates it into readable 
// RV32I assembly mnemonic and operands
//******************************************************************************

class rv32i_decode : public hex
{
public:
    static constexpr uint32_t XLEN = 32;

    static constexpr int mnemonic_width = 8;

    static constexpr uint32_t opcode_lui    = 0b0110111;
    static constexpr uint32_t opcode_auipc  = 0b0010111;
    static constexpr uint32_t opcode_jal    = 0b1101111;
    static constexpr uint32_t opcode_jalr   = 0b1100111;
    static constexpr uint32_t opcode_btype  = 0b1100011;
    static constexpr uint32_t opcode_load   = 0b0000011;
    static constexpr uint32_t opcode_store  = 0b0100011;
    static constexpr uint32_t opcode_alu_imm= 0b0010011;
    static constexpr uint32_t opcode_alu_reg= 0b0110011;
    static constexpr uint32_t opcode_system = 0b1110011;

    static constexpr uint32_t funct3_beq  = 0b000;
    static constexpr uint32_t funct3_bne  = 0b001;
    static constexpr uint32_t funct3_blt  = 0b100;
    static constexpr uint32_t funct3_bge  = 0b101;
    static constexpr uint32_t funct3_bltu = 0b110;
    static constexpr uint32_t funct3_bgeu = 0b111;

    static constexpr uint32_t funct3_lb   = 0b000;
    static constexpr uint32_t funct3_lh   = 0b001;
    static constexpr uint32_t funct3_lw   = 0b010;
    static constexpr uint32_t funct3_lbu  = 0b100;
    static constexpr uint32_t funct3_lhu  = 0b101;

    static constexpr uint32_t funct3_sb   = 0b000;
    static constexpr uint32_t funct3_sh   = 0b001;
    static constexpr uint32_t funct3_sw   = 0b010;

    static constexpr uint32_t funct3_add_sub = 0b000;
    static constexpr uint32_t funct3_sll     = 0b001;
    static constexpr uint32_t funct3_slt     = 0b010;
    static constexpr uint32_t funct3_sltu    = 0b011;
    static constexpr uint32_t funct3_xor     = 0b100;
    static constexpr uint32_t funct3_srl_sra = 0b101;
    static constexpr uint32_t funct3_or      = 0b110;
    static constexpr uint32_t funct3_and     = 0b111;

    static constexpr uint32_t funct3_csrrw  = 0b001;
    static constexpr uint32_t funct3_csrrs  = 0b010;
    static constexpr uint32_t funct3_csrrc  = 0b011;
    static constexpr uint32_t funct3_csrrwi = 0b101;
    static constexpr uint32_t funct3_csrrsi = 0b110;
    static constexpr uint32_t funct3_csrrci = 0b111;

    static constexpr uint32_t funct7_add  = 0b0000000;
    static constexpr uint32_t funct7_sub  = 0b0100000;
    static constexpr uint32_t funct7_srl  = 0b0000000;
    static constexpr uint32_t funct7_sra  = 0b0100000;

    static std::string decode(uint32_t addr, uint32_t insn);

    static uint32_t get_opcode(uint32_t insn);
    static uint32_t get_rd(uint32_t insn);
    static uint32_t get_rs1(uint32_t insn);
    static uint32_t get_rs2(uint32_t insn);
    static uint32_t get_funct3(uint32_t insn);
    static uint32_t get_funct7(uint32_t insn);

    static int32_t get_imm_i(uint32_t insn);
    static int32_t get_imm_u(uint32_t insn);
    static int32_t get_imm_b(uint32_t insn);
    static int32_t get_imm_s(uint32_t insn);
    static int32_t get_imm_j(uint32_t insn);

    static std::string render_illegal_insn();
    static std::string render_lui(uint32_t insn);
    static std::string render_auipc(uint32_t insn);
    static std::string render_jal(uint32_t addr, uint32_t insn);
    static std::string render_jalr(uint32_t insn);
    static std::string render_btype(uint32_t addr, uint32_t insn, const char *mnemonic);
    static std::string render_itype_load(uint32_t insn, const char *mnemonic);
    static std::string render_stype(uint32_t insn, const char *mnemonic);
    static std::string render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i);
    static std::string render_rtype(uint32_t insn, const char *mnemonic);
    static std::string render_system(uint32_t insn); // csrr*, ecall, ebreak

    static std::string render_reg(int r);
    static std::string render_base_disp(uint32_t reg, int32_t imm);
    static std::string render_mnemonic(const std::string &mnemonic);
};
