//******************************************************************************
// Yusuf Oner
// z2048138
// CSCI 463
//
// I certify that this is my own work, and where applicable an extension
// of the starter code for the assignment.
//
//******************************************************************************

#include "rv32i_hart.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// PUBLIC INTERFACE

//******************************************************************************
// This function resets the rv32i object and the register file. 
//
// Parameters:
//   None.
//
// Return value:
//   None
//******************************************************************************
void rv32i_hart::reset()
{
    pc = 0;
    regs.reset();
    halt = false;
    halt_reason = "none";
    insn_counter = 0;
}

//******************************************************************************
// This function prints the complete state of the hart
// Parameters:
//   hdr - A string printed at the beginning of each output line
//
// Return value:
//   None. Output is written directly to std::cout.
//******************************************************************************
void rv32i_hart::dump(const string &hdr) const
{
    regs.dump(hdr);
    cout << hdr << " pc " << hex::to_hex32(pc) << endl;
}

//******************************************************************************
// This function simulates a single CPU cycle. It tells the simulator to execute an
// instruction
//
// Parameters:
//   hdr - A string printed at the start of each trace line when register or
//         instruction logging is enabled
//
// Return value:
//   None
//******************************************************************************
void rv32i_hart::tick(const string &hdr)
{
    if (halt)
        return;

    if (show_registers)
        dump(hdr);    

    if (pc & 0x3)
    {
        halt = true;
        halt_reason = "PC alignment error";
        return;
    }

    uint32_t cur_pc = pc;
    uint32_t insn   = mem.get32(cur_pc);

    ++insn_counter;

    ostream *pos = nullptr;
    if (show_instructions)
    {
        pos = &cout;
        cout << hdr
             << hex::to_hex32(cur_pc) << ": "
             << hex::to_hex32(insn) << "  ";
    }

    exec(insn, pos);

    if (show_instructions)
        cout << endl;
}


//******************************************************************************
// This function will execute the given RV32I instruction by making use of the get_xxx() 
// methods to extract the needed instruction fields to decode the instruction and invoke
// the associated exec_xxx() helper function by using the same sort of switch-logic from version 2
//
// Parameters:
//   insn - The 32-bit instruction fetched from memory.
//   pos  - Pointer to an output stream used to print trace information.
//
// Return value:
//   None
//******************************************************************************
void rv32i_hart::exec(uint32_t insn, ostream *pos)
{
    if (pos)
    {
        string s = rv32i_decode::decode(pc, insn);
        *pos << left << setw(instruction_width) << s << right;
    }

    uint32_t opcode = get_opcode(insn);

    switch (opcode)
    {
        case opcode_lui:      exec_lui(insn, pos);      break;
        case opcode_auipc:    exec_auipc(insn, pos);    break;
        case opcode_jal:      exec_jal(insn, pos);      break;
        case opcode_jalr:     exec_jalr(insn, pos);     break;

        case opcode_btype:
        {
            uint32_t f3 = get_funct3(insn);
            switch (f3)
            {
                case funct3_beq:  exec_beq(insn, pos);  break;
                case funct3_bne:  exec_bne(insn, pos);  break;
                case funct3_blt:  exec_blt(insn, pos);  break;
                case funct3_bge:  exec_bge(insn, pos);  break;
                case funct3_bltu: exec_bltu(insn, pos); break;
                case funct3_bgeu: exec_bgeu(insn, pos); break;
                default:          exec_illegal_insn(insn, pos); break;
            }
        }
        break;

        case opcode_load:
        {
            uint32_t f3 = get_funct3(insn);
            switch (f3)
            {
                case funct3_lb:  exec_lb(insn, pos);  break;
                case funct3_lh:  exec_lh(insn, pos);  break;
                case funct3_lw:  exec_lw(insn, pos);  break;
                case funct3_lbu: exec_lbu(insn, pos); break;
                case funct3_lhu: exec_lhu(insn, pos); break;
                default:         exec_illegal_insn(insn, pos); break;
            }
        }
        break;

        case opcode_store:
        {
            uint32_t f3 = get_funct3(insn);
            switch (f3)
            {
                case funct3_sb: exec_sb(insn, pos); break;
                case funct3_sh: exec_sh(insn, pos); break;
                case funct3_sw: exec_sw(insn, pos); break;
                default:        exec_illegal_insn(insn, pos); break;
            }
        }
        break;

        case opcode_alu_imm:
        {
            uint32_t f3 = get_funct3(insn);
            switch (f3)
            {
                case funct3_add_sub: exec_addi(insn, pos);  break; // addi
                case funct3_slt:     exec_slti(insn, pos);  break; // 0b010
                case funct3_sltu:    exec_sltiu(insn, pos); break; // 0b011
                case funct3_xor:     exec_xori(insn, pos);  break;
                case funct3_or:      exec_ori(insn, pos);   break;
                case funct3_and:     exec_andi(insn, pos);  break;
                case funct3_sll:     exec_slli(insn, pos);  break;
                case funct3_srl_sra:
                {
                    uint32_t f7 = get_funct7(insn);
                    if (f7 == funct7_srl)      exec_srli(insn, pos);
                    else if (f7 == funct7_sra) exec_srai(insn, pos);
                    else                       exec_illegal_insn(insn, pos);
                    break;
                }
                default:
                    exec_illegal_insn(insn, pos);
            }
        }
        break;

        case opcode_alu_reg:
        {
            uint32_t f3 = get_funct3(insn);
            uint32_t f7 = get_funct7(insn);

            switch (f3)
            {
                case funct3_add_sub:
                    if (f7 == funct7_add) exec_add(insn, pos);
                    else if (f7 == funct7_sub) exec_sub(insn, pos);
                    else exec_illegal_insn(insn, pos);
                    break;

                case funct3_sll:
                    if (f7 == funct7_add) exec_sll(insn, pos);
                    else exec_illegal_insn(insn, pos);
                    break;

                case funct3_slt:
                    if (f7 == funct7_add) exec_slt(insn, pos);
                    else exec_illegal_insn(insn, pos);
                    break;

                case funct3_sltu:
                    if (f7 == funct7_add) exec_sltu(insn, pos);
                    else exec_illegal_insn(insn, pos);
                    break;

                case funct3_xor:
                    if (f7 == funct7_add) exec_xor(insn, pos);
                    else exec_illegal_insn(insn, pos);
                    break;

                case funct3_srl_sra:
                    if (f7 == funct7_srl) exec_srl(insn, pos);
                    else if (f7 == funct7_sra) exec_sra(insn, pos);
                    else exec_illegal_insn(insn, pos);
                    break;

                case funct3_or:
                    if (f7 == funct7_add) exec_or(insn, pos);
                    else exec_illegal_insn(insn, pos);
                    break;

                case funct3_and:
                    if (f7 == funct7_add) exec_and(insn, pos);
                    else exec_illegal_insn(insn, pos);
                    break;

                default:
                    exec_illegal_insn(insn, pos);
            }
        }
        break;

        case opcode_system:
            exec_system(insn, pos);
            break;

        default:
            exec_illegal_insn(insn, pos);
            break;
    }
}

