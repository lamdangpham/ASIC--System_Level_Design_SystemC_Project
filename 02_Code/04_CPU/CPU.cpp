#include "CPU.h"

CPU::CPU(sc_module_name name, sc_time _period): CPU_Function(name, period), period(_period), clk("clk")
{
	SC_THREAD(Fetch);
	sensitive << clk.pos();
	dont_initialize();
	SC_THREAD(Decode);
	SC_THREAD(EvaluateAddress);
	SC_THREAD(FetchOperands);
	SC_THREAD(Execute);
	SC_THREAD(Store);
}