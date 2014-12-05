/***************************************************************

    The implementation of instruction decoder

    @date: December 3, 2014
    @author: Kirill Korolev <kirill.korolef@gmail.com>
    @version: 1.0 (December 3, 2014) 

***************************************************************/

// Generic C
#include <string.h>

// Generic C++
#include <sstream>

// uArchSim modules
#include <func_instr.h>

uint32 get_instr(uint8 *where)
{
    ASSERT(where != NULL, "bad address");

    uint32 instr = 0;
    for(uint8 i = 0; i < 4; i++)
        instr |= where[i] << 8 * (3 - i);

    return instr;
}

FuncInstr::ISAEntry::ISAEntry(uint32 bytes)
{
    instr.raw = bytes;

    initFormat(bytes);

    switch (format) {
    case R:
        parseR(bytes);
        break;
    case I:
        parseI(bytes);
        break;
    case J:
        parseJ(bytes);
        break;
    default:
        ASSERT(0, "unexpected instruction format");
    }
};

void FuncInstr::ISAEntry::initFormat(uint32 bytes)
{
    switch (instr.R.opcode) {
    case 0:
        format = R;
        break;
    case 8: case 9: case 4: case 5:
        format = I;
        break;
    case 2:
        format = J;
        break;
    default:
        format = UNEXP;
    }
}

void FuncInstr::ISAEntry::parseR(uint32 bytes)
{
    switch (instr.R.funct) {
    case 32:
        type = ADD;
        name = "add";
        break;
    case 33:
        type = ADD;
        name = "addu";
        break;
    case 34:
        type = SUB;
        name = "sub";
        break;
    case 35:
        type = SUB;
        name = "subu";
        break;
    case 0:
        type = SHIFT;
        name = "sll";
        break;
    case 2:
        type = SHIFT;
        name = "srl";
        break;
    case 8:
        type = JUMP;
        name = "jr";
        break;
    default:
        ASSERT(0, "bad function type");
    }
}

void FuncInstr::ISAEntry::parseI(uint32 bytes)
{
    switch (instr.I.opcode) {
    case 8:
        type = ADD;
        name = "addi";
        break;
    case 9:
        type = ADD;
        name = "addiu";
        break;
    case 4:
        type = BRNCH;
        name = "beq";
        break;
    case 5:
        type = BRNCH;
        name = "bne";
        break;
    default:
        ASSERT(0, "bad operation code");
    }
}

void FuncInstr::ISAEntry::parseJ(uint32 bytes)
{
    type = JUMP;
    name = "j";
}

const char *FuncInstr::ISAEntry::get_name(REGTYPE type) const
{
    uint8 bits; 
    switch (type) {
    case S:
        bits = instr.R.s;
        break;
    case T:
        bits = instr.R.s;
        break;
    case D:
        bits = instr.R.s;
        break;
    default:
        ASSERT(0, "wrong type");
    }
    
    switch (bits) {
    case ZERO: return "zero";
    case AT:   return "at";
    case T0:   return "t0";
    case T1:   return "t1";
    case T2:   return "t2";
    case T3:   return "t3";
    case T4:   return "t4";
    case T5:   return "t5";
    case T6:   return "t6";
    case T7:   return "t7";
    case T8:   return "t8";
    case T9:   return "t9";
    default: ASSERT(0, "unexpected register");
    }
}

string FuncInstr::Dump(string indent) const
{
    ostringstream oss;
    
    oss << "section \"" << section_name << "\":" << endl;

    for (vector<ISAEntry>::const_iterator it = isaTable.begin();
         it != isaTable.end(); 
         ++it) {
        oss << indent << it->name << hex;
        switch (it->format) {
        case R:
            switch (it->type) {
            case ADD: case SUB:
                oss << indent
                    << " $" << it->get_name(S) << ","
                    << " $" << it->get_name(T) << ","
                    << " $" << it->get_name(D) << endl;
                break;
            case SHIFT:
                oss << indent
                    << " $" << it->get_name(S) << ","
                    << " $" << it->get_name(T) << ","
                    << " " << it->instr.R.d << endl;
                break;
            case JUMP:
                oss << indent << " $" << it->get_name(S) << endl;
                break;
            }
            break;
        case I:
            switch (it->type) {
            case ADD:
                oss << " $" << it->get_name(S) << ","
                    << " $" << it->get_name(T) << ","
                    << " $" << it->instr.I.imm << endl;
                break;
            case BRNCH:
                oss << " $" << it->get_name(T) << ","
                    << " $" << it->get_name(S) << ","
                    << " $" << it->instr.I.imm << endl;
                break;
            }
            break;
        case J:
            oss << it->instr.J.addr << endl;
            break;
        default:
            ASSERT(0, "unexpected format");
        }
    }

    return oss.str();
}