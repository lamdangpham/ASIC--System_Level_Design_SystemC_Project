#include "monitor.h"

monitor::monitor(sc_module_name name): sc_module(name)
{
	SC_THREAD(proc_monitor);
	sensitive << m_RXDi << m_cpu_trans << m_cpu_trans << m_TXDi;
}

void monitor::proc_monitor()
{
	while(true)
	{
		wait();
		wait(SC_ZERO_TIME);
		cout << "RXDi data is	   (@" << sc_time_stamp() << "): " << m_RXDi.read() << endl;
		cout << "cpu_recei data is  (@" << sc_time_stamp() << "): " << m_cpu_recei.read() << endl;
		cout << "cpu_trans data is  (@" << sc_time_stamp() << "): " << m_cpu_trans.read() << endl;
		cout << "TXDi data is       (@" << sc_time_stamp() << "): " << m_TXDi.read() << endl << endl;
	}
}

