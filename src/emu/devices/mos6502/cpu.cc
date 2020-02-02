#include "emu/devices/mos6502/cpu.h"
#include "emu/devices/mos6502/interrupt.h"
#include "core/logger.h"

using namespace SuperDendy;
using namespace SuperDendy::Core;
using namespace SuperDendy::Emu::MOS6502;

// Status flag bitmasks
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_V 0x40
#define FLAG_N 0x80

// Flag manip macros
#define SET_FLAG(f) s |= (f)
#define CLEAR_FLAG(f) s &= ~(f)
#define GET_FLAG(f) (s & (f))

// Stack base address
#define STACK_ADDR 0x100

// Flag calc macros
#define CALC_C(c) if ((c)) SET_FLAG(FLAG_C); else CLEAR_FLAG(FLAG_C)
#define CALC_Z(n) if ((n) == 0) SET_FLAG(FLAG_Z); else CLEAR_FLAG(FLAG_Z)
#define CALC_I(c) if ((c)) SET_FLAG(FLAG_I); else CLEAR_FLAG(FLAG_I)
#define CALC_B(c) if ((c)) SET_FLAG(FLAG_B); else CLEAR_FLAG(FLAG_B)
#define CALC_V(c) if ((c)) SET_FLAG(FLAG_V); else CLEAR_FLAG(FLAG_V)
#define CALC_N(n) if ((n) & 0x80) SET_FLAG(FLAG_N); else CLEAR_FLAG(FLAG_N)

#define ADD_OFFSET() \
	Word old_pc = pc; \
	pc += (int8_t)operand; \
	if ((pc & 0xFF00) != (old_pc & 0xFF00)) cycles += 2; \
	else cycles++

#define CALC_PAGE_PENALTY() \
	if (((addr_mode == ABX) && (x > (oper_addr & 0x00FF))) || \
		((addr_mode == ABY || addr_mode == IDY) && (y > (oper_addr & 0x00FF)))) \
		cycles++

// Read 2 bytes
#define READ_W(a) ((Word)memory.read((a)) | ((Word)memory.read((a) + 1) << 8))

// Addressing mode function pointer table for fetching operands; indexed by opcode
static const AddressingMode addr_mode_lookup[256] = {
/*    | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | A  | B  | C  | D  | E  | F  |*/
/* 0 */ IMP, IDX, IMP, IDX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, ACC, IMM, ABS, ABS, ABS, ABS,
/* 1 */ REL, IDY, IMP, IDY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX,
/* 2 */ ABS, IDX, IMP, IDX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, ACC, IMM, ABS, ABS, ABS, ABS,
/* 3 */ REL, IDY, IMP, IDY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX,
/* 4 */ IMP, IDX, IMP, IDX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, ACC, IMM, ABS, ABS, ABS, ABS,
/* 5 */ REL, IDY, IMP, IDY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX,
/* 6 */ IMP, IDX, IMP, IDX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, ACC, IMM, IND, ABS, ABS, ABS,
/* 7 */ REL, IDY, IMP, IDY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX,
/* 8 */ IMM, IDX, IMM, IDX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,
/* 9 */ REL, IDY, IMP, IDY, ZPX, ZPX, ZPY, ZPY, IMP, ABY, IMP, ABY, ABX, ABX, ABY, ABY,
/* A */ IMM, IDX, IMM, IDX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,
/* B */ REL, IDY, IMP, IDY, ZPX, ZPX, ZPY, ZPY, IMP, ABY, IMP, ABY, ABX, ABX, ABY, ABY,
/* C */ IMM, IDX, IMM, IDX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,
/* D */ REL, IDY, IMP, IDY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX,
/* E */ IMM, IDX, IMM, IDX, ZPG, ZPG, ZPG, ZPG, IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,
/* F */ REL, IDY, IMP, IDY, ZPX, ZPX, ZPX, ZPX, IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX
};

