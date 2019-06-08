#include "systemc.h"
#include "stdio.h"
#include "conio.h"

using namespace std;
using namespace sc_dt;

typedef sc_bv<8> DatWidMem;
typedef sc_bv<16> AddWidMem;

class Memory: public sc_module
{
public:
	sc_in<sc_bv<16>> Buss;
	sc_out<sc_bv<16>> mdrOut;

private:
	typedef struct Word
	{
	DatWidMem memDat;
	AddWidMem memAdd;
	Word *next;
	};

	typedef Word *Mem;

	Mem m;

	void Init_Mem(Mem &m);
	void Insert(Mem &m, AddWidMem add, DatWidMem dat);
	void Gen_Mem(Mem &m, int size);

public:
	Memory(sc_module_name name, int size);
	void Put_Data(AddWidMem add, DatWidMem dat);
	DatWidMem Get_Data(AddWidMem add);
	void Display();
};

