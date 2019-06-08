#include "Memory.h"

void Memory::Init_Mem(Mem &m)
{
	m = NULL;
}

void Memory::Insert(Mem &m, AddWidMem add, DatWidMem dat)
{
	Word *p;
	p = new Word;
	p->memAdd = add;
	p->memDat = dat;
	p->next = m;
	m = p;
}

void Memory::Gen_Mem(Mem &m, int size)
{
	Init_Mem(m);
	for(int i = (size-1); i >= 0; i--)
	{
		Insert(m, i, "00000000");
	}
}

void Memory::Put_Data(AddWidMem add, DatWidMem dat)
{
	bool flag = false;
	Word *p = m;
	while((p != NULL) && (flag == false))
	{
		if(add == p->memAdd)
		{
			p->memDat = dat;
			flag = true;
		}
		else p = p->next;
	}
	if(p == NULL) cout << "Address is wrong!";
}

DatWidMem Memory::Get_Data(AddWidMem add)
{
	bool flag = false;
	Word *p = m;
	DatWidMem temp;
	while((p != NULL) && (flag == false))
	{
		if(add == p->memAdd) 
		{
			temp = p->memDat;
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

void Memory::Display()
{
	Word *p = m;
	cout << "--------------------------------" << endl;
	cout << "Addess                  " << "Data" << endl;
	while(p != NULL)
	{
		cout << p->memAdd << "        " << p->memDat << endl;
		p = p->next;
	}
	cout << "--------------------------------" << endl;
}

Memory::Memory(sc_module_name name, int size)
{
	Gen_Mem(m, size);
}