//******************************************************************************
// This function handles illegal or unimplemented instructions
// Parameters:
//   insn - The 32-bit illegal instruction that triggered the error.
//   pos  - Optional pointer to an output stream for printing the error
//          message
//
// Return value:
//   None
//******************************************************************************
void rv32i_hart::exec_illegal_insn(uint32_t /*insn*/, ostream * /*pos*/)
{
    halt = true;
    halt_reason = "Illegal instruction";
}

//******************************************************************************
// HELPER FUNCTIONS
//******************************************************************************

//******************************************************************************
// U-TYPE
// These functions implement the RV32I U-type (upper-immediate) instructions,
// including LUI and AUIPC. They extract the 20-bit U-immediate, shift or add
// it to the PC as required, and write the result into register rd.
// 
// Parameters:
//   insn - the 32-bit encoded instruction being executed
//   pos  - optional ostream used to print execution trace comments; if nullptr,
//          no trace output is produced
//
// Return value:
//   None
//******************************************************************************

void rv32i_hart::exec_lui(uint32_t insn, ostream *pos)
{
    uint32_t rd    = get_rd(insn);
    int32_t imm_u  = get_imm_u(insn);   // << 12 already
    uint32_t res   = static_cast<uint32_t>(imm_u);

    if (pos)
    {
        *pos << "// x" << rd
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, imm_u);
    pc += 4;
}

void rv32i_hart::exec_auipc(uint32_t insn, ostream *pos)
{
    uint32_t rd       = get_rd(insn);
    uint32_t pc_before = pc;
    int32_t imm_u     = get_imm_u(insn);
    uint32_t res      = pc_before + static_cast<uint32_t>(imm_u);

    if (pos)
    {
        *pos << "// x" << rd
             << " = " << hex::to_hex0x32(pc_before)
             << " + " << hex::to_hex0x32(static_cast<uint32_t>(imm_u))
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, static_cast<int32_t>(res));
    pc += 4;
}

