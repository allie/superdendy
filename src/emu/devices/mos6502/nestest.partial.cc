#include <iostream>

#define CALC_OFFSET() \
	((operand & 0x80) ? pc - (operand & 0x7F) : pc + (operand & 0x7F))

static const char* addrsymbols[256] = {
/*    | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | A    | B    | C    | D    | E    | F    |*/
/* 0 */ "IMP", "IDX", "IMP", "IDX", "ZPG", "ZPG", "ZPG", "ZPG", "IMP", "IMM", "ACC", "IMM", "ABS", "ABS", "ABS", "ABS",
/* 1 */ "REL", "IDY", "IMP", "IDY", "ZPX", "ZPX", "ZPX", "ZPX", "IMP", "ABY", "IMP", "ABY", "ABX", "ABX", "ABX", "ABX",
/* 2 */ "ABS", "IDX", "IMP", "IDX", "ZPG", "ZPG", "ZPG", "ZPG", "IMP", "IMM", "ACC", "IMM", "ABS", "ABS", "ABS", "ABS",
/* 3 */ "REL", "IDY", "IMP", "IDY", "ZPX", "ZPX", "ZPX", "ZPX", "IMP", "ABY", "IMP", "ABY", "ABX", "ABX", "ABX", "ABX",
/* 4 */ "IMP", "IDX", "IMP", "IDX", "ZPG", "ZPG", "ZPG", "ZPG", "IMP", "IMM", "ACC", "IMM", "ABS", "ABS", "ABS", "ABS",
/* 5 */ "REL", "IDY", "IMP", "IDY", "ZPX", "ZPX", "ZPX", "ZPX", "IMP", "ABY", "IMP", "ABY", "ABX", "ABX", "ABX", "ABX",
/* 6 */ "IMP", "IDX", "IMP", "IDX", "ZPG", "ZPG", "ZPG", "ZPG", "IMP", "IMM", "ACC", "IMM", "IND", "ABS", "ABS", "ABS",
/* 7 */ "REL", "IDY", "IMP", "IDY", "ZPX", "ZPX", "ZPX", "ZPX", "IMP", "ABY", "IMP", "ABY", "ABX", "ABX", "ABX", "ABX",
/* 8 */ "IMM", "IDX", "IMM", "IDX", "ZPG", "ZPG", "ZPG", "ZPG", "IMP", "IMM", "IMP", "IMM", "ABS", "ABS", "ABS", "ABS",
/* 9 */ "REL", "IDY", "IMP", "IDY", "ZPX", "ZPX", "ZPY", "ZPY", "IMP", "ABY", "IMP", "ABY", "ABX", "ABX", "ABY", "ABY",
/* A */ "IMM", "IDX", "IMM", "IDX", "ZPG", "ZPG", "ZPG", "ZPG", "IMP", "IMM", "IMP", "IMM", "ABS", "ABS", "ABS", "ABS",
/* B */ "REL", "IDY", "IMP", "IDY", "ZPX", "ZPX", "ZPY", "ZPY", "IMP", "ABY", "IMP", "ABY", "ABX", "ABX", "ABY", "ABY",
/* C */ "IMM", "IDX", "IMM", "IDX", "ZPG", "ZPG", "ZPG", "ZPG", "IMP", "IMM", "IMP", "IMM", "ABS", "ABS", "ABS", "ABS",
/* D */ "REL", "IDY", "IMP", "IDY", "ZPX", "ZPX", "ZPX", "ZPX", "IMP", "ABY", "IMP", "ABY", "ABX", "ABX", "ABX", "ABX",
/* E */ "IMM", "IDX", "IMM", "IDX", "ZPG", "ZPG", "ZPG", "ZPG", "IMP", "IMM", "IMP", "IMM", "ABS", "ABS", "ABS", "ABS",
/* F */ "REL", "IDY", "IMP", "IDY", "ZPX", "ZPX", "ZPX", "ZPX", "IMP", "ABY", "IMP", "ABY", "ABX", "ABX", "ABX", "ABX"
};

