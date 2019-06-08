#include "systemc.h"
#include "stdio.h"
#include "conio.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

typedef sc_bv<16> DatWidMem;
typedef sc_bv<16> AddWidMem;

class Memory: public sc_module
{
private:
	typedef struct Word
	{
	DatWidMem memDat;
	AddWidMem memAdd;
	Word *next;
	};

	typedef Word *Mem;

	Mem m;

	bool flag;

	void Init_Mem();
	void Insert(AddWidMem add, DatWidMem dat);
	void Gen_Mem(int size);

public:
	Memory(sc_module_name name, int size);
	void Put(AddWidMem add, DatWidMem dat);
	DatWidMem Get(AddWidMem add);
	void Display();
};