//******************************************************************************
// J-TYPE
// These functions implement the jump instructions JAL and JALR. They
// compute a new PC from a sign-extended immediate (and optionally rs1),
// write the return address into rd, and perform the jump
// 
// Parameters:
//   insn - the 32-bit encoded instruction being executed
//   pos  - optional ostream to print trace comments, or nullptr for no output
//
// Return value:
//   None. These functions modify the destination register and update the PC.
//******************************************************************************

void rv32i_hart::exec_jal(uint32_t insn, ostream *pos)
{
    uint32_t rd        = get_rd(insn);
    uint32_t pc_before = pc;
    int32_t imm_j      = get_imm_j(insn);
    uint32_t link      = pc_before + 4;
    uint32_t target    = pc_before + static_cast<uint32_t>(imm_j);

    if (pos)
    {
        *pos << "// x" << rd
             << " = " << hex::to_hex0x32(link)
             << ",  pc = " << hex::to_hex0x32(pc_before)
             << " + "    << hex::to_hex0x32(static_cast<uint32_t>(imm_j))
             << " = "    << hex::to_hex0x32(target);
    }

    regs.set(rd, static_cast<int32_t>(link));
    pc = target;
}

void rv32i_hart::exec_jalr(uint32_t insn, ostream *pos)
{
    uint32_t rd        = get_rd(insn);
    uint32_t rs1       = get_rs1(insn);
    uint32_t pc_before = pc;
    int32_t imm_i      = get_imm_i(insn);

    uint32_t base   = static_cast<uint32_t>(regs.get(rs1));
    uint32_t link   = pc_before + 4;
    uint32_t sum    = base + static_cast<uint32_t>(imm_i);
    uint32_t target = sum & ~1u;

    if (pos)
    {
        *pos << "// x" << rd
             << " = " << hex::to_hex0x32(link)
             << ",  pc = (" << hex::to_hex0x32(static_cast<uint32_t>(imm_i))
             << " + "       << hex::to_hex0x32(base)
             << ") & 0xfffffffe = " << hex::to_hex0x32(target);
    }

    regs.set(rd, static_cast<int32_t>(link));
    pc = target;
}

//******************************************************************************
// This function constructs and returns a formatted textual comment describing
// the behavior of a branch instruction.

// Parameters:
//   rs1_val      - The 32-bit value of the rs1 register used in the branch.
//   rs2_val      - The 32-bit value of the rs2 register used in the branch.
//   offset       - The signed, 32-bit branch offset computed from the
//                  instruction’s immediate field.
//   take_branch  - Boolean indicating whether the branch condition evaluated
//                  true (branch taken) or false (branch not taken).
//
// Return value:
//   A string describing the branch decision and the effect on the program counter.
//******************************************************************************

static void branch_comment(ostream *pos,
                           const char *op,
                           bool is_unsigned,
                           uint32_t pc_before,
                           int32_t imm_b,
                           uint32_t v1_u,
                           uint32_t v2_u,
                           bool take)
{
    if (!pos) return;

    uint32_t offset      = static_cast<uint32_t>(imm_b);
    uint32_t fallthrough = pc_before + 4;
    uint32_t target      = pc_before + static_cast<uint32_t>(imm_b);

    *pos << "// pc += ("
         << hex::to_hex0x32(v1_u) << ' ' << op;
    if (is_unsigned)
        *pos << 'U';
    *pos << ' ' << hex::to_hex0x32(v2_u)
         << " ? " << hex::to_hex0x32(offset)
         << " : 4) = "
         << hex::to_hex0x32(take ? target : fallthrough);
}

//******************************************************************************
// B-TYPE
// These functions implement the conditional branch instructions. They
// evaluate the branch condition using rs1 and rs2, compute the sign-extended
// branch offset, and either add the offset to the PC (branch taken) or advance
// to the next sequential instruction.
// 
// Parameters:
//   insn - the 32-bit encoded instruction being executed
//   pos  - optional ostream for printing execution trace comments
//
// Return value:
//   None
//******************************************************************************

void rv32i_hart::exec_beq(uint32_t insn, ostream *pos)
{
    uint32_t rs1       = get_rs1(insn);
    uint32_t rs2       = get_rs2(insn);
    uint32_t pc_before = pc;
    int32_t imm_b      = get_imm_b(insn);

    int32_t v1 = regs.get(rs1);
    int32_t v2 = regs.get(rs2);
    bool take = (v1 == v2);

    branch_comment(pos, "==", false, pc_before, imm_b,
                   static_cast<uint32_t>(v1),
                   static_cast<uint32_t>(v2),
                   take);

    pc = take ? pc_before + static_cast<uint32_t>(imm_b)
              : pc_before + 4;
}

