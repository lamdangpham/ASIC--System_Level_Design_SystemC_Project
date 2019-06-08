// Filename: info_register.h
//----------------------------------------------------------------------
//  Copyright (c) 2016 by PhamGiaHuy-HCMUT
//  https://www.facebook.com/profile.php?id=100010839165665
//----------------------------------------------------------------------
// Getting Started with System-Level Design, General Register_v2.1

#ifndef INFO_H
#define INFO_H

#include "iostream"
#include "fstream"
#include "stdio.h"
#include "systemc.h"
#include "string"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

template <int N>
class Info_Reg: public sc_module
{
public:
	Info_Reg(sc_module_name name, char *file_name_): sc_module(name), file_name(file_name_)
	{
		char ref[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20', '21'
			, '22', '23', '24', '25', '26', '27', '28', '29', '30', '31', '32', '33', '34', '35', '36', '37', '38', '39', '40', '41', '42', '43'
			, '44', '45', '46', '47', '48', '49', '50', '51', '52', '53', '54', '55', '56', '57', '58', '59', '60', '61', '62', '63'};

		ifstream file_read(file_name);
		char s[8];

		file_read >> reg_Default;
		file_read >> bit_RangeNum;

		bit_Name = new string[bit_RangeNum];
		bit_Start = new int[bit_RangeNum];
		bit_End = new int[bit_RangeNum];
		bit_Option = new int[bit_RangeNum];
		bit_OptionTemp = new string[bit_RangeNum];

		for(int i = 0; i < bit_RangeNum; i++)
		{
			int j = 0;
			int temp = 0;
			file_read >> bit_Name[i];

			file_read >> s;
			if(s[2] == ':')
			{
				j = 0;
				while(s[1] != ref[j])
				{
					j++;
				}
				bit_Start[i] = j;

				if(s[4] == ']')
				{
					j = 0;
					while(s[3] != ref[j])
					{
						j++;
					}
					bit_End[i] = j;
				}
				else
				{
					j = 0;
					while(s[3] != ref[j])
					{
						j++;
					}
					temp = j;

					j = 0;
					while(s[4] != ref[j])
					{
						j++;
					}
					bit_End[i] = temp*10 + j;
				}
			}
			else
			{
				j = 0;
				while(s[1] != ref[j])
				{
					j++;
				}
				temp = j;

				j = 0;
				while(s[2] != ref[j])
				{
					j++;
				}
				bit_Start[i] = temp*10 + j;

				if(s[5] == ']')
				{
					j = 0;
					while(s[3] != ref[j])
					{
						j++;
					}
					bit_End[i] = j;
				}
				else
				{
					j = 0;
					while(s[4] != ref[j])
					{
						j++;
					}
					temp = j;

					j = 0;
					while(s[5] != ref[j])
					{
						j++;
					}
					bit_End[i] = temp*10 + j;
				}
			}

			file_read >> bit_OptionTemp[i];
			if(bit_OptionTemp[i] == "NONE")			
			{
				bit_Option[i] = 0;
			}
			if(bit_OptionTemp[i] == "READ_ONLY")			
			{
				bit_Option[i] = 1;
			}
			if(bit_OptionTemp[i] == "WRITE_ONLY")			
			{
				bit_Option[i] = 2;
			}
			if(bit_OptionTemp[i] == "READ_WRITE")			
			{
				bit_Option[i] = 3;
			}
		}
	}

public:
	int bit_RangeNum;
	string *bit_Name;
	int *bit_Start;
	int *bit_End;
	int *bit_Option;
	string *bit_OptionTemp;
	sc_bv<N> reg_Default;
	char *file_name;
};

#endif