static const char* instrsymbols[256] = {
/*    | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | A    | B    | C    | D    | E    | F    |*/
/* 0 */ "BRK", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO", "PHP", "ORA", "ASL", "NOP", "NOP", "ORA", "ASL", "SLO",
/* 1 */ "BPL", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO", "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
/* 2 */ "JSR", "AND", "NOP", "RLA", "BIT", "AND", "ROL", "RLA", "PLP", "AND", "ROL", "NOP", "BIT", "AND", "ROL", "RLA",
/* 3 */ "BMI", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA", "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
/* 4 */ "RTI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE", "PHA", "EOR", "LSR", "NOP", "JMP", "EOR", "LSR", "SRE",
/* 5 */ "BVC", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE", "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
/* 6 */ "RTS", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA", "PLA", "ADC", "ROR", "NOP", "JMP", "ADC", "ROR", "RRA",
/* 7 */ "BVS", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA", "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
/* 8 */ "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX", "DEY", "NOP", "TXA", "NOP", "STY", "STA", "STX", "SAX",
/* 9 */ "BCC", "STA", "NOP", "NOP", "STY", "STA", "STX", "SAX", "TYA", "STA", "TXS", "NOP", "NOP", "STA", "NOP", "NOP",
/* A */ "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX", "TAY", "LDA", "TAX", "NOP", "LDY", "LDA", "LDX", "LAX",
/* B */ "BCS", "LDA", "NOP", "LAX", "LDY", "LDA", "LDX", "LAX", "CLV", "LDA", "TSX", "LAX", "LDY", "LDA", "LDX", "LAX",
/* C */ "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP", "INY", "CMP", "DEX", "NOP", "CPY", "CMP", "DEC", "DCP",
/* D */ "BNE", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP", "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
/* E */ "CPX", "SBC", "NOP", "ISB", "CPX", "SBC", "INC", "ISB", "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISB",
/* F */ "BEQ", "SBC", "NOP", "ISB", "NOP", "SBC", "INC", "ISB", "SED", "SBC", "NOP", "ISB", "NOP", "SBC", "INC", "ISB"
};

void CPU::nestest_disassemble1() {
	printf("%04X  %02X ", pc - 1, opcode);
}

void CPU::nestest_disassemble2(AddressingMode addr_mode) {
	if (addr_mode == ABX || addr_mode == ABY || addr_mode == IND)
		printf("%02X %02X  ", ind_oper_addr & 0xFF, ((ind_oper_addr >> 8) & 0xFF));

	else if (addr_mode == ABS)
		printf("%02X %02X  ", oper_addr & 0xFF, (oper_addr >> 8) & 0xFF);

	else if (addr_mode == IMP || addr_mode == ACC)
		printf("       ");

	else if (addr_mode == ZPG || addr_mode == ZPX || addr_mode == ZPY || addr_mode == IDX || addr_mode == IDY)
		printf("%02X     ", ind_operand);

	else
		printf("%02X     ", operand);

	printf("%s ", instrsymbols[opcode]);

	if (addr_mode == IMP)
		printf("                            ");

	else if (addr_mode == ACC)
		printf("A                           ");

	else if (addr_mode == IMM)
			printf("#$%02X                        ", operand);

	else if (addr_mode == ZPG)
		printf("$%02X = %02X                    ", ind_operand, operand);

	else if (addr_mode == ZPX)
		printf("$%02X,X @ %02X = %02X             ", ind_operand, oper_addr & 0xFF, operand);

	else if (addr_mode == ZPY)
		printf("$%02X,Y @ %02X = %02X             ", ind_operand, oper_addr & 0xFF, operand);

	else if (addr_mode == REL)
		printf("$%04X                       ", pc + (int8_t)operand);

	else if (addr_mode == ABS) {
		if (opcode == 0x20 || opcode == 0x4C)
			printf("$%04X                       ", oper_addr);
		else
			printf("$%04X = %02X                  ", oper_addr, operand);
	}

	else if (addr_mode == ABX)
		printf("$%04X,X @ %04X = %02X         ", ind_oper_addr, oper_addr, operand);

	else if (addr_mode == ABY)
		printf("$%04X,Y @ %04X = %02X         ", ind_oper_addr, oper_addr, operand);

	else if (addr_mode == IND)
		printf("($%04X) = %04X              ", ind_oper_addr, memory.peek(ind_oper_addr));

	else if (addr_mode == IDX)
		printf("($%02X,X) @ %02X = %04X = %02X    ",
			ind_operand, ind_oper_addr & 0xFF,  oper_addr, operand);

	else if (addr_mode == IDY)
		printf("($%02X),Y = %04X @ %04X = %02X  ",
			ind_operand, ind_oper_addr, oper_addr, operand);

	else
		printf("OOPS");

	printf("A:%02X X:%02X Y:%02X P:%02X SP:%02X ", a, x, y, s, sp);
}

void CPU::nestest_disassemble3() {
	printf("CYC:%3d\n", cycles);
}