void rv32i_hart::exec_bne(uint32_t insn, ostream *pos)
{
    uint32_t rs1       = get_rs1(insn);
    uint32_t rs2       = get_rs2(insn);
    uint32_t pc_before = pc;
    int32_t imm_b      = get_imm_b(insn);

    int32_t v1 = regs.get(rs1);
    int32_t v2 = regs.get(rs2);
    bool take = (v1 != v2);

    branch_comment(pos, "!=", false, pc_before, imm_b,
                   static_cast<uint32_t>(v1),
                   static_cast<uint32_t>(v2),
                   take);

    pc = take ? pc_before + static_cast<uint32_t>(imm_b)
              : pc_before + 4;
}

void rv32i_hart::exec_blt(uint32_t insn, ostream *pos)
{
    uint32_t rs1       = get_rs1(insn);
    uint32_t rs2       = get_rs2(insn);
    uint32_t pc_before = pc;
    int32_t imm_b      = get_imm_b(insn);

    int32_t v1 = regs.get(rs1);
    int32_t v2 = regs.get(rs2);
    bool take = (v1 < v2);

    branch_comment(pos, "<", false, pc_before, imm_b,
                   static_cast<uint32_t>(v1),
                   static_cast<uint32_t>(v2),
                   take);

    pc = take ? pc_before + static_cast<uint32_t>(imm_b)
              : pc_before + 4;
}

void rv32i_hart::exec_bge(uint32_t insn, ostream *pos)
{
    uint32_t rs1       = get_rs1(insn);
    uint32_t rs2       = get_rs2(insn);
    uint32_t pc_before = pc;
    int32_t imm_b      = get_imm_b(insn);

    int32_t v1 = regs.get(rs1);
    int32_t v2 = regs.get(rs2);
    bool take = (v1 >= v2);

    branch_comment(pos, ">=", false, pc_before, imm_b,
                   static_cast<uint32_t>(v1),
                   static_cast<uint32_t>(v2),
                   take);

    pc = take ? pc_before + static_cast<uint32_t>(imm_b)
              : pc_before + 4;
}

void rv32i_hart::exec_bltu(uint32_t insn, ostream *pos)
{
    uint32_t rs1       = get_rs1(insn);
    uint32_t rs2       = get_rs2(insn);
    uint32_t pc_before = pc;
    int32_t imm_b      = get_imm_b(insn);

    uint32_t v1 = static_cast<uint32_t>(regs.get(rs1));
    uint32_t v2 = static_cast<uint32_t>(regs.get(rs2));
    bool take = (v1 < v2);

    branch_comment(pos, "<", true, pc_before, imm_b, v1, v2, take);

    pc = take ? pc_before + static_cast<uint32_t>(imm_b)
              : pc_before + 4;
}

void rv32i_hart::exec_bgeu(uint32_t insn, ostream *pos)
{
    uint32_t rs1       = get_rs1(insn);
    uint32_t rs2       = get_rs2(insn);
    uint32_t pc_before = pc;
    int32_t imm_b      = get_imm_b(insn);

    uint32_t v1 = static_cast<uint32_t>(regs.get(rs1));
    uint32_t v2 = static_cast<uint32_t>(regs.get(rs2));
    bool take = (v1 >= v2);

    branch_comment(pos, ">=", true, pc_before, imm_b, v1, v2, take);

    pc = take ? pc_before + static_cast<uint32_t>(imm_b)
              : pc_before + 4;
}

//******************************************************************************
// I-TYPE
// These functions implement the load instructions, which compute an
// effective address as rs1 + imm, read memory at that address, optionally
// apply sign or zero extension depending on instruction type, and write the
// resulting value into rd.
// 
// Parameters:
//   insn - the 32-bit load instruction to execute
//   pos  - optional ostream for writing formatted trace comments
//
// Return value:
//   None
//******************************************************************************

