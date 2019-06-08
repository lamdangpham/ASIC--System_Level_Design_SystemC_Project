#include "Memory.h"

void Memory::Init_Mem()
{
	m = NULL;
}

void Memory::Insert(AddWidMem add, DatWidMem dat)
{
	Word *p;
	p = new Word;
	p->memAdd = add;
	p->memDat = dat;
	p->next = m;
	m = p;
}

void Memory::Gen_Mem(int size)
{
	Init_Mem();
	for(int i = (size-1); i >= 0; i--)
	{
		Insert(i, "0000000000000000");
	}
}

void Memory::Put(AddWidMem add, DatWidMem dat)
{
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
	flag = false;
	if(p == NULL) cout << "Address Memory is wrong!" << endl;
}

DatWidMem Memory::Get(AddWidMem add)
{
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
	flag = false;
	if(p == NULL)
	{
		cout << "Address Memory is wrong!" << endl;
		return 0;
	}
	else return temp;
}

void Memory::Display()
{
	int i = 0;
	Word *p = m;
	cout << "Mem " << ":=============================================================================================" << name() << endl;
	cout << "  Addess                  " << "Data" << endl;
	while(p != NULL)
	{
		cout << "  " << p->memAdd << "        " << p->memDat << "       ( " << i << "  )" << endl;
		i++;
		p = p->next;
	}
	cout << "==================================================================================================" << endl << endl;
}

Memory::Memory(sc_module_name name, int size)
{
	flag = false;
	Gen_Mem(size);
}