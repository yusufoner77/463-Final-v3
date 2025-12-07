//******************************************************************************
// Yusuf Oner
// z2048138
// CSCI 463
//
// I certify that this is my own work, and where applicable an extension
// of the starter code for the assignment.
//
//******************************************************************************

#include "rv32i_decode.h"
#include <sstream>
#include <iomanip>

using namespace std;

//******************************************************************************
// Takes a uint32_t instruction as its parameter and extracts and returns the opcode
// field from the given instruction as a uint32_t
//******************************************************************************
uint32_t rv32i_decode::get_opcode(uint32_t insn)
{
    return insn & 0x7f;
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and extracts and returns the rd field
// as an uint32_t value from 0 to 31.
//******************************************************************************
uint32_t rv32i_decode::get_rd(uint32_t insn)
{
    return (insn >> 7) & 0x1f;
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and extracts and returns the rs1 field
// as an uint32_t value from 0 to 31.
//******************************************************************************
uint32_t rv32i_decode::get_rs1(uint32_t insn)
{
    return (insn >> 15) & 0x1f;
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and extracts and returns the rs2 field
// as an uint32_t value from 0 to 31.
//******************************************************************************
uint32_t rv32i_decode::get_rs2(uint32_t insn)
{
    return (insn >> 20) & 0x1f;
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and extracts and returns the funct3 field
// as an uint32_t value from 0 to 7.
//******************************************************************************
uint32_t rv32i_decode::get_funct3(uint32_t insn)
{
    return (insn >> 12) & 0x7;
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and extracts and returns the funct7 field
// as an uint32_t value from 0x00 to 0x7f.
//******************************************************************************
uint32_t rv32i_decode::get_funct7(uint32_t insn)
{
    return (insn >> 25) & 0x7f;
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and extracts and returns the imm_i field
// from the given instruction as a 32-bit signed integer as shown in RVALP.
//******************************************************************************
int32_t rv32i_decode::get_imm_i(uint32_t insn)
{
    return static_cast<int32_t>(insn) >> 20;
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and extracts and returns the imm_u field
// from the given instruction as a 32-bit signed integer as shown in RVALP.
//******************************************************************************
int32_t rv32i_decode::get_imm_u(uint32_t insn)
{
    return static_cast<int32_t>(insn & 0xfffff000);
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and extracts and returns the imm_s field
// from the given instruction as a 32-bit signed integer as shown in RVALP.
//******************************************************************************
int32_t rv32i_decode::get_imm_s(uint32_t insn)
{
    int32_t imm_s = (insn & 0xfe000000) >> (25 - 5);
    imm_s |= (insn & 0x00000f80) >> (7 - 0);

    if (insn & 0x80000000)
        imm_s |= 0xfffff000;
    return imm_s;
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and extracts and returns the imm_b field
// from the given instruction as a 32-bit signed integer as shown in RVALP.
//******************************************************************************
int32_t rv32i_decode::get_imm_b(uint32_t insn)
{
    int32_t imm_b = 0;
    imm_b |= (insn & 0x80000000) >> (31 - 12);
    imm_b |= (insn & 0x00000080) << (11 - 7);
    imm_b |= (insn & 0x7e000000) >> (25 - 5);
    imm_b |= (insn & 0x00000f00) >> (8 - 1);

    if (insn & 0x80000000)
        imm_b |= 0xffffe000;
    return imm_b;
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and extracts and returns the imm_j field
// from the given instruction as a 32-bit signed integer as shown in RVALP.
//******************************************************************************
int32_t rv32i_decode::get_imm_j(uint32_t insn)
{
    int32_t imm_j = 0;
    imm_j |= (insn & 0x80000000) >> (31 - 20);
    imm_j |= (insn & 0x000ff000);
    imm_j |= (insn & 0x00100000) >> (20 - 11);
    imm_j |= (insn & 0x7fe00000) >> (21 - 1); 
    if (insn & 0x80000000)
        imm_j |= 0xffe00000;
    return imm_j;
}

//******************************************************************************
// Takes an integer of register r as its parameter and returns a string with the name
// of that register
//******************************************************************************
string rv32i_decode::render_reg(int r)
{
    return "x" + to_string(r);
}

//******************************************************************************
// Takes an uint32_t of register and int32_t immediate as its parameter and returns 
// a string representing the operands of the form imm(register)
//******************************************************************************
string rv32i_decode::render_base_disp(uint32_t reg, int32_t imm)
{
    ostringstream os;
    os << imm << "(" << render_reg(reg) << ")";
    return os.str();
}

//******************************************************************************
// Takes a string of the instruction mnemonic and returns a string of the mnemonic
// with appropriate space padding
//******************************************************************************
string rv32i_decode::render_mnemonic(const string &mnemonic)
{
    if (mnemonic == "ecall" || mnemonic == "ebreak")
        return mnemonic;

    ostringstream os;
    os << left << setw(mnemonic_width) << mnemonic;
    return os.str();
}

//******************************************************************************
// No parameters, returns a string if the instruction is illegal
//******************************************************************************
string rv32i_decode::render_illegal_insn()
{
    return "ERROR: UNIMPLEMENTED INSTRUCTION";
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and formats a LUI instruction by extracting
// its destination register and 20-bit immediate, then returning the correctly formatted 
// assembly string
//******************************************************************************
string rv32i_decode::render_lui(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);
    uint32_t imm20 = static_cast<uint32_t>(imm_u) >> 12;

    ostringstream os;
    os << render_mnemonic("lui") << render_reg(rd) << ","
       << hex::to_hex0x20(imm20);
    return os.str();
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and formats a AUIPC instruction by extracting
// its destination register and 20-bit immediate, then returning the correctly formatted 
// assembly string
//******************************************************************************
string rv32i_decode::render_auipc(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);
    uint32_t imm20 = static_cast<uint32_t>(imm_u) >> 12;

    ostringstream os;
    os << render_mnemonic("auipc") << render_reg(rd) << ","
       << hex::to_hex0x20(imm20);
    return os.str();
}

//******************************************************************************
// Takes a uint32_t instruction and a uint32_t address as its parameter and formats a 
// JAL instruction by extracting its destination register and offset, 
// finding the target address, and returning the correctly formatted assembly string
//******************************************************************************
string rv32i_decode::render_jal(uint32_t addr, uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_j = get_imm_j(insn);
    uint32_t target = addr + imm_j;

    ostringstream os;
    os << render_mnemonic("jal")
       << render_reg(rd) << ","
       << hex::to_hex0x32(target);
    return os.str();
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and formats a JALR instruction by extracting
// its destination register, base register, and immediate offset, then returning 
// the properly formatted assembly string
//******************************************************************************
string rv32i_decode::render_jalr(uint32_t insn)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    ostringstream os;
    os << render_mnemonic("jalr")
       << render_reg(rd) << ","
       << render_base_disp(rs1, imm_i);
    return os.str();
}

//******************************************************************************
// Takes a uint32_t instruction, uint32_t address, and char mnemonic pointer as its 
// parameter and formats a branch instruction by extracting its two source registers 
// and branch offset, finding the target address, and returning the correctly formatted
// assembly string
//******************************************************************************
string rv32i_decode::render_btype(uint32_t addr, uint32_t insn, const char *mnemonic)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    uint32_t target = addr + imm_b;

    ostringstream os;
    os << render_mnemonic(mnemonic)
       << render_reg(rs1) << ","
       << render_reg(rs2) << ","
       << hex::to_hex0x32(target);
    return os.str();
}

//******************************************************************************
// Takes a uint32_t instruction and char mnemonic pointer as its parameter and
// formats an I-type load instruction by extracting its destination register, base register,
// and immediate offset, then returning the properly formatted assembly
//******************************************************************************
string rv32i_decode::render_itype_load(uint32_t insn, const char *mnemonic)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);

    ostringstream os;
    os << render_mnemonic(mnemonic)
       << render_reg(rd) << ","
       << render_base_disp(rs1, imm_i);
    return os.str();
}

//******************************************************************************
// Takes a uint32_t instruction and char mnemonic pointer as its parameter and
// formats an S-type store instruction by extracting its source register, base register,
// and offset, then returning the correctly formatted assembly string
//******************************************************************************
string rv32i_decode::render_stype(uint32_t insn, const char *mnemonic)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn);

    ostringstream os;
    os << render_mnemonic(mnemonic)
       << render_reg(rs2) << ","
       << render_base_disp(rs1, imm_s);
    return os.str();
}

//******************************************************************************
// Takes a uint32_t instruction and char mnemonic pointer, as well as
// a int32_t immediate as its parameter and formats an I-type ALU instruction by 
// extracting its destination register, source register, and immediate value, then 
// returning the correctly formatted assembly string
//******************************************************************************
string rv32i_decode::render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);

    ostringstream os;
    os << render_mnemonic(mnemonic)
       << render_reg(rd) << ","
       << render_reg(rs1) << ","
       << imm_i;
    return os.str();
}

//******************************************************************************
// Takes a uint32_t instruction and char mnemonic pointer as its parameter and
// formats an R-type ALU instruction by extracting its destination register and two 
// source registers, then returning the correctly formatted assembly string
//******************************************************************************
string rv32i_decode::render_rtype(uint32_t insn, const char *mnemonic)
{
    uint32_t rd  = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    ostringstream os;
    os << render_mnemonic(mnemonic)
       << render_reg(rd) << ","
       << render_reg(rs1) << ","
       << render_reg(rs2);
    return os.str();
}

//******************************************************************************
// Takes a uint32_t instruction as its parameter and formats system instructions by
// using the funct3 and CSR fields, extracting the registers or immediate values, and 
// returning the correctly formatted assembly string
//******************************************************************************
string rv32i_decode::render_system(uint32_t insn)
{
    uint32_t funct3 = get_funct3(insn);

    if (funct3 == 0)
    {
        if (insn == 0x00000073)
            return "ecall";
        else if (insn == 0x00100073)
            return "ebreak";
        else
            return render_illegal_insn();
    }

    uint32_t rd   = get_rd(insn);
    uint32_t rs1  = get_rs1(insn);
    uint32_t zimm = rs1;
    uint32_t csr  = (insn >> 20) & 0xfff;

    ostringstream os;

    switch (funct3)
    {
        case funct3_csrrw:
            os << render_mnemonic("csrrw")
               << render_reg(rd) << ","
               << hex::to_hex0x12(csr) << ","
               << render_reg(rs1);
            break;
        case funct3_csrrs:
            os << render_mnemonic("csrrs")
               << render_reg(rd) << ","
               << hex::to_hex0x12(csr) << ","
               << render_reg(rs1);
            break;
        case funct3_csrrc:
            os << render_mnemonic("csrrc")
               << render_reg(rd) << ","
               << hex::to_hex0x12(csr) << ","
               << render_reg(rs1);
            break;
        case funct3_csrrwi:
            os << render_mnemonic("csrrwi")
               << render_reg(rd) << ","
               << hex::to_hex0x12(csr) << ","
               << zimm;
            break;
        case funct3_csrrsi:
            os << render_mnemonic("csrrsi")
               << render_reg(rd) << ","
               << hex::to_hex0x12(csr) << ","
               << zimm;
            break;
        case funct3_csrrci:
            os << render_mnemonic("csrrci")
               << render_reg(rd) << ","
               << hex::to_hex0x12(csr) << ","
               << zimm;
            break;
        default:
            return render_illegal_insn();
    }

    return os.str();
}

//******************************************************************************
// Takes a uint32_t instruction and uint32_t address as its parameter and returns
// a string containing the disassembled instruction text by using a switch statement
// woth the value of the opcode field
//******************************************************************************
string rv32i_decode::decode(uint32_t addr, uint32_t insn)
{
    uint32_t opcode = get_opcode(insn);

    switch (opcode)
    {
        case opcode_lui:
            return render_lui(insn);

        case opcode_auipc:
            return render_auipc(insn);

        case opcode_jal:
            return render_jal(addr, insn);

        case opcode_jalr:
            return render_jalr(insn);

        case opcode_btype:
        {
            uint32_t funct3 = get_funct3(insn);
            switch (funct3)
            {
                case funct3_beq:  return render_btype(addr, insn, "beq");
                case funct3_bne:  return render_btype(addr, insn, "bne");
                case funct3_blt:  return render_btype(addr, insn, "blt");
                case funct3_bge:  return render_btype(addr, insn, "bge");
                case funct3_bltu: return render_btype(addr, insn, "bltu");
                case funct3_bgeu: return render_btype(addr, insn, "bgeu");
                default:          return render_illegal_insn();
            }
        }

        case opcode_load:
        {
            uint32_t funct3 = get_funct3(insn);
            switch (funct3)
            {
                case funct3_lb:  return render_itype_load(insn, "lb");
                case funct3_lh:  return render_itype_load(insn, "lh");
                case funct3_lw:  return render_itype_load(insn, "lw");
                case funct3_lbu: return render_itype_load(insn, "lbu");
                case funct3_lhu: return render_itype_load(insn, "lhu");
                default:         return render_illegal_insn();
            }
        }

        case opcode_store:
        {
            uint32_t funct3 = get_funct3(insn);
            switch (funct3)
            {
                case funct3_sb: return render_stype(insn, "sb");
                case funct3_sh: return render_stype(insn, "sh");
                case funct3_sw: return render_stype(insn, "sw");
                default:        return render_illegal_insn();
            }
        }

        case opcode_alu_imm:
        {
            uint32_t funct3 = get_funct3(insn);
            int32_t imm_i = get_imm_i(insn);

            switch (funct3)
            {
                case funct3_add_sub:
                    return render_itype_alu(insn, "addi", imm_i);
                case 0b010:
                    return render_itype_alu(insn, "slti", imm_i);
                case 0b011:
                    return render_itype_alu(insn, "sltiu", imm_i);
                case funct3_xor: 
                    return render_itype_alu(insn, "xori", imm_i);
                case funct3_or: 
                    return render_itype_alu(insn, "ori", imm_i);
                case funct3_and: 
                    return render_itype_alu(insn, "andi", imm_i);
                case funct3_sll: 
                    return render_itype_alu(insn, "slli", imm_i & 0x1f);
                case funct3_srl_sra:
                {
                    uint32_t funct7 = get_funct7(insn);
                    if (funct7 == funct7_srl)
                        return render_itype_alu(insn, "srli", imm_i & 0x1f);
                    else if (funct7 == funct7_sra)
                        return render_itype_alu(insn, "srai", imm_i & 0x1f);
                    else
                        return render_illegal_insn();
                }
                default:
                    return render_illegal_insn();
            }
        }

        case opcode_alu_reg:
        {
            uint32_t funct3 = get_funct3(insn);
            uint32_t funct7 = get_funct7(insn);

            switch (funct3)
            {
                case funct3_add_sub:
                    if (funct7 == funct7_add) return render_rtype(insn, "add");
                    if (funct7 == funct7_sub) return render_rtype(insn, "sub");
                    break;
                case funct3_sll:
                    if (funct7 == funct7_add) return render_rtype(insn, "sll");
                    break;
                case funct3_slt:
                    if (funct7 == funct7_add) return render_rtype(insn, "slt");
                    break;
                case funct3_sltu:
                    if (funct7 == funct7_add) return render_rtype(insn, "sltu");
                    break;
                case funct3_xor:
                    if (funct7 == funct7_add) return render_rtype(insn, "xor");
                    break;
                case funct3_srl_sra:
                    if (funct7 == funct7_srl) return render_rtype(insn, "srl");
                    if (funct7 == funct7_sra) return render_rtype(insn, "sra");
                    break;
                case funct3_or:
                    if (funct7 == funct7_add) return render_rtype(insn, "or");
                    break;
                case funct3_and:
                    if (funct7 == funct7_add) return render_rtype(insn, "and");
                    break;
            }
            return render_illegal_insn();
        }

        case opcode_system:
            return render_system(insn);

        default:
            return render_illegal_insn();
    }
}
