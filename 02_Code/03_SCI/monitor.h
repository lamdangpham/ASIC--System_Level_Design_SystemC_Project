#include "systemc.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

class monitor: public sc_module
{
public:
	sc_in_clk m_clk;
	sc_in<sc_bv<12>> m_TXDi;
	sc_in<sc_bv<12>> m_RXDi;
	sc_in<sc_bv<8>> m_cpu_recei;
	sc_in<sc_bv<8>> m_cpu_trans;

	void proc_monitor();

	SC_HAS_PROCESS(monitor);
	monitor(sc_module_name name);
};