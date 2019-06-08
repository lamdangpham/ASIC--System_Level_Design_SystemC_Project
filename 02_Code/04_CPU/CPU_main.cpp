#include "CPU.h"

int sc_main(int argc,char* argv[])
{
	sc_clock clk("clk", 250, SC_PS);

	CPU cpu("LC3", clk.period());
	cpu.clk(clk);

	cpu.LoadFile("test.hex");

	sc_start();

	getch();
	return 0;
}