#include "Top.h"

Top::Top(sc_module_name name, sc_time period_): sc_module(name), period(period_), sci("SCI_01", period), cpu("CPU_01")
{
	cpu.cpu_trans(cpu_trans);
	cpu.cpu_recei(cpu_recei);
	cpu.i_socketCPU(sci.t_socketSCI);

	sci.i_socketSCI(cpu.t_socketCPU);
	sci.SCKi(clk);
	sci.RXDi(RXDi);
	sci.TXDi(TXDi);
}