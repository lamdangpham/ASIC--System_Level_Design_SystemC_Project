#include "Memory.h"
#include "RegFile.h"
#include "math.h"
#include <iostream>
#include "info_register.h"
#include "general_register.h"

#define ADD			1
#define AND			5
#define NOT			9
#define BR			0
#define JMP_RET		12
#define JSR_JSRR	4
#define LD			2
#define LDI			10
#define LDR			6
#define LEA			14
#define ST			3
#define STI			11
#define STR			7
#define TRAP		15
#define RTI			8

void StringHexToDecMem(char s[], int &datHex);
void DecToBinaryMem(int datDec, DatWidMem &dat);
void BinaryToDecMem(AddWidMem add, int &add_int);

class CPU_Function: public sc_module
{
public:
	sc_bv<16> IRDat;
	int cnt_Instruction;
	sc_bv<4> Opcode;
	sc_bv<1> Mode;
	sc_bv<16> ALU_out;
	bool N;
	bool Z;
	bool P;
	bool n;
	bool z;
	bool p;
	sc_bv<3> DR, SR1, SR2;
	sc_bv<5> Imm5;
	sc_bv<6> Offset6;
	sc_bv<9> PCOffset9;
	sc_bv<11> PCOffset11;
	sc_bv<8> trapvect8;

	sc_event Fetch_done;
	sc_event Decode_done;
	sc_event EvaluateAddress_done;
	sc_event FetchOperands_done;
	sc_event Execute_done;
	sc_event Store_done;

	sc_time period;

	Memory mem;
	RegFile regfile;

	Info_Reg<16> IR_Info;
	General_Reg<16> IR;

	Info_Reg<16> MAR_Info;
	General_Reg<16> MAR;

	Info_Reg<16> MDR_Info;
	General_Reg<16> MDR;

	Info_Reg<16> PC_Info;
	General_Reg<16> PC;

	CPU_Function(sc_module_name name, sc_time _period);

	template <int N> void ALU(sc_bv<16> &DR, sc_bv<16> SR1, sc_bv<N> Operand2)
	{
		DR = BinaryToDec<16>(SR1) + BinaryToDec<N>(Operand2);
	}

	void ALU(sc_bv<16> &DR, sc_bv<16> SR1);
	int LoadFile(char *fileName);
	void Fetch();
	void Decode();
	void EvaluateAddress();
	void FetchOperands();
	void Execute();
	void Store();
	void Display_Memory();
	void Display_RegFile();
};