void rv32i_hart::exec_lb(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t base = static_cast<uint32_t>(regs.get(rs1));
    uint32_t addr = base + static_cast<uint32_t>(imm_i);
    int32_t val   = mem.get8_sx(addr);

    if (pos)
    {
        *pos << "// x" << rd
             << " = sx(m8(" << hex::to_hex0x32(base)
             << " + "       << hex::to_hex0x32(static_cast<uint32_t>(imm_i))
             << ")) = "     << hex::to_hex0x32(static_cast<uint32_t>(val));
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lh(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t base = static_cast<uint32_t>(regs.get(rs1));
    uint32_t addr = base + static_cast<uint32_t>(imm_i);
    int32_t val   = mem.get16_sx(addr);

    if (pos)
    {
        *pos << "// x" << rd
             << " = sx(m16(" << hex::to_hex0x32(base)
             << " + "        << hex::to_hex0x32(static_cast<uint32_t>(imm_i))
             << ")) = "      << hex::to_hex0x32(static_cast<uint32_t>(val));
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lw(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t base = static_cast<uint32_t>(regs.get(rs1));
    uint32_t addr = base + static_cast<uint32_t>(imm_i);
    int32_t val   = mem.get32_sx(addr);

    if (pos)
    {
        *pos << "// x" << rd
             << " = sx(m32(" << hex::to_hex0x32(base)
             << " + "        << hex::to_hex0x32(static_cast<uint32_t>(imm_i))
             << ")) = "      << hex::to_hex0x32(static_cast<uint32_t>(val));
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lbu(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t base = static_cast<uint32_t>(regs.get(rs1));
    uint32_t addr = base + static_cast<uint32_t>(imm_i);
    uint8_t  b    = mem.get8(addr);
    uint32_t val  = static_cast<uint32_t>(b);

    if (pos)
    {
        *pos << "// x" << rd
             << " = zx(m8(" << hex::to_hex0x32(base)
             << " + "        << hex::to_hex0x32(static_cast<uint32_t>(imm_i))
             << ")) = "      << hex::to_hex0x32(val);
    }

    regs.set(rd, static_cast<int32_t>(val));
    pc += 4;
}

void rv32i_hart::exec_lhu(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t base = static_cast<uint32_t>(regs.get(rs1));
    uint32_t addr = base + static_cast<uint32_t>(imm_i);
    uint16_t h    = mem.get16(addr);
    uint32_t val  = static_cast<uint32_t>(h);

    if (pos)
    {
        *pos << "// x" << rd
             << " = zx(m16(" << hex::to_hex0x32(base)
             << " + "         << hex::to_hex0x32(static_cast<uint32_t>(imm_i))
             << ")) = "       << hex::to_hex0x32(val);
    }

    regs.set(rd, static_cast<int32_t>(val));
    pc += 4;
}

//******************************************************************************
// S-TYPE
// These functions implement the store instructions SB, SH, and SW. They
// compute an address from rs1 + imm, retrieve the source value from
// rs2, and write 8, 16, or 32 bits into simulated memory at the computed
// address.
// 
// Parameters:
//   insn - the 32-bit store instruction being executed
//   pos  - optional ostream for trace output
//
// Return value:
//   None
//******************************************************************************

void rv32i_hart::exec_sb(uint32_t insn, ostream *pos)
{
    uint32_t rs1  = get_rs1(insn);
    uint32_t rs2  = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn);

    uint32_t base = static_cast<uint32_t>(regs.get(rs1));
    uint32_t addr = base + static_cast<uint32_t>(imm_s);
    uint32_t val  = static_cast<uint32_t>(regs.get(rs2)) & 0xffu;

    if (pos)
    {
        *pos << "// m8(" << hex::to_hex0x32(base)
             << " + "     << hex::to_hex0x32(static_cast<uint32_t>(imm_s))
             << ") = "    << hex::to_hex0x32(val);
    }

    mem.set8(addr, static_cast<uint8_t>(val));
    pc += 4;
}

void rv32i_hart::exec_sh(uint32_t insn, ostream *pos)
{
    uint32_t rs1  = get_rs1(insn);
    uint32_t rs2  = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn);

    uint32_t base = static_cast<uint32_t>(regs.get(rs1));
    uint32_t addr = base + static_cast<uint32_t>(imm_s);
    uint32_t val  = static_cast<uint32_t>(regs.get(rs2)) & 0xffffu;

    if (pos)
    {
        *pos << "// m16(" << hex::to_hex0x32(base)
             << " + "      << hex::to_hex0x32(static_cast<uint32_t>(imm_s))
             << ") = "     << hex::to_hex0x32(val);
    }

    mem.set16(addr, static_cast<uint16_t>(val));
    pc += 4;
}

void rv32i_hart::exec_sw(uint32_t insn, ostream *pos)
{
    uint32_t rs1  = get_rs1(insn);
    uint32_t rs2  = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn);

    uint32_t base = static_cast<uint32_t>(regs.get(rs1));
    uint32_t addr = base + static_cast<uint32_t>(imm_s);
    uint32_t val  = static_cast<uint32_t>(regs.get(rs2));

    if (pos)
    {
        *pos << "// m32(" << hex::to_hex0x32(base)
             << " + "      << hex::to_hex0x32(static_cast<uint32_t>(imm_s))
             << ") = "     << hex::to_hex0x32(val);
    }

    mem.set32(addr, val);
    pc += 4;
}

//******************************************************************************
// I-TYPE ALU
// These functions implement the immediate arithmetic and logical
// instructions. They compute a result using rs1 and a sign-extended immediate
// (or shift amount), then store the result into rd.
// 
// Parameters:
//   insn - the 32-bit ALU-immediate instruction
//   pos  - optional ostream for trace comments
//
// Return value:
//   None
//******************************************************************************

void rv32i_hart::exec_addi(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    int32_t v1  = regs.get(rs1);
    int32_t res = v1 + imm_i;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(static_cast<uint32_t>(v1))
             << " + " << hex::to_hex0x32(static_cast<uint32_t>(imm_i))
             << " = " << hex::to_hex0x32(static_cast<uint32_t>(res));
    }

    regs.set(rd, res);
    pc += 4;
}

void rv32i_hart::exec_slti(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    int32_t v1   = regs.get(rs1);
    bool cond    = (v1 < imm_i);
    int32_t res  = cond ? 1 : 0;

    if (pos)
    {
        *pos << "// x" << rd << " = ("
             << hex::to_hex0x32(static_cast<uint32_t>(v1))
             << " < " << imm_i
             << ") ? 1 : 0 = "
             << hex::to_hex0x32(static_cast<uint32_t>(res));
    }

    regs.set(rd, res);
    pc += 4;
}

void rv32i_hart::exec_sltiu(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t v1  = static_cast<uint32_t>(regs.get(rs1));
    uint32_t uimm = static_cast<uint32_t>(imm_i);
    bool cond    = (v1 < uimm);
    int32_t res  = cond ? 1 : 0;

    if (pos)
    {
        *pos << "// x" << rd << " = ("
             << hex::to_hex0x32(v1)
             << " <U " << imm_i
             << ") ? 1 : 0 = "
             << hex::to_hex0x32(static_cast<uint32_t>(res));
    }

    regs.set(rd, res);
    pc += 4;
}

void rv32i_hart::exec_xori(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t v1   = static_cast<uint32_t>(regs.get(rs1));
    uint32_t uimm = static_cast<uint32_t>(imm_i);
    uint32_t res  = v1 ^ uimm;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(v1)
             << " ^ " << hex::to_hex0x32(uimm)
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, static_cast<int32_t>(res));
    pc += 4;
}

void rv32i_hart::exec_ori(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t v1   = static_cast<uint32_t>(regs.get(rs1));
    uint32_t uimm = static_cast<uint32_t>(imm_i);
    uint32_t res  = v1 | uimm;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(v1)
             << " | " << hex::to_hex0x32(uimm)
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, static_cast<int32_t>(res));
    pc += 4;
}

