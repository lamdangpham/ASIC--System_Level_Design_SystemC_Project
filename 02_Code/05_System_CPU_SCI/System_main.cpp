#include "System.h"

int sc_main(int argc,char* argv[])
{
	sc_clock t_clk("clk", 250, SC_PS);
	
	sc_signal<sc_bv<12>> t_TXDi;
	sc_signal<sc_bv<12>> t_RXDi;

	System system("System", "demo_02.hex", t_clk.period());
	system.clk(t_clk);
	system.TXDi(t_TXDi);
	system.RXDi(t_RXDi);

	/*System system_01("System_01", "demo_02.hex", t_clk.period());
	system_01.clk(t_clk);
	system_01.TXDi(t_TXDi);
	system_01.RXDi(t_RXDi);

	System system_02("System_02", "demo_03.hex", t_clk.period());
	system_02.clk(t_clk);
	system_02.TXDi(t_RXDi);
	system_02.RXDi(t_TXDi);*/

	sc_trace_file *wf = sc_create_vcd_trace_file("System");
	sc_trace(wf, t_clk, "clk");
	sc_trace(wf, t_TXDi, "TXDi");
	sc_trace(wf, t_RXDi, "RXDi");

	//t_RXDi.write("000001010011");

	sc_start();

	sc_close_vcd_trace_file(wf);

	getch();
	return 0;
}