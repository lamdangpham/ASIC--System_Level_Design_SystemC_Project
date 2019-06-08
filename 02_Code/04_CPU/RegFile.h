#include "systemc.h"
#include "stdio.h"
#include "conio.h"
#include "string"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

typedef sc_bv<3> AddWidReg;
typedef sc_bv<16> DatWidReg;

class RegFile: public sc_module
{
public:
	DatWidReg Buss;
	AddWidReg DR;
	//sc_in<AddWidReg> LD;
	AddWidReg SR1;
	AddWidReg SR2;
	DatWidReg SR1_out;
	DatWidReg SR2_out;

private:
	typedef struct Word
	{
		AddWidReg regAdd;
		DatWidReg regDat;
		Word *next;
	};

	typedef Word *Reg;

	Reg r;

	bool flag;

	void Init();
	void Insert(AddWidReg add, DatWidReg dat);
	void Gen_RegFile(int size);

public:
	RegFile(sc_module_name, int size);
	void Put(AddWidReg add, DatWidReg dat);
	DatWidReg Get(AddWidReg add);
	void Display();
};