void rv32i_hart::exec_andi(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    uint32_t v1   = static_cast<uint32_t>(regs.get(rs1));
    uint32_t uimm = static_cast<uint32_t>(imm_i);
    uint32_t res  = v1 & uimm;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(v1)
             << " & " << hex::to_hex0x32(uimm)
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, static_cast<int32_t>(res));
    pc += 4;
}

void rv32i_hart::exec_slli(uint32_t insn, ostream *pos)
{
    uint32_t rd    = get_rd(insn);
    uint32_t rs1   = get_rs1(insn);
    int32_t imm_i  = get_imm_i(insn);
    uint32_t shamt = static_cast<uint32_t>(imm_i) & 0x1f;

    uint32_t v1  = static_cast<uint32_t>(regs.get(rs1));
    uint32_t res = v1 << shamt;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(v1)
             << " << " << shamt
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, static_cast<int32_t>(res));
    pc += 4;
}

void rv32i_hart::exec_srli(uint32_t insn, ostream *pos)
{
    uint32_t rd    = get_rd(insn);
    uint32_t rs1   = get_rs1(insn);
    int32_t imm_i  = get_imm_i(insn);
    uint32_t shamt = static_cast<uint32_t>(imm_i) & 0x1f;

    uint32_t v1  = static_cast<uint32_t>(regs.get(rs1));
    uint32_t res = v1 >> shamt;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(v1)
             << " >> " << shamt
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, static_cast<int32_t>(res));
    pc += 4;
}

void rv32i_hart::exec_srai(uint32_t insn, ostream *pos)
{
    uint32_t rd    = get_rd(insn);
    uint32_t rs1   = get_rs1(insn);
    int32_t imm_i  = get_imm_i(insn);
    uint32_t shamt = static_cast<uint32_t>(imm_i) & 0x1f;

    int32_t v1  = regs.get(rs1);
    int32_t res = v1 >> shamt;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(static_cast<uint32_t>(v1))
             << " >> " << shamt
             << " = " << hex::to_hex0x32(static_cast<uint32_t>(res));
    }

    regs.set(rd, res);
    pc += 4;
}