// CPU instruction function pointer table; indexed by opcode
static const Instruction instr_lookup[256] = {
/*    | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | A   | B  | C  | D  | E  | F  |*/
/* 0 */ BRK, ORA, NOP, SLO, NOP, ORA, ASL, SLO, PHP, ORA, ASLA, NOP, NOP, ORA, ASL, SLO,
/* 1 */ BPL, ORA, NOP, SLO, NOP, ORA, ASL, SLO, CLC, ORA, NOP,  SLO, NOP, ORA, ASL, SLO,
/* 2 */ JSR, AND, NOP, RLA, BIT, AND, ROL, RLA, PLP, AND, ROLA, NOP, BIT, AND, ROL, RLA,
/* 3 */ BMI, AND, NOP, RLA, NOP, AND, ROL, RLA, SEC, AND, NOP,  RLA, NOP, AND, ROL, RLA,
/* 4 */ RTI, EOR, NOP, SRE, NOP, EOR, LSR, SRE, PHA, EOR, LSRA, NOP, JMP, EOR, LSR, SRE,
/* 5 */ BVC, EOR, NOP, SRE, NOP, EOR, LSR, SRE, CLI, EOR, NOP,  SRE, NOP, EOR, LSR, SRE,
/* 6 */ RTS, ADC, NOP, RRA, NOP, ADC, ROR, RRA, PLA, ADC, RORA, NOP, JMP, ADC, ROR, RRA,
/* 7 */ BVS, ADC, NOP, RRA, NOP, ADC, ROR, RRA, SEI, ADC, NOP,  RRA, NOP, ADC, ROR, RRA,
/* 8 */ NOP, STA, NOP, SAX, STY, STA, STX, SAX, DEY, NOP, TXA,  NOP, STY, STA, STX, SAX,
/* 9 */ BCC, STA, NOP, NOP, STY, STA, STX, SAX, TYA, STA, TXS,  NOP, NOP, STA, NOP, NOP,
/* A */ LDY, LDA, LDX, LAX, LDY, LDA, LDX, LAX, TAY, LDA, TAX,  NOP, LDY, LDA, LDX, LAX,
/* B */ BCS, LDA, NOP, LAX, LDY, LDA, LDX, LAX, CLV, LDA, TSX,  LAX, LDY, LDA, LDX, LAX,
/* C */ CPY, CMP, NOP, DCP, CPY, CMP, DEC, DCP, INY, CMP, DEX,  NOP, CPY, CMP, DEC, DCP,
/* D */ BNE, CMP, NOP, DCP, NOP, CMP, DEC, DCP, CLD, CMP, NOP,  DCP, NOP, CMP, DEC, DCP,
/* E */ CPX, SBC, NOP, ISB, CPX, SBC, INC, ISB, INX, SBC, NOP,  SBC, CPX, SBC, INC, ISB,
/* F */ BEQ, SBC, NOP, ISB, NOP, SBC, INC, ISB, SED, SBC, NOP,  ISB, NOP, SBC, INC, ISB
};

// Clock cycle table; indexed by opcode
static const Dword timing_lookup[256] = {
/*    | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9| A| B| C| D| E| F|*/
/* 0 */ 7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
/* 1 */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 2 */ 6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
/* 3 */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 4 */ 6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
/* 5 */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 6 */ 6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
/* 7 */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 8 */ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
/* 9 */ 2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
/* A */ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
/* B */ 2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
/* C */ 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
/* D */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* E */ 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
/* F */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7
};

CPU::CPU(
	Memory8& memory,
	InterruptLine& interrupt_line,
	Mode mode
) :
	memory(memory),
	interrupt_line(interrupt_line)
{
	Logger::debug("Instantiated MOS6502 emulator");
}

CPU::~CPU() {
}

// Stack operation helper methods
void CPU::pushb(Byte val) {
	memory.write(STACK_ADDR | sp--, val);
}

void CPU::pushw(Word val) {
	pushb((val >> 8) & 0xFF);
	pushb(val & 0xFF);
}

Byte CPU::pullb() {
	return memory.read(STACK_ADDR | ++sp);
}

Word CPU::pullw() {
	return (Word)pullb() | ((Word)pullb() << 8);
}

// Initialize registers
void CPU::power_on() {
	a = 0;
	x = 0;
	y = 0;
	s = 0x34;
	sp = 0xFD;
#ifdef NESTEST
	pc = 0xC000;
#else
	pc = READ_W(0xFFFC);
#endif
	opcode = 0;
	oper_addr = 0;
	ind_oper_addr = 0;
	ind_operand = 0;
	operand = 0;
	cycles = 0;
	suspended = 0;
	Logger::debug("MOS6502 powered on");
}

// Reset the CPU
void CPU::reset() {
	pc = READ_W(0xFFFC);
	sp -= 3;
	SET_FLAG(FLAG_I);
	cycles = 0;
	suspended = 0;
}

