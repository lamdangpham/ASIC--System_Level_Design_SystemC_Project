#include "SCI_register.h"

class SCI_Func: public SCI_Reg
{

public:
	SCI_Func(sc_module_name name);
	void sel_Clock(string);
	void sel_Mode(string);
	void sel_DatLen(string);
	void en_Parity(string);
	void sel_Parity(string);
	void sel_StopBitLen(string);
	void sel_CountSource(string);
	void sel_DatDir(string);
	void sel_ClkPolarity(string);
	void sel_ClkPhase(string);
	void SCI_setting(string, string, string, string, string, string, string); // for asynchronous
	void SCI_setting(string, string, string); // for synchronous
	void SCI_BitRate(int);
	void SCI_WBuffTrans(sc_bv<8>);
	void SCI_WBuffRecei(sc_bv<8>);
	void SCI_enTrans();
	void SCI_disTrans();
	void SCI_ProcDatAsynTrans(sc_bv<8>, sc_bv<12> &);
	int SCI_check_num1(sc_bv<8>);
	void SCI_ProcDatAsynRecei(sc_bv<12>, sc_bv<8> &);
};