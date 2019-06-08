#include "CPU_Function.h"

CPU_Function::CPU_Function(sc_module_name name,sc_time _period): sc_module(name), period(_period), mem("Memory", 32), regfile("RegFile", 8),
	IR_Info("IR_Info", "IR.txt"), IR("IR", IR_Info.reg_Default, IR_Info.bit_RangeNum, IR_Info.bit_Name, IR_Info.bit_Start, IR_Info.bit_End, IR_Info.bit_Option),
	MAR_Info("MAR_Info", "MAR.txt"), MAR("MAR", MAR_Info.reg_Default, MAR_Info.bit_RangeNum, MAR_Info.bit_Name, MAR_Info.bit_Start, MAR_Info.bit_End, MAR_Info.bit_Option),
	MDR_Info("MDR_Info", "MDR.txt"), MDR("MDR", MDR_Info.reg_Default, MDR_Info.bit_RangeNum, MDR_Info.bit_Name, MDR_Info.bit_Start, MDR_Info.bit_End, MDR_Info.bit_Option),
	PC_Info("PC_Info", "PC.txt"), PC("PC", PC_Info.reg_Default, PC_Info.bit_RangeNum, PC_Info.bit_Name, PC_Info.bit_Start, PC_Info.bit_End, PC_Info.bit_Option)
{
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
	for(int i = 0; i < T; i++)
	{
		temp += Bin[i]*pow(2, static_cast<double>(i));
	}
	return temp;
}

int CPU_Function::LoadFile(char *fileName)
{
	FILE *file;
	file = fopen(fileName, "r");
	char s[5];
	DatWidMem dat;
	AddWidMem add;
	int add_int;
	int datDec;

	cnt_Instruction = 0;

	if(file == NULL)
	{
		cout << "\nFile " << fileName << " does not exist!";
		return 0;
	}

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
		if(datDec == 61477)
		{
			flag = false;
		}
		if(flag == true)
		{
			cnt_Instruction++;
		}

	}

	mem.Display();

	fclose(file);

	return 1;
}

void CPU_Function::Fetch()
{
	bool flag = true;
	while(true)
	{
		wait();
		if(flag == true)
		{
			MAR.write(PC.read());
			MDR.write(mem.Get(MAR.read()));
			IR.write(MDR.read());
			int temp = PC.read_uint();
			temp++;
			PC.write(temp);
			Fetch_done.notify();
			cout << "[Fetch_done] " << sc_time_stamp() << endl;
			cnt_Instruction--;
		}
		if(cnt_Instruction == 0)
		{
			flag = false;
		}
	}
}

void CPU_Function::Decode()
{
	while(true)
	{
		//wait();
		wait(Fetch_done);
		Opcode = IR.read().range(15, 12);
		switch(Binary_4ToDec(Opcode))
		{
		case ADD:
			Mode = IR.read()[5];
			if(Mode == 0)
			{
				DR = IR.read().range(11, 9);
				SR1 = IR.read().range(8, 6);
				SR2 = IR.read().range(2, 0);
			}
			if(Mode == 1)
			{
				DR = IR.read().range(11, 9);
				SR1 = IR.read().range(8, 6);
				Imm5 = IR.read().range(4, 0);
			}
			break;
		case AND:
			Mode = IR.read()[5];
			if(Mode == 0)
			{
				DR = IR.read().range(11, 9);
				SR1 = IR.read().range(8, 6);
				SR2 = IR.read().range(2, 0);
			}
			if(Mode == 1)
			{
				DR = IR.read().range(11, 9);
				SR1 = IR.read().range(8, 6);
				Imm5 = IR.read().range(4, 0);
			}
			break;
		case NOT:
				DR = IR.read().range(11, 9);
				SR1 = IR.read().range(8, 6);
				break;
		case BR:
			n = IR.read()[11];
			z = IR.read()[10];
			p = IR.read()[9];
			PCOffset9 = IR.read().range(8, 0);
			break;
		case JMP_RET:
			SR1 = IR.read().range(8, 6);
			break;
		case JSR_JSRR:
			Mode = IR.read()[11];
			if(Mode == 1) PCOffset11 = IR.read().range(10, 0);
			if(Mode == 0) SR1 = IR.read().range(8, 6);
			break;
		case LD:
			DR = IR.read().range(11, 9);
			PCOffset9 = IR.read().range(8, 0);
			break;
		case LDI:
			DR = IR.read().range(11, 9);
			PCOffset9 = IR.read().range(8, 0);
			break;
		case LDR:
			DR = IR.read().range(11, 9);
			SR1 = IR.read().range(8, 6);
			Offset6 = IR.read().range(5, 0);
			break;
		case LEA:
			DR = IR.read().range(9, 3);
			PCOffset9 = IR.read().range(8, 0);
			break;
		case ST:
			DR = IR.read().range(11, 9);
			PCOffset9 = IR.read().range(8, 0);
			break;
		case STI:
			DR = IR.read().range(11, 9);
			PCOffset9 = IR.read().range(8, 0);
			break;
		case STR:
			DR = IR.read().range(11, 9);
			SR1 = IR.read().range(8, 6);
			Offset6 = IR.read().range(5, 0);
			break;
		case TRAP:
			trapvect8 = IR.read().range(7, 0);
			break;
		}
		switch(Binary_4ToDec(Opcode))
		{
		case ADD:
			cout << "[ADD]" << endl;
			break;
		case AND:
			cout << "[AND]" << endl;
		case NOT:
				cout << "[NOT]" << endl;
		case BR:
			cout << "[BR]" << endl;
		case JMP_RET:
			cout << "[JMP_RET]" << endl;
			break;
		case JSR_JSRR:
			cout << "[JSR_JSRR]" << endl;
		case LD:
			cout << "[LD]" << endl;
			break;
		case LDI:
			cout << "[LDI]" << endl;
			break;
		case LDR:
			cout << "[LDR]" << endl;
			break;
		case LEA:
			cout << "[LEA]" << endl;
			break;
		case ST:
			cout << "[ST]" << endl;
			break;
		case STI:
			cout << "[STI]" << endl;
			break;
		case STR:
			cout << "[STR]" << endl;
			break;
		case TRAP:
			cout << "[TRAP]" << endl;
			break;
		}
		//cout << "Opcode   " << "DR   " << "SR1  " << "SR2  " << "Imm5   " << "PCOffset9  " << "PCOffset11   " << "Offset6  " << endl;
		//cout << Opcode << "     " << DR << "  " << SR1 << "  " << SR2 << "  " << Imm5 << "  " << PCOffset9 << "  " << PCOffset11 << "  " << Offset6 << endl << endl;
		cout << "[Decode_done] " << sc_time_stamp() << endl;
		Decode_done.notify();
	}
}

