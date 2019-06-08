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
	sc_in<DatWidReg> Buss;
	sc_in<AddWidReg> DR;
	//sc_in<AddWidReg> LD;
	sc_in<AddWidReg> SR1;
	sc_in<AddWidReg> SR2;
	sc_out<DatWidReg> SR1_out;
	sc_out<DatWidReg> SR2_out;

private:
	typedef struct Word
	{
		AddWidReg regAdd;
		DatWidReg regDat;
		Word *next;
	};

	typedef Word *Reg;

	Reg r;

	void Init(Reg &r);
	void Insert(Reg &r, AddWidReg add, DatWidReg dat);
	void Gen_RegFile(Reg &r, int size);

public:
	RegFile(sc_module_name, int size);
	void Put_Reg(Reg &r, AddWidReg add, DatWidReg dat);
	DatWidReg Get_Reg(Reg r, AddWidReg add, DatWidReg dat);
	void Display();
};