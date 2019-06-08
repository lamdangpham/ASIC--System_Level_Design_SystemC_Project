#include "System.h"

System::System(sc_module_name name, char *fileInstruction, sc_time _period): 
	sc_module(name), period(_period), clk("clock"), TXDi("TXDi"), RXDi("RXDi"), cpu("LC3", period), sci("SCI", period)
{
	cpu.clk(clk);
	sci.SCKi(clk);

	sci.TXDi(TXDi);
	sci.RXDi(RXDi);

	cpu.i_socketCPU_to_SCI(sci.t_socketSCI);
	sci.i_socketSCI_to_CPU(cpu.t_socketCPU);

	cpu.LoadFile(fileInstruction);
}