void CPU_Function::EvaluateAddress()
{
	while(true)
	{
		//wait();
		wait(Decode_done);
		switch(Binary_4ToDec(Opcode))
		{
		case ADD:
			if(Mode == 0)
			{
				regfile.DR = DR;
				regfile.SR1 = SR1;
				regfile.SR2 = SR2;
			}
			if(Mode == 1)
			{
				regfile.DR = DR;;
				regfile.SR1 = SR1;
			}
			break;
		case AND:
			if(Mode == 0)
			{
				regfile.DR = DR;;
				regfile.SR1 = SR1;
				regfile.SR2 = SR2;
			}
			if(Mode == 1)
			{
				regfile.DR = DR;;
				regfile.SR1 = SR1;
			}
			break;
		case NOT:
			regfile.DR = DR;;
			regfile.SR1 = SR1;
			break;
		case JMP_RET:
			regfile.SR1 = SR1;
			break;
		case JSR_JSRR:
			if(Mode == 0) regfile.SR1 = SR1;
			break;
		case LD:
			regfile.DR = DR;
			break;
		case LDI:
			regfile.DR = DR;
			break;
		case LDR:
			regfile.DR = DR;
			regfile.SR1 = SR1;
			break;
		case LEA:
			regfile.DR = DR;;
			break;
		case ST:
			regfile.DR = DR;;
			break;
		case STI:
			regfile.DR = DR;;
			break;
		case STR:
			regfile.DR = DR;;
			regfile.SR1 = SR1;
			break;
		}
		cout << "[EvaluateAddress_done] " << sc_time_stamp() << endl;
		EvaluateAddress_done.notify();
	}
}

void CPU_Function::FetchOperands()
{
	while(true)
	{
		//wait();
		wait(EvaluateAddress_done);
		switch(Binary_4ToDec(Opcode))
		{
		case ADD:
			if(Mode == 0)
			{
				regfile.SR1_out = regfile.Get(SR1);
				regfile.SR2_out = regfile.Get(SR2);
			}
			if(Mode == 1)
			{
				regfile.SR1_out = regfile.Get(SR1);
			}
			break;
		case AND:
			if(Mode == 0)
			{
				regfile.SR1_out = regfile.Get(SR1);
				regfile.SR2_out = regfile.Get(SR2);
			}
			if(Mode == 1)
			{
				regfile.SR1_out = regfile.Get(SR1);
			}
			break;
		case NOT:
			regfile.SR1_out = regfile.Get(SR1);
			break;
		case JMP_RET:
			regfile.SR1_out = regfile.Get(SR1);
			break;
		case JSR_JSRR:
			if(Mode == 0) regfile.SR1_out = regfile.Get(SR1);
			break;
		case LDI:
			regfile.DR = DR;
			break;
		case LDR:
			regfile.SR1_out = regfile.Get(SR1);
			break;
		case LEA:
			regfile.DR = DR;
			break;
		case STI:
			regfile.SR1_out = regfile.Get(SR1);
			break;
		case STR:
			regfile.SR1_out = regfile.Get(SR1);
			break;
		}
		cout << "[FetchOperands_done] " << sc_time_stamp() << endl;
		FetchOperands_done.notify();
	}
}

