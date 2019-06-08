#include "systemc.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

class driver: public sc_module
{
public:
	sc_in_clk d_clk;
	sc_out<sc_bv<12>> d_RXDi;
	sc_out<sc_bv<8>> d_cpu_trans;

	void proc_driver();

	SC_HAS_PROCESS(driver);
	driver(sc_module_name name);
};