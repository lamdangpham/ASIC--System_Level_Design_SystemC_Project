#include "CPU_Function.h"

CPU_Function::CPU_Function(sc_module_name name,sc_time _period): 
	sc_module(name), period(_period), mem("Memory", 32), regfile("RegFile", 8), t_socketCPU("t_socketCPU"), i_socketCPU_to_SCI("i_socketCPU_to_SCI"),
	IR_Info("IR_Info", "IR.txt"), IR("IR", IR_Info.reg_Default, IR_Info.bit_RangeNum, IR_Info.bit_Name, IR_Info.bit_Start, IR_Info.bit_End, IR_Info.bit_Option),
	MAR_Info("MAR_Info", "MAR.txt"), MAR("MAR", MAR_Info.reg_Default, MAR_Info.bit_RangeNum, MAR_Info.bit_Name, MAR_Info.bit_Start, MAR_Info.bit_End, MAR_Info.bit_Option),
	MDR_Info("MDR_Info", "MDR.txt"), MDR("MDR", MDR_Info.reg_Default, MDR_Info.bit_RangeNum, MDR_Info.bit_Name, MDR_Info.bit_Start, MDR_Info.bit_End, MDR_Info.bit_Option),
	PC_Info("PC_Info", "PC.txt"), PC("PC", PC_Info.reg_Default, PC_Info.bit_RangeNum, PC_Info.bit_Name, PC_Info.bit_Start, PC_Info.bit_End, PC_Info.bit_Option)
{
	t_socketCPU.register_b_transport(this, &CPU_Function::b_transport);
	N = false;
	Z = false;
	P = false;
}