//******************************************************************************
// R-TYPE ALU
// These functions implement the register-to-register ALU instructions,
// performing arithmetic or logical operations using rs1 and rs2 and writing
// the result into rd.
// 
// Parameters:
//   insn - the 32-bit R-type instruction being executed
//   pos  - optional ostream for trace output
//
// Return value:
//   None
//******************************************************************************

void rv32i_hart::exec_add(uint32_t insn, ostream *pos)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    int32_t v1  = regs.get(rs1);
    int32_t v2  = regs.get(rs2);
    int32_t res = v1 + v2;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(static_cast<uint32_t>(v1))
             << " + " << hex::to_hex0x32(static_cast<uint32_t>(v2))
             << " = " << hex::to_hex0x32(static_cast<uint32_t>(res));
    }

    regs.set(rd, res);
    pc += 4;
}

void rv32i_hart::exec_sub(uint32_t insn, ostream *pos)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    int32_t v1  = regs.get(rs1);
    int32_t v2  = regs.get(rs2);
    int32_t res = v1 - v2;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(static_cast<uint32_t>(v1))
             << " - " << hex::to_hex0x32(static_cast<uint32_t>(v2))
             << " = " << hex::to_hex0x32(static_cast<uint32_t>(res));
    }

    regs.set(rd, res);
    pc += 4;
}

void rv32i_hart::exec_sll(uint32_t insn, ostream *pos)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    uint32_t v1   = static_cast<uint32_t>(regs.get(rs1));
    uint32_t shamt = static_cast<uint32_t>(regs.get(rs2)) & 0x1f;
    uint32_t res  = v1 << shamt;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(v1)
             << " << " << shamt
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, static_cast<int32_t>(res));
    pc += 4;
}

void rv32i_hart::exec_slt(uint32_t insn, ostream *pos)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    int32_t v1   = regs.get(rs1);
    int32_t v2   = regs.get(rs2);
    bool cond    = (v1 < v2);
    int32_t res  = cond ? 1 : 0;

    if (pos)
    {
        *pos << "// x" << rd << " = ("
             << hex::to_hex0x32(static_cast<uint32_t>(v1))
             << " < " << hex::to_hex0x32(static_cast<uint32_t>(v2))
             << ") ? 1 : 0 = "
             << hex::to_hex0x32(static_cast<uint32_t>(res));
    }

    regs.set(rd, res);
    pc += 4;
}

void rv32i_hart::exec_sltu(uint32_t insn, ostream *pos)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    uint32_t v1  = static_cast<uint32_t>(regs.get(rs1));
    uint32_t v2  = static_cast<uint32_t>(regs.get(rs2));
    bool cond    = (v1 < v2);
    int32_t res  = cond ? 1 : 0;

    if (pos)
    {
        *pos << "// x" << rd << " = ("
             << hex::to_hex0x32(v1)
             << " <U " << hex::to_hex0x32(v2)
             << ") ? 1 : 0 = "
             << hex::to_hex0x32(static_cast<uint32_t>(res));
    }

    regs.set(rd, res);
    pc += 4;
}

void rv32i_hart::exec_xor(uint32_t insn, ostream *pos)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    uint32_t v1  = static_cast<uint32_t>(regs.get(rs1));
    uint32_t v2  = static_cast<uint32_t>(regs.get(rs2));
    uint32_t res = v1 ^ v2;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(v1)
             << " ^ " << hex::to_hex0x32(v2)
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, static_cast<int32_t>(res));
    pc += 4;
}

void rv32i_hart::exec_srl(uint32_t insn, ostream *pos)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    uint32_t v1   = static_cast<uint32_t>(regs.get(rs1));
    uint32_t shamt = static_cast<uint32_t>(regs.get(rs2)) & 0x1f;
    uint32_t res  = v1 >> shamt;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(v1)
             << " >> " << shamt
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, static_cast<int32_t>(res));
    pc += 4;
}

void rv32i_hart::exec_sra(uint32_t insn, ostream *pos)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    int32_t v1    = regs.get(rs1);
    uint32_t shamt = static_cast<uint32_t>(regs.get(rs2)) & 0x1f;
    int32_t res   = v1 >> shamt;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(static_cast<uint32_t>(v1))
             << " >> " << shamt
             << " = " << hex::to_hex0x32(static_cast<uint32_t>(res));
    }

    regs.set(rd, res);
    pc += 4;
}

