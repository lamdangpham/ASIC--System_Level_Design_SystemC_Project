#include "RegFile.h"

void RegFile::Init(Reg &r)
{
	r = NULL;
}

void RegFile::Insert(Reg &r, AddWidReg add, DatWidReg dat)
{
	Word *p = r;
	p = new Word;
	p->regAdd = add;
	p->regDat = dat;
	p->next = r;
	r = p;
}

void RegFile::Gen_RegFile(Reg &r, int size)
{
	//char ref[] = {'0', '1', '2', '3', '4', '5', '6', '7'};
	char* ref = "01234567";
	Init(r);
	for(int i = (size - 1); i >= 0; i--)
	{
		Insert(r, i, "0000000000000000");
	}
}

void RegFile::Put_Reg(Reg &r, AddWidReg add, DatWidReg dat)
{
	bool flag = false;
	Word *p = r;
	while((p != NULL) && (flag == false))
	{
		if(p->regAdd == add)
		{
			p->regDat = dat;
			flag = true;
		}
		else p = p->next;
	}
	if(p == NULL) cout << "Address is wrong !";
}

DatWidReg RegFile::Get_Reg(Reg r, AddWidReg add, DatWidReg dat)
{
	DatWidReg temp;
	bool flag;
	Word *p = r;
	while((p != NULL) && (flag = false))
	{
		if(p->regAdd == add)
		{
			temp = p->regDat;
			flag = true;
		}
		else p = p->next;
	}
	if(p == NULL)
	{
		cout << "Address is wrong!";
		return 0;
	}
	else return temp;
}

void RegFile::Display()
{
	Word *p = r;
	cout << "--------------------------------------------------" << endl;
	cout << "Addess          " << "Data" << endl;
	while(p != NULL)
	{
		cout << p->regAdd << "             " << p->regDat << endl;
		p = p->next;
	}
	cout << "--------------------------------------------------" << endl;
}

RegFile::RegFile(sc_module_name name, int size)
{
	Gen_RegFile(r, size);
}