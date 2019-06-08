//#include "SCI.h"
#include "conio.h"
//#include "CPU_test.h"
//#include "delay_recei.h"
#include "Top.h"
//#include "SCI_tb.h"
#include "driver.h"
#include "monitor.h"

int sc_main(int argc, char* argv[])
{
	//sc_time t(10, SC_PS);

	sc_clock t_clk("clk", 10, SC_PS, 0.5, 0, SC_PS, true);

	//sc_time t = 16*clk.period();

	sc_signal<sc_bv<12>> t_RXDi;
	sc_signal<sc_bv<12>> t_TXDi;
	sc_signal<sc_bv<8>> t_cpu_trans;
	sc_signal<sc_bv<8>> t_cpu_recei;

	Top top("Top_01", t_clk.period());
	top.clk(t_clk);
	top.RXDi(t_RXDi);
	top.TXDi(t_TXDi);
	top.cpu_trans(t_cpu_trans);
	top.cpu_recei(t_cpu_recei);

	driver d("driver");
	d.d_clk(t_clk);
	d.d_RXDi(t_RXDi);
	d.d_cpu_trans(t_cpu_trans);

	monitor m("monitor");
	m.m_clk(t_clk);
	m.m_RXDi(t_RXDi);
	m.m_TXDi(t_TXDi);
	m.m_cpu_trans(t_cpu_trans);
	m.m_cpu_recei(t_cpu_recei);

	sc_trace_file *wf= sc_create_vcd_trace_file("SCI");
	sc_trace(wf, t_clk, "clk");
	sc_trace(wf, t_RXDi, "RXDi");
	sc_trace(wf, t_TXDi, "TXDi");
	sc_trace(wf, t_cpu_recei, "cpu_recei");
	sc_trace(wf, t_cpu_trans, "cpu_trans");

	sc_start(100, SC_NS);

	sc_close_vcd_trace_file(wf);

	getch();
	return 0;
}