void CPU_Function::Execute()
{
	while(true)
	{
		//wait();
		wait(FetchOperands_done);
		switch(Binary_4ToDec(Opcode))
		{
		case ADD:
			if(Mode == 0)
			{
				ALU<16>(ALU_out, regfile.SR1_out, regfile.SR2_out);
				if(BinaryToDec(ALU_out) < 0) N = true;
				if(BinaryToDec(ALU_out) == 0) Z = true;
				if(BinaryToDec(ALU_out) > 0) P = true;
			}
			if(Mode == 1)
			{
				ALU<5>(ALU_out, regfile.SR1_out, Imm5);
				if(BinaryToDec(ALU_out) < 0) N = true;
				if(BinaryToDec(ALU_out) == 0) Z = true;
				if(BinaryToDec(ALU_out) > 0) P = true;
			}
			break;
		case AND:
			if(Mode == 0)
			{
				ALU<16>(ALU_out, regfile.SR1_out, regfile.SR2_out);
				if(BinaryToDec(ALU_out) < 0) N = true;
				if(BinaryToDec(ALU_out) == 0) Z = true;
				if(BinaryToDec(ALU_out) > 0) P = true;
			}
			if(Mode == 1)
			{
				ALU<5>(ALU_out, regfile.SR1_out, Imm5);
				if(BinaryToDec(ALU_out) < 0) N = true;
				if(BinaryToDec(ALU_out) == 0) Z = true;
				if(BinaryToDec(ALU_out) > 0) P = true;
			}
			break;
		case NOT:
			ALU(ALU_out, regfile.SR1_out);
			if(BinaryToDec(ALU_out) < 0) N = true;
			if(BinaryToDec(ALU_out) == 0) Z = true;
			if(BinaryToDec(ALU_out) > 0) P = true;
			break;
		case BR:
			ALU<9>(ALU_out, PC.read(), PCOffset9);
			if((n&N)|(z&Z)|(p&P)) PC.write(ALU_out);
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
			if(BinaryToDec(ALU_out) < 0) N = true;
			if(BinaryToDec(ALU_out) == 0) Z = true;
			if(BinaryToDec(ALU_out) > 0) P = true;
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
		cout << "[Execute_done] " << sc_time_stamp() << endl;
		Execute_done.notify();
	}
}

void CPU_Function::Store()
{
	while(true)
	{
		//wait();
		wait(Execute_done);
		switch(Binary_4ToDec(Opcode))
		{
		case ADD:
			if(Mode == 0)
			{
				regfile.Put(DR, ALU_out);
			}
			if(Mode == 1)
			{
				regfile.Put(DR, ALU_out);
			}
			break;
		case AND:
			if(Mode == 0)
			{
				regfile.Put(DR, ALU_out);
			}
			if(Mode == 1)
			{
				regfile.Put(DR, ALU_out);
			}
			break;
		case NOT:
			regfile.Put(DR, ALU_out);
			break;
		case LD:
			MAR.write(ALU_out);
			MDR.write(mem.Get(MAR.read()));
			regfile.Put(DR, MDR.read());
			if(BinaryToDec(MDR.read()) < 0) N = true;
			if(BinaryToDec(MDR.read()) == 0) Z = true;
			if(BinaryToDec(MDR.read()) > 0) P = true;
			break;
		case LDI:
			MAR.write(ALU_out);
			MDR.write(mem.Get(MAR.read()));
			MDR.write(mem.Get(MDR.read()));
			regfile.Put(DR, MDR.read());
			if(BinaryToDec(MDR.read()) < 0) N = true;
			if(BinaryToDec(MDR.read()) == 0) Z = true;
			if(BinaryToDec(MDR.read()) > 0) P = true;
			break;
		case LDR:
			MAR.write(ALU_out);
			MDR.write(mem.Get(MAR.read()));
			regfile.Put(DR, MDR.read());
			if(BinaryToDec(MDR.read()) < 0) N = true;
			if(BinaryToDec(MDR.read()) == 0) Z = true;
			if(BinaryToDec(MDR.read()) > 0) P = true;
			break;
		case LEA:
			regfile.Put(DR, ALU_out);
			break;
		case ST:
			MAR.write(ALU_out);
			MDR.write(regfile.Get(DR));
			mem.Put(MAR.read(), MDR.read());
			break;
		case STI:
			MAR.write(ALU_out);
			MDR.write(mem.Get(MAR.read()));
			MAR.write(MDR.read());
			MDR.write(regfile.Get(DR));
			mem.Put(MAR.read(), MDR.read());
			break;
		case STR:
			MAR.write(ALU_out);
			MDR.write(regfile.Get(DR));
			mem.Put(MAR.read(), MDR.read());
			break;
		}
		cout << "[Store_done] " << sc_time_stamp() << endl;
		Display_RegFile();
	}
}

void CPU_Function::ALU(sc_bv<16> &DR, sc_bv<16> SR1)
{
	DR = ~SR1;
}

void CPU_Function::Display_Memory()
{
	mem.Display();
}

void CPU_Function::Display_RegFile()
{
	regfile.Display();
}