void StringHexToDecMem(char s[], int &datDec)
{
	datDec = 0;
	char ref[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	int len_s = sizeof(s)/sizeof(char);
	int len_ref = sizeof(ref)/sizeof(char);
	for(int i = 0; i < len_s; i++)
	{
		for(int j = 0; j < len_ref; j++)
		{
			if(s[i] == ref[j])
			{
				datDec = datDec + j*pow(16, static_cast<double>(len_s-i-1));
			}
		}
	}
}

void DecToBinaryMem(int datDec, DatWidMem &dat)
{
	dat = 0;
	int du;
	int index = 0;
	while(datDec)
	{
		du = datDec % 2;
		dat[index] = du;
		datDec /= 2;
		index++;
	}
}

void BinaryToDecMem(AddWidMem add, int &add_int)
{
	add_int = 0;
	for(int i = 0; i < 16; i++)
	{
		add_int += add[i]*pow(2, static_cast<double>(i));
	}
}

int Binary_4ToDec(sc_bv<4> binary)
{
	int dec = 0;
	for(int i = 0; i < 16; i++)
	{
		dec += binary[i]*pow(2, static_cast<double>(i));
	}
	return dec;
}

template <int T>
int BinaryToDec(sc_bv<T> Bin)
{
	int temp = 0;
	if(Bin[T-1] == SC_LOGIC_1)
	{
		Bin = TwoCom<T>(Bin);
		for(int i = 0; i < T; i++)
		{
			temp += Bin[i]*pow(2, static_cast<double>(i));
		}
		return -temp;
	}
	else
	{
		for(int i = 0; i < T; i++)
		{
			temp += Bin[i]*pow(2, static_cast<double>(i));
		}
		return temp;
	}
}

template <int N>
sc_bv<N> TwoCom(sc_bv<N> binary)
{
	binary = BinaryToDec<N>(~binary) + 1;
	return binary;
}

void CPU_Function::LoadFile(char *fileName)
{
	FILE *file;
	file = fopen(fileName, "r");
	char s[5];
	DatWidMem dat;
	AddWidMem add;
	int add_int;
	int datDec;

	fgets(s, 5, file);
	getc(file);
	StringHexToDecMem(s, datDec);
	DecToBinaryMem(datDec, dat);
	add = dat;
	PC.write(add);
	BinaryToDecMem(add, add_int);

	bool flag = true;
	while(fgets(s, 5, file) != NULL)
	{
		getc(file);

		StringHexToDecMem(s, datDec);
		DecToBinaryMem(datDec, dat);

		mem.Put(add, dat);
		add_int++;
		DecToBinaryMem(add_int, add);
	}

	mem.Display();

	fclose(file);
}

void CPU_Function::Fetch()
{
	bool flag = true;
	while(true)
	{
		wait(1);
		if(flag == true)
		{
			MAR.write(PC.read());
			MDR.write(mem.Get(MAR.read()));
			IR.write(MDR.read());
		}
		if(IR.read() == "1111000000100101")
		{
			flag = false;
		}
		if(flag == true)
		{
			cout.setf(ios::binary,ios::basefield);
			cout << "--------------------" << name() << endl;   
			cout << "FETCH" << endl;
			cout << "PC      = " << PC.read() << endl;
			cout << "Mem     = " << MDR.read() << endl;
			cout << "IR      = " << IR.read() << endl;
			int temp = PC.read_uint();
			temp++;
			PC.write(temp); 
			cout << "next_PC = " << PC.read() << endl;   
			cout.setf(ios::dec,ios::basefield);
			cout << "At        " << sc_time_stamp() << endl;
			cout << "--------------------------" << endl;
			Fe.disable();
			De.enable();
		}
		else
		{
			Fe.disable();
			De.disable();
			Ev.disable();
			FeO.disable();
			Ex.disable();
			St.disable();
		}
	}
}

void CPU_Function::Decode()
{
	while(true)
	{
		wait(2);
		Opcode = IR.read().range(15, 12);
		cout.setf(ios::binary,ios::basefield);
		cout << "\t\t\t----------------" << name() << endl;   
		cout << "\t\t\tDECODE" << endl;
		switch(Binary_4ToDec(Opcode))
		{
		case ADD:
			cout << "\t\t\tOpcode  = " << Opcode << " (ADD)" << endl;
			Mode = IR.read()[5];
			if(Mode == 0)
			{
				DR = IR.read().range(11, 9);
				SR1 = IR.read().range(8, 6);
				SR2 = IR.read().range(2, 0);
				cout << "\t\t\tDR      = " << DR << endl;
				cout << "\t\t\tSR1     = " << SR1 << endl;
				cout << "\t\t\tSR2     = " << SR2 << endl;  
			}
			if(Mode == 1)
			{
				DR = IR.read().range(11, 9);
				SR1 = IR.read().range(8, 6);
				Imm5 = IR.read().range(4, 0);
				cout << "\t\t\tDR      = " << DR << endl;
				cout << "\t\t\tSR1     = " << SR1 << endl;
				cout << "\t\t\tImm5    = " << Imm5 << endl;
			}
			break;
		case AND:
			cout << "\t\t\tOpcode  = " << Opcode << " (AND)" << endl;
			Mode = IR.read()[5];
			if(Mode == 0)
			{
				DR = IR.read().range(11, 9);
				SR1 = IR.read().range(8, 6);
				SR2 = IR.read().range(2, 0);
				cout << "\t\t\tDR      = " << DR << endl;
				cout << "\t\t\tSR1     = " << SR1 << endl;
				cout << "\t\t\tSR2     = " << SR2 << endl;
			}
			if(Mode == 1)
			{
				DR = IR.read().range(11, 9);
				SR1 = IR.read().range(8, 6);
				Imm5 = IR.read().range(4, 0);
				cout << "\t\t\tDR      = " << DR << endl;
				cout << "\t\t\tSR1     = " << SR1 << endl;
				cout << "\t\t\tImm5    = " << Imm5 << endl;
			}
			break;
		case NOT:
				DR = IR.read().range(11, 9);
				SR1 = IR.read().range(8, 6);
				cout << "\t\t\tOpcode  = " << Opcode << " (NOT)" << endl;
				cout << "\t\t\tDR      = " << DR << endl;
				cout << "\t\t\tSR1     = " << SR1 << endl;
				break;
		case BR:
			n = IR.read()[11];
			z = IR.read()[10];
			p = IR.read()[9];
			PCOffset9 = IR.read().range(8, 0);
			cout << "\t\t\tOpcode    = " << Opcode << " (BR)" << endl;
			cout << "\t\t\tnzp       = " << n << p << p << endl;
			cout << "\t\t\tPCOffset9 = " << PCOffset9 << endl;
			break;
		case JMP_RET:
			SR1 = IR.read().range(8, 6);
			cout << "\t\t\tOpcode  = " << Opcode << " (JMP_RET)" << endl;
			cout << "\t\t\tSR1     = " << SR1 << endl;
			break;
		case JSR_JSRR:
			Mode = IR.read()[11];
			if(Mode == 1) 
			{
				PCOffset11 = IR.read().range(10, 0);
				cout << "\t\t\tOpcode     = " << Opcode << " (JSR)" << endl;
				cout << "\t\t\tPCOffset11 = " << PCOffset11 << endl;
			}
			if(Mode == 0)
			{
				SR1 = IR.read().range(8, 6);
				cout << "\t\t\tOpcode = " << Opcode << " (JSRR)" << endl;
				cout << "\t\t\tSR1    = " << SR1 << endl;
			}
			break;
		case LD:
			DR = IR.read().range(11, 9);
			PCOffset9 = IR.read().range(8, 0);
			cout << "\t\t\tOpcode    = " << Opcode << " (LD)" << endl;
			cout << "\t\t\tDR        = " << DR << endl;
			cout << "\t\t\tPCOffset9 = " << PCOffset9 << endl;
			break;
		case LDI:
			DR = IR.read().range(11, 9);
			PCOffset9 = IR.read().range(8, 0);
			cout << "\t\t\tOpcode    = " << Opcode << " (LDI)" << endl;
			cout << "\t\t\tDR        = " << DR << endl;
			cout << "\t\t\tPCOffset9 = " << PCOffset9 << endl;
			break;
		case LDR:
			DR = IR.read().range(11, 9);
			SR1 = IR.read().range(8, 6);
			Offset6 = IR.read().range(5, 0);
			cout << "\t\t\tOpcode  = " << Opcode << " (LDR)" << endl;
			cout << "\t\t\tSR1     = " << SR1 << endl;
			cout << "\t\t\tOffset6 = " << Offset6 << endl;
			break;
		case LEA:
			DR = IR.read().range(9, 3);
			PCOffset9 = IR.read().range(8, 0);
			cout << "\t\t\tOpcode    = " << Opcode << " (LEA)" << endl;
			cout << "\t\t\tDR        = " << DR << endl;
			cout << "\t\t\tPCOffset9 = " << PCOffset9 << endl;
			break;
		case ST:
			DR = IR.read().range(11, 9);
			PCOffset9 = IR.read().range(8, 0);
			cout << "\t\t\tOpcode    = " << Opcode << " (ST)" << endl;
			cout << "\t\t\tDR        = " << DR << endl;
			cout << "\t\t\tPCOffset9 = " << PCOffset9 << endl;
			break;
		case STI:
			DR = IR.read().range(11, 9);
			PCOffset9 = IR.read().range(8, 0);
			cout << "\t\t\tOpcode    = " << Opcode << " (STI)" << endl;
			cout << "\t\t\tDR        = " << DR << endl;
			cout << "\t\t\tPCOffset9 = " << PCOffset9 << endl;
			break;
		case STR:
			DR = IR.read().range(11, 9);
			SR1 = IR.read().range(8, 6);
			Offset6 = IR.read().range(5, 0);
			cout << "\t\t\tOpcode  = " << Opcode << " (STR)" << endl;
			cout << "\t\t\tSR1     = " << SR1 << endl;
			cout << "\t\t\tOffset6 = " << Offset6 << endl;
			break;
		//case TRAP:
			//trapvect8 = IR.read().range(7, 0);
			//break;
		}
		cout.setf(ios::dec,ios::basefield);
		cout << "\t\t\tAt        " << sc_time_stamp() << endl;
		cout << "\t\t\t---------------------" << endl;
		De.disable();
		Ev.enable();
	}
}

void CPU_Function::EvaluateAddress()
{
	while(true)
	{
		wait(3);
		cout.setf(ios::binary,ios::basefield);
		cout << "\t\t\t\t\t----------------" << name() << endl;   
		cout << "\t\t\t\t\tEVALUATE ADDRESS" << endl;
		switch(Binary_4ToDec(Opcode))
		{
		case ADD:
			if(Mode == 0)
			{
				regfile.DR = DR;
				regfile.SR1 = SR1;
				regfile.SR2 = SR2;
				cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
				cout << "\t\t\t\t\tregfile.SR1 = " << SR1 << endl;
				cout << "\t\t\t\t\tregfile.SR2 = " << SR2 << endl;
			}
			if(Mode == 1)
			{
				regfile.DR = DR;;
				regfile.SR1 = SR1;
				cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
				cout << "\t\t\t\t\tregfile.SR1 = " << SR1 << endl;
			}
			break;
		case AND:
			if(Mode == 0)
			{
				regfile.DR = DR;;
				regfile.SR1 = SR1;
				regfile.SR2 = SR2;
				cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
				cout << "\t\t\t\t\tregfile.SR1 = " << SR1 << endl;
				cout << "\t\t\t\t\tregfile.SR2 = " << SR2 << endl;
			}
			if(Mode == 1)
			{
				regfile.DR = DR;;
				regfile.SR1 = SR1;
				cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
				cout << "\t\t\t\t\tregfile.SR1 = " << SR1 << endl;
			}
			break;
		case NOT:
			regfile.DR = DR;;
			regfile.SR1 = SR1;
			cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
			cout << "\t\t\t\t\tregfile.SR1 = " << SR1 << endl;
			break;
		case JMP_RET:
			regfile.SR1 = SR1;
			cout << "\t\t\t\t\tregfile.SR1 = " << SR1 << endl;
			break;
		case JSR_JSRR:
			if(Mode == 0)
			{
				regfile.SR1 = SR1;
				cout << "\t\t\t\t\tregfile.SR1 = " << SR1 << endl;
			}
			break;
		case LD:
			regfile.DR = DR;
			cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
			break;
		case LDI:
			regfile.DR = DR;
			cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
			break;
		case LDR:
			regfile.DR = DR;
			regfile.SR1 = SR1;
			cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
			cout << "\t\t\t\t\tregfile.SR1 = " << SR1 << endl;
			break;
		case LEA:
			regfile.DR = DR;
			cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
			break;
		case ST:
			regfile.DR = DR;;
			cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
			break;
		case STI:
			regfile.DR = DR;
			cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
			break;
		case STR:
			regfile.DR = DR;
			regfile.SR1 = SR1;
			cout << "\t\t\t\t\tregfile.DR  = " << DR << endl;
			cout << "\t\t\t\t\tregfile.SR1 = " << SR1 << endl;
			break;
		}
		cout.setf(ios::dec,ios::basefield);
		cout << "\t\t\t\t\tAt            " << sc_time_stamp() << endl;
		cout << "\t\t\t\t\t----------------------" << endl;
		Ev.disable();
		FeO.enable();
	}
}

void CPU_Function::FetchOperands()
{
	while(true)
	{
		wait(4);
		cout.setf(ios::binary,ios::basefield);
		cout << "\t\t\t\t\t\t\t----------------" << name() << endl;   
		cout << "\t\t\t\t\t\t\tFETCH OPERANDS" << endl;
		switch(Binary_4ToDec(Opcode))
		{
		case ADD:
			if(Mode == 0)
			{
				regfile.SR1_out = regfile.Get(SR1);
				regfile.SR2_out = regfile.Get(SR2);
				cout << "\t\t\t\t\t\t\tSR1 = " << regfile.SR1_out << endl;
				cout << "\t\t\t\t\t\t\tSR2 = " << regfile.SR2_out << endl;
			}
			if(Mode == 1)
			{
				regfile.SR1_out = regfile.Get(SR1);
				cout << "\t\t\t\t\t\t\tSR1 = " << regfile.SR1_out << endl;
			}
			break;
		case AND:
			if(Mode == 0)
			{
				regfile.SR1_out = regfile.Get(SR1);
				regfile.SR2_out = regfile.Get(SR2);
				cout << "\t\t\t\t\t\t\tSR1 = " << regfile.SR1_out << endl;
				cout << "\t\t\t\t\t\t\tSR2 = " << regfile.SR2_out << endl;
			}
			if(Mode == 1)
			{
				regfile.SR1_out = regfile.Get(SR1);
				cout << "\t\t\t\t\t\t\tSR1 = " << regfile.SR1_out << endl;
			}
			break;
		case NOT:
			regfile.SR1_out = regfile.Get(SR1);
			cout << "\t\t\t\t\t\t\tSR1 = " << regfile.SR1_out << endl;
			break;
		case JMP_RET:
			regfile.SR1_out = regfile.Get(SR1);
			cout << "\t\t\t\t\t\t\tSR1 = " << regfile.SR1_out << endl;
			break;
		case JSR_JSRR:
			if(Mode == 0) 
			{
				regfile.SR1_out = regfile.Get(SR1);
				cout << "\t\t\t\t\t\t\tSR1 = " << regfile.SR1_out << endl;
			}
			break;
		case LDI:
			regfile.DR = DR;
			cout << "\t\t\t\t\t\t\tDR  = " << regfile.DR << endl;
			break;
		case LDR:
			regfile.SR1_out = regfile.Get(SR1);
			cout << "\t\t\t\t\t\t\tSR1 = " << regfile.SR1_out << endl;
			break;
		case LEA:
			regfile.DR = DR;
			cout << "\t\t\t\t\t\t\tDR  = " << regfile.DR << endl;
			break;
		case STI:
			regfile.SR1_out = regfile.Get(SR1);
			cout << "\t\t\t\t\t\t\tSR1 = " << regfile.SR1_out << endl;
			break;
		case STR:
			regfile.SR1_out = regfile.Get(SR1);
			cout << "\t\t\t\t\t\t\tSR1 = " << regfile.SR1_out << endl;
			break;
		}
		cout.setf(ios::dec,ios::basefield);
		cout << "\t\t\t\t\t\t\tAt    " << sc_time_stamp() << endl;
		cout << "\t\t\t\t\t\t\t----------------------" << endl;
		FeO.disable();
		Ex.enable();
	}
}

void CPU_Function::Execute()
{
	while(true)
	{
		wait(5);
		cout.setf(ios::dec,ios::basefield);
		cout << "\t\t\t\t\t\t\t\t\t------------------------" << name() << endl;   
		cout << "\t\t\t\t\t\t\t\t\tEXECUTE" << endl;
		switch(Binary_4ToDec(Opcode))
		{
		case ADD:
			if(Mode == 0)
			{
				ALU<16>(ALU_out, regfile.SR1_out, regfile.SR2_out);
				if(ALU_out[15] == SC_LOGIC_1) N = true;
				else N = false;
				if(BinaryToDec(ALU_out) == 0) Z = true;
				else Z = false;
				if((ALU_out[15] == SC_LOGIC_0) && (BinaryToDec(ALU_out) != 0)) P = true;
				else P = false;
				cout << "\t\t\t\t\t\t\t\t\t     NPZ = " << N << P << Z << endl;
				break;
			}
			if(Mode == 1)
			{
				ALU<5>(ALU_out, regfile.SR1_out, Imm5);
				if(ALU_out[15] == SC_LOGIC_1) N = true;
				else N = false;
				if(BinaryToDec(ALU_out) == 0) Z = true;
				else Z = false;
				if((ALU_out[15] == SC_LOGIC_0) && (BinaryToDec(ALU_out) != 0)) P = true;
				else P = false;
				cout << "\t\t\t\t\t\t\t\t\t     NPZ = " << N << P << Z << endl;
				break;
			}
			break;
		case AND:
			if(Mode == 0)
			{
				ALU<16>(ALU_out, regfile.SR1_out, regfile.SR2_out);
				if(ALU_out[15] == SC_LOGIC_1) N = true;
				else N = false;
				if(BinaryToDec(ALU_out) == 0) Z = true;
				else Z = false;
				if((ALU_out[15] == SC_LOGIC_0) && (BinaryToDec(ALU_out) != 0)) P = true;
				else P = false;
				cout << "\t\t\t\t\t\t\t\t\t     NPZ = " << N << P << Z << endl;
				break;
			}
			if(Mode == 1)
			{
				ALU<5>(ALU_out, regfile.SR1_out, Imm5);
				if(ALU_out[15] == SC_LOGIC_1) N = true;
				else N = false;
				if(BinaryToDec(ALU_out) == 0) Z = true;
				else Z = false;
				if((ALU_out[15] == SC_LOGIC_0) && (BinaryToDec(ALU_out) != 0)) P = true;
				else P = false;
				cout << "\t\t\t\t\t\t\t\t\t     NPZ = " << N << P << Z << endl;
				break;
			}
			break;
		case NOT:
			ALU(ALU_out, regfile.SR1_out);
			if(ALU_out[15] == SC_LOGIC_1) N = true;
			else N = false;
			if(BinaryToDec(ALU_out) == 0) Z = true;
			else Z = false;
			if((ALU_out[15] == SC_LOGIC_0) && (BinaryToDec(ALU_out) != 0)) P = true;
			else P = false;
			cout << "\t\t\t\t\t\t\t\t\t     NPZ = " << N << P << Z << endl;
			break;
		case BR:
			ALU<9>(ALU_out, PC.read(), PCOffset9);
			if((n&N)|(z&Z)|(p&P)) 
			{
				PC.write(ALU_out);
			}
			cout << "\t\t\t\t\t\t\t\t\t     PC = " << PC.read() << endl;
			break;
		case JMP_RET:
			PC.write(regfile.SR1_out);
			break;
		case JSR_JSRR:
			if(Mode == 1)
			{
				regfile.Put("111", PC.read());
				ALU<11>(ALU_out, PC.read(), PCOffset11);
				PC.write(ALU_out);
			}
			if(Mode == 0)
			{
				regfile.Put("111", PC.read());
				PC.write(regfile.SR1_out);
			}
			break;
		case LD:
			ALU<9>(ALU_out, PC.read(), PCOffset9);
			break;
		case LDI:
			ALU<9>(ALU_out, PC.read(), PCOffset9);
			break;
		case LDR:
			ALU<9>(ALU_out, regfile.SR1_out, Offset6);
			break;
		case LEA:
			ALU<9>(ALU_out, PC.read(), PCOffset9);
			if(ALU_out[15] == SC_LOGIC_1) N = true;
			else N = false;
			if(BinaryToDec(ALU_out) == 0) Z = true;
			else Z = false;
			if((ALU_out[15] == SC_LOGIC_0) && (BinaryToDec(ALU_out) != 0)) P = true;
			else P = false;
			cout << "\t\t\t\t\t\t\t\t\t     NPZ = " << N << P << Z << endl;
			break;
		case ST:
			ALU<9>(ALU_out, PC.read(), PCOffset9);
			break;
		case STI:
			ALU<9>(ALU_out, PC.read(), PCOffset9);
			break;
		case STR:
			ALU<9>(ALU_out, regfile.SR1_out, Offset6);
			break;
		}
		cout.setf(ios::dec,ios::basefield);
		cout << "\t\t\t\t\t\t\t\t\tAt   " << sc_time_stamp() << endl;
		cout << "\t\t\t\t\t\t\t\t\t------------------------------" << endl;
		Ex.disable();
		St.enable();
	}
}

void CPU_Function::Store()
{
	while(true)
	{
		wait(6);
		cout.setf(ios::dec,ios::basefield);
		cout << "\t\t\t\t\t\t\t\t\t\t\t------------------------------------------" << name() << endl;   
		cout << "\t\t\t\t\t\t\t\t\t\t\tSTORE" << endl;
		switch(Binary_4ToDec(Opcode))
		{
		case ADD:
			if(Mode == 0)
			{
				regfile.Put(DR, ALU_out);
				cout << "\t\t\t\t\t\t\t\t\t\t\tR" << BinaryToDec<3>(DR) << " <- " << ALU_out << endl;
			}
			if(Mode == 1)
			{
				regfile.Put(DR, ALU_out);
				cout << "\t\t\t\t\t\t\t\t\t\t\tR" << BinaryToDec<3>(DR) << " <- " << ALU_out << endl;
			}
			break;
		case AND:
			if(Mode == 0)
			{
				regfile.Put(DR, ALU_out);
				cout << "\t\t\t\t\t\t\t\t\t\t\tR" << BinaryToDec<3>(DR) << " <- " << ALU_out << endl;
			}
			if(Mode == 1)
			{
				regfile.Put(DR, ALU_out);
				cout << "\t\t\t\t\t\t\t\t\t\t\tR" << BinaryToDec<3>(DR) << " <- " << ALU_out << endl;
			}
			break;
		case NOT:
			regfile.Put(DR, ALU_out);
			cout << "\t\t\t\t\t\t\t\t\t\t\tR" << BinaryToDec<3>(DR) << " <- " << ALU_out << endl;
			break;
		case LD:
			MAR.write(ALU_out);
			MDR.write(mem.Get(MAR.read()));
			regfile.Put(DR, MDR.read());
			cout << "\t\t\t\t\t\t\t\t\t\t\tR" << BinaryToDec<3>(DR) << " <- mem(" << MAR.read() << ") = " << MDR.read() << endl;
			if(MDR.read()[15] == SC_LOGIC_1) N = true;
			else N = false;
			if(BinaryToDec<16>(MDR.read()) == 0) Z = true;
			else Z = false;
			if((MDR.read()[15] == SC_LOGIC_0) && (BinaryToDec<16>(MDR.read()) != 0)) P = true;
			else P = false;
			cout << "\t\t\t\t\t\t\t\t\t\t\tNPZ    = " << N << P << Z << endl;
			break;
		case LDI:
			MAR.write(ALU_out);
			MDR.write(mem.Get(MAR.read()));
			MDR.write(mem.Get(MDR.read()));
			regfile.Put(DR, MDR.read());
			cout << "\t\t\t\t\t\t\t\t\t\t\tR" << BinaryToDec<3>(DR) << " <- mem(" << MAR.read() << ") = " << MDR.read() << endl;
			if(MDR.read()[15] == SC_LOGIC_1) N = true;
			else N = false;
			if(BinaryToDec<16>(MDR.read()) == 0) Z = true;
			else Z = false;
			if((MDR.read()[15] == SC_LOGIC_0) && (BinaryToDec<16>(MDR.read()) != 0)) P = true;
			else P = false;
			cout << "\t\t\t\t\t\t\t\t\t\t\tNPZ    = " << N << P << Z << endl;
			break;
		case LDR:
			MAR.write(ALU_out);
			MDR.write(mem.Get(MAR.read()));
			regfile.Put(DR, MDR.read());
			cout << "\t\t\t\t\t\t\t\t\t\t\tR" << BinaryToDec<3>(DR) << " <- mem(" << MAR.read() << ") = " << MDR.read() << endl;
			if(MDR.read()[15] == SC_LOGIC_1) N = true;
			else N = false;
			if(BinaryToDec<16>(MDR.read()) == 0) Z = true;
			else Z = false;
			if((MDR.read()[15] == SC_LOGIC_0) && (BinaryToDec<16>(MDR.read()) != 0)) P = true;
			else P = false;
			cout << "\t\t\t\t\t\t\t\t\t\t\tNPZ    = " << N << P << Z << endl;
			break;
		case LEA:
			regfile.Put(DR, ALU_out);
			cout << "\t\t\t\t\t\t\t\t\t\t\tR" << BinaryToDec<3>(DR) << " <- " << ALU_out << endl;
			break;
		case ST:
			MAR.write(ALU_out);
			MDR.write(regfile.Get(DR));
			mem.Put(MAR.read(), MDR.read());
			cout << "\t\t\t\t\t\t\t\t\t\t\tmem(" << MAR.read() << ") <- R" << BinaryToDec<3>(DR) << " = " << MDR.read()  << " (" << BinaryToDec<16>(MDR.read()) << ")"<< endl;
			if(MAR.read_uint() == PORT_SCI)
			{
				tlm_write<16>(mem.Get(MAR.read()));
			}
			break;
		case STI:
			MAR.write(ALU_out);
			MDR.write(mem.Get(MAR.read()));
			MAR.write(MDR.read());
			MDR.write(regfile.Get(DR));
			mem.Put(MAR.read(), MDR.read());
			cout << "\t\t\t\t\t\t\t\t\t\t\tmem(" << MAR.read() << ") <- R" << BinaryToDec<3>(DR) << " = " << MDR.read() << " (" << BinaryToDec<16>(MDR.read()) << ")" << endl;
			if(MAR.read_uint() == PORT_SCI)
			{
				tlm_write<16>(mem.Get(MAR.read()));
			}
			break;
		case STR:
			MAR.write(ALU_out);
			MDR.write(regfile.Get(DR));
			mem.Put(MAR.read(), MDR.read());
			cout << "\t\t\t\t\t\t\t\t\t\t\tmem(" << MAR.read() << ") <- R" << BinaryToDec<3>(DR) << " = " << MDR.read()  << " (" << BinaryToDec<16>(MDR.read()) << ")"<< endl;
			if(MAR.read_uint() == PORT_SCI)
			{
				tlm_write<16>(mem.Get(MAR.read()));
			}
			break;
		}
		cout.setf(ios::dec,ios::basefield);
		cout << "\t\t\t\t\t\t\t\t\t\t\tAt   " << sc_time_stamp() << endl;
		cout << "\t\t\t\t\t\t\t\t\t\t\t----------------------------------------------------" << endl;
		//St.disable();
		Fe.enable();
		De.enable();
		Ev.enable();
		FeO.enable();
		Ex.enable();
		//St.enable();

		Display_RegFile();
		if((Binary_4ToDec(Opcode) == ST) || (Binary_4ToDec(Opcode) == STR) || (Binary_4ToDec(Opcode) == STI))
		{
			Display_Memory();
		}
	}
}

void CPU_Function::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	//wait(delay);
	tlm::tlm_command	cmd = trans.get_command();
    sc_dt::uint64		adr = trans.get_address() / 4;
    unsigned char*		ptr = trans.get_data_ptr();
    unsigned int		len = trans.get_data_length();
    unsigned char*		byt = trans.get_byte_enable_ptr();
    unsigned int		wid = trans.get_streaming_width();

    //if (adr >= sc_dt::uint64(256) || byt != 0 || len > 4 || wid < len)
      //SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");

	trans.set_response_status( tlm::TLM_OK_RESPONSE );

	for(int i = 0; i < 16; i++)
	{
		if(ptr[i] == '1') data_recei[i] = SC_LOGIC_1;
		if(ptr[i] == '0') data_recei[i] = SC_LOGIC_0;
	}	
	MAR.write(PORT_SCI);
	MDR.write(data_recei);
	mem.Put(MAR.read(), MDR.read());
	cout << "Data SCI " << name() <<" send to CPU " << name() << " " << mem.Get(PORT_SCI) << " " << endl << endl; //sc_time_stamp() << endl;
	Display_Memory();
}

void CPU_Function::ALU(sc_bv<16> &DR, sc_bv<16> SR1)
{
	DR = ~SR1;
	if(SR1[15] == SC_LOGIC_1)
	{
		cout << "\t\t\t\t\t\t\t\t\tALU: In1 = " << -BinaryToDec<16>(TwoCom(SR1)) << endl;
		cout << "\t\t\t\t\t\t\t\t\t     Out = " << BinaryToDec<16>(DR) << endl;
	}
	else
	{
		cout << "\t\t\t\t\t\t\t\t\tALU: In1 = " << BinaryToDec<16>(SR1) << endl;
		cout << "\t\t\t\t\t\t\t\t\t     Out = " << -BinaryToDec<16>(TwoCom(DR)) << endl;
	}
}

void CPU_Function::Display_Memory()
{
	mem.Display();
}

void CPU_Function::Display_RegFile()
{
	regfile.Display();
}