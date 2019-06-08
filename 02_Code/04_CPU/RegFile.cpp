#include "RegFile.h"

void RegFile::Init()
{
	r = NULL;
}

void RegFile::Insert(AddWidReg add, DatWidReg dat)
{
	Word *p = r;
	p = new Word;
	p->regAdd = add;
	p->regDat = dat;
	p->next = r;
	r = p;
}

void RegFile::Gen_RegFile(int size)
{
	//char ref[] = {'0', '1', '2', '3', '4', '5', '6', '7'};
	char* ref = "01234567";
	Init();
	for(int i = (size - 1); i >= 0; i--)
	{
		Insert(i, "0000000000000000");
	}
}

void RegFile::Put(AddWidReg add, DatWidReg dat)
{
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
	flag = false;
	if(p == NULL) cout << "Address RegFile is wrong !" << endl;
}

DatWidReg RegFile::Get(AddWidReg add)
{
	DatWidReg temp;
	Word *p = r;
	while((p != NULL) && (flag == false))
	{
		if(p->regAdd == add)
		{
			temp = p->regDat;
			flag = true;
		}
		else p = p->next;
	}
	flag = false;
	if(p == NULL)
	{
		cout << "Address RegFile is wrong!" << endl;
		return 0;
	}
	else return temp;
}

void RegFile::Display()
{
	Word *p = r;
	cout << "--------------------------------" << endl;
	cout << "Addess          " << "Data" << endl;
	while(p != NULL)
	{
		cout << p->regAdd << "             " << p->regDat << endl;
		p = p->next;
	}
	cout << "--------------------------------" << endl;
}

RegFile::RegFile(sc_module_name name, int size)
{
	flag = false;
	Gen_RegFile(size);
}