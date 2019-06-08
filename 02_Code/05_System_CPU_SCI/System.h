#include "CPU.h"
#include "SCI.h"

class System: public sc_module
{
public:
	sc_in_clk clk;
	sc_in<sc_bv<12>> RXDi;
	sc_out<sc_bv<12>> TXDi;

	sc_time period;

	CPU cpu;
	SCI sci;

	SC_HAS_PROCESS(System);
	System(sc_module_name name, char *fileInstruction, sc_time _period);
};