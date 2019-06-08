// Filename: general_register.h
//----------------------------------------------------------------------
//  Copyright (c) 2016 by PhamGiaHuy-HCMUT
//  https://www.facebook.com/profile.php?id=100010839165665
//----------------------------------------------------------------------
// Getting Started with System-Level Design, General Register_v2.1

#include "systemc.h"
#include "string"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

#define NONE		0
#define READ_ONLY	1
#define WRITE_ONLY	2
#define READ_WRITE	3

template <int N>
class General_Reg: public sc_module
{
public:
	General_Reg(sc_module_name reg_Name_, sc_bv<N> reg_DataDefault_, int reg_BitRangeNum_, string bit_Name_[], int bit_Start_[], int bit_End_[], int bit_Option_[]):
	  sc_module(reg_Name_), reg_Name(reg_Name_), reg_DataDefault(reg_DataDefault_), reg_BitRangeNum(reg_BitRangeNum_), bit_Name(bit_Name_), bit_Start(bit_Start_), bit_End(bit_End_), bit_Option(bit_Option_)
	{
		reg_Data = reg_DataDefault;
		//cout << "Data_default [" << reg_Name << "] " << reg_DataDefault << endl << endl;
		length = reg_BitRangeNum * 2;

		for(int i = 0; i < reg_BitRangeNum; i++)
		{
			for(int j = bit_Start[i]; j < (bit_End[i] + 1); j++)
			{
				reg_Status[j] = bit_Option[i];
			}
		}

//------------------------------------------------------------------------------------		
		/*for(int i = N-1; i >= 0; i--)
		{
			if(reg_Status[i] == 0)
			{
				cout << "NONE | ";
			}
			if(reg_Status[i] == 1)
			{
				cout << "READ_ONLY | ";
			}
			if(reg_Status[i] == 2)
			{
				cout << "WRITE_ONLY | ";
			}
			if(reg_Status[i] == 3)
			{
				cout << "READ_WRITE | ";
			}
		}
		cout << endl << endl;*/
//-----------------------------------------------------------------------------------
	}

public:
	sc_module_name reg_Name;
	sc_bv<N> reg_DataDefault;
	int reg_BitRangeNum;
	int length;
	string *bit_Name;
	int *bit_Start;
	int *bit_End;
	int *bit_Option;
	int reg_Status[N];
	sc_bv<N> reg_Data;
	sc_uint<N> Data;

public:
	void write(sc_bv<N> data_write = 0)
	{
		for(int i = 0; i < N; i++)
		{
			if(reg_Status[i] == NONE)
			{
			
			}

			if(reg_Status[i] == READ_ONLY)
			{
				cout << "Can not WRITE to bit " << i << " of " << reg_Name << " !!!!!!" << endl << endl;
			}

			if((reg_Status[i] == WRITE_ONLY) | (reg_Status[i] == READ_WRITE))
			{
				reg_Data[i] = data_write[i];
			}
		}
		//cout << "[WRITE] " << reg_Name << " " << reg_Data << endl << endl;
	}

public:
	sc_bv<N> read()
	{
		//sc_bv<N> data_read = 0;
		for(int i = 0; i < N; i++)
		{
			if(reg_Status[i] == NONE)
			{
			
			}

			if(reg_Status[i] == WRITE_ONLY)
			{
				cout << "Can not READ from bit " << i << " of " << reg_Name << " !!!!!!" << endl << endl;
			}

			if((reg_Status[i] == READ_ONLY) | (reg_Status[i] == READ_WRITE))
			{
				Data[i] = reg_Data[i];
			}
		}
		//cout << "[Read] " << reg_Name << " " << reg_Data << endl << endl;
		return Data;
	}

public:
		template <int N>
		void write_bit(string Bit_Name, sc_bv<N> data_WriteBit)
		{
			int index = 0;
			for(int i = 0; i < reg_BitRangeNum; i++)
			{
				if(Bit_Name == bit_Name[i]) 
				{
					index = i;
				}
			}

			if(bit_Option[index] == NONE)
			{

			}

			if(bit_Option[index] == READ_ONLY)
			{
				cout << "Can not WRITE to " << Bit_Name << " of " << reg_Name << " !!!!!!" << endl << endl;
			}

			if((bit_Option[index] == WRITE_ONLY) | (bit_Option[index] == READ_WRITE))
			{
				int temp = 0;
				for(int i = bit_Start[index]; i < bit_End[index] + 1; i++)
				{
					reg_Data[i] = data_WriteBit[temp];
					temp++;
				} 
			}
			//cout << "[WRITE " << N << " BIT] " << reg_Name << " " << Bit_Name << " " << reg_Data << endl << endl;
		}

public:
		template <int N>
		sc_bv<N> read_bit(string Bit_Name)
		{
			int index = 0;
			for(int i = 0; i < reg_BitRangeNum; i++)
			{
				if(Bit_Name == bit_Name[i]) 
				{
					index = i;
				}
			}

			if(bit_Option[index] == NONE)
			{

			}

			if(bit_Option[index] == WRITE_ONLY)
			{
				cout << "Can not READ from " << Bit_Name << " of " << reg_Name << " !!!!!!" << endl << endl;
			}

			if((bit_Option[index] == READ_ONLY) | (bit_Option[index] == READ_WRITE))
			{
				int temp = 0;
				for(int i = bit_Start[index]; i < bit_End[index] + 1; i++)
				{
					Data[temp] = reg_Data[i];
					temp++;
				} 
			}
			//cout << "[READ " << N << " BIT] " << reg_Name << " " << Bit_Name << " " << Data << endl << endl;
			return Data;
		}

public:
		template <int N>
		sc_uint<N> read_bit_uint(string Bit_Name)
		{
			int index = 0;
			for(int i = 0; i < reg_BitRangeNum; i++)
			{
				if(Bit_Name == bit_Name[i]) 
				{
					index = i;
				}
			}

			if(bit_Option[index] == NONE)
			{

			}

			if(bit_Option[index] == WRITE_ONLY)
			{
				cout << "Can not READ from " << Bit_Name << " of " << reg_Name << " !!!!!!" << endl << endl;
			}

			if((bit_Option[index] == READ_ONLY) | (bit_Option[index] == READ_WRITE))
			{
				int temp = 0;
				for(int i = bit_Start[index]; i < bit_End[index] + 1; i++)
				{
					Data[temp] = reg_Data[i];
					temp++;
				} 
			}
			//cout << "[READ " << N << " BIT] " << reg_Name << " " << Bit_Name << " " << Data << endl << endl;
			return Data;
		}
};