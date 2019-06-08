#include "SCI.h"
#include "CPU_test.h"

class Top: public sc_module
{
public:
	sc_time period;
	sc_in_clk clk;
	sc_in<sc_bv<12>> RXDi;
	sc_out<sc_bv<12>> TXDi;
	sc_in<sc_bv<8>> cpu_trans;
	sc_out<sc_bv<8>> cpu_recei;

	SCI sci;
	CPU cpu;

	Top(sc_module_name name, sc_time period_);
};