void rv32i_hart::exec_or(uint32_t insn, ostream *pos)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    uint32_t v1  = static_cast<uint32_t>(regs.get(rs1));
    uint32_t v2  = static_cast<uint32_t>(regs.get(rs2));
    uint32_t res = v1 | v2;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(v1)
             << " | " << hex::to_hex0x32(v2)
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, static_cast<int32_t>(res));
    pc += 4;
}

void rv32i_hart::exec_and(uint32_t insn, ostream *pos)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    uint32_t v1  = static_cast<uint32_t>(regs.get(rs1));
    uint32_t v2  = static_cast<uint32_t>(regs.get(rs2));
    uint32_t res = v1 & v2;

    if (pos)
    {
        *pos << "// x" << rd << " = "
             << hex::to_hex0x32(v1)
             << " & " << hex::to_hex0x32(v2)
             << " = " << hex::to_hex0x32(res);
    }

    regs.set(rd, static_cast<int32_t>(res));
    pc += 4;
}

//******************************************************************************
// SYSTEM
// These functions implement the SYSTEM instructions, including ECALL,
// EBREAK, and the CSR (Control and Status Register) read/modify/write family.
// 
// Parameters:
//   insn - the 32-bit SYSTEM or CSR instruction to execute
//   pos  - optional ostream for printing execution trace comments
//
// Return value:
//   None
//******************************************************************************


void rv32i_hart::exec_system(uint32_t insn, ostream *pos)
{
    uint32_t f3 = get_funct3(insn);

    if (f3 == 0)
    {
        // ecall / ebreak
        if (insn == 0x00000073)
            exec_ecall(insn, pos);
        else if (insn == 0x00100073)
            exec_ebreak(insn, pos);
        else
            exec_illegal_insn(insn, pos);
    }
    else
    {
        switch (f3)
        {
            case funct3_csrrw:  exec_csrrw(insn, pos);  break;
            case funct3_csrrs:  exec_csrrs(insn, pos);  break;
            case funct3_csrrc:  exec_csrrc(insn, pos);  break;
            case funct3_csrrwi: exec_csrrwi(insn, pos); break;
            case funct3_csrrsi: exec_csrrsi(insn, pos); break;
            case funct3_csrrci: exec_csrrci(insn, pos); break;
            default:            exec_illegal_insn(insn, pos); break;
        }
    }
}

void rv32i_hart::exec_ecall(uint32_t, ostream *pos)
{
    if (pos)
        *pos << "// ECALL";
    halt = true;
    halt_reason = "ECALL instruction";
}

void rv32i_hart::exec_ebreak(uint32_t, ostream *pos)
{
    if (pos)
        *pos << "// HALT";
    halt = true;
    halt_reason = "EBREAK instruction";
}

void rv32i_hart::exec_csrrw(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    uint32_t csr  = get_imm_i(insn) & 0xfff;

    uint32_t rs1_val = static_cast<uint32_t>(regs.get(rs1));

    if (pos)
    {
        *pos << "// x" << rd << " = 0";
    }

    regs.set(rd, 0);

    (void)csr;
    (void)rs1_val;

    pc += 4;
}

void rv32i_hart::exec_csrrs(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t csr  = get_imm_i(insn) & 0xfff;

    if (pos)
    {
        *pos << "// x" << rd << " = 0";
    }

    regs.set(rd, 0);
    (void)csr;
    pc += 4;
}

void rv32i_hart::exec_csrrc(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t csr  = get_imm_i(insn) & 0xfff;

    if (pos)
    {
        *pos << "// x" << rd << " = 0";
    }

    regs.set(rd, 0);
    (void)csr;
    pc += 4;
}

void rv32i_hart::exec_csrrwi(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t csr  = get_imm_i(insn) & 0xfff;

    if (pos)
    {
        *pos << "// x" << rd << " = 0";
    }

    regs.set(rd, 0);
    (void)csr;
    pc += 4;
}

void rv32i_hart::exec_csrrsi(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t csr  = get_imm_i(insn) & 0xfff;

    if (pos)
    {
        *pos << "// x" << rd << " = 0";
    }

    regs.set(rd, 0);
    (void)csr;
    pc += 4;
}

void rv32i_hart::exec_csrrci(uint32_t insn, ostream *pos)
{
    uint32_t rd   = get_rd(insn);
    uint32_t csr  = get_imm_i(insn) & 0xfff;

    if (pos)
    {
        *pos << "// x" << rd << " = 0";
    }

    regs.set(rd, 0);
    (void)csr;
    pc += 4;
}
