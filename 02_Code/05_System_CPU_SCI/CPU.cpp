#include "CPU.h"

CPU::CPU(sc_module_name name, sc_time _period): CPU_Function(name, period), period(_period), clk("clk")
{
	SC_CTHREAD(Fetch, clk.pos());
	Fe = sc_get_current_process_handle();

	SC_CTHREAD(Decode, clk.pos());
	De = sc_get_current_process_handle();

	SC_CTHREAD(EvaluateAddress, clk.pos());
	Ev = sc_get_current_process_handle();

	SC_CTHREAD(FetchOperands, clk.pos());
	FeO = sc_get_current_process_handle();

	SC_CTHREAD(Execute, clk.pos());
	Ex = sc_get_current_process_handle();

	SC_CTHREAD(Store, clk.pos());
	St = sc_get_current_process_handle();
}