// Suspend the CPU for a specified number of cycles
void CPU::suspend(Dword duration) {
	suspended = duration;
}

// Execute one CPU instruction
Dword CPU::step() {
	if (suspended) {
		suspended--;
		return 1;
	}

	Dword lastcycles = cycles;

	// Handle interrupts
	int interrupt = interrupt_line.get_current();
	switch (interrupt) {
		case IRQ:
			if (!GET_FLAG(FLAG_I)) {
				CLEAR_FLAG(FLAG_B);
				pushw(pc);
				pushb(s);
				SET_FLAG(FLAG_I);
				pc = READ_W(0xFFFE);
				interrupt_line.service(IRQ);
			}
			break;

		case NMI:
			CLEAR_FLAG(FLAG_B);
			pushw(pc);
			pushb(s);
			SET_FLAG(FLAG_I);
			pc = READ_W(0xFFFA);
			interrupt_line.service(NMI);
			break;

		case RESET:
			pc = READ_W(0xFFFC);
			sp = 0xFD;
			s |= 0x24;
			interrupt_line.service(RESET);
			break;
	}

	// Fetch opcode
	opcode = memory.read(pc++);

#ifdef NESTEST
	nestest_disassemble1();
#endif

	// Addressing mode: fetch operand address
	AddressingMode addr_mode = addr_mode_lookup[opcode];
	switch (addr_mode) {
		case IMP: { // Implied
			// Leave empty
			break;
		}
		case ACC: { // Accumulator
			// Leave empty
			break;
		}
		case IMM: { // Immediate
			oper_addr = pc++;
			break;
		}
		case ZPG: { // Zero-page
			ind_operand = memory.read(pc++);
			oper_addr = (Word)ind_operand;
			break;
		}
		case ZPX: { // Zero-page, X
			ind_operand = memory.read(pc++);
			oper_addr = ((Word)ind_operand + (Word)x) & 0xFF;
			break;
		}
		case ZPY: { // Zero-page, Y
			ind_operand = memory.read(pc++);
			oper_addr = ((Word)ind_operand + (Word)y) & 0xFF;
			break;
		}
		case REL: { // Relative
			oper_addr = pc++;
			break;
		}
		case ABS: { // Absolute
			oper_addr = READ_W(pc);
			pc += 2;
			break;
		}
		case ABX: { // Absolute, X
			ind_oper_addr = READ_W(pc);
			oper_addr = ind_oper_addr + x;
			pc += 2;
			break;
		}
		case ABY: { // Absolute, Y
			ind_oper_addr = READ_W(pc);
			oper_addr = ind_oper_addr + y;
			pc += 2;
			break;
		}
		case IND: { // Indirect
			ind_oper_addr = READ_W(pc);
			Word tmp = ind_oper_addr;
			Word tmp2 = (tmp & 0xFF00) | ((tmp + 1) & 0x00FF);
			oper_addr = (Word)memory.read(tmp) | ((Word)memory.read(tmp2) << 8);
			pc += 2;
			break;
		}
		case IDX: { // Indexed indirect
			ind_operand = memory.read(pc++);
			ind_oper_addr = (Word)x + ind_operand;
			if (ind_oper_addr == 0xFF) {
				oper_addr = (Word)memory.read(0xFF) | ((Word)memory.read(0) << 8);
			} else {
				oper_addr = READ_W(0x00FF & ind_oper_addr);
			}
			break;
		}
		case IDY: { // Indirect indexed
			ind_operand = memory.read(pc++);
			if (ind_operand == 0xFF) {
				ind_oper_addr = (Word)memory.read(0xFF) | ((Word)memory.read(0) << 8);
			} else {
				ind_oper_addr = READ_W(0x00FF & ind_operand);
			}
			oper_addr = ind_oper_addr + y;
			break;
		}
		default: break;
	}

	// Cache operand value
	if (addr_mode != ACC && addr_mode != IMP && (opcode != 0x8D)) {
		operand = memory.read(oper_addr);
	} else {
		operand = 0;
	}

#ifdef NESTEST
	nestest_disassemble2(addr_mode);
#endif

	// Execute CPU instruction
	Instruction instr = instr_lookup[opcode];
	Byte b;
	Word w;
	switch (instr) {
		// Add memory to the accumulator with carry
		// N Z C V
		case ADC: {
			w = (Word)a + operand + GET_FLAG(FLAG_C);
			CALC_Z(w & 0x00FF);
			CALC_C(w & 0xFF00);
			CALC_N(w);
			CALC_V(~(a ^ operand) & (a ^ w) & 0x80);
			a = (Byte)(w & 0x00FF0FF);
			CALC_PAGE_PENALTY();
			break;
		}
		// AND memory with accumulator
		// N Z
		case AND: {
			b = a & operand;
			CALC_N(b);
			CALC_Z(b);
			a = b;
			CALC_PAGE_PENALTY();
			break;
		}
		// Shift accumulator left one bit
		// N Z C
		case ASLA: {
			b = a << 1;
			CALC_C(a & 0x80);
			CALC_N(b);
			CALC_Z(b);
			a = b;
			break;
		}
		// Shift memory left one bit
		// N Z C
		case ASL: {
			b = operand << 1;
			CALC_C(operand & 0x80);
			CALC_N(b);
			CALC_Z(b);
			memory.write(oper_addr, b);
			break;
		}
		// Branch on carry clear
		// No flags changed
		case BCC: {
			if (!GET_FLAG(FLAG_C)) {
				ADD_OFFSET();
			}
			break;
		}
		// Branch on carry set
		// No flags changed
		case BCS: {
			if (GET_FLAG(FLAG_C)) {
				ADD_OFFSET();
			}
			break;
		}
		// Branch on b zero (zero set)
		// No flags changed
		case BEQ: {
			if (GET_FLAG(FLAG_Z)) {
				ADD_OFFSET();
			}
			break;
		}
		// Test bits in memory with accumulator
		// N=M7 Z V=M6
		case BIT: {
			b = a & operand;
			CALC_N(operand);
			CALC_Z(b);
			CALC_V(operand & FLAG_V);
			break;
		}
		// Branch on b minus (N set)
		// No flags changed
		case BMI: {
			if (GET_FLAG(FLAG_N)) {
				ADD_OFFSET();
			}
			break;
		}
		// Branch on b not zero (zero cleared)
		// No flags changed
		case BNE: {
			if (!GET_FLAG(FLAG_Z)) {
				ADD_OFFSET();
			}
			break;
		}
		// Branch on b plus (N cleared)
		// No flags changed
		case BPL: {
			if (!GET_FLAG(FLAG_N)) {
				ADD_OFFSET();
			}
			break;
		}
		// Force break
		// I=1
		case BRK: {
			SET_FLAG(FLAG_B);
			pc++;
			pushw(pc);
			pushb(s);
			SET_FLAG(FLAG_I);
			pc = READ_W(0xFFFE);
			break;
		}
		// Branch on overflow clear
		// No flags changed
		case BVC: {
			if (!GET_FLAG(FLAG_V)) {
				ADD_OFFSET();
			}
			break;
		}
		// Branch on overflow set
		// No flags changed
		case BVS: {
			if (GET_FLAG(FLAG_V)) {
				ADD_OFFSET();
			}
			break;
		}
		// Clear carry flag
		// C=0
		case CLC: {
			CLEAR_FLAG(FLAG_C);
			break;
		}
		// Clear decimal mode (NOT USED IN NES MODE)
		// D=0
		case CLD: {
			CLEAR_FLAG(FLAG_D);
			break;
		}
		// Clear interrupt disable flag
		// I=0
		case CLI: {
			CLEAR_FLAG(FLAG_I);
			break;
		}
		// Clear overflow flag
		// V=0
		case CLV: {
			CLEAR_FLAG(FLAG_V);
			break;
		}
		// Compare memory and accumulator
		// N Z C
		case CMP: {
			b = a - operand;
			CALC_C(a >= operand);
			CALC_Z(b);
			CALC_N(b);
			CALC_PAGE_PENALTY();
			break;
		}
		// Compare memory and index X
		// N Z C
		case CPX: {
			b = x - operand;
			CALC_C(x >= operand);
			CALC_Z(b);
			CALC_N(b);
			break;
		}
		// Compare memory and index Y
		// N Z C
		case CPY: {
			b = y - operand;
			CALC_C(y >= operand);
			CALC_Z(b);
			CALC_N(b);
			break;
		}
		// Decrement memory by one
		// N Z
		case DEC: {
			b = operand - 1;
			CALC_Z(b);
			CALC_N(b);
			memory.write(oper_addr, b);
			break;
		}
		// Decrement index X by one
		// N Z
		case DEX: {
			b = x - 1;
			CALC_Z(b);
			CALC_N(b);
			x = b;
			break;
		}
		// Decrement index Y by one
		// N Z
		case DEY: {
			b = y - 1;
			CALC_Z(b);
			CALC_N(b);
			y = b;
			break;
		}
		// XOR memory with accumulator
		// N Z
		case EOR: {
			b = a ^ operand;
			CALC_Z(b);
			CALC_N(b);
			a = b;
			CALC_PAGE_PENALTY();
			break;
		}
		// Increment memory by one
		// N Z
		case INC: {
			b = operand + 1;
			CALC_Z(b);
			CALC_N(b);
			memory.write(oper_addr, b);
			break;
		}
		// Increment index X by one
		// N Z
		case INX: {
			b = x + 1;
			CALC_Z(b);
			CALC_N(b);
			x = b;
			break;
		}
		// Increment index Y by one
		// N Z
		case INY: {
			b = y + 1;
			CALC_Z(b);
			CALC_N(b);
			y = b;
			break;
		}
		// Jump to new location
		// No flags changed
		case JMP: {
			pc = oper_addr;
			break;
		}
		// Jump to new location saving return address
		// No flags changed
		case JSR: {
			pushw(pc - 1);
			pc = oper_addr;
			break;
		}
		// Load accumulator with memory
		// N Z
		case LDA: {
			a = operand;
			CALC_Z(a);
			CALC_N(a);
			CALC_PAGE_PENALTY();
			break;
		}
		// Load index X with memory
		// N Z
		case LDX: {
			x = operand;
			CALC_Z(x);
			CALC_N(x);
			CALC_PAGE_PENALTY();
			break;
		}
		// Load index Y with memory
		// N Z
		case LDY: {
			y = operand;
			CALC_Z(y);
			CALC_N(y);
			CALC_PAGE_PENALTY();
			break;
		}
		// Shift accumulator right one bit
		// N=0 Z C
		case LSRA: {
			b = a >> 1;
			CALC_C(a & 1);
			CALC_Z(b);
			CALC_N(b);
			a = b;
			break;
		}
		// Shift memory right one bit
		// N=0 Z C
		case LSR: {
			b = operand >> 1;
			CALC_C(operand & 1);
			CALC_Z(b);
			CALC_N(b);
			memory.write(oper_addr, b);
			break;
		}
		// No operation
		// No flags changed
		case NOP: {
			// Leave empty
			break;
		}
		// OR memory with accumulator
		// N Z
		case ORA: {
			b = a | operand;
			CALC_Z(b);
			CALC_N(b);
			a = b;
			CALC_PAGE_PENALTY();
			break;
		}
		// Push accumulator onto stack
		// No flags changed
		case PHA: {
			pushb(a);
			break;
		}
		// Push processor status onto stack
		// No flags changed
		case PHP: {
			pushb(s | FLAG_B);
			break;
		}
		// Pull accumulator from stack
		// N Z
		case PLA: {
			a = pullb();
			CALC_Z(a);
			CALC_N(a);
			break;
		}
		// Pull processor status from stack
		// N Z C I D V = pull from stack
		case PLP: {
			s = (pullb() & ~(FLAG_B)) | 0x20;
			break;
		}
		// Rotate accumulator left one bit
		// N Z C
		case ROLA: {
			b = a << 1;
			b |= GET_FLAG(FLAG_C);
			CALC_C(a & 0x80);
			CALC_Z(b);
			CALC_N(b);
			a = b;
			break;
		}
		// Rotate memory left one bit
		// N Z C
		case ROL: {
			b = operand << 1;
			b |= GET_FLAG(FLAG_C);
			CALC_C(operand & 0x80);
			CALC_Z(b);
			CALC_N(b);
			memory.write(oper_addr, b);
			break;
		}
		// Rotate accumulator right one bit
		// N Z C
		case RORA: {
			b = a >> 1;
			if (GET_FLAG(FLAG_C))
				b |= 0x80;
			CALC_C(a & 1);
			CALC_Z(b);
			CALC_N(b);
			a = b;
			break;
		}
		// Rotate memory right one bit
		// N Z C
		case ROR: {
			b = operand >> 1;
			if (GET_FLAG(FLAG_C))
				b |= 0x80;
			CALC_C(operand & 1);
			CALC_Z(b);
			CALC_N(b);
			memory.write(oper_addr, b);
			break;
		}
		// Return from interrupt
		// N Z C I D V = pull from stack
		case RTI: {
			s = (pullb() & ~(FLAG_B)) | 0x20;
			pc = pullw();
			break;
		}
		// Return from subroutine
		// No flags changed
		case RTS: {
			pc = pullw();
			pc++;
			break;
		}
		// Subtract memory from accumulator with borrow
		// N Z C V
		case SBC: {
			w = (Word)a - operand - (1 - GET_FLAG(FLAG_C));
			CALC_Z(w & 0x00FF);
			CALC_N(w);
			CALC_C(!(w & 0xFF00));
			CALC_V((a ^ w) & (a ^ w) & 0x80);
			a = (Byte)(w & 0x00FF);
			CALC_PAGE_PENALTY();
			break;
		}
		// Set carry flag
		// C=1
		case SEC: {
			SET_FLAG(FLAG_C);
			break;
		}
		// Set decimal mode (NOT USED IN NES MODE)
		// D=1
		case SED: {
			SET_FLAG(FLAG_D);
			break;
		}
		// Set interrupt disable flag
		// I=1
		case SEI: {
			SET_FLAG(FLAG_I);
			break;
		}
		// Store accumulator in memory
		// No flags changed
		case STA: {
			memory.write(oper_addr, a);
			break;
		}
		// Store index X in memory
		// No flags changed
		case STX: {
			memory.write(oper_addr, x);
			break;
		}
		// Store index Y in memory
		// No flags changed
		case STY: {
			memory.write(oper_addr, y);
			break;
		}
		// Transfer accumulator to index X
		// N Z
		case TAX: {
			x = a;
			CALC_Z(x);
			CALC_N(x);
			break;
		}
		// Transfer accumulator to index Y
		// N Z
		case TAY: {
			y = a;
			CALC_Z(y);
			CALC_N(y);
			break;
		}
		// Transfer stack pointer to index X
		// N Z
		case TSX: {
			x = sp;
			CALC_Z(x);
			CALC_N(x);
			break;
		}
		// Transfer index X to accumulator
		// N Z
		case TXA: {
			a = x;
			CALC_Z(a);
			CALC_N(a);
			break;
		}
		// Transfer index X to stack pointer
		// No flags changed
		case TXS: {
			sp = x;
			break;
		}
		// Transfer index Y to accumulator
		// N Z
		case TYA: {
			a = y;
			CALC_Z(a);
			CALC_N(a);
			break;
		}
/* TODO: finish porting these
		// DEC memory and CMP b with accumulator (UNOFFICIAL)
		// ?
		case DCP: {
			DEC();
			operand = memory.read(oper_addr);
			CMP();
			break;
		}
		// INC memory and SBC b from the accumulator (UNOFFICIAL)
		// ?
		case ISB: {
			INC();
			operand = memory.read(oper_addr);
			SBC();
			break;
		}
		// Load accumulator and index X with memory (UNOFFICIAL)
		// ?
		case LAX: {
			LDA();
			LDX();
			break;
		}
		// ROL memory and AND b with accumulator (UNOFFICIAL)
		// ?
		case RLA: {
			ROL();
			operand = memory.read(oper_addr);
			AND();
			break;
		}
		// ROR memory and ADC b with accumulator (UNOFFICIAL)
		// ?
		case RRA: {
			ROR();
			operand = memory.read(oper_addr);
			ADC();
			break;
		}
		// AND accumulator with index X and store the b in memory (UNOFFICIAL)
		// No flags changed
		case SAX: {
			STA();
			STX();
			memory.write(oper_addr, (a & x));
			break;
		}
		// ASL memory and OR b with accumulator (UNOFFICIAL)
		// ?
		case SLO: {
			ASL();
			operand = memory.read(oper_addr);
			ORA();
			break;
		}
		// LSR memory and XOR b with accumulator (UNOFFICIAL)
		// ?
		case SRE: {
			LSR();
			operand = memory.read(oper_addr);
			EOR();
			break;
		}
*/
		case DCP: break;
		case ISB: break;
		case LAX: break;
		case RLA: break;
		case RRA: break;
		case SAX: break;
		case SLO: break;
		case SRE: break;
	}

	// Add cycles to the total cycle count
	cycles += timing_lookup[opcode];

#ifdef NESTEST
	nestest_disassemble3();
#endif

	return cycles - lastcycles;
}

#ifdef NESTEST
#include "nestest.partial.cc"
#endif
