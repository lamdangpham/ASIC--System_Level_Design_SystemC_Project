#include "driver.h"

driver::driver(sc_module_name name): sc_module(name)
{
	SC_CTHREAD(proc_driver, d_clk.pos()); 
}

void driver::proc_driver()
{
	for(int i = 0; i < 100; i++)
	{
		d_RXDi.write(i);
		wait(48);
		d_RXDi.write(0);
	}

	for(int i = 0; i < 100; i++)
	{
		d_cpu_trans.write(i);
		wait(48);
		d_cpu_trans.write